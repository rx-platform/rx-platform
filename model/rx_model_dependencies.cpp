

/****************************************************************************
*
*  model\rx_model_dependencies.cpp
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

#include "system/meta/rx_types.h"
#include "system/meta/rx_obj_types.h"
#include "system/runtime/rx_objbase.h"
#include "system/runtime/rx_runtime_holder.h"
#include "rx_model_algorithms.h"
#include "system/server/rx_directory_cache.h"

// rx_model_dependencies
#include "model/rx_model_dependencies.h"



namespace rx_internal {

namespace model {

namespace transactions {

// Class rx_internal::model::transactions::dependency_cache 


void dependency_cache::add_dependency (const rx_node_id& id, const rx_node_id& from)
{
	std::vector<rx_node_id> ids;
	ids.emplace_back(from);
	auto it = depend_of_.find(from);
	if (it != depend_of_.end())
	{
		std::copy(it->second.begin()
			, it->second.end()
			, std::back_inserter(ids));
	}
	// id is dependent from all these
	for (const auto& one : ids)
		add_single_dependency(id, one);
}

void dependency_cache::remove_item (const rx_node_id& id)
{
	auto it = depend_of_.find(id);
	if (it != depend_of_.end())
	{
		for (auto& one : it->second)
		{
			auto sub_it = depend_from_.find(one);
			if (sub_it != depend_from_.end())
			{
				size_t res = sub_it->second.erase(id);
				// we should find it here
				RX_ASSERT(res);
			}
			else
			{
				// this should not happened
				RX_ASSERT(false);
			}
		}
		depend_of_.erase(it);
	}
}

rx_result dependency_cache::fill_dependents (const rx_node_id& id, std::set<rx_node_id>& results)
{
	auto it = depend_from_.find(id);
	if (it != depend_from_.end())
	{
		for (auto& one : it->second)
		{
			results.emplace(one);
		}
	}
	return true;
}

void dependency_cache::add_single_dependency (const rx_node_id& id, const rx_node_id& from)
{
	auto it_of = depend_of_.find(id);
	if (it_of != depend_of_.end())
	{
		it_of->second.emplace(from);
	}
	else
	{
		cached_items temp{ from };
		auto res = depend_of_.emplace(id, temp);
		if (res.second)
			it_of = res.first;
	}
	auto it = depend_from_.find(from);
	if (it != depend_from_.end())
	{
		it->second.emplace(id);
	}
	else
	{
		cached_items temp{ id };
		depend_from_.emplace(from, temp);
	}
}


// Class rx_internal::model::transactions::local_dependecy_builder 


void local_dependecy_builder::add_runtime (const object_runtime_data& what, bool remove, bool create, bool save)
{
	add_runtime_concrete(objects_, what, remove, create, save);
}

void local_dependecy_builder::add_runtime (const domain_runtime_data& what, bool remove, bool create, bool save)
{
	add_runtime_concrete(domains_, what, remove, create, save);
}

void local_dependecy_builder::add_runtime (const port_runtime_data& what, bool remove, bool create, bool save)
{
	add_runtime_concrete(ports_, what, remove, create, save);
}

void local_dependecy_builder::add_runtime (const application_runtime_data& what, bool remove, bool create, bool save)
{
	add_runtime_concrete(applications_, what, remove, create, save);
}

void local_dependecy_builder::add_type (rx_application_type_ptr what, bool remove, bool create, bool save)
{
	add_type_concrete(application_types_, what, remove, create, save);
}

void local_dependecy_builder::add_type (rx_domain_type_ptr what, bool remove, bool create, bool save)
{
	add_type_concrete(domain_types_, what, remove, create, save);
}

void local_dependecy_builder::add_type (rx_port_type_ptr what, bool remove, bool create, bool save)
{
	add_type_concrete(port_types_, what, remove, create, save);
}

void local_dependecy_builder::add_type (rx_object_type_ptr what, bool remove, bool create, bool save)
{
	add_type_concrete(object_types_, what, remove, create, save);
}

void local_dependecy_builder::add_sub_type (struct_type_ptr what, bool remove, bool create, bool save)
{
	add_type_concrete(struct_types_, what, remove, create, save);
}

void local_dependecy_builder::add_sub_type (variable_type_ptr what, bool remove, bool create, bool save)
{
	add_type_concrete(variable_types_, what, remove, create, save);
}

rx_result local_dependecy_builder::add_query_result (const api::query_result_detail& what, bool remove, bool create, bool save)
{
	rx_result res;
	switch (what.type)
	{
	case rx_application:
		res = add_query_runtime_concrete<application_type>(applications_, what, remove, create, save);
		break;
	case rx_domain:
		res = add_query_runtime_concrete<domain_type>(domains_, what, remove, create, save);
		break;
	case rx_port:
		res = add_query_runtime_concrete<port_type>(ports_, what, remove, create, save);
		break;
	case rx_object:
		res = add_query_runtime_concrete<object_type>(objects_, what, remove, create, save);
		break;

	case rx_application_type:
		res = add_query_type_concrete<application_type>(application_types_, what, remove, create, save);
		break;
	case rx_domain_type:
		res = add_query_type_concrete<domain_type>(domain_types_, what, remove, create, save);
		break;
	case rx_port_type:
		res = add_query_type_concrete<port_type>(port_types_, what, remove, create, save);
		break;
	case rx_object_type:
		res = add_query_type_concrete<object_type>(object_types_, what, remove, create, save);
		break;

	case rx_struct_type:
		res = add_query_simple_type_concrete<struct_type>(struct_types_, what, remove, create, save);
		break;
	case rx_variable_type:
		res = add_query_simple_type_concrete<variable_type>(variable_types_, what, remove, create, save);
		break;
	default:
		return RX_NOT_IMPLEMENTED;
	}
	return res;
}

void local_dependecy_builder::add_config_part (const meta::config_part_container& part)
{
	for (const auto& one : part.apps)
	{
		add_runtime(*one, false, true, true);
	}
	for (const auto& one : part.domains)
	{
		add_runtime(*one, false, true, true);
	}
	for (const auto& one : part.ports)
	{
		add_runtime(*one, false, true, true);
	}
	for (const auto& one : part.objects)
	{
		add_runtime(*one, false, true, true);
	}
	for (const auto& one : part.app_types)
	{
		add_type(one, false, true, true);
	}
	for (const auto& one : part.domain_types)
	{
		add_type(one, false, true, true);
	}
	for (const auto& one : part.port_types)
	{
		add_type(one, false, true, true);
	}
	for (const auto& one : part.object_types)
	{
		add_type(one, false, true, true);
	}
	for (const auto& one : part.struct_types)
	{
		add_sub_type(one, false, true, true);
	}
	for (const auto& one : part.variable_types)
	{
		add_sub_type(one, false, true, true);
	}
}

rx_result local_dependecy_builder::apply_items (rx_result_callback&& callback)
{
	// consolidate meta data for all items if needed
	do_consolidate_for_item(applications_);
	do_consolidate_for_item(domains_);
	do_consolidate_for_item(ports_);
	do_consolidate_for_item(objects_);

	do_consolidate_for_types(application_types_);
	do_consolidate_for_types(domain_types_);
	do_consolidate_for_types(port_types_);
	do_consolidate_for_types(object_types_);

	do_consolidate_for_types(struct_types_);
	do_consolidate_for_types(variable_types_);

	callback_ = std::move(callback);
	state_.phase = builder_phase::idle;
	state_.objects_it = objects_.begin();
	state_.ports_it = ports_.begin();
	state_.domains_it = domains_.begin();
	state_.applications_it = applications_.begin();
	process(true);
	return true;
}

rx_result local_dependecy_builder::delete_types ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_types);

	rx_result ret = delete_types<application_type>(application_types_);
	if (!ret)
		return ret;
	ret = delete_types<domain_type>(domain_types_);
	if (!ret)
		return ret;
	ret = delete_types<port_type>(port_types_);
	if (!ret)
		return ret;
	ret = delete_types<object_type>(object_types_);
	if (!ret)
		return ret;
	ret = delete_simple_types<struct_type>(struct_types_);
	if (!ret)
		return ret;
	ret = delete_simple_types<variable_type>(variable_types_);
	if (!ret)
		return ret;

	state_.phase = builder_phase::deleting_directories;

	return true;
}

rx_result local_dependecy_builder::delete_objects ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_objects);
	bool waiting = false;
	while (state_.objects_it != objects_.end())
	{
		if (state_.objects_it->second.remove)
		{
			rx_node_id id = state_.objects_it->second.item.meta_info.id;
			state_.objects_it++;

			rx_result_callback callback(smart_this(), [this](rx_result&& res)
				{
					process(std::move(res));
				});
			auto result = algorithms::runtime_model_algorithm<object_types::object_type>::delete_runtime_sync(
				id, RX_DOMAIN_META, std::move(callback));
			if (!result)
			{
				return result.errors();
			}
			else
			{
				waiting = true;
				break;
			}
		}
		else
		{
			state_.objects_it++;
		}
	}
	if (!waiting)
	{
		state_.phase = builder_phase::deleting_ports;
		process(true);
	}

	return true;
}

rx_result local_dependecy_builder::delete_ports ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_ports);
	bool waiting = false;
	while (state_.ports_it != ports_.end())
	{
		if (state_.ports_it->second.remove)
		{
			rx_node_id id = state_.ports_it->second.item.meta_info.id;
			state_.ports_it++;

			rx_result_callback callback(smart_this(), [this](rx_result&& res)
				{
					process(std::move(res));
				});
			auto result = algorithms::runtime_model_algorithm<object_types::port_type>::delete_runtime_sync(
				id, RX_DOMAIN_META, std::move(callback));
			if (!result)
			{
				return result.errors();
			}
			else
			{
				waiting = true;
				break;
			}
		}
		else
		{
			state_.ports_it++;
		}
	}
	if (!waiting)
	{
		state_.phase = builder_phase::deleting_domains;
		process(true);
	}
	return true;
}

rx_result local_dependecy_builder::delete_domains ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_domains);
	bool waiting = false;
	while (state_.domains_it != domains_.end())
	{
		if (state_.domains_it->second.remove)
		{
			rx_node_id id = state_.domains_it->second.item.meta_info.id;
			state_.domains_it++;

			rx_result_callback callback(smart_this(), [this](rx_result&& res)
				{
					process(std::move(res));
				});
			auto result = algorithms::runtime_model_algorithm<object_types::domain_type>::delete_runtime_sync(
				id, RX_DOMAIN_META, std::move(callback));
			if (!result)
			{
				return result.errors();
			}
			else
			{
				waiting = true;
				break;
			}
		}
		else
		{
			state_.domains_it++;
		}
	}
	if (!waiting)
	{
		state_.phase = builder_phase::deleting_apps;
		process(true);
	}
	return true;
}

rx_result local_dependecy_builder::delete_apps ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_apps);
	bool waiting = false;
	while (state_.applications_it != applications_.end())
	{
		if (state_.applications_it->second.remove)
		{
			rx_node_id id = state_.applications_it->second.item.meta_info.id;
			state_.applications_it++;

			rx_result_callback callback(smart_this(), [this](rx_result&& res)
				{
					process(std::move(res));
				});
			auto result = algorithms::runtime_model_algorithm<object_types::application_type>::delete_runtime_sync(
				id, RX_DOMAIN_META, std::move(callback));
			if (!result)
			{
				return result.errors();
			}
			else
			{
				waiting = true;
				break;
			}
		}
		else
		{
			state_.applications_it++;
		}
	}
	if (!waiting)
	{
		state_.phase = builder_phase::deleting_types;
		process(true);
	}
	return true;
}

rx_result local_dependecy_builder::delete_directories ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_directories);
	for (auto it = dirs_to_delete_.rbegin(); it!=dirs_to_delete_.rend(); it++)
	{
		auto ret = ns::rx_directory_cache::instance().remove_directory(*it);
		if (!ret)
			return ret;
	}
	state_.phase = builder_phase::building_directories;
	return true;
}

rx_result local_dependecy_builder::build_directories ()
{
	RX_ASSERT(state_.phase == builder_phase::building_directories);
	for (auto it = dirs_to_create_.begin(); it != dirs_to_create_.end(); it++)
	{
		auto ret = ns::rx_directory_cache::instance().get_or_create_directory(*it);
		if (!ret)
			return ret.errors();
	}
	state_.phase = builder_phase::building_types;
	return true;
}

rx_result local_dependecy_builder::build_types ()
{
	RX_ASSERT(state_.phase == builder_phase::building_types);

	rx_result ret = create_simple_types<variable_type>(variable_types_, built_variable_types_);
	if (!ret)
		return ret;
	ret = create_simple_types<struct_type>(struct_types_, built_struct_types_);
	if (!ret)
		return ret;

	ret = create_types<application_type>(application_types_, built_app_types_);
	if (!ret)
		return ret;
	ret = create_types<domain_type>(domain_types_, built_domain_types_);
	if (!ret)
		return ret;
	ret = create_types<port_type>(port_types_, built_port_types_);
	if (!ret)
		return ret;
	ret = create_types<object_type>(object_types_, built_object_types_);
	if (!ret)
		return ret;
	state_.phase = builder_phase::building_runtimes;

	return true;
}

rx_result local_dependecy_builder::build_runtimes ()
{
	RX_ASSERT(state_.phase == builder_phase::building_runtimes);
	rx_result ret = true;
	for (auto& one : applications_)
	{
		if (one.second.create)
		{
			auto result = algorithms::runtime_model_algorithm<object_types::application_type>::create_runtime_sync(
				application_runtime_data(one.second.item), data::runtime_values_data());
			if (!result)
				return result.errors();
			if (one.second.save_result)
				built_apps_.emplace_back(result.move_value());
		}
	}
	for (auto& one : ports_)
	{
		if (one.second.create)
		{
			auto result = algorithms::runtime_model_algorithm<object_types::port_type>::create_runtime_sync(
				port_runtime_data(one.second.item), data::runtime_values_data());
			if (!result)
				return result.errors();
			if (one.second.save_result)
				built_ports_.emplace_back(result.move_value());
		}
	}
	for (auto& one : domains_)
	{
		if (one.second.create)
		{
			auto result = algorithms::runtime_model_algorithm<object_types::domain_type>::create_runtime_sync(
				domain_runtime_data(one.second.item), data::runtime_values_data());
			if (!result)
				return result.errors();
			if (one.second.save_result)
				built_domains_.emplace_back(result.move_value());
		}
	}
	for (auto& one : objects_)
	{
		if (one.second.create)
		{
			auto result = algorithms::runtime_model_algorithm<object_types::object_type>::create_runtime_sync(
				object_runtime_data(one.second.item), data::runtime_values_data());
			if (!result)
				return result.errors();
			if (one.second.save_result)
				built_objects_.emplace_back(result.move_value());
		}
	}
	state_.phase = builder_phase::done;
	return ret;
}

void local_dependecy_builder::process (rx_result&& result)
{
	if (!result)
	{
		state_.phase = builder_phase::done;
		callback_(std::move(result));
		return;
	}
	rx_result res = true;
	switch (state_.phase)
	{
	case builder_phase::idle:
		state_.phase = builder_phase::deleting_objects;
		res = delete_objects();
		break;
	case builder_phase::deleting_objects:
		res = delete_objects();
		break;
	case builder_phase::deleting_ports:
		res = delete_ports();
		break;
	case builder_phase::deleting_domains:
		res = delete_domains();
		break;
	case builder_phase::deleting_apps:
		res = delete_apps();
		break;
	case builder_phase::deleting_types:
		res = delete_types();
		if(!res)
			break;
		[[fallthrough]];
	case builder_phase::deleting_directories:
		res = delete_directories();
		if (!res)
			break;
		[[fallthrough]];
	case builder_phase::building_directories:
		res = build_directories();
		if (!res)
			break;
		[[fallthrough]];
	case builder_phase::building_types:
		res = build_types();
		if(!res)
			break;
		[[fallthrough]];
	case builder_phase::building_runtimes:
		res = build_runtimes();
		if(!res)
			break;
		[[fallthrough]];
	case builder_phase::done:
		callback_(std::move(res));
		break;
	};
	if (!res)
	{
		state_.phase = builder_phase::done;
		callback_(std::move(res));
	}
}

rx_result local_dependecy_builder::consolidate_meta_data (meta::meta_data& new_data, const meta::meta_data& old_data)
{
	new_data.id = old_data.id;
	if (old_data.created_time.is_valid_time())
		new_data.created_time = old_data.created_time;
	if (new_data.version < old_data.version)
	{
		new_data.version = old_data.version;
		new_data.increment_version(false);
	}
	new_data.attributes = new_data.attributes | old_data.attributes;
	return true;
}

rx_result local_dependecy_builder::consolidate_meta_data (meta::meta_data& data)
{
	if (data.id.is_null())
		data.id = rx_node_id::generate_new();
	if (data.attributes == 0)
		data.attributes = namespace_item_full_access;
	if(!data.modified_time.is_valid_time())
		data.modified_time = rx_time::now();
	if (data.version == 0)
		data.version = RX_INITIAL_ITEM_VERSION;
	return true;
}


template<typename T>
void local_dependecy_builder::do_consolidate_for_item(T& container)
{
	for (auto& one : container)
	{
		if (one.second.remove)
			dirs_to_delete_.emplace(one.second.item.meta_info.path);
		if (one.second.create)
		{
			dirs_to_create_.emplace(one.second.item.meta_info.path);
			consolidate_meta_data(one.second.item.meta_info);
		}
	}
}
template<typename T, typename dataT>
void local_dependecy_builder::add_runtime_concrete(T& container, const dataT& what, bool remove, bool create, bool save)
{
	string_type path = what.meta_info.get_full_path();
	auto it = container.find(path);
	if (it == container.end())
	{
		item_creation_data<dataT> data;
		data.item = what;
		data.create = create;
		data.remove = remove;
		data.save_result = save;
		container.insert_or_assign(path, std::move(data));
	}
	else
	{
		it->second.create |= create;
		it->second.remove |= remove;
		it->second.save_result |= save;
		it->second.item = what;
	}
}

template<typename T, typename dataT>
void local_dependecy_builder::add_type_concrete(T& container, const dataT& what, bool remove, bool create, bool save)
{
	string_type path = what->meta_info.get_full_path();
	auto it = container.find(path);
	if (it == container.end())
	{
		item_creation_data<dataT> data;
		data.item = what;
		data.create = create;
		data.remove = remove;
		data.save_result = save;
		container.insert_or_assign(path, std::move(data));
	}
	else
	{
		it->second.create |= create;
		it->second.remove |= remove;
		it->second.save_result |= save;
		it->second.item = what;
	}
}


template<typename typeT>
rx_result local_dependecy_builder::add_query_runtime_concrete(local_dependecy_builder::rt_container_t<typeT>& container, const api::query_result_detail& what, bool remove, bool create, bool save)
{

	using instance_type_t = typename typeT::instance_data_t;
	auto result = platform_types_manager::instance().get_type_repository<typeT>().get_runtime(what.data.id, false);
	if (result)
	{
		auto it = container.find(what.data.get_full_path());
		if (it == container.end())
		{
			item_creation_data<instance_type_t> data;
			data.item = result.value()->get_definition_data();
			if (what.data.version > data.item.meta_info.version)
				data.item.meta_info.version = what.data.version;
			data.create = create;
			data.remove = remove;
			data.save_result = save;
			container.emplace(what.data.get_full_path(), data);
			return true;
		}
		else if (remove)
		{
			auto res = consolidate_meta_data(it->second.item.meta_info, what.data);
			if (!res)
				return res;
			it->second.remove = true;
			return true;
		}
		else
		{
			RX_ASSERT(false);
			return "Internal error!!!";
		}
	}
	else
	{
		return result.errors();
	}
}

template<typename typeT>
rx_result local_dependecy_builder::add_query_type_concrete(local_dependecy_builder::container_t<typeT>& container, const api::query_result_detail& what, bool remove, bool create, bool save)
{
	using type_ptr_t = typename typeT::smart_ptr;
	auto result = platform_types_manager::instance().get_type_repository<typeT>().get_type_definition(what.data.id);
	if (result)
	{
		auto it = container.find(what.data.get_full_path());
		if (it == container.end())
		{
			item_creation_data<type_ptr_t> data;
			data.item = result.value();
			if (what.data.version > data.item->meta_info.version)
				data.item->meta_info.version = what.data.version;
			data.create = create;
			data.remove = remove;
			data.save_result = save;
			container.emplace(what.data.get_full_path(), data);
			return true;
		}
		else if (remove)
		{
			auto res = consolidate_meta_data(it->second.item->meta_info, what.data);
			if (!res)
				return res;
			it->second.remove = true;
			return true;
		}
		else
		{
			RX_ASSERT(false);
			return "Internal error!!!";
		}
	}
	else
	{
		return result.errors();
	}
}

template<typename typeT>
rx_result local_dependecy_builder::add_query_simple_type_concrete(local_dependecy_builder::container_t<typeT>& container, const api::query_result_detail& what, bool remove, bool create, bool save)
{
	using type_ptr_t = typename typeT::smart_ptr;
	auto result = platform_types_manager::instance().get_simple_type_repository<typeT>().get_type_definition(what.data.id);
	if (result)
	{
		auto it = container.find(what.data.get_full_path());
		if (it == container.end())
		{
			item_creation_data<type_ptr_t> data;
			data.item = result.value();
			if (what.data.version > data.item->meta_info.version)
				data.item->meta_info.version = what.data.version;
			data.create = create;
			data.remove = remove;
			data.save_result = save;
			container.emplace(what.data.get_full_path(), data);
			return true;
		}
		else if (remove)
		{
			auto res = consolidate_meta_data(it->second.item->meta_info, what.data);
			if (!res)
				return res;
			it->second.remove = true;
			return true;
		}
		else
		{
			RX_ASSERT(false);
			return "Internal error!!!";
		}
	}
	else
	{
		return result.errors();
	}
}
template<typename typeT>
rx_result local_dependecy_builder::create_types(local_dependecy_builder::container_t<typeT>& data, std::vector<typename typeT::smart_ptr>& built)
{
	rx_result ret = true;
	std::vector<string_type> add_order;
	std::vector<std::pair<string_type, string_type> > local_to_add;
	local_to_add.reserve(data.size());
	add_order.reserve(data.size());
	for (const auto& one : data)
	{
		if (one.second.create)
		{
			ns::rx_directory_resolver dirs;
			dirs.add_paths({ one.second.item->meta_info.path });
			auto parent_id = algorithms::resolve_reference(one.second.item->meta_info.parent, dirs);
			if (parent_id)
			{
				local_to_add.emplace_back(one.second.item->meta_info.get_full_path(), string_type());
			}
			else
			{
				auto it_local = data.find(one.second.item->meta_info.parent.to_string());
				if (it_local != data.end())
				{
					local_to_add.emplace_back(one.second.item->meta_info.get_full_path(), one.second.item->meta_info.parent.to_string());
				}
				else
				{//!!!
					local_to_add.emplace_back(one.second.item->meta_info.get_full_path(), string_type());
				}
			}
		}
	}
	std::set<string_type> to_add;
	// first add all items to set for faster search
	for (const auto& one : local_to_add)
		to_add.insert(one.first);

	while (!to_add.empty())
	{
		// check for items not dependent on any items and add them next
		for (auto& one : local_to_add)
		{
			if (!one.first.empty())
			{
				auto it_help = to_add.find(one.second);
				if (it_help == to_add.end())
				{
					add_order.push_back(one.first);
					to_add.erase(one.first);
					one.first.clear();
				}
			}
		}
	}
	for (auto& one : add_order)
	{
		auto it = data.find(one);
		RX_ASSERT(it != data.end());
		if (it != data.end())
		{
			if (it->second.create)
			{
				auto result = algorithms::types_model_algorithm<typeT>::create_type_sync(
					it->second.item);
				if (!result)
					return result.errors();
				if (it->second.save_result)
					built.emplace_back(result.move_value());
			}
		}
	}
	return ret;
}

template<typename typeT>
rx_result local_dependecy_builder::delete_types(container_t<typeT>& data)
{
	rx_result ret = true;
	for (auto it = data.rbegin(); it!=data.rend(); it++)
	{
		if (it->second.remove)
		{
			auto result = algorithms::types_model_algorithm<typeT>::delete_type_sync(
				it->second.item->meta_info.id);
			if (!result)
				return result.errors();
		}
	}
	return ret;
}


template<typename typeT>
rx_result local_dependecy_builder::create_simple_types(local_dependecy_builder::container_t<typeT>& data, std::vector<typename typeT::smart_ptr>& built)
{
	rx_result ret = true;
	std::vector<string_type> add_order;
	std::vector<std::pair<string_type, string_type> > local_to_add;
	local_to_add.reserve(data.size());
	add_order.reserve(data.size());
	for (const auto& one : data)
	{
		if (one.second.create)
		{
			ns::rx_directory_resolver dirs;
			dirs.add_paths({ one.second.item->meta_info.path });
			auto parent_id = algorithms::resolve_reference(one.second.item->meta_info.parent, dirs);
			if (parent_id)
			{
				local_to_add.emplace_back(one.second.item->meta_info.get_full_path(), string_type());
			}
			else
			{
				auto it_local = data.find(one.second.item->meta_info.parent.to_string());
				if (it_local != data.end())
				{
					local_to_add.emplace_back(one.second.item->meta_info.get_full_path(), one.second.item->meta_info.parent.to_string());
				}
				else
				{//!!!
					local_to_add.emplace_back(one.second.item->meta_info.get_full_path(), string_type());
				}
			}
		}
	}
	std::set<string_type> to_add;
	// first add all items to set for faster search
	for (const auto& one : local_to_add)
		to_add.insert(one.first);

	while (!to_add.empty())
	{
		// check for items not dependent on any items and add them next
		for (auto& one : local_to_add)
		{
			if (!one.first.empty())
			{
				auto it_help = to_add.find(one.second);
				if (it_help == to_add.end())
				{
					add_order.push_back(one.first);
					to_add.erase(one.first);
					one.first.clear();
				}
			}
		}
	}
	for (auto& one : add_order)
	{
		auto it = data.find(one);
		RX_ASSERT(it != data.end());
		if (it != data.end())
		{
			if (it->second.create)
			{
				auto result = algorithms::simple_types_model_algorithm<typeT>::create_type_sync(
					it->second.item);
				if (!result)
					return result.errors();
				if (it->second.save_result)
					built.emplace_back(result.move_value());
			}
		}
	}
	return ret;
}

template<typename typeT>
rx_result local_dependecy_builder::delete_simple_types(container_t<typeT>& data)
{
	rx_result ret = true; std::vector<string_type> add_order;
	std::vector<std::pair<string_type, string_type> > local_to_add;
	local_to_add.reserve(data.size());
	add_order.reserve(data.size());
	for (const auto& one : data)
	{
		if (one.second.remove)
		{
			auto it_local = data.find(one.second.item->meta_info.parent.to_string());
			if (it_local != data.end())
			{
				local_to_add.emplace_back(one.second.item->meta_info.get_full_path(), one.second.item->meta_info.parent.to_string());
			}
			else
			{//!!!
				local_to_add.emplace_back(one.second.item->meta_info.get_full_path(), string_type());
			}
		}
	}
	std::set<string_type> to_add;
	// first add all items to set for faster search
	for (const auto& one : local_to_add)
		to_add.insert(one.first);

	while (!to_add.empty())
	{
		// check for items not dependent on any items and add them next
		for (auto& one : local_to_add)
		{
			if (!one.first.empty())
			{
				auto it_help = to_add.find(one.second);
				if (it_help == to_add.end())
				{
					add_order.push_back(one.first);
					to_add.erase(one.first);
					one.first.clear();
				}
			}
		}
	}
	// just reverse the order
	for (auto it_str = add_order.rbegin(); it_str != add_order.rend(); it_str++)
	{
		auto it = data.find(*it_str);
		RX_ASSERT(it != data.end());
		if (it != data.end())
		{
			if (it->second.remove)
			{
				auto result = algorithms::simple_types_model_algorithm<typeT>::delete_type_sync(
					it->second.item->meta_info.id);
				if (!result)
					return result.errors();
			}
		}
	}
	return ret;
}

template<typename T>
void local_dependecy_builder::do_consolidate_for_types(T& container)
{
	for (auto& one : container)
	{
		if (one.second.remove)
			dirs_to_delete_.emplace(one.second.item->meta_info.path);
		if (one.second.create)
		{
			dirs_to_create_.emplace(one.second.item->meta_info.path);
			consolidate_meta_data(one.second.item->meta_info);
		}
	}
}

template<>
rx_application_ptr local_dependecy_builder::extract_single_result<application_type>()
{
	if (built_apps_.empty())
		return rx_application_ptr();
	else
		return built_apps_[0];
}
template<>
rx_domain_ptr local_dependecy_builder::extract_single_result<domain_type>()
{
	if (built_domains_.empty())
		return rx_domain_ptr();
	else
		return built_domains_[0];
}
template<>
rx_port_ptr local_dependecy_builder::extract_single_result<port_type>()
{
	if (built_ports_.empty())
		return rx_port_ptr();
	else
		return built_ports_[0];
}
template<>
rx_object_ptr local_dependecy_builder::extract_single_result<object_type>()
{
	if (built_objects_.empty())
		return rx_object_ptr();
	else
		return built_objects_[0];
}
} // namespace transactions
} // namespace model
} // namespace rx_internal

