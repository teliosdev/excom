#include "excom.h"
#include <string.h>
#include <errno.h>

inline void excom_string_init(excom_string_t* string)
{
  string->size = 0;
  string->body = NULL;
}

inline void excom_string_fill(excom_string_t* string, uint32_t size,
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

  if(string->size == 0)
  {
    out->size = 0;
    out->body = NULL;

    return 0;
  }

  buf = excom_malloc(string->size);

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
