#ifndef _EXCOM_EVENT_KQEUE_H
#define _EXCOM_EVENT_QUEUE_H

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

typedef struct excom_event_base {
  bool loop;
  int kqueuefd;

  int timeout;
  int maxevents;

  excom_event_runner_t* runner;
} excom_event_base_t;

#define EXCOM_EVENT_READ  (1<<1)
#define EXCOM_EVENT_WRITE (1<<2)
#define EXCOM_EVENT_ERROR (1<<3)
#define EXCOM_EVENT_CLOSE (1<<4)

#endif
