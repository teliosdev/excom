#include "excom.h"

const char* excom_encryption_version() {
  return sodium_version_string();
}

void excom_encryption_load()
{
  sodium_init();
  randombytes_stir();
}

void excom_encrypt_init(excom_encrypt_t* enc)
{
  sodium_memzero(enc, sizeof(excom_encrypt_t));
}

void excom_encrypt_pair_fill(excom_encrypt_pair_t* enc)
{
  crypto_box_keypair(enc->pkey, enc->skey);
}

void excom_encrypt_prep(excom_encrypt_t* enc)
{
  if(!enc->nm.active)
  {
    enc->nm.active = true;
    crypto_box_beforenm(enc->nm.kkey, enc->remote.pkey, enc->local->skey);
  }
}

// This encrypts from buf->pos and on, and outputs it to the given
// output buffer.  This uses the public key encryption method.
int excom_pencrypt_buffer(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out)
{
  return excom_pencrypt_buffer2(enc, buf, out,
                                excom_buffer_remaining(buf));
}

// TODO: Optimise
//   the buffers dont have to be allocated, we can use the
//   passed buffers to do exactly what we need.
int excom_pencrypt_buffer2(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out, size_t use)
{
  uint8_t nonce[crypto_box_NONCEBYTES];
  uint8_t* crypt,* message;
  int err;
  size_t m_len, c_len;

  if(use == 0)
  {
    return 0;
  }

  if(out->unmutable) { return EACCES; }
  excom_mutex_lock(&out->mutex);
  m_len = use + crypto_box_ZEROBYTES;
  c_len = m_len;

  randombytes_buf(nonce, crypto_box_NONCEBYTES);

  message = excom_malloc(m_len);
  crypt = excom_malloc(c_len);

  if(crypt == NULL || message == NULL)
  {
    if(message != NULL) { excom_free(message); }
    if(crypt != NULL) { excom_free(crypt); }

    excom_mutex_unlock(&out->mutex);
    return ENOMEM;
  }

  memset(message, 0, crypto_box_ZEROBYTES);
  memcpy(message + crypto_box_ZEROBYTES, buf->pos,
    use);
  excom_mutex_unlock(&out->mutex);
  excom_encrypt_prep(enc);

  err = crypto_box_afternm(crypt, message, m_len, nonce, enc->nm.kkey);

  excom_free(message);
  if(err != 0)
  {
    excom_free(crypt);
    return err;
  }

  excom_buffer_cappend(out, (char*) nonce, crypto_box_NONCEBYTES);
  excom_buffer_cappend(out, (char*) crypt, c_len);
  excom_free(crypt);

  return 0;
}

int excom_pdecrypt_buffer(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out)
{
  return excom_pdecrypt_buffer2(enc, buf, out,
                                excom_buffer_remaining(buf));
}

// TODO: Optimise
//   the buffers dont have to be allocated, we can use the
//   passed buffers to do exactly what we need.
int excom_pdecrypt_buffer2(excom_encrypt_t* enc,
  excom_buffer_t* buf, excom_buffer_t* out, size_t use)
{
  uint8_t nonce[crypto_box_NONCEBYTES];
  uint8_t* message,* crypt;
  size_t m_len, c_len;
  int err;

  if(use == 0)
  {
    return 0;
  }

  if(out->unmutable) { return EACCES; }
  excom_mutex_lock(&out->mutex);
  memcpy(nonce, buf->pos, crypto_box_NONCEBYTES);
  c_len = use - crypto_box_NONCEBYTES;
  m_len = c_len;
  buf->pos += crypto_box_NONCEBYTES;

  crypt   = excom_malloc(c_len);
  message = excom_malloc(m_len);

  if(crypt == NULL || message == NULL)
  {
    if(message != NULL) { excom_free(message); }
    if(crypt   != NULL) { excom_free(crypt);   }

    excom_mutex_unlock(&out->mutex);
    return ENOMEM;
  }

  memcpy(crypt, buf->pos, c_len);
  excom_mutex_unlock(&out->mutex);
  excom_encrypt_prep(enc);

  errno = 0;
  err = crypto_box_open_afternm(message, crypt,
          c_len, nonce, enc->nm.kkey);

  excom_free(crypt);
  if(err != 0)
  {
    excom_check_error(errno, 0, "crypto_box");
    excom_free(message);
    return err;
  }

  excom_buffer_cappend(out, (char*) message + crypto_box_ZEROBYTES,
                       m_len - crypto_box_ZEROBYTES);
  excom_free(message);
  return 0;
}
