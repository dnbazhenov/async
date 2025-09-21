/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/pending_op.hpp>

#include <list>

namespace async
{

struct pending_group
{
    struct op : pending_op_base
    {
        constexpr op(pending_group& group, pending_op_base&& base) :
            pending_op_base(std::move(base)), _group(group)
        {}

        constexpr bool operator==(const op& o)
        {
            return this == &o;
        }

      protected:
        void invoke(bool) const override;

      private:
        pending_group& _group;
    };

    friend struct op;

    pending_group& operator+=(pending_op_base&& from)
    {
        _list.emplace_back(*this, std::move(from));
        return *this;
    }

    void wait_any();
    bool wait_all();

  private:
    void on_pending_op(const op&, bool);

    static void default_cb(bool)
    {}

  private:
    std::list<op> _list;
    function_ref<void(bool)> _callback;
    // std::function<void(bool)> _callback;
};

} // namespace async