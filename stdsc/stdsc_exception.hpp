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

#ifndef STDSC_EXCEPTION_HPP
#define STDSC_EXCEPTION_HPP

#include <errno.h>
#include <stdexcept>
#include <sstream>

#define STDSC_THROW_SOCKET(message)                   \
    {                                                 \
        throw stdsc::SocketException(message, errno); \
    }

#define STDSC_THROW_INVARIANT(message)            \
    {                                             \
        throw stdsc::InvariantException(message); \
    }

#define STDSC_THROW_CALLBACK(message)            \
    {                                            \
        throw stdsc::CallbackException(message); \
    }

#define STDSC_THROW_REJECT(message)            \
    {                                          \
        throw stdsc::RejectException(message); \
    }

#define STDSC_THROW_FAILURE(message)            \
    {                                           \
        throw stdsc::FailureException(message); \
    }

#define STDSC_THROW_FILE(message)            \
    {                                        \
        throw stdsc::FileException(message); \
    }

#define STDSC_THROW_INVPARAM(message)            \
    {                                            \
        throw stdsc::InvParamException(message); \
    }

#define STDSC_IF_CHECK(cond, message)       \
    {                                       \
        if (!(cond))                        \
            STDSC_THROW_INVARIANT(message); \
    }

#define STDSC_THROW_SOCKET_IF_CHECK(cond, msg) \
    do                                         \
    {                                          \
        if (!(cond))                           \
        {                                      \
            STDSC_THROW_SOCKET(msg);           \
        }                                      \
    } while (0)

#define STDSC_THROW_CALLBACK_IF_CHECK(cond, msg) \
    do                                           \
    {                                            \
        if (!(cond))                             \
        {                                        \
            STDSC_THROW_CALLBACK(msg);           \
        }                                        \
    } while (0)

#define STDSC_THROW_REJECT_IF_CHECK(cond, msg) \
    do                                         \
    {                                          \
        if (!(cond))                           \
        {                                      \
            STDSC_THROW_REJECT(msg);           \
        }                                      \
    } while (0)

#define STDSC_THROW_FAILURE_IF_CHECK(cond, msg) \
    do                                          \
    {                                           \
        if (!(cond))                            \
        {                                       \
            STDSC_THROW_FAILURE(msg);           \
        }                                       \
    } while (0)

#define STDSC_THROW_FILE_IF_CHECK(cond, msg) \
    do                                       \
    {                                        \
        if (!(cond))                         \
        {                                    \
            STDSC_THROW_FILE(msg);           \
        }                                    \
    } while (0)

#define STDSC_THROW_INVPARAM_IF_CHECK(cond, msg) \
    do                                           \
    {                                            \
        if (!(cond))                             \
        {                                        \
            STDSC_THROW_INVPARAM(msg);           \
        }                                        \
    } while (0)

namespace stdsc
{

/**
 * @brief Defines a exception interface.
 * That any implementation of this interface can be used by inheritor's to
 * handle a exception.
 */
class AbstractException : public std::runtime_error
{
private:
    mutable char* message_ptr_;
    int32_t outward_error_code_;

protected:
    const char* marshal(const std::string& message) const
    {
        std::size_t size = message.length() + 1;
        message_ptr_ = new char[size];
        std::fill(&message_ptr_[0], &message_ptr_[size], 0);
        std::copy(message.begin(), message.end(), message_ptr_);
        return message_ptr_;
    }

public:
    AbstractException(const std::string& message,
                      int32_t outward_error_code = 0)
      : runtime_error(message),
        message_ptr_(nullptr),
        outward_error_code_(outward_error_code)
    {
    }

    virtual ~AbstractException() throw()
    {
        if (nullptr != message_ptr_)
        {
            delete[] message_ptr_;
        }
    }

    int32_t get_outward_error_code() const
    {
        return outward_error_code_;
    }

    virtual const char* what(void) const throw()
    {
        std::stringstream ss;
        ss << std::runtime_error::what();
        return marshal(ss.str());
    }
};

/**
 * @brief Provides socket exception.
 */
class SocketException : public AbstractException
{
private:
    int32_t error_code_;

public:
    SocketException(const std::string& message, int32_t error_code,
                    int32_t outward_error_code = 0)
      : AbstractException(message, outward_error_code), error_code_(error_code)
    {
    }

    int32_t get_error_code(void) const
    {
        return error_code_;
    }

    virtual const char* what(void) const throw()
    {
        std::stringstream ss;
        ss << AbstractException::what();
        return marshal(ss.str());
    }
};

/**
 * @brief Provides invariant exception.
 */
class InvariantException : public AbstractException
{
public:
    InvariantException(const std::string& message,
                       int32_t outward_error_code = 0)
      : AbstractException(message, outward_error_code)
    {
    }
};

/**
 * @brief Provides callback exception.
 */
class CallbackException : public AbstractException
{
public:
    CallbackException(const std::string& message,
                      int32_t outward_error_code = 0)
      : AbstractException(message, outward_error_code)
    {
    }
};

/**
 * @brief Provides reject exception.
 */
class RejectException : public AbstractException
{
public:
    RejectException(const std::string& message, int32_t outward_error_code = 0)
      : AbstractException(message, outward_error_code)
    {
    }
};

/**
 * @brief Provides failure exception.
 */
class FailureException : public AbstractException
{
public:
    FailureException(const std::string& message, int32_t outward_error_code = 0)
      : AbstractException(message, outward_error_code)
    {
    }
};

/**
 * @brief Provides file exception.
 */
class FileException : public AbstractException
{
public:
    FileException(const std::string& message, int32_t outward_error_code = 0)
      : AbstractException(message, outward_error_code)
    {
    }
};

/**
 * @brief Provides invalid param exception.
 */
class InvParamException : public AbstractException
{
public:
    InvParamException(const std::string& message, int32_t outward_error_code = 0)
      : AbstractException(message, outward_error_code)
    {
    }
};

} /* namespace stdsc */

#endif /* STDSC_EXCEPTION_HPP */
