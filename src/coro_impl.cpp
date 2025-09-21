/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/exceptions.hpp>
#include <async/impl/asio_fwd.hpp>
#include <async/impl/coro_context.hpp>
#include <async/impl/coro_impl.hpp>
#include <boost/asio/post.hpp>

namespace async::impl
{

struct coro_service : public boost::asio::detail::execution_context_service_base<coro_service>
{
    coro_service(impl::io_context& io) :
        boost::asio::detail::execution_context_service_base<coro_service>(io)
    {}

    void shutdown() override
    {
        while (!_list.empty())
        {
            auto& coro = _list.front();
            _list.pop_front();
            coro.set_cancel_throws(true);
            coro.cancel();
        }
    }

    boost::intrusive::list<coro_base, boost::intrusive::constant_time_size<false>> _list;
};

coro_base::coro_base(std::string&& name) : _name(std::move(name))
{
    auto& service = boost::asio::use_service<coro_service>(impl::io::get_context());
    service._list.push_back(*this);
}

void coro_base::check_result()
{
    if (_exception)
    {
        std::rethrow_exception(_exception);
    }
    if (_cancel_throws && _canceled)
    {
        throw exception::canceled();
    }
}

void coro_base::cancel()
{
    _canceled = true;

    if (_cancel_throws && _ctx)
    {
        _ctx->cancel();
    }
}

std::shared_ptr<void> coro_base::get_data(const std::type_info& info)
{
    if (&info == _data_info || (_data_info && *_data_info == info))
    {
        return _data;
    }

    return nullptr;
}

void coro_base::set_data(const std::type_info& info, std::shared_ptr<void>&& data)
{
    _data_info = &info;
    _data = std::move(data);
}

coro_ptr coro_base::current_coro()
{
    auto context = coro_context::current();
    if (!context)
    {
        throw exception::no_coroutine();
    }

    return context->get_impl();
}

void coro_base::start(const coro_ptr& impl, std::move_only_function<void()>&& func)
{
    auto& ctx = impl::coro_context::create(std::move(func));

    ctx.attach(impl);

    impl->attach(&ctx);
    resume(impl);
}

void coro_base::await(coro_ptr impl)
{
    auto curr = coro_context::current();
    auto ctx = impl->_ctx;

    check_coro(ctx != curr);

    if (impl->_state == state::done)
    {
        return;
    }

    if (impl->_state != state::ready)
    {
        auto curr_impl = curr->get_impl();

        curr_impl->set_state(state::suspended);

        impl->set_waiter(curr_impl);

        resume(impl);

        curr->suspend();
    }

    if (impl->_state == state::ready && impl->_ctx)
    {
        impl->set_state(state::suspended);
    }
}

void coro_base::yield(coro_ptr impl)
{
    auto curr = coro_context::current();
    auto ctx = impl->_ctx;

    check_coro(ctx && ctx == curr);
    impl->wake();
    impl->set_state(state::ready);
    curr->suspend();
}

void coro_base::reschedule(coro_ptr impl)
{
    auto curr = coro_context::current();
    auto ctx = impl->_ctx;

    check_coro(ctx && ctx == curr);

    // set state to suspended to allow resuming
    impl->set_state(state::suspended);

    // reschedule itself
    resume(impl);

    // now suspend
    curr->suspend();
}

void coro_base::suspend(const coro_ptr& impl)
{
    auto curr = coro_context::current();
    auto ctx = impl->_ctx;

    check_coro(ctx && ctx == curr);

    impl->set_state(state::suspended);
    curr->suspend();
}

void coro_base::resume(const coro_ptr& impl)
{
    if (impl->_state == state::suspended)
    {
        // avoid multiple resuming
        impl->set_state(state::pending);
        boost::asio::post(impl::io::get_executor(), [=] {
            // check if coro still pends execution
            if (impl->_state == state::pending)
            {
                impl->set_state(state::running);
                impl->_ctx->resume();
            }
            if (impl->_state == state::done && impl->_ctx)
            {
                impl->_ctx->destroy();
                impl->attach(nullptr);
            }
        });
    }
}

void coro_base::check_coro(bool check)
{
    if (!check)
    {
        throw exception::no_coroutine();
    }
}

void coro_base::set_waiter(const coro_ptr& w)
{
    _waiter = w;
}

void coro_base::wake()
{
    if (_waiter)
    {
        resume(_waiter);
        _waiter = nullptr;
    }
}

void coro_base::finish(coro_ptr impl)
{
    impl->wake();
    impl->set_state(state::done);
}

} // namespace async::impl
