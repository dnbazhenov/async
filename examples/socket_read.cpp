/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/coro.hpp>
#include <async/pending_group.hpp>
#include <async/scheduler.hpp>
#include <async/socket.hpp>

namespace aio = async;

static void main_coro(async::coro<>)
{
    printf("main_coro\n");

    aio::tcp::socket sock;
    aio::tcp::endpoint peer(aio::tcp::address_v4({127, 0, 0, 1}), 22);

    char data[1000];
    aio::tcp::socket::mutable_buffer buf(data, sizeof(data));

    aio::error_code ec(1, aio::system_category());
    aio::error_code ec1(1, aio::system_category());
    size_t read = 0;

    auto cb1 = [&](aio::error_code e) -> bool {
        ec = std::move(e);
        return true;
    };

    auto cb2 = [&](aio::error_code e, std::size_t transferred) mutable -> bool {
        ec1 = std::move(e);
        read = transferred;
        return true;
    };

    aio::pending_group pg;

    pg += sock.async_connect(peer, &cb1);

    pg += sock.async_receive(buf, &cb2);

    pg.wait_all();

    printf("ec={%s}\n", ec.message().data());
    printf("ec1={%s/%ld}\n", ec1.message().data(), read);

    sock.close();

    ec = sock.async_connect(peer);

    printf("ec={%s}\n", ec.message().data());

    aio::scheduler::stop();
}

int main()
{
    aio::scheduler::setup_signal_handlers();
    aio::coro<>::start("main", main_coro);
    aio::scheduler::run();

    return 0;
}
