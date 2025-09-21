/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once
#include <async/error_code.hpp>
#include <async/function_ref.hpp>
#include <async/impl/asio_fwd.hpp>
#include <async/pending_op.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace async
{

namespace ip
{

using address_v4 = boost::asio::ip::address_v4;
using tcp = boost::asio::ip::tcp;

} // namespace ip

namespace tcp
{

using proto = boost::asio::ip::tcp;
using endpoint = boost::asio::ip::tcp::endpoint;
using address_v4 = boost::asio::ip::address_v4;

struct socket_base
{
    static constexpr auto max_listen_connections = boost::asio::socket_base::max_listen_connections;
    using wait_type = boost::asio::socket_base::wait_type;
};

template <typename T>
concept void_or_bool = std::same_as<T, void> || std::same_as<T, bool>;

struct socket
{
    using socket_type = boost::asio::basic_stream_socket<proto, impl::io_executor>;

    using const_buffer = boost::asio::const_buffer;
    using mutable_buffer = boost::asio::mutable_buffer;
    using executor_type = impl::io_executor;

    socket();
    socket(socket&&);
    socket(const proto&);
    socket(const endpoint&);
    socket(const proto&, const int&);
    ~socket();

    socket& operator=(socket&&);
    socket& operator=(socket_type&&);

    socket_type& boost_socket()
    {
        return _socket;
    }

    socket& lowest_layer()
    {
        return *this;
    }

    const socket& lowest_layer() const
    {
        return *this;
    }

    error_code open(const proto&);
    error_code assign(const proto&, const int&);
    error_code close();
    error_code release(int&);
    error_code cancel();
    error_code at_mark(bool&);
    error_code available(std::size_t) const;
    error_code bind(const endpoint&) const;
    pending_op async_wait(socket_base::wait_type, function_ref<bool(error_code)>);
    error_code async_wait(socket_base::wait_type);
    template <typename Function>
    pending_op async_connect(const endpoint& e, Function&& f)
    {
        function_ref fr{f};
        return async_connect(e, std::move(fr));
    }
    error_code async_connect(const endpoint&);
    pending_op async_send(const_buffer, function_ref<bool(error_code, std::size_t)>);
    error_code async_send(const_buffer, std::size_t&);
    template <typename Function>
    pending_op async_receive(mutable_buffer buffer, Function&& f)
    {
        function_ref fr{f};
        return async_receive(buffer, std::move(fr));
    }
    error_code async_receive(mutable_buffer, std::size_t&);

  protected:
    pending_op async_connect(const endpoint&, function_ref<bool(error_code)>);
    pending_op async_receive(mutable_buffer, function_ref<bool(error_code, std::size_t)>);

  private:
    socket_type _socket;
};

struct acceptor
{
    using executor_type = impl::io_executor;

    acceptor();
    acceptor(acceptor&&) = default;
    acceptor(const proto&);
    acceptor(const endpoint&, bool = false);
    acceptor(const proto&, const int&);

    acceptor& operator=(acceptor&&) = default;

    error_code open(const proto&);
    error_code close();
    error_code assign(const proto&, const int&);
    error_code release(int&);
    error_code bind(const endpoint&);
    error_code listen(int = socket_base::max_listen_connections);
    error_code cancel();
    template <void_or_bool R>
    pending_op async_wait(socket_base::wait_type, function_ref<R(error_code)>);
    error_code async_wait(socket_base::wait_type);
    template <void_or_bool R>
    pending_op async_accept(function_ref<R(error_code, socket&)>);
    error_code async_accept(socket&);
    template <void_or_bool R>
    pending_op async_accept(function_ref<R(error_code, socket&, endpoint&)>);
    error_code async_accept(socket&, endpoint&);

  private:
    boost::asio::basic_socket_acceptor<proto, impl::io_executor> _acceptor;
};

} // namespace tcp

} // namespace async
