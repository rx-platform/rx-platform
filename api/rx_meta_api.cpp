

/****************************************************************************
*
*  api\rx_meta_api.cpp
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

#include "rx_platform.h"

// rx_meta_api
#include "api/rx_meta_api.h"

#include "system/meta/rx_meta_data.h"
#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"

namespace rx_platform
{
namespace api
{
namespace meta
{

rx_result rx_delete_object(const string_type& name
	, std::function<void(rx_result&&)> callback, rx_context ctx)
{
	model::algorithms::runtime_model_algorithm<object_type>::delete_runtime(
		name, ctx.directory, callback, ctx.object);
	return true;
}

template<class typeT>
rx_result rx_create_runtime(const string_type& name, const string_type& type_name, data::runtime_values_data* init_data
	, namespace_item_attributes attributes, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_context ctx)
{
	data::runtime_values_data* ptr_copy = nullptr;
	if (init_data)// copy values to resolve lifetime
		ptr_copy = new data::runtime_values_data(std::move(*init_data));
	model::algorithms::runtime_model_algorithm<typeT>::create_runtime(
		name, type_name, attributes, ptr_copy, ctx.directory, callback, ctx.object);
	return true;
}

template rx_result rx_create_runtime<object_type>(const string_type& name, const string_type& type_name, data::runtime_values_data* init_data
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback, rx_context ctx);
template rx_result rx_create_runtime<domain_type>(const string_type& name, const string_type& type_name, data::runtime_values_data* init_data
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<domain_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_create_runtime<application_type>(const string_type& name, const string_type& type_name, data::runtime_values_data* init_data
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<application_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_create_runtime<port_type>(const string_type& name, const string_type& type_name, data::runtime_values_data* init_data
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<port_type::RTypePtr>&&)> callback, rx_context ctx);


template<class typeT>
rx_result rx_create_prototype(const string_type& name, const rx_node_id& instance_id, const string_type& type_name
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_context ctx)
{
	model::algorithms::runtime_model_algorithm<typeT>::create_prototype(
		name, type_name, ctx.directory, namespace_item_attributes::namespace_item_null, callback, ctx.object);
	return true;
}
template rx_result rx_create_prototype<object_type>(const string_type& name, const rx_node_id& instance_id, const string_type& type_name
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback, rx_context ctx);
template rx_result rx_create_prototype<domain_type>(const string_type& name, const rx_node_id& instance_id, const string_type& type_name
	, std::function<void(rx_result_with<domain_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_create_prototype<application_type>(const string_type& name, const rx_node_id& instance_id, const string_type& type_name
	, std::function<void(rx_result_with<application_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_create_prototype<port_type>(const string_type& name, const rx_node_id& instance_id, const string_type& type_name
	, std::function<void(rx_result_with<rx_port_ptr>&&)> callback, rx_context ctx);


template<class typeT>
rx_result rx_create_type(const string_type& name
	, const string_type& base_name
	, typename typeT::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx)
{
	model::algorithms::types_model_algorithm<typeT>::create_type(
		name, base_name, prototype, ctx.directory, attributes | namespace_item_attributes::namespace_item_full_type_access, callback, ctx.object);
	return true;
}
template rx_result rx_create_type<object_type>(const string_type& name, const string_type& base_name
	, object_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<object_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_type<domain_type>(const string_type& name
	, const string_type& base_name
	, domain_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<domain_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_type<port_type>(const string_type& name
	, const string_type& base_name
	, port_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<port_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_type<application_type>(const string_type& name
	, const string_type& base_name
	, application_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<application_type::smart_ptr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_update_type(typename typeT::smart_ptr prototype
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx)
{
	model::algorithms::types_model_algorithm<typeT>::update_type(
		prototype, ctx.directory, callback, ctx.object);
	return true;
}
template rx_result rx_update_type<object_type>(object_type::smart_ptr prototype
	, std::function<void(rx_result_with<object_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_update_type<domain_type>(domain_type::smart_ptr prototype
	, std::function<void(rx_result_with<domain_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_update_type<port_type>(port_type::smart_ptr prototype
	, std::function<void(rx_result_with<port_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_update_type<application_type>(application_type::smart_ptr prototype
	, std::function<void(rx_result_with<application_type::smart_ptr>&&)> callback
	, rx_context ctx);


rx_result recursive_save_directory(rx_directory_ptr dir)
{
	platform_directories_type dirs;
	platform_items_type items;
	dir->get_content(dirs, items, "");
	for (auto& dir : dirs)
	{
		auto result = recursive_save_directory(dir);
		if (!result)
			return result;
	}
	for (auto& item : items)
	{
		auto result = item->save();
		if (!result)
			return result;
	}
	return true;
}
// save functionalities
rx_result save_item_helper(const string_type& name, rx_directory_ptr dir)
{
	// check to see if we are to save whole directory?
	auto dir_ptr = dir->get_sub_directory(name);
	if (dir_ptr)
	{// this is a directory do recursive save!!!
		return recursive_save_directory(dir_ptr);
	}
	else
	{// just plain item save
		auto item = dir->get_sub_item(name);
		if (!item)
		{
			return name + " does not exists!";
		}
		return item->save();
	}
}

rx_result rx_save_item(const string_type& name
	, std::function<void(rx_result&&)> callback, rx_context ctx)
{
	std::function<rx_result()> func = [=]() {
		return save_item_helper(name, ctx.directory);
	};
	rx_do_with_callback<rx_result, rx_object_ptr>(func, RX_DOMAIN_META, callback, ctx.object);
	
	return true;
}

template<class T>
rx_result rx_get_type(const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<typename T::smart_ptr>&&)> callback
	, rx_context ctx, tl::type2type<T>)
{
	std::function<rx_result_with<typename T::smart_ptr>()> func = [=]() {
		return model::platform_types_manager::instance().get_type_cache<T>().get_type_definition(id);
	};
	rx_do_with_callback<rx_result_with<typename T::smart_ptr>, rx_object_ptr>(func, RX_DOMAIN_META, callback, ctx.object);

	return true;
}


rx_result rx_get_object_type(const rx_node_id& id // item's path
	, const string_type name
	, std::function<void(rx_result_with<rx_object_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_type(id, name, callback, ctx, tl::type2type<object_type>());
}

rx_result rx_get_domain_type(const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<rx_domain_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_type(id, name, callback, ctx, tl::type2type<domain_type>());
}

rx_result rx_get_application_type(const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<rx_application_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_type(id, name, callback, ctx, tl::type2type<application_type>());
}

rx_result rx_get_port_type(const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<rx_port_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_type(id, name, callback, ctx, tl::type2type<port_type>());
}


template<class T>
rx_result rx_get_simple_type(const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<typename T::smart_ptr>&&)> callback
	, rx_context ctx, tl::type2type<T>)
{
	std::function<rx_result_with<typename T::smart_ptr>()> func = [=]() {
		return model::platform_types_manager::instance().get_simple_type_cache<T>().get_type_definition(id);
	};
	rx_do_with_callback<rx_result_with<typename T::smart_ptr>, rx_object_ptr>(func, RX_DOMAIN_META, callback, ctx.object);

	return true;
}

rx_result rx_get_struct_type(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<struct_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_simple_type(id, name, callback, ctx, tl::type2type<struct_type>());
}

rx_result rx_get_variable_type(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<variable_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_simple_type(id, name, callback, ctx, tl::type2type<variable_type>());
}

rx_result rx_get_source_type(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<source_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_simple_type(id, name, callback, ctx, tl::type2type<source_type>());
}

rx_result rx_get_filter_type(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<filter_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_simple_type(id, name, callback, ctx, tl::type2type<filter_type>());
}

rx_result rx_get_event_type(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<event_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_simple_type(id, name, callback, ctx, tl::type2type<event_type>());
}

rx_result rx_get_mapper_type(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<mapper_type_ptr>&&)> callback
	, rx_context ctx)
{
	return rx_get_simple_type(id, name, callback, ctx, tl::type2type<mapper_type>());
}

}
}
}


