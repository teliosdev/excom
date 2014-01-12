#include "excom.h"

#include <stdio.h>
#include <errno.h>

#ifdef EXCOM_POSIX
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netdb.h>
#endif

#include <fcntl.h>

/*!
 * For now, we'll assume the number of threads we'll need is 5.  This
 * may be a terrible assumption, but hey.
 */
#define EXCOM_THREADS 5

void excom_server_init(excom_server_t* server)
{
  server->sock = -1;
  server->addr = EXCOM_DEFAULT_ADDR;
  server->port = EXCOM_DEFAULT_PORT;
}

#define ERROR_CHECK(value) do   \
{                             \
  if(value < 0)               \
  {                           \
    err = errno;              \
    errno = 0;                \
    return err;               \
  }                           \
} while(0)

static int socket_non_blocking(int fd)
{
  int flags, err;
  unsigned long param;
#ifdef EXCOM_POSIX
  (void) param;
  flags = fcntl(fd, F_GETFL);
  ERROR_CHECK(flags);

  flags |= O_NONBLOCK;

  err = fcntl(fd, F_SETFL, flags);
  ERROR_CHECK(err);

#else
  param = 1;
  (void) flags;

  ioctlsocket(fd, FIONBIO, &param);
#endif

  return 0;
}

int excom_server_bind(excom_server_t* server)
{
  bool val = true;
  int err;
  size_t len = sizeof(bool);
  struct sockaddr_in name;

  server->sock = socket(PF_INET, SOCK_STREAM, 0);
  ERROR_CHECK(server->sock);

  name.sin_family      = AF_INET;
  name.sin_port        = htons(server->port);
  name.sin_addr.s_addr = htonl(server->addr);

  err = bind(server->sock, (struct sockaddr*) &name, sizeof(name));
  ERROR_CHECK(err);

  err = listen(server->sock, 128);
  ERROR_CHECK(err);

  val = true;
  setsockopt(server->sock, SOL_SOCKET, SO_KEEPALIVE, &val, len);

  err = socket_non_blocking(server->sock);
  ERROR_CHECK(err);

  return 0;
}

static void _accept(excom_event_t event, excom_server_t* server)
{
  int err;
  excom_event_t* new_event = excom_malloc(sizeof(excom_event_t));
  excom_string_t str;

  while(1)
  {
    err = accept(event.fd, NULL, NULL);
    if(err == -1 && (errno != EAGAIN && errno != EWOULDBLOCK))
    {
      excom_check_error(errno, 0, "Failed to accept client");
      return;
    }
    else if(err == -1)
    {
      return;
    }

    new_event->fd    = err;
    new_event->flags = EXCOM_EVENT_READ | EXCOM_EVENT_WRITE |
      EXCOM_EVENT_ERROR | EXCOM_EVENT_CLOSE;
    new_event->data  = excom_malloc(sizeof(excom_event_buffer_t));
    excom_string_fill(&str, 11, "hello world");
    excom_string_unfreeable(&str);
    excom_event_buffer_init2(new_event->data, &str);
    err = socket_non_blocking(err);
    if(err < 0)
    {
      close(new_event->fd);
    }
    else
    {
      excom_event_add(&server->base, new_event);
    }
  }
}

static void on_event(excom_event_t event, void* ptr)
{
  excom_server_t* server = (excom_server_t*) ptr;
  int err;
  char buf[256];
  excom_string_t str;

  // this is our listen socket!
  if(event.fd == server->sock)
  {
    // we can accept a new client :3
    if(event.flags & EXCOM_EVENT_READ)
    {
      _accept(event, server);
    }
    else if(event.flags & EXCOM_EVENT_ERROR)
    {
      err = read(event.fd, NULL, 0);
      if((err == 0) && (errno == 0)) errno = EIO;
      excom_check_error(errno, 0, "Error on accept socket");
      excom_event_loop_end(&server->base);
      return;
    }
  }
  else // it's a client socket?? :o!!
  {
    if(event.flags & EXCOM_EVENT_READ)
    {
      err = read(event.fd, buf, 256);
      if(err < 0)
      {
        excom_check_error(errno, 0, "read");
        close(event.fd);
        return;
      }
      if(err > 3 && strncmp("stop", buf, 4) == 0)
      {
        excom_event_loop_end(&server->base);
      }
      else
      {
        excom_string_fill(&str, err, buf);
        excom_string_unfreeable(&str);
        excom_event_buffer_append(event.data, 1, &str);
      }
      printf("Data available!\n");
    }
    if(event.flags & EXCOM_EVENT_WRITE)
    {
      excom_event_buffer_write(event.data, event.fd);
    }
    if(event.flags & EXCOM_EVENT_ERROR)
    {
      printf("ERR!\n");
    }
    if(event.flags & EXCOM_EVENT_CLOSE)
    {
      close(event.fd);
      excom_event_buffer_destroy(event.data);
      excom_free(event.root);
    }
  }
}

int excom_server_run(excom_server_t* server)
{
  int err;
  (void) server;

  excom_event_t listen;

  err = excom_event_base_init(&server->base,
    on_event);
  if(err != 0)
  {
    excom_return_errno();
  }

  listen.fd    = server->sock;
  listen.flags = EXCOM_EVENT_READ | EXCOM_EVENT_ERROR |
    EXCOM_EVENT_CLOSE;
  listen.data  = server;

  excom_event_add(&server->base, &listen);

  excom_event_loop(&server->base, server);

  return 0;

}
