

/****************************************************************************
*
*  system\runtime\rx_operational.cpp
*
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_operational
#include "system/runtime/rx_operational.h"

#include "system/runtime/rx_blocks.h"


namespace rx_platform {

namespace runtime {

namespace operational {

// Class rx_platform::runtime::operational::rx_write_task_callback 


// Class rx_platform::runtime::operational::rx_execute_task 


// Class rx_platform::runtime::operational::rx_read_task 


// Class rx_platform::runtime::operational::connected_tags 

connected_tags::connected_tags()
{
}


connected_tags::~connected_tags()
{
}



runtime_handle_t connected_tags::get_new_handle ()
{
	static std::atomic<runtime_handle_t> g_handle(1);
	runtime_handle_t ret = g_handle++;
	if (!ret)// avoid zero
		return get_new_handle();
	else
		return ret;
}

rx_result_with<runtime_handle_t> connected_tags::connect_tag (const string_type& path, blocks::runtime_holder* item)
{
	locks::auto_lock_t<decltype(lock_)> _(&lock_);
	auto it_tags = referenced_tags_.find(path);
	if (it_tags != referenced_tags_.end())
	{// not new one, just add reference
		auto it_handles = handles_map_.find(it_tags->second);
		if (it_handles != handles_map_.end())
		{
			it_handles->second.reference_count++;
			return it_tags->second;
		}
		else
		{
			RX_ASSERT(false);
			return "Internal error";
		}
	}
	else
	{// new one, connect item
		rt_value_ref ref;
		auto ref_result = item->get_value_ref(path, ref);
		if (!ref_result)
			return ref_result.errors();
		// fill out the data
		auto handle = get_new_handle();
		switch (ref.ref_type)
		{
		case rt_value_ref_type::rt_const_value:
			const_values_.emplace(ref.ref_value_ptr.const_value, handle);
			break;
		case rt_value_ref_type::rt_value:
			values_.emplace(ref.ref_value_ptr.value, handle);
			break;
		case rt_value_ref_type::rt_variable:
			variables_.emplace(ref.ref_value_ptr.variable, handle);
			break;
		default:
			RX_ASSERT(false);
			return "Internal error";
		}
		handles_map_.emplace(handle, one_tag_data{ ref, 1 });
		referenced_tags_.emplace(path, handle);
		return handle;
	}
}

rx_result connected_tags::disconnect_tag (runtime_handle_t handle)
{
	if (!handle)
		return true;
	locks::auto_lock_t<decltype(lock_)> _(&lock_);
	auto it_handles = handles_map_.find(handle);
	if (it_handles == handles_map_.end() || it_handles->second.reference_count == 0)
		return "Invalid item handle";
	it_handles->second.reference_count--;
	return true;
}


// Class rx_platform::runtime::operational::binded_tags 

binded_tags::binded_tags()
{
}


binded_tags::~binded_tags()
{
}



rx_result_with<runtime_handle_t> binded_tags::bind_tag (const rt_value_ref& ref, runtime_handle_t handle)
{
	// fill out the data
	switch (ref.ref_type)
	{
	case rt_value_ref_type::rt_const_value:
		const_values_.emplace(ref.ref_value_ptr.const_value, handle);
		break;
	case rt_value_ref_type::rt_value:
		values_.emplace(ref.ref_value_ptr.value, handle);
		break;
	default:
		RX_ASSERT(false);
		return "Internal error";
	}
	handles_map_.emplace(handle, ref);
	return handle;
}

rx_result binded_tags::get_value (runtime_handle_t handle, rx_simple_value& val) const
{
	auto it_handles = handles_map_.find(handle);
	if (it_handles != handles_map_.end())
	{
		switch (it_handles->second.ref_type)
		{
		case rt_value_ref_type::rt_const_value:
			val = it_handles->second.ref_value_ptr.const_value->simple_get_value();
			return true;
		case rt_value_ref_type::rt_value:
			val = it_handles->second.ref_value_ptr.value->simple_get_value();
			return true;
		case rt_value_ref_type::rt_variable:
		default:
			RX_ASSERT(false);
			return "Internal error";
		}
	}
	else
	{
		return "Invalid Handle value!";
	}
}

rx_result binded_tags::set_value (runtime_handle_t handle, rx_simple_value&& val)
{
	auto it_handles = handles_map_.find(handle);
	if (it_handles != handles_map_.end())
	{
		switch (it_handles->second.ref_type)
		{
		case rt_value_ref_type::rt_const_value:
			return "Not supported for constant value!";
			return true;
		case rt_value_ref_type::rt_value:
			it_handles->second.ref_value_ptr.value->simple_set_value(std::move(val));
			return true;
		case rt_value_ref_type::rt_variable:
		default:
			RX_ASSERT(false);
			return "Internal error";
		}
	}
	else
	{
		return "Invalid Handle value!";
	}
}

rx_result_with<runtime_handle_t> binded_tags::bind_item (const string_type& path, runtime_init_context& ctx)
{
	auto it = ctx.binded_tags.find(path);
	if (it != ctx.binded_tags.end())
		return it->second;

	string_type revisied_path;
	rt_value_ref ref;
	ref.ref_type = rt_value_ref_type::rt_null;

	if (!path.empty())
	{
		switch (path[0])
		{
		case RX_PATH_CURRENT:
			{
				string_type full_path(ctx.path.get_current_path() + path);
				auto it = ctx.binded_tags.find(full_path);
				if (it != ctx.binded_tags.end())
					return it->second;

				auto ref_result = ctx.structure.get_current_item().get_value_ref(path, ref);
				if (!ref_result)
					return ref_result.errors();
			}
			break;
		case RX_PATH_PARENT:
			{
				size_t idx = 1;
				while (idx < path.size() && path[idx] == RX_PATH_PARENT)
					idx++;
				string_type full_path(ctx.path.get_parent_path(idx) + path);
				auto it = ctx.binded_tags.find(full_path);
				if (it != ctx.binded_tags.end())
					return it->second;

				auto ref_result = ctx.structure.get_current_item().get_value_ref(path, ref);
				if (!ref_result)
					return ref_result.errors();
			}
			break;
		}
	}
	if (ref.ref_type == rt_value_ref_type::rt_null)
	{
		auto ref_result = ctx.structure.get_root()->get_value_ref(path, ref);
		if (!ref_result)
			return ref_result.errors();
	}

	if (ref.ref_type == rt_value_ref_type::rt_variable)
		return "Can't bind to variable";
	// fill out the data
	auto handle = ctx.get_new_handle();
	auto bind_result = bind_tag(ref, handle);
	if (bind_result)
	{
		ctx.binded_tags.emplace(path, bind_result.value());
	}
	return bind_result;
}


} // namespace operational
} // namespace runtime
} // namespace rx_platform
