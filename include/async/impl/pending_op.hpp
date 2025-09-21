/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/asio_fwd.hpp>
#include <async/pending_op.hpp>
#include <boost/asio/async_result.hpp>

namespace boost::asio
{

template <typename Function, typename Signature>
struct async_result<async::defer_t<Function>, Signature>
{
    using handler = async::defer_t<Function>;
    using completion_handler_type = async::defer_t<Signature>;
    using return_type = typename handler::return_type;

    template <typename Initiation, typename... InitArgs>
    static auto initiate(Initiation&& init, handler&& token, InitArgs&&... args) -> return_type
    {
        auto r = token.get_sink();
        std::forward<Initiation>(init)(std::forward<handler>(token),
                                       std::forward<InitArgs>(args)...);
        return r;
    }
};

} // namespace boost::asio