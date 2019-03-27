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

#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>
#include <stdsc/stdsc_server.hpp>
#include <server/callback_function.hpp>
#include <share/define.hpp>

namespace stdsc
{
    class StateContext;
}

namespace server
{

struct AddServer
{
    AddServer(stdsc::StateContext& state, const char* port);
    ~AddServer(void) = default;

    void start(void);
    void join(void);

private:
    CallbackParam param_;
    std::shared_ptr<stdsc::Server<>> server_;
};

} /* namespace server */

#endif /* SERVER_HPP */
