/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <exception>
#include <stdexcept>

namespace async::exception
{

struct bad_coroutine : std::logic_error
{
    explicit bad_coroutine() : std::logic_error("using unitialized coroutine")
    {}
};

struct no_coroutine : std::logic_error
{
    explicit no_coroutine() :
        std::logic_error("coroutine operation invoked in non-coroutine context")
    {}
};

struct canceled : std::runtime_error
{
    explicit canceled() : std::runtime_error("coroutine has been canceled")
    {}
};

} // namespace async::exception
