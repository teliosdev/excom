#include "excom.h"
#include <ctype.h>


int excom_buffer_init(excom_buffer_t* buffer, size_t start)
{
  buffer->_init = buffer->max = start;
  buffer->used = 0;
  buffer->unmutable = false;
  buffer->freeable = false;

  buffer->pos = buffer->buf = excom_malloc(start * sizeof(char));

  if(buffer->buf == NULL)
  {
    excom_return_errno();
  }

  if(excom_mutex_init(&buffer->mutex) != 0)
  {
    excom_return_errno();
  }

  return 0;
}

int excom_buffer_reset(excom_buffer_t* buffer)
{
  int err = 0;

  excom_mutex_lock(&buffer->mutex);
  excom_buffer_destroy(buffer);

  buffer->max = buffer->_init;
  buffer->used = 0;
  buffer->freeable = buffer->unmutable = false;

  buffer->pos = buffer->buf = excom_malloc(buffer->max *
    sizeof(char));

  if(buffer->buf == NULL)
  {
    err = errno;
    errno = 0;
    excom_mutex_unlock(&buffer->mutex);
    return err;
  }

  excom_mutex_unlock(&buffer->mutex);

  return 0;
}

int excom_buffer_resize(excom_buffer_t* buffer, size_t to_fit)
{
  size_t new_size = buffer->max;
  uint8_t* new_buf;
  uint32_t distance;

  do {
    new_size *= 2;
  } while(new_size < to_fit);
  new_buf = excom_malloc(new_size);

  if(new_buf == NULL)
  {
    excom_return_errno();
  }

  memcpy(new_buf, buffer->buf, buffer->used);

  distance = buffer->pos - buffer->buf;
  free(buffer->buf);
  buffer->buf = new_buf;
  buffer->pos = buffer->buf + distance;
  buffer->max = new_size;

  return 0;
}

int excom_buffer_cappend(excom_buffer_t* buffer, const char* str, size_t size)
{
  int chk = 0, err = 0;

  if(buffer->unmutable)
  {
    return EACCES;
  }

  excom_mutex_lock(&buffer->mutex);

  if(buffer->used + size > buffer->max)
  {
    chk = excom_buffer_resize(buffer, buffer->used + size);
  }

  if(chk != 0)
  {
    err = errno;
    errno = 0;
    excom_mutex_unlock(&buffer->mutex);
    return err;
  }

  memcpy(buffer->buf + buffer->used, str, size);
  buffer->used += size;

  excom_mutex_unlock(&buffer->mutex);

  return 0;
}

int excom_buffer_sappend(excom_buffer_t* buffer, excom_string_t* str)
{
  int chk = 0, err = 0;

  if(buffer->unmutable)
  {
    return EACCES;
  }

  excom_mutex_lock(&buffer->mutex);

  if(buffer->used + str->size > buffer->max)
  {
    excom_buffer_resize(buffer, buffer->used + str->size);
  }

  if(chk != 0)
  {
    err = errno;
    errno = 0;
    excom_mutex_unlock(&buffer->mutex);
    return err;
  }

  memcpy(buffer->buf + buffer->used, str->body, str->size);
  buffer->used += str->size;

  excom_mutex_unlock(&buffer->mutex);

  return 0;
}

int excom_buffer_bappend(excom_buffer_t* buffer, excom_buffer_t* src)
{
  return excom_buffer_bappend2(buffer, src, src->used);
}

int excom_buffer_bappend_remaining(excom_buffer_t* buffer, excom_buffer_t* src)
{
  return excom_buffer_bappend_remaining2(buffer, src,
                                         excom_buffer_remaining(src) - 1);
}

int excom_buffer_bappend_remaining2(excom_buffer_t* buffer,
                                    excom_buffer_t* src, size_t s)
{
  int chk = 0, err = 0;

  if(buffer->unmutable)
  {
    return EACCES;
  }

  if(s == 0) { return 0; }

  excom_mutex_lock(&buffer->mutex);

  if(buffer->used + s > buffer->max)
  {
    chk = excom_buffer_resize(buffer, buffer->used + s);
  }

  if(chk != 0)
  {
    err = errno;
    errno = 0;
    excom_mutex_unlock(&buffer->mutex);
    return err;
  }

  memcpy(buffer->buf + buffer->used, src->pos, s);
  buffer->used += s;

  excom_mutex_unlock(&buffer->mutex);

  return 0;
}

int excom_buffer_bappend2(excom_buffer_t* buffer, excom_buffer_t* src, size_t amount)
{
  int chk = 0, err = 0;

  if(buffer->unmutable)
  {
    return EACCES;
  }

  if(amount == 0) { return 0; }

  excom_mutex_lock(&buffer->mutex);

  if(buffer->used + amount > buffer->max)
  {
    chk = excom_buffer_resize(buffer, buffer->used + amount);
  }

  if(chk != 0)
  {
    err = errno;
    errno = 0;
    excom_mutex_unlock(&buffer->mutex);
    return err;
  }
  memcpy(buffer->buf + buffer->used, src->buf, amount);
  buffer->used += amount;

  excom_mutex_unlock(&buffer->mutex);

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
    return EACCES;
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
  size_t i,d = 0;

  //excom_mutex_lock(&buffer->mutex);
  printf("\nbuffer(%zu/%zu): ", buffer->used, buffer->max);

  for(i = 0; i < buffer->used && d < 1000; i++)
  {
    d += 1;
    if((buffer->buf + i) == buffer->pos)
    {
      printf("\e[0;1;34m|\e[0m %02x", buffer->buf[i]);
    }
    else
    {
      printf("%02x", buffer->buf[i]);
    }

    if(buffer->buf[i] > 0x20 && buffer->buf[i] < 0x7e)
    {
      printf("(%c)", buffer->buf[i]);
    }
    else
    {
      printf("( )");
    }

    printf(" ");
  }

  if((buffer->buf + buffer->used) == buffer->pos)
  {
    printf("\e[0;1;34m|\e[0m");
  }

  //excom_mutex_unlock(&buffer->mutex);

  printf("\n");
}

void excom_buffer_destroy(excom_buffer_t* buffer)
{
  free(buffer->buf);
  buffer->max  = 0;
  buffer->used = 0;
  buffer->buf  = NULL;
}

int excom_buffer_write(excom_buffer_t* buffer,
  int sock)
{
  int err = 0;
  ssize_t written;
  size_t to_write;

  if(excom_buffer_remaining(buffer) != 0)
  {
    excom_mutex_lock(&buffer->mutex);
    to_write = excom_buffer_remaining(buffer);
    written = write(sock, buffer->pos, to_write);

    if(written < 0)
    {
      err = errno;
      errno = 0;
      excom_mutex_unlock(&buffer->mutex);
      return err;
    }

    buffer->pos += written;

    if(excom_buffer_remaining(buffer) == 0 && buffer->freeable)
    {
      err = excom_mutex_unlock(&buffer->mutex);
      if(err)
      {
        return err;
      }
      excom_buffer_destroy(buffer);
      excom_mutex_lock(&buffer->mutex);
    }

    err = excom_mutex_unlock(&buffer->mutex);
  }

  return err;
}
