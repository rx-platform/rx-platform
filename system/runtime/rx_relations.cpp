

/****************************************************************************
*
*  system\runtime\rx_relations.cpp
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


// rx_holder_algorithms
#include "system/runtime/rx_holder_algorithms.h"
// rx_relations
#include "system/runtime/rx_relations.h"

#include "system/server/rx_server.h"
#include "sys_internal/rx_internal_ns.h"
#include "rx_objbase.h"
#include "model/rx_model_algorithms.h"
#include "sys_internal/rx_async_functions.h"
#include "runtime_internal/rx_relations_runtime.h"
#include "runtime_internal/rx_runtime_internal.h"
using namespace rx_internal::sys_runtime;
using namespace rx_internal::sys_runtime::relations_runtime;


namespace rx_platform {

namespace runtime {

namespace relations {

// Class rx_platform::runtime::relations::relation_connector 

relation_connector::~relation_connector()
{
}



// Class rx_platform::runtime::relations::relation_data 

relation_data::relation_data()
      : executer_(0),
        context_(nullptr),
        runtime_handle(0),
        is_target_(false)
{
}



rx_result relation_data::initialize_relation (runtime::runtime_init_context& ctx)
{
	object_directory = ctx.meta.path;
	resolver_user_.my_relation = smart_this();
	context_ = ctx.context;
	auto result = implementation_->initialize_relation(ctx);
	if (result)
	{
		rx_item_reference ref = implementation_->get_implicit_reference(ctx.meta);
		
		if (!ref.is_null())
		{
			meta_data target_meta;
			if (ref.is_node_id())
			{
				auto type = rx_internal::model::platform_types_manager::instance().get_types_resolver().get_item_data(ref.get_node_id(), target_meta);
				if (type != rx_item_type::rx_invalid_type)
				{
					target_path = target_meta.get_full_path() + RX_OBJECT_DELIMETER + target_relation_name;
				}
			}
			else
			{
				target_path = ref.get_path();
			}
			resolve_inverse_name();
			rx_simple_value val;
			val.assign_static<string_type>(string_type(target_path));
			value.set_value(std::move(val), ctx.now);
		}
	}
	return result;
}

rx_result relation_data::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
	auto result = implementation_->deinitialize_relation(ctx);
	return result;
}

rx_result relation_data::start_relation (runtime::runtime_start_context& ctx)
{
	rx_item_reference ref = implementation_->get_implicit_reference(ctx.context->meta_info);
	if (!ref.is_null())
	{
		meta_data target_meta;
		if (ref.is_node_id())
		{
			auto type = rx_internal::model::platform_types_manager::instance().get_types_resolver().get_item_data(ref.get_node_id(), target_meta);
			if (type != rx_item_type::rx_invalid_type)
			{
				target_path = target_meta.get_full_path() + RX_OBJECT_DELIMETER + target_relation_name;
			}
			rx_simple_value val;
			val.assign_static<string_type>(string_type(target_path));
			value.set_value(std::move(val), ctx.now);
			target_id = ref.get_node_id();
		}
	}
	executer_ = rx_thread_context();
	auto result = implementation_->start_relation(ctx);
	if (result)
	{
		resolve_inverse_name();

		rx_item_reference ref;
		if (target_id.is_null() && !parent_path.empty())
			ref = parent_path;
		else
			ref = target_id;
		resolver_.start_resolver(ref, &resolver_user_, context_->get_directory_resolver());
	}
	return result;
}

rx_result relation_data::stop_relation (runtime::runtime_stop_context& ctx)
{
	if (target_id)
	{		
		auto remove_result = platform_runtime_manager::instance().get_cache().remove_target_relation(target_id, target_relation_name);
		if (!remove_result)
			RUNTIME_LOG_ERROR("relation_runtime", 999, "Error removing target relation from "s + parent_path);
	}
	my_state_ = relation_state::stopping;
	auto result = implementation_->stop_relation(ctx);
	return result;
}

void relation_data::fill_data (const data::runtime_values_data& data, runtime_process_context* ctx)
{
	auto it = data.values.find(name);
	if (it != data.values.end())
	{
		target_path = it->second.value.get_storage().get_string_value();
	}
	rx_simple_value val;
	val.assign_static(target_path);
	value.set_value(std::move(val), ctx->now);
}

void relation_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	rx_simple_value temp;
	temp.assign_static<string_type>(string_type(target_path));
	data.add_value(name, std::move(temp));
}

rx_result relation_data::read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx) const
{
	return RX_NOT_IMPLEMENTED;
}

rx_result relation_data::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result relation_data::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	if (connector_)
	{
		auto result = connector_->browse(prefix, path, filter, items);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}

rx_result relation_data::read_tag (runtime_handle_t item, tags_callback_ptr monitor, runtime_process_context* ctx)
{
	if (connector_)
	{
		auto result = connector_->read_tag(item, monitor, ctx);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}

rx_result relation_data::write_tag (runtime_handle_t item, rx_simple_value&& value, tags_callback_ptr monitor, runtime_process_context* ctx)
{
	if (connector_)
	{
		auto result = connector_->write_tag(item, std::move(value), monitor, ctx);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}

rx_result_with<runtime_handle_t> relation_data::connect_tag (const string_type& path, tags_callback_ptr monitor, runtime_process_context* ctx)
{
	if (connector_)
	{
		auto result = connector_->connect_tag(path, monitor, ctx);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}

rx_result relation_data::disconnect_tag (runtime_handle_t handle, tags_callback_ptr monitor)
{
	if (connector_)
	{
		auto result = connector_->disconnect_tag(handle, monitor);
		return result;
	}
	else
	{
		return "Wrong object state!";
	}
}

void relation_data::try_resolve ()
{
	if (my_state_ != relation_state::idle)
		return;// not in right state

	resolve_inverse_name();

	if (target_id.is_null())
	{
		rx_directory_resolver dirs;
		dirs.add_paths({ object_directory });
		auto resolve_result = rx_internal::model::algorithms::resolve_reference(parent_path, dirs);
		if (!resolve_result)
		{
			RUNTIME_LOG_ERROR("relation_runtime", 100, "Unable to resolve relation reference to "s + parent_path);
			return;
		}
		target_id = resolve_result.move_value();
	}
	
	if (target_id.is_null())
		return;

	auto result = implementation_->resolve_runtime_sync(target_id);
	if (result)
	{
		auto&& item_ptr = result.move_value();
		if (item_ptr->get_executer() == executer_)
		{
			my_state_ = relation_state::same_domain;

			algorithms::runtime_relation_algorithms::notify_relation_connected(name, item_ptr, context_);
			connector_ = std::make_unique<local_relation_connector>(std::move(item_ptr));
			implementation_->relation_connected();
			if (!is_target_)
			{
				auto target_rel = make_target_relation(context_->meta_info.get_full_path());

				auto add_result = platform_runtime_manager::instance().get_cache().add_target_relation(target_id, std::move(target_rel));
				if (!add_result)
					RUNTIME_LOG_ERROR("relation_runtime", 999, "Error adding target relation to "s + parent_path);
			}
		}
		else
		{
			my_state_ = relation_state::remote;
			RUNTIME_LOG_ERROR("relation_runtime", 999, "Not supported relation type "s + parent_path);
		}
	}
}

relation_data::smart_ptr relation_data::make_target_relation (const string_type& path)
{
	auto ret = rx_create_reference<relation_data>();
	ret->target_path = path + RX_OBJECT_DELIMETER + name;
	ret->resolve_inverse_name();
	ret->target_id = context_->meta_info.id;
	ret->name = target_relation_name;
	ret->implementation_ = implementation_->make_target_relation();
	rx_timed_value str_val;
	str_val.assign_static<string_type>(string_type(ret->target_path), rx_time::now());
	ret->value.value = str_val;
	ret->value.value_opt[runtime::structure::value_data::opt_readonly] = true;
	return ret;
}

rx_result relation_data::start_target_relation (runtime::runtime_start_context& ctx)
{
	object_directory = "/";
	resolver_user_.my_relation = smart_this();
	context_ = ctx.context;
	executer_ = rx_thread_context();
	is_target_ = true;
	ctx.context->meta_info.get_full_path();

	rx_simple_value val;
	val.assign_static<string_type>(string_type(target_path));
	value.set_value(std::move(val), ctx.now);

	auto result = implementation_->start_relation(ctx);
	if (result)
	{
		try_resolve();
		/*rx_item_reference ref;
		if (target_id.is_null() && !target.empty())
			ref = target;
		else
			ref = target_id;
		resolver_.start_resolver(ref, &resolver_user_, context_->get_directory_resolver());*/
	}
	return result;
}

