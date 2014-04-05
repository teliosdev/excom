#include "excom.h"
#include <stdarg.h>

#ifndef EXCOM_WIN
#include <arpa/inet.h>
#endif

int excom_protocol_read_packet(excom_packet_t* packet,
                               excom_buffer_t* in)
{
  uint32_t size;
  uint16_t type, id;
  uint32_t cur = 0;

  cur += excom_protocol_unpack_uint32_t((char*) in->pos, &size);
  cur += excom_protocol_unpack_uint16_t((char*) in->pos + cur, &id);
  cur += excom_protocol_unpack_uint16_t((char*) in->pos + cur, &type);

  packet->size = size;
  packet->id   = id;
  packet->type = type;

# define PACKET(name, id, body) case packet(name): {      \
    excom_protocol_##name##_t* pack = &packet->data.name; \
    (void) pack;                                          \
    body                                                  \
    break;                                                \
  }

# define string(name) do                                  \
  {                                                       \
    uint32_t str_size = 0;                                \
    cur += excom_protocol_unpack_uint32_t((char*) in->pos \
      + cur, &str_size);                                  \
    excom_string_init(&pack->name);                       \
    excom_string_dupfill(&pack->name, str_size,           \
      (char*) in->pos + cur);                             \
    cur += str_size;                                      \
  } while(0);

# define number(type, name) do                            \
  {                                                       \
    type _buf;                                            \
    cur += excom_protocol_unpack_##type((char*) in->pos   \
      + cur,  &_buf);                                     \
    pack->name = _buf;                                    \
  } while(0);

  switch(type)
  {
    case packet(INVALID):
      printf("INVALID!\n");
      return 1;
#   include "excom/protocol/packets.def"
  }

# undef PACKET
# undef string
# undef number

  in->pos += cur;

  return 0;
}

int excom_protocol_write_packet(excom_packet_t* packet, excom_buffer_t* o)
{
  char buf[4];
  excom_buffer_t _buf;

  // this makes this function work on clang.  I'm not sure
  // why, to be honest.
  excom_buffer_t* out = o;
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
      excom_protocol_pack_uint32_t(buf,       \
        pack.name.size);                      \
      excom_buffer_cappend(&_buf, buf, 4);    \
      excom_buffer_sappend(&_buf, &pack.name);\
      size += 4 + pack.name.size;             \
    } while(0);

# define number(type, name) do                \
    {                                         \
      int s;                                  \
      s = excom_protocol_pack_##type(buf,     \
        pack.name);                           \
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

  excom_protocol_pack_uint32_t(buf, size);
  excom_buffer_cappend(out, buf, 4);
  excom_protocol_pack_uint16_t(buf, packet->id);
  excom_buffer_cappend(out, buf, 2);
  excom_protocol_pack_uint16_t(buf, type);
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

  err = excom_protocol_write_packet(packet, &buffer);
  if(err) { return err; }

  err = excom_buffer_write(&buffer, sock);
  if(err) { return err; }

  excom_buffer_destroy(&buffer);

  return err;
}
