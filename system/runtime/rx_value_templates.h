

/****************************************************************************
*
*  system\runtime\rx_value_templates.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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
struct connected_value
{
    using callback_t = std::function<void(const typeT&, bool valid)>;
    callback_t callback_;
    typeT value_;
    typeT bad_value_;
    bool valid_ = false;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;
public:
    connected_value() = default;
    ~connected_value() = default;
    connected_value(const connected_value&) = default;
    connected_value(connected_value&&) = default;
    connected_value& operator=(const connected_value&) = default;
    connected_value& operator=(connected_value&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx, callback_t callback = callback_t())
    {
        callback_ = callback;
        auto result = ctx.connect_item(path, [this](const rx_value& val)
            {
                if (ctx_->is_mine_value(val))
                {
                    if (val.is_good())
                    {
                        value_ = val.extract_static(value_);
                        valid_ = true;
                    }
                    else
                    {
                        if (value_ == bad_value_)
                            return; // we already had it!, skip the callback

                        value_ = bad_value_;
                        valid_ = false;
                    }
                    if (callback_)
                    {
                        callback_(value_, valid_);
                    }
                }
            });
        if (result)
        {
            ctx_ = ctx.context;
            handle_ = result.move_value();
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    connected_value(const typeT& right)
    {
        bad_value_ = right;
        value_ = right;
    }
    connected_value(typeT&& right)
    {
        bad_value_ = right;
        value_ = std::move(right);
    }
    connected_value(const typeT& right, const typeT& bad_value)
    {
        bad_value_ = bad_value;
        value_ = right;
    }
    connected_value(typeT&& right, typeT&& bad_value)
    {
        value_ = std::move(right);
        bad_value_ = std::move(bad_value);
    }
    connected_value& operator=(typeT right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            values::rx_simple_value temp_val;
            temp_val.assign_static<typeT>(std::forward<typeT>(right));
            ctx_->write_connected(handle_, std::move(temp_val), 1);
        }
        return *this;
    }
    operator typeT() const
    {
        return value_;
    }
};


template <typename ...OutArgs>
struct connected_function
{
    template < typename Type>
    Type from_arg(const rx_simple_value& what)
    {
        return what.extract_static<Type>(0);
    }
    bool valid_ = false;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;
    std::map<runtime_transaction_id_t, typename std::function<void(rx_result, OutArgs...)> > transactions_;
public:
    connected_function() = default;
    ~connected_function() = default;
    connected_function(const connected_function&) = delete;
    connected_function(connected_function&&) = default;
    connected_function& operator=(const connected_function&) = delete;
    connected_function& operator=(connected_function&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx)
    {
        tag_blocks::binded_execute_result_callback_t callback = [](void* target, struct typed_value_type output, runtime_transaction_id_t trans_id, rx_result_struct result)
            {
                connected_function* self = (connected_function*)target;
                auto it = self->transactions_.find(trans_id);
                if (it != self->transactions_.end())
                {
                    self->callback_execute(std::move(it->second), rx_result(result), rx_simple_value(std::move(output)));
                    self->transactions_.erase(it);
                }
            };

        auto result = ctx.connect_item(path, 0, tag_blocks::binded_callback_t()
            , tag_blocks::binded_write_result_callback_t(), callback);
        if (result)
        {
            ctx_ = ctx.context;
            handle_ = result.move_value();
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    template<typename funcT, std::size_t... I>
    void invoke_helper(runtime_transaction_id_t id, std::vector<rx_simple_value> inputs,
        std::index_sequence<I...>)
    {
        execute(id, from_arg<OutArgs>(std::move(inputs.at(I)))...);
    }
    template<typename funcT>
    rx_result callback_execute(funcT callback, rx_result result, rx_simple_value args)
    {
        if (args.is_struct())
        {
            std::vector<rx_simple_value> inputs;
            inputs.reserve(args.struct_size());
            for (size_t i = 0; i < args.struct_size(); i++)
            {
                inputs.push_back(args[(int)i]);
            }
            invoke_helper(std::move(callback), std::move(result), std::move(inputs), std::index_sequence_for<OutArgs...>{});
            return true;
        }
        else
        {
            return RX_INVALID_ARGUMENT;
        }
    }
    template<typename funcT, typename ...InArgs>
    rx_result operator()(InArgs... in_args, funcT callback)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            values::rx_simple_value temp_val = rx_create_value_static(std::forward<InArgs>(in_args)...);
            ctx_->execute_connected(handle_, std::move(temp_val),
                [this, callback = std::move(callback)](rx_result result, rx_simple_value data)
                {
                    callback_execute(callback, data);
                });
        }
        return *this;
    }
};

template <typename typeT>
struct local_value
{
    using callback_t = std::function<void(const typeT&)>;
    callback_t callback_;
    typeT bad_value_;
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

                if (ctx_->is_mine_value(val))
                {
                    if (val.is_good())
                    {
                        value_ = val.extract_static(value_);
                    }
                    else
                    {
                        if (value_ == bad_value_)
                            return; // we already had it!, skip the callback

                        value_ = bad_value_;
                    }
                    if (callback_)
                    {
                        callback_(value_);
                    }
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
        bad_value_ = right;
        value_ = right;
    }
    local_value(typeT&& right)
    {
        bad_value_ = right;
        value_ = std::move(right);
    }
    local_value(const typeT& right, const typeT& bad_value)
    {
        bad_value_ = bad_value;
        value_ = right;
    }
    local_value(typeT&& right, typeT&& bad_value)
    {
        value_ = std::move(right);
        bad_value_ = std::move(bad_value);
    }
    rx_result write(typeT right, tag_blocks::binded_write_result_callback_t callback)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            values::rx_simple_value temp_val;
            temp_val.assign_static<typeT>(std::forward<typeT>(right));
            ctx_->set_value(handle_, std::move(temp_val), callback);
        }
        return *this;
    }
    local_value& operator=(typeT right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            values::rx_simple_value temp_val;
            temp_val.assign_static<typeT>(std::forward<typeT>(right));
            ctx_->set_value(handle_, std::move(temp_val), tag_blocks::binded_write_result_callback_t());
        }
        return *this;
    }
    operator typeT() const
    {
        return value_;
    }
};



template <typename ...OutArgs>
struct local_function
{
    template < typename Type>
    Type from_arg(const rx_simple_value& what)
    {
        return what.extract_static<Type>(0);
    }
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;
public:
    local_function() = default;
    ~local_function() = default;
    local_function(const local_function&) = default;
    local_function(local_function&&) = default;
    local_function& operator=(const local_function&) = delete;
    local_function& operator=(local_function&&) = delete;
    rx_result bind(const string_type& path, runtime_init_context& ctx)
    {
        auto result = ctx.bind_item(path, tag_blocks::binded_callback_t());
        if (result)
        {
            ctx_ = ctx.context;
            handle_ = result.move_value();
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    template<typename funcT, std::size_t... I>
    void invoke_helper(runtime_transaction_id_t id, std::vector<rx_simple_value> inputs,
        std::index_sequence<I...>)
    {
        execute(id, from_arg<OutArgs>(std::move(inputs.at(I)))...);
    }
    template<typename funcT>
    rx_result callback_execute(funcT callback, rx_result result, rx_simple_value args)
    {
        if (args.is_struct())
        {
            std::vector<rx_simple_value> inputs;
            inputs.reserve(args.struct_size());
            for (size_t i = 0; i < args.struct_size(); i++)
            {
                inputs.push_back(args[(int)i]);
            }
            invoke_helper(std::move(callback), std::move(result), std::move(inputs), std::index_sequence_for<OutArgs...>{});
            return true;
        }
        else
        {
            return RX_INVALID_ARGUMENT;
        }
    }

    template<typename funcT, typename ...InArgs>
    rx_result execute(InArgs... in_args, funcT callback)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            values::rx_simple_value temp_val = rx_create_value_static(std::forward<InArgs>(in_args)...);
            ctx_->execute_connected(handle_, std::move(temp_val),
                [this, callback = std::move(callback)](rx_result result, rx_simple_value data)
                {
                    callback_execute(callback, data);
                });
        }
        return *this;
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
            if (handle_)
                internal_commit();
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


struct owned_complex_value
{
    rx_simple_value val_;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;

    void internal_commit();
public:
    owned_complex_value() = default;
    ~owned_complex_value() = default;
    owned_complex_value(const owned_complex_value&) = default;
    owned_complex_value(owned_complex_value&&) = default;
    owned_complex_value& operator=(const owned_complex_value&) = default;
    owned_complex_value& operator=(owned_complex_value&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx);
    owned_complex_value(const rx_simple_value& right);
    owned_complex_value(rx_simple_value&& right);
    rx_simple_value& value();
    void commit(const rx_simple_value& val);
    void commit(rx_simple_value&& val);
};

struct local_complex_value
{
    using callback_t = std::function<void(const rx_simple_value&)>;
    callback_t callback_;
    rx_simple_value bad_value_;
    rx_simple_value value_;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;
public:
    local_complex_value() = default;
    ~local_complex_value() = default;
    local_complex_value(const local_complex_value&) = default;
    local_complex_value(local_complex_value&&) = default;
    local_complex_value& operator=(const local_complex_value&) = default;
    local_complex_value& operator=(local_complex_value&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx, callback_t callback = callback_t());
    local_complex_value(const rx_simple_value& right);
    local_complex_value(rx_simple_value&& right);
    local_complex_value(const rx_simple_value& right, const rx_simple_value& bad_value);
    local_complex_value(rx_simple_value&& right, rx_simple_value&& bad_value);
    const rx_simple_value& value() const;
};

template <typename typeT, bool manual = false>
struct async_owned_value
{
    typeT val_;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;

    void internal_commit()
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            typeT temp(val_);
            ctx_->set_async_binded_as<typeT>(handle_, std::move(temp), tag_blocks::binded_write_result_callback_t());
        }
    }
public:
    async_owned_value() = default;
    ~async_owned_value() = default;
    async_owned_value(const async_owned_value&) = default;
    async_owned_value(async_owned_value&&) = default;
    async_owned_value& operator=(const async_owned_value&) = default;
    async_owned_value& operator=(async_owned_value&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx)
    {
        auto result = ctx.bind_item(path, runtime::tag_blocks::binded_callback_t());
        if (result)
        {
            ctx_ = ctx.context;
            handle_ = result.move_value();
            if (handle_)
                internal_commit();
            return true;
        }
        else
        {
            return result.errors();
        }
    }
    async_owned_value(const typeT& right)
    {
        val_ = right;
    }
    async_owned_value(typeT&& right)
    {
        val_ = std::move(right);
    }
    async_owned_value& operator=(const typeT& right)
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
    async_owned_value& operator=(typeT&& right)
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
    async_owned_value& operator+=(const typeT& right)
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
    async_owned_value& operator-=(const typeT& right)
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
struct async_local_value
{
    using callback_t = std::function<void(const typeT&)>;
    callback_t callback_;
    lockT lock_;
    typeT bad_value_;
    typeT value_;
    runtime_handle_t handle_ = 0;
    runtime_process_context* ctx_ = nullptr;
public:
    async_local_value() = default;
    ~async_local_value() = default;
    async_local_value(const async_local_value&) = delete;
    async_local_value(async_local_value&&) = default;
    async_local_value& operator=(const async_local_value&) = delete;
    async_local_value& operator=(async_local_value&&) = default;
    rx_result bind(const string_type& path, runtime_init_context& ctx, callback_t callback = callback_t())
    {
        auto result = ctx.bind_item(path, [this](const rx_value& val)
            {
                if (ctx_->is_mine_value(val))
                {
                    typeT temp_val;
                    if (val.is_good())
                    {
                        locks::auto_lock_t<lockT> _(&lock_);
                        value_ = val.extract_static(value_);
                        temp_val = value_;
                    }
                    else
                    {
                        locks::auto_lock_t<lockT> _(&lock_);
                        if (value_ == bad_value_)
                            return; // we already had it!, skip the callback
                        value_ = bad_value_;
                        temp_val = value_;

                    }
                    if (callback_)
                    {
                        callback_(temp_val);
                    }
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

    async_local_value(const typeT& right)
    {
        bad_value_ = right;
        value_ = right;
    }
    async_local_value(typeT&& right)
    {
        bad_value_ = right;
        value_ = std::move(right);
    }
    async_local_value(const typeT& right, const typeT& bad_value)
    {

        bad_value_ = bad_value;
        value_ = right;
    }
    async_local_value(typeT&& right, typeT&& bad_value)
    {
        bad_value_ = std::move(bad_value);
        value_ = std::move(right);
    }
    async_local_value& operator=(typeT right)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            typeT temp(right);
            ctx_->set_async_binded_as<typeT>(handle_, std::move(temp), tag_blocks::binded_write_result_callback_t());
        }
        return this;
    }
    rx_result write(typeT right, tag_blocks::binded_write_result_callback_t callback)
    {
        if (ctx_ && handle_)// just in case both of them...
        {
            typeT temp(right);
            ctx_->set_async_binded_as<typeT>(handle_, std::move(temp), std::move(callback));
            return true;
        }
        return RX_NOT_CONNECTED;
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
