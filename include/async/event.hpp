/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/signaled.hpp>

namespace async
{

struct event : private impl::signaled
{
    using impl::signaled::signaled;

    void wait()
    {
        if (is_signaled())
        {
            return;
        }

        signaled::wait();
    }

    void set(bool wake_one = false)
    {
        signaled::signal(wake_one);
    }
};

} // namespace async
