/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/coro.hpp>
#include <async/scheduler.hpp>
#include <async/this_coro.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <chrono>
#include <iostream>

namespace aio = async;

namespace tcp
{
using v4address = boost::asio::ip::address_v4;
using proto = boost::asio::ip::tcp;
using socket = boost::asio::basic_stream_socket<proto, aio::impl::io_executor>;
using endpoint = proto::endpoint;

} // namespace tcp

static void main_coro(async::coro<>)
{
    printf("main_coro\n");

    tcp::socket sock(aio::impl::get_executor());
    tcp::endpoint peer(tcp::v4address({127, 0, 0, 1}), 22);

    auto ec = sock.async_connect(peer, aio::this_coro);

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
