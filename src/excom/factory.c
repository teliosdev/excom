#include "excom.h"


static void* excom_factory_work(void* ptr)
{



  return NULL;
}

int excom_factory_init(excom_factory_t* factory, int workers)
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
    worker = factory->workers[i];
    worker->factory = factory;
    worker->status  = EXCOM_FACTORY_WORKER_WORK;
    worker->err     = 0;
    worker->thread  = 0;

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
    worker->status = EXCOM_FACTORY_WORKER_TERM;
  }


}

