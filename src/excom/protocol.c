#include "excom.h"
#include <stdarg.h>

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
  excom_buffer_t _buf;
  excom_buffer_init(&_buf, 32);
  uint32_t size = 0;
  uint16_t type = (uint16_t) packet->type;

# define PACKET(name, id, body) case packet(name): {    \
    excom_protocol_##name##_t pack = packet->data.name; \
    (void) pack;                                        \
    body                                                \
    break;                                              \
  }
# define string(name) do                      \
    {                                         \
      uint32_t_pack(buf, pack.name.size);     \
      excom_buffer_cappend(&_buf, buf, 4);    \
      excom_buffer_sappend(&_buf, &pack.name);\
      size += 4 + pack.name.size;             \
    } while(0);

# define number(type, name) do                \
    {                                         \
      uint32_t s;                             \
      s = type##_pack(buf, pack.name);        \
      excom_buffer_cappend(&_buf, buf, s);    \
      size += s;                              \
    } while(0);

  switch(packet->type)
  {
    case packet(INVALID):
      excom_buffer_destroy(&_buf);
      return 1;
      break;
#   include "excom/protocol/packets.def"
  }

# undef PACKET
# undef string
# undef number

  packet->size = size;

  uint32_t_pack(buf, size);
  excom_buffer_cappend(out, buf, 4);
  uint16_t_pack(buf, packet->id);
  excom_buffer_cappend(out, buf, 2);
  uint16_t_pack(buf, type);
  excom_buffer_cappend(out, buf, 2);
  excom_buffer_bappend(out, &_buf);

  excom_buffer_destroy(&_buf);

  return 0;
}

void excom_protocol_prefill(excom_packet_t* packet, ...)
{
  va_list args;
  va_start(args, packet);

# define PACKET(name, id, body) case packet(name): {    \
    excom_protocol_##name##_t* pack = &packet->data.name;\
    (void) pack;                                        \
    body                                                \
    break;                                              \
  }

# define string(name) do                                \
  {                                                     \
    char* str = va_arg(args, char*);                    \
    excom_string_fill(&pack->name, strlen(str), str);   \
  } while(0);

# define number(type, name) do                          \
  {                                                     \
    pack->name = va_arg(args, uintmax_t);               \
  } while(0);

  switch(packet->type)
  {
    case packet(INVALID):
      return;
#   include "excom/protocol/packets.def"
  }

# undef PACKET
# undef string
# undef number
}

int excom_protocol_write(excom_packet_t* packet, int sock)
{
  int err = 0;

  excom_buffer_t buffer;
  err = excom_buffer_init(&buffer, 32);
  if(err) { return err; }

  err = excom_protocol_write_packet(&buffer, packet);
  if(err) { return err; }

  err = excom_buffer_write(&buffer, sock);
  if(err) { return err; }

  excom_buffer_destroy(&buffer);

  return err;
}
