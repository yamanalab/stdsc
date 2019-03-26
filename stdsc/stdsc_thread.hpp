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

#ifndef STDSC_THREAD_HPP
#define STDSC_THREAD_HPP

#include <thread>
#include <memory>

namespace stdsc
{

/**
 * @brief This class is used to hold the exception on thread.
 */
class ThreadException
{
public:
    static std::shared_ptr<ThreadException> create(void)
    {
        std::shared_ptr<ThreadException> sp(new ThreadException());
        return sp;
    }
    ~ThreadException(void);

    void set_current_exception(void);
    void rethrow_if_has_exception(void) const;
    bool has_exception(void) const;

private:
    ThreadException(void);

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Defined a thread interface.
 * That any implementation of this interface can be used by inheritor's to
 * handle a thread.
 */
template <class T>
class Thread
{
public:
    Thread(void)
    {
    }
    virtual ~Thread(void)
    {
    }

    void start(T& args, std::shared_ptr<ThreadException> te)
    {
        std::thread th(&Thread<T>::exec, this, std::ref(args), te);
        th_ = std::move(th);
    }

    void join(void)
    {
        if (th_.joinable())
        {
            th_.join();
        }
    }

protected:
    inline virtual void exec(T& args,
                             std::shared_ptr<ThreadException> te) const;

private:
    std::thread th_;
};

template <class T>
inline void Thread<T>::exec(T&, std::shared_ptr<ThreadException> te) const
{
}

} /* namespace stdsc */

#endif /* STDSC_THREAD_HPP */
