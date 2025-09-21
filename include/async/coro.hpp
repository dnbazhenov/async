/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/exceptions.hpp>
#include <async/impl/coro_impl.hpp>

#include <expected>

namespace async
{

template <typename R = void>
struct coro;

/**
 * @brief concept requirements for coroutine functions.
 */
template <typename Function, typename R, typename... Args>
concept coro_func_t = std::is_invocable_r_v<R, Function, coro<R>, Args...>;

/**
 * @brief Class for coroutines.
 */
template <typename R>
struct coro
{
    /**
     * @brief Empty coroutine constructor.
     */
    constexpr coro() = default;

    /**
     * @brief requirement for coroutine returning nothing.
     */
    static constexpr auto is_void_result = std::is_same_v<R, void>;

    /**
     * @brief get coroutine name.
     */
    const std::string& name() const
    {
        return get_ptr()->name();
    }

    /**
     * @brief check if coroutine is running.
     */
    bool running() const
    {
        return get_ptr()->running();
    };

    /**
     * @brief check if coroutine is canceled.
     */
    bool canceled() const
    {
        return get_ptr()->canceled();
    }

    /**
     * @brief check if cancel would throw exception.
     */
    bool cancel_throws() const
    {
        return get_ptr()->cancel_throws();
    }

    /**
     * @brief cancel coroutine.
     */
    void cancel() const
    {
        return get_ptr()->cancel();
    }

    /**
     * @brief set cancel_throws flag.
     */
    void set_cancel_throws(bool cancel_throws = true) const
    {
        return get_ptr()->set_cancel_throws(cancel_throws);
    }

    /**
     * @brief asynchronously wait for result.
     */
    R await() const
    {
        auto ptr = get_ptr();

        impl::coro_base::await(ptr);

        if constexpr (is_void_result)
        {
            ptr->get_result();
        }
        else
        {
            return ptr->get_result();
        }
    }

    /**
     * @brief yield execution with the specified result.
     */
    template <typename U = R>
    void yield(U&& r)
        requires(!is_void_result)

    {
        auto ptr = get_ptr();
        ptr->set_result(std::forward<R>(r));
        impl::coro_base::yield(ptr);
    }

    /**
     * @brief yield no result and suspend execution.
     */
    void yield()
        requires is_void_result

    {
        impl::coro_base::yield(get_ptr());
    }

    /**
     * @brief reschedule execution without yielding.
     */
    void reschedule()
    {
        impl::coro_base::reschedule(get_ptr());
    }

    /**
     * @brief start a new coroutine.
     */
    template <typename... Args>
    static coro start(std::string name, coro_func_t<R, Args...> auto&& func, Args&&... args)
        requires(!is_void_result)
    {
        auto c = make_coro(std::move(name));
        impl::coro_base::start(
            c, [c, func = std::move(func), ... args = std::forward<Args>(args)] mutable {
                c->set_result(func(c, std::forward<Args>(args)...));
            });
        return c;
    }

    /**
     * @brief start a new coroutine.
     */
    template <typename... Args>
    static coro start(std::string name, coro_func_t<void, Args...> auto&& func, Args&&... args)
        requires is_void_result
    {
        auto c = make_coro(std::move(name));
        impl::coro_base::start(
            c, [c, func = std::move(func), ... args = std::forward<Args>(args)] mutable {
                func(c, std::forward<Args>(args)...);
            });
        return c;
    }

  private:
    using impl_ptr = std::shared_ptr<impl::coro_impl<R>>;

    /**
     * @brief construct coroutine from implementation.
     */
    constexpr coro(impl_ptr impl) : _impl(std::move(impl))
    {}

    /**
     * @brief check if corotuine implementation is instantiated.
     */
    impl_ptr get_ptr() const
    {
        if (!_impl)
        {
            throw exception::bad_coroutine();
        }
        return _impl;
    }

    /**
     * @brief set return value.
     */
    template <typename U>
    void set_result(U&& r)
        requires(!is_void_result)
    {
        get_ptr()->set_result(std::forward<R>(r));
    }

    /**
     * @brief get return value.
     */
    auto get_result()
    {
        if constexpr (is_void_result)
        {
            get_ptr()->get_result();
        }
        else
        {
            return get_ptr()->get_result();
        }
    }

  private:
    /**
     * @brief allocate and construct coroutine implemenation.
     */
    static impl_ptr make_coro(std::string&& name)
    {
        return std::make_shared<impl::coro_impl<R>>(std::move(name));
    }

  private:
    impl_ptr _impl;
};

} // namespace async
