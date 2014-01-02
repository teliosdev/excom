#include "excom.h"

excom_tls_key_t local_data;

#ifdef EXCOM_POSIX
void* excom_thread_wrapper(void* arg)
#else
DWORD WINAPI excom_thread_wrapper(void* arg)
#endif
{
  excom_thread_data_t* data = (excom_thread_data_t*) arg;
  excom_tls_set(local_data, data);

  data->thread->ret = (*data->proc)(data->arg);

#ifdef EXCOM_POSIX
  return NULL;
#else
  return 0;
#endif
}

int excom_thread_load()
{
  return excom_tls_key_init(&local_data);
}

int excom_thread_init(
  excom_thread_t* thread,
  excom_thread_proc_t* proc,
  void* arg)
{
  excom_thread_data_t* data;
  thread->ret = NULL;

  data = malloc(sizeof(excom_thread_data_t));

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

void excom_thread_exit(void* retval)
{
  excom_thread_data_t* d = excom_tls_get(local_data);

  if(d != NULL)
  {
    d->thread->ret = retval;
  }

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
    *result = thread->ret;
  }
  return out;
}

excom_thread_t* excom_thread_current()
{
  return (excom_thread_t*) excom_tls_get(local_data);
}

int excom_mutex_init(excom_mutex_t* mutex)
{
#ifdef EXCOM_POSIX
  return pthread_mutex_init(mutex, NULL);
#else
  InitializeCriticalSection(mutex);
  return 0;
#endif
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

int excom_cond_init(excom_cond_t* cond)
{
#ifdef EXCOM_POSIX
  return pthread_cond_init(cond, NULL);
#else
  InitializeConditionVariable(cond);
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

int excom_tls_key_init(excom_tls_key_t* key)
{
#ifdef EXCOM_POSIX
  return pthread_key_create(key, NULL);
#else
  *key = TlsAlloc();
  if(*key == TLS_OUT_OF_INDEXES)
  {
    return GetLastError();
  }
  else
  {
    return 0;
  }
#endif
}

void* excom_tls_get(excom_tls_key_t key)
{
#ifdef EXCOM_POSIX
  return pthread_getspecific(key);
#else
  return TlsGetValue(key);
#endif
}

int excom_tls_set(excom_tls_key_t key, void* value)
{
#ifdef EXCOM_POSIX
  return pthread_setspecific(key, value);
#else
  if(TlsSetValue(key, value) == FALSE)
  {
    return GetLastError();
  }
  else
  {
    return 0;
  }
#endif
}
