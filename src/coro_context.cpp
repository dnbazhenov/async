/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/coro.hpp>
#include <async/impl/asio_fwd.hpp>
#include <async/impl/coro_context.hpp>
#include <async/impl/coro_impl.hpp>

namespace async::impl
{

using fiber = boost::context::fiber;

struct coro_run_scope
{
    using coro_context_ptr = coro_context*;

    coro_run_scope(coro_context::coro_func&& func, coro_context_ptr& context_ptr) :
        _func(std::move(func)), _context_ptr(context_ptr)
    {}

    fiber operator()(fiber&& caller)
    {
        // create coroutine context and save pointer to the caller fiber
        coro_context context(std::move(caller));

        auto func = std::move(_func);

        // store pointer to the created context
        _context_ptr = &context;

        // return to coro_context::create
        context.suspend();

        try
        {
            func();
        }
        catch (const boost::context::detail::forced_unwind&)
        {
            context.do_finish();
            throw;
        }
        catch (const std::exception& e)
        {
            context.set_exception(std::current_exception());
        }

        return context.finish();
    }

  private:
    coro_context::coro_func _func;
    coro_context_ptr& _context_ptr;
};

void coro_context::resume()
{
    _parent_ctx = s_current;
    s_current = this;

    _coro = fiber(std::move(_coro)).resume();
}

void coro_context::suspend()
{
    s_current = _parent_ctx;
    _caller = fiber(std::move(_caller)).resume();
}

coro_context& coro_context::create(coro_func&& func)
{
    coro_context* context;

    fiber callee(coro_run_scope(std::move(func), context));

    context->_coro = std::move(callee).resume();

    return *context;
}

void coro_context::do_finish()
{
    const auto& impl = get_impl();
    coro_base::finish(impl);
    s_current = _parent_ctx;
}

fiber coro_context::finish()
{
    do_finish();
    return std::move(_caller).resume();
}

void coro_context::destroy()
{
    auto coro = std::move(_coro);
    std::move(coro).resume();
}

void coro_context::cancel()
{
    if (this == s_current)
    {
        throw boost::context::detail::forced_unwind();
    }
    else
    {
        auto canceled = std::move(_coro);
    }
}

} // namespace async::impl
