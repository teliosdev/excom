#ifndef _EXCOM_THREAD_H
#define _EXCOM_THREAD_H

typedef void* (excom_thread_proc_t)(void*);

#ifdef EXCOM_POSIX
#  include "excom/thread/posix.h"
#else
#  include "excom/thread/windows.h"
#endif

/*
 * Thread procs must be defined as such:
 *
 *   EXCOM_THREAD_PROC some_thread_func(void* arg)
 *
 * This is to maintain compatibility between platforms.  They must
 * also return the value EXCOM_THREAD_RETURN, for the same reason.
 * If these are not met, compilers will scream, the Earth will open,
 * and the Third Impact will occur.
 */

/*!
 * Creates a thread, and returns the type to identify it.  This type,
 * depending on the platform, could either be a pthread_t or a struct;
 * therefore, my advice is, don't rely on it being one thing or
 * another.
 *
 * @param [in] proc The function that will be executed on thread
 *   start.
 * @param [in] arg The argument that will be passed to the function
 *   that starts in the thread.
 * @returns An excom_thread_t to describe the thread.
 */
int excom_thread_create(
  excom_thread_t* thread,
  excom_thread_proc_t* proc,
  void* arg);

int excom_mutex_lock(excom_mutex_t* mutex);
int excom_mutex_unlock(excom_mutex_t* mutex);

#endif
