#ifndef _EXCOM_EVENT_EPOLL_H
#define _EXCOM_EVENT_EPOLL_H

#include <sys/epoll.h>

typedef struct excom_event_base {
  bool loop;
  int epollfd;
  int timeout;
  int maxevents;
  excom_event_runner_t* runner;
} excom_event_base_t;

#define EXCOM_EVENT_READ EPOLLIN
#define EXCOM_EVENT_WRITE EPOLLOUT
#define EXCOM_EVENT_ERROR EPOLLERR
#define EXCOM_EVENT_CLOSE (EPOLLRDHUP|EPOLLHUP)

#endif
