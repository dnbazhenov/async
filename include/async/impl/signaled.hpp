/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/wait_queue.hpp>

namespace async::impl
{

struct signaled : protected wait_queue::head
{
    signaled() = default;
    signaled(bool state) : _state(state)
    {}

    void signal(bool wake_one = false, bool auto_clear = false)
    {
        if (is_signaled())
        {
            return;
        }

        _state = !wake(wake_one) || !auto_clear;
    }

    void clear()
    {
        _state = false;
    }

    bool is_signaled() const
    {
        return _state;
    }

  private:
    bool _state = false;
};

} // namespace async::impl
