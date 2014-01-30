#include "excom.h"
#include <ctype.h>

int excom_buffer_init(excom_buffer_t* buffer, size_t start)
{
  buffer->max = start;
  buffer->used = 0;
  buffer->unmutable = false;

  buffer->buf = excom_malloc(start * sizeof(char));

  if(buffer->buf == NULL)
  {
    excom_return_errno();
  }
  else
  {
    return 0;
  }
}

int excom_buffer_resize(excom_buffer_t* buffer, size_t to_fit)
{
  size_t new_size = buffer->max;
  uint8_t* new_buf;

  do {
    new_size *= 2;
  } while(new_size < to_fit);
  new_buf = excom_malloc(new_size);

  if(new_buf == NULL)
  {
    excom_return_errno();
  }

  memcpy(new_buf, buffer->buf, buffer->used);

  free(buffer->buf);
  buffer->buf = new_buf;
  buffer->max = new_size;

  return 0;
}

int excom_buffer_cappend(excom_buffer_t* buffer, const char* str,
  size_t size)
{
  int chk = 0;

  if(buffer->used + size > buffer->max)
  {
    chk = excom_buffer_resize(buffer, buffer->used + size);
  }

  if(chk != 0)
  {
    excom_return_errno();
  }

  memcpy(buffer->buf + buffer->used, str, size);
  buffer->used += size;

  return 0;
}

int excom_buffer_sappend(excom_buffer_t* buffer, excom_string_t* str)
{
  int chk = 0;

  if(buffer->used + str->size > buffer->max)
  {
    excom_buffer_resize(buffer, buffer->used + str->size);
  }

  if(chk != 0)
  {
    excom_return_errno();
  }

  memcpy(buffer->buf + buffer->used, str->body, str->size);
  buffer->used += str->size;
  return 0;
}

int excom_buffer_bappend(excom_buffer_t* buffer, excom_buffer_t* src)
{
  int chk = 0;

  if(buffer->used + src->used > buffer->max)
  {
    chk = excom_buffer_resize(buffer, buffer->used + src->used);
  }

  if(chk != 0)
  {
    excom_return_errno();
  }

  memcpy(buffer->buf + buffer->used, src->buf, src->used);
  buffer->used += src->used;

  return 0;
}

int excom_buffer_format(excom_buffer_t* out,
  const char* format, ...)
{
  char *pos, *tmp;
  const char* cur = format;
  uint32_t s = 0;
  va_list va;
  int err = 0;
  va_start(va, format);

  if(out->unmutable)
  {
    errno = EACCES;
    return 1;
  }

  while((pos = strchr(cur, '%')))
  {
    if(pos != cur)
    {
      err = excom_buffer_cappend(out, cur, pos - cur);
      if(err) { return err; }
    }

    switch(pos[1])
    {
      case 's':
        tmp = va_arg(va, char*);
        err = excom_buffer_cappend(out, tmp, strlen(tmp));
        break;
      case 'S':
        err = excom_buffer_sappend(out, va_arg(va, excom_string_t*));
        break;
      case 'b':
        s = va_arg(va, uint32_t);
        err = excom_buffer_cappend(out, va_arg(va, char*), s);
        break;
      case '%':
        err = excom_buffer_cappend(out, "%", 1);
        break;
      default:
        excom_check_error(EILSEQ, 0, "Unknown parameter in " \
          "excom_event_buffer_format");
        break;
    }

    if(err) { return err; }

    cur = pos + 2;
  }

  if(cur[0])
  {
    err = excom_buffer_cappend(out, cur, strlen(cur));
    if(err) { return err; }
  }

  va_end(va);

  return 0;
}

void excom_buffer_inspect(excom_buffer_t* buffer)
{
  size_t i;

  printf("\nbuffer(%zu/%zu): ", buffer->used, buffer->max);

  for(i = 0; i < buffer->used; i++)
  {
    printf("%02x(%c) ", buffer->buf[i], buffer->buf[i]);
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
