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

#include <memory>

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
template <class T>
struct Server<T>::Impl
{
    std::shared_ptr<ThreadException> te_;

    Impl(const char* port, StateContext& state) : state_(state)
    {
        auto listen_socket = Socket::make_listen_socket(port, SO_REUSEADDR);
        sock_ =
          std::make_shared<Socket>(Socket::accept_connection(listen_socket));
        te_ = ThreadException::create();
    }

    void set_callback(CallbackFunctionContainer& callback)
    {
        callback_ = callback;
    }

    void exec(T& args, std::shared_ptr<ThreadException> te)
    {
        Packet packet;
        bool running = true;

        while (running)
        {
            try
            {
                sock_->recv_packet(packet);
                STDSC_LOG_TRACE("Received packet. (code:0x%08x)",
                                 packet.control_code);

                if (kControlCodeExit == packet.control_code)
                {
                    running = false;
                    break;
                }

                try
                {
                    callback_.eval(*sock_, packet, state_);
                    STDSC_LOG_TRACE("callback finished.");
                    sock_->send_packet(make_packet(kControlCodeAccept));
                }
                catch (const CallbackException& e)
                {
                    STDSC_LOG_TRACE("Failed to execute callback function. %s",
                                     e.what());
                    sock_->send_packet(make_packet(kControlCodeReject));
                }
            }
            catch (const stdsc::AbstractException& e)
            {
                STDSC_LOG_ERR("Failed to client process (%s)", e.what());
                te->set_current_exception();
                break;
            }
        }

        sock_->shutdown();
        sock_->close();
    }

private:
    StateContext& state_;
    CallbackFunctionContainer callback_;
    std::shared_ptr<Socket> sock_;
};

template <class T>
Server<T>::Server(const char* port, StateContext& state)
  : pimpl_(new Impl(port, state))
{
}

template <class T>
Server<T>::~Server(void)
{
}

template <class T>
void Server<T>::set_callback(CallbackFunctionContainer& callback)
{
    pimpl_->set_callback(callback);
}

template <class T>
void Server<T>::start(void)
{
    ServerParam p = {0};
    super::start(p, pimpl_->te_);
}

template <class T>
void Server<T>::wait_for_finish(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void Server<T>::exec(T& args, std::shared_ptr<ThreadException> te) const
{
    try
    {
        pimpl_->exec(args, te);
    }
    catch (...)
    {
        te->set_current_exception();
    }
}

template class Server<ServerParam>;

} /* namespace stdsc */
