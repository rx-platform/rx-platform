

/****************************************************************************
*
*  model\rx_meta_internals.cpp
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


// rx_meta_internals
#include "model/rx_meta_internals.h"

#include "runtime_internal/rx_runtime_internal.h"
#include "rx_model_algorithms.h"
#include "system/runtime/rx_holder_algorithms.h"
#include "system/meta/rx_meta_algorithm.h"
#include "lib/security/rx_security.h"
using namespace rx;


namespace rx_internal {

namespace model {

// Class rx_internal::model::platform_types_manager 

platform_types_manager::platform_types_manager()
{
}



platform_types_manager& platform_types_manager::instance ()
{
	static platform_types_manager g_instance;
	return g_instance;//ROOT of CLASSES!!!! $$$ Important Object Here!!!
}

rx_result platform_types_manager::initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data)
{

	auto result = get_data_types_repository().initialize(host, data);
	if (!result)
		return result;

	result = get_simple_type_repository<basic_types::struct_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_simple_type_repository<basic_types::variable_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_simple_type_repository<basic_types::source_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_simple_type_repository<basic_types::filter_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_simple_type_repository<basic_types::event_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_simple_type_repository<basic_types::mapper_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_simple_type_repository<basic_types::method_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_simple_type_repository<basic_types::program_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_simple_type_repository<basic_types::display_type>().initialize(host, data);
	if (!result)
		return result;

	result = get_relations_repository().initialize(host, data);
	if (!result)
		return result;

	result = get_type_repository<object_types::application_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_type_repository<object_types::domain_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_type_repository<object_types::port_type>().initialize(host, data);
	if (!result)
		return result;
	result = get_type_repository<object_types::object_type>().initialize(host, data);
	if (!result)
		return result;

	return result;
}

void platform_types_manager::deinitialize ()
{
}

rx_result platform_types_manager::start (hosting::rx_platform_host* host, const meta_configuration_data_t& data)
{
	return true;
}

void platform_types_manager::stop ()
{
}


// Class rx_internal::model::relations_hash_data 

relations_hash_data::relations_hash_data()
{
}


relations_hash_data::~relations_hash_data()
{
}



void relations_hash_data::add_to_hash_data (const rx_node_id& new_id, const rx_node_id& first_backward)
{
	std::vector<rx_node_id> super_array;
	auto itf = backward_hash_.find(first_backward);
	if (itf != backward_hash_.end())
	{
		// reserve the memory, you might need it
		super_array.reserve(itf->second->size() + 1);
		super_array.push_back(first_backward);
		for (auto ite : (*itf->second))
			super_array.push_back(ite);
	}
	else
		super_array.push_back(first_backward);
	// now populate the caches
	bool first = true;
	for (auto one : super_array)
	{
		if (first)
		{// this is first stuff do it once
			first = false;
			// backward update
			auto it_elem = first_backward_hash_.find(new_id);
			if (it_elem == first_backward_hash_.end())
			{
				auto result = first_backward_hash_.emplace(new_id, std::make_unique<relation_elements_type>());
				result.first->second->insert(one);
			}
			else
				it_elem->second->insert(one);

			// forward update
			it_elem = first_forward_hash_.find(one);
			if (it_elem == first_forward_hash_.end())
			{
				auto result = first_forward_hash_.emplace(new_id, std::make_unique<relation_elements_type>());
				result.first->second->insert(new_id);
			}
			else
				it_elem->second->insert(new_id);
		}
		// backward update
		auto it_elem = backward_hash_.find(new_id);
		if (it_elem == backward_hash_.end())
		{
			auto result = backward_hash_.emplace(new_id, std::make_unique<relation_elements_type>());
			result.first->second->insert(one);
		}
		else
			it_elem->second->insert(one);

		// forward update
		it_elem = forward_hash_.find(one);
		if (it_elem == forward_hash_.end())
		{
			auto result = forward_hash_.emplace(new_id, std::make_unique<relation_elements_type>());
			result.first->second->insert(new_id);
		}
		else
			it_elem->second->insert(new_id);
	}
}

void relations_hash_data::remove_from_hash_data (const rx_node_id& id)
{
	auto it_elem = backward_hash_.find(id);
	if (it_elem != backward_hash_.end())
	{
		for (auto ite : (*it_elem->second))
		{
			auto it_super = forward_hash_.find(ite);
			if (it_super != forward_hash_.end())
				it_super->second->erase(id);
		}
		it_elem->second->clear();
		backward_hash_.erase(it_elem);
	}
#ifdef _DEBUG
	else
	{// this shouldn't happen!!!
		RX_ASSERT(false);
	}
#endif
	// first instance hash data
	it_elem = first_backward_hash_.find(id);
	if (it_elem != first_backward_hash_.end())
	{
		for (auto ite : (*it_elem->second))
		{
			auto it_super = first_forward_hash_.find(ite);
			if (it_super != first_forward_hash_.end())
				it_super->second->erase(id);
		}
		it_elem->second->clear();
		first_backward_hash_.erase(it_elem);
	}
#ifdef _DEBUG
	else
	{// this shouldn't happen!!!
		RX_ASSERT(false);
	}
#endif
}

void relations_hash_data::change_hash_data (const rx_node_id& id, const rx_node_id& first_backward_old, const rx_node_id& first_backward_new)
{
	remove_from_hash_data(id);
	add_to_hash_data(id, first_backward_new);
}

bool relations_hash_data::is_backward_from (const rx_node_id& id, const rx_node_id& parent)
{
	auto it = backward_hash_.find(id);
	if (it != backward_hash_.end())
	{
		return (it->second->find(parent) != it->second->end());
	}
	return false;
}

void relations_hash_data::get_full_forward (const rx_node_id& id, std::vector< rx_node_id>& result) const
{
	auto it = forward_hash_.find(id);
	if (it != forward_hash_.end())
	{
		for (const auto& one : *(it->second))
			result.push_back(one);
	}
}

void relations_hash_data::get_full_backward (const rx_node_id& id, std::vector< rx_node_id>& result) const
{
	auto it = backward_hash_.find(id);
	if (it != backward_hash_.end())
	{
		for (const auto& one : *(it->second))
			result.push_back(one);
	}
}

void relations_hash_data::get_first_forward (const rx_node_id& id, std::vector< rx_node_id>& result) const
{
	auto it = first_forward_hash_.find(id);
	if (it != first_forward_hash_.end())
	{
		for (const auto& one : *(it->second))
			result.push_back(one);
	}
}

void relations_hash_data::get_first_backward (const rx_node_id& id, std::vector< rx_node_id>& result) const
{
	auto it = first_backward_hash_.find(id);
	if (it != first_backward_hash_.end())
	{
		for (const auto& one : *(it->second))
			result.push_back(one);
	}
}


// Parameterized Class rx_internal::model::types_repository 

template <class typeT>
types_repository<typeT>::types_repository()
{
	if constexpr (typeT::has_default_constructor)
	{
		default_constructor_ = []()
		{
			return rx_create_reference<RImplType>();
		};
	}
}



template <class typeT>
rx_result types_repository<typeT>::register_constructor (const rx_node_id& id, std::function<RImplPtr()> f)
{
	constructors_.emplace(id, [f](const rx_node_id&)
		{
			constructed_data_t<RImplPtr> ret;
			ret.ptr = f();
			return ret;
		});
	return true;
}

template <class typeT>
rx_result types_repository<typeT>::register_constructor (const rx_node_id& id, std::function<constructed_data_t<RImplPtr>(const rx_node_id&)> f)
{
	constructors_.emplace(id, f);
	return true;
}

template <class typeT>
rx_result types_repository<typeT>::register_behavior (const rx_node_id& id, std::function<RBeh()> f)
{
	behaviors_.emplace(id, f);
	return true;
}

template <class typeT>
rx_result_with<create_runtime_result<typeT> > types_repository<typeT>::create_runtime (typename typeT::instance_data_t&& instance_data, data::runtime_values_data&& runtime_data, bool prototype)
{
	instance_data.meta_info = create_meta_for_new(instance_data.meta_info);
	meta_data& meta = instance_data.meta_info;

	if (!platform_types_manager::instance().get_types_resolver().is_available_id(meta.id)
		|| registered_objects_.find(meta.id) != registered_objects_.end())
	{
		return "Duplicate Id!";
	}

	create_runtime_result<typeT> ret;
	RImplPtr implementation_ptr;
	RBeh behavior;
	rx_node_id parent_id;

	construct_context ctx(meta.name);
	ctx.get_directories().add_paths({ meta.path });
	if (!meta.parent.is_null())
	{
		auto parent_id_result = algorithms::resolve_reference(meta.parent, ctx.get_directories());
		if (!parent_id_result)
		{
			rx_result ret(parent_id_result.errors());
			ret.register_error("Unable to resolve reference "s + meta.parent.to_string());
			return ret.errors();
		}
		parent_id = parent_id_result.value();
	}
	rx_node_ids base;
	base.emplace_back(parent_id);
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		auto base_result = inheritance_hash_.get_base_types(parent_id, base);
		if (!base_result)
			return base_result.errors();
	}
	else
	{
		ns::rx_directory_resolver resolver;
		rx_node_id temp_base = parent_id;
		while (!temp_base.is_null())
		{
			auto temp_type = get_type_definition(temp_base);			
			if (temp_type)
			{
				if (temp_type.value()->meta_info.parent.is_null())
				{
					temp_base = rx_node_id::null_id;
				}
				else
				{
					resolver.add_paths({ temp_type.value()->meta_info.path });
					auto parent_id = algorithms::resolve_reference(temp_type.value()->meta_info.parent, resolver);
					if (parent_id)
					{
						temp_base = parent_id.move_value();
						base.emplace_back(temp_base);
					}
					else
					{
						rx_result ret("Unable to resolve reference "s + temp_type.value()->meta_info.parent.to_string());
						ret.register_errors(parent_id.errors());
						return ret.errors();
					}
				}
			}
			else
				return temp_type.errors();
		}
	}
	bool found_constructor = false;
	bool found_behavior = false;
	for (const auto& one : base)
	{
		if (!found_constructor)
		{
			auto it = constructors_.find(one);
			if (it != constructors_.end())
			{
				auto construct_data = (it->second)(meta.id);
				implementation_ptr = construct_data.ptr;
				ret.register_f = construct_data.register_f;
				ret.unregister_f = construct_data.unregister_f;
				found_constructor = true;
			}
		}
		if (!found_behavior)
		{
			auto it = behaviors_.find(one);
			if (it != behaviors_.end())
			{
				behavior = it->second();
				found_behavior = true;
			}
		}
		if (found_behavior && found_constructor)
			break;
	}
	if (!implementation_ptr)
	{
		if constexpr (typeT::has_default_constructor)
		{
			implementation_ptr = default_constructor_();
		}
		else if(!prototype)
		{
			return "Unable to construct base runtime!";
		}
		else
		{
			implementation_ptr = default_constructor_();
		}
	}

	std::vector<data::runtime_values_data> overrides;
	ret.ptr = rx_create_reference<RType>(meta, instance_data, std::move(behavior));
	ret.ptr->implementation_ = implementation_ptr;
	for (auto one_id : base)
	{
		auto my_class = get_type_definition(one_id);
		if (my_class)
		{
			ctx.reinit();
			std::unique_ptr<data::runtime_values_data> temp_values= std::make_unique<data::runtime_values_data>();
			ctx.push_overrides("", temp_values.get());
			auto result = meta::meta_algorithm::object_types_algorithm<typeT>::construct_runtime(*my_class.value(), ret.ptr, ctx);
			if (!result)
			{// error constructing object
				return result.errors();
			}
			ctx.pop_overrides();
			//overrides.push_back(*ctx.get_overrides());
			overrides.push_back(my_class.value()->complex_data.get_overrides());
		}
		else
		{
			my_class.register_error("Error finding type definition");
			return my_class.errors();
		}
	}
	rx_simple_value name_value;
	name_value.assign_static<string_type>(string_type(meta.name));
	rx_timed_value ts_value;
	rx_time now = rx_time::now();
	ts_value.assign_static(now, now);
	auto rt_data = ctx.pop_rt_name();
	rt_data.add_const_value("Name", name_value);
	ret.ptr->tags_.set_runtime_data(create_runtime_data(rt_data));
	// go reverse with overrides
	for (auto it = overrides.rbegin(); it!= overrides.rend(); it++)
	{
		if (!it->empty())
			runtime::algorithms::runtime_holder_algorithms<typeT>::fill_data(*it, *ret.ptr);
	}
	runtime::algorithms::runtime_holder_algorithms<typeT>::fill_data(instance_data.overrides, *ret.ptr);
	if (!runtime_data.empty())// quick check won't harm 
		runtime::algorithms::runtime_holder_algorithms<typeT>::fill_data(runtime_data, *ret.ptr);
	ret.ptr->get_overrides() = instance_data.overrides;
	ret.ptr->directories_.add_paths({ ret.ptr->meta_info_.path });

	if (!prototype)
	{
		registered_objects_.emplace(meta.id, runtime_data_t{ ret.ptr, runtime_state::runtime_state_created });
		if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			instance_hash_.add_to_hash_data(meta.id, parent_id, base);
		auto type_ret = platform_types_manager::instance().get_types_resolver().add_id(meta.id, typeT::RImplType::type_id, meta);
		RX_ASSERT(type_ret);// has to be, we checked earlier
	}
	return ret;
}

template <class typeT>
rx_result_with<typename types_repository<typeT>::RTypePtr> types_repository<typeT>::get_runtime (const rx_node_id& id, bool only_running) const
{
	auto it = registered_objects_.find(id);
	if (it != registered_objects_.end() && (!only_running || it->second.state == runtime_state::runtime_state_running))
	{
		return it->second.target;
	}
	else
	{
		return "Runtime not registered";
	}
}

template <class typeT>
rx_result types_repository<typeT>::delete_runtime (rx_node_id id)
{
	auto it = registered_objects_.find(id);
	if (it != registered_objects_.end())
	{
		if (it->second.state == runtime_state::runtime_state_deleting || it->second.state == runtime_state::runtime_state_created)
		{
			ns::rx_directory_resolver resolver;
			resolver.add_paths({ it->second.target->meta_info().path});
			auto type_id_result = algorithms::resolve_reference(it->second.target->meta_info().parent, resolver);
			rx_node_id type_id;
			if (type_id_result)
				type_id = type_id_result.value();
			rx_node_ids base;
			base.emplace_back(type_id);
			auto base_result = inheritance_hash_.get_base_types(type_id, base);
			registered_objects_.erase(it);
			instance_hash_.remove_from_hash_data(id, type_id, base);
			auto type_ret = platform_types_manager::instance().get_types_resolver().remove_id(id);
			RX_ASSERT(type_ret);
			return true;
		}
		switch (it->second.state)
		{
		case runtime_state::runtime_state_checkout:
			return "Wrong state, object checked out!";
		case runtime_state::runtime_state_initializing:
			return "Wrong state, object starting!";
		case runtime_state::runtime_state_running:
			return "Wrong state, object not marked for delete!";
		case runtime_state::runtime_state_destroyed:
			return "Wrong state, object destroyed!";
		default:
			return "Unknown state!";
		}
	}
	else
	{
		return "Object does not exists!";
	}
}

template <class typeT>
api::query_result types_repository<typeT>::get_derived_types (const rx_node_id& id) const
{
	api::query_result ret;
	std::vector<rx_node_id> temp;
	rx_result result = inheritance_hash_.get_derived_from(id, temp);
	if (result)
	{
		for (auto one : temp)
		{
			auto type = registered_types_.find(one);

			if (type != registered_types_.end())
			{
				ret.items.emplace_back(api::query_result_detail{ typeT::type_id, type->second->meta_info });
			}
		}
	}
	ret.success = true;
	return ret;
}

template <class typeT>
rx_result types_repository<typeT>::register_type (typename types_repository<typeT>::Tptr what)
{
	rx_node_id parent_id;
	if (!what->meta_info.parent.is_null())
	{
		ns::rx_directory_resolver dirs;
		dirs.add_paths({ what->meta_info.path });
		auto parent_id_result = algorithms::resolve_reference(what->meta_info.parent, dirs);
		if (!parent_id_result)
		{
			return "Unable to resolve reference to: "s + what->meta_info.parent.to_string() + " for " + what->meta_info.name;
		}
		parent_id = parent_id_result.move_value();
	}

	const auto& id = what->meta_info.id;

	auto it = registered_types_.find(id);
	if (it == registered_types_.end())
	{
		registered_types_.emplace(what->meta_info.id, what);
		if(rx_gate::instance().get_platform_status()== rx_platform_status::running)
			auto hash_result = inheritance_hash_.add_to_hash_data(id, parent_id);
		auto type_res = platform_types_manager::instance().get_types_resolver().add_id(what->meta_info.id, typeT::type_id, what->meta_info);
		RX_ASSERT(type_res);
		return true;
	}
	else
	{
		return "Duplicated Node Id: "s + what->meta_info.id.to_string() + " for " + what->meta_info.name;
	}
}

template <class typeT>
rx_result types_repository<typeT>::check_type (const rx_node_id& id, type_check_context& ctx) const
{
	ctx.push_source(rx_item_type_name(typeT::type_id) + " repository");
	auto temp = get_type_definition(id);
	if (temp)
	{
		return meta_algorithm::object_types_algorithm<typeT>::check_type(*temp.value(), ctx);
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(typeT::type_id)
			<< " with node_id "
			<< id;
		ctx.add_error(ss.str(), RX_ITEM_NOT_FOUND, rx_critical_severity, temp.errors());
		return true;
	}
}

template <class typeT>
rx_result types_repository<typeT>::update_type (types_repository<typeT>::Tptr what)
{
	const auto& id = what->meta_info.id;
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		it->second = what;
		platform_types_manager::instance().get_types_resolver().update_id(id);
		// TODO Should check and change if parent is different
		/*if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			inheritance_hash_.add_to_hash_data(id, what->meta_info.parent);*/
		return true;
	}
	else
	{
		return "Node Id: "s + what->meta_info.id.to_string() + " for " + what->meta_info.name + " does not exists";
	}
}

