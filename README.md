# asynclib

A boost asio adapter library for single-threaded application
with several stackful coroutines.

Each stackful coroutine can yield execution with result, and
asynchronously wait for yield or completion of another coroutine.

In addition, the library implements this_coro completion token to
be used with the rest boost::asio elements, i.e. sockets, file
descriptors, etc.

For more information on usage see examples.

WORK IN PROGRESS
