#ifndef _EXCOM_ENCRYPTION_H
#define _EXCOM_ENCRYPTION_H

#include <sodium.h>

typedef struct excom_encrypt_pair {
  uint8_t pkey[crypto_box_PUBLICKEYBYTES];
  uint8_t skey[crypto_box_SECRETKEYBYTES];
} excom_encrypt_pair_t;

typedef struct excom_encrypt
{
  struct {
    bool active;
    uint8_t pkey[crypto_box_PUBLICKEYBYTES];
  } remote;

  excom_encrypt_pair_t* local;

  struct {
    bool active;
    uint8_t kkey[crypto_box_BEFORENMBYTES];
  } nm;

  bool active;
  uint8_t key [crypto_secretbox_KEYBYTES];
} excom_encrypt_t;

void excom_encryption_load();
const char* excom_encryption_version();
void excom_encrypt_init(excom_encrypt_t* enc);
void excom_encrypt_prep(excom_encrypt_t* enc);
void excom_encrypt_pair_fill(excom_encrypt_pair_t* enc);

// This encrypts from buf->pos and on, and outputs it to the given
// output buffer.  This uses the public key encryption method.
int excom_pencrypt_buffer(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out);
int excom_pencrypt_buffer2(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out, size_t use);

int excom_pdecrypt_buffer(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out);
int excom_pdecrypt_buffer2(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out, size_t use);

#endif
