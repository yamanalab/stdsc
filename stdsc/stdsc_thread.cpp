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

#include <mutex>
#include <exception>

#include <stdsc/stdsc_thread.hpp>

namespace stdsc
{

struct ThreadException::Impl
{
    void set_current_exception(void)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        ep_ = std::current_exception();
    }

    void rethrow_if_has_exception(void) const
    {
        if (ep_)
        {
            std::rethrow_exception(ep_);
        }
    }

    bool has_exception(void) const
    {
        return ep_ != nullptr;
    }

    std::exception_ptr ep_;
    std::mutex mtx_;
};

ThreadException::ThreadException(void) : pimpl_(new Impl())
{
}

ThreadException::~ThreadException(void)
{
}

void ThreadException::set_current_exception(void)
{
    pimpl_->set_current_exception();
}

void ThreadException::rethrow_if_has_exception(void) const
{
    pimpl_->rethrow_if_has_exception();
}

bool ThreadException::has_exception(void) const
{
    return pimpl_->has_exception();
}

} /* namespace stdsc */
