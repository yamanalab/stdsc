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

#include <iostream>
#include <server/state.hpp>

namespace server
{

StateInit::StateInit(void)
{
    std::cout << "State: Init" << std::endl;
}

std::shared_ptr<stdsc::State> StateInit::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateInit());
    return s;
}

void StateInit::set(stdsc::StateContext& sc, uint64_t event)
{
    switch (static_cast<Event_t>(event))
    {
        case kEventConnectSocket:
            sc.next_state(StateConnected::create());
            break;
        case kEventDisconnectSocket:
            sc.next_state(StateExit::create());
            break;
        default:
            break;
    }
}

StateConnected::StateConnected(bool is_received_valueA, bool is_received_valueB)
  : is_received_valueA_(is_received_valueA),
    is_received_valueB_(is_received_valueB)
{
    std::cout << "State: Connected" << std::endl;
}

std::shared_ptr<stdsc::State> StateConnected::create(bool is_received_valueA,
                                                     bool is_received_valueB)
{
    auto s = std::shared_ptr<stdsc::State>(
      new StateConnected(is_received_valueA, is_received_valueB));
    return s;
}

void StateConnected::set(stdsc::StateContext& sc, uint64_t event)
{
    switch (static_cast<Event_t>(event))
    {
        case kEventReceivedValueA:
            is_received_valueA_ = true;
            break;
        case kEventReceivedValueB:
            is_received_valueB_ = true;
            break;
        case kEventDisconnectSocket:
            sc.next_state(StateExit::create());
            break;
        default:
            break;
    }

    if (is_received_valueA_ && is_received_valueB_)
    {
        sc.next_state(StateReady::create());
    }
}

StateReady::StateReady(void)
{
    std::cout << "State: Ready" << std::endl;
}

std::shared_ptr<stdsc::State> StateReady::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateReady());
    return s;
}

void StateReady::set(stdsc::StateContext& sc, uint64_t event)
{
    switch (static_cast<Event_t>(event))
    {
        case kEventReceivedComputeRequest:
            sc.next_state(StateComputed::create());
            break;
        case kEventDisconnectSocket:
            sc.next_state(StateExit::create());
            break;
        default:
            break;
    }
}

StateComputed::StateComputed(void)
{
    std::cout << "State: Computed" << std::endl;
}

std::shared_ptr<stdsc::State> StateComputed::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateComputed());
    return s;
}

void StateComputed::set(stdsc::StateContext& sc, uint64_t event)
{
    switch (static_cast<Event_t>(event))
    {
        case kEventReceivedValueA:
        case kEventReceivedValueB:
            sc.next_state(StateReady::create());
            break;
        case kEventDisconnectSocket:
            sc.next_state(StateExit::create());
            break;
        default:
            break;
    }
}

StateExit::StateExit(void)
{
    std::cout << "State: Exit" << std::endl;
}

std::shared_ptr<stdsc::State> StateExit::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateExit());
    return s;
}

void StateExit::set(stdsc::StateContext& sc, uint64_t event)
{
}

} /* namespace server */
