#ifndef _EXCOM_EVENT_H
#define _EXCOM_EVENT_H

/*!
 * @file "excom/event.h"
 * Manages I/O events for excom.  This implements multiple backends.
 *
 * This is the order in which we will handle choosing event solutions.
 * The solutions that are on the same level recieve the same priority.
 *
 *     kqueue epoll
 *     /dev/poll
 *     poll
 *     select
 *
 * Assuming all of the above are implemented.  Excom makes
 * sure that only one implementation is used.  Otherwise, it will
 * get messy :(.
 */

#undef EXCOM_USE_EPOLL
#undef EXCOM_USE_KQUEUE
#undef EXCOM_USE_DEV_POLL
#undef EXCOM_USE_POLL
#undef EXCOM_USE_SELECT

#ifdef EXCOM_HAVE_SYS_EPOLL_H
#  define EXCOM_EPOLL
#endif

#ifdef EXCOM_HAS_SYS_EVENT_H
#  define EXCOM_KQUEUE
#endif

/*!
 * An event to keep track of, or an event that occurred.  If it's the
 * former, the `fd`, `flags`, and `data` fields need to be filled.
 * If it's the latter, the `base`, `fd`, `flags`, `data`, and `root`
 * fields will be filled.
 */
typedef struct excom_event
{
  /*!
   * A reference to the base that this was registered to.  An event
   * may only be registered to one base.
   */
  void* base;

  /*!
   * The file descriptor of the I/O device to listen for events to.
   */
  int fd;

  /*!
   * The flags of the event.
   *
   * @see  EXCOM_EVENT_READ
   * @see  EXCOM_EVENT_WRITE
   * @see  EXCOM_EVENT_ERROR
   * @see  EXCOM_EVENT_CLOSE
   * @see  EXCOM_EVENT_ALL
   */
  uint32_t flags;

  /*!
   * The data of the event.  This can be anything you want.  The same
   * field on the event that occured will have the same value as the
   * registered event.
   */
  void* data;

  /*!
   * Data that the backend uses.  It is not recommended to use this
   * field for anything.
   */
  uintptr_t _bdata;

  /*!
   * If this is a triggered event, this is a reference to the original
   * event that was registered.
   */
  struct excom_event* root;
} excom_event_t;

/*!
 * A function that handles any event that pops up.  The first argument
 * is a triggered event, and the second is the argument passed to
 * @ref excom_event_loop.
 */
typedef void (excom_event_runner_t)(excom_event_t, void*);

#include "excom/event/undef.h"

#if defined(DOXYGEN)
#elif defined(EXCOM_EPOLL)
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

/*
 * Please note anything within this section will not and should not
 * be compiled with the primary code base; _IT WILL NOT COMPILE_.
 */
#ifdef DOXYGEN

#error Using Doxygen header for code base!

/*!
 * Either a read event occured, or a read event should be looked for.
 */
#define EXCOM_EVENT_READ  (1<<0)

/*!
 * Either a write event occured, or a write event should be looked
 * for.
 */
#define EXCOM_EVENT_WRITE (1<<1)

/*!
 * Either an error event occured, or an error event should be looked
 * for.
 */
#define EXCOM_EVENT_ERROR (1<<2)

/*!
 * Either a close event occured, or a close event should be looked
 * for.
 */
#define EXCOM_EVENT_CLOSE (1<<4)

/*!
 * Contains the state of the event backend.  Most backends will,
 * however, have this be a struct, with at least one pointer - the
 * @ref excom_event_runner_t field.
 */
typedef void excom_event_base_t;

/*!
 * Initialize an event base with a given runner.
 *
 * @param [inout] base The event base to initialize.
 * @param [in] runner The event runner.
 * @return 0 on success, an error code otherwise.
 */
#define excom_event_base_init(base, runner)

/*!
 * Adds an event to the event backend.  The given event should be
 * filled out properly, as described in the excom_event_t type.
 * Calling this multiple times with different event arguments with
 * the same fd results in undefined behavior.
 *
 * @param [inout] base The event base backend to add the event to.
 * @param [in] event The event to add.  Note that adding the event
 *   may cause a reference to the event to be made, so freeing it
 *   after this function call may result in undefined behavior.
 * @return 0 on success, an error code otherwise.
 */
#define excom_event_add(base, event)

/*!
 * Removes an event from the event backend.  The given event
 * should be the same as passed to @ref excom_event_add,
 * but it need
 * not be; it only requires the same `fd` (for now).
 *
 * @param [inout] base The event base backend to remove the event
 *   from.
 * @param [in] event The event to remove.
 * @return 0 on success, an error code otherwise.
 */
#define excom_event_remove(base, event)

/*!
 * Begin the event loop, looking for events.  The argument passed
 * will be the last argument passed to the runner.  Events may
 * be added and removed during the loop.
 *
 * @param [in] base The event base to begin the loop on.
 * @param [in] ptr The pointer to pass to the event runner.
 */
#define excom_event_loop(base, ptr)

/*!
 * Stop the loop.  Can be called from anywhere.
 *
 * @param [inout] base The event base whose loop needs to stop.
 */
#define excom_event_loop_end(base)


#endif

/*!
 * All events should be looked for.  This _WILL NOT_ be in a triggered
 * event.
 */
#define EXCOM_EVENT_ALL (EXCOM_EVENT_READ|EXCOM_EVENT_WRITE \
  |EXCOM_EVENT_ERROR|EXCOM_EVENT_CLOSE)

/*!
 * Retrieves the event data, and casts it to an excom_server_client_t.
 */
#define excom_event_client(event) \
  ((excom_server_client_t*) (event)->data)

#endif
