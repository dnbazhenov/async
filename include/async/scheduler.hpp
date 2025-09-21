/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

namespace async
{

struct scheduler
{
    static void setup_signal_handlers();
    static void run();
    static void stop(bool failure = false);
};

} // namespace async
