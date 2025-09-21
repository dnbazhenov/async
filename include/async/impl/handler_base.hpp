/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/coro_impl.hpp>

namespace async::impl
{
struct handler_base
{
    constexpr handler_base() : _coro(coro_base::current_coro())
    {}
    constexpr handler_base(coro_ptr coro) : _coro(std::move(coro))
    {}

    constexpr bool canceled() const
    {
        return _coro->canceled();
    }

    auto get_coro() const
    {
        return _coro;
    }

    void resume() const
    {
        coro_base::resume(_coro);
    }

    void suspend() const
    {
        coro_base::suspend(_coro);
    }

  private:
    coro_ptr _coro;
};

} // namespace async::impl