rx_result relation_data::stop_target_relation (runtime::runtime_stop_context& ctx)
{
	my_state_ = relation_state::stopping;
	auto result = implementation_->stop_relation(ctx);
	return result;
}

bool relation_data::runtime_connected (platform_item_ptr&& item)
{
	try_resolve();
	return true;
}

void relation_data::runtime_disconnected ()
{
	if (my_state_ != relation_state::idle || my_state_ == relation_state::stopping)
	{
		implementation_->relation_disconnected();
		if (my_state_ != relation_state::stopping)
			my_state_ = relation_state::idle;
		algorithms::runtime_relation_algorithms::notify_relation_disconnected(name, context_);
	}
}

rx_result relation_data::resolve_inverse_name ()
{
	if (!target_path.empty())
	{
		string_type inverse_name;
		auto idx_obj = target_path.rfind('.');
		if (idx_obj != string_type::npos && idx_obj > 1)// safely 1 
		{
			auto idx_path = target_path.rfind(RX_DIR_DELIMETER);
			if (idx_path != string_type::npos && idx_path>idx_obj)
			{
				// this is a strange thing like "../name
				parent_path = target_path;
			}
			else
			{
				inverse_name = target_path.substr(idx_obj + 1);
				parent_path = target_path.substr(0, idx_obj);
				if (!rx_is_valid_namespace_name(inverse_name))
				{
					RUNTIME_LOG_ERROR("relation_runtime", 100, inverse_name + " in " + target_path + " is not a valid namespace name");
					return true;
				}
				target_relation_name = inverse_name;
			}
		}
		else
		{
			parent_path = target_path;
		}		
	}
	return true;
}


