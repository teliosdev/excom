#include "excom.h"
#ifdef EXCOM_USE_EPOLL

int excom_event_base_init(excom_event_base_t* base,
  excom_event_runner_t* runner)
{
  base->loop      = true;
  base->epollfd   = epoll_create1(0);
  base->timeout   = 500;
  base->maxevents = 10;
  base->runner    = runner;

  if(base->epollfd < 0)
  {
    excom_return_errno();
  }

  return 0;
}

int excom_event_add(excom_event_base_t* base,
  excom_event_t* event)
{
  int err;
  struct epoll_event epevent;

  epevent.events  = event->flags;
  epevent.data.ptr = event;

  err = epoll_ctl(base->epollfd, EPOLL_CTL_ADD, event->fd, &epevent);

  if(err < 0)
  {
    excom_return_errno();
  }

  return 0;
}

int excom_event_remove(excom_event_base_t* base,
  excom_event_t* event)
{
  int err;

  // don't worry, the last argument is ignored.
  err = epoll_ctl(base->epollfd, EPOLL_CTL_DEL, event->fd,
    (struct epoll_event*) event);

  if(err < 0)
  {
    excom_return_errno();
  }

  return 0;
}

void excom_event_loop(excom_event_base_t* base, void* ptr)
{
  //struct epoll_event* events;
  excom_event_t event, *eptr;
  int n, i;
  struct epoll_event events[base->maxevents];
  event.base = base;

  while(base->loop)
  {
    n = epoll_wait(base->epollfd, events, base->maxevents,
      base->timeout);

    for(i = 0; i < n; i++)
    {
      eptr = (excom_event_t*) events[i].data.ptr;
      event.root  = eptr;
      event.fd    = eptr->fd;
      event.data  = eptr->data;
      event.flags = events[i].events;
      (*base->runner)(event, ptr);
    }
  }
}

void excom_event_loop_end(excom_event_base_t* base)
{
  base->loop = false;
}

#endif
