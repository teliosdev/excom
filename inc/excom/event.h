#ifndef _EXCOM_EVENT_H
#define _EXCOM_EVENT_H

/*!
 * @file "excom/event.h"
 * Nothing here yet.
 *
 * Level-Triggered?  Level-Triggered.
 */

typedef struct excom_event {
  struct excom_event_base* base;
  int fd;
  uint32_t flags;
  void* data;
  struct excom_event* root;
} excom_event_t;

typedef struct excom_event_buffer {
  size_t max;
  // The number of characters in the buffer.
  size_t data;
  size_t pos;

  char* start;
  char* cur;
  bool freeable;
  excom_mutex_t mutex;
} excom_event_buffer_t;

typedef void (excom_event_runner_t)(struct excom_event, void* ptr);

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

#if defined(EXCOM_EPOLL)
#  define EXCOM_USE_EPOLL
#  include "excom/event/epoll.h"
#elif defined(EXCOM_KQUEUE)
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

int excom_event_base_init(struct excom_event_base* base,
  excom_event_runner_t* runner);

int excom_event_add(struct excom_event_base* base,
  struct excom_event* event);

void excom_event_loop(struct excom_event_base* base, void* ptr);

void excom_event_loop_end(struct excom_event_base* base);

int excom_event_buffer_init(excom_event_buffer_t* buf,
  size_t max);

int excom_event_buffer_init2(excom_event_buffer_t* buf,
  excom_string_t* str);

int excom_event_buffer_append(excom_event_buffer_t* buf,
  size_t appends, ...);

int excom_event_buffer_write(excom_event_buffer_t* buf,
  int sock);

void excom_event_buffer_destroy(excom_event_buffer_t* buf);

#endif
