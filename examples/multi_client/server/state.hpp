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

#ifndef STATE_HPP
#define STATE_HPP

#include <memory>
#include <cstdbool>
#include <stdsc/stdsc_state.hpp>

namespace server
{

/**
 * @brief Enumeration for state.
 */
enum StateId_t : int32_t
{
    kStateNil      = 0,
    kStateInit     = 1,
    kStateReady    = 2,
    kStateComputed = 3,
};

/**
 * @brief Enumeration for events.
 */
enum Event_t : uint64_t
{
    kEventReceivedValueA         = 1,
    kEventReceivedValueB         = 2,
    kEventReceivedComputeRequest = 3,
    kEventReceivedResultRequest  = 4,
};

/**
 * @brief Provides 'Connected' state.
 */
struct StateInit : public stdsc::State
{
    static std::shared_ptr<stdsc::State> create(bool is_received_valueA = false,
                                                bool is_received_valueB = false);
    StateInit(bool is_received_valueA = false,
                   bool is_received_valueB = false);
    virtual void set(stdsc::StateContext& sc, uint64_t event) override;
    STDSC_STATE_DEFID(kStateInit);

private:
    bool is_received_valueA_;
    bool is_received_valueB_;
};

/**
 * @brief Provides 'Ready' state.
 */
struct StateReady : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateReady(void);
    virtual void set(stdsc::StateContext& sc, uint64_t event) override;
    STDSC_STATE_DEFID(kStateReady);

};

/**
 * @brief Provides 'Computed' state.
 */
struct StateComputed : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateComputed(void);
    virtual void set(stdsc::StateContext& sc, uint64_t event) override;
    STDSC_STATE_DEFID(kStateComputed);

};


} /* server */

#endif /* STATE_HPP */
