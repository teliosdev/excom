#include "excom.h"

struct thread_data {
  void* arg;
  excom_thread_proc_t* proc;
  excom_thread_t* thread;
};

#ifdef EXCOM_POSIX
void* excom_thread_wrapper(void* arg)
#else
DWORD WINAPI excom_thread_wrapper(void* arg)
#endif
{
  struct thread_data* thread_data = (struct thread_data*) arg;

  thread_data->thread->ret = (*thread_data->proc)(thread_data->arg);

  free(thread_data);

#ifdef EXCOM_POSIX
  return NULL;
#else
  return 0;
#endif
}

int excom_thread_create(
  excom_thread_t* thread,
  excom_thread_proc_t* proc,
  void* arg)
{
  struct thread_data* data;
  thread->ret = NULL;

  data = malloc(sizeof(struct thread_data));

  data->arg    = arg;
  data->proc   = proc;
  data->thread = thread;

#ifdef EXCOM_POSIX
  return pthread_create((pthread_t*) &thread->thread, NULL,
    excom_thread_wrapper, data);
#else
  thread->thread = CreateThread(NULL, 0,
    excom_thread_wrapper, data, 0, NULL);
  if(thread->thread == NULL)
  {
    return GetLastError();
  }
#endif
}

void excom_thread_exit()
{
#ifdef EXCOM_POSIX
  pthread_exit(NULL);
#else
  ExitThread(0);
#endif
}

int excom_thread_join(excom_thread_t* thread, void** result)
{
  int out;
#ifdef EXCOM_POSIX
  out = pthread_join(thread->thread, NULL);
#else
  if(WaitForSingleObject(thread->thread, INFINITE) == WAIT_FAILED)
  {
    out = GetLastError();
  }
  else
  {
    out = 0;
  }

#endif

  if(result != NULL)
  {
    result[0] = thread->ret;
  }
  return out;
}

int excom_mutex_lock(excom_mutex_t* mutex)
{
#ifdef EXCOM_POSIX
  return pthread_mutex_lock(mutex);
#else
  EnterCriticalSection(mutex);
  return 0;
#endif
}

int excom_mutex_unlock(excom_mutex_t* mutex)
{
#ifdef EXCOM_POSIX
  return pthread_mutex_unlock(mutex);
#else
  LeaveCriticalSection(mutex);
  return 0;
#endif
}

int excom_cond_wait(excom_cond_t* cond, excom_mutex_t* mutex)
{
#ifdef EXCOM_POSIX
  return pthread_cond_wait(cond, mutex);
#else
  if(SleepConditionVariableCS(cond, mutex, INFINITE))
  {
    return 0;
  }
  else
  {
    return GetLastError();
  }
#endif
}

int excom_cond_signal(excom_cond_t* cond)
{
#ifdef EXCOM_POSIX
  return pthread_cond_signal(cond);
#else
  WakeConditionVariable(cond);
  return 0;
#endif
}

int excom_cond_broadcast(excom_cond_t* cond)
{
#ifdef EXCOM_POSIX
  return pthread_cond_broadcast(cond);
#else
  WakeAllConditionVariable(cond);
  return 0;
#endif
}
