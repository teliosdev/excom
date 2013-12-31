#include "excom.h"

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

/*!
 * For now, we'll assume the number of threads we'll need is 5.  This
 * may be a terrible assumption, but hey.
 */
#define EXCOM_THREADS 5

void excom_server_init(excom_server_t* server)
{
  server->sock = 0;
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
int excom_server_bind(excom_server_t* server)
{
  bool val = true;
  int err, flags;
  socklen_t len = sizeof(bool);
  struct sockaddr_in name;
  unsigned long param;

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

#ifdef EXCOM_POSIX
  (void) param;
  flags = fcntl(server->sock, F_GETFL);
  ERROR_CHECK(flags);

  flags |= O_NONBLOCK;

  err = fcntl(server->sock, F_SETFL, flags);
  ERROR_CHECK(err);

  evthread_use_pthreads();
#else
  param = 1;

  ioctlsocket(server->sock, FIONBIO, &param);
  evthread_use_windows_threads();
#endif

  return 0;
}

int excom_server_run(excom_server_t* server)
{
  struct event_base* base;
  int err;
  (void) server;

  base = event_base_new();

  if(base == NULL)
  {
    ERROR_CHECK(-1);
  }

  return 0;
}
