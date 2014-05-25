#ifndef _EXCOM_EVENT_EPOLL_H
#define _EXCOM_EVENT_EPOLL_H

#include <sys/epoll.h>

typedef struct excom_event_base_epoll
{
  bool loop;
  int epollfd;
  int timeout;
  int maxevents;
  excom_event_runner_t* runner;
} excom_event_base_epoll_t;

#define excom_event_base excom_event_base_epoll
#define excom_event_base_t excom_event_base_epoll_t

#define EXCOM_EVENT_READ EPOLLIN
#define EXCOM_EVENT_WRITE EPOLLOUT
#define EXCOM_EVENT_ERROR EPOLLERR
#define EXCOM_EVENT_CLOSE (EPOLLRDHUP|EPOLLHUP)

int excom_event_base_epoll_init(excom_event_base_t* base,
  excom_event_runner_t* runner);
int excom_event_base_epoll_destroy(excom_event_base_t* base);
int excom_event_epoll_add(excom_event_base_t* base,
  struct excom_event* event);
int excom_event_epoll_update(excom_event_base_t* base,
  struct excom_event* event);
int excom_event_epoll_remove(excom_event_base_t* base,
  struct excom_event* event);
void excom_event_epoll_loop(excom_event_base_t* base, void* ptr);
void excom_event_epoll_loop_end(excom_event_base_t* base);

#define excom_event_base_init excom_event_base_epoll_init
#define excom_event_base_destroy excom_event_base_epoll_destroy
#define excom_event_add excom_event_epoll_add
#define excom_event_update excom_event_epoll_update
#define excom_event_remove excom_event_epoll_remove
#define excom_event_loop excom_event_epoll_loop
#define excom_event_loop_end excom_event_epoll_loop_end

#endif
