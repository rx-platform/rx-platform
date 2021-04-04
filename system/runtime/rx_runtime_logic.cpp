

/****************************************************************************
*
*  system\runtime\rx_runtime_logic.cpp
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


#include "pch.h"


// rx_runtime_logic
#include "system/runtime/rx_runtime_logic.h"



namespace rx_platform {

namespace runtime {

namespace logic_blocks {

// Class rx_platform::runtime::logic_blocks::logic_holder 


rx_result logic_holder::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx, bool& not_mine) const
{
    return RX_NOT_IMPLEMENTED;
}

rx_result logic_holder::initialize_logic (runtime::runtime_init_context& ctx)
{
    rx_result ret(true);
    for (auto& one : runtime_programs_)
    {
        ret = one.item->initialize_runtime(ctx);
        if(ret)
            ret = one.program_ptr->initialize_runtime(ctx);
        if (!ret)
            return ret;
    }
    for (auto& one : runtime_methods_)
    {
        ret = one.item->initialize_runtime(ctx);
        if (ret)
            ret = one.method_ptr->initialize_runtime(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

rx_result logic_holder::deinitialize_logic (runtime::runtime_deinit_context& ctx)
{
    rx_result ret(true);
    for (auto& one : runtime_programs_)
    {
        ret = one.item->deinitialize_runtime(ctx);
        if (ret)
            ret = one.program_ptr->deinitialize_runtime(ctx);
        if (!ret)
            return ret;
    }
    for (auto& one : runtime_methods_)
    {
        ret = one.item->deinitialize_runtime(ctx);
        if (ret)
            ret = one.method_ptr->deinitialize_runtime(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

rx_result logic_holder::start_logic (runtime::runtime_start_context& ctx)
{
    rx_result ret(true);
    for (auto& one : runtime_programs_)
    {
        ret = one.item->start_runtime(ctx);
        if (ret)
            ret = one.program_ptr->start_runtime(ctx);
        if (!ret)
            return ret;
    }
    for (auto& one : runtime_methods_)
    {
        ret = one.item->start_runtime(ctx);
        if (ret)
            ret = one.method_ptr->start_runtime(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

rx_result logic_holder::stop_logic (runtime::runtime_stop_context& ctx)
{
    rx_result ret(true);
    for (auto& one : runtime_programs_)
    {
        ret = one.item->stop_runtime(ctx);
        if (ret)
            ret = one.program_ptr->stop_runtime(ctx);
        if (!ret)
            return ret;
    }
    for (auto& one : runtime_methods_)
    {
        ret = one.item->stop_runtime(ctx);
        if (ret)
            ret = one.method_ptr->stop_runtime(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

void logic_holder::fill_data (const data::runtime_values_data& data, runtime_process_context* ctx)
{
}

void logic_holder::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
}

rx_result logic_holder::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, bool& not_mine)
{
    return RX_NOT_IMPLEMENTED;
}

bool logic_holder::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_array("programs", runtime_programs_.size()))
        return false;
    for (const auto& one : runtime_programs_)
    {
        ///!!!!! this is wrong stuff need to reconsider this serialization
        if (!one.program_ptr->save_program(stream, type))
            return false;
    }
    if (!stream.end_array())
        return false;
    
    return true;
}

bool logic_holder::deserialize (base_meta_reader& stream, uint8_t type)
{
    return true;
}


// Class rx_platform::runtime::logic_blocks::program_data 

program_data::program_data (structure::runtime_item::smart_ptr&& rt, program_runtime_ptr&& var)
    : program_ptr(std::move(var))
    , item(std::move(rt))
{
}



// Class rx_platform::runtime::logic_blocks::method_data 

method_data::method_data (structure::runtime_item::smart_ptr&& rt, method_runtime_ptr&& var)
    : method_ptr(std::move(var))
    , item(std::move(rt))
{
}



} // namespace logic_blocks
} // namespace runtime
} // namespace rx_platform

