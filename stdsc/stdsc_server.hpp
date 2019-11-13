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

#ifndef STDSC_SERVER_HPP
#define STDSC_SERVER_HPP

#include <memory>
#include <stdsc/stdsc_thread.hpp>

namespace stdsc
{
class Socket;
class CallbackFunctionContainer;
class StateContext;
class ServerParam;
class ServerThreadParam;

/**
 * @brief Provides server function
 */
template <class T = ServerParam>
class Server : public Thread<T>
{
    using super = Thread<T>;
    
public:
    Server(const char* port,
           StateContext& state,
           CallbackFunctionContainer& callback);
    virtual ~Server(void);

    void start(const bool async=false);
    void stop(void);
    void wait(void);
    
private:
    virtual void exec(T& args, std::shared_ptr<ThreadException> te) const override;

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

struct ServerParam
{
    bool force_finish = false;
};

    
/**
 * @brief Provides server thread
 */
template <class T = ServerThreadParam>
class ServerThread : public Thread<T>
{
    using super = Thread<T>;

public:
    ServerThread(Socket& sock,
                 StateContext& state,
                 CallbackFunctionContainer& callback);
    virtual ~ServerThread(void);

    void start(void);
    void stop(void);
    void join(void);

private:
    virtual void exec(T& args, std::shared_ptr<ThreadException> te) const override;

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

struct ServerThreadParam
{
    bool force_finish = false;
};

} /* namespace stdsc */

#endif /* STDSC_SERVER_HPP */
