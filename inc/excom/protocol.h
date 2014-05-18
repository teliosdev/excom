#ifndef _EXCOM_PROTOCOL_H
#define _EXCOM_PROTOCOL_H

/*!
 * @file "excom/protocol.h"
 * Manages protocol interactions.  This includes things like packets,
 * handshakes, errors/rejects, and state.
 *
 * @see https://github.com/medcat/excom/wiki/Protocol
 */

#include "excom/protocol/packets.h"

/*!
 */
typedef struct excom_packet
{
  uint32_t size;
  uint16_t id;
  excom_protocol_packets_t type;
  excom_protocol_packet_t data;
  struct excom_packet* _next;
} excom_packet_t;

/*!
 * The current state of the protocol.  Behavior changes based
 * on this value.
 */
typedef enum excom_protocol_state {
  /*!
   * This state is when the protocol has not yet performed the
   * handshake.  The next two states are during the handshake.
   */
  EXCOM_PROTOCOL_STATE_PREHANDSHAKE = 0u,
  /*!
   * This is the state right before the encryption happens;
   * in this state, the client is waiting for an OK packet
   * response from the server, and sends back its public
   * key.
   */
  EXCOM_PROTOCOL_STATE_PREENCRYPT,
  /*!
   * This is the state in which the encryption starts.  This occurs
   * after both ends recieve the other's public key, and the client
   * should ping the server at this point.
   */
  EXCOM_PROTOCOL_STATE_ENCRYPT,
  /*!
   * At this point, packets can be sent back and forth normally.
   */
  EXCOM_PROTOCOL_STATE_BODY,
  /*!
   * The state transitions to this when one end sends a CLOSE packet.
   * The other responds with an OK, and both ends close the socket.
   */
  EXCOM_PROTOCOL_STATE_PRECLOSE,
  /*!
   * The protocol transitions to this state after the close occurs,
   * or after a REJECT packet was recieved.
   */
  EXCOM_PROTOCOL_STATE_INVALID
} excom_protocol_state_t;

/*!
 * The reason the connection was rejected.
 */
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
