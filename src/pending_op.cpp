/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/pending_op.hpp>

namespace async
{

bool pending_op::wait()
{
    impl::wait_queue::head wq;
    bool res;

    _wq = &wq;
    _res = &res;

    wq.wait();

    return res;
}

void pending_op::invoke(bool r) const
{
    *_res = r;
    _wq->wake();
}

} // namespace async