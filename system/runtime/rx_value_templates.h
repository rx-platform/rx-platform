

/****************************************************************************
*
*  system\runtime\rx_value_templates.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*
*  This file is part of {rx-platform}
*
*
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/


#ifndef rx_value_templates_h
#define rx_value_templates_h 1




#include "rx_process_context.h"

namespace rx_platform
{
namespace runtime
{

template <typename typeT>
struct local_value
{
    using callback_t = std::function<void(const typeT&)>;
    callback_t callback_;
    typeT value_;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;
public:
    local_value() = default;
    ~local_value() = default;
    local_value(const local_value&) = default;
    local_value(local_value&&) = default;
    local_value& operator=(const local_value&) = default;
    local_value& operator=(local_value&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx, callback_t callback = callback_t())
    {
        callback_ = callback;
        auto result = ctx.bind_item(path, [this](const rx_value& val)
            {
                value_ = values::extract_value(val.get_storage(), value_);
                if (callback_)
                {
                    callback_(value_);
                }
            });
        if (result)
        {
            ctx_ = ctx.context;
            handle_ = result.move_value();
            value_ = ctx_->get_binded_as(handle_, value_);
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    local_value(const typeT& right)
    {
        value_ = right;
    }
    local_value(typeT&& right)
    {
        value_ = std::move(right);
    }
    local_value& operator=(typeT right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            values::rx_simple_value temp_val;
            temp_val.assign_static<typeT>(std::forward<typeT>(right));
            ctx_->set_value(handle_, std::move(right));
        }
        return this;
    }
    operator typeT() const
    {
        return value_;
    }
};
template <typename typeT, bool manual = false>
struct owned_value
{
    typeT val_;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;

    void internal_commit()
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            typeT temp(val_);
            ctx_->set_binded_as<typeT>(handle_, std::move(temp));
        }
    }
public:
    owned_value() = default;
    ~owned_value() = default;
    owned_value(const owned_value&) = default;
    owned_value(owned_value&&) = default;
    owned_value& operator=(const owned_value&) = default;
    owned_value& operator=(owned_value&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx)
    {
        auto result = ctx.bind_item(path, runtime::tag_blocks::binded_callback_t());
        if (result)
        {
            ctx_ = ctx.context;
            handle_ = result.move_value();
            operator=(val_);
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    owned_value(const typeT& right)
    {
        val_ = right;
    }
    owned_value(typeT&& right)
    {
        val_ = std::move(right);
    }
    owned_value& operator=(const typeT& right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = right;
                if constexpr (!manual)
                {
                    internal_commit();
                }
            }
        }
        return *this;
    }
    owned_value& operator=(typeT&& right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = std::move(right);
                if constexpr (!manual)
                {
                    internal_commit();
                }
            }
        }
        return *this;
    }
    owned_value& operator+=(const typeT& right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            val_ += right;
            if constexpr (!manual)
            {
                internal_commit();
            }
        }
        return *this;
    }
    owned_value& operator-=(const typeT& right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            val_ -= right;
            if constexpr (!manual)
            {
                internal_commit();
            }
        }
        return *this;
    }
    operator typeT() const
    {
        return val_;
    }
    void commit()
    {
        if constexpr (manual)
        {
            internal_commit();
        }
        else
        {
            RX_ASSERT(false);
        }
    }
};

template <typename typeT, bool manual = false>
struct remote_owned_value
{
    typeT val_;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;

    void internal_commit()
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            typeT temp(val_);
            ctx_->set_remote_binded_as<typeT>(handle_, std::move(temp));
        }
    }
public:
    remote_owned_value() = default;
    ~remote_owned_value() = default;
    remote_owned_value(const remote_owned_value&) = default;
    remote_owned_value(remote_owned_value&&) = default;
    remote_owned_value& operator=(const remote_owned_value&) = default;
    remote_owned_value& operator=(remote_owned_value&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx)
    {
        auto result = ctx.bind_item(path, runtime::tag_blocks::binded_callback_t());
        if (result)
        {
            ctx_ = ctx.context;
            handle_ = result.move_value();
            operator=(val_);
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    remote_owned_value(const typeT& right)
    {
        val_ = right;
    }
    remote_owned_value(typeT&& right)
    {
        val_ = std::move(right);
    }
    remote_owned_value& operator=(const typeT& right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = right;
                if constexpr (!manual)
                {
                    internal_commit();
                }
            }
        }
        return *this;
    }
    remote_owned_value& operator=(typeT&& right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            if (val_ != right)
            {
                val_ = std::move(right);
                if constexpr (!manual)
                {
                    internal_commit();
                }
            }
        }
        return *this;
    }
    remote_owned_value& operator+=(const typeT& right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            val_ += right;
            if constexpr (!manual)
            {
                internal_commit();
            }
        }
        return *this;
    }
    remote_owned_value& operator-=(const typeT& right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            val_ -= right;
            if constexpr (!manual)
            {
                internal_commit();
            }
        }
        return *this;
    }
    operator typeT() const
    {
        return val_;
    }
    void commit()
    {
        if constexpr (manual)
        {
            internal_commit();
        }
        else
        {
            RX_ASSERT(false);
        }
    }
};


template <typename typeT, typename lockT = locks::slim_lock>
struct remote_local_value
{
    using callback_t = std::function<void(const typeT&)>;
    callback_t callback_;
    lockT lock_;
    typeT value_;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;
public:
    remote_local_value() = default;
    ~remote_local_value() = default;
    remote_local_value(const remote_local_value&) = delete;
    remote_local_value(remote_local_value&&) = default;
    remote_local_value& operator=(const remote_local_value&) = delete;
    remote_local_value& operator=(remote_local_value&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx, callback_t callback = callback_t())
    {
        auto result = ctx.bind_item(path, [this](const rx_value& val)
            {
                typeT temp_val;
                {
                    locks::auto_lock_t<lockT> _(&lock_);
                    value_ = values::extract_value(val.get_storage(), value_);
                    temp_val = value_;
                }
                if (callback_)
                {
                    callback_(temp_val);
                }
            });
        if (result)
        {
            ctx_ = ctx.context;
            handle_ = result.move_value();
            value_ = ctx_->get_binded_as(handle_, value_);
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    remote_local_value(const typeT& right)
    {
        value_ = right;
    }
    remote_local_value(typeT&& right)
    {
        value_ = std::move(right);
    }
    remote_local_value& operator=(typeT right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            typeT temp(right);
            ctx_->set_remote_binded_as<typeT>(handle_, std::move(temp));
        }
        return this;
    }
    operator typeT() const
    {
        locks::const_auto_lock_t<lockT> _(&lock_);
        return value_;
    }
};

} // namespace runtime
} // namespace rx_platform




#endif
