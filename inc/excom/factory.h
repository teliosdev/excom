#ifndef _EXCOM_FACTORY_H
#define _EXCOM_FACTORY_H

typedef enum excom_factory_worker_status {
  EXCOM_FACTORY_WORKER_FAIL = 0u,
  EXCOM_FACTORY_WORKER_WORK,
  EXCOM_FACTORY_WORKER_TERM
} excom_factory_worker_status_t;

typedef struct excom_factory_job {
  void* (*runner)(struct excom_factory_job*, void*);
  void* data;
  struct job* prev;
  struct job* next;
} excom_factory_job_t;

typedef struct excom_factory_worker {
  excom_thread_t thread;
  excom_factory_worker_status_t status;
  int err;
  struct excom_factory* factory;
} excom_factory_worker_t;

typedef struct excom_factory {
  size_t num;
  excom_factory_worker_t* workers;
  excom_factory_job_t* jobs;
  struct {
    excom_mutex_t mutex;
    excom_cond_t cond;
  } job;
} excom_factory_t;

int excom_factory_init(excom_factory_t* factory, int workers);

#endif
