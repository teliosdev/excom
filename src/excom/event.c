#include "excom.h"
#include <stdarg.h>

// you're gone, gone, gone away, I watched you disappear
// all that's left is the ghost of you
// now we're torn, torn, torn apart
// there's nothing we can do
// just let me go, we'll meet again soon

int excom_event_buffer_init(excom_event_buffer_t* buf,
  size_t max)
{
  buf->max = max + 1;
  buf->data = 0;
  buf->pos = 0;

  buf->start = buf->cur = excom_malloc((max + 1) * sizeof(char));
  buf->freeable = true;

  if(buf->start == NULL)
  {
    excom_return_errno();
  }

  return excom_mutex_init(&buf->mutex);
}

int excom_event_buffer_init2(excom_event_buffer_t* buf,
  excom_string_t* str)
{
  buf->max = str->size + 1;
  buf->data = str->size;
  buf->pos = 0;

  buf->start = buf->cur = str->body;
  buf->freeable = str->freeable;

  return excom_mutex_init(&buf->mutex);
}

int excom_event_buffer_append(excom_event_buffer_t* buf,
  size_t appends, ...)
{
  va_list args;
  size_t i, new_size, cur;
  excom_string_t* strs[appends];
  void* new_buf;

  i = excom_mutex_lock(&buf->mutex);
  if(i != 0)
  {
    return i;
  }

  new_size = buf->data - buf->pos + 1;
  va_start(args, appends);
  for(i = 0; i < appends; i++)
  {
    strs[i] = va_arg(args, excom_string_t*);
    new_size += strs[i]->size;
  }

  new_buf = excom_malloc(new_size);

  if(new_buf == NULL)
  {
    excom_return_errno();
  }

  cur = 0;
  if(buf->start)
  {
    memcpy(new_buf, buf->start + buf->pos, buf->data - buf->pos);
  }
  cur = buf->data;
  for(i = 0; i < appends; i++)
  {
    memcpy(new_buf + cur, strs[i]->body, strs[i]->size);
    cur += strs[i]->size;
  }

  if(buf->freeable)
  {
    excom_free(buf->start);
  }

  buf->max   = new_size;
  buf->data  = new_size - 1;
  buf->freeable = true;
  buf->pos   = 0;
  buf->start = buf->cur = new_buf;
  buf->start[buf->data] = '\0';
  printf("New buffer is %s\n", buf->start);

  return excom_mutex_unlock(&buf->mutex);
}

int excom_event_buffer_write(excom_event_buffer_t* buf,
  int sock)
{
  int err = 0;
  ssize_t written;

  if(buf->pos != buf->data)
  {
    excom_mutex_lock(&buf->mutex);
    written = write(sock, buf->cur, buf->data - buf->pos);

    if(written < 0)
    {
      err = errno;
      errno = 0;
      excom_mutex_unlock(&buf->mutex);
      return err;
    }

    buf->pos += written;
    buf->cur += written;

    if(buf->pos == buf->data)
    {
      if(buf->freeable)
      {
        free(buf->start);
      }

      buf->start = buf->cur = NULL;
      buf->data = buf->max = buf->pos = 0;
    }

    err = excom_mutex_unlock(&buf->mutex);
  }

  return err;
}

void excom_event_buffer_destroy(excom_event_buffer_t* buf)
{
  if(buf->freeable)
  {
    excom_free(buf->start);
  }

  excom_free(buf);
}