template <class typeT>
typename types_repository<typeT>::TdefRes types_repository<typeT>::get_type_definition (const rx_node_id& id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return it->second;
	}
	else
	{
		std::ostringstream ss;
		ss << id
			<< " is not registered as valid "
			<< rx_item_type_name(typeT::type_id);
		return ss.str();
	}
}

template <class typeT>
rx_result types_repository<typeT>::delete_type (rx_node_id id)
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		auto type_ret = inheritance_hash_.remove_from_hash_data(id);
		RX_ASSERT(type_ret);
		registered_types_.erase(it);
		type_ret = platform_types_manager::instance().get_types_resolver().remove_id(id);
		RX_ASSERT(type_ret);
		return true;
	}
	else
	{
		return "Type not registered!";
	}
}

template <class typeT>
rx_result types_repository<typeT>::initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data)
{
	std::vector<std::pair<rx_node_id, rx_node_id> > to_add;
	to_add.reserve(registered_types_.size());
	for (const auto& one : registered_types_)
	{
		ns::rx_directory_resolver dirs;
		dirs.add_paths({ one.second->meta_info.path });
		auto parent_id = algorithms::resolve_reference(one.second->meta_info.parent, dirs);
		if (parent_id)
			to_add.emplace_back(one.second->meta_info.id, parent_id.value());
		else
			to_add.emplace_back(one.second->meta_info.id, rx_node_id::null_id);
	}
	auto result = inheritance_hash_.add_to_hash_data(to_add);
	for (auto& one : registered_objects_)
	{
		auto init_result = sys_runtime::platform_runtime_manager::instance().init_runtime<typeT>(one.second.target);
		if (init_result)
			one.second.state = runtime_state::runtime_state_running;
	}
	return result;
}

