#include "excom.h"

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
