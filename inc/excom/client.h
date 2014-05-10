#ifndef _EXCOM_CLIENT_H
#define _EXCOM_CLIENT_H


typedef struct excom_client {
  int sock;
  uint16_t port;
  char* addr;
  bool disconnected;
  excom_protocol_state_t state;

  excom_event_base_t base;
  excom_thread_t thread;
  excom_mutex_t mutex;
  excom_packet_t* packets;
  void (*handler)(excom_packet_t*, struct excom_client*);

  struct
  {
    excom_buffer_t in;
    excom_buffer_t out;
  } buf;

} excom_client_t;

typedef void (excom_client_packet_handler_t)(excom_packet_t*,
  struct excom_client*);

void excom_client_init(excom_client_t* client);
int excom_client_connect(excom_client_t* client);
void excom_client_destroy(excom_client_t* client);
void excom_client_handle_packets(excom_client_t* client,
  excom_client_packet_handler_t* handler);

#endif
