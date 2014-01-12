#include "excom.h"

void excom_server_client_read(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;

  printf("DATA AVAILABLE\n");
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
}
