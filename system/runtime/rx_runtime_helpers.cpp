

/****************************************************************************
*
*  system\runtime\rx_runtime_helpers.cpp
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


// rx_ns
#include "system/server/rx_ns.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_operational
#include "system/runtime/rx_operational.h"
// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_runtime_holder
#include "system/runtime/rx_runtime_holder.h"
// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"

#include "rx_configuration.h"


namespace rx_platform {

namespace runtime {

// Class rx_platform::runtime::io_capabilities 


void io_capabilities::set_input (bool val)
{
    settings_[0] = val;
}

void io_capabilities::set_output (bool val)
{
    settings_[1] = val;
}

bool io_capabilities::get_input () const
{
    return settings_.test(0);
}

bool io_capabilities::get_output () const
{
    return settings_.test(1);
}


// Class rx_platform::runtime::runtime_deinit_context 


// Class rx_platform::runtime::runtime_init_context 

runtime_init_context::runtime_init_context (structure::runtime_item& root, const meta::meta_data& meta, runtime_process_context* context, operational::binded_tags* binded, ns::rx_directory_resolver* directories)
      : context(context),
        next_handle_(0x80000000),
        meta(meta),
        directories(directories),
        now(rx_time::now())
    , structure(root)
    , tags(binded)
{
}



runtime_handle_t runtime_init_context::get_new_handle ()
{
	return next_handle_++;
}

rx_result_with<runtime_handle_t> runtime_init_context::bind_item (const string_type& path)
{
	return tags->bind_item(path, *this);
}

rx_result runtime_init_context::set_item (const string_type& path, rx_simple_value&& value)
{
	return tags->set_item(path, std::move(value), *this);
}


// Class rx_platform::runtime::runtime_path_resolver 


void runtime_path_resolver::push_to_path (const string_type& name)
{
	if (!path_.empty())
		path_ += RX_OBJECT_DELIMETER;
	path_ += name;
}

void runtime_path_resolver::pop_from_path ()
{
	RX_ASSERT(!path_.empty());
	if (!path_.empty())
	{
		size_t idx = path_.rfind(RX_OBJECT_DELIMETER);
		if (idx == string_type::npos)
		{
			path_.clear();
		}
		else
		{
			path_.resize(idx);
		}
	}
}

const string_type& runtime_path_resolver::get_current_path () const
{
	return path_;
}

string_type runtime_path_resolver::get_parent_path (size_t level) const
{
	if (!path_.empty() && level > 0)
	{
		size_t idx = path_.rfind(RX_OBJECT_DELIMETER);
		if (idx == string_type::npos)
		{
			return "";
		}
		else
		{
			string_type ret(path_.substr(idx));
			level--;
			size_t off_idx = string_type::npos;
			while (!ret.empty() && level > 0 && off_idx > 0)
			{
				off_idx = path_.rfind(RX_OBJECT_DELIMETER, off_idx - 1);
				if (off_idx == string_type::npos)
					return "";
				else
					ret.resize(off_idx);
			}
			return ret;
		}
	}
	return "";
}


// Class rx_platform::runtime::runtime_start_context 

runtime_start_context::runtime_start_context (structure::runtime_item& root, runtime_process_context* context, ns::rx_directory_resolver* directories)
      : context(context),
        directories(directories),
        now(rx_time::now())
    , structure(root)
{
}



runtime_handle_t runtime_start_context::connect (const string_type& path, uint32_t rate, std::function<void(const values::rx_value&)> callback)
{
	return context->connect(path, rate, callback, *this);
}


// Class rx_platform::runtime::runtime_stop_context 


// Class rx_platform::runtime::runtime_structure_resolver 

runtime_structure_resolver::runtime_structure_resolver (structure::runtime_item& root)
      : root_(root)
{
}



void runtime_structure_resolver::push_item (structure::runtime_item& item)
{
	items_.push(item);
}

void runtime_structure_resolver::pop_item ()
{
	RX_ASSERT(!items_.empty());
	if (!items_.empty())
		items_.pop();
}

structure::runtime_item& runtime_structure_resolver::get_current_item ()
{
	RX_ASSERT(!items_.empty());
	static structure::empty_item_data g_empty;
	if (!items_.empty())
		return items_.top();
	else
		return g_empty;
}

structure::runtime_item& runtime_structure_resolver::get_root ()
{
    return root_;
}


// Class rx_platform::runtime::variables_stack 


void variables_stack::push_variable (structure::variable_data* what)
{
	variables_.push(what);
}

void variables_stack::pop_variable ()
{
	if (!variables_.empty())
		variables_.pop();
}

structure::variable_data* variables_stack::get_current_variable () const
{
	if (!variables_.empty())
		return variables_.top();
	else
		return nullptr;
}


// Class rx_platform::runtime::mappers_stack 


void mappers_stack::push_mapper (const rx_node_id& id, structure::mapper_data* what)
{
	auto it = mappers_.find(id);
	if (it != mappers_.end())
	{
		it->second.push_back(what);
	}
	else
	{
		auto result = mappers_.emplace(id, mappers_type::mapped_type());
		result.first->second.push_back(what);
	}
}

void mappers_stack::pop_mapper (const rx_node_id& id)
{
	auto it = mappers_.find(id);
	if (it != mappers_.end())
	{
		it->second.pop_back();
	}
	else
	{
		RX_ASSERT(false);
	}
}

std::vector<rx_value> mappers_stack::get_mapped_values (const rx_node_id& id, const string_type& path)
{
	std::vector<rx_value> ret_value;
	auto it = mappers_.find(id);
	if (it != mappers_.end() && !it->second.empty())
	{
		for (auto& one : it->second)
		{
			rx_simple_value val;
			if (one->item->get_local_value(path, val))
				ret_value.emplace_back(rx_value::from_simple(std::move(val), rx_time::now()));
		}
	}
	return ret_value;
}


} // namespace runtime
} // namespace rx_platform

