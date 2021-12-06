

/****************************************************************************
*
*  model\rx_model_dependencies.cpp
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

#include "system/meta/rx_types.h"
#include "system/meta/rx_obj_types.h"
#include "system/runtime/rx_objbase.h"
#include "system/runtime/rx_runtime_holder.h"
#include "rx_model_algorithms.h"

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
	rx_node_id id = what.meta_info.id;
	auto it = objects_.find(id);
	if(it==objects_.end())
	{
		item_creation_data<object_runtime_data> data;
		data.item = what;
		data.create = create;
		data.remove = remove;
		data.save_result = save;
		objects_.emplace(id, std::move(data));
	}
	else
	{
		it->second.create |= create;
		it->second.remove |= remove;
		it->second.save_result |= save;
		it->second.item = what;
	}
}

void local_dependecy_builder::add_runtime (const domain_runtime_data& what, bool remove, bool create, bool save)
{
	rx_node_id id = what.meta_info.id;
	auto it = domains_.find(id);
	if (it == domains_.end())
	{
		item_creation_data<domain_runtime_data> data;
		data.item = what;
		data.create = create;
		data.remove = remove;
		data.save_result = save;
		domains_.emplace(id, std::move(data));
	}
	else
	{
		it->second.create |= create;
		it->second.remove |= remove;
		it->second.save_result |= save;
		it->second.item = what;
	}
}

void local_dependecy_builder::add_runtime (const port_runtime_data& what, bool remove, bool create, bool save)
{
	rx_node_id id = what.meta_info.id;
	auto it = ports_.find(id);
	if (it == ports_.end())
	{
		item_creation_data<port_runtime_data> data;
		data.item = what;
		data.create = create;
		data.remove = remove;
		data.save_result = save;
		ports_.emplace(id, std::move(data));
	}
	else
	{
		it->second.create |= create;
		it->second.remove |= remove;
		it->second.save_result |= save;
		it->second.item = what;
	}
}

void local_dependecy_builder::add_runtime (const application_runtime_data& what, bool remove, bool create, bool save)
{
	rx_node_id id = what.meta_info.id;
	auto it = applications_.find(id);
	if (it == applications_.end())
	{
		item_creation_data<application_runtime_data> data;
		data.item = what;
		data.create = create;
		data.remove = remove;
		data.save_result = save;
		applications_.emplace(id, std::move(data));
	}
	else
	{
		it->second.create |= create;
		it->second.remove |= remove;
		it->second.save_result |= save;
		it->second.item = what;
	}
}

rx_result local_dependecy_builder::add (const api::query_result_detail& what, bool remove, bool create, bool save)
{
	switch (what.type)
	{
	case rx_application:
		{
			auto result = platform_types_manager::instance().get_type_repository<application_type>().get_runtime(what.data.id, false);
			if (result)
			{
				item_creation_data<runtime_data::application_runtime_data> data;
				data.item = result.value()->get_definition_data();
				if(what.data.version> data.item.meta_info.version)
					data.item.meta_info.version = what.data.version;
				data.create = create;
				data.remove = remove;
				data.save_result = save;
				applications_.emplace(what.data.id, data);
				return true;
			}
			else
			{
				return result.errors();
			}
		}
		break;
	case rx_domain:
		{
			auto result = platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(what.data.id, false);
			if (result)
			{
				item_creation_data<runtime_data::domain_runtime_data> data;
				data.item = result.value()->get_definition_data();
				if (what.data.version > data.item.meta_info.version)
					data.item.meta_info.version = what.data.version;
				data.create = create;
				data.remove = remove;
				data.save_result = save;
				domains_.emplace(what.data.id, data);
				return true;
			}
			else
			{
				return result.errors();
			}
		}
		break;
	case rx_port:
		{
			auto result = platform_types_manager::instance().get_type_repository<port_type>().get_runtime(what.data.id, false);
			if (result)
			{
				item_creation_data<runtime_data::port_runtime_data> data;
				data.item = result.value()->get_definition_data();
				if (what.data.version > data.item.meta_info.version)
					data.item.meta_info.version = what.data.version;
				data.create = create;
				data.remove = remove;
				data.save_result = save;
				ports_.emplace(what.data.id, data);
				return true;
			}
			else
			{
				return result.errors();
			}
		}
		break;
	case rx_object:
		{
			auto result = platform_types_manager::instance().get_type_repository<object_type>().get_runtime(what.data.id, false);
			if (result)
			{
				item_creation_data<runtime_data::object_runtime_data> data;
				data.item = result.value()->get_definition_data();
				if (what.data.version > data.item.meta_info.version)
					data.item.meta_info.version = what.data.version;
				data.create = create;
				data.remove = remove;
				data.save_result = save;
				objects_.emplace(what.data.id, data);
				return true;
			}
			else
			{
				return result.errors();
			}
		}
	default:
		return RX_NOT_IMPLEMENTED;
	}
}

rx_result local_dependecy_builder::apply_items (rx_result_callback&& callback)
{
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
	state_.phase = builder_phase::bulding_types;
	return true;
}

rx_result local_dependecy_builder::delete_objects ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_objects);
	if (state_.objects_it != objects_.end())
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
			return result.errors();
	}
	else
	{
		state_.phase = builder_phase::deleting_ports;
		process(true);
	}
	return true;
}

rx_result local_dependecy_builder::delete_ports ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_ports);
	if (state_.ports_it != ports_.end())
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
			return result.errors();
	}
	else
	{
		state_.phase = builder_phase::deleting_domains;
		process(true);
	}
	return true;
}

rx_result local_dependecy_builder::delete_domains ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_domains);
	if (state_.domains_it != domains_.end())
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
			return result.errors();
	}
	else
	{
		state_.phase = builder_phase::deleting_apps;
		process(true);
	}
	return true;
}

rx_result local_dependecy_builder::delete_apps ()
{
	RX_ASSERT(state_.phase == builder_phase::deleting_apps);
	if (state_.applications_it != applications_.end())
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
			return result.errors();
	}
	else
	{
		state_.phase = builder_phase::deleting_types;
		process(true);
	}
	return true;
}

rx_result local_dependecy_builder::build_types ()
{
	RX_ASSERT(state_.phase == builder_phase::bulding_types);
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
	case builder_phase::bulding_types:
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

