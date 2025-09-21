/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/intrusive_list.hpp>

#include <memory>

namespace async
{

struct shared_buffer : impl::list::list_hook
{
    using shared_data = std::shared_ptr<char[]>;

    shared_buffer(const shared_buffer&) = delete;
    shared_buffer(shared_buffer&&) = default;

    shared_buffer(shared_data buffer, std::size_t size, std::size_t max_size) :
        _data(std::move(buffer)), _end(get_end(max_size)), _size(size)
    {}

    /**
     * @brief Get current read pointer.
     */
    constexpr const char* read_ptr() const
    {
        return _data.get();
    }

    /**
     * @brief Get current write pointer.
     */
    constexpr char* write_ptr() const
    {
        return _data.get() + _size;
    }

    /**
     * @brief Commit written data.
     */
    void commit(std::size_t count);

    /**
     * @brief Consume read data.
     */
    void consume(std::size_t count);

    /**
     * @brief Size of available data for reading.
     */
    std::size_t avail_read() const
    {
        return _size;
    }

    /**
     * @brief Size of available data for writing.
     */
    std::size_t avail_write() const
    {
        return static_cast<std::size_t>(_end - _data.get());
    }

    /**
     * @brief Split filled-in part into a separate buffer.
     */
    shared_buffer split_read(std::size_t);

  private:
    char* get_end(std::size_t size) const
    {
        return _data.get() + size;
    }

  private:
    shared_data _data;
    char* _end;
    size_t _size = 0;
};

struct read_buffer final : private shared_buffer
{
    read_buffer(shared_buffer&& shared) : shared_buffer(std::move(shared))
    {}
    read_buffer(const read_buffer&) = delete;
    read_buffer(read_buffer&&) = default;

    const char* data() const
    {
        return shared_buffer::read_ptr();
    }

    std::size_t size() const
    {
        return shared_buffer::avail_read();
    }

    void consume(std::size_t count)
    {
        shared_buffer::consume(count);
    }
};

struct write_buffer : private shared_buffer
{
    write_buffer(shared_buffer&& shared) : shared_buffer(std::move(shared))
    {}
    write_buffer(const read_buffer&) = delete;
    write_buffer(write_buffer&&) = default;

    char* data() const
    {
        return shared_buffer::write_ptr();
    }

    std::size_t size() const
    {
        return shared_buffer::avail_write();
    }

    void conmmit(std::size_t count)
    {
        shared_buffer::commit(count);
    }
};

using shared_buffer_seq = impl::list::list<shared_buffer>;
using read_buffer_seq = impl::list::list<read_buffer>;
using write_buffer_seq = impl::list::list<write_buffer>;

} // namespace async
