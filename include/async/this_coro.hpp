/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

namespace async
{

/**
 * @brief Completion token for currently running coroutine.
 */
struct this_coro_t
{};

static constexpr auto this_coro = this_coro_t{};

} // namespace async
