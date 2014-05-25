#include "excom.h"
#include "excom/event/select.h"

#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>

#define EVENT_SIZE sizeof(struct excom_event*)
#

int excom_event_base_select_init(excom_event_base_t* base,
  excom_event_runner_t* runner)
{
  base->nfds   = 0;
  base->max    = EXCOM_SELECT_INITIAL_SIZE;
  base->events = excom_malloc(EVENT_SIZE * EXCOM_SELECT_INITIAL_SIZE);
  base->runner = runner;
  base->loop   = true;

  excom_check_alloc(base->events);
  return excom_mutex_init(&base->mutex);
}

int excom_event_base_select_destroy(excom_event_base_t* base)
{
  base->nfds = 0;
  base->max  = 0;
  excom_free(base->events);
  base->events = NULL;
  base->runner = NULL;
  base->loop = false;

  return 0;
}

int excom_event_select_add(excom_event_base_t* base,
  struct excom_event* event)
{
  excom_mutex_lock(&base->mutex);
  if(base->nfds + 1 > base->max)
  {
    base->max *= 2;
    base->events = excom_realloc(base->events,
                                 EVENT_SIZE * base->max);
    excom_check_alloc(base->events);
  }

  base->events[base->nfds] = event;
  event->_bdata = base->nfds;
  printf("ADDING %p: %zu\n", event, base->nfds);
  base->nfds += 1;
  excom_mutex_unlock(&base->mutex);
  return 0;
}

int excom_event_select_update(excom_event_base_t* base,
  struct excom_event* event)
{
  (void) base;
  (void) event;
  return 0;
}

int excom_event_select_remove(excom_event_base_t* base,
  struct excom_event* event)
{
  uintptr_t index;
  index = event->_bdata;

  excom_mutex_lock(&base->mutex);
  if(base->events[index]->fd != event->fd)
  {
    return EINVAL;
  }

  base->events[index] = NULL;

  if(index + 1 < base->nfds)
  {
    memmove(base->events + (index + 1), base->events + index,
      EVENT_SIZE * (base->nfds - index));
  }

  base->nfds -= 1;

  if(base->nfds < (base->max / 2))
  {
    base->max /= 2;
    base->events = excom_realloc(base->events, EVENT_SIZE * base->max);
    excom_check_alloc(base->events);
  }
  excom_mutex_unlock(&base->mutex);

  return 0;
}

// NOTE: THIS IS NOT WORKING ATM FOR SOME REASON.
// DON'T ASK ME.
void excom_event_select_loop(excom_event_base_t* base, void* ptr)
{
  fd_set readfds;
  fd_set writefds;
  fd_set errorfds;
  int diff, rnum, ernum, wnum, lnum;
  unsigned int i, e, in;
  excom_event_t* ev, event;
  struct timeval timeout;
  event.base = base;

  while(base->loop)
  {
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&errorfds);

    in = 0;

    timeout.tv_sec  = 10;
    timeout.tv_usec = 0;
    rnum = 0;
    ernum = 0;
    wnum = 0;
    lnum = 0;

    excom_mutex_lock(&base->mutex);
    for(i = 0; i < base->nfds; i++)
    {
      ev = base->events[i];
      printf("%p(%i): %zu\n", ev, ev->fd, i);
      if(ev->flags & EXCOM_EVENT_READ ||
         ev->flags & EXCOM_EVENT_CLOSE)
      {
        printf("   READ\n");
        FD_SET(ev->fd, &readfds);
        rnum += 1;
      }

      if(ev->flags & EXCOM_EVENT_WRITE)
      {
        printf("   WRITE\n");
        FD_SET(ev->fd, &writefds);
        wnum += 1;
      }

      if(ev->flags & EXCOM_EVENT_ERROR)
      {
        printf("   ERROR\n");
        FD_SET(ev->fd, &errorfds);
        ernum += 1;
      }

    }
    excom_mutex_unlock(&base->mutex);

    if(rnum > wnum && rnum > ernum)
    {
      lnum = rnum;
    }
    else if(wnum > ernum)
    {
      lnum = wnum;
    }
    else
    {
      lnum = ernum;
    }

    diff = select(lnum, &readfds, &writefds, &errorfds,
                  &timeout);

    if(diff < 0)
    {
      printf("DIFF < 0\n");
      return;
    }

    printf("DIFF: %i\n", diff);
    excom_mutex_lock(&base->mutex);
    for(i = 0, e = 0; e < (unsigned int) diff && i < base->nfds; i++)
    {
      ev = base->events[i];
      event.flags = 0;

      if(FD_ISSET(ev->fd, &readfds))
      {
        event.flags |= EXCOM_EVENT_READ;
        in = 1;
        e += 1;
      }
      if(FD_ISSET(ev->fd, &writefds))
      {
        event.flags |= EXCOM_EVENT_WRITE;
        in = 1;
        e += 1;
      }
      if(FD_ISSET(ev->fd, &errorfds))
      {
        event.flags |= EXCOM_EVENT_ERROR;
        in = 1;
        e += 1;
      }

      if(in)
      {
        event.root = ev;
        event.fd   = ev->fd;
        event.data = ev->data;
        excom_mutex_unlock(&base->mutex);
        (*base->runner)(event, ptr);
        excom_mutex_lock(&base->mutex);
      }
    }
    excom_mutex_unlock(&base->mutex);
  }
}

void excom_event_select_loop_end(excom_event_base_t* base)
{
  base->loop = false;
}
