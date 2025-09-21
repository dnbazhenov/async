/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/impl/asio_fwd.hpp>
#include <async/scheduler.hpp>
#include <boost/asio/signal_set.hpp>

namespace async
{

namespace impl
{

static io_context async_io_context;
static boost::asio::signal_set ss(async_io_context, SIGINT, SIGHUP);

io_context& io::get_context() noexcept
{
    return async_io_context;
}

io_executor io::get_executor() noexcept
{
    return async_io_context.get_executor();
}

} // namespace impl

void scheduler::setup_signal_handlers()
{
    impl::ss.async_wait([](auto, auto) { impl::io::get_context().stop(); });
}

void scheduler::run()
{
    impl::io::get_context().run();
}

void scheduler::stop(bool)
{
    impl::io::get_context().stop();
}

} // namespace async
