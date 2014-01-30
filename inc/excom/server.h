#ifndef _EXCOM_SERVER_H
#define _EXCOM_SERVER_H

#include "excom/server/client.h"

/*!
 * @file "excom/server.h"
 * Contains data types and functions related to how the server is set
 * up, as well as getting it running.  Excom servers normally run on
 * port `5656`, and listen on the address `0.0.0.0`.  I don't think
 * excom servers have IPv6 support yet.  Note that the server process
 * happens like so:
 *
 * 1. Initialization (excom_server_init()).
 * 2. Custom values are filled, if any.
 * 3. Binding (excom_server_bind()).
 *    - binding the socket
 *    - listening on the socket
 *    - setting non-blocking
 * 4. Accepting (clients) (excom_server_run()).
 */

/*!
 * The default port that an excom server will listen on.  I tried to
 * pick a port that isn't commonly used.
 */
#define EXCOM_DEFAULT_PORT 5656

/*!
 * The default address that an excom server will listen on.
 * Defaulting to `INADDR_ANY` is a good idea, since most of the time
 * the server is created with the idea that other computers will
 * connect.
 */
#define EXCOM_DEFAULT_ADDR INADDR_ANY

/*!
 * An object to represent the state of a server.  It contains things
 * like the port and address that the server is bound to, and the
 * socket that the server has open, and it will contain the main
 * event struct from `event.h`.
 */
typedef struct excom_server
{
  /*!
   * A fd of the current server.  This is normally `-1` if there is no
   * socket open.
   */
  int sock;

  /*!
   * The port that the server is listening on.  Check
   * `EXCOM_DEFAULT_PORT` for more information.
   */
  uint16_t port;

  /*!
   * The IP address that the server is listening on.  Check
   * `EXCOM_DEFAULT_ADDR` for more information.
   */
  uint32_t addr;

  struct excom_event_base base;

  struct excom_server_client* clients;
} excom_server_t;

/*!
 * Initializes the server to default values.  This should be called
 * before any operations on the server are made, even modifications to
 * the data contained within the struct (mainly since the
 * modifications **will** be overwritten).  This is part 1 of the
 * server process.
 *
 * @param [in,out] server The server to initialize.
 * @returns nothing.
 */
void excom_server_init(excom_server_t* server);

/*!
 * Binds the server to a socket, then to the port, and then listens
 * for clients.  This is part 3 of the server process.  This should be
 * called **after** the values within the server have been modified,
 * since any changes to the values after this call will not affect the
 * server.
 *
 * @param [in] server The server to bind.
 * @returns nothing.
 */
int excom_server_bind(excom_server_t* server);

/*!
 * Listens for clients.  I'm not sure how this'll work, but I'll fix
 * it up later.  This is part 4 of the server process.  It should only
 * be called after it has been bound.
 *
 * @param [in] server The server to accept on.
 * @returns An error code, or 0 if there was none.
 */
int excom_server_run(excom_server_t* server);

void excom_server_destroy(excom_server_t* server);

#endif
