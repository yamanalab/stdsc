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

#ifndef STDSC_STATE_HPP
#define STDSC_STATE_HPP

#include <string>
#include <memory>

#define STDSC_STATE_DEFID(_id_)                 \
    virtual int32_t id(void) const {            \
        return (_id_);                          \
    }                                           \
    virtual std::string str(void) const {       \
        return #_id_;                           \
    }                                           \

namespace stdsc
{

struct StateContext;

/**
 * @brief Defines a state interface.
 * That any implementation of this interface can be used by inheritor's to
 * handle a state.
 */
struct State
{
    enum StateId_t : int32_t
    {
        StateUndefined = -1,
    };
        
    virtual ~State() = default;
    virtual void set(StateContext& sc, uint64_t act) = 0;
    virtual int32_t id(void) const;
    virtual std::string str(void) const;
};

/**
 * @brief This class is used to hold the state.
 */
struct StateContext
{
    StateContext(std::shared_ptr<State> state);
    void next_state(std::shared_ptr<State> next);
    void set(uint64_t act);
    int32_t current_state(void) const;
    std::string current_state_str(void) const;

private:
    std::shared_ptr<State> state_;
};

} /* stdsc */

#endif /* STDSC_STATE_HPP */
