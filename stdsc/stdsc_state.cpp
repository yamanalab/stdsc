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

#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_log.hpp>

namespace stdsc
{
StateContext::StateContext(std::shared_ptr<State> state) : state_(state)
{
}

void StateContext::next_state(std::shared_ptr<State> next)
{
    STDSC_LOG_TRACE("Next state: %lu -> %lu", state_->id(), next->id());
    state_ = next;
}

void StateContext::set(uint64_t act)
{
    state_->set(*this, act);
}

uint64_t StateContext::current_state(void) const
{
    return state_->id();
}

} /* stdsc */
