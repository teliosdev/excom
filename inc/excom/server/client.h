#ifndef _EXCOM_SERVER_CLIENT_H
#define _EXCOM_SERVER_CLIENT_H

typedef struct excom_server_client
{
  excom_event_t event;
  //excom_event_buffer_t outbuf;
  struct excom_server* server;
  void* data;
  struct excom_server_client* _next;
} excom_server_client_t;

void excom_server_client_connect(excom_event_t event,
  excom_server_client_t* client);
void excom_server_client_read(excom_event_t event,
  excom_server_client_t* client);
void excom_server_client_write(excom_event_t event,
  excom_server_client_t* client);
void excom_server_client_error(excom_event_t event,
  excom_server_client_t* client);
void excom_server_client_close(excom_event_t event,
  excom_server_client_t* client);

#endif
