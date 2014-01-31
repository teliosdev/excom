# excom [![Build Status](https://travis-ci.org/redjazz96/excom.png?branch=master)](https://travis-ci.org/redjazz96/excom)

An alternative to current chat implementations.

## Compiling

Before the project can be compiled, the `configure` script must be
run.  It generates a header file that is used by the project.

The project uses [libtoml](https://github.com/ajwans/libtoml), which
is under the 3 clause BSD license.

This project uses make.  Sorry, I'm comfortable with `make`, and I
know how to use it.  However, building excom without make shouldn't
be that difficult.  Excom requires the c99 standard, excluding
`stdbool.h` (it will use it if it can).  Some defines and how you can
use them:

<table>
  <thead>
    <tr>
      <th>
        Define
      </th>
      <th>
        Possible Values
      </th>
      <th>
        Meaning
      </th>
    </tr>
  </thead>

  <tbody>
    <tr>
      <td><code>EXCOM_NO_STDBOOL</code></td>
      <td><i>none</i></td>
      <td>If this is defined, excom will use its own stdbool implementation.</td>
    </tr>
    <tr>
      <td><code>EXCOM_INCLUDE_SERVER_CLIENT</code></td>
      <td><i>none</i></td>
      <td>If this is defined, excom will include a very basic implementation of a server that can handle reading and writing to clients.  It will not be able to handle anything more than that, though.</td>
    </tr>
    <tr>
      <td><code>EXCOM_EPOLL</code></td>
      <td><i>none</i></td>
      <td>If this is defined, excom will try to use epoll.  epoll <b>must</b> have support for <code>epoll_create1()</code>.</td>
    </tr>
    <tr>
      <td><code>EXCOM_KQUEUE</code></td>
      <td><i>none</i></td>
      <td>If this is defined, excom will try to use kqueue.  If it does not exist on the system, the compiler will error with <q>fatal error: sys/event.h: No such file or directory.</q> <i>Note: the support for kqueue in excom is still a little shakey, since I cannot test it in my current environment.</i></td>
    </tr>
  </tbody>
</table>

After you've defined what you need to, the main body of the code
should be able to be compiled.  The Makefile compiles it into a
static library, but you can compile it into a shared object if you so
wish.

## Documentation

One of the primary goals of this library is to have 100%
documentation - every function, every define, every struct.  This is
to ensure that the code is understandable as to how to use it.  The
code comes with a Doxyfile to allow doxygen to generate a site for the
documentation.

## Usage

Sadly, in the current state, the library can't really be used :(

But we're getting there!

## Contribution

You wanna contribute?  Awesome!

1. Fork it (<http://github.com/redjazz96/excom/fork>)
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
