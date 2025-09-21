/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/function_ref.hpp>

#include <vector>

namespace async::impl
{

// struct event_source;

struct event_sink
{
    friend struct event_source;

    constexpr event_sink() = default;
    constexpr event_sink(const event_sink&) = delete;
    constexpr event_sink(event_sink&& old) : _owner(old._owner)
    {
        old._owner = nullptr;
        if (_owner)
        {
            *_owner = this;
        }
    }
    constexpr event_sink(event_sink** owner) : _owner(owner)
    {
        if (_owner)
        {
            *_owner = this;
        }
    }

    constexpr ~event_sink()
    {
        if (_owner)
        {
            *_owner = nullptr;
        }
    }

    event_sink& operator=(const event_sink&) = delete;
    constexpr event_sink& operator=(event_sink&& old)
    {
        if (_owner)
        {
            *_owner = nullptr;
        }

        _owner = old._owner;

        old._owner = nullptr;

        if (_owner)
        {
            *_owner = this;
        }

        return *this;
    }

  protected:
    virtual void invoke(bool) const {};

  private:
    event_sink** _owner = nullptr;
};

struct event_source
{
    constexpr event_source() = default;
    constexpr event_source(const event_source&) = delete;
    constexpr event_source(event_source&& old) : _sink(old._sink)
    {
        old._sink = nullptr;
        if (_sink)
        {
            _sink->_owner = &_sink;
        }
    }
    constexpr ~event_source()
    {
        if (_sink)
        {
            _sink->_owner = nullptr;
        }
    }

    event_source& operator=(const event_source&) = delete;
    event_source& operator=(event_source&&) = delete;

  protected:
    constexpr event_sink make_sink()
    {
        return event_sink(&this->_sink);
    }

    void invoke_sink(bool r)
    {
        if (_sink)
        {
            _sink->invoke(r);
        }
    }

    constexpr bool has_sink() const
    {
        return !!_sink;
    }

  private:
    event_sink* _sink = nullptr;
};

} // namespace async::impl
