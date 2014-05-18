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

#define ERROR_CHECK(value) do \
{                             \
  if(value < 0)               \
  {                           \
    err = errno;              \
    errno = 0;                \
    return err;               \
  }                           \
} while(0)
#define error if(err) { return err; }

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

void excom_client_init(excom_client_t* client)
{
  client->sock    = -1;
  client->port    = EXCOM_DEFAULT_PORT;
  client->addr    = "127.0.0.1";
  client->packets = NULL;
  client->state   = EXCOM_PROTOCOL_STATE_PREHANDSHAKE;
  client->disconnected = false;

  excom_encrypt_init(&client->keys);
  excom_encrypt_pair_fill(&client->enc);
  excom_mutex_init(&client->mutex);

  client->keys.local = &client->enc;
}

int excom_client_connect(excom_client_t* client)
{
  struct hostent *he;
  int err;
  struct sockaddr_in name;
  excom_string_t str;
  excom_packet_t version;

  client->sock = socket(AF_INET, SOCK_STREAM, 0);
  ERROR_CHECK(client->sock);

  he = gethostbyname(client->addr);
  if(he == NULL)
  {
    return 1;
  }

  name.sin_family = AF_INET;
  name.sin_port   = htons(client->port);
  memcpy(&name.sin_addr, he->h_addr_list[0], he->h_length);

  if(connect(client->sock, (struct sockaddr*) &name,
    sizeof(name)) == -1)
  {
    excom_return_errno();
  }

  err = socket_non_blocking(client->sock);
  error;

  err = excom_buffer_init(&client->buf.in, 32);
  error;
  err = excom_buffer_init(&client->buf.out, 32);
  error;

  client->buf.in.freeable  = false;
  client->buf.out.freeable = false;

  excom_string_init(&str);
  excom_string_fill(&str, sizeof(EXCOM_VERSION), EXCOM_VERSION);
  version.type = packet(protocol_version);
  excom_protocol_prefill(&version, &str, EXCOM_VERSION_MAJOR,
    EXCOM_VERSION_MINOR, EXCOM_VERSION_PATCH);
  version.id = 1;

  excom_protocol_write_packet(&version, &client->buf.out, &client->keys);
  excom_buffer_write(&client->buf.out, client->sock);

  return err;
}

void excom_client_destroy(excom_client_t* client)
{
  close(client->sock);

  excom_thread_join(&client->thread, NULL);
  excom_buffer_destroy(&client->buf.out);
}
