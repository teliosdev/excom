#include "excom.h"
#include <sodium.h>

const char* excom_encryption_version() {
  return sodium_version_string();
}

void excom_encryption_load()
{
  sodium_init();
  randombytes_stir();
}

void excom_encrypt_init(excom_encrypt_t* enc, bool alloc)
{
  sodium_memzero(enc, sizeof(excom_encrypt_t));

  if(alloc) {
    sodium_mlock(enc->local.skey, crypto_box_SECRETKEYBYTES);
    sodium_mlock(enc->nm.kkey, crypto_box_BEFORENMBYTES);
  }
}

void excom_encrypt_prep(excom_encrypt_t* enc)
{
  if(!enc->nm.active)
  {
    enc->nm.active = true;
    crypto_box_beforenm(enc->nm.kkey, enc->remote.pkey, enc->local.skey);
  }
}

// This encrypts from buf->pos and on, and outputs it to the given
// output buffer.  This uses the public key encryption method.
int excom_pencrypt_buffer(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out)
{
  uint8_t nonce[crypto_box_NONCEBYTES];
  uint8_t* crypt,* message;
  int err;
  uintptr_t remaining;
  size_t m_len, c_len;

  if(out->unmutable) { return EACCES; }
  excom_mutex_lock(&buf->mutex);
  remaining = excom_buffer_remaining(buf);
  m_len = remaining + crypto_box_ZEROBYTES;
  c_len = m_len + crypto_box_MACBYTES;

  randombytes_buf(nonce, crypto_box_NONCEBYTES);

  message = excom_malloc(m_len);
  crypt = excom_malloc(c_len);

  if(crypt == NULL || message == NULL)
  {
    if(message != NULL) { excom_free(message); }
    if(crypt != NULL) { excom_free(crypt); }

    excom_mutex_unlock(&buf->mutex);
    return ENOMEM;
  }

  memset(message, 0, crypto_box_ZEROBYTES);
  sodium_mlock(message, m_len);
  memcpy(message + crypto_box_ZEROBYTES, buf->pos,
    remaining);
  excom_mutex_unlock(&buf->mutex);
  excom_encrypt_prep(enc);

  err = crypto_box_afternm(crypt, message, m_len, nonce, enc->nm.kkey);

  sodium_munlock(message, m_len);
  excom_free(message);
  if(err != 0)
  {
    excom_free(crypt);
    return err;
  }

  excom_buffer_cappend(out, (char*) nonce,
    crypto_box_NONCEBYTES);
  excom_buffer_cappend(out, (char*) crypt + crypto_box_BOXZEROBYTES,
    c_len);
  excom_free(crypt);

  return 0;
}

int excom_pdecrypt_buffer(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out)
{
  uint8_t nonce[crypto_box_NONCEBYTES];
  uint8_t* message,* crypt;
  size_t m_len, c_len;
  int err;

  if(out->unmutable) { return EACCES; }
  excom_mutex_lock(&buf->mutex);
  memcpy(nonce, buf->buf, crypto_box_NONCEBYTES);
  c_len = excom_buffer_remaining(buf) - crypto_box_NONCEBYTES;
  m_len = c_len - crypto_box_MACBYTES;

  crypt = excom_malloc(c_len + crypto_box_ZEROBYTES);
  message = excom_malloc(m_len);

  if(crypt == NULL || message == NULL) {
    if(message != NULL) { excom_free(message); }
    if(crypt != NULL) { excom_free(crypt); }

    excom_mutex_unlock(&buf->mutex);
    return ENOMEM;
  }

  sodium_mlock(message, m_len);
  sodium_memzero(crypt, crypto_box_ZEROBYTES);
  memcpy(crypt + crypto_box_ZEROBYTES,
         buf->pos + crypto_box_NONCEBYTES, c_len);
  excom_mutex_unlock(&buf->mutex);
  excom_encrypt_prep(enc);

  err = crypto_box_open_afternm(message, crypt,
          c_len + crypto_box_ZEROBYTES, nonce, enc->nm.kkey);

  excom_free(crypt);
  if(err != 0)
  {
    sodium_munlock(message, m_len);
    excom_free(message);
    return err;
  }

  excom_buffer_cappend(out, (char*) message + crypto_box_ZEROBYTES, m_len);
  sodium_munlock(message, m_len);
  excom_free(message);
  return 0;
}
