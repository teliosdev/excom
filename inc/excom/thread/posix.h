#ifndef _EXCOM_THREAD_POSIX_H
#define _EXCOM_THREAD_POSIX_H

typedef struct excom_thread {
  pthread_t thread;
  void* ret;
} excom_thread_t;

typedef pthread_mutex_t excom_mutex_t;

typedef pthread_cond_t excom_cond_t;

#define EXCOM_THREAD_RETURN NULL

#define excom_cond_init(cond) (pthread_cond_init(&cond, NULL))
#define excom_mutex_init(mutex) (pthread_mutex_init(&mutex, NULL))

#endif
