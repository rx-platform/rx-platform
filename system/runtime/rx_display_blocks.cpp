

/****************************************************************************
*
*  system\runtime\rx_display_blocks.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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

#include "system/server/rx_log.h"

// rx_display_blocks
#include "system/runtime/rx_display_blocks.h"



namespace rx_platform {

namespace runtime {

namespace display_blocks {

// Class rx_platform::runtime::display_blocks::displays_holder 


rx_result displays_holder::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
    size_t idx = path.find(RX_OBJECT_DELIMETER);
    string_view_type name = path.substr(0, idx);
    if (idx != string_type::npos)
    {
        for (auto& one : displays_)
        {
            if (one.name == name)
            {
                return one.get_value(path.substr(idx + 1), val, ctx);
            }
        }
    }
    return RX_INVALID_PATH;
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
        auto it = data.children.find(one.name);
        if (it != data.children.end() && std::holds_alternative<data::runtime_values_data>(it->second))
        {
            one.fill_data(std::get< data::runtime_values_data>(it->second));
            break;
        }
    }
}

void displays_holder::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    for (const auto& one : displays_)
    {
        data::runtime_values_data one_data;
        one.collect_data(one_data, type);
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

bool displays_holder::is_this_yours (string_view_type path) const
{
    size_t idx = path.find(RX_OBJECT_DELIMETER);
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
        string_view_type name = path.substr(0, idx);
        for (const auto& one : displays_)
        {
            if (one.name == name)
                return true;
        }
    }
    return false;
}

rx_result displays_holder::get_value_ref (string_view_type path, rt_value_ref& ref)
{
    size_t idx = path.find(RX_OBJECT_DELIMETER);
    string_view_type name;
    if (idx != string_type::npos)
    {
        size_t len = idx;
        for (auto& one : displays_)
        {
            if (one.name == name)
            {
                return one.get_value_ref(path.substr(idx+1), ref);
            }
        }
    }
    return "Invalid path";
}

rx_result displays_holder::get_struct_value (string_view_type item, string_view_type path, data::runtime_values_data& data, runtime_value_type type, runtime_process_context* ctx) const
{
    const structure::runtime_item* item_ptr = nullptr;
    for (const auto& one : displays_)
    {
        if (one.name == item)
        {
            item_ptr = one.item->get_child_item(path);
            break;
        }
    }
    if (item_ptr)
    {
        item_ptr->collect_data(data, type);
        return true;
    }
    else
    {
        return "Invalid path";
    }
}

void displays_holder::set_displays (std::vector<display_data> data)
{
    displays_ = const_size_vector<display_data>(std::move(data));
}


// Class rx_platform::runtime::display_blocks::display_data 

display_data::display_data (structure::runtime_item::smart_ptr&& rt, display_runtime_ptr&& var, const display_data& prototype)
{
}



void display_data::fill_data (const data::runtime_values_data& data)
{
    item->fill_data(data);
}

void display_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    item->collect_data(data, type);
}

rx_result display_data::browse_items (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx)
{
    return item->browse_items(prefix, path, filter, items, ctx);
}

rx_result display_data::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
    return item->get_value(path, val, ctx);
}

rx_result display_data::get_value_ref (string_view_type path, rt_value_ref& ref)
{
    return item->get_value_ref(path, ref, false);
}

rx_result display_data::get_local_value (const string_type& path, rx_simple_value& val) const
{
    return item->get_local_value(path, val);
}


} // namespace display_blocks
} // namespace runtime
} // namespace rx_platform

