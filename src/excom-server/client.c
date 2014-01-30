#include "excom.h"

void excom_server_client_connect(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;

  printf("[excom] server: Client connected\n");

  excom_packet_t version;
  version.type = packet(protocol_version);
  excom_protocol_prefill(&version, EXCOM_VERSION, EXCOM_VERSION_MAJOR,
    EXCOM_VERSION_MINOR, EXCOM_VERSION_PATCH);
  version.id = 1;

  excom_protocol_write(&version, client->event.fd);
}

void excom_server_client_read(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;
}

void excom_server_client_write(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;
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
  (void) event;
  (void) client;

  printf("[excom] server: Client disconnected\n");
}
