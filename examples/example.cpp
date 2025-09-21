/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/coro.hpp>
#include <async/scheduler.hpp>

#include <iostream>

namespace aio = async;

static int generator_coro(aio::coro<int> c)
{
    int i;

    for (i = 0; i < 200000; i++)
    {
        if (c.canceled())
        {
            break;
        }

        c.yield(i);
    }

    return i;
}

static void main_coro(aio::coro<>)
{
    printf("main_coro\n");

    auto c = aio::coro<int>::start("gen1", generator_coro);
    c.set_cancel_throws();

    while (c.running())
    {
        int r = c.await();
        printf("gen1 returned %d\n", r);
    }

    c = aio::coro<int>::start("gen2", generator_coro);

    c.set_cancel_throws();

    int r = 0;

    while (c.running())
    {

        try
        {
            if (r)
            {
                c.cancel();
            }

            r = c.await();

            printf("gen2 returned %d\n", r);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            break;
        }
    }

    aio::scheduler::stop();
}

int main()
{
    aio::scheduler::setup_signal_handlers();

    aio::coro<>::start("main", main_coro);

    aio::scheduler::run();

    return 0;
}
