#ifndef _EXCOM_EVENT_KQEUE_H
#define _EXCOM_EVENT_KQEUE_H

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#if defined(EXCOM64) && defined(EXCOM_MAC)
#  define excom_kevent_t struct kevent64_s
#  define excom_kevent(a,b,c,d,e,f) kevent64(a,b,c,d,e,0,f)
#  define excom_kevent_set(a,b,c,d,e,f,i) EV_SET64(a,b,c,d,e,f,i,0,0)
#else
#  define excom_kevent_t struct kevent
#  define excom_kevent kevent
#  define excom_kevent_set(a,b,c,d,e,f,i) EV_SET(a,b,c,d,e,f,i)
#endif

typedef struct excom_event_base_kqueue
{
  bool loop;
  int kqueuefd;

  int timeout;
  int maxevents;

  excom_event_runner_t* runner;
} excom_event_base_kqueue_t;

#define excom_event_base excom_event_base_kqueue
#define excom_event_base_t excom_event_base_kqueue_t

#define EXCOM_EVENT_READ  (1<<1)
#define EXCOM_EVENT_WRITE (1<<2)
#define EXCOM_EVENT_ERROR (1<<3)
#define EXCOM_EVENT_CLOSE (1<<4)

int excom_event_base_kqueue_init(excom_event_base_t* base,
  excom_event_runner_t* runner);
int excom_event_kqueue_add(excom_event_base_t* base,
  struct excom_event* event);
int excom_event_kqueue_update(excom_event_base_t* base,
  struct excom_event* event);
int excom_event_kqueue_remove(excom_event_base_t* base,
  struct excom_event* event);
void excom_event_kqueue_loop(excom_event_base_t* base, void* ptr);
void excom_event_kqueue_loop_end(excom_event_base_t* base);

#define excom_event_base_init excom_event_base_kqueue_init
#define excom_event_add excom_event_kqueue_add
#define excom_event_update excom_event_kqueue_update
#define excom_event_remove excom_event_kqueue_remove
#define excom_event_loop excom_event_kqueue_loop
#define excom_event_loop_end excom_event_kqueue_loop_end

#endif
