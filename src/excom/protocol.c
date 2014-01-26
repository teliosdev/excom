#include "excom.h"

#ifndef EXCOM_WIN
#include <arpa/inet.h>
#endif

static int uint32_t_pack(char* dest, uint32_t n)
{
  uint32_t net = htonl(n);
  int part;
  char* ptr = dest;
  for(part = 0; part < 25; part += 8)
  {
    *ptr++ = net >> part;
  }
  *ptr++ = 0;
  return 4;
}

static int uint16_t_pack(char* dest, uint16_t n)
{
  uint16_t net = htons(n);
  int part;
  char* ptr = dest;
  for(part = 0; part < 11; part += 8)
  {
    *ptr++ = net >> part;
  }
  *ptr++ = 0;
  return 2;
}

static int uint8_t_pack(char* dest, uint8_t n)
{
  dest[0] = n;

  return 1;
}

int excom_protocol_write_packet(excom_buffer_t* out,
  excom_packet_t* packet)
{
  char buf[4];

# define PACKET(name, id, body) case packet(name): {    \
    excom_protocol_##name##_t pack = packet->data.name; \
    (void) pack;                                        \
    body                                                \
    break;                                              \
  }
# define string(name) do                      \
    {                                         \
      uint32_t_pack(buf, pack.name.size);     \
      excom_buffer_cappend(out, buf, 4);      \
      excom_buffer_sappend(out, &pack.name);  \
    } while(0);

#define number(type, name) do                 \
    {                                         \
      uint32_t s;                             \
      s = type##_pack(buf, pack.name);        \
      excom_buffer_cappend(out, buf, s);      \
    } while(0);

  switch(packet->type)
  {
#include "excom/protocol/packets.def"
  }

  return 0;
}

