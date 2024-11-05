

/****************************************************************************
*
*  system\runtime\rx_relations.cpp
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

#include "system/server/rx_server.h"

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
	// point_count!!!!
	//ctx.points_count++;
	object_directory = ctx.meta.path;
	resolver_user_.my_relation = smart_this();
	context_ = ctx.context;
	value.value_opt[structure::value_opt_persistent] = true;
	connections.context = context_;
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
			val.assign_static(target_path.c_str());
			value.set_value(std::move(val), ctx.now());
		}
	}
	return result;
}

rx_result relation_data::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
	auto result = implementation_->deinitialize_relation(ctx);
	resolver_user_.my_relation = smart_ptr::null_ptr;
	implementation_ = rx_relation_impl_ptr::null_ptr;
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
			val.assign_static(target_path.c_str());
			value.set_value(std::move(val), ctx.now());
			target_id = ref.get_node_id();
		}
	}
	executer_ = rx_thread_context();
	auto result = implementation_->start_relation(ctx, false);
	if (result)
	{
		rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().add_to_cache(smart_this());
		resolve_inverse_name();

		rx_item_reference ref;
		if (target_id.is_null() && !parent_path.empty())
			ref = parent_path;
		else
			ref = target_id;
		my_state_  = relation_state::querying;
		resolver_.start_resolver(ref, &resolver_user_, context_->get_directory_resolver());
	}
	return result;
}

rx_result relation_data::stop_relation (runtime::runtime_stop_context& ctx)
{
	my_state_ = relation_state::stopping;
	resolver_.stop_resolver();
	auto result = implementation_->stop_relation(ctx, false);

	rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().remove_from_cache(get_item_ptr());

	return result;
}

void relation_data::fill_data (const data::runtime_values_data& data, runtime_process_context* ctx)
{
	auto init_val = data.get_value(name);
	if (!init_val.is_null())
	{
		target_path = init_val.get_string();
	}
	rx_simple_value val;
	val.assign_static(target_path.c_str());
	value.set_value(std::move(val), ctx->now());
}

void relation_data::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	if (type == runtime_value_type::persistent_runtime_value && value.value_opt[runtime::structure::value_opt_readonly])
		return;

	rx_simple_value temp;
	temp.assign_static(target_path.c_str());
	data.add_value(name, std::move(temp));
}

void relation_data::try_resolve ()
{
	if (my_state_ != relation_state::querying && my_state_ != relation_state::idle)
	{
		RX_ASSERT(false);
		return;// not in right state
	}
	if (my_state_ == relation_state::querying)
		my_state_ = relation_state::idle;

	resolve_inverse_name();

	if (target_id.is_null())
	{
		rx_directory_resolver dirs;
		dirs.add_paths({ object_directory });
		auto resolve_result = rx_internal::model::algorithms::resolve_reference(parent_path, dirs);
		if (!resolve_result)
		{
			RUNTIME_LOG_DEBUG("relation_runtime", 100, "Unable to resolve relation reference to "s + parent_path);
			return;
		}
		if (!target_base_id.is_null())
		{
			if (!rx_internal::model::algorithms::is_instanced_from(resolve_result.value(), target_base_id))
			{
				RUNTIME_LOG_DEBUG("relation_runtime", 100, parent_path + " is not valid type for this relation type.");
				return;
			}
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
			connections.local_relation_connected(std::move(item_ptr));

			implementation_->relation_connected();
			if (!is_target_)
			{
				auto target_rel = make_target_relation(context_->meta_info.get_full_path());

				auto add_result = platform_runtime_manager::instance().get_cache().add_target_relation(target_id, target_rel);
				if (!add_result)
					RUNTIME_LOG_ERROR("relation_runtime", 999, "Error adding target relation to "s + parent_path);
			}
		}
		else
		{
			my_state_ = relation_state::remote;

			algorithms::runtime_relation_algorithms::notify_relation_connected(name, item_ptr, context_);
			connections.remote_relation_connected(std::move(item_ptr));

			implementation_->relation_connected();
			if (!is_target_)
			{
				auto target_rel = make_target_relation(context_->meta_info.get_full_path());

				auto add_result = platform_runtime_manager::instance().get_cache().add_target_relation(target_id, target_rel);
				if (!add_result)
					RUNTIME_LOG_ERROR("relation_runtime", 999, "Error adding target relation to "s + parent_path);
			}
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
	str_val.assign_static(ret->target_path.c_str(), rx_time::now());
	ret->value.value = str_val;
	ret->value.value_opt[runtime::structure::value_opt_readonly] = true;
	return ret;
}

rx_result relation_data::start_target_relation (runtime::runtime_start_context& ctx)
{
	object_directory = "/";
	resolver_user_.my_relation = smart_this();
	context_ = ctx.context;
	connections.context = context_;
	executer_ = rx_thread_context();
	is_target_ = true;
	ctx.context->meta_info.get_full_path();

	rx_simple_value val;
	val.assign_static(target_path.c_str());
	value.set_value(std::move(val), ctx.now());

	auto result = implementation_->start_relation(ctx, true);
	if (result)
	{
		rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().add_to_cache(smart_this());

		my_state_ = relation_state::querying;
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
	implementation_->relation_disconnected();
	auto result = implementation_->stop_relation(ctx, true);
	resolver_user_.my_relation = smart_ptr::null_ptr;
	implementation_ = rx_relation_impl_ptr::null_ptr;

	rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().remove_from_cache(get_item_ptr());

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
		if (!is_target_ && target_id)
		{
			auto remove_result = platform_runtime_manager::instance().get_cache().remove_target_relation(target_id, target_relation_name);
			if (!remove_result)
				RUNTIME_LOG_ERROR("relation_runtime", 999, "Error removing target relation from "s + parent_path + " " + remove_result.errors_line());
		}
		implementation_->relation_disconnected();

		connections.relation_disconnected();

		target_id = rx_node_id();
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

rx_result relation_data::write_value (write_data&& data, runtime_process_context* ctx)
{
	
	bool changed = false;
	auto result =  value.write_value(std::move(data), ctx, changed);
	if (result && changed)
	{
		if (my_state_ == relation_state::local_domain
			|| my_state_ == relation_state::same_domain
			|| my_state_ == relation_state::remote)
		{
			runtime_disconnected();
		}


		auto val = value.simple_get_value();
		target_path = val.get_string();

		if (!target_path.empty())
		{
			resolve_inverse_name();

			rx_item_reference ref;
			if (target_id.is_null() && !parent_path.empty())
				ref = parent_path;
			else
				ref = target_id;

			my_state_ = relation_state::querying;
			result = resolver_.start_resolver(ref, &resolver_user_, context_->get_directory_resolver());
		}
	}
	return result;
}

platform_item_ptr relation_data::get_item_ptr () const
{
	return std::make_unique <rx_internal::internal_ns::rx_relation_item_implementation<rx_relation_ptr> >(smart_this());
}

rx_item_type relation_data::get_type_id () const
{
	return rx_item_type::rx_relation;
}

values::rx_value relation_data::get_value () const
{
	return rx_value(value.value);
}

const string_type& relation_data::get_name () const
{
	return name;
}

security::security_guard_ptr relation_data::get_security_guard ()
{
	return context_->get_security_guard();
}


// Class rx_platform::runtime::relations::relations_holder 


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
	if(result)
		implicit_relations_.clear();
	for (auto& one : source_relations_)
	{
		result = one->deinitialize_relation(ctx);
		if (!result)
			break;
	}
	if (result)
		source_relations_.clear();
	for (auto& one : target_relations_)
	{
		if (one)
		{
			one->deinitialize_relation(ctx);
		}
	}
	target_relations_.clear();
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
			//if(one->value.value_opt[runtime::structure::value_opt_persistent])
				one->collect_data(data, type);
		}
	}
}

rx_result relations_holder::browse (const string_type& prefix, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
	for (auto& one : source_relations_)
	{
		runtime_item_attribute attr;
		attr.full_path = prefix.empty() ? one->name : prefix + RX_OBJECT_DELIMETER + one->name;
		attr.name = one->name;
		attr.type = rx_attribute_type::relation_attribute_type;
		attr.value.assign_static(one->target_path.c_str());
		items.push_back(attr);
	}
	for (auto& one : implicit_relations_)
	{
		runtime_item_attribute attr;
		attr.full_path = prefix.empty() ? one->name : prefix + RX_OBJECT_DELIMETER + one->name;
		attr.name = one->name;
		attr.type = rx_attribute_type::relation_target_attribute_type;
		attr.value.assign_static(one->target_path.c_str());
		items.push_back(attr);
	}
	for (auto& one : target_relations_)
	{
		if (one)
		{
			runtime_item_attribute attr;
			attr.full_path = prefix.empty() ? one->name : prefix + RX_OBJECT_DELIMETER + one->name;
			attr.name = one->name;
			attr.type = rx_attribute_type::relation_target_attribute_type;
			attr.value.assign_static(one->target_path.c_str());
			items.push_back(attr);
		}
	}
	return true;
}

void relations_holder::browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback)
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
			one->connections.browse(bellow_prefix, rest_path, filter, std::move(callback));
			return;
		}
	}
	for (auto& one : implicit_relations_)
	{
		if (one->name == sub_path)
		{
			one->connections.browse(bellow_prefix, rest_path, filter, std::move(callback));
			return;
		}
	}
	for (auto& one : target_relations_)
	{
		if (one && one->name == sub_path)
		{
			one->connections.browse(bellow_prefix, rest_path, filter, std::move(callback));
			return;
		}
	}
	callback(path + " not found", std::vector<runtime_item_attribute>());
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
		if (one && one->name == name)
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
		if(!one)
		{
			one = data;
			return one->start_target_relation(ctx);
		}
	}
	auto& one = target_relations_.emplace_back(data);
	return one->start_target_relation(ctx);
}

rx_result_with<relation_data::smart_ptr> relations_holder::remove_target_relation (const string_type& name, runtime::runtime_stop_context& ctx)
{
	relation_data::smart_ptr ret;
	for (auto& one : target_relations_)
	{
		if (one && one->name == name)
		{
			ret = one;
			auto result = one->stop_target_relation(ctx);
			one = relation_data::smart_ptr::null_ptr;
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
	implicit_relations_.emplace_back(std::move(data));
	return true;
}

bool relations_holder::is_this_yours (string_view_type path) const
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
			if (one && one->name == path)
				return true;
		}
	}
	else
	{
		string_view_type name = path.substr(0, idx);
		for (const auto& one : implicit_relations_)
		{
			if (one->name == name)
				return true;
		}
		for (const auto& one : source_relations_)
		{
			if (one->name == name)
				return true;
		}
		for (const auto& one : target_relations_)
		{
			if (one && one->name == name)
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
			if (one && one->name == path)
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
			if (name == one->name)
			{
				relation_subscribers_[name].emplace_back(who);
				return true;
			}
		}
	}
	return "Invalid relation name!";
}

rx_result relations_holder::register_extern_relation_subscriber (const string_type& name, relation_subscriber_data* who)
{
	auto ptr = std::make_unique<extern_relation_subscriber>(who);
	auto result = register_relation_subscriber(name, ptr.get());
	if(result)
		extern_subscribers_.emplace_back(std::move(ptr));
	return result;
}

void relations_holder::read_value (const string_type& path, read_result_callback_t callback, runtime_process_context* ctx) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	if (idx == string_type::npos)
	{
		rx_value val;
		auto result = get_value(path, val, ctx);
		callback(std::move(result), std::move(val));
	}
	else
	{
		string_type sub_path = path.substr(idx + 1);
		size_t len = idx;
		for (const auto& one : implicit_relations_)
		{
			if (one->name.size() == len && memcmp(one->name.c_str(), path.c_str(), len) == 0)
			{
				one->connections.read_value(sub_path, std::move(callback), ctx);
			}
		}
		for (const auto& one : source_relations_)
		{
			if (one->name.size() == len && memcmp(one->name.c_str(), path.c_str(), len) == 0)
			{
				one->connections.read_value(sub_path, std::move(callback), ctx);
			}
		}
		for (const auto& one : target_relations_)
		{
			if (one && one->name.size() == len && memcmp(one->name.c_str(), path.c_str(), len) == 0)
			{
				one->connections.read_value(sub_path, std::move(callback), ctx);
			}
		}
	}
}

rx_result relations_holder::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const
{
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

void relations_holder::read_struct (string_view_type path, read_struct_data data) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_view_type sub_path;
	string_view_type name;
	if (idx != string_type::npos)
	{
		sub_path = path.substr(idx + 1);
		name = path.substr(0, idx);
	}
	else
	{
		name = path;
	}
	for (const auto& one : implicit_relations_)
	{
		if (one->name == name)
		{
			one->connections.read_struct(sub_path, std::move(data));
			return;
		}
	}
	for (const auto& one : source_relations_)
	{
		if (one->name == name)
		{
			one->connections.read_struct(sub_path, std::move(data));
			return;
		}
	}
	for (const auto& one : target_relations_)
	{
		if (one->name == name)
		{
			one->connections.read_struct(sub_path, std::move(data));
			return;
		}
	}
	data.callback("Invalid path", data::runtime_values_data());
}

void relations_holder::write_struct (string_view_type path, write_struct_data data)
{
}


// Class rx_platform::runtime::relations::relation_value_data 


rx_value relation_value_data::get_value (runtime_process_context* ctx) const
{
	if (ctx)
		return ctx->adapt_value(value);
	else
		return value;
}

rx_result relation_value_data::write_value (context_write_data&& data, std::unique_ptr<structure::write_task> task, runtime_process_context* ctx)
{
	if (ctx->get_mode().is_off())
		return "Runtime if in Off state!";

	if(data.test != ctx->get_mode().is_test())
		return "Test mode mismatch!";

	security::secured_scope _(data.identity);
	return parent->write_tag(handle, std::move(data), std::move(task), ctx);
}

rx_result relation_value_data::execute (context_execute_data&& data, std::unique_ptr<structure::execute_task> task, runtime_process_context* ctx)
{
	security::secured_scope _(data.identity);
	return parent->execute_tag(handle, std::move(data), std::move(task), ctx);
}


// Class rx_platform::runtime::relations::relation_connections 


rx_result_with<relations::relation_value_data*> relation_connections::connect_tag (const string_type& path, runtime_handle_t handle)
{
	// duplicates are handled by parent from connected_tags object
	RX_ASSERT(tag_paths_.find(path) == tag_paths_.end());
	auto value_data_ptr = std::make_unique<relation_value_data>();
	value_data_ptr->value.set_quality(RX_NOT_CONNECTED_QUALITY);
	value_data_ptr->value.set_time(rx_time::now());
	value_data_ptr->parent = this;
	value_data_ptr->handle = 0;
	relation_value_data* emplace_result = values_cache_.emplace_back(std::move(value_data_ptr)).get();
	tag_paths_.emplace(path, emplace_result);
	if (connector_)
	{
		string_array paths{ path };
		auto result = connector_->connect_items(paths);
		if (!result.empty() && result[0])
		{
			// check for insane stuff
			RX_ASSERT(handles_map_.find(result[0].value()) == handles_map_.end());
			emplace_result->handle = result[0].value();
			handles_map_.emplace(result[0].value(), emplace_result);
			//inv_handles_map_.emplace(handle, result[0].value());
		}
		else
		{
			RX_ASSERT(result.size() == 1);
		}
	}
	return emplace_result;
}

rx_result relation_connections::write_tag (runtime_handle_t item, context_write_data&& data, std::unique_ptr<structure::write_task> task, runtime_process_context* ctx)
{
	if (connector_)
	{
		auto new_trans = rx_get_new_transaction_id();
		pending_tasks_.emplace(new_trans, std::move(task));
		data.transaction_id = new_trans;
		rx_result result;
		if (std::holds_alternative<rx_simple_value>(data.data))
			result = connector_->write_tag(new_trans, data.test, item, std::get<rx_simple_value>(std::move(data.data)));
		else if (std::holds_alternative<data::runtime_values_data>(data.data))
			result = connector_->write_tag(new_trans, data.test, item, std::get<data::runtime_values_data>(std::move(data.data)));
		else
			result = RX_INTERNAL_ERROR;

		if (!result)
			pending_tasks_.erase(new_trans);

		return result;
	}
	else
	{
		return RX_NOT_CONNECTED;
	}
}

rx_result relation_connections::execute_tag (runtime_handle_t item, context_execute_data&& data, std::unique_ptr<structure::execute_task> task, runtime_process_context* ctx)
{
	if (connector_)
	{
		auto new_trans = rx_get_new_transaction_id();
		pending_execute_tasks_.emplace(new_trans, std::move(task));
		data.transaction_id = new_trans;
		rx_result result;
		if(std::holds_alternative<rx_simple_value>(data.data))
			result = connector_->execute_tag(new_trans, data.test, item, std::move(std::get<rx_simple_value>(data.data)));
		else if (std::holds_alternative<data::runtime_values_data>(data.data))
			result = connector_->execute_tag(new_trans, data.test, item, std::move(std::get<data::runtime_values_data>(data.data)));
		else
			result = RX_INTERNAL_ERROR;

		if (!result)
			pending_execute_tasks_.erase(new_trans);

		return result;
	}
	else
	{
		return RX_NOT_CONNECTED;
	}
}

void relation_connections::browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback)
{
	if (connector_)
	{
		connector_->browse(prefix, path, filter, std::move(callback));
	}
	else if(path.empty())
	{
		callback(true, std::vector<runtime_item_attribute>());
	}
	else
	{
		callback("Wrong object state!", std::vector<runtime_item_attribute>());
	}
}

void relation_connections::read_value (const string_type& path, read_result_callback_t callback, runtime_process_context* ctx) const
{
	if (connector_)
	{
		auto it = tag_paths_.find(path);
		if (it != tag_paths_.end())
		{
			auto value = it->second->get_value(ctx);
			callback(rx_result(true), std::move(value));
		}
		else
		{
			connector_->read_value(path, std::move(callback));
		}
	}
	else
	{
		callback(rx_result(RX_NOT_CONNECTED), rx_value());
	}
}

void relation_connections::read_struct (string_view_type path, read_struct_data data) const
{
	if (connector_)
	{

		connector_->read_struct(path, std::move(data));
	}
	else
	{
		data.callback(rx_result(RX_NOT_CONNECTED), data::runtime_values_data());
	}
}

void relation_connections::local_relation_connected (platform_item_ptr item)
{
	RX_ASSERT(!connector_);
	connector_ = std::make_unique<local_relation_connector>(std::move(item), this);
	if (!tag_paths_.empty())
	{
		RX_ASSERT(handles_map_.empty());// we shouldn't have any connection!
		string_array paths;
		std::vector<relation_value_data*> values;
		size_t tags_count = tag_paths_.size();
		paths.reserve(tags_count);
		values.reserve(tags_count);
		for (const auto& one : tag_paths_)
		{
			paths.emplace_back(one.first);
			values.emplace_back(one.second);
		}
		auto connect_results = connector_->connect_items(paths);
		if (connect_results.size() == tags_count)
		{
			for (size_t i = 0; i < tags_count; i++)
			{
				if (connect_results[i])
				{
					values[i]->handle = connect_results[i].value();
					handles_map_.emplace(connect_results[i].value(), values[i]);
					//inv_handles_map_.emplace(handles[i], connect_results[i].value());
				}
			}
		}
		else
		{
			RX_ASSERT(false);
		}
	}
}

void relation_connections::remote_relation_connected (platform_item_ptr item)
{
	RX_ASSERT(!connector_);
	connector_ = std::make_unique<remote_relation_connector>(std::move(item), this);
	if (!tag_paths_.empty())
	{
		RX_ASSERT(handles_map_.empty());// we shouldn't have any connection!
		string_array paths;
		std::vector<relation_value_data*> values;
		size_t tags_count = tag_paths_.size();
		paths.reserve(tags_count);
		values.reserve(tags_count);
		for (const auto& one : tag_paths_)
		{
			paths.emplace_back(one.first);
			values.emplace_back(one.second);
		}
		auto connect_results = connector_->connect_items(paths);
		if (connect_results.size() == tags_count)
		{
			for (size_t i = 0; i < tags_count; i++)
			{
				if (connect_results[i])
				{
					values[i]->handle = connect_results[i].value();
					handles_map_.emplace(connect_results[i].value(), values[i]);
					//inv_handles_map_.emplace(handles[i], connect_results[i].value());
				}
			}
		}
		else
		{
			RX_ASSERT(false);
		}
	}
}

void relation_connections::relation_disconnected ()
{
	if (connector_)
	{
		if (!handles_map_.empty())
		{
			std::vector<runtime_handle_t> handles;
			handles.reserve(handles_map_.size());
			for (const auto& one : handles_map_)
				handles.emplace_back(one.first);
			auto disconnect_result = connector_->disconnect_items(handles);
			RX_ASSERT(disconnect_result);
		}
		connector_.reset();
		if (!pending_tasks_.empty())
		{
			auto it = pending_tasks_.begin();
			while (it != pending_tasks_.end())
			{
				it->second->process_result(RX_NOT_CONNECTED);
				it = pending_tasks_.erase(it);
			}
		}
	}
	handles_map_.clear();
	rx_time now = rx_time::now();
	for (auto& one : values_cache_)
	{
		one->handle = 0;
		one->value.set_quality(RX_NOT_CONNECTED_QUALITY);
		one->value.set_time(now);
		algorithms::runtime_relation_algorithms::relation_value_change(one.get(), one->value, context);
	}
}

void relation_connections::items_changed (const std::vector<update_item>& items)
{
	for (const auto& one : items)
	{
		auto it = handles_map_.find(one.handle);
		if (it != handles_map_.end())
		{
			it->second->value = one.value;
			algorithms::runtime_relation_algorithms::relation_value_change(it->second,one.value, context);
		}
	}
}

void relation_connections::execute_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, values::rx_simple_value data)
{
	auto it = pending_execute_tasks_.find(transaction_id);
	if (it != pending_execute_tasks_.end())
	{
		it->second->process_result(std::move(result), std::move(data));
		pending_execute_tasks_.erase(it);
	}
}

void relation_connections::execute_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, data::runtime_values_data data)
{
	auto it = pending_execute_tasks_.find(transaction_id);
	if (it != pending_execute_tasks_.end())
	{
		it->second->process_result(std::move(result), std::move(data));
		pending_execute_tasks_.erase(it);
	}
}

void relation_connections::write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result)
{
	auto it = pending_tasks_.find(transaction_id);
	if (it != pending_tasks_.end())
	{
		it->second->process_result(std::move(result));
		pending_tasks_.erase(it);
	}
}


} // namespace relations
} // namespace runtime
} // namespace rx_platform

