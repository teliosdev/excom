#include "excom.h"

struct excom_server_client_data {
  excom_event_buffer_t inbuf;
  int fd;
};

void excom_server_client_connect(excom_event_t event,
  excom_server_client_t* client)
{
  (void) event;
  (void) client;

  excom_protocol_packet_protocol_version_t ver;
  excom_string_init(&ver.version);
  excom_string_dupfill(&ver.version, sizeof(EXCOM_VERSION),
    EXCOM_VERSION);
  ver.major = EXCOM_VERSION_MAJOR;
  ver.minor = EXCOM_VERSION_MINOR;
  ver.patch = EXCOM_VERSION_PATCH;
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
}
