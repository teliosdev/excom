#ifndef _EXCOM_H
#define _EXCOM_H

#if defined (__unix__) || defined (__APPLE__)
#  define EXCOM_UNIX
#  include <pthread.h>
#  include <unistd.h>
#  define TRUE true
#  define FALSE false
#  ifdef _POSIX_VERSION
#    define EXCOM_POSIX
#  endif
#elif defined(_WIN32)
#  define EXCOM_WIN
#  include <windows.h>
#else
#  error Unsupported platform.
#endif

#ifdef __APPLE__
#  define EXCOM_MAC
#endif

#ifndef EXCOM_NO_STDBOOL
#  include <stdbool.h>
#else
#  define bool int
#  define true 1
#  define false 0
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <toml.h>

#include "excom/config.h"

#include "excom/string.h"
#include "excom/thread.h"
#include "excom/buffer.h"
#include "excom/encryption.h"
#include "excom/protocol.h"
#include "excom/factory.h"
#include "excom/event.h"
#include "excom/server.h"
#include "excom/client.h"

#if UINTPTR_MAX == 0xffffffffffffffff
#  define EXCOM64
#else
#  define EXCOM32
#endif

#define EXCOM_VERSION "0.0.1"
#define EXCOM_VERSION_MAJOR 0
#define EXCOM_VERSION_MINOR 0
#define EXCOM_VERSION_PATCH 1

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
    fprintf(stderr, "[excom] error: " desc   \
      ": %s\n", buf);                        \
    if(failcode)                             \
      exit(failcode);                        \
  }                                          \
} while(0)

#define excom_return_errno() do { \
  int err;                        \
  err = errno;                    \
  errno = 0;                      \
  return err;                     \
} while(0)

#define excom_check_alloc(alloc) if((alloc) == NULL) excom_return_errno()

#endif
