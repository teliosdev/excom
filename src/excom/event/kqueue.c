#include "excom.h"
#ifdef EXCOM_KQUEUE

int excom_event_base_kqueue_init(excom_event_base_kqueue_t* base,
  excom_event_runner_t* runner)
{
  int err = 0;
  base->loop      = true;
  base->kqueuefd  = kqueue();
  base->runner    = runner;
  base->maxevents = 32;
  base->timeout   = 500;

  if(base->kqueuefd == -1)
  {
    err = errno;
    errno = 0;
  }

  return err;
}

int excom_event_kqueue_update(excom_event_base_kqueue_t* base,
  excom_event_t* event)
{
  return excom_event_kqueue_add(base, event);
}

int excom_event_kqueue_add(excom_event_base_kqueue_t* base,
  excom_event_t* event)
{
  int ret;
  short filter = 0;
  excom_kevent_t kev;
  event->base = base;

  if(event->flags & (EXCOM_EVENT_READ | EXCOM_EVENT_ERROR |
    EXCOM_EVENT_CLOSE))
  {
    filter |= EVFILT_READ;
  }
  if(event->flags & EXCOM_EVENT_WRITE)
  {
    filter |= EVFILT_WRITE;
  }

  excom_kevent_set(&kev, event->fd, filter, EV_ADD | EV_ENABLE, 0, 0, event);

  ret = excom_kevent(base->kqueuefd, &kev, 1, NULL, 0, NULL);

  if(ret < 0)
  {
    excom_return_errno();
  }
  return 0;
}

int excom_event_kqueue_remove(excom_event_base_t* base,
  excom_event_t* event)
{
  int ret;
  excom_kevent_t kev;

  excom_kevent_set(&kev, event->fd, 0, EV_DELETE, 0, 0, NULL);

  ret = excom_kevent(base->kqueuefd, &kev, 1, NULL, 0, NULL);

  if(ret < 0)
  {
    excom_return_errno();
  }

  return 0;
}

void excom_event_kqueue_loop(excom_event_base_kqueue_t* base, void* ptr)
{
  //excom_kevent_t* events;
  excom_event_t event, *eptr;
  int n, i;
  excom_kevent_t events[base->maxevents];
  event.base = base;
  struct timespec timeout;


  // convert our given timeout to a timespec.
  timeout.tv_sec  = (time_t) base->timeout / 1000;
  timeout.tv_nsec = (base->timeout % 1000) * 1000000;

#define curev (events[i])
  while(base->loop)
  {
    n = excom_kevent(base->kqueuefd, NULL, 0, events,
      base->maxevents, &timeout);

    if(n < 0)
    {
      event.root = NULL;
      event.fd   = base->kqueuefd;
      event.data = NULL;
      event.flags = EXCOM_EVENT_ERROR;

      (*base->runner)(event, ptr);
    }

    for(i = 0; i < n; i++)
    {
      eptr = (excom_event_t*) curev.udata;
      event.root = eptr;
      event.fd   = curev.ident;
      event.flags = 0;
      if(eptr)
        event.data = eptr->data;

      if(curev.flags & EV_EOF)
      {
        event.flags |= EXCOM_EVENT_CLOSE;
      }
      if(curev.flags & EV_ERROR)
      {
        event.flags |= EXCOM_EVENT_ERROR;
      }
      if(curev.filter & EVFILT_READ)
      {
        event.flags |= EXCOM_EVENT_READ;
      }
      if(curev.filter & EVFILT_WRITE)
      {
        event.flags |= EXCOM_EVENT_WRITE;
      }

      (*base->runner)(event, ptr);

    }
  }
#undef curev
}

void excom_event_kqueue_loop_end(excom_event_base_kqueue_t* base)
{
  base->loop = false;
}

#endif