template <class typeT>
api::query_result types_repository<typeT>::get_instanced_objects (const rx_node_id& id) const
{
	api::query_result ret;
	std::vector<rx_node_id> temp;
	auto instanced_result = instance_hash_.get_instanced_from(id, temp);
	for (auto one : temp)
	{
		auto type = registered_objects_.find(one);

		if (type != registered_objects_.end())
		{
			ret.items.emplace_back(typeT::type_id, type->second.target->meta_info());
		}
	}
	ret.success = true;
	return ret;
}

template <class typeT>
rx_result_with<typename typeT::RTypePtr> types_repository<typeT>::mark_runtime_for_delete (rx_node_id id, const rx_uuid& checkout)
{
	auto it = registered_objects_.find(id);
	if (it != registered_objects_.end())
	{
		if (it->second.state == runtime_state::runtime_state_checkout && !checkout.is_null())
		{
			if (it->second.checkout.id != checkout)
				return "Object checked out by "s + security::active_security()->get_full_name() + " .";
			it->second.state = runtime_state::runtime_state_deleting;
			return it->second.target;
		}
		if (it->second.state == runtime_state::runtime_state_running)
		{
			it->second.state = runtime_state::runtime_state_deleting;
			return it->second.target;
		}
		switch (it->second.state)
		{
		case runtime_state::runtime_state_checkout:
			return "Wrong state, object checked out!";
		case runtime_state::runtime_state_created:
			return "Wrong state, object just created!";
		case runtime_state::runtime_state_initializing:
			return "Wrong state, object starting!";
		case runtime_state::runtime_state_deleting:
			return "Wrong state, object already marked for delete!";
		case runtime_state::runtime_state_destroyed:
			return "Wrong state, object destroyed!";
		default:
			return "Unknown state!";
		}
	}
	else
	{
		return "Object does not exists!";
	}
}

