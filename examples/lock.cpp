/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/coro.hpp>
#include <async/lock.hpp>
#include <async/scheduler.hpp>

#include <iostream>

namespace aio = async;

static aio::sema s_sema;
static aio::sema s_sema2;

static void do_locked(aio::coro<void> c)
{
    printf("%s: start\n", c.name().data());
    s_sema.lock();
    printf("%s: has lock\n", c.name().data());
    s_sema.unlock();
}

static void do_locked2(aio::coro<void> c)
{
    printf("%s: start\n", c.name().data());
    s_sema2.lock();
    printf("%s: has lock\n", c.name().data());
    s_sema2.unlock();
}

static void main_coro(aio::coro<> c)
{
    printf("%s: start\n", c.name().data());
    s_sema.lock();
    s_sema2.lock();

    auto c1 = aio::coro<void>::start("do1", do_locked);
    auto c2 = aio::coro<void>::start("do2", do_locked2);

    printf("%s: keeping locks\n", c.name().data());

    c.reschedule();

    printf("%s: removing locks\n", c.name().data());

    s_sema.unlock();
    s_sema2.unlock();

    printf("%s: wait tasks\n", c.name().data());

    c2.await();
    c1.await();

    aio::scheduler::stop();
}

int main()
{
    aio::scheduler::setup_signal_handlers();

    aio::coro<>::start("main", main_coro);

    aio::scheduler::run();

    return 0;
}
