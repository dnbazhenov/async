/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/event_sink.hpp>
#include <async/impl/wait_queue.hpp>

namespace async
{

struct pending_op_base : private impl::event_sink
{
    // using impl::event_sink::event_sink;
    constexpr pending_op_base(impl::event_sink&& old) : impl::event_sink(std::move(old))
    {}

  protected:
    virtual void invoke(bool) const = 0;
};

struct pending_op : pending_op_base
{
    using pending_op_base::pending_op_base;

    bool wait();

  protected:
    void invoke(bool) const override;

  private:
    impl::wait_queue::head* _wq = nullptr;
    bool* _res = nullptr;
};

template <typename Signature>
struct defer_t : private impl::event_source
{
    using function_type = typename function_ref<Signature>::return_type;
    // using function_type = typename signature_traits<Signature>::function;
    using return_type = pending_op;

    static constexpr auto void_return = std::same_as<function_type, void>;

    constexpr defer_t(function_ref<Signature> func) : _func(func)
    {}
    defer_t(const defer_t&) = delete;
    constexpr defer_t(defer_t&&) = default;

    template <typename... Args>
    void operator()(Args&&... args)
    {
        if (has_sink())
        {
            bool res = true;

            if constexpr (void_return)
            {
                _func(std::forward<Args>(args)...);
            }
            else
            {
                res = _func(std::forward<Args>(args)...);
            }

            impl::event_source::invoke_sink(res);
        }
    }

    pending_op get_sink()
    {
        return std::move(make_sink());
    }

  private:
    function_ref<Signature> _func;
};

} // namespace async
