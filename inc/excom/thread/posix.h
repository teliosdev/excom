#ifndef _EXCOM_THREAD_POSIX_H
#define _EXCOM_THREAD_POSIX_H

typedef struct excom_thread
{
  pthread_t thread;
  void* ret;
  excom_thread_status_t status;
} excom_thread_t;

typedef pthread_mutex_t excom_mutex_t;

typedef pthread_cond_t excom_cond_t;

typedef pthread_key_t excom_tls_key_t;

#endif
