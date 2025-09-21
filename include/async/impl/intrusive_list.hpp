/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <boost/intrusive/list.hpp>

namespace async::impl::list
{

using namespace boost::intrusive;

using list_hook = list_base_hook<link_mode<auto_unlink>>;

template <typename T>
using list = boost::intrusive::list<T, constant_time_size<false>>;

} // namespace async::impl::list
