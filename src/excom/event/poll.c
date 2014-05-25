#include "excom.h"
#include "excom/event/undef.h"
#include "excom/event/poll.h"

#define POLL_SIZE sizeof(struct pollfd)
#define EVENT_SIZE sizeof(struct excom_event*)

int excom_event_base_poll_init(excom_event_base_poll_t* base,
  excom_event_runner_t* runner)
{
  int err;
  base->loop    = true;
  base->fds     = excom_malloc(POLL_SIZE  * EXCOM_POLL_INITIAL_SIZE);
  base->events  = excom_malloc(EVENT_SIZE * EXCOM_POLL_INITIAL_SIZE);
  base->nfds    = 0;
  base->max     = EXCOM_POLL_INITIAL_SIZE;
  base->runner  = runner;
  base->timeout = 500;

  excom_check_alloc(base->fds);
  excom_check_alloc(base->events);
  err = excom_mutex_init(&base->mutex);
  if(err) { return err; }
  return base->fds == NULL;
}

int excom_event_base_poll_destroy(excom_event_base_poll_t* base)
{
  base->loop = false;
  excom_free(base->fds);
  excom_free(base->events);
  base->nfds = 0;
  base->max  = 0;
  base->runner = NULL;
  base->fds = NULL;
  base->events = NULL;
  return 0;
}

int excom_event_poll_update(excom_event_base_poll_t* base,
  excom_event_t* event)
{
  uintptr_t index;
  index = event->_bdata;

  if(index > base->nfds || base->events[index]->fd != event->fd)
  {
    return EINVAL;
  }

  base->fds[index].events = event->flags;
  return 0;
}

int excom_event_poll_add(excom_event_base_poll_t* base,
  excom_event_t* event)
{
  struct pollfd* ev;
  if(base->nfds + 1 > base->max)
  {
    base->max *= 2;
    base->fds    = excom_realloc(base->fds, POLL_SIZE * base->max);
    base->events = excom_realloc(base->events, EVENT_SIZE * base->max);

    excom_check_alloc(base->fds);
    excom_check_alloc(base->events);
  }

  event->base = base;
  event->_bdata = base->nfds;
  ev = base->fds + base->nfds;
  base->events[base->nfds] = event;
  ev->fd = event->fd;
  ev->events = event->flags;
  ev->revents = 0;

  base->nfds += 1;

  return 0;
}

int excom_event_poll_remove(excom_event_base_poll_t* base,
  excom_event_t* event)
{
  uintptr_t index;
  index = event->_bdata;

  if(index > base->nfds || base->events[index]->fd != event->fd)
  {
    return EINVAL;
  }

  if(index + 1 < base->nfds)
  {
    memmove(base->fds + (index + 1), base->fds + index,
      POLL_SIZE * (base->nfds - index));
    memmove(base->events + (index + 1), base->events + index,
      EVENT_SIZE * (base->nfds - index));
  }

  base->nfds -= 1;

  if(base->nfds < (base->max / 2))
  {
    base->max /= 2;
    base->fds    = excom_realloc(base->fds, POLL_SIZE * base->max);
    base->events = excom_realloc(base->events, EVENT_SIZE * base->max);
    excom_check_alloc(base->fds);
    excom_check_alloc(base->events);
  }

  return 0;
}

void excom_event_poll_loop(excom_event_base_poll_t* base, void* ptr)
{
  excom_event_t event, *eptr;
  int n, e;
  unsigned int i;
  event.base = base;

  while(base->loop)
  {
    n = poll(base->fds, base->nfds, base->timeout);
    e = 0;

    if(n < 0 && errno == EINTR)
    {
      base->loop = false;
    }

    for(i = 0; e < n && i < base->nfds; i++)
    {
      if(base->fds[i].revents != 0)
      {
        eptr = (excom_event_t*) base->events[i];
        event.root  = eptr;
        event.fd    = eptr->fd;
        event.data  = eptr->data;
        event.flags = base->fds[i].revents;
        (*base->runner)(event, ptr);
        base->fds[i].revents = 0;
        e++;
      }
    }
  }
}

void excom_event_poll_loop_end(excom_event_base_poll_t* base)
{
  base->loop = false;
}