// Class rx_platform::runtime::relations::relations_holder 


rx_result relations_holder::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
	// check relations, but only for exact name
	for (const auto& one : source_relations_)
	{
		if (one->name == path)
		{
			val = one->value.get_value(ctx);
			return true;
		}
	}
	for (const auto& one : target_relations_)
	{
		if (one->name == path)
		{
			val = one->value.get_value(ctx);
			return true;
		}
	}
	return path + " not found!";
}

rx_result relations_holder::initialize_relations (runtime::runtime_init_context& ctx)
{
	rx_result result = true;
	for (auto& one : source_relations_)
	{
		result = one->initialize_relation(ctx);
		if (!result)
			break;
	}
	for (auto& one : implicit_relations_)
	{
		result = one->initialize_relation(ctx);
		if (!result)
			break;
	}
	return result;
}

rx_result relations_holder::deinitialize_relations (runtime::runtime_deinit_context& ctx)
{
	rx_result result = true;
	for (auto& one : implicit_relations_)
	{
		result = one->deinitialize_relation(ctx);
		if (!result)
			break;
	}
	for (auto& one : source_relations_)
	{
		result = one->deinitialize_relation(ctx);
		if (!result)
			break;
	}
	return result;
}

rx_result relations_holder::start_relations (runtime::runtime_start_context& ctx)
{
	rx_result result = true;
	for (auto& one : source_relations_)
	{
		result = one->start_relation(ctx);
		if (!result)
			break;
	}
	for (auto& one : implicit_relations_)
	{
		result = one->start_relation(ctx);
		if (!result)
			break;
	}
	return result;
}

