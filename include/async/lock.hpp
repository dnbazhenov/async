/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/signaled.hpp>

namespace async
{

struct sema : private impl::signaled
{
    sema() : impl::signaled(true)
    {}

    void lock()
    {
        if (is_signaled())
        {
            clear();
            return;
        }

        signaled::wait();
    }

    void unlock()
    {
        signaled::signal(true, true);
    }
};

} // namespace async
