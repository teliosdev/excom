#ifndef _EXCOM_CLIENT_H
#define _EXCOM_CLIENT_H

typedef struct excom_client {
  int sock;
  uint16_t port;
  char* addr;
} excom_client_t;

#endif
