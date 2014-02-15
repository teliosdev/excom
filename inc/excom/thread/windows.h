#ifndef _EXCOM_THREAD_WINDOWS_H
#define _EXCOM_THREAD_WINDOWS_H

#include <synchapi.h>

/*!
*/
typedef struct excom_thread
{
  HANDLE thread;
  void* ret;
  excom_thread_status_t status;
} excom_thread_t;

typedef CRITICAL_SECTION excom_mutex_t;

typedef CONDITION_VARIABLE excom_cond_t;

typedef DWORD excom_tls_key_t;

#endif
