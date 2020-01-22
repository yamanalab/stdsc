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

#include <unistd.h>
#include <memory>
#include <limits>
#include <vector>
#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_state.hpp>

namespace stdsc
{

static constexpr std::size_t RETRY_INTERVAL_SEC = 1;
static constexpr std::size_t MAX_RETRY_COUNT =
    std::numeric_limits<size_t>::max();
    
//
// Server
//

template <class T>
struct Server<T>::Impl
{
    struct ResourceContainer
    {
        ResourceContainer(Socket& sock,
                          StateContext& state,
                          CallbackFunctionContainer& callback)
            : sock_(sock),
              state_(state),        // copy
              callback_(callback),  // ref
              th_(new ServerThread<>(sock_, state_, callback)),
              is_released_(false)
        {}

        virtual ~ResourceContainer()
        {
            release();
        }

        void invoke(void)
        {
            th_->start();
        }

        void wait(void)
        {
            try
            {
                th_->join();
            }
            catch (const stdsc::AbstractException& e)
            {
                STDSC_LOG_ERR("Failed to server process (%s)", e.what());
                release();
            }
        }

        void release(void)
        {
            if (!is_released_) {
                sock_.shutdown();
                sock_.close();
                is_released_ = true;
            }
        }

    private:
        Socket sock_;
        StateContext state_;
        CallbackFunctionContainer& callback_;
        std::shared_ptr<ServerThread<>> th_;
        bool is_released_;
    };

    Impl(const char* port,
         StateContext& state,
         CallbackFunctionContainer& callback)
        : param_(),
          port_(port),
          state_(state),       // copy
          callback_(callback)  // copy
    {
        te_ = ThreadException::create();
    }

    void exec(T& args, std::shared_ptr<ThreadException> te)
    {
        auto listen_socket =
            Socket::make_listen_socket(port_, SO_REUSEADDR);
        STDSC_LOG_INFO("Listen socket.");

        std::vector<std::shared_ptr<ResourceContainer>> resources;
            
        while (!args.force_finish)
        {
            try
            {
                Socket sock = Socket::accept_connection(listen_socket);
                
                std::shared_ptr<ResourceContainer>
                    rc(new ResourceContainer(sock, state_, callback_));
                rc->invoke();
                
                resources.push_back(std::move(rc));
            }
            catch (stdsc::SocketException& e)
            {}
        }

        for (auto& r : resources)
        {
            r->wait();
            r->release();
        }

        listen_socket.close();
    }

public:
    std::shared_ptr<ThreadException> te_;
    ServerParam param_;
    
private:
    const char* port_;
    StateContext state_;
    CallbackFunctionContainer callback_;
};

template <class T>
Server<T>::Server(const char* port,
                  StateContext& state,
                  CallbackFunctionContainer& callback)
    : pimpl_(new Impl(port, state, callback))
{
}

template <class T>
Server<T>::~Server(void)
{
}

template <class T>
void Server<T>::start(const bool async)
{
    pimpl_->param_.force_finish = false;
    super::start(pimpl_->param_, pimpl_->te_);

    if (!async) {
        wait();
    }
}

template <class T>
void Server<T>::stop(void)
{
    pimpl_->param_.force_finish = true;
}

template <class T>
void Server<T>::wait(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void Server<T>::exec(T& args, std::shared_ptr<ThreadException> te) const
{
    pimpl_->exec(args, te);
}

template class Server<ServerParam>;
    
//
// ServerThread
//
    
template <class T>
struct ServerThread<T>::Impl
{
    Impl(Socket& sock,
         StateContext& state,
         CallbackFunctionContainer& callback)
        : param_(),
          sock_(sock),
          state_(state),      // ref
          callback_(callback) // ref
    {
        te_ = ThreadException::create();
    }

    void exec(T& args, std::shared_ptr<ThreadException> te)
    {
        while (!args.force_finish)
        {
            try
            {
                Packet packet;
                sock_.recv_packet(packet);
                STDSC_LOG_TRACE("Received packet. (code:0x%08x)",
                                packet.control_code);

                try
                {
                    callback_.eval(sock_, packet, state_);
                    STDSC_LOG_TRACE("callback finished.");
                    sock_.send_packet(make_packet(kControlCodeAccept));
                }
                catch (const CallbackException& e)
                {
                    STDSC_LOG_TRACE(
                        "Failed to execute callback function. %s", e.what());
                    sock_.send_packet(make_packet(kControlCodeReject));
                }
            }
            catch (const stdsc::AbstractException& e)
            {
                STDSC_LOG_ERR("Failed to server process (%s)", e.what());
                te->set_current_exception();
                break;
            }
        }
    }

public:
    std::shared_ptr<ThreadException> te_;
    ServerThreadParam param_;
    
private:
    Socket& sock_;
    StateContext& state_;
    CallbackFunctionContainer& callback_;
};

template <class T>
ServerThread<T>::ServerThread(Socket& sock,
                              StateContext& state,
                              CallbackFunctionContainer& callback)
    : pimpl_(new Impl(sock, state, callback))
{
}

template <class T>
ServerThread<T>::~ServerThread(void)
{
}

template <class T>
void ServerThread<T>::start(void)
{
    pimpl_->param_.force_finish = false;
    super::start(pimpl_->param_, pimpl_->te_);
}

template <class T>
void ServerThread<T>::stop(void)
{
    pimpl_->param_.force_finish = true;
}

template <class T>
void ServerThread<T>::join(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void ServerThread<T>::exec(T& args, std::shared_ptr<ThreadException> te) const
{
    pimpl_->exec(args, te);
}

template class ServerThread<ServerThreadParam>;

} /* namespace stdsc */
