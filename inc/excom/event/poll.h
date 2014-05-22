#ifndef _EXCOM_EVENT_POLL_H
#define _EXCOM_EVENT_POLL_H

#include <poll.h>

typedef struct excom_event_base_poll {
  excom_mutex_t mutex;
  struct pollfd* fds;
  struct excom_event** events;
  nfds_t nfds;
  nfds_t max;
  excom_event_runner_t* runner;
  bool loop;
  int timeout;
} excom_event_base_poll_t;

#define excom_event_base excom_event_base_poll
#define excom_event_base_t excom_event_base_poll_t

#define EXCOM_POLL_INITIAL_SIZE 16
#define EXCOM_EVENT_READ POLLIN
#define EXCOM_EVENT_WRITE POLLOUT
#define EXCOM_EVENT_ERROR (POLLERR|POLLNVAL)
#define EXCOM_EVENT_CLOSE (POLLHUP)

int excom_event_base_poll_init(excom_event_base_t* base,
  excom_event_runner_t* runner);
int excom_event_poll_add(excom_event_base_t* base,
  struct excom_event* event);
int excom_event_poll_update(excom_event_base_t* base,
  struct excom_event* event);
int excom_event_poll_remove(excom_event_base_t* base,
  struct excom_event* event);
void excom_event_poll_loop(excom_event_base_t* base, void* ptr);
void excom_event_poll_loop_end(excom_event_base_t* base);

#define excom_event_base_init excom_event_base_poll_init
#define excom_event_add excom_event_poll_add
#define excom_event_update excom_event_poll_update
#define excom_event_remove excom_event_poll_remove
#define excom_event_loop excom_event_poll_loop
#define excom_event_loop_end excom_event_poll_loop_end

#endif