template <class typeT>
rx_result types_repository<typeT>::mark_runtime_running (rx_node_id id)
{
	auto it = registered_objects_.find(id);
	if (it != registered_objects_.end())
	{
		switch (it->second.state)
		{
		case runtime_state::runtime_state_created:
		case runtime_state::runtime_state_initializing:
		case runtime_state::runtime_state_deleting:// this is when delete fails to reset object as running
			it->second.state = runtime_state::runtime_state_running;
			return true;
		case runtime_state::runtime_state_checkout:
			return "Wrong state, object not initialized!";
		case runtime_state::runtime_state_running:
			return "Wrong state, object already running!";
		case runtime_state::runtime_state_destroyed:
			return "Wrong state, object destroyed!";
		default:
			return "Unknown state!";
		}
	}
	else
	{
		return "Object does not exists!";
	}
}

template <class typeT>
rx_result types_repository<typeT>::type_exists (rx_node_id id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return true;
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(typeT::type_id)
			<< " with node_id "
			<< id;
		return ss.str();
	}
}


// Class rx_internal::model::inheritance_hash 

inheritance_hash::inheritance_hash()
{
}



rx_result inheritance_hash::add_to_hash_data (const rx_node_id& new_id, const rx_node_id& base_id)
{
	if (hash_data_.find(new_id) != hash_data_.end())
		return "Node already exists!";

	relation_map_type::iterator base_data_it = hash_data_.end();
	if (!base_id.is_null())
	{
		base_data_it = hash_data_.find(base_id);
		if (base_data_it == hash_data_.end())
			return "Base node does not exists!";
	}
	auto new_data_it = hash_data_.emplace(new_id, relation_elements_data()).first;
	if (!base_id.is_null())
	{
		new_data_it->second.ordered.emplace_back(base_id);
		if (base_data_it != hash_data_.end())
		{// copy data from base id
			std::copy(base_data_it->second.ordered.begin()
				, base_data_it->second.ordered.end()
				, std::back_inserter(new_data_it->second.ordered));
			new_data_it->second.unordered = base_data_it->second.unordered;
		}
		new_data_it->second.unordered.emplace(base_id);
	}
	// now add data to other caches
	auto hash_it = derived_first_hash_.find(base_id);
	if (hash_it == derived_first_hash_.end())
	{
		hash_it = derived_first_hash_.emplace(base_id, hash_elements_type()).first;
	}
	hash_it->second.emplace(new_id);

	for (auto one : new_data_it->second.ordered)
	{
		hash_it = derived_hash_.find(one);
		if (hash_it == derived_hash_.end())
		{
			hash_it = derived_hash_.emplace(base_id, hash_elements_type()).first;
		}
		hash_it->second.emplace(new_id);
	}

	return true;
}

rx_result inheritance_hash::get_base_types (const rx_node_id& id, rx_node_ids& result) const
{
	auto it = hash_data_.find(id);
	if (it != hash_data_.end())
	{
		std::copy(it->second.ordered.begin(), it->second.ordered.end(), std::back_inserter(result));
		return true;
	}
	else
	{
		return "Node does not exists!";
	}
}

rx_result inheritance_hash::get_derived_from (const rx_node_id& id, rx_node_ids& result) const
{
	auto it = derived_first_hash_.find(id);
	if (it != derived_first_hash_.end())
	{
		std::copy(it->second.begin(), it->second.end(), std::back_inserter(result));
	}
	return true;
}

rx_result inheritance_hash::get_all_derived_from (const rx_node_id& id, rx_node_ids& result) const
{
	auto it = derived_hash_.find(id);
	if (it != derived_hash_.end())
	{
		std::copy(it->second.begin(), it->second.end(), std::back_inserter(result));
	}
	return true;
}

rx_result inheritance_hash::remove_from_hash_data (const rx_node_id& id)
{
	rx_node_ids ids;
	auto hash_it = hash_data_.find(id);

	auto hash_first_it = derived_first_hash_.find(id);
	auto hash_all_it = derived_hash_.find(id);
	auto result = get_base_types(id, ids);
	if (result)
	{
		bool first = true;
		for (auto& one : ids)
		{
			if (first)
			{
				first = false;
				auto first_it = derived_first_hash_.find(one);
				if (first_it != derived_first_hash_.end())
				{
					first_it->second.erase(id);
				}
			}
			auto it = derived_hash_.find(one);
			if (it != derived_hash_.end())
			{
				it->second.erase(id);
			}
		}
	}
	ids.clear();
	result = get_all_derived_from(id, ids);
	if (result)
	{
		for (auto& one : ids)
		{
			auto rel_it = hash_data_.find(one);
			if (rel_it != hash_data_.end())
			{
				rel_it->second.unordered.erase(id);
				auto remove_result = std::remove(rel_it->second.ordered.begin(), rel_it->second.ordered.end(), id);
				if (remove_result == rel_it->second.ordered.end())
					META_LOG_DEBUG("inheritance_hash", 999, "Unexpected remove result: strange but id is not cached");
			}
		}
	}
	return result;
}

rx_result inheritance_hash::add_to_hash_data (const std::vector<std::pair<rx_node_id, rx_node_id> >& items)
{
	std::unordered_set<rx_node_id> to_add;
	std::vector<std::pair<rx_node_id, rx_node_id> > local_to_add(items);
	// first add all items to set for faster search
	for (const auto& one : items)
		to_add.insert(one.first);

	rx_result result(true);

	while (!to_add.empty())
	{
		// check for items not dependent on any items and add them next
		for (auto& one : local_to_add)
		{
			if (one.first)
			{
				auto it_help = to_add.find(one.second);
				if (it_help == to_add.end())
				{
					result = add_to_hash_data(one.first, one.second);
					if (!result)
						return result;
					to_add.erase(one.first);
					one.first = rx_node_id::null_id;
				}
			}
		}
	}
	return true;
}


