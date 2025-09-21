/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <boost/asio/io_context.hpp>

namespace async::impl
{

using io_context = boost::asio::io_context;
using io_executor = io_context::executor_type;

io_context& get_context();
io_executor get_executor();

} // namespace async::impl
