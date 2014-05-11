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

typedef enum excom_protocol_state {
  EXCOM_PROTOCOL_STATE_PREHANDSHAKE = 0u,
  EXCOM_PROTOCOL_STATE_PREENCRYPT,
  EXCOM_PROTOCOL_STATE_ENCRYPT,
  EXCOM_PROTOCOL_STATE_BODY,
  EXCOM_PROTOCOL_STATE_PRECLOSE,
  EXCOM_PROTOCOL_STATE_INVALID
} excom_protocol_state_t;

typedef enum excom_reject_reason
{
  EXCOM_REJECT_STANDARD_ERROR = 0x00u,
  EXCOM_REJECT_INVALID_PROTOCOL_ERROR
} excom_reject_reason_t;

typedef enum excom_error_reason
{
  EXCOM_ERROR_STANDARD_ERROR = 0x00u,
  EXCOM_ERROR_IMPROPER_PERMISSIONS_ERROR,
  EXCOM_ERROR_INVALID_ERROR,
  EXCOM_ERROR_NOT_AVAILABLE_ERROR,
  EXCOM_ERROR_PUBLIC_KEY_ERROR
} excom_error_reason_t;

int excom_protocol_read_packet(excom_packet_t* packet,
                               excom_buffer_t* in,
                               excom_encrypt_t* enc);
int excom_protocol_write_packet(excom_packet_t* packet,
                                excom_buffer_t* out,
                                excom_encrypt_t* enc);
void excom_protocol_prefill(excom_packet_t* packet, ...);
int excom_protocol_write(excom_packet_t* packet, int sock,
                         excom_encrypt_t* enc);

int excom_protocol_pack_uint32_t(char* dest, uint32_t n);
int excom_protocol_unpack_uint32_t(char* src, uint32_t* o);
int excom_protocol_pack_uint16_t(char* dest, uint16_t n);
int excom_protocol_unpack_uint16_t(char* src, uint16_t* o);
int excom_protocol_pack_uint8_t(char* dest, uint8_t n);
int excom_protocol_unpack_uint8_t(char* src, uint8_t* o);

#endif