// Class rx_internal::model::instance_hash 

instance_hash::instance_hash()
{
}



bool instance_hash::add_to_hash_data (const rx_node_id& new_id, const rx_node_id& type_id, const rx_node_ids& all_type_ids)
{
	auto type_data_it = instance_first_hash_.find(type_id);
	if (type_data_it == instance_first_hash_.end())
	{// type node id does not exists
		type_data_it = instance_first_hash_.emplace(type_id, std::make_unique<hash_elements_type>()).first;
	}
	type_data_it->second->emplace(new_id);

	for (const auto& one : all_type_ids)
	{
		type_data_it = instance_hash_.find(one);
		if (type_data_it == instance_hash_.end())
		{// type node id does not exists
			type_data_it = instance_hash_.emplace(type_id, std::make_unique<hash_elements_type>()).first;
		}
		type_data_it->second->emplace(new_id);
	}
	return true;
}

bool instance_hash::remove_from_hash_data (const rx_node_id& new_id, const rx_node_id& type_id, const rx_node_ids& all_type_ids)
{
	auto type_data_it = instance_first_hash_.find(type_id);
	if (type_data_it == instance_first_hash_.end())
	{// type node id does not exists
		return false;
	}
	type_data_it->second->erase(new_id);

	for (const auto& one : all_type_ids)
	{
		type_data_it = instance_hash_.find(one);
		if (type_data_it != instance_hash_.end())
		{
			type_data_it->second->erase(new_id);
		}
	}
	return true;
}

rx_result instance_hash::get_instanced_from (const rx_node_id& id, rx_node_ids& result) const
{
	auto it = instance_hash_.find(id);
	if (it != instance_hash_.end())
	{
		for (auto one : *it->second)
		{
			result.emplace_back(one);
		}
		return true;
	}
	else
	{
		return "Node id does not exists";
	}
}


// Parameterized Class rx_internal::model::simple_types_repository 

template <class typeT>
simple_types_repository<typeT>::simple_types_repository()
{
	default_constructor_ = []()
	{
		return rx_create_reference<RType>();
	};
}



template <class typeT>
typename simple_types_repository<typeT>::TdefRes simple_types_repository<typeT>::get_type_definition (const rx_node_id& id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return it->second;
	}
	else
	{
		std::ostringstream ss;
		ss << id
			<< " is not registered as valid "
			<< rx_item_type_name(typeT::type_id);
		return ss.str();
	}
}

template <class typeT>
rx_result simple_types_repository<typeT>::register_type (typename simple_types_repository<typeT>::Tptr what)
{
	rx_node_id parent_id;
	if (!what->meta_info.parent.is_null())
	{
		ns::rx_directory_resolver dirs;
		dirs.add_paths({ what->meta_info.path });
		auto parent_id_result = algorithms::resolve_reference(what->meta_info.parent, dirs);
		if (!parent_id_result)
		{
			return "Unable to resolve reference to: "s + what->meta_info.parent.to_string() + " for " + what->meta_info.name;
		}
		parent_id = parent_id_result.move_value();
	}
	const auto& id = what->meta_info.id;
	auto it = registered_types_.find(id);
	if (it == registered_types_.end())
	{
		registered_types_.emplace(what->meta_info.id, what);
		if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			auto hash_result = inheritance_hash_.add_to_hash_data(id, parent_id);
		auto type_res = platform_types_manager::instance().get_types_resolver().add_id(what->meta_info.id, typeT::type_id, what->meta_info);
		RX_ASSERT(type_res);
		return true;
	}
	else
	{
		return false;
	}
}

template <class typeT>
rx_result simple_types_repository<typeT>::register_constructor (const rx_node_id& id, std::function<RTypePtr()> f)
{
	constructors_.emplace(id, f);
	return true;
}

template <class typeT>
rx_result_with<typename simple_types_repository<typeT>::RDataType> simple_types_repository<typeT>::create_simple_runtime (const rx_node_id& type_id, const string_type& rt_name, construct_context& ctx) const
{
	RTypePtr ret;
	rx_node_ids base;
	base.emplace_back(type_id);
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		auto base_result = inheritance_hash_.get_base_types(type_id, base);
		if (!base_result)
			return base_result.errors();
	}
	else
	{
		ns::rx_directory_resolver resolver;
		rx_node_id temp_base = type_id;
		while (!temp_base.is_null())
		{
			auto temp_type = get_type_definition(temp_base);
			if (temp_type)
			{
				if (temp_type.value()->meta_info.parent.is_null())
				{
					temp_base = rx_node_id::null_id;
				}
				else
				{
					resolver.add_paths({ temp_type.value()->meta_info.path });
					auto parent_id = algorithms::resolve_reference(temp_type.value()->meta_info.parent, resolver);
					if (parent_id)
					{
						temp_base = parent_id.move_value();
						base.emplace_back(temp_base);
					}
					else
					{
						rx_result ret("Unable to resolve reference "s + temp_type.value()->meta_info.parent.to_string());
						ret.register_errors(parent_id.errors());
						return ret.errors();
					}
				}
			}
			else
				return temp_type.errors();
		}
	}

	for (const auto& one : base)
	{
		auto it = constructors_.find(one);
		if (it != constructors_.end())
		{
			ret = (it->second)();
			break;
		}
	}
	if (!ret)
		ret = default_constructor_();

	ctx.push_rt_name(rt_name);

	std::vector<data::runtime_values_data> overrides;
	RDataType rt_prototype;
	for (auto one_id : base)
	{
		auto my_class = get_type_definition(one_id);
		if (my_class)
		{
			std::unique_ptr<data::runtime_values_data> temp_values = std::make_unique<data::runtime_values_data>();
			ctx.get_directories().add_paths({my_class.value()->meta_info.path});
			auto result = meta::meta_algorithm::basic_types_algorithm<typeT>::construct(*my_class.value(), ctx, rt_prototype);

			if (!result)
			{// error constructing object
				return result.errors();
			}
			
			overrides.push_back(my_class.value()->complex_data.get_overrides());
		}
		else
			return my_class.errors();
	}
	
	runtime_data_prototype rt = ctx.pop_rt_name();
	auto runtime = create_runtime_data(rt);
	// go reverse with overrides
	for (auto it = overrides.rbegin(); it != overrides.rend(); it++)
	{
		if (!it->empty())
			runtime->fill_data(*it);
	}
	return RDataType(std::move(runtime), std::move(ret), rt_prototype);
}

