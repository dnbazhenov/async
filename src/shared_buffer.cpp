/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <async/shared_buffer.hpp>

namespace async
{

void shared_buffer::commit(std::size_t count)
{
    auto new_size = _size + count;

    // check overflow and boundary
    if (new_size < _size || _data.get() + new_size > _end)
    {
        throw std::out_of_range("buffer size write overrun");
    }

    _size = new_size;
}

void shared_buffer::consume(std::size_t count)
{
    // check boundary
    if (count > _size)
    {
        throw std::out_of_range("buffer size read overrun");
    }

    auto data = _data.get();

    _data = shared_data{std::move(_data), data + count};
    _size -= count;
}

shared_buffer shared_buffer::split_read(std::size_t count)
{
    char* data = _data.get();

    // treat split data as consumed
    consume(count);

    // create shared buffer with the original data point
    return {{_data, data}, count, count};
}

} // namespace async