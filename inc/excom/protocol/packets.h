#ifndef _EXCOM_PROTOCOL_PACKETS_H
#define _EXCOM_PROTOCOL_PACKETS_H

#define PACKET(name, __, body) \
  typedef struct excom_protocol_##name body excom_protocol_##name##_t;

#define string(name) excom_string_t name;
#define number(type, name) type name;

#include "./packets.def"

#undef number
#undef string
#undef PACKET

#define PACKET(name, __, ___) \
  excom_protocol_##name##_t name;

typedef union excom_protocol_packet
{
#include "./packets.def"
} excom_protocol_packet_t;

#undef PACKET

#define PACKET(name, id, ___) \
  EXCOM_PROTOCOL_PACKET_##name = id##u,

typedef enum excom_protocol_packets
{
  EXCOM_PROTOCOL_PACKET_INVALID = 0ul,
# include "./packets.def"
} excom_protocol_packets_t;

#undef PACKET

#define packet(name) EXCOM_PROTOCOL_PACKET_##name

#endif
