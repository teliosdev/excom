#ifndef _EXCOM_PROTOCOL_H
#define _EXCOM_PROTOCOL_H

/*!
 * @file "excom/protocol.h"
 * Nothing here yet.
 */

#include "excom/protocol/packets.h"

typedef struct excom_packet
{
  uint32_t size;
  uint16_t id;
  excom_protocol_packets_t type;
  excom_protocol_packet_t data;
  struct excom_packet* _next;
} excom_packet_t;

int excom_protocol_read_packet(excom_buffer_t* in,
  excom_packet_t* packet);
int excom_protocol_write_packet(struct excom_buffer* out,
  excom_packet_t* packet);
void excom_protocol_prefill(excom_packet_t* packet, ...);
int excom_protocol_write(excom_packet_t* packet, int sock);

int excom_protocol_pack_uint32_t(char* dest, uint32_t n);
int excom_protocol_unpack_uint32_t(char* src, uint32_t* out);
int excom_protocol_pack_uint16_t(char* dest, uint16_t n);
int excom_protocol_unpack_uint16_t(char* src, uint16_t* out);
int excom_protocol_pack_uint8_t(char* dest, uint8_t n);
int excom_protocol_unpack_uint8_t(char* src, uint8_t* out);

#endif
