/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/coro.hpp>

namespace async
{

struct current
{
    template <typename R>
    static coro<R> coro()
    {
        return std::dynamic_pointer_cast<impl::coro_impl<R>>(coro_ptr());
    }

    template <typename D>
        requires std::same_as<D, std::decay_t<D>>
    static std::shared_ptr<D> data()
    {
        return std::reinterpret_pointer_cast<D>(coro_data(typeid(D)));
    }

    template <typename D>
        requires std::same_as<D, std::decay_t<D>>
    static void data(const std::shared_ptr<D>& p)
    {
        coro_data(typeid(D), std::reinterpret_pointer_cast<void>(p));
    }

  private:
    static async::impl::coro_ptr coro_ptr()
    {
        return impl::coro_base::current_coro();
    }

    static std::shared_ptr<void> coro_data(const std::type_info& info)
    {
        return coro_ptr()->get_data(info);
    }
    static void coro_data(const std::type_info& info, std::shared_ptr<void>&& p)
    {
        coro_ptr()->set_data(info, std::move(p));
    }
};

} // namespace async
