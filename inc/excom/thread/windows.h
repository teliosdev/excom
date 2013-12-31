#ifndef _EXCOM_THREAD_WINDOWS_H
#define _EXCOM_THREAD_WINDOWS_H

typedef struct excom_thread {
  HANDLE thread;
  void* ret;
} excom_thread_t;

typedef HANDLE excom_mutex_t;

typedef CONDITION_VARIABLE excom_cond_t;

#define EXCOM_THREAD_RETURN 0

#define excom_cond_init(cond) (InitializeConditionVariable(&cond))
#define excom_mutex_init(mutex) \
  ((mutex) = CreateMutex(NULL, FALSE, NULL))

#endif
