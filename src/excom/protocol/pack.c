#include "excom.h"

#ifndef EXCOM_WIN
#include <arpa/inet.h>
#endif

static int endianness()
{
  union { uint8_t c[4]; uint32_t i; } data;
  data.i = 0x12345678;
  return (data.c[0] == 0x78);
}

int excom_protocol_pack_uint32_t(char* dest, uint32_t n)
{
  uint32_t net;

  if(endianness())
  {
    net = htonl(n);
  }
  else
  {
    net = n;
  }

  int part;
  char* ptr = dest;
  for(part = 0; part < 25; part += 8)
  {
    *ptr++ = net >> part;
  }
  *ptr++ = 0;
  return 4;
}

int excom_protocol_unpack_uint32_t(char* src, uint32_t* out)
{
  union { uint8_t d[4]; uint32_t r; } v;

  v.d[0] = src[0];
  v.d[1] = src[1];
  v.d[2] = src[2];
  v.d[3] = src[3];

  if(endianness())
  {
    *out = ntohl(v.r);
  }
  else
  {
    *out = v.r;
  }

  return 4;
}

int excom_protocol_pack_uint16_t(char* dest, uint16_t n)
{
  uint16_t net;

  if(endianness())
  {
    net = htons(n);
  }
  else
  {
    net = n;
  }

  int part;
  char* ptr = dest;
  for(part = 0; part < 11; part += 8)
  {
    *ptr++ = net >> part;
  }
  *ptr++ = 0;
  return 2;
}

int excom_protocol_unpack_uint16_t(char* src, uint16_t* out)
{
  union { uint8_t d[2]; uint16_t r; } v;

  v.d[0] = src[0];
  v.d[1] = src[1];

  if(endianness())
  {
    *out = ntohs(v.r);
  }
  else
  {
    *out = v.r;
  }

  return 2;
}

int excom_protocol_pack_uint8_t(char* dest, uint8_t n)
{
  dest[0] = n;

  return 1;
}

int excom_protocol_unpack_uint8_t(char* src, uint8_t* out)
{
  *out = src[0];

  return 1;
}
