#include "excom.h"
#include <string.h>
#include <errno.h>

// TODO: excom_string_fill copies memory over,
// excom_string_fill_no_copy doesn't

void excom_string_init(excom_string_t* string)
{
  string->size = 0;
  string->body = NULL;
  string->freeable = false;
}

void excom_string_fill(excom_string_t* string, uint32_t size,
  char* body)
{
  string->size = size;
  string->body = body;
}

int excom_string_dup(excom_string_t* string,
  excom_string_t* out)
{
  char* buf;
  int err = 0;

  errno = 0;
  excom_string_init(out);

  if(string->size == 0)
  {
    return 0;
  }


  buf = excom_malloc(string->size + 1);

  if(buf == NULL)
  {
    out->size = 0;
    out->body = NULL;

    err = errno;
    errno = 0;
  }
  else
  {
    buf = memcpy(buf, string->body, string->size);
    buf[string->size] = 0;

    out->freeable = true;
    out->size = string->size;
    out->body = buf;
  }

  return err;
}

int excom_string_dupfill(excom_string_t* string, uint32_t size,
  char* body)
{
  excom_string_t buf;
  buf.size = size;
  buf.body = body;

  return excom_string_dup(&buf, string);
}

void excom_string_destroy(excom_string_t* string)
{
  if(string->freeable)
  {
    free(string->body);
  }
}

void excom_buffer_init(excom_buffer_t* buffer, size_t start)
{
  buffer->max = start;
  buffer->used = 0;

  buffer->buf = excom_malloc(start * sizeof(char));
}

void excom_buffer_resize(excom_buffer_t* buffer)
{
  size_t new_size;
  uint8_t* new_buf;

  new_size = buffer->max * 2;
  new_buf = excom_malloc(new_size);

  memcpy(new_buf, buffer->buf, buffer->used);

  free(buffer->buf);
  buffer->buf = new_buf;
  buffer->max = new_size;
}

void excom_buffer_cappend(excom_buffer_t* buffer, char* str,
  size_t size)
{
  if(buffer->used + size > buffer->max)
  {
    excom_buffer_resize(buffer);
  }

  memcpy(buffer->buf + buffer->used, str, size);
  buffer->used += size;
}

void excom_buffer_sappend(excom_buffer_t* buffer, excom_string_t* str)
{
  if(buffer->used + str->size > buffer->max)
  {
    excom_buffer_resize(buffer);
  }

  memcpy(buffer->buf + buffer->used, str->body, str->size);
  buffer->used += str->size;
}

void excom_buffer_bappend(excom_buffer_t* buffer, excom_buffer_t* src)
{
  if(buffer->used + src->used > buffer->max)
  {
    excom_buffer_resize(buffer);
  }

  memcpy(buffer->buf + buffer->used, src->buf, src->used);
  buffer->used += src->used;
}

void excom_buffer_inspect(excom_buffer_t* buffer)
{
  size_t i;

  printf("\nbuffer(%p): ", buffer->buf);

  for(i = 0; i < buffer->used; i++)
  {
    printf("%02x ", buffer->buf[i]);
  }

  printf("\n");
}

void excom_buffer_destroy(excom_buffer_t* buffer)
{
  free(buffer->buf);
  buffer->max  = 0;
  buffer->used = 0;
  buffer->buf  = NULL;

}