template <class typeT>
api::query_result simple_types_repository<typeT>::get_derived_types (const rx_node_id& id) const
{
	api::query_result ret;
	std::vector<rx_node_id> temp;
	auto derived_result = inheritance_hash_.get_derived_from(id, temp);
	for (auto one : temp)
	{
		auto type = registered_types_.find(one);

		if (type != registered_types_.end())
		{
			ret.items.emplace_back(api::query_result_detail{ typeT::type_id, type->second->meta_info });
		}
	}
	ret.success = true;
	return ret;
}

template <class typeT>
rx_result simple_types_repository<typeT>::check_type (const rx_node_id& id, type_check_context& ctx) const
{
	ctx.push_source(rx_item_type_name(typeT::type_id) + " repository");
	auto temp = get_type_definition(id);
	if (temp)
	{
		return meta::meta_algorithm::basic_types_algorithm<typeT>::check_type(*temp.value(), ctx);
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(typeT::type_id)
			<< " with node_id "
			<< id;
		ctx.add_error(ss.str(), RX_ITEM_NOT_FOUND, rx_critical_severity, temp.errors());
		return false;
	}
}

template <class typeT>
rx_result simple_types_repository<typeT>::delete_type (rx_node_id id)
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		auto type_id = it->second->meta_info.parent;
		registered_types_.erase(it);
		auto remove_result = inheritance_hash_.remove_from_hash_data(id);
		auto type_ret = platform_types_manager::instance().get_types_resolver().remove_id(id);
		RX_ASSERT(type_ret);
		return true;
	}
	else
	{
		return false;
	}
}

template <class typeT>
rx_result simple_types_repository<typeT>::type_exists (rx_node_id id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return true;
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(typeT::type_id)
			<< " with node_id "
			<< id;
		return ss.str();
	}
}

template <class typeT>
rx_result simple_types_repository<typeT>::initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data)
{
	std::vector<std::pair<rx_node_id, rx_node_id> > to_add;
	to_add.reserve(registered_types_.size());
	for (const auto& one : registered_types_)
	{
		ns::rx_directory_resolver dirs;
		dirs.add_paths({ one.second->meta_info.path });
		auto parent_id = algorithms::resolve_reference(one.second->meta_info.parent, dirs);
		if (parent_id)
			to_add.emplace_back(one.second->meta_info.id, parent_id.value());
		else
			to_add.emplace_back(one.second->meta_info.id, rx_node_id::null_id);
	}
	auto result = inheritance_hash_.add_to_hash_data(to_add);
	return result;
}

template <class typeT>
rx_result simple_types_repository<typeT>::update_type (typename simple_types_repository<typeT>::Tptr what)
{
	const auto& id = what->meta_info.id;
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		it->second = what;
		platform_types_manager::instance().get_types_resolver().update_id(id);
		// TODO Should check and change if parent is different
		/*if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			inheritance_hash_.add_to_hash_data(id, what->meta_info.parent);*/
		return true;
	}
	else
	{
		return "Node Id: "s + what->meta_info.id.to_string() + " for " + what->meta_info.name + " does not exists";
	}
}


// Class rx_internal::model::types_resolver 


rx_result types_resolver::add_id (const rx_node_id& id, rx_item_type type, const meta_data& data)
{
	auto it = hash_.find(id);
	if (it != hash_.end())
		return "Duplicated id";
	hash_.emplace(id, resolver_data{ type, data });

	resolver_event_data event_data;
	event_data.verb = resolver_event_verb::created;
	event_data.id = id;
	event_data.path = data.get_full_path();

	for (auto& one : subscribers_)
	{
		one.second.callback(event_data);
	}
	return true;
}

rx_item_type types_resolver::get_item_type (const rx_node_id& id) const
{
	auto it = hash_.find(id);
	if (it == hash_.end())
		return rx_item_type::rx_invalid_type;
	else
		return it->second.type;
}

bool types_resolver::is_available_id (const rx_node_id& id) const
{
	return hash_.find(id) == hash_.end();
}

rx_result types_resolver::remove_id (const rx_node_id& id)
{
	auto it = hash_.find(id);
	if (it == hash_.end())
		return "Unknown Id";
	else
	{
		resolver_event_data event_data;
		event_data.verb = resolver_event_verb::deleted;
		event_data.id = id;
		event_data.path = it->second.data.get_full_path();
		hash_.erase(it);
		for (auto& one : subscribers_)
		{
			one.second.callback(event_data);
		}
		return true;
	}
}

rx_item_type types_resolver::get_item_data (const rx_node_id& id, meta_data& data) const
{
	auto it = hash_.find(id);
	if (it == hash_.end())
		return rx_invalid_type;
	data = it->second.data;
	return it->second.type;
}

rx_result types_resolver::update_id (const rx_node_id& id)
{
	auto it = hash_.find(id);
	if (it == hash_.end())
		return "Unknown Id";
	else
	{
		resolver_event_data event_data;
		event_data.verb = resolver_event_verb::deleted;
		event_data.id = id;
		event_data.path = it->second.data.get_full_path();

		for (auto& one : subscribers_)
		{
			one.second.callback(event_data);
		}
		return true;
	}
}


// Class rx_internal::model::relations_type_repository 

relations_type_repository::relations_type_repository()
{
	default_constructor_ = []()
	{
		return rx_create_reference<RType>();
	};
}



relations_type_repository::TdefRes relations_type_repository::get_type_definition (const rx_node_id& id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return it->second;
	}
	else
	{
		std::ostringstream ss;
		ss << id
			<< " is not registered as valid "
			<< rx_item_type_name(relation_type::type_id);
		return ss.str();
	}
}

rx_result relations_type_repository::register_type (relations_type_repository::Tptr what)
{
	rx_node_id parent_id;
	if (!what->meta_info.parent.is_null())
	{
		ns::rx_directory_resolver dirs;
		dirs.add_paths({ what->meta_info.path });
		auto parent_id_result = algorithms::resolve_reference(what->meta_info.parent, dirs);
		if (!parent_id_result)
		{
			return "Unable to resolve reference to: "s + what->meta_info.parent.to_string() + " for " + what->meta_info.name;
		}
		parent_id = parent_id_result.move_value();
	}
	const auto& id = what->meta_info.id;
	auto it = registered_types_.find(id);
	if (it == registered_types_.end())
	{
		registered_types_.emplace(what->meta_info.id, what);
		if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			auto hash_result = inheritance_hash_.add_to_hash_data(id, parent_id);
		auto type_res = platform_types_manager::instance().get_types_resolver().add_id(what->meta_info.id, relation_type::type_id, what->meta_info);
		RX_ASSERT(type_res);
		return true;
	}
	else
	{
		return "Duplicated Node Id: "s + what->meta_info.id.to_string() + " for " + what->meta_info.name;
	}
}

