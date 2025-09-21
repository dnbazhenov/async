/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/impl/wait_queue.hpp>

namespace async::impl::wait_queue
{

void head::wait()
{
    waiter w;

    _waiters.push_back(w);

    w.suspend();
}

bool head::wake(bool wake_one)
{
    if (_waiters.empty())
    {
        return false;
    }

    do
    {
        auto& w = _waiters.front();

        _waiters.pop_front();

        w.resume();
    } while (!_waiters.empty() && !wake_one);

    return true;
}

} // namespace async::impl::wait_queue