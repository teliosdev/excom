#ifndef _EXCOM_EVENT_H
#define _EXCOM_EVENT_H

/*!
 * @file "excom/event.h"
 * Nothing here yet.
 */

typedef struct excom_event
{
  struct excom_event_base* base;
  int fd;
  uint32_t flags;
  void* data;
  void* _bdata;
  struct excom_event* root;
} excom_event_t;

typedef void (excom_event_runner_t)(excom_event_t, void*);

/*
* This is the order in which we will handle choosing event solutions.
* The solutions that are on the same level recieve the same priority.
*
*   kqueue epoll /dev/poll
*   poll
*   select
*
* Assuming all of the above are implemented.  The following makes
* sure that only one implementation is used.  Otherwise, it will
* get messy :(.
*/

#undef EXCOM_USE_EPOLL
#undef EXCOM_USE_KQUEUE
#undef EXCOM_USE_DEV_POLL
#undef EXCOM_USE_POLL
#undef EXCOM_USE_SELECT

#if defined(EXCOM_EPOLL) || defined(EXCOM_HAVE_SYS_EPOLL_H)
#  define EXCOM_USE_EPOLL
#  include "excom/event/epoll.h"
#elif defined(EXCOM_KQUEUE) || defined(EXCOM_HAVE_SYS_EVENT_H)
#  define EXCOM_USE_KQUEUE
#  include "excom/event/kqueue.h"
#elif defined(EXCOM_DEV_POLL)
#  define EXCOM_USE_DEV_POLL
#  include "excom/event/dev_poll.h"
#elif defined(EXCOM_POLL)
#  define EXCOM_USE_POLL
#  include "excom/event/poll.h"
#else
#  define EXCOM_USE_SELECT
#  include "excom/event/select.h"
#endif

#define EXCOM_EVENT_ALL (EXCOM_EVENT_READ|EXCOM_EVENT_WRITE \
  |EXCOM_EVENT_ERROR|EXCOM_EVENT_CLOSE)

int excom_event_base_init(struct excom_event_base* base,
  excom_event_runner_t* runner);

int excom_event_add(struct excom_event_base* base,
  struct excom_event* event);

int excom_event_remove(excom_event_base_t* base,
  excom_event_t* event);

void excom_event_loop(struct excom_event_base* base, void* ptr);

void excom_event_loop_end(struct excom_event_base* base);

#define excom_event_client(event) \
  ((excom_server_client_t*) (event)->data)

#endif
