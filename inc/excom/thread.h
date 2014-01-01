#ifndef _EXCOM_THREAD_H
#define _EXCOM_THREAD_H

/*!
 * @file "excom/thread.h"
 * Defines types and headers that are used to interface the excom
 * threading API.  The threading API implements threads, mutexes,
 * and conditional variables; it provides a platform-independent
 * interface, but uses the platform's implementation details under
 * the cover.
 *
 * As far as this library is concerned, there are only two types of
 * platforms: those who implement pthreads, and windows platforms.
 * As such, it'll check for either platform and then include the
 * correct files.
 */

/*!
 * A callback for when a new thread is created.  The callback accepts
 * one argument, which can be a pointer to anything (although I don't
 * recommend making it a pointer to anything on the stack).  It can
 * return a pointer to anything, which can be retrieved on the call to
 * excom_thread_join.
 */
typedef void* (excom_thread_proc_t)(void*);

typedef struct excom_thread_data {
  void* arg;
  excom_thread_proc_t* proc;
  excom_thread_t* thread;
} excom_thread_data_t;

#ifdef EXCOM_POSIX
#  include "excom/thread/posix.h"
#else
#  include "excom/thread/windows.h"
#endif

/*!
 * Performs the required operations to start the threading for the
 * excom library. MUST BE CALLED BEFORE ANY OTHER THREADING FUNCTIONS
 * ARE CALLED.  This helps each thread keep track of who they are,
 * and what their return values should be.
 */
void excom_thread_load();

/*!
 * Creates a thread, and returns the type to identify it.  This
 * information such as platform-specific information to keep track of
 * the thread, and the return value of the thread.
 *
 * @param [out] thread The place to store the thread information.
 *   This can be on the stack as long as the stack exists for the
 *   duration of the thread.
 * @param [in] proc The function that will be executed on thread
 *   start.
 * @param [in] arg The argument that will be passed to the function
 *   that starts in the thread.
 * @returns An error, if it occured; 0 otherwise.
 */
int excom_thread_init(
  excom_thread_t* thread,
  excom_thread_proc_t* proc,
  void* arg);

/*!
 * Exists the current thread, with the given return value.  If called
 * in the main thread, discards the return value, but otherwise exits
 * main thread, as well.
 *
 * @param [in] retval The return value of the thread.
 */
void excom_thread_exit(void* retval);

/*!
* @returns An error code.
*/
int excom_thread_join(excom_thread_t* thread, void** result);

excom_thread_t* excom_thread_current();

int excom_mutex_lock(excom_mutex_t* mutex);
int excom_mutex_unlock(excom_mutex_t* mutex);
int excom_cond_wait(excom_cond_t* cond, excom_mutex_t* mutex);

int excom_cond_broadcast(excom_cond_t* cond);
int excom_cond_signal(excom_cond_t* cond);

int excom_tls_key_init(excom_tls_key_t* key);
void* excom_tls_get(excom_tls_key_t key);
int excom_tls_set(excom_tls_key_t key, void* value);

#endif
