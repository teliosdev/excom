l# Todo List

1. More event backends.
   - specifically:
     - ~~`/dev/poll`~~ _`/dev/poll` would not give enough of a benifit to be used.  See event.h for more._
     - [I/O Completion Ports](http://msdn.microsoft.com/en-us/library/aa365198(VS.85).aspx)
2. Add code to client management.
3. Begin client code.
4. Documentation.
   - files needed:
     - `encryption.h`
     - `protocol.h`
     - `client.h`
     - `thread/posix.h`
     - `thread/windows.h`
     - `server/client.h`
     - `protocol/packets.h`
     - `event/epoll.h`
     - `event/kqueue.h`
     - `event/kqueue.h`
     - `event/poll.h`
5. Memory management.
6. Optimisation.
7. Threading destructors.
8. *BSD-friendly Makefile.
   - especially for TOML.
