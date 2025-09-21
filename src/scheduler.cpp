/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/impl/io.hpp>
#include <async/scheduler.hpp>
#include <boost/asio/signal_set.hpp>

namespace async
{

namespace impl
{

static io_context io;
static boost::asio::signal_set ss(io, SIGINT, SIGHUP);

io_context& get_context()
{
    return io;
}

io_executor get_executor()
{
    return io.get_executor();
}

} // namespace impl

void scheduler::setup_signal_handlers()
{
    impl::ss.async_wait([](auto, auto) { impl::io.stop(); });
}

void scheduler::run()
{
    impl::io.run();
    //    impl::io.restart();
    //    impl::io.poll();
}

void scheduler::stop(bool)
{
    impl::io.stop();
}

} // namespace async
