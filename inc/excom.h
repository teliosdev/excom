#ifndef _EXCOM_H
#define _EXCOM_H


#if defined (__unix__) || defined (__APPLE__)
#  define EXCOM_UNIX
#  include <pthread.h>
#  include <unistd.h>
#  include <stdbool.h>
#  ifdef _POSIX_VERSION
#    define EXCOM_POSIX
#  endif
#elif defined(_WIN32)
#  define EXCOM_WIN
#else
#  error Unsupported platform.
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <event2/event.h>
#include <event2/thread.h>

#include "excom/string.h"
#include "excom/protocol.h"
#include "excom/server.h"
#include "excom/thread.h"
#include "excom/factory.h"


#define excom_malloc malloc
#define excom_calloc calloc
#define excom_free free
#define excom_realloc realloc

#define excom_check_error(err, failcode, desc) do \
{                                            \
  char* buf;                                 \
  if(err)                                    \
  {                                          \
    buf = strerror(err);                     \
    fprintf("[excom] error: " #desc          \
      ": %s\n", buf);                        \
    if(failcode)                             \
      exit(failcode);                        \
  }                                          \
} while(0)

#endif
