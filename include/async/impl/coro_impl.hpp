/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <boost/intrusive/list.hpp>

#include <functional>
#include <memory>
#include <string>

namespace async::impl
{

struct coro_context;
struct coro_base;

using coro_ptr = std::shared_ptr<coro_base>;

namespace details
{
using namespace boost::intrusive;
// auto-unlink from the list in destructor
using list_hook = list_base_hook<link_mode<auto_unlink>>;
} // namespace details

struct coro_base : details::list_hook
{
    friend struct coro_context;
    friend struct handler_base;

    coro_base(std::string&& name);
    ~coro_base()
    {
        // printf("%s: destroy coro\n", _name.data());
    }

    const std::string& name()
    {
        return _name;
    }

    bool running() const
    {
        return _state != state::done;
    }

    bool canceled() const
    {
        return _canceled;
    }

    bool cancel_throws() const
    {
        return _cancel_throws;
    }

    bool has_exception() const
    {
        return !!_exception;
    }

    std::exception_ptr get_exception() const
    {
        return _exception;
    }

    void set_cancel_throws(bool enable)
    {
        _cancel_throws = enable;
    }

    void set_exception(std::exception_ptr ex)
    {
        _exception = ex;
    }

    void check_result();

    void cancel();

    std::shared_ptr<void> get_data(const std::type_info& info);
    void set_data(const std::type_info& info, std::shared_ptr<void>&&);

  public:
    static coro_ptr current_coro();

    static void start(const coro_ptr&, std::move_only_function<void()>&&);
    static void await(coro_ptr);
    static void yield(coro_ptr);
    static void reschedule(coro_ptr);

  private:
    enum class state
    {
        suspended,
        pending,
        running,
        ready,
        done,
    };

    void set_state(state s)
    {
        _state = s;
    }

    void attach(coro_context* ctx)
    {
        _ctx = ctx;
    }

    void set_waiter(const coro_ptr& w);
    void wake();

  protected:
    static void check_coro(bool);
    static void suspend(const coro_ptr&);
    static void resume(const coro_ptr&);
    static void finish(coro_ptr);

  private:
    std::string _name;
    coro_context* _ctx = nullptr;
    std::exception_ptr _exception = nullptr;
    coro_ptr _waiter;
    state _state = state::suspended;
    bool _canceled = false;
    bool _cancel_throws = false;
    std::shared_ptr<void> _data;
    const std::type_info* _data_info = nullptr;
};

template <typename R>
struct coro_impl : coro_base
{
    coro_impl(std::string&& name) : coro_base(std::move(name))
    {}

    void set_result(R&& value)
    {
        _r = std::forward<R>(value);
    }

    R get_result()
    {
        check_result();
        return std::move(_r);
    }

  private:
    R _r;
};

template <>
struct coro_impl<void> : coro_base
{
    using coro_base::coro_base;
    void get_result()
    {
        check_result();
    }
};

} // namespace async::impl