rx_result_with<relations_type_repository::RTypePtr> relations_type_repository::create_runtime (const rx_node_id& type_id, const string_type& rt_name, runtime::relations::relation_data& data, const rx_directory_resolver& dirs)
{
	auto type_result = get_type_definition(type_id);
	if (!type_result)
		return type_result.errors();

	relation_type::smart_ptr relation_type_ptr = type_result.move_value();
	auto ret = create_relation_runtime(relation_type_ptr);
	if (!ret)
		return ret;

	if (!relation_type_ptr->relation_data.target.is_null())
	{
		rx_directory_resolver rel_dirs;
		rel_dirs.add_paths({ relation_type_ptr->meta_info.path });
		auto resolve_result = algorithms::resolve_reference(relation_type_ptr->relation_data.target, rel_dirs);
		if (resolve_result)
		{
			data.target_base_id = resolve_result.move_value();
		}
	}

	data.value.value_opt[runtime::structure::value_data::opt_readonly] = !relation_type_ptr->relation_data.dynamic;

	data.implementation_ = ret.value();
	if (!relation_type_ptr->relation_data.symmetrical)
	{
		data.target_relation_name =
			relation_type_ptr->relation_data.inverse_name.empty() ? rt_name : relation_type_ptr->relation_data.inverse_name;
	}
	return ret;
}

api::query_result relations_type_repository::get_derived_types (const rx_node_id& id) const
{
	api::query_result ret;
	std::vector<rx_node_id> temp;
	rx_result result = inheritance_hash_.get_derived_from(id, temp);
	if (result)
	{
		for (auto one : temp)
		{
			auto type = registered_types_.find(one);

			if (type != registered_types_.end())
			{
				ret.items.emplace_back(api::query_result_detail{ relation_type::type_id, type->second->meta_info });
			}
		}
	}
	ret.success = true;
	return ret;
}

rx_result relations_type_repository::check_type (const rx_node_id& id, type_check_context& ctx) const
{
	ctx.push_source(rx_item_type_name(relation_type::type_id) + " repository");
	auto temp = get_type_definition(id);
	if (temp)
	{
		return rx_platform::meta::meta_algorithm::relation_type_algorithm::check_type(*temp.value(), ctx);
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(relation_type::type_id)
			<< " with node_id "
			<< id;
		ctx.add_error(ss.str(), RX_ITEM_NOT_FOUND, rx_critical_severity, temp.errors());
		return false;
	}
}

rx_result relations_type_repository::delete_type (rx_node_id id)
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		auto type_ret = inheritance_hash_.remove_from_hash_data(id);
		RX_ASSERT(type_ret);
		registered_types_.erase(it);
		type_ret = platform_types_manager::instance().get_types_resolver().remove_id(id);
		RX_ASSERT(type_ret);
		return true;
	}
	else
	{
		return "Node id not found";
	}
}

rx_result relations_type_repository::initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data)
{
	std::vector<std::pair<rx_node_id, rx_node_id> > to_add;
	to_add.reserve(registered_types_.size());
	for (const auto& one : registered_types_)
	{
		ns::rx_directory_resolver dirs;
		dirs.add_paths({ one.second->meta_info.path });
		auto parent_id = algorithms::resolve_reference(one.second->meta_info.parent, dirs);
		if(parent_id)
			to_add.emplace_back(one.second->meta_info.id, parent_id.value());
		else
			to_add.emplace_back(one.second->meta_info.id, rx_node_id::null_id);
	}
	auto result = inheritance_hash_.add_to_hash_data(to_add);
	return result;
}

rx_result relations_type_repository::update_type (relations_type_repository::Tptr what)
{
	const auto& id = what->meta_info.id;
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		it->second = what;
		platform_types_manager::instance().get_types_resolver().update_id(id);
		// TODO Should check and change if parent is different
		/*if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			inheritance_hash_.add_to_hash_data(id, what->meta_info.parent);*/
		return true;
	}
	else
	{
		return "Node Id: "s + what->meta_info.id.to_string() + " for " + what->meta_info.name + " does not exists";
	}
}

rx_result_with<relations_type_repository::RTypePtr> relations_type_repository::create_relation_runtime (relations_type_repository::Tptr form_what)
{
	RTypePtr ret;
	rx_node_ids base;
	auto type_id = form_what->meta_info.id;
	base.emplace_back(type_id);
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		auto base_result = inheritance_hash_.get_base_types(type_id, base);
		if (!base_result)
			return base_result.errors();
	}
	else
	{
		ns::rx_directory_resolver resolver;
		rx_node_id temp_base = type_id;
		rx_node_id parent_id;
		while (!temp_base.is_null())
		{
			auto temp_type = get_type_definition(temp_base);
			if (temp_type)
			{
				if (temp_type.value()->meta_info.parent.is_null())
				{
					temp_base = rx_node_id::null_id;
					base.emplace_back(temp_base);
				}
				else
				{
					resolver.add_paths({ temp_type.value()->meta_info.path });
					auto parent_id = algorithms::resolve_reference(temp_type.value()->meta_info.parent, resolver);
					if (parent_id)
					{
						temp_base = parent_id.move_value();
						base.emplace_back(temp_base);
					}
					else
					{
						rx_result ret("Unable to resolve reference "s + temp_type.value()->meta_info.parent.to_string());
						ret.register_errors(parent_id.errors());
						return ret.errors();
					}
				}
			}
			else
				return temp_type.errors();
		}
	}

	for (const auto& one : base)
	{

		auto it = constructors_.find(one);
		if (it != constructors_.end())
		{
			ret = (it->second)();
			break;
		}
	}
	if (!ret)
		ret = default_constructor_();
	return ret;
}

rx_result relations_type_repository::type_exists (rx_node_id id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return true;
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(relation_type::type_id)
			<< " with node_id "
			<< id;
		return ss.str();
	}
}

rx_result relations_type_repository::register_constructor (const rx_node_id& id, std::function<RTypePtr()> f)
{
	constructors_.emplace(id, f);
	return true;
}


// Class rx_internal::model::data_type_repository 

data_type_repository::data_type_repository()
{
}



data_type_repository::TdefRes data_type_repository::get_type_definition (const rx_node_id& id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return it->second;
	}
	else
	{
		std::ostringstream ss;
		ss << id
			<< " is not registered as valid "
			<< rx_item_type_name(rx_item_type::rx_data_type);
		return ss.str();
	}
}

rx_result data_type_repository::register_type (data_type_repository::Tptr what)
{
	rx_node_id parent_id;
	if (!what->meta_info.parent.is_null())
	{
		ns::rx_directory_resolver dirs;
		dirs.add_paths({ what->meta_info.path });
		auto parent_id_result = algorithms::resolve_reference(what->meta_info.parent, dirs);
		if (!parent_id_result)
		{
			return "Unable to resolve reference to: "s + what->meta_info.parent.to_string() + " for " + what->meta_info.name;
		}
		parent_id = parent_id_result.move_value();
	}
	const auto& id = what->meta_info.id;
	auto it = registered_types_.find(id);
	if (it == registered_types_.end())
	{
		registered_types_.emplace(what->meta_info.id, what);
		if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			auto hash_result = inheritance_hash_.add_to_hash_data(id, parent_id);
		auto type_res = platform_types_manager::instance().get_types_resolver().add_id(what->meta_info.id, data_type::type_id, what->meta_info);
		RX_ASSERT(type_res);
		return true;
	}
	else
	{
		return false;
	}
}

