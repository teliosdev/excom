#ifndef _EXCOM_SERVER_H
#define _EXCOM_SERVER_H

#define EXCOM_DEFAULT_PORT 5656
#define EXCOM_DEFAULT_ADDR INADDR_ANY

typedef struct excom_server {
  int sock;
  uint16_t port;
  uint32_t addr;
  struct event_base* base;
} excom_server_t;

#endif
