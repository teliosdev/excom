#include "excom.h"


typedef struct client_data
{
  excom_protocol_state_t state;
  excom_encrypt_t keys;
  excom_packet_t* packets;
  excom_packet_t* last;
  excom_mutex_t mutex;
  excom_cond_t cond;
  excom_thread_t thread;
  bool disconnected;

  struct {
    excom_buffer_t  in;
    excom_buffer_t out;
    excom_buffer_t enc;
  } buf;
} client_data_t;

static void check_packet(excom_server_client_t* client);
static void* worker(void* cl);

#define client_data ((client_data_t*) client->data)
#include "./handle_packets.ci"

void excom_server_client_connect(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;
  excom_string_t str;

  printf("[excom] server: Client connected (%d)\n", client->event.fd);

  client->data = excom_malloc(sizeof(client_data_t));

  client_data->state = EXCOM_PROTOCOL_STATE_PREHANDSHAKE;
  excom_buffer_init(&client_data->buf.in , 16);
  excom_buffer_init(&client_data->buf.out, 16);
  excom_buffer_init(&client_data->buf.enc, 16);
  excom_mutex_init(&client_data->mutex);
  excom_cond_init(&client_data->cond);
  excom_encrypt_init(&client_data->keys);
  excom_string_init(&str);
  client_data->keys.local = &client->server->enc;
  client_data->disconnected = false;
  client_data->packets = NULL;
  client_data->last  = NULL;

  excom_thread_init(&client_data->thread, worker, client);
}

void excom_server_client_read(excom_event_t event,
  excom_server_client_t* client)
{
  char buf[32];
  ssize_t out;
  int err = 0;

  (void) event;
  (void) client;

  errno = 0;

  while(err == 0 && client_data && !client_data->disconnected)
  {
    out = read(event.fd, buf, 32);

    if(out > 0)
    {
      excom_buffer_cappend(&client_data->buf.in, buf, out);
    }
    else if(out < 0)
    {
      err = errno;
    }
    else if(out == 0)
    {
      excom_server_client_close(event, client);
      close(event.fd);
      return;
    }
    else
    {
      err = -1;
    }
  }

  if(err == EAGAIN || err == EWOULDBLOCK)
  {
    check_packet(client);
  }

}

void excom_server_client_write(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;
  int err;

  if(!client_data || client_data->disconnected)
  {
    return;
  }

  err = excom_buffer_write(&client_data->buf.out,
                           client->event.fd);

  if((err == EAGAIN || err == EWOULDBLOCK) &&
     !(event.flags ^ EXCOM_EVENT_WRITE))
  {
    event.root->flags |= EXCOM_EVENT_WRITE;
    excom_event_update(event.base, event.root);
  }
  else if(err == 0 &&
          (event.flags & EXCOM_EVENT_WRITE))
  {
    event.root->flags ^= EXCOM_EVENT_WRITE;
    excom_event_update(event.base, event.root);
  }
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
  excom_packet_t* packet, *next;
  int err;
  (void) event;
  (void) client;

  if(!client_data || client_data->disconnected)
    return;

  excom_buffer_destroy(&client_data->buf.in);
  excom_buffer_destroy(&client_data->buf.out);

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
  client->data = NULL;

  err = excom_event_remove(&client->server->base, event.root);
  excom_check_error(err, 0, "excom_event_remove");

  printf("[excom] server: Client disconnected\n");
}

static void check_packet(excom_server_client_t* client)
{
  excom_packet_t* packet;
  uint32_t size;
  int err;

  // We need to at least have 4 bytes to read.  The four bytes should
  // represent the size of what is to come.

  excom_mutex_lock(&client_data->mutex);
  while(excom_buffer_remaining(&client_data->buf.in) > 4)
  {
    excom_mutex_lock(&client_data->buf.in.mutex);

    excom_protocol_unpack_uint32_t((char*) client_data->buf.in.pos, &size);

    // The entire packet hasn't come through yet, so we need to wait
    // until it does.
    if(excom_buffer_remaining(&client_data->buf.in) < 4 + size)
    {
      break;
    }

    packet = excom_malloc(sizeof(excom_packet_t));
    err = excom_protocol_read_packet(packet, &client_data->buf.in,
                                     &client_data->keys);
    excom_mutex_unlock(&client_data->buf.in.mutex);

    if(err)
    {
      excom_check_error(err, 0, "read_packet");
      excom_free(packet);
      break;
    }

    if(client_data->packets == NULL)
    {
      client_data->packets = client_data->last = packet;
      packet->_next = NULL;
    }
    else
    {
      client_data->last->_next = packet;
      client_data->last = packet;
      packet->_next = NULL;
    }
  }


  excom_mutex_unlock(&client_data->buf.in.mutex);
  excom_mutex_unlock(&client_data->mutex);
  excom_cond_signal(&client_data->cond);
}

static void* worker(void* cl)
{
  excom_server_client_t* client = cl;
  excom_packet_t* packet;

  while(1)
  {
    excom_mutex_lock(&client_data->mutex);

    if(!client_data->packets)
    {
      excom_cond_wait(&client_data->cond, &client_data->mutex);
    }

    packet = client_data->packets;

    if(client_data->disconnected)
    {
      break;
    }

    excom_mutex_unlock(&client_data->mutex);

    while(packet)
    {
      client_data->packets = packet->_next;

      if(client_data->packets == NULL)
      {
        client_data->last = NULL;
      }

      #define PACKET(name, __, ___)                              \
        case packet(name):                                       \
          printf("[excom-server] Recieved %s packet!\n", #name); \
          handle_packet_##name(client_data, packet);             \
          printf("[excom-server] Done handling %s.\n", #name);   \
          break;

      printf("checking packet!\n");

      switch(packet->type)
      {
        #include "excom/protocol/packets.def"
        case packet(INVALID):
        default:
          printf("[excom-server] Recieved invalid packet type %d!\n",
            packet->type);
          break;
        #undef PACKET
      }

      client->event.flags |= EXCOM_EVENT_WRITE;
      excom_event_update(client->event.base, &client->event);

      packet = client_data->packets;
    }

  }

  excom_thread_exit(NULL);
  return NULL;
}
