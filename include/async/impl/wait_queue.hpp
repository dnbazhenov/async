/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/coro_impl.hpp>
#include <async/impl/handler_base.hpp>
#include <async/impl/intrusive_list.hpp>

namespace async::impl::wait_queue
{

struct waiter : handler_base, list::list_hook
{};

struct head
{
    void wait();
    bool wake(bool wake_one = false);

  private:
    list::list<waiter> _waiters;
};

} // namespace async::impl::wait_queue
