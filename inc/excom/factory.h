#ifndef _EXCOM_FACTORY_H
#define _EXCOM_FACTORY_H

/*!
 * @file "excom/factory.h"
 * Distributes tasks to worker threads in order to perform said task.
 * This allows parallelization of processes, but also causes the
 * concern of thread safety to the application.
 */

/*!
 * The status of the current worker.  It primarily describes the
 * status to the worker itself, since `EXCOM_FACTORY_WORKER_TERM` will
 * cause a worker to terminate; however, it can be used externally to
 * detect the status of the worker.
 */
typedef enum excom_factory_worker_status
{

  /*!
   * This is set when the worker failed to start.  It is the only time
   * this is set as the status.
   */
  EXCOM_FACTORY_WORKER_FAIL = 0u,

  /*!
   * This is set when the worker is working.  This is the default
   * status of a worker.
   */
  EXCOM_FACTORY_WORKER_WORK,

  /*!
   * This is set when a worker should terminate or has terminated.
   * If checked externally, it should be assumed that the worker has
   * terminated.  If checked internally by the worker, the worker
   * should shut down.
   */
  EXCOM_FACTORY_WORKER_TERM
} excom_factory_worker_status_t;

/*!
 * A function that contains the actual job that will be run by the
 * worker.  This should contain time consuming tasks.
 */
typedef void* (excom_factory_job_runner_t)(void*);


/*!
 * Describes a job that can be performed by the factory.  Ideally,
 * this struct is always allocated.  The main job list is a
 * doubly linked list that can be appended to.  It is also locked
 * when being read from, or when being added to.
 */
typedef struct excom_factory_job
{

  /*!
   * The function that contains the code to run the job.
   */
  excom_factory_job_runner_t* runner;

  /*!
   * The argument to be passed to the function.
   */
  void* data;

  /*!
   * The previous job in the list.
   */
  struct excom_factory_job* prev;

  /*!
   * The next job in the list.
   */
  struct excom_factory_job* next;
} excom_factory_job_t;

/*!
 * Describes a worker that runs tasks.
 */
typedef struct excom_factory_worker
{

  /*!
   * The thread that the worker is running in.  The value of this is
   * undefined if status isn't `EXCOM_FACTORY_WORKER_WORK`.
   */
  excom_thread_t thread;

  /*!
   * The status of the worker.
   */
  excom_factory_worker_status_t status;

  /*!
   * The specific error that occured with the worker.  The value of
   * this is undefined if status isn't `EXCOM_FACTORY_WORKER_FAIL`.
   */
  int err;

  /*!
   * A pointer to the factory this worker is a part of.
   */
  struct excom_factory* factory;
} excom_factory_worker_t;


/*!
 * The factory that manages the workers and jobs.
 */
typedef struct excom_factory
{

  /*!
   * The number of workers there are.
   */
  size_t num;

  /*!
   * An array of workers.  The workers are allocated as an entire
   * block in memory, so pointer arithmatic is required to access
   * the pointers of other workers.  This defaults to the pointer
   * of the first worker.
   */
  excom_factory_worker_t* workers;

  /*!
   * The first job in the doubly linked list of jobs.  The first
   * job is the next job that the workers will work on.
   */
  excom_factory_job_t* jobs;

  /*!
   * Contains the locking information required for the job linked
   * list, in order to make the read and writes to it atomic.
   */
  struct
  {
    /*!
     * A mutex to lock in order to gain access to the job list.
     */
    excom_mutex_t mutex;

    /*!
     * A conditional variable to signal that the job list has changed.
     */
    excom_cond_t cond;
  } job;
} excom_factory_t;


/*!
 * Initializes the factory, giving it the specified number of workers.
 * This starts each worker in its own thread, ready to recieve work.
 *
 * @param [in] factory The factory to initialize.
 * @param [in] workers The number of workers to create.
 * @returns An error code, or `0` if there was none.
 */
int excom_factory_init(excom_factory_t* factory, uint8_t workers);

/*!
 * Destroys the factory, terminating the workers and freeing them, as
 * well as clearing the job list.  All of the values of the factory
 * struct are undefined after this call.  It is highly recommended to
 * call this, since adding jobs requires allocating memory (and
 * destroying the factory frees remaining memory).
 *
 * @param [in] factory The factory to destroy.
 * @returns nothing.
 */
void excom_factory_destroy(excom_factory_t* factory);


/*!
 * Adds a job to the factory list.
 *
 * @param [in] factory The factory to add the job to.
 * @param [in] runner The function to execute.
 * @param [in] data The argument to pass to the function to execute.
 * @returns nothing.
 */
void excom_factory_add_job(excom_factory_t* factory,
  excom_factory_job_runner_t* runner, void* data);

#endif
