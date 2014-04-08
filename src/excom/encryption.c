#include "excom.h"
#include <sodium.h>

void excom_encryption_load()
{
  sodium_init();
  randombytes_stir();
}

typedef struct excom_encrypt 
{
  uint8_t pkey[crypto_box_PUBLICKEYBYTES];
  uint8_t skey[crypto_box_SECRETKEYBYTES];
} excom_encrypt_t;

// This encrypts from buf->pos and on, and outputs it to the given
// output buffer.  This uses the public key encryption method.
int excom_pencrypt_buffer(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out)
{
  uint8_t nonce[crypto_box_NONCEBYTES];
  uint8_t* crypt,* message;
  int err;
  size_t m_len;
  size_t c_len;
  
  if(out->unmutable) { return EACCES; }
  excom_mutex_lock(&buf->mutex);
  m_len = excom_buffer_remaining(buf) + crypto_box_ZEROBYTES;
  c_len = excom_buffer_remaining(buf) + crypto_box_ZEROBYTES + crypto_box_MACBYTES;
  
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
  memcpy(message + crypto_box_ZEROBYTES, buf->pos, 
    excom_buffer_remaining(buf));
  excom_mutex_unlock(&buf->mutex);
  err = crypto_box(crypt, message, m_len, nonce, enc->pkey, enc->skey);
  sodium_memzero(message, m_len);
  free(message);
  if(err != 0)
  {
    free(crypt);
    return err;
  }
  
  excom_buffer_cappend(out, (char*) nonce, 
    crypto_box_NONCEBYTES);
  excom_buffer_cappend(out, (char*) crypt + crypto_box_BOXZEROBYTES, 
    excom_buffer_remaining(buf) + crypto_box_MACBYTES);
  excom_free(crypt);
    
  return 0;
}