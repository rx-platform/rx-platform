

/****************************************************************************
*
*  system\runtime\rx_operational.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
// rx_relations
#include "system/runtime/rx_relations.h"
// rx_operational
#include "system/runtime/rx_operational.h"

#include "system/runtime/rx_blocks.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "sys_internal/rx_async_functions.h"


namespace rx_platform {

namespace runtime {

namespace operational {

// Class rx_platform::runtime::operational::rx_tags_callback 


// Class rx_platform::runtime::operational::connected_tags 

connected_tags::connected_tags()
{
}


connected_tags::~connected_tags()
{
}



rx_result_with<runtime_handle_t> connected_tags::connect_tag (const string_type& path, structure::runtime_item& item, tags_callback_ptr monitor, const structure::hosting_object_data& state)
{
	auto it_tags = referenced_tags_.find(path);
	if (it_tags != referenced_tags_.end())
	{// not new one, just add reference
		auto it_handles = handles_map_.find(it_tags->second);
		if (it_handles != handles_map_.end())
		{
			it_handles->second.reference_count++;
			it_handles->second.monitors.emplace(monitor);
			switch (it_handles->second.reference.ref_type)
			{
			case rt_value_ref_type::rt_const_value:
				next_send_[monitor].emplace(it_tags->second, it_handles->second.reference.ref_value_ptr.const_value->get_value(state));
				break;
			case rt_value_ref_type::rt_value:
				next_send_[monitor].emplace(it_tags->second, it_handles->second.reference.ref_value_ptr.value->get_value(state));
				break;
			case rt_value_ref_type::rt_variable:
				next_send_[monitor].emplace(it_tags->second, it_handles->second.reference.ref_value_ptr.variable->get_value(state));
				break;
			case rt_value_ref_type::rt_relation:
				next_send_[monitor].emplace(it_tags->second, it_handles->second.reference.ref_value_ptr.relation->value.get_value(state));
				break;
			default:
				RX_ASSERT(false);
				return "Internal error";
			}
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

		auto ref_result = item.get_value_ref(path, ref);
		if (ref_result)
		{
			// fill out the data
			auto handle = rx_internal::sys_runtime::platform_runtime_manager::get_new_handle();
			switch (ref.ref_type)
			{
			case rt_value_ref_type::rt_const_value:
				const_values_.emplace(ref.ref_value_ptr.const_value, handle);
				next_send_[monitor].emplace(handle, ref.ref_value_ptr.const_value->get_value(state));
				break;
			case rt_value_ref_type::rt_value:
				values_.emplace(ref.ref_value_ptr.value, handle);
				next_send_[monitor].emplace(handle, ref.ref_value_ptr.value->get_value(state));
				break;
			case rt_value_ref_type::rt_variable:
				variables_.emplace(ref.ref_value_ptr.variable, handle);
				next_send_[monitor].emplace(handle, ref.ref_value_ptr.variable->get_value(state));
				break;
			default:
				RX_ASSERT(false);
				return "Internal error";
			}
			handles_map_.emplace(handle, one_tag_data{ ref, 1,  {monitor} });
			referenced_tags_.emplace(path, handle);
			return handle;
		}
		else
		{// try relations to see if there is something!!!
			// relations code here!!!
			auto result = connect_tag_from_relations(path, item, monitor, state);
			if (result)
			{
				referenced_tags_.emplace(path, result.value());
			}
			return result;
		}
	}
}

rx_result connected_tags::disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor)
{
	if (!handle)
		return true;
	auto it_handles = handles_map_.find(handle);
	if (it_handles == handles_map_.end() || it_handles->second.reference_count == 0)
		return "Invalid item handle";
	it_handles->second.reference_count--;
	it_handles->second.monitors.erase(monitor);
	return true;
}

bool connected_tags::process_runtime (algorithms::runtime_process_context& ctx)
{
	if (!next_send_.empty())
	{
		std::vector<update_item> update_data;
		next_send_type next_send;
		{
			next_send = next_send_;
			next_send_.clear();			
		}
		for (auto one : next_send)
		{
			auto monitor = one.first;
			update_data.clear();
			for(const auto& item : one.second)
				update_data.emplace_back(update_item{ item.first, item.second });
			monitor->items_changed(update_data);
		}
	}
	return false;
}

rx_result connected_tags::read_tag (runtime_handle_t item, tags_callback_ptr monitor, const structure::hosting_object_data& state)
{
	auto it = handles_map_.find(item);
	if (it != handles_map_.end())
	{
		switch (it->second.reference.ref_type)
		{
		case rt_value_ref_type::rt_const_value:
			next_send_[monitor].emplace(item, it->second.reference.ref_value_ptr.const_value->get_value(state));
			break;
		case rt_value_ref_type::rt_value:
			next_send_[monitor].emplace(item, it->second.reference.ref_value_ptr.value->get_value(state));
			break;
		case rt_value_ref_type::rt_variable:
			next_send_[monitor].emplace(item, it->second.reference.ref_value_ptr.variable->get_value(state));
			break;
		case rt_value_ref_type::rt_relation:
			next_send_[monitor].emplace(item, it->second.reference.ref_value_ptr.relation->value.get_value(state));
			break;
		default:
			RX_ASSERT(false);
			return "Internal error";
		}
		return true;
	}
	else
	{
		auto it_rel = relations_handles_map_.find(item);
		if (it_rel != relations_handles_map_.end())
		{
			auto result = it_rel->second->read_tag(item, monitor, state);
			return result;
		}
	}
	return true;
}

void connected_tags::binded_tags_change (structure::value_data* whose, const rx_value& val, structure::hosting_object_data& state)
{
	auto it = values_.find(whose);
	if (it != values_.end())
	{
		auto handle = it->second;
		auto it_data = handles_map_.find(handle);
		if (it_data != handles_map_.end())
		{
			if (!it_data->second.monitors.empty())
			{
				for (auto& one : it_data->second.monitors)
				{
					next_send_[one].emplace(handle, val);
				}
				state.context->tag_updates_pending();
			}
		}
	}
}

rx_result connected_tags::write_tag (runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor, const structure::hosting_object_data& state)
{
	auto it = handles_map_.find(item);
	if (it != handles_map_.end())
	{
		auto ctx = structure::write_context::create_write_context(state, false);
		switch (it->second.reference.ref_type)
		{
		case rt_value_ref_type::rt_const_value:
			return "Can't write to const value!";
		case rt_value_ref_type::rt_value:
			{
				auto result = it->second.reference.ref_value_ptr.value->write_value(std::move(value), ctx);
				if (result)
				{
					auto val = it->second.reference.ref_value_ptr.value->get_value(state);
					for(const auto& one : it->second.monitors)
						next_send_[one].emplace(item, val);
				}
				return result;
			}
		case rt_value_ref_type::rt_variable:
			{
				auto result = it->second.reference.ref_value_ptr.variable->write_value(std::move(value), ctx, state.context);
				return result;
			}
		case rt_value_ref_type::rt_relation:
			{
				auto result = it->second.reference.ref_value_ptr.relation->value.write_value(std::move(value), ctx);
				if (result)
				{
					auto val = it->second.reference.ref_value_ptr.relation->value.get_value(state);
					for (const auto& one : it->second.monitors)
						next_send_[one].emplace(item, val);
				}
				return result;
			}
		default:
			RX_ASSERT(false);
			return "Internal error";
		}
	}
	else
	{
		auto it_rel = relations_handles_map_.find(item);
		if (it_rel != relations_handles_map_.end())
		{
			auto result = it_rel->second->write_tag(item, std::move(value), monitor, state);
			return result;
		}
		return "Invalid handle value!";
	}
	return true;
}

void connected_tags::relation_tags_change (relations::relation_runtime* whose, const rx_value& val, structure::hosting_object_data& state)
{
}

rx_result_with<runtime_handle_t> connected_tags::connect_tag_from_relations (const string_type& path, structure::runtime_item& item, tags_callback_ptr monitor, const structure::hosting_object_data& state)
{
	string_type sub_path = path;
	auto idx = path.find(RX_OBJECT_DELIMETER);
	if (idx != string_type::npos)
	{
		string_type rest_path(path.substr(idx + 1));
		auto it = mapped_relations_.find(path.substr(0, idx));
		if (it == mapped_relations_.end())
		{
			auto relation = state.get_relation(path.substr(0, idx));
			if (relation)
			{
				auto result = mapped_relations_.emplace(path.substr(0, idx), relation.unsafe_ptr());
				if (result.second)
					it = result.first;
			}
		}
		if (it != mapped_relations_.end())
		{
			auto result = it->second->connect_tag(path.substr(idx + 1), monitor, state);
			if (result)
			{
				relations_handles_map_.emplace(result.value(), it->second);
			}
			return result;
		}
		else
		{
			return "Invalid path!";
		}
	}
	else
	{
		auto it = mapped_relations_.find(path);
		if (it == mapped_relations_.end())
		{
			auto relation = state.get_relation(path);
			if (relation)
			{
				auto result = mapped_relations_.emplace(path, relation.unsafe_ptr());
				if (result.second)
					it = result.first;
			}
		}
		if (it != mapped_relations_.end())
		{
			if (it->second->runtime_handle == 0)
			{
				auto handle = rx_internal::sys_runtime::platform_runtime_manager::get_new_handle();

				rt_value_ref reference;
				reference.ref_type = rt_value_ref_type::rt_relation;
				reference.ref_value_ptr.relation = it->second;
				handles_map_.emplace(handle, one_tag_data{ reference, 1,  {monitor} });
				it->second->runtime_handle = handle;

			}
			auto result = it->second->runtime_handle;
			next_send_[monitor].emplace(it->second->runtime_handle, it->second->value.get_value(state));
			return result;
		}
		else
		{
			return "Invalid path!";
		}
	}
}

void connected_tags::runtime_stopped (const rx_time& now)
{
	rx_value temp_val;
	temp_val.set_time(now);
	temp_val.set_quality(RX_DEAD_QUALITY);
	for (const auto& one : handles_map_)
	{
		for (auto& monitor : one.second.monitors)
		{
			next_send_[monitor].emplace(one.first, temp_val);
		}
	}
	std::vector<update_item> update_data;
	for (auto one : next_send_)
	{
		auto monitor = one.first;
		update_data.clear();
		for (const auto& item : one.second)
			update_data.emplace_back(update_item{ item.first, item.second });
		monitor->items_changed(update_data);
	}
	next_send_.clear();
}

void connected_tags::variable_change (structure::variable_data* whose, const rx_value& val, structure::hosting_object_data& state)
{
	auto it = variables_.find(whose);
	if (it != variables_.end())
	{
		auto handle = it->second;
		auto it_data = handles_map_.find(handle);
		if (it_data != handles_map_.end())
		{
			if (!it_data->second.monitors.empty())
			{
				for (auto& one : it_data->second.monitors)
				{
					next_send_[one].emplace(handle, val);
				}
				state.context->tag_updates_pending();
			}
		}
	}
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

rx_result binded_tags::set_value (runtime_handle_t handle, rx_simple_value&& val, connected_tags& tags, structure::hosting_object_data& state)
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
			{
				auto result= it_handles->second.ref_value_ptr.value->simple_set_value(std::move(val));
				if (result)
				{
					auto new_value = it_handles->second.ref_value_ptr.value->get_value(state);
					tags.binded_tags_change(it_handles->second.ref_value_ptr.value, new_value, state);
				}
				return result;
			}
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
				revisied_path = full_path;
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
				revisied_path = full_path;
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
		auto ref_result = ctx.structure.get_root().get_value_ref(path, ref);
		if (!ref_result)
			return ref_result.errors();
		revisied_path = path;
	}

	if (ref.ref_type == rt_value_ref_type::rt_variable)
		return "Can't bind to variable";
	// fill out the data
	auto handle = ctx.get_new_handle();
	auto bind_result = bind_tag(ref, handle);
	if (bind_result)
	{
		ctx.binded_tags.emplace(revisied_path, bind_result.value());
	}
	return bind_result;
}

rx_result binded_tags::set_item (const string_type& path, rx_simple_value&& what, runtime_init_context& ctx)
{
	return internal_set_item(path, std::move(what), ctx.structure);
}

rx_result binded_tags::pool_value (runtime_handle_t handle, std::function<void(const rx_value&)> callback) const
{
	return RX_NOT_IMPLEMENTED;
}

void binded_tags::connected_tags_change (structure::value_data* whose, const rx_value& val)
{
}

rx_result binded_tags::set_item (const string_type& path, rx_simple_value&& what, runtime_start_context& ctx)
{
	return internal_set_item(path, std::move(what), ctx.structure);
}

rx_result binded_tags::internal_set_item (const string_type& path, rx_simple_value&& what, runtime_structure_resolver& structure)
{
	string_type revisied_path;
	rt_value_ref ref;
	ref.ref_type = rt_value_ref_type::rt_null;

	if (!path.empty())
	{
		switch (path[0])
		{
		case RX_PATH_CURRENT:
			{
				auto ref_result = structure.get_current_item().get_value_ref(path, ref);
				if (!ref_result)
					return ref_result.errors();
			}
			break;
		case RX_PATH_PARENT:
			{
				size_t idx = 1;
				while (idx < path.size() && path[idx] == RX_PATH_PARENT)
					idx++;
				auto ref_result = structure.get_current_item().get_value_ref(path, ref);
				if (!ref_result)
					return ref_result.errors();
			}
			break;
		}
	}
	if (ref.ref_type == rt_value_ref_type::rt_null)
	{
		auto ref_result = structure.get_root().get_value_ref(path, ref);
		if (!ref_result)
			return ref_result.errors();
	}
	switch (ref.ref_type)
	{
	case rt_value_ref_type::rt_const_value:
		return ref.ref_value_ptr.const_value->set_value(std::move(what));
	case rt_value_ref_type::rt_value:
		ref.ref_value_ptr.value->set_value(std::move(what), rx_time::now());
		return true;
	default:
		return "Unsupported type!.";
	}
}


} // namespace operational
} // namespace runtime
} // namespace rx_platform

