#ifndef _EXCOM_PROTOCOL_PACKETS_H
#define _EXCOM_PROTOCOL_PACKETS_H

#define PACKET(name, __, body) \
  typedef struct excom_protocol_##name body excom_protocol_##name##_t;

#define string(name) excom_string_t name;
#define int(type, name) type name;

#include "./packets.def"

#undef int
#undef string
#undef PACKET

#endif
