#ifndef _EXCOM_EVENT_POLL_H
#define _EXCOM_EVENT_POLL_H

#include <poll.h>

typedef struct excom_event_base {
  excom_mutex_t mutex;
  struct pollfd* fds;
  struct excom_event** events;
  nfds_t nfds;
  nfds_t max;
  excom_event_runner_t* runner;
  bool loop;
  int timeout;
} excom_event_base_t;

#define EXCOM_POLL_INITIAL_SIZE 16
#define EXCOM_EVENT_READ POLLIN
#define EXCOM_EVENT_WRITE POLLOUT
#define EXCOM_EVENT_ERROR (POLLERR|POLLNVAL)
#define EXCOM_EVENT_CLOSE (POLLHUP)

#endif
