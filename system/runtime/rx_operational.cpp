

/****************************************************************************
*
*  system\runtime\rx_operational.cpp
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


#include "pch.h"


// rx_runtime_logic
#include "system/runtime/rx_runtime_logic.h"
// rx_relations
#include "system/runtime/rx_relations.h"
// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_operational
#include "system/runtime/rx_operational.h"

#include "system/runtime/rx_blocks.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "sys_internal/rx_async_functions.h"


namespace rx_platform {

namespace runtime {

namespace tag_blocks {

// Class rx_platform::runtime::tag_blocks::binded_tags 

binded_tags::binded_tags()
{
}


binded_tags::~binded_tags()
{
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
		case rt_value_ref_type::rt_method:
			val = it_handles->second.ref_value_ptr.method->value.simple_get_value();
			return true;
		case rt_value_ref_type::rt_full_value:
			val = it_handles->second.ref_value_ptr.full_value->simple_get_value();
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

rx_result binded_tags::set_value (runtime_handle_t handle, rx_simple_value&& val, connected_tags& tags, runtime_process_context* ctx)
{
	auto it_handles = handles_map_.find(handle);
	if (it_handles != handles_map_.end())
	{
		switch (it_handles->second.ref_type)
		{
		case rt_value_ref_type::rt_full_value:
		case rt_value_ref_type::rt_method:
		case rt_value_ref_type::rt_const_value:
			return "Not supported!";
		case rt_value_ref_type::rt_value:
			{
				auto result= it_handles->second.ref_value_ptr.value->simple_set_value(std::move(val), ctx);
				if (result)
				{
					auto new_value = it_handles->second.ref_value_ptr.value->get_value(ctx);
					tags.binded_value_change(it_handles->second.ref_value_ptr.value, new_value);
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

rx_result_with<runtime_handle_t> binded_tags::bind_item (const string_type& path, runtime_init_context& ctx, binded_callback_t callback)
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

				auto ref_result = ctx.structure.get_current_item().get_value_ref(&path.c_str()[1], ref, false);
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

				auto ref_result = ctx.structure.get_current_item().get_value_ref(path, ref, false);
				if (!ref_result)
					return ref_result.errors();
			}
			break;
		}
	}
	if (ref.ref_type == rt_value_ref_type::rt_null)
	{
		auto ref_result = ctx.structure.get_root().get_value_ref(path, ref, false);
		if (!ref_result)
			return ref_result.errors();
		revisied_path = path;
	}

	if (ref.ref_type == rt_value_ref_type::rt_variable)
		return "Can't bind to variable";
	// fill out the data
	auto handle = ctx.get_new_handle();
	switch (ref.ref_type)
	{
	case rt_value_ref_type::rt_const_value:
		// this is constant so just do nothing
		break;
	case rt_value_ref_type::rt_value:
		{
			auto result = values_.emplace(ref.ref_value_ptr.value, values_type::mapped_type{ handle, values_type::mapped_type::second_type() });
			if (callback)
				result.first->second.second.emplace_back(callback);
		}
		break;
	case rt_value_ref_type::rt_full_value:
		{
			auto result = full_values_.emplace(ref.ref_value_ptr.full_value, values_type::mapped_type{ handle, full_values_type::mapped_type::second_type() });
			if (callback)
				result.first->second.second.emplace_back(callback);
		}
		break;
	case rt_value_ref_type::rt_method:
		{
			auto result = methods_.emplace(ref.ref_value_ptr.method, values_type::mapped_type{ handle, methods_type::mapped_type::second_type() });
			if (callback)
				result.first->second.second.emplace_back(callback);
		}
		break;
	default:
		RX_ASSERT(false);
		return "Internal error";
	}
	handles_map_.emplace(handle, ref);
	ctx.binded_tags.emplace(revisied_path, handle);

	return handle;
}

rx_result binded_tags::set_item (const string_type& path, rx_simple_value&& what, runtime_init_context& ctx)
{
	return internal_set_item(path, std::move(what), ctx.structure);
}

rx_result binded_tags::pool_value (runtime_handle_t handle, std::function<void(const rx_value&)> callback) const
{
	return RX_NOT_IMPLEMENTED;
}

void binded_tags::value_change (structure::value_data* whose, const rx_value& val)
{
	auto it = values_.find(whose);
	if (it != values_.end())
	{
		for (auto& one : it->second.second)
		{
			one(val);
		}
	}
}

void binded_tags::full_value_changed (structure::full_value_data* whose, const rx_value& val, connected_tags& tags)
{
	auto it = full_values_.find(whose);
	if (it != full_values_.end())
	{
		for (auto& one : it->second.second)
		{
			one(val);
		}
	}
	tags.full_value_changed(whose, val);
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
				auto ref_result = structure.get_current_item().get_value_ref(&path.c_str()[1], ref, false);
				if (!ref_result)
					return ref_result.errors();
			}
			break;
		case RX_PATH_PARENT:
			{
				size_t idx = 1;
				while (idx < path.size() && path[idx] == RX_PATH_PARENT)
					idx++;
				auto ref_result = structure.get_current_item().get_value_ref(path, ref, false);
				if (!ref_result)
					return ref_result.errors();
			}
			break;
		}
	}
	if (ref.ref_type == rt_value_ref_type::rt_null)
	{
		auto ref_result = structure.get_root().get_value_ref(path, ref, false);
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

rx_result binded_tags::get_item (const string_type& path, rx_simple_value& what, runtime_start_context& ctx)
{
	return internal_get_item(path, what, ctx.structure, ctx.context);
}

rx_result binded_tags::internal_get_item (const string_type& path, rx_simple_value& what, runtime_structure_resolver& structure, runtime_process_context* ctx)
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
				auto ref_result = structure.get_current_item().get_value_ref(&path.c_str()[1], ref, false);
				if (!ref_result)
					return ref_result.errors();
			}
			break;
		case RX_PATH_PARENT:
			{
				size_t idx = 1;
				while (idx < path.size() && path[idx] == RX_PATH_PARENT)
					idx++;
				auto ref_result = structure.get_current_item().get_value_ref(path, ref, false);
				if (!ref_result)
					return ref_result.errors();
			}
			break;
		}
	}
	if (ref.ref_type == rt_value_ref_type::rt_null)
	{
		auto ref_result = structure.get_root().get_value_ref(path, ref, false);
		if (!ref_result)
			return ref_result.errors();
	}
	switch (ref.ref_type)
	{
	case rt_value_ref_type::rt_const_value:
		what = ref.ref_value_ptr.const_value->get_value(ctx).to_simple();
		return true;
	case rt_value_ref_type::rt_value:
		what = ref.ref_value_ptr.value->get_value(ctx).to_simple();
		return true;
	case rt_value_ref_type::rt_full_value:
		what = ref.ref_value_ptr.full_value->get_value(ctx).to_simple();
		return true;
	case rt_value_ref_type::rt_method:
		what = ref.ref_value_ptr.method->value.get_value(ctx).to_simple();
		return true;
	default:
		return "Unsupported type!.";
	}
}

rx_result binded_tags::get_item (const string_type& path, rx_simple_value& what, runtime_init_context& ctx)
{
	return internal_get_item(path, what, ctx.structure, ctx.context);
}


// Class rx_platform::runtime::tag_blocks::connected_tags 

connected_tags::connected_tags()
      : binded_(nullptr),
        parent_relations_(nullptr),
        parent_logic_(nullptr),
        parent_displays_(nullptr)
{
}


connected_tags::~connected_tags()
{
}



void connected_tags::init_tags (runtime_process_context* ctx, relations::relations_holder* relations, logic_blocks::logic_holder* logic, display_blocks::displays_holder* displays, binded_tags* binded)
{
	context_ = ctx;
	parent_relations_ = relations;
	parent_logic_ = logic;
	parent_displays_ = displays;
	binded_ = binded;
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
			next_send_[monitor].insert_or_assign(one.first, temp_val);
		}
	}
	std::vector<update_item> update_data;
	for (auto& one : next_send_)
	{
		auto monitor = one.first;
		if (monitor)
		{
			update_data.clear();
			for (const auto& item : one.second)
				update_data.emplace_back(update_item{ item.first, item.second });
			monitor->items_changed(update_data);
		}
	}
	next_send_.clear();
}

rx_result_with<runtime_handle_t> connected_tags::connect_tag (const string_type& path, structure::runtime_item& item, tags_callback_ptr monitor)
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
				next_send_[monitor].insert_or_assign(it_tags->second, it_handles->second.reference.ref_value_ptr.const_value->get_value(context_));
				break;
			case rt_value_ref_type::rt_value:
				next_send_[monitor].insert_or_assign(it_tags->second, it_handles->second.reference.ref_value_ptr.value->get_value(context_));
				break;
			case rt_value_ref_type::rt_full_value:
				next_send_[monitor].insert_or_assign(it_tags->second, it_handles->second.reference.ref_value_ptr.full_value->get_value(context_));
				break;
			case rt_value_ref_type::rt_variable:
				next_send_[monitor].insert_or_assign(it_tags->second, it_handles->second.reference.ref_value_ptr.variable->get_value(context_));
				break;
			case rt_value_ref_type::rt_method:
				next_send_[monitor].insert_or_assign(it_tags->second, it_handles->second.reference.ref_value_ptr.method->value.get_value(context_));
				break;
			case rt_value_ref_type::rt_relation:
				next_send_[monitor].insert_or_assign(it_tags->second, it_handles->second.reference.ref_value_ptr.relation->value.get_value(context_));
				break;
			case rt_value_ref_type::rt_relation_value:
				next_send_[monitor].insert_or_assign(it_tags->second, it_handles->second.reference.ref_value_ptr.relation_value->get_value(context_));
				break;
			default:
				RX_ASSERT(false);
				return "Internal error";
			}
			context_->tag_updates_pending();
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

		auto ref_result = item.get_value_ref(path, ref, false);
		if (ref_result)
		{
			return register_new_tag_ref(path, ref, monitor);
		}
		// try logic and displays
		ref_result = parent_logic_->get_value_ref(path, ref);
		if (ref_result)
		{
			return register_new_tag_ref(path, ref, monitor);
		}
		ref_result = parent_displays_->get_value_ref(path, ref);
		if (ref_result)
		{
			return register_new_tag_ref(path, ref, monitor);
		}

		// try relations to see if there is something!!!
		// relations code here!!!
		auto result = connect_tag_from_relations(path, item, monitor);
		if (result)
		{
			referenced_tags_.emplace(path, result.value());
		}
		return result;
	}
}

rx_result_with<runtime_handle_t> connected_tags::connect_tag_from_relations (const string_type& path, structure::runtime_item& item, tags_callback_ptr monitor)
{
	string_type rest_path;
	string_type rel_name;
	auto idx = path.find(RX_OBJECT_DELIMETER);
	if (idx != string_type::npos)
	{
		rest_path = path.substr(idx + 1);
		rel_name = path.substr(0, idx);
	}
	else
	{
		rel_name = path;
	}
	auto it = mapped_relations_.find(rel_name);
	if (it == mapped_relations_.end())
	{
		auto relation = get_parent_relation(rel_name);
		if (relation)
		{
			auto result = mapped_relations_.emplace(rel_name, relation);
			if (result.second)
				it = result.first;
		}
	}
	if (it != mapped_relations_.end())
	{
		if (rest_path.empty())
		{
			// relation target
			if (it->second->runtime_handle == 0)
			{
				auto handle = rx_internal::sys_runtime::platform_runtime_manager::get_new_handle();

				rt_value_ref reference;
				reference.ref_type = rt_value_ref_type::rt_relation;
				reference.ref_value_ptr.relation = it->second.unsafe_ptr();
				handles_map_.emplace(handle, one_tag_data{ reference, 1,  {monitor} });
				it->second->runtime_handle = handle;

			}
			auto result = it->second->runtime_handle;
			next_send_[monitor].insert_or_assign(it->second->runtime_handle, it->second->value.get_value(context_));
			context_->tag_updates_pending();
			return result;
		}
		else
		{
			// relation value
			auto handle = rx_internal::sys_runtime::platform_runtime_manager::get_new_handle();

			rx_result_with<relations::relation_value_data*> rel_result = it->second->connections.connect_tag(rest_path, handle);
			if (rel_result)
			{
				rt_value_ref reference;
				reference.ref_type = rt_value_ref_type::rt_relation_value;
				reference.ref_value_ptr.relation_value = rel_result.value();
				handles_map_.emplace(handle, one_tag_data{ reference, 1,  {monitor} });
				relation_values_.emplace(rel_result.value(), handle);
				next_send_[monitor].insert_or_assign(handle, rel_result.value()->get_value(context_));
				context_->tag_updates_pending();
				return handle;
			}
			else
			{
				return rel_result.errors();
			}
		}
	}
	else
	{
		return RX_INVALID_PATH;
	}
}

rx_result connected_tags::read_tag (runtime_handle_t item, tags_callback_ptr monitor)
{
	auto it = handles_map_.find(item);
	if (it != handles_map_.end())
	{
		switch (it->second.reference.ref_type)
		{
		case rt_value_ref_type::rt_const_value:
			next_send_[monitor].insert_or_assign(item, it->second.reference.ref_value_ptr.const_value->get_value(context_));
			break;
		case rt_value_ref_type::rt_value:
			next_send_[monitor].insert_or_assign(item, it->second.reference.ref_value_ptr.value->get_value(context_));
			break;
		case rt_value_ref_type::rt_full_value:
			next_send_[monitor].insert_or_assign(item, it->second.reference.ref_value_ptr.full_value->get_value(context_));
			break;
		case rt_value_ref_type::rt_variable:
			next_send_[monitor].insert_or_assign(item, it->second.reference.ref_value_ptr.variable->get_value(context_));
			break;
		case rt_value_ref_type::rt_method:
			next_send_[monitor].insert_or_assign(item, it->second.reference.ref_value_ptr.method->value.get_value(context_));
			break;
		case rt_value_ref_type::rt_relation:
			next_send_[monitor].insert_or_assign(item, it->second.reference.ref_value_ptr.relation->value.get_value(context_));
			break;
		case rt_value_ref_type::rt_relation_value:
			next_send_[monitor].insert_or_assign(item, it->second.reference.ref_value_ptr.relation_value->get_value(context_));
			break;
		default:
			RX_ASSERT(false);
			return "Internal error";
		}
		context_->tag_updates_pending();
		return true;
	}
	return true;
}

rx_result connected_tags::write_tag (runtime_transaction_id_t trans_id, runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor)
{
	write_requests_.push_back({ trans_id, item, std::move(value), monitor });
	context_->tag_writes_pending();
	return true;
}

rx_result connected_tags::execute_tag (runtime_transaction_id_t trans_id, runtime_handle_t item, data::runtime_values_data data, tags_callback_ptr monitor)
{
	execute_requests_.push_back({ trans_id, item, std::move(data), monitor,  security::active_security()->get_handle()});
	context_->tag_writes_pending();
	return true;
}

rx_result connected_tags::disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor)
{
	if (!handle)
		return true;
	auto it_handles = handles_map_.find(handle);
	if (it_handles == handles_map_.end() || it_handles->second.reference_count == 0)
		return "Invalid item handle";
	it_handles->second.reference_count--;
	if(monitor)
		it_handles->second.monitors.erase(monitor);
	return true;
}

bool connected_tags::process_runtime ()
{
	if (!write_results_.empty())
	{
		for (auto& one : write_results_)
		{
			auto monitor = one.first;
			for (auto& item : one.second)
				monitor->write_complete(item.transaction_id, item.item, item.signal_level, std::move(item.result));
		}
		write_results_.clear();
	}
	if (!execute_results_.empty())
	{
		for (auto& one : execute_results_)
		{
			auto monitor = one.first;
			for (auto& item : one.second)
				monitor->execute_complete(item.transaction_id, item.item, item.signal_level, std::move(item.result), std::move(item.data));
		}
		execute_results_.clear();
	}
	if (!next_send_.empty())
	{
		std::vector<update_item> update_data;
		next_send_type next_send;
		{
			next_send = next_send_;
			next_send_.clear();			
		}
		for (auto& one : next_send)
		{
			auto monitor = one.first;
			if (monitor)
			{
				update_data.clear();
				for (const auto& item : one.second)
					update_data.emplace_back(update_item{ item.first, item.second });
				monitor->items_changed(update_data);
			}
		}
	}
	return false;
}

bool connected_tags::process_transactions ()
{
	if (!write_requests_.empty())
	{
		for (auto& one : write_requests_)
		{
			auto result = internal_write_tag(one.transaction_id, one.item, std::move(one.value), one.callback);
			if (!result)
			{
				write_results_[one.callback].emplace_back(write_result_data{ one.transaction_id, one.item, std::move(result) });
				context_->tag_updates_pending();
			}
		}
		write_requests_.clear();
	}
	if (!execute_requests_.empty())
	{
		for (auto& one : execute_requests_)
		{
			auto result = internal_execute_tag(one.transaction_id, one.item, std::move(one.data), one.callback, one.identity);
			if (!result)
			{
				execute_results_[one.callback].emplace_back(execute_result_data{ one.transaction_id, one.item, std::move(result) });
				context_->tag_updates_pending();
			}
		}
		execute_requests_.clear();
	}
	return false;
}

void connected_tags::binded_value_change (structure::value_data* whose, const rx_value& val)
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
					next_send_[one].insert_or_assign(handle, val);
				}
			}
		}
	}
	context_->tag_updates_pending();
}

void connected_tags::variable_change (structure::variable_data* whose, const rx_value& val)
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
					next_send_[one].insert_or_assign(handle, val);
				}
				context_->tag_updates_pending();
			}
		}
	}
}

void connected_tags::relation_value_change (relations::relation_value_data* whose, const rx_value& val)
{
	auto it = relation_values_.find(whose);
	if (it != relation_values_.end())
	{
		auto handle = it->second;
		auto it_data = handles_map_.find(handle);
		if (it_data != handles_map_.end())
		{
			if (!it_data->second.monitors.empty())
			{
				for (auto& one : it_data->second.monitors)
				{
					next_send_[one][handle] = val;
				}
				context_->tag_updates_pending();
			}
		}
	}
}

void connected_tags::write_result_arrived (tags_callback_ptr whose, write_result_data&& data)
{
	write_results_[whose].emplace_back(std::move(data));
	context_->tag_updates_pending();
}

rx_result connected_tags::internal_write_tag (runtime_transaction_id_t trans_id, runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor)
{
	auto it = handles_map_.find(item);
	if (it != handles_map_.end())
	{
		write_data data;
		data.transaction_id = trans_id;
		data.value = std::move(value);
		data.internal = false;
		switch (it->second.reference.ref_type)
		{
		case rt_value_ref_type::rt_const_value:
		case rt_value_ref_type::rt_full_value:
		case rt_value_ref_type::rt_method:
			return "Unsupported!";
		case rt_value_ref_type::rt_value:
			{
				auto result = it->second.reference.ref_value_ptr.value->write_value(std::move(data), context_);
				if (result)
				{
					auto val = it->second.reference.ref_value_ptr.value->get_value(context_);
					write_results_[monitor].emplace_back(write_result_data{trans_id, item, std::move(result)});
					for(const auto& one : it->second.monitors)
						next_send_[one].insert_or_assign(item, val);
					if (binded_)
						binded_->value_change(it->second.reference.ref_value_ptr.value, val);
					context_->tag_updates_pending();
				}
				return result;
			}
		case rt_value_ref_type::rt_variable:
			{
				auto result = it->second.reference.ref_value_ptr.variable->write_value(std::move(data)
					, new connected_write_task(this, monitor, trans_id, item), context_);
				return result;
			}
		case rt_value_ref_type::rt_relation:
			{
				auto result = it->second.reference.ref_value_ptr.relation->write_value(std::move(data), context_);
				if (result)
				{
					auto val = it->second.reference.ref_value_ptr.relation->value.get_value(context_);
					write_results_[monitor].emplace_back(write_result_data{ trans_id, item, std::move(result) });
					for (const auto& one : it->second.monitors)
						next_send_[one].insert_or_assign(item, val);
					context_->tag_updates_pending();
				}
				return result;
			}
		case rt_value_ref_type::rt_relation_value:
			{
				auto result = it->second.reference.ref_value_ptr.relation_value->write_value(std::move(data)
					, new connected_write_task(this, monitor, trans_id, item), context_);
				return result;
			}
		default:
			RX_ASSERT(false);
			return "Internal error";
		}
	}
	else
	{
		
		return "Invalid handle value!";
	}
	return true;
}

void connected_tags::execute_result_arrived (tags_callback_ptr whose, execute_result_data&& data)
{
	execute_results_[whose].emplace_back(std::move(data));
	context_->tag_updates_pending();
}

rx_result connected_tags::internal_execute_tag (runtime_transaction_id_t trans_id, runtime_handle_t item, data::runtime_values_data args, tags_callback_ptr monitor, rx_security_handle_t identity)
{
	auto it = handles_map_.find(item);
	if (it != handles_map_.end())
	{
		execute_data data;
		data.transaction_id = trans_id;
		data.data = std::move(args);
		data.internal = false;
		data.identity = identity;
		switch (it->second.reference.ref_type)
		{
		case rt_value_ref_type::rt_const_value:
		case rt_value_ref_type::rt_full_value:
		case rt_value_ref_type::rt_value:
		case rt_value_ref_type::rt_variable:
		case rt_value_ref_type::rt_relation:
			return "Unsupported!";
		case rt_value_ref_type::rt_method:
			{
				auto result = it->second.reference.ref_value_ptr.method->execute(std::move(data)
					, new connected_execute_task(this, monitor, trans_id, item), context_);
				return result;
			}
		case rt_value_ref_type::rt_relation_value:
			{
				auto result = it->second.reference.ref_value_ptr.relation_value->execute(std::move(data)
					, new connected_execute_task(this, monitor, trans_id, item), context_);
				return result;
			}
		default:
			RX_ASSERT(false);
			return "Internal error";
		}
	}
	else
	{

		return "Invalid handle value!";
	}
	return true;
}

connected_tags::relation_ptr connected_tags::get_parent_relation (const string_type& name)
{
	if (parent_relations_)
		return parent_relations_->get_relation(name);
	else
		return relations::relation_data::smart_ptr::null_ptr;
}

void connected_tags::target_relation_removed (relation_ptr&& whose)
{
}

rx_result_with<runtime_handle_t> connected_tags::register_new_tag_ref (const string_type& path, rt_value_ref ref, tags_callback_ptr monitor)
{
	// fill out the data
	auto handle = rx_internal::sys_runtime::platform_runtime_manager::get_new_handle();
	switch (ref.ref_type)
	{
	case rt_value_ref_type::rt_const_value:
		next_send_[monitor].insert_or_assign(handle, ref.ref_value_ptr.const_value->get_value(context_));
		break;
	case rt_value_ref_type::rt_value:
		values_.emplace(ref.ref_value_ptr.value, handle);
		next_send_[monitor].insert_or_assign(handle, ref.ref_value_ptr.value->get_value(context_));
		break;
	case rt_value_ref_type::rt_full_value:
		full_values_.emplace(ref.ref_value_ptr.full_value, handle);
		next_send_[monitor].insert_or_assign(handle, ref.ref_value_ptr.full_value->get_value(context_));
		break;
	case rt_value_ref_type::rt_method:
		methods_.emplace(ref.ref_value_ptr.method, handle);
		next_send_[monitor].insert_or_assign(handle, ref.ref_value_ptr.method->value.get_value(context_));
		break;
	case rt_value_ref_type::rt_variable:
		variables_.emplace(ref.ref_value_ptr.variable, handle);
		next_send_[monitor].insert_or_assign(handle, ref.ref_value_ptr.variable->get_value(context_));
		break;
	default:
		RX_ASSERT(false);
		return "Internal error";
	}
	handles_map_.emplace(handle, one_tag_data{ ref, 1,  {monitor} });
	referenced_tags_.emplace(path, handle);
	context_->tag_updates_pending();
	return handle;
}

void connected_tags::full_value_changed (structure::full_value_data* whose, const rx_value& val)
{
	auto it = full_values_.find(whose);
	if (it != full_values_.end())
	{
		auto handle = it->second;
		auto it_data = handles_map_.find(handle);
		if (it_data != handles_map_.end())
		{
			if (!it_data->second.monitors.empty())
			{
				for (auto& one : it_data->second.monitors)
				{
					next_send_[one].insert_or_assign(handle, val);
				}
			}
		}
	}
	context_->tag_updates_pending();
}


// Class rx_platform::runtime::tag_blocks::connected_write_task 

connected_write_task::connected_write_task (connected_tags* parent, tags_callback_ptr callback, runtime_transaction_id_t id, runtime_handle_t item)
      : parent_(parent),
        id_(id),
        callback_(callback),
        item_(item)
{
}



void connected_write_task::process_result (rx_result&& result)
{
	parent_->write_result_arrived(callback_, write_result_data{ id_, item_, std::move(result) });
}

runtime_transaction_id_t connected_write_task::get_id () const
{
	return id_;
}


// Class rx_platform::runtime::tag_blocks::rx_tags_callback 


// Class rx_platform::runtime::tag_blocks::connected_execute_task 

connected_execute_task::connected_execute_task (connected_tags* parent, tags_callback_ptr callback, runtime_transaction_id_t id, runtime_handle_t item)
      : parent_(parent),
        id_(id),
        callback_(callback),
        item_(item)
{
}



void connected_execute_task::process_result (rx_result&& result, data::runtime_values_data&& data)
{
	parent_->execute_result_arrived(callback_, execute_result_data{ id_, item_, std::move(result), std::move(data) });
}


} // namespace tag_blocks
} // namespace runtime
} // namespace rx_platform

