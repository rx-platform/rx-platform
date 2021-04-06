

/****************************************************************************
*
*  system\runtime\rx_display_blocks.cpp
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


// rx_display_blocks
#include "system/runtime/rx_display_blocks.h"



namespace rx_platform {

namespace runtime {

namespace display_blocks {

// Class rx_platform::runtime::display_blocks::displays_holder 


rx_result displays_holder::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
	return RX_NOT_IMPLEMENTED;
}

rx_result displays_holder::initialize_displays (runtime::runtime_init_context& ctx)
{
    rx_result ret(true);
    for (auto& one : displays_)
    {
        ret = one.item->initialize_runtime(ctx);
        if (ret)
            ret = one.display_ptr->initialize_display(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

rx_result displays_holder::deinitialize_displays (runtime::runtime_deinit_context& ctx)
{
    rx_result ret(true);
    for (auto& one : displays_)
    {
        ret = one.item->deinitialize_runtime(ctx);
        if (ret)
            ret = one.display_ptr->deinitialize_display(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

rx_result displays_holder::start_displays (runtime::runtime_start_context& ctx)
{
    rx_result ret(true);
    for (auto& one : displays_)
    {
        ret = one.item->start_runtime(ctx);
        if (ret)
            ret = one.display_ptr->start_display(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

rx_result displays_holder::stop_displays (runtime::runtime_stop_context& ctx)
{
    rx_result ret(true);
    for (auto& one : displays_)
    {
        ret = one.item->stop_runtime(ctx);
        if (ret)
            ret = one.display_ptr->stop_display(ctx);
        if (!ret)
            return ret;
    }
    return ret;
}

void displays_holder::fill_data (const data::runtime_values_data& data, runtime_process_context* ctx)
{
    for (auto& one : displays_)
    {
        one.item->fill_data(data);
    }
}

void displays_holder::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    for (const auto& one : displays_)
    {
        data::runtime_values_data one_data;
        one.item->collect_data(one_data, type);
        data.add_child(one.name, std::move(one_data));
    }
}

rx_result displays_holder::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx)
{
    rx_result ret = true;
    if (!displays_.empty())
    {
        if (path.empty())
        {
            for (auto& one : displays_)
            {
                runtime_item_attribute args_attr;
                args_attr.name = one.name;
                args_attr.type = rx_attribute_type::display_attribute_type;
                args_attr.full_path = prefix + RX_OBJECT_DELIMETER + args_attr.name;
                items.emplace_back(std::move(args_attr));
            }
        }
        else
        {
            auto idx = path.find(RX_OBJECT_DELIMETER);
            string_type mine;
            string_type bellow;
            if (idx == string_type::npos)
            {
                mine = path;
            }
            else
            {
                mine = path.substr(0, idx);
                bellow = path.substr(idx + 1);
            }
            string_type new_prefix(prefix + RX_OBJECT_DELIMETER + mine);
            for (auto& one : displays_)
            {
                ret = one.item->browse_items(new_prefix, bellow, filter, items, ctx);
                if (!ret)
                    break;
            }
        }
    }
    return ret;
}

bool displays_holder::serialize (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool displays_holder::deserialize (base_meta_reader& stream, uint8_t type)
{
	return false;
}

bool displays_holder::is_this_yours (const string_type& path) const
{
    size_t idx = path.find(RX_OBJECT_DELIMETER);
    string_type name;
    if (idx == string_type::npos)
    {
        for (const auto& one : displays_)
        {
            if (one.name == path)
                return true;
        }
    }
    else
    {
        size_t len = idx;
        for (const auto& one : displays_)
        {
            if (one.name.size() == len && memcmp(one.name.c_str(), path.c_str(), len) == 0)
                return true;
        }
    }
    return false;
}

rx_result displays_holder::get_value_ref (const string_type& path, rt_value_ref& ref)
{
    size_t idx = path.find(RX_OBJECT_DELIMETER);
    string_type name;
    if (idx != string_type::npos)
    {
        size_t len = idx;
        for (const auto& one : displays_)
        {
            if (one.name.size() == len && memcmp(one.name.c_str(), path.c_str(), len) == 0)
            {
                return one.item->get_value_ref(&path.c_str()[idx + 1], ref);
            }
        }
    }
    return path + " not found!";
}


// Class rx_platform::runtime::display_blocks::display_data 

display_data::display_data (structure::runtime_item::smart_ptr&& rt, display_runtime_ptr&& var, const display_data& prototype)
{
}



} // namespace display_blocks
} // namespace runtime
} // namespace rx_platform

