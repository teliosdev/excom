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
  excom_event_t new_event;

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

    new_event.fd    = err;
    new_event.flags = EXCOM_EVENT_READ | EXCOM_EVENT_WRITE |
      EXCOM_EVENT_ERROR;
    new_event.data  = NULL;
    err = socket_non_blocking(err);
    if(err < 0)
    {
      close(new_event.fd);
    }
    else
    {
      excom_event_add(&server->base, &new_event);
    }
  }
}

static void on_event(excom_event_t event, void* ptr)
{
  excom_server_t* server = (excom_server_t*) ptr;
  int err;

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
      excom_check_error(EIO, 0, "Error on accept socket");
      excom_event_loop_end(&server->base);
      return;
    }
  }
  else // it's a client socket?? :o!!
  {
    if(event.flags & EXCOM_EVENT_READ)
    {
      printf("Data available!\n");
    }
    else if(event.flags & EXCOM_EVENT_WRITE)
    {
      printf("Can write!\n");
    }
    else if(event.flags & EXCOM_EVENT_ERROR)
    {
      printf("Error!\n");
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
  listen.flags = EXCOM_EVENT_READ | EXCOM_EVENT_ERROR;
  listen.data  = server;

  excom_event_add(&server->base, &listen);

  excom_event_loop(&server->base, server);

  return 0;

}
