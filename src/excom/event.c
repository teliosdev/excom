#include "excom.h"
#include <stdarg.h>
#include <ctype.h>

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

int excom_event_buffer_append_num(excom_event_buffer_t* buf,
  size_t appends, excom_string_t** strs)
{
  size_t i, new_size, cur;
  void* new_buf;

  i = excom_mutex_lock(&buf->mutex);
  if(i != 0)
  {
    return i;
  }

  new_size = buf->data - buf->pos + 1;
  for(i = 0; i < appends; i++)
  {
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

int excom_event_buffer_append(excom_event_buffer_t* buf,
  size_t appends, ...)
{
  va_list args;
  excom_string_t* strs[appends];
  size_t i;

  va_start(args, appends);
  for(i = 0; i < appends; i++)
  {
    strs[i] = va_arg(args, excom_string_t*);
  }
  va_end(args);

  return excom_event_buffer_append_num(buf, appends, strs);

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

void excom_event_buffer_format_va(excom_event_buffer_t* buf,
  const char* format, va_list va)
{
  excom_string_t ary[64];
  excom_string_t* ptrs[64];
  char *pos, *tmp, *b = NULL;
  const char* cur = format;
  size_t strs = 0;
  uint32_t s = 0;

  while((pos = strchr(cur, '%')))
  {
    if(pos != cur)
    {
      excom_string_dupfill(&ary[strs++], pos - cur, pos);
    }

  top_switch:
    switch(pos[1])
    {
      case '%':
        excom_string_dupfill(&ary[strs++], 2, "%\0");
        b = NULL;
        break;
      case 's':
        tmp = va_arg(va, char*);
        excom_string_dupfill(&ary[strs++], strlen(tmp), tmp);
        b = NULL;
        break;
      case 'S': {
        excom_string_t* str = va_arg(va, excom_string_t*);
        excom_string_dup(str, &ary[strs++]);
        b = NULL;
        break;
      }
      case 'd':
        s = va_arg(va, uint32_t);
      case 'b': {
        uint32_t num;
        if(b != NULL)
          num = strtol(b, NULL, 0);
        else
          num = s;
        tmp = va_arg(va, char*);
        excom_string_dupfill(&ary[strs++], num, tmp);
        b = NULL;
        break;
      }

      default:
        if(isxdigit(pos[1]) || pos[1] == 'x' || pos[1] == 'X') {
          if(b == NULL) { b = pos + 1; }
          pos++;
          goto top_switch;
        } else {
          excom_check_error(EILSEQ, 0, "Unknown parameter in " \
            "excom_event_buffer_format");
          break;
        }
    }

  }

  for(s = 0; s < strs; s++)
  {
    ptrs[s] = &ary[s];
  }

  excom_event_buffer_append_num(buf, strs, ptrs);
}

void excom_event_buffer_format(excom_event_buffer_t* buf,
  const char* format, ...)
{
  va_list va;

  va_start(va, format);
  excom_event_buffer_format_va(buf, format, va);
  va_end(va);
}