rx_result relations_holder::stop_relations (runtime::runtime_stop_context& ctx)
{
	rx_result result = true;
	for (auto& one : implicit_relations_)
	{
		result = one->stop_relation(ctx);
		if (!result)
			break;
	}
	for (auto& one : source_relations_)
	{
		result = one->stop_relation(ctx);
		if (!result)
			break;
	}
	return result;
}

void relations_holder::fill_data (const data::runtime_values_data& data, runtime_process_context* ctx)
{
	for (auto& one : source_relations_)
	{
		one->fill_data(data, ctx);
	}
}

void relations_holder::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	if (type != runtime_value_type::persistent_runtime_value)
	{
		for (const auto& one : source_relations_)
		{
			one->collect_data(data, type);
		}
		for (const auto& one : implicit_relations_)
		{
			one->collect_data(data, type);
		}
		for (const auto& one : target_relations_)
		{
			one->collect_data(data, type);
		}
	}
	else // type == runtime_value_type::persistent_runtime_value
	{
		for (const auto& one : source_relations_)
		{
			if(one->value.value_opt[structure::value_data::opt_persistent])
				one->collect_data(data, type);
		}
	}
}

rx_result relations_holder::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	if (path.empty())
	{
		for (auto& one : source_relations_)
		{
			runtime_item_attribute attr;
			attr.full_path = prefix.empty() ? one->name : prefix + RX_OBJECT_DELIMETER + one->name;
			attr.name = one->name;
			attr.type = rx_attribute_type::relation_attribute_type;
			attr.value.assign_static<string_type>(string_type(one->target_path));
			items.push_back(attr);
		}
		for (auto& one : implicit_relations_)
		{
			runtime_item_attribute attr;
			attr.full_path = prefix.empty() ? one->name : prefix + RX_OBJECT_DELIMETER + one->name;
			attr.name = one->name;
			attr.type = rx_attribute_type::relation_target_attribute_type;
			attr.value.assign_static<string_type>(string_type(one->target_path));
			items.push_back(attr);
		}
		for (auto& one : target_relations_)
		{
			runtime_item_attribute attr;
			attr.full_path = prefix.empty() ? one->name : prefix + RX_OBJECT_DELIMETER + one->name;
			attr.name = one->name;
			attr.type = rx_attribute_type::relation_target_attribute_type;
			attr.value.assign_static<string_type>(string_type(one->target_path));
			items.push_back(attr);
		}
		return true;
	}
	else
	{
		auto idx = path.find(RX_OBJECT_DELIMETER);
		string_type sub_path;
		string_type rest_path;
		if (idx != string_type::npos)
		{
			sub_path = path.substr(0, idx);
			rest_path = path.substr(idx + 1);
		}
		else
		{
			sub_path = path;
		}
		string_type bellow_prefix = prefix.empty() ? sub_path : prefix + RX_OBJECT_DELIMETER + sub_path;
		for (auto& one : source_relations_)
		{
			if (one->name == sub_path)
			{
				return one->browse(bellow_prefix, rest_path, filter, items);
			}
		}
		for (auto& one : implicit_relations_)
		{
			if (one->name == sub_path)
			{
				return one->browse(bellow_prefix, rest_path, filter, items);
			}
		}
		for (auto& one : target_relations_)
		{
			if (one->name == sub_path)
			{
				return one->browse(bellow_prefix, rest_path, filter, items);
			}
		}
		return path + " not found";
	}
}

