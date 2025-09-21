/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/asio_fwd.hpp>
#include <async/impl/handler_base.hpp>
#include <async/this_coro.hpp>
#include <boost/asio/async_result.hpp>

namespace boost::asio
{

template <typename... Signatures>
struct wake_return_type;

template <typename... Signatures>
struct wake_return_type<void(Signatures...)>
{
    using return_type = std::tuple<Signatures...>;
};

template <typename Signature>
struct wake_return_type<void(Signature)>
{
    using return_type = Signature;
};

template <typename... Signatures>
using wake_return_type_t = typename wake_return_type<Signatures...>::return_type;

template <typename Executor, typename... CompletionArgs>
struct wake_handler;

template <typename... CompletionArgs>
struct wake_handler<async::impl::io_executor, CompletionArgs...> : async::impl::handler_base
{
    using return_type = wake_return_type_t<CompletionArgs...>;

    constexpr wake_handler(return_type& r) : _r(r)
    {}

    template <typename... Args>
        requires(sizeof...(Args) > 1)
    void operator()(Args&&... args)
    {
        // NOTE: check if coro is canceled
        //       when canceled, _r points to the deallocated memory
        if (canceled())
        {
            return;
        }

        _r = std::forward_as_tuple<Args...>(args...);
        resume();
    }

    template <typename Arg>
    void operator()(Arg&& arg)
    {
        _r = std::forward<Arg>(arg);
        resume();
    }

    return_type& _r;
};

template <typename... Signatures>
struct async_result<async::this_coro_t, Signatures...>
{
    using handler_type = wake_handler<async::impl::io_executor, Signatures...>;
    using return_type = typename handler_type::return_type;

    template <typename Initiation, typename... InitArgs>
    static auto initiate(Initiation&& init, const async::this_coro_t&, InitArgs&&... args)
        -> return_type
    {
        return_type r;
        handler_type h{r};
        auto coro = h.get_coro();
        std::forward<Initiation>(init)(h, std::forward<InitArgs>(args)...);
        h.suspend();
        return r;
    }
};

} // namespace boost::asio
