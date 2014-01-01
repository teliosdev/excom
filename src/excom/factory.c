#include "excom.h"
#include <string.h>

#define POP(item, list) do                                      \
{                                                               \
  if((item)->prev != NULL) (item)->prev->next = (item)->next;   \
  if((item)->next != NULL) (item)->next->prev = (item)->prev;   \
  if((list) == (item)) (list) = (item)->next;                   \
  (item)->prev = (item)->next = NULL;                           \
} while(0)

#define PUSH(item, list) do    \
{                              \
  (item)->prev = NULL;         \
  (item)->next = (list);       \
  (item)->next->prev = (list); \
  (list) = (item);             \
} while(0)

static void* excom_factory_work(void* ptr)
{
  excom_factory_worker_t* worker = (excom_factory_worker_t*) ptr;
  excom_factory_job_t* job;

  while(true)
  {
    excom_mutex_lock(&worker->factory->job.mutex);
    excom_cond_wait(&worker->factory->job.cond, &worker->factory->job.mutex);
    job = worker->factory->jobs;

    if(job != NULL)
    {
      POP(job, worker->factory->jobs);
    }
    excom_mutex_unlock(&worker->factory->job.mutex);

    if(worker->status == EXCOM_FACTORY_WORKER_TERM)
    {
      break;
    }

    if(job == NULL)
    {
      continue;
    }

    (*job->runner)(job->data);

    free(job);
  }

  excom_thread_exit(NULL);

  return NULL;
}

int excom_factory_init(excom_factory_t* factory, uint8_t workers)
{
  int err;
  size_t i;
  excom_factory_worker_t* worker;

  if(workers < 1)
  {
    return EINVAL;
  }

  excom_cond_init(factory->job.cond);
  excom_mutex_init(factory->job.mutex);

  if(errno > 0)
  {
    err = errno;
    errno = 0;

    return err;
  }

  factory->num = workers;
  factory->workers = excom_malloc(workers *
    sizeof(excom_factory_worker_t));

  if(factory->workers == NULL)
  {
    err = errno;
    errno = 0;

    return err;
  }

  for(i = 0; i < workers; i++)
  {
    worker = factory->workers + i;
    worker->factory = factory;
    worker->status  = EXCOM_FACTORY_WORKER_WORK;
    worker->err     = 0;
    memset(&worker->thread, 0, sizeof(worker->thread));

    err = excom_thread_create(&worker->thread, excom_factory_work,
      worker);

    if(err != 0)
    {
      worker->err    = err;
      worker->status = EXCOM_FACTORY_WORKER_FAIL;
    }
  }

  return 0;
}

void excom_factory_destroy(excom_factory_t* factory)
{
  size_t i;
  excom_factory_worker_t* worker;

  for(i = 0; i < factory->num; i++)
  {
    worker = factory->workers + i;
    worker->status = EXCOM_FACTORY_WORKER_TERM;
  }

  excom_mutex_lock(&factory->job.mutex);
  factory->jobs = NULL;

  excom_cond_broadcast(&factory->job.cond);
  excom_mutex_unlock(&factory->job.mutex);

  for(i = 0; i < factory->num; i++)
  {
    excom_thread_join(&factory->workers[i].thread);
  }

  free(factory->workers);
}

void excom_factory_add_job(excom_factory_t* factory,
  excom_factory_job_runner_t* runner, void* data)
{
  excom_factory_job_t* job;

  job = malloc(sizeof(excom_factory_job_t));
  job->runner = runner;
  job->data   = data;

  excom_mutex_lock(&factory->job.mutex);
  PUSH(job, factory->jobs);
  excom_cond_signal(&factory->job.cond);
  excom_mutex_unlock(&factory->job.mutex);
}
