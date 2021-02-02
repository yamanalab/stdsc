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

std::ios::pos_type BufferStream::seekoff(
    std::ios::off_type __off, 
    std::ios_base::seekdir __way, 
    std::ios_base::openmode __which)
{
    std::ios::pos_type result;

    bool is_input  = (__which & std::ios_base::in)  == std::ios_base::in;
    bool is_output = (__which & std::ios_base::out) == std::ios_base::out;

    if ((!is_input && !is_output)
        || ((is_input && is_output) && (__way == std::ios_base::cur))) {
        return std::ios::pos_type(std::ios::off_type(-1));
    }

    char* pos_in_base;
    char* pos_out_base;

    std::ios::pos_type pos_in;
    std::ios::pos_type pos_out;

    if (is_input) {

        switch (__way) {
        case std::ios_base::end:
            pos_in_base = this->egptr() + __off - 1;
            break;
        case std::ios_base::cur:
            pos_in_base = this->gptr() + __off;
            break;
        case std::ios_base::beg:
        default:
            pos_in_base = this->eback() + __off;
            break;
        }

        if (this->eback() <= pos_in_base && pos_in_base < this->egptr()) {
            pos_in = pos_in_base - this->eback();
        } else {
            pos_in_base = nullptr;
            pos_in = std::ios::pos_type(std::ios::off_type(-1));
        }

    } else {
        pos_in_base = nullptr;
        pos_in = std::ios::pos_type(std::ios::off_type(-1));
    }

    if (is_output) {

        switch (__way) {
        case std::ios_base::end:
            pos_out_base = this->epptr() + __off - 1;
            break;
        case std::ios_base::cur:
            pos_out_base = this->pptr() + __off;
            break;
        case std::ios_base::beg:
        default:
            pos_out_base = this->pbase() + __off;
            break;
        }

        if (this->pbase() <= pos_out_base && pos_out_base < this->epptr()) {
            pos_out = pos_out_base - this->pbase();
        } else {
            pos_out_base = nullptr;
            pos_out = std::ios::pos_type(std::ios::off_type(-1));
        }

    } else {
        pos_out_base = nullptr;
        pos_out = std::ios::pos_type(std::ios::off_type(-1));
    }

    if (((is_input && !pos_in_base) || (is_output && !pos_out_base))
        || ((is_input && is_output) && (pos_in != pos_out))) {
        result = std::ios::pos_type(std::ios::off_type(-1));
    } else {
        result = (is_input ? pos_in : pos_out);

        if (is_input && (gptr() != pos_in_base)) {
            this->setg(this->eback(), pos_in_base, this->egptr());
        }

        if (is_output && (pptr() != pos_out_base)) {
            this->pbump(pos_out_base - this->pptr());
        }
    }

    return result;
}

std::ios::pos_type BufferStream::seekpos(pos_type __sp, std::ios_base::openmode __which)
{
    return seekoff(off_type(__sp), std::ios_base::beg, __which);
}

void BufferStream::setup(char* p, size_t size)
{
    char* sp = p;
    char* ep = p + size;
    setp(sp, ep);
    setg(sp, sp, ep);
}

} /* namespace stdsc */
