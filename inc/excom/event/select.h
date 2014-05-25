#ifndef _EXCOM_EVENT_SELECT_H
#define _EXCOM_EVENT_SELECT_H

#ifndef EXCOM_WIN
#include <sys/types.h>
#endif

typedef struct excom_event_base_select {
  excom_mutex_t mutex;
  size_t nfds;
  size_t max;
  uint32_t timeout;
  excom_event_t** events;
  excom_event_runner_t* runner;
  bool loop;
} excom_event_base_select_t;

#define excom_event_base excom_event_base_select
#define excom_event_base_t excom_event_base_select_t

#define EXCOM_SELECT_INITIAL_SIZE 16
#define EXCOM_EVENT_READ  (1<<1)
#define EXCOM_EVENT_WRITE (1<<2)
#define EXCOM_EVENT_ERROR (1<<3)
#define EXCOM_EVENT_CLOSE (1<<4)

int excom_event_base_select_init(excom_event_base_t* base,
  excom_event_runner_t* runner);
int excom_event_base_select_destroy(excom_event_base_t* base);
int excom_event_select_add(excom_event_base_t* base,
  struct excom_event* event);
int excom_event_select_update(excom_event_base_t* base,
  struct excom_event* event);
int excom_event_select_remove(excom_event_base_t* base,
  struct excom_event* event);
void excom_event_select_loop(excom_event_base_t* base, void* ptr);
void excom_event_select_loop_end(excom_event_base_t* base);

#define excom_event_base_init excom_event_base_select_init
#define excom_event_base_destroy excom_event_base_select_destroy
#define excom_event_add excom_event_select_add
#define excom_event_update excom_event_select_update
#define excom_event_remove excom_event_select_remove
#define excom_event_loop excom_event_select_loop
#define excom_event_loop_end excom_event_select_loop_end

#endif
