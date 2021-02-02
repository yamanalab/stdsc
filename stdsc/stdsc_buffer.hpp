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

#ifndef STDSC_BUFFER_HPP
#define STDSC_BUFFER_HPP

#include <memory>
#include <iostream>

namespace stdsc
{

/**
 * @brief This class is used to hold the generic data.
 */
class Buffer
{
    friend class BufferStream;

public:
    Buffer(void);
    explicit Buffer(std::size_t size);
    Buffer(std::size_t size, uint8_t val);

    virtual ~Buffer(void) = default;

    Buffer(const Buffer&) = default;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& buffer);
    Buffer& operator=(Buffer&& buffer);

    void resize(std::size_t size);

    std::size_t size(void) const;

    const void* data(void) const;

    void* data(void);

    void release(void);

private:
    struct Impl;
    const std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides a streambuf interface for Buffer.
 */
class BufferStream : public Buffer, public std::streambuf
{
    using super = Buffer;

public:
    BufferStream(void) = delete;
    BufferStream(std::size_t size);
    BufferStream(std::size_t size, uint8_t val);
    BufferStream(const Buffer& buffer);

    virtual ~BufferStream(void) = default;

    BufferStream(const BufferStream&) = delete;
    BufferStream& operator=(const BufferStream&) = delete;
    BufferStream(BufferStream&& buffer) = delete;
    BufferStream& operator=(BufferStream&& buffer) = delete;

    std::ios::pos_type seekoff(
        std::ios::off_type __off, 
        std::ios_base::seekdir __way, 
        std::ios_base::openmode __which = std::ios_base::in | std::ios_base::out) override;
    std::ios::pos_type seekpos(
        std::ios::pos_type __sp, 
        std::ios_base::openmode __which = std::ios_base::in | std::ios_base::out) override;

private:
    void setup(char* p, size_t size);
};

} /* namespace stdsc */

#endif /* STDSC_BUFFER_HPP */
