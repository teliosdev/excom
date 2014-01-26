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
} excom_packet_t;

#endif
