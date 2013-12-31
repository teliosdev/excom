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
  *(thread->thread) = CreateThread(NULL, 0,
    excom_thread_wrapper, data, 0, NULL);
  if(*thread == NULL)
  {
    return GetLastError();
  }
#endif
}

int excom_mutex_lock(excom_mutex_t* mutex)
{
#ifdef EXCOM_POSIX
  return pthread_mutex_lock(mutex);
#else
  return !(WaitForSingleObject(*mutex, INFINITE) == WAIT_OBJECT_0);
#endif
}

int excom_mutex_unlock(excom_mutex_t* mutex)
{
#ifdef EXCOM_POSIX
  return pthread_mutex_unlock(mutex);
#else
  if(ReleaseMutex(*mutex) == 0)
  {
    return GetLastError();
  }
  else
  {
    return 0;
  }
#endif
}