relation_data::smart_ptr relations_holder::get_relation (const string_type& name)
{
	for (auto& one : source_relations_)
	{
		if (one->name == name)
			return one;
	}
	for (auto& one : target_relations_)
	{
		if (one->name == name)
			return one;
	}
	for (auto& one : implicit_relations_)
	{
		if (one->name == name)
			return one;
	}
	return relation_data::smart_ptr::null_ptr;;
}

rx_result relations_holder::add_target_relation (relations::relation_data::smart_ptr data, runtime::runtime_start_context& ctx)
{
	for (auto& one : target_relations_)
	{
		if (one->name.empty())
		{
			one = std::move(data);
			return one->start_target_relation(ctx);
		}
	}
	auto& one = target_relations_.emplace_back(std::move(data));
	return one->start_target_relation(ctx);
}

rx_result_with<relation_data::smart_ptr> relations_holder::remove_target_relation (const string_type& name, runtime::runtime_stop_context& ctx)
{
	relation_data::smart_ptr ret;
	for (auto& one : target_relations_)
	{
		if (one->name == name)
		{
			ret = one;
			auto result = one->stop_target_relation(ctx);
			one->name.clear();
			break;
		}
	}
	if (ret)
		return ret;
	else
		return "Relation "s + name + "not found.";
}

rx_result relations_holder::add_implicit_relation (relations::relation_data::smart_ptr data)
{
	for (auto& one : implicit_relations_)
	{
		if (one->name.empty())
		{
			one = std::move(data);
			return true;
		}
	}
	implicit_relations_.emplace_back(std::move(data));
	return true;
}

bool relations_holder::is_this_yours (const string_type& path) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	if (idx == string_type::npos)
	{
		for (const auto& one : implicit_relations_)
		{
			if (one->name == path)
				return true;
		}
		for (const auto& one : source_relations_)
		{
			if (one->name == path)
				return true;
		}
		for (const auto& one : target_relations_)
		{
			if (one->name == path)
				return true;
		}
	}
	else
	{
		size_t len = idx;
		for (const auto& one : implicit_relations_)
		{
			if (one->name.size() == len && memcmp(one->name.c_str(), path.c_str(), len) == 0)
				return true;
		}
		for (const auto& one : source_relations_)
		{
			if (one->name.size() == len && memcmp(one->name.c_str(), path.c_str(), len) == 0)
				return true;
		}
		for (const auto& one : target_relations_)
		{
			if (one->name.size() == len && memcmp(one->name.c_str(), path.c_str(), len) == 0)
				return true;
		}
	}
	return false;
}

rx_result relations_holder::get_value_ref (const string_type& path, rt_value_ref& ref)
{
	relation_data* ret = nullptr;

	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type name;
	if (idx == string_type::npos)
	{
		for (auto& one : implicit_relations_)
		{
			if (one->name == path)
				ret = one.unsafe_ptr();
		}
		for (auto& one : source_relations_)
		{
			if (one->name == path)
				ret = one.unsafe_ptr();
		}
		for (auto& one : target_relations_)
		{
			if (one->name == path)
				ret = one.unsafe_ptr();
		}
	}

	if (ret)
	{
		ref.ref_type = rt_value_ref_type::rt_relation;
		ref.ref_value_ptr.relation = ret;
		return true;
	}
	else
	{
		return path + " not found!";
	}
}

rx_result relations_holder::register_relation_subscriber (const string_type& name, relation_subscriber* who)
{
	auto it = relation_subscribers_.find(name);
	if (it != relation_subscribers_.end())
	{
		it->second.emplace_back(who);
		return true;
	}
	else
	{
		for (auto& one : source_relations_)
		{
			if (one->name == name)
			{
				relation_subscribers_[name].emplace_back(who);
				return true;
			}
		}
	}
	return "Invalid relation name!";
}


} // namespace relations
} // namespace runtime
} // namespace rx_platform

