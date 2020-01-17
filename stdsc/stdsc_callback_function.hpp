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

#ifndef STDSC_CALLBACK_FUNCTION_HPP
#define STDSC_CALLBACK_FUNCTION_HPP

#include <cstdint>
#include <memory>

#define DECLARE_REQUEST_CLASS(cls)                                      \
    class cls : public stdsc::CallbackFunction                          \
    {                                                                   \
    protected:                                                          \
        virtual void request_function(                                  \
            uint64_t code,                                              \
            stdsc::StateContext& state,                                 \
            void* cdata_on_each, void* cdata_on_all) override;          \
    }

#define DECLARE_DATA_CLASS(cls)                                         \
    class cls : public stdsc::CallbackFunction                          \
    {                                                                   \
    protected:                                                          \
        virtual void data_function(                                     \
            uint64_t code,                                              \
            const stdsc::Buffer& buffer,                                \
            stdsc::StateContext& state,                                 \
            void* cdata_on_each, void* cdata_on_all) override;          \
    }

#define DECLARE_DOWNLOAD_CLASS(cls)                                     \
    class cls : public stdsc::CallbackFunction                          \
    {                                                                   \
    protected:                                                          \
        virtual void download_function(                                 \
            uint64_t code,                                              \
            const stdsc::Socket& sock,                                  \
            stdsc::StateContext& state,                                 \
            void* cdata_on_each, void* cdata_on_all) override;          \
    }

#define DECLARE_UPDOWNLOAD_CLASS(cls)                                   \
    class cls : public stdsc::CallbackFunction                          \
    {                                                                   \
    protected:                                                          \
        virtual void updownload_function(                               \
            uint64_t code,                                              \
            const stdsc::Buffer& buffer,                                \
            const stdsc::Socket& sock,                                  \
            stdsc::StateContext& state,                                 \
            void* cdata_on_each, void* cdata_on_all) override;          \
    }

#define DEFUN_DATA(cls)                                                 \
    void cls::data_function(uint64_t code,                              \
                            const stdsc::Buffer& buffer,                \
                            stdsc::StateContext& state,                 \
                            void* cdata_on_each, void* cdata_on_all)

#define DEFUN_REQUEST(cls)                                              \
    void cls::request_function(uint64_t code,                           \
                               stdsc::StateContext& state,              \
                               void* cdata_on_each, void* cdata_on_all)

#define DEFUN_DOWNLOAD(cls)                                             \
    void cls::download_function(uint64_t code,                          \
                                const stdsc::Socket& sock,              \
                                stdsc::StateContext& state,             \
                                void* cdata_on_each, void* cdata_on_all)

#define DEFUN_UPDOWNLOAD(cls)                                           \
    void cls::updownload_function(uint64_t code,                        \
                                  const stdsc::Buffer& buffer,          \
                                  const stdsc::Socket& sock,            \
                                  stdsc::StateContext& state,           \
                                  void* cdata_on_each, void* cdata_on_all)

/* DEFINE_REQUEST_FUNC macro is deplicated in v2.x */
#define DEFINE_REQUEST_FUNC(cls)                                        \
    void cls::request_function(uint64_t code,                           \
                               stdsc::StateContext& state,              \
                               void* cdata_on_each, void* cdata_on_all) \
    {}

/* DEFINE_DATA_FUNC macro is deplicated in v2.x */
#define DEFINE_DATA_FUNC(cls)                                           \
    void cls::data_function(uint64_t code,                              \
                            const stdsc::Buffer& buffer,                \
                            stdsc::StateContext& state,                 \
                            void* cdata_on_each, void* cdata_on_all)    \
    {}

/* DEFINE_DOWNLOAD_FUNC macro is deplicated in v2.x */
#define DEFINE_DOWNLOAD_FUNC(cls)                                       \
    void cls::download_function(uint64_t code,                          \
                                const stdsc::Socket& sock,              \
                                stdsc::StateContext& state,             \
                                void* cdata_on_each, void* cdata_on_all)\
    {}

#define DEF_CDATA_ON_EACH(type) auto* cdata_e = static_cast< type *>(cdata_on_each)
#define DEF_CDATA_ON_ALL(type)  auto* cdata_a = static_cast< type *>(cdata_on_all)

namespace stdsc
{
class Buffer;
class Socket;
class StateContext;

/**
 * @brief Defines a callback function interface.
 * That any implementation of this interface can be used by inheritor's to handle a callback function.
 */
class CallbackFunction
{
public:
    CallbackFunction(void) = default;
    virtual ~CallbackFunction(void) = default;

    void eval(uint64_t code, StateContext& state,
              void* cdata_on_each=nullptr, void* cdata_on_all=nullptr);
    void eval(uint64_t code, const Buffer& buffer, StateContext& state,
              void* cdata_on_each=nullptr, void* cdata_on_all=nullptr);
    void eval(uint64_t code, const Socket& sock, StateContext& state,
              void* cdata_on_each=nullptr, void* cdata_on_all=nullptr);
    void eval(uint64_t code, const Buffer& buffer, const Socket& sock, StateContext& state,
              void* cdata_on_each=nullptr, void* cdata_on_all=nullptr);

protected:
    virtual void request_function(uint64_t code, StateContext& state,
                                  void* cdata_on_each, void* cdata_on_all);
    virtual void data_function(uint64_t code, const Buffer& buffer,
                               StateContext& state,
                               void* cdata_on_each, void* cdata_on_all);
    virtual void download_function(uint64_t code,
                                   const Socket& sock, StateContext& state,
                                   void* cdata_on_each, void* cdata_on_all);
    virtual void updownload_function(uint64_t code, const Buffer& buffer,
                                     const Socket& sock, StateContext& state,
                                     void* cdata_on_each, void* cdata_on_all);
};
} /* namespace stdsc */

#endif /* STDSC_CALLBACK_FUNCTION_HPP */
