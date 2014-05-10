#ifndef _EXCOM_BUFFER_H
#define _EXCOM_BUFFER_H

typedef struct excom_buffer
{
  size_t _init;
  size_t max;
  size_t used;
  uint8_t* pos;

  uint8_t* buf;
  bool unmutable;
  bool freeable;
  excom_mutex_t mutex;
} excom_buffer_t;

int excom_buffer_init(excom_buffer_t* buffer, size_t start);

int excom_buffer_reset(excom_buffer_t* buffer);

int excom_buffer_resize(excom_buffer_t* buffer, size_t to_fit);

int excom_buffer_cappend(excom_buffer_t* buffer, const char* str,
  size_t size);

int excom_buffer_sappend(excom_buffer_t* buffer,
  excom_string_t* str);

int excom_buffer_bappend(excom_buffer_t* buffer,
  excom_buffer_t* src);

int excom_buffer_format(excom_buffer_t* out,
  const char* format, ...);

void excom_buffer_destroy(excom_buffer_t* buffer);

int excom_buffer_write(excom_buffer_t* buffer, int sock);

static inline uintptr_t excom_buffer_remaining(excom_buffer_t* buffer) {
  return buffer->used - (buffer->pos - (buffer)->buf);
}


#endif
