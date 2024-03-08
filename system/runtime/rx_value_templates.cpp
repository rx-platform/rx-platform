

/****************************************************************************
*
*  system\runtime\rx_value_templates.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#include "pch.h"


// rx_value_templates
#include "system/runtime/rx_value_templates.h"


namespace rx_platform
{
namespace runtime
{

void owned_complex_value::internal_commit()
{
    if (ctx_ && handle_)// just in case both of them...
    {
        ctx_->set_value(handle_, rx_simple_value(val_));
    }
}
rx_result owned_complex_value::bind(const string_type& path, runtime_init_context& ctx)
{
    auto result = ctx.bind_item(path, runtime::tag_blocks::binded_callback_t());
    if (result)
    {
        ctx_ = ctx.context;
        handle_ = result.move_value();
        if (handle_)
        {
            auto ret = ctx.context->get_value(handle_, val_);
        }
        return true;
    }
    else
    {
        return result.errors();
    }
}
owned_complex_value::owned_complex_value(const rx_simple_value& right)
{
    val_ = right;
}
owned_complex_value::owned_complex_value(rx_simple_value&& right)
{
    val_ = std::move(right);
}
rx_simple_value& owned_complex_value::value()
{
    return val_;
}

void owned_complex_value::commit(const rx_simple_value& val)
{
    val_ = val;
    internal_commit();
}
void commit(rx_simple_value&& val)
{

}
void owned_complex_value::owned_complex_value::commit(rx_simple_value&& val)
{
    val_ = std::move(val);
    internal_commit();
}

rx_result local_complex_value::bind(const string_type& path, runtime_init_context& ctx, callback_t callback)
{
    callback_ = callback;
    auto result = ctx.bind_item(path, [this](const rx_value& val)
        {

            if (ctx_->is_mine_value(val))
            {
                if (val.is_good())
                {
                    value_ = val.to_simple();
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
        auto result = ctx_->get_value(handle_, value_);
        if (!result)
            value_ = bad_value_;

        return result;
    }
    else
    {
        return result.errors();
    }
}
local_complex_value::local_complex_value(const rx_simple_value& right)
{
    bad_value_ = right;
    value_ = right;
}
local_complex_value::local_complex_value(rx_simple_value&& right)
{
    bad_value_ = right;
    value_ = std::move(right);
}
local_complex_value::local_complex_value(const rx_simple_value& right, const rx_simple_value& bad_value)
{
    bad_value_ = bad_value;
    value_ = right;
}
local_complex_value::local_complex_value(rx_simple_value&& right, rx_simple_value&& bad_value)
{
    value_ = std::move(right);
    bad_value_ = std::move(bad_value);
}
const rx_simple_value& local_complex_value::value() const
{
    return value_;
}


} // namespace runtime
} // namespace rx_platform




