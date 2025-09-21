/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/coro_impl.hpp>
#include <boost/context/fiber.hpp>

namespace async::impl
{

struct coro_context
{
    using fiber = boost::context::fiber;
    using coro_func = std::move_only_function<void()>;
    using suspend_arg = void*;
    using suspend_func = void (*)(suspend_arg);

    coro_context(fiber&& caller) : _caller(std::move(caller)), _parent_ctx(current())
    {}

    ~coro_context()
    {
        // printf("%s: destroy stack\n", _impl->name().data());
    }

    void cancel();
    void resume();
    void suspend();
    void destroy();
    void do_finish();
    fiber finish();
    void set_exception(std::exception_ptr ex)
    {
        if (_impl)
        {
            _impl->set_exception(ex);
        }
    }

    void attach(const coro_ptr& impl)
    {
        _impl = impl;
    }

    auto get_impl() const
    {
        return _impl;
    }
    static coro_context* current()
    {
        return s_current;
    }

  public:
    static coro_context& create(coro_func&&);

  private:
    // pointer to the caller fiber
    fiber _caller;
    // this coroutine fiber
    fiber _coro;
    // associated coroutine object
    coro_ptr _impl;
    // parent context
    coro_context* _parent_ctx = nullptr;

  private:
    static inline coro_context* s_current = nullptr;
};

} // namespace async::impl
