#include "excom.h"

typedef struct client_data
{
  excom_buffer_t outbuf;
  excom_buffer_t inbuf;
  excom_packet_t* packets;
  excom_mutex_t mutex;
  excom_cond_t cond;
  excom_thread_t thread;
  bool disconnected;
} client_data_t;

static void check_packet(excom_server_client_t* client);
static void* worker(void* cl);

#define client_data ((client_data_t*) client->data)

void excom_server_client_connect(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;

  printf("[excom] server: Client connected\n");

  client->data = excom_malloc(sizeof(client_data_t));

  excom_buffer_init(&client_data->outbuf, 16);
  excom_buffer_init(&client_data->inbuf, 16);
  excom_mutex_init(&client_data->mutex);
  excom_cond_init(&client_data->cond);
  client_data->disconnected = false;

  excom_thread_init(&client_data->thread, worker, client);

  excom_packet_t version;
  version.type = packet(protocol_version);
  excom_protocol_prefill(&version, EXCOM_VERSION, EXCOM_VERSION_MAJOR,
    EXCOM_VERSION_MINOR, EXCOM_VERSION_PATCH);
  version.id = 1;

  printf("Writing packet to socket...\n");

  excom_protocol_write_packet(&client_data->outbuf, &version);
  excom_buffer_write(&client_data->outbuf, client->event.fd);

  printf("done!\n");
}

void excom_server_client_read(excom_event_t event,
  excom_server_client_t* client)
{
  char buf[32];
  ssize_t out;
  int err;

  (void) event;
  (void) client;

  errno = 0;

  while(err == 0 && !client_data->disconnected)
  {
    out = read(event.fd, buf, 32);

    printf("recieved %zd bytes from client\n", out);

    if(out > 0)
    {
      excom_buffer_cappend(&client_data->inbuf, buf, out);
    }
    else if(out < 0)
    {
      err = errno;
    }
    else
    {
      err = -1;
    }
  }

  if(errno != 0)
  {
    check_packet(client);
  }
}

void excom_server_client_write(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;

  excom_buffer_write(&client_data->outbuf, client->event.fd);
}

void excom_server_client_error(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;
}

void excom_server_client_close(excom_event_t event,
  excom_server_client_t* client)
{
  excom_packet_t* packet, * next;
  (void) event;
  (void) client;

  excom_buffer_destroy(&client_data->inbuf);
  excom_buffer_destroy(&client_data->outbuf);

  excom_mutex_lock(&client_data->mutex);

  packet = client_data->packets;

  while(packet)
  {
    next = packet->_next;
    excom_free(packet);
    packet = next;
  }

  client_data->disconnected = true;

  excom_cond_broadcast(&client_data->cond);
  excom_mutex_unlock(&client_data->mutex);

  excom_thread_join(&client_data->thread, NULL);

  free(client_data);

  printf("[excom] server: Client disconnected\n");
}

static void check_packet(excom_server_client_t* client)
{
  excom_packet_t* packet;
  uint32_t size;
  int err;

  excom_mutex_lock(&client_data->inbuf.mutex);
  printf("checking packet...\n");

  // We need to at least have 4 bytes to read.  The four bytes should
  // represent the size of what is to come.
  if(excom_buffer_remaining(&client_data->inbuf) < 4)
  {
    printf("not enough to read size\n");
    excom_mutex_unlock(&client_data->inbuf.mutex);
    return;
  }

  excom_protocol_unpack_uint32_t((char*) client_data->inbuf.pos, &size);

  // The entire packet hasn't come through yet, so we need to wait
  // until it does.
  if(excom_buffer_remaining(&client_data->inbuf) < 4 + size)
  {
    printf("not enough to read entire packet\n");
    excom_mutex_unlock(&client_data->inbuf.mutex);
    return;
  }

  printf("read packet\n");
  packet = excom_malloc(sizeof(excom_packet_t));
  err = excom_protocol_read_packet(&client_data->inbuf, packet);
  excom_mutex_unlock(&client_data->inbuf.mutex);

  printf("checking err\n");
  if(err)
  {
    excom_check_error(err, 0, "read_packet");
    excom_free(packet);
    return;
  }

  printf("adding packet to list\n");
  packet->_next = client_data->packets;
  client_data->packets = packet;

  excom_cond_signal(&client_data->cond);
}

static void* worker(void* cl)
{
  excom_server_client_t* client = cl;
  excom_packet_t* packet;

  while(1)
  {
    excom_mutex_lock(&client_data->mutex);
    excom_cond_wait(&client_data->cond, &client_data->mutex);

    packet = client_data->packets;

    if(packet != NULL)
    {
      client_data->packets = packet->_next;
    }

    excom_mutex_unlock(&client_data->mutex);

    if(client_data->disconnected)
    {
      break;
    }

    if(packet == NULL)
    {
      continue;
    }

#   define PACKET(name, __, ___) \
      case packet(name): \
        printf("[excom-server] Recieved %s packet!\n", #name);

    switch(packet->type)
    {
#     include "excom/protocol/packets.def"
    }

#   undef PACKET

    excom_free(packet);
  }

  excom_thread_exit(NULL);
  return NULL;
}
