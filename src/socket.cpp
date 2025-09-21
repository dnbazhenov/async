/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/impl/pending_op.hpp>
#include <async/impl/this_coro.hpp>
#include <async/socket.hpp>

template class boost::wrapexcept<boost::asio::invalid_service_owner>;

namespace async::tcp
{

socket::socket() : _socket(impl::io::get_executor())
{}

socket::socket(const proto& p) : _socket(impl::io::get_executor(), p)
{}

socket::socket(const endpoint& e) : _socket(impl::io::get_executor(), e)
{}

socket::socket(const proto& p, const int& h) : _socket(impl::io::get_executor(), p, h)
{}

socket::~socket()
{}

error_code socket::close()
{
    error_code ec;
    _socket.close(ec);
    return ec;
}

error_code socket::async_connect(const endpoint& e)
{
    return _socket.async_connect(e, this_coro);
}

pending_op socket::async_connect(const endpoint& ep, function_ref<bool(error_code)> f)
{
    defer_t<bool(error_code)> d(f);
    return _socket.async_connect(ep, std::move(d));
};

pending_op socket::async_receive(mutable_buffer buffer, function_ref<bool(error_code, size_t)> f)
{
    defer_t<bool(error_code, size_t)> d(f);
    return _socket.async_receive(buffer, std::move(d));
};

acceptor::acceptor() : _acceptor(impl::io::get_executor())
{}

acceptor::acceptor(const proto& p) : _acceptor(impl::io::get_executor(), p)
{}

acceptor::acceptor(const endpoint& e, bool reuse) : _acceptor(impl::io::get_executor(), e, reuse)
{}

acceptor::acceptor(const proto& p, const int& h) : _acceptor(impl::io::get_executor(), p, h)
{}

error_code acceptor::close()
{
    error_code ec;
    _acceptor.close(ec);
    return ec;
}

error_code acceptor::bind(const endpoint& ep)
{
    error_code ec;
    _acceptor.bind(ep, ec);
    return ec;
}
error_code acceptor::listen(int backlog)
{
    error_code ec;
    _acceptor.listen(backlog, ec);
    return ec;
}

error_code acceptor::async_accept(socket& s, endpoint& e)
{
    return _acceptor.async_accept(s.boost_socket(), e, this_coro);
}

} // namespace async::tcp