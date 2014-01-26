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
 * @defgroup threading Threading API
 *   Contains functions that relate directly to threading, and threading
 *   alone.
 *
 * @defgroup mutex Mutex API
 *   Contains functions that relate directly to mutexes.
 *
 * @defgroup condvar Conditional Variable API
 *   Contains functions that relate directly to conditional variables.
 *
 * @defgroup tls Thread-Local Storage API
 *   Contains functions that relate directly to thread-local storage.
 */

/*!
 * A callback for when a new thread is created.  The callback accepts
 * one argument, which can be a pointer to anything (although I don't
 * recommend making it a pointer to anything on the stack).  It can
 * return a pointer to anything, which can be retrieved on the call to
 * excom_thread_join().
 */
typedef void* (excom_thread_proc_t)(void*);

/*!
 * Local thread data that contains information about the thread, such
 * as the original thread function and the argument passed to it, as
 * well as a pointer to the thread struct.
 */
typedef struct excom_thread_data
{

  /*!
   * A pointer to the argument that is passed to the thread function.
   */
  void* arg;

  /*!
   * The function that is/was executed on thread start.
   */
  excom_thread_proc_t* proc;

  /*!
   * The thread struct that was returned that contains information
   * about the thread.
   */
  struct excom_thread* thread;
} excom_thread_data_t;

#ifdef EXCOM_POSIX
#  include "excom/thread/posix.h"
#elif defined(EXCOM_WIN)
#  include "excom/thread/windows.h"
#endif

#ifdef DOXYGEN

/*!
 * Contains information about the thread.  This can be used to
 * identify a thread.
 *
 * @ingroup threading
 */
typedef struct excom_thread
{

  /*!
   * A platform-specific identifier for the thread.
   */
  void thread;

  /*!
   * The return value of the thread.  This is returned by
   * excom_thread_join().  It should never be accessed directly.
   */
  void* ret;
} excom_thread_t;

/*!
 * Typedefs the platform-specific type for mutexes to our
 * platform-independent type.
 *
 * @ingroup mutex
 */
typedef void excom_mutex_t;

/*!
 * Typedefs the platform-specific type for conditional variables to
 * our platform-independent type.
 *
 * @ingroup condvar
 */
typedef void excom_cond_t;

/*!
 * Typedefs the platform-specific type for Thread-Local Storage keys
 * to our platform-independent type.
 *
 * @ingroup tls
 */
typedef void excom_tls_key_t;

#endif

/*!
 * Performs the required operations to start the threading for the
 * excom library. **MUST BE CALLED BEFORE ANY OTHER THREADING
 * FUNCTIONS ARE CALLED**.  This helps each thread keep track of who
 * they are, and what their return values should be.
 *
 * @ingroup threading
 * @returns An error code, or `0` if there was none.
 */
int excom_thread_load();

/*!
 * Creates a thread, and returns the type to identify it.  This
 * information such as platform-specific information to keep track of
 * the thread, and the return value of the thread.
 *
 * @ingroup threading
 * @param [out] thread The place to store the thread information.
 *   This can be on the stack as long as the stack exists for the
 *   duration of the thread.
 * @param [in]  proc   The function that will be executed on thread
 *   start.
 * @param [in]  arg    The argument that will be passed to the
 *   function that starts in the thread.
 * @returns An error code, or `0` if there was none.
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
 * @ingroup threading
 * @param [in] retval The return value of the thread.
 */
void excom_thread_exit(void* retval);

/*!
 * Joins the given thread to the current thread.  If the given thread
 * is terminated, this function returns immediately; otherwise, it
 * blocks and waits for the given thread to terminate.  If result is
 * a non-null value, then the result of the thread will be placed
 * in that address.
 *
 * @ingroup threading
 * @param [in]  thread The identifier for the thread to join.
 * @param [out] result The place to put the result of the thread.  If
 *   this is non-null, the result of the thread will be stored there;
 *   otherwise, it is ignored.
 * @returns An error code, or `0` if there was none.
 */
int excom_thread_join(excom_thread_t* thread, void** result);

/*!
 * Retrieves the current thread information.  If this is called in the
 * main thread, it will return NULL.
 *
 * @ingroup threading
 * @returns The struct identifying the thread, or NULL.
 */
excom_thread_t* excom_thread_current();

/*!
 * Initializes the given mutex.  Note that this must be called before
 * any other actions take place.  Another thing to note is that at the
 * moment, excom mutexes do not provide any recursion or error checking
 * guarentees.
 *
 * @ingroup mutex
 * @param [out] mutex The mutex to initialize.
 * @returns An error code, or `0` if there was none.
 */
int excom_mutex_init(excom_mutex_t* mutex);

/*!
 * Locks the given mutex.  This WILL block until a lock can be
 * obtained.
 *
 * @ingroup mutex
 * @param [in] mutex The mutex to lock.
 * @returns An error code, or `0` if there was none.
 */
int excom_mutex_lock(excom_mutex_t* mutex);

/*!
 * Unlocks the given mutex, reguardless if the current thread is the
 * owner of the mutex.
 *
 * @ingroup mutex
 * @param [in] mutex The mutex to unlock.
 * @returns An error code, or `0` if there was none.
 */
int excom_mutex_unlock(excom_mutex_t* mutex);

/*!
 * Initializes the given condition variable.  The condition variable
 * takes on the default properties on all systems.
 *
 * @ingroup condvar
 * @param [out] cond The place to store condition variable.
 * @returns An error code, or `0` if there was none.
 */
int excom_cond_init(excom_cond_t* cond);

/*!
 * Waits for a signal on a condition variable.  The given mutex should
 * be locked to the current thread when this function is called, and
 * the mutex will be locked to the current thread on return.
 *
 * @ingroup mutex condvar
 * @param [in] cond  The condition variable to wait on.
 * @param [in] mutex The mutex to lock and unlock.
 * @returns An error code, or `0` if there was none.
 */
int excom_cond_wait(excom_cond_t* cond, excom_mutex_t* mutex);

/*!
 * Broadcasts a signal to all threads that are listening to the
 * condition variable.
 *
 * @ingroup condvar
 * @param [in] cond The condition variable to broadcast on.
 * @returns An error code, or `0` if there was none.
 */
int excom_cond_broadcast(excom_cond_t* cond);

/*!
 * Sends a signal to the first listening thread.
 *
 * @ingroup condvar
 * @param [in] cond The condition variable to signal on.
 * @returns An error code, or `0` if there was none.
 */
int excom_cond_signal(excom_cond_t* cond);

/*!
 * Initializes a TLS key.  TLS keys are used on a global basis, to
 * identify a key-value pair that can be stored with every thread.
 * The key-value pair itself is thread-local, but the key is the
 * same between all threads.
 *
 * @ingroup tls
 * @param [out] key The place to store the TLS key.
 * @returns An error code, or `0` if there was none.
 */
int excom_tls_key_init(excom_tls_key_t* key);

/*!
 * Gets the value that is associated with the TLS key in this thread.
 *
 * @ingroup tls
 * @param [in] key The key in the key-value pair.
 * @returns The value in the key-value pair, or NULL (if nothing was
 *   set).
 */
void* excom_tls_get(excom_tls_key_t key);

/*!
 * Sets the value that is associated with the TLS key in this thread.
 *
 * @ingroup tls
 * @param [in] key   The key in the key-value pair.
 * @param [in] value The value to set in the key-value pair.
 * @returns An error code, or `0` if there was none.
 */
int excom_tls_set(excom_tls_key_t key, void* value);

#endif
