/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/pending_group.hpp>

namespace async
{

void pending_group::op::invoke(bool res) const
{
    _group.on_pending_op(*this, res);
}

void pending_group::on_pending_op(const op&, bool res)
{
    _callback(res);
}

bool pending_group::wait_all()
{
    impl::wait_queue::head wq;
    bool res = true;
    size_t size = _list.size();

    auto cb = [&](bool r) mutable {
        if (r == false && res)
        {
            res = false;
            wq.wake();
        }
        else if (--size == 0)
        {
            wq.wake();
        }
    };

    _callback = &cb;

    wq.wait();

    _list.clear();
    _callback = nullptr;

    return res;
}

} // namespace async