/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vector>
#include <stdsc/stdsc_buffer.hpp>

namespace stdsc
{

/* Buffer */

struct Buffer::Impl
{
    Impl(void) : buffer_()
    {
    }

    Impl(std::size_t size) : buffer_(size)
    {
    }

    Impl(std::size_t size, uint8_t val) : buffer_(size, val)
    {
    }

    std::vector<uint8_t> buffer_;
};

Buffer::Buffer(void) : pimpl_(new Impl())
{
}

Buffer::Buffer(std::size_t size) : pimpl_(new Impl(size))
{
}

Buffer::Buffer(std::size_t size, uint8_t val) : pimpl_(new Impl(size, val))
{
}

Buffer::Buffer(Buffer&& buffer) : pimpl_(new Impl())
{
    pimpl_->buffer_ = std::move(buffer.pimpl_->buffer_);
}

Buffer& Buffer::operator=(Buffer&& buffer)
{
    pimpl_->buffer_ = std::move(buffer.pimpl_->buffer_);
    return *this;
}

void Buffer::resize(std::size_t size)
{
    pimpl_->buffer_.resize(size);
}

std::size_t Buffer::size(void) const
{
    return pimpl_->buffer_.size();
}

const void* Buffer::data(void) const
{
    return reinterpret_cast<const void*>(pimpl_->buffer_.data());
}

void* Buffer::data(void)
{
    return reinterpret_cast<void*>(pimpl_->buffer_.data());
}

/* BufferStream */

BufferStream::BufferStream(std::size_t size) : BufferStream(size, 0)
{
}

BufferStream::BufferStream(std::size_t size, uint8_t val) : Buffer(size, val)
{
    setup(reinterpret_cast<char*>(super::data()), size);
}

BufferStream::BufferStream(const Buffer& buffer) : Buffer(buffer)
{
    setup(reinterpret_cast<char*>(super::data()), super::size());
}

void BufferStream::setup(char* p, size_t size)
{
    char* sp = p;
    char* ep = p + size;
    setp(sp, ep);
    setg(sp, sp, ep);
}

} /* namespace stdsc */