rx_result_with<data_blocks_prototype> data_type_repository::create_data_type (const rx_node_id& type_id, const string_type& rt_name, construct_context& ctx, const rx_directory_resolver& dirs)
{
	runtime::structure::block_data ret;
	rx_node_ids base;
	base.emplace_back(type_id);
	if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
	{
		auto base_result = inheritance_hash_.get_base_types(type_id, base);
		if (!base_result)
			return base_result.errors();
	}
	else
	{
		ns::rx_directory_resolver resolver;
		rx_node_id temp_base = type_id;
		while (!temp_base.is_null())
		{
			auto temp_type = get_type_definition(temp_base);
			if (temp_type)
			{
				if (temp_type.value()->meta_info.parent.is_null())
				{
					temp_base = rx_node_id::null_id;
				}
				else
				{
					resolver.add_paths({ temp_type.value()->meta_info.path });
					auto parent_id = algorithms::resolve_reference(temp_type.value()->meta_info.parent, resolver);
					if (parent_id)
					{
						temp_base = parent_id.move_value();
						base.emplace_back(temp_base);
					}
					else
					{
						rx_result ret("Unable to resolve reference "s + temp_type.value()->meta_info.parent.to_string());
						ret.register_errors(parent_id.errors());
						return ret.errors();
					}
				}
			}
			else
				return temp_type.errors();
		}
	}


	ctx.push_rt_name(rt_name);

	data_blocks_prototype ret_data;
	ret_data.success = false;

	std::vector<data::runtime_values_data> overrides;
	for (auto one_id : base)
	{
		auto my_class = get_type_definition(one_id);
		if (my_class)
		{
			std::unique_ptr<data::runtime_values_data> temp_values = std::make_unique<data::runtime_values_data>();
			ctx.get_directories().add_paths({ my_class.value()->meta_info.path });
			auto result = meta::meta_algorithm::data_types_algorithm::construct_runtime(*my_class.value(), ret_data, ctx);

			if (!result)
			{// error constructing object
				return result.errors();
			}

			overrides.push_back(my_class.value()->complex_data.get_overrides());
		}
		else
			return my_class.errors();
	}

	runtime_data_prototype rt = ctx.pop_rt_name();
	auto runtime = create_runtime_data(rt);
	// go reverse with overrides
	for (auto it = overrides.rbegin(); it != overrides.rend(); it++)
	{
		if (!it->empty())
			runtime->fill_data(*it);
	}
	ret_data.success = true;
	return ret_data;
}

api::query_result data_type_repository::get_derived_types (const rx_node_id& id) const
{
	api::query_result ret;
	std::vector<rx_node_id> temp;
	auto derived_result = inheritance_hash_.get_derived_from(id, temp);
	for (auto one : temp)
	{
		auto type = registered_types_.find(one);

		if (type != registered_types_.end())
		{
			ret.items.emplace_back(api::query_result_detail{ data_type::type_id, type->second->meta_info });
		}
	}
	ret.success = true;
	return ret;
}

rx_result data_type_repository::check_type (const rx_node_id& id, type_check_context& ctx) const
{
	ctx.push_source(rx_item_type_name(data_type::type_id) + " repository");
	auto temp = get_type_definition(id);
	if (temp)
	{
		return meta::meta_algorithm::data_types_algorithm::check_type(*temp.value(), ctx);
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(data_type::type_id)
			<< " with node_id "
			<< id;
		ctx.add_error(ss.str(), RX_ITEM_NOT_FOUND, rx_critical_severity, temp.errors());
		return true;
	}
}

rx_result data_type_repository::delete_type (rx_node_id id)
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		auto type_id = it->second->meta_info.parent;
		registered_types_.erase(it);
		auto remove_result = inheritance_hash_.remove_from_hash_data(id);
		auto type_ret = platform_types_manager::instance().get_types_resolver().remove_id(id);
		RX_ASSERT(type_ret);
		return true;
	}
	else
	{
		return false;
	}
}

rx_result data_type_repository::initialize (hosting::rx_platform_host* host, const meta_configuration_data_t& data)
{
	std::vector<std::pair<rx_node_id, rx_node_id> > to_add;
	to_add.reserve(registered_types_.size());
	for (const auto& one : registered_types_)
	{
		ns::rx_directory_resolver dirs;
		dirs.add_paths({ one.second->meta_info.path });
		auto parent_id = algorithms::resolve_reference(one.second->meta_info.parent, dirs);
		if (parent_id)
			to_add.emplace_back(one.second->meta_info.id, parent_id.value());
		else
			to_add.emplace_back(one.second->meta_info.id, rx_node_id::null_id);
	}
	auto result = inheritance_hash_.add_to_hash_data(to_add);
	return result;
}

rx_result data_type_repository::update_type (data_type_repository::Tptr what)
{
	const auto& id = what->meta_info.id;
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		it->second = what;
		platform_types_manager::instance().get_types_resolver().update_id(id);
		// TODO Should check and change if parent is different
		/*if (rx_gate::instance().get_platform_status() == rx_platform_status::running)
			inheritance_hash_.add_to_hash_data(id, what->meta_info.parent);*/
		return true;
	}
	else
	{
		return "Node Id: "s + what->meta_info.id.to_string() + " for " + what->meta_info.name + " does not exists";
	}
}

rx_result data_type_repository::type_exists (rx_node_id id) const
{
	auto it = registered_types_.find(id);
	if (it != registered_types_.end())
	{
		return true;
	}
	else
	{
		std::ostringstream ss;
		ss << "Not existing "
			<< rx_item_type_name(data_type::type_id)
			<< " with node_id "
			<< id;
		return ss.str();
	}
}


} // namespace model
} // namespace rx_internal

// explicit template instantiation here!!!
template class rx_internal::model::types_repository<object_type>;
template class rx_internal::model::types_repository<application_type>;
template class rx_internal::model::types_repository<domain_type>;
template class rx_internal::model::types_repository<port_type>;

template class rx_internal::model::simple_types_repository<struct_type>;
template class rx_internal::model::simple_types_repository<variable_type>;
template class rx_internal::model::simple_types_repository<mapper_type>;
template class rx_internal::model::simple_types_repository<filter_type>;
template class rx_internal::model::simple_types_repository<event_type>;
template class rx_internal::model::simple_types_repository<source_type>;
template class rx_internal::model::simple_types_repository<method_type>;
template class rx_internal::model::simple_types_repository<program_type>;
template class rx_internal::model::simple_types_repository<display_type>;
