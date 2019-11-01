

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
#include "system/server/rx_async_functions.h"

namespace rx_platform
{
namespace api
{
namespace meta
{

template<class typeT>
rx_result rx_delete_runtime(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx)
{
	model::algorithms::runtime_model_algorithm<typeT>::delete_runtime(
		ref, ctx.directory, callback, ctx.object);
	return true;
}
template rx_result rx_delete_runtime<object_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_runtime<port_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_runtime<domain_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_runtime<application_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);

template<class typeT>
rx_result rx_create_runtime(
	rx_platform::meta::meta_data& meta_info, data::runtime_values_data* init_data
	, typename typeT::instance_data_t instance_data
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_context ctx)
{
	data::runtime_values_data* ptr_copy = nullptr;
	if (init_data)// copy values to resolve lifetime
		ptr_copy = new data::runtime_values_data(std::move(*init_data));
	model::algorithms::runtime_model_algorithm<typeT>::create_runtime(
		meta_info, ptr_copy, instance_data, ctx.directory, callback, ctx.object);
	return true;
}

template rx_result rx_create_runtime<object_type>(
	rx_platform::meta::meta_data& meta_info , data::runtime_values_data* init_data
	, runtime::objects::object_instance_data instance_data
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback, rx_context ctx);
template rx_result rx_create_runtime<domain_type>(
	rx_platform::meta::meta_data& meta_info, data::runtime_values_data* init_data
	, runtime::objects::domain_instance_data instance_data
	, std::function<void(rx_result_with<domain_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_create_runtime<application_type>(
	rx_platform::meta::meta_data& meta_info, data::runtime_values_data* init_data
	, runtime::objects::application_instance_data instance_data
	, std::function<void(rx_result_with<application_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_create_runtime<port_type>(
	rx_platform::meta::meta_data& meta_info, data::runtime_values_data* init_data
	, runtime::objects::port_instance_data instance_data
	, std::function<void(rx_result_with<port_type::RTypePtr>&&)> callback, rx_context ctx);


template<class typeT>
rx_result rx_update_runtime(
	rx_platform::meta::meta_data& meta_info, data::runtime_values_data* init_data
	, typename typeT::instance_data_t instance_data, bool increment_version
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_context ctx)
{
	data::runtime_values_data* ptr_copy = nullptr;
	if (init_data)// copy values to resolve lifetime
		ptr_copy = new data::runtime_values_data(std::move(*init_data));
	model::algorithms::runtime_model_algorithm<typeT>::update_runtime(
		meta_info, ptr_copy, instance_data, increment_version, ctx.directory, callback, ctx.object);
	return true;
}

template rx_result rx_update_runtime<object_type>(
	rx_platform::meta::meta_data& meta_info, data::runtime_values_data* init_data
	, runtime::objects::object_instance_data instance_data, bool increment_version
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback, rx_context ctx);
template rx_result rx_update_runtime<domain_type>(
	rx_platform::meta::meta_data& meta_info, data::runtime_values_data* init_data
	, runtime::objects::domain_instance_data instance_data, bool increment_version
	, std::function<void(rx_result_with<domain_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_update_runtime<application_type>(
	rx_platform::meta::meta_data& meta_info, data::runtime_values_data* init_data
	, runtime::objects::application_instance_data instance_data, bool increment_version
	, std::function<void(rx_result_with<application_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_update_runtime<port_type>(
	rx_platform::meta::meta_data& meta_info, data::runtime_values_data* init_data
	, runtime::objects::port_instance_data instance_data, bool increment_version
	, std::function<void(rx_result_with<port_type::RTypePtr>&&)> callback, rx_context ctx);


template<class typeT>
rx_result rx_create_runtime_implicit(
	const string_type& name, const string_type& type_name
	, namespace_item_attributes attributes // required attributes
	, data::runtime_values_data* init_data  // initialization data
	, typename typeT::instance_data_t instance_data
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback
	, rx_context ctx)
{
	data::runtime_values_data* ptr_copy = nullptr;
	if (init_data)// copy values to resolve lifetime
		ptr_copy = new data::runtime_values_data(std::move(*init_data));
	model::algorithms::runtime_model_algorithm<typeT>::create_runtime_implicit(name, type_name, attributes
		,ptr_copy, instance_data, ctx.directory, callback, ctx.object);
	return true;
}

template rx_result rx_create_runtime_implicit<object_type>(
	const string_type& name, const string_type& type_name
	, namespace_item_attributes attributes // required attributes
	, data::runtime_values_data* init_data  // initialization data
	, runtime::objects::object_instance_data instance_data
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_runtime_implicit<domain_type>(
	const string_type& name, const string_type& type_name
	, namespace_item_attributes attributes // required attributes
	, data::runtime_values_data* init_data  // initialization data
	, runtime::objects::domain_instance_data instance_data
	, std::function<void(rx_result_with<rx_domain_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_runtime_implicit<port_type>(
	const string_type& name, const string_type& type_name
	, namespace_item_attributes attributes // required attributes
	, data::runtime_values_data* init_data  // initialization data
	, runtime::objects::port_instance_data instance_data
	, std::function<void(rx_result_with<port_type::RTypePtr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_runtime_implicit<application_type>(
	const string_type& name, const string_type& type_name
	, namespace_item_attributes attributes // required attributes
	, data::runtime_values_data* init_data  // initialization data
	, runtime::objects::application_instance_data instance_data
	, std::function<void(rx_result_with<rx_application_ptr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_create_prototype(rx_platform::meta::meta_data& meta_info, typename typeT::instance_data_t instance_data
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_context ctx)
{
	model::algorithms::runtime_model_algorithm<typeT>::create_prototype(meta_info, instance_data, ctx.safe_directory(), callback, ctx.object);
	return true;
}
template rx_result rx_create_prototype<object_type>(rx_platform::meta::meta_data& meta_info, object_type::instance_data_t instance_data
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback, rx_context ctx);
template rx_result rx_create_prototype<domain_type>(rx_platform::meta::meta_data& meta_info, domain_type::instance_data_t instance_data
	, std::function<void(rx_result_with<domain_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_create_prototype<application_type>(rx_platform::meta::meta_data& meta_info, application_type::instance_data_t instance_data
	, std::function<void(rx_result_with<application_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_create_prototype<port_type>(rx_platform::meta::meta_data& meta_info, port_type::instance_data_t instance_data
	, std::function<void(rx_result_with<rx_port_ptr>&&)> callback, rx_context ctx);


template<class typeT>
rx_result rx_get_runtime(const item_reference& ref
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_context ctx)
{
	model::algorithms::runtime_model_algorithm<typeT>::get_runtime(ref,
		ctx.safe_directory(), callback, ctx.object);
	return true;
}

template rx_result rx_get_runtime<object_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename object_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_get_runtime<application_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename application_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_get_runtime<domain_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename domain_type::RTypePtr>&&)> callback, rx_context ctx);
template rx_result rx_get_runtime<port_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename port_type::RTypePtr>&&)> callback, rx_context ctx);


template<class typeT>
rx_result rx_create_type(const string_type& name
	, const item_reference& base_reference
	, typename typeT::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx)
{
	model::algorithms::types_model_algorithm<typeT>::create_type(
		name, base_reference, prototype, ctx.directory, attributes | namespace_item_attributes::namespace_item_full_type_access, callback, ctx.object);
	return true;
}
template rx_result rx_create_type<object_type>(const string_type& name
	, const item_reference& base_reference
	, object_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<object_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_type<domain_type>(const string_type& name
	, const item_reference& base_reference
	, domain_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<domain_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_type<port_type>(const string_type& name
	, const item_reference& base_reference
	, port_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<port_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_type<application_type>(const string_type& name
	, const item_reference& base_reference
	, application_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<application_type::smart_ptr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_update_type(typename typeT::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx)
{
	model::algorithms::types_model_algorithm<typeT>::update_type(
		prototype, ctx.directory, increment_version, callback, ctx.object);
	return true;
}
template rx_result rx_update_type<object_type>(object_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<object_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_update_type<domain_type>(domain_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<domain_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_update_type<port_type>(port_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<port_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_update_type<application_type>(application_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<application_type::smart_ptr>&&)> callback
	, rx_context ctx);


template<class typeT>
rx_result rx_create_simple_type(const string_type& name
	, const item_reference& base_reference // base reference
	, typename typeT::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx)
{
	model::algorithms::simple_types_model_algorithm<typeT>::create_type(
		name, base_reference, prototype, ctx.directory, attributes | namespace_item_attributes::namespace_item_full_type_access, callback, ctx.object);
	return true;
}
template rx_result rx_create_simple_type<struct_type>(const string_type& name
	, const item_reference& base_reference
	, struct_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<struct_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_simple_type<variable_type>(const string_type& name
	, const item_reference& base_reference
	, variable_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<variable_type::smart_ptr>&&)> callback
	, rx_context ctx);

template rx_result rx_create_simple_type<source_type>(const string_type& name
	, const item_reference& base_reference
	, source_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<source_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_simple_type<filter_type>(const string_type& name
	, const item_reference& base_reference
	, filter_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<filter_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_create_simple_type<event_type>(const string_type& name
	, const item_reference& base_reference
	, event_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<event_type::smart_ptr>&&)> callback
	, rx_context ctx);

template rx_result rx_create_simple_type<mapper_type>(const string_type& name
	, const item_reference& base_reference
	, mapper_type::smart_ptr prototype
	, namespace_item_attributes attributes
	, std::function<void(rx_result_with<mapper_type::smart_ptr>&&)> callback
	, rx_context ctx);


template<class typeT>
rx_result rx_update_simple_type(typename typeT::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx)
{
	model::algorithms::simple_types_model_algorithm<typeT>::update_type(
		prototype, ctx.directory, increment_version, callback, ctx.object);
	return true;
}
template rx_result rx_update_simple_type<struct_type>(struct_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<struct_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_update_simple_type<variable_type>(variable_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<variable_type::smart_ptr>&&)> callback
	, rx_context ctx);

template rx_result rx_update_simple_type<source_type>(source_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<source_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_update_simple_type<filter_type>(filter_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<filter_type::smart_ptr>&&)> callback
	, rx_context ctx);
template rx_result rx_update_simple_type<event_type>(event_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<event_type::smart_ptr>&&)> callback
	, rx_context ctx);

template rx_result rx_update_simple_type<mapper_type>(mapper_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<mapper_type::smart_ptr>&&)> callback
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
	rx_do_with_callback<rx_result, rx_reference_ptr>(func, RX_DOMAIN_META, callback, ctx.object);
	
	return true;
}

template<class T>
rx_result rx_get_type(const item_reference& ref
	, std::function<void(rx_result_with<typename T::smart_ptr>&&)> callback, rx_context ctx)
{
	model::algorithms::types_model_algorithm<T>::get_type(ref,
		ctx.directory, callback, ctx.object);
	return true;
}

template rx_result rx_get_type<object_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename object_type::smart_ptr>&&)> callback, rx_context ctx);
template rx_result rx_get_type<domain_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename domain_type::smart_ptr>&&)> callback, rx_context ctx);
template rx_result rx_get_type<application_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename application_type::smart_ptr>&&)> callback, rx_context ctx);
template rx_result rx_get_type<port_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename port_type::smart_ptr>&&)> callback, rx_context ctx);



template<class T>
rx_result rx_get_simple_type(const item_reference& ref
	, std::function<void(rx_result_with<typename T::smart_ptr>&&)> callback, rx_context ctx)
{
	model::algorithms::simple_types_model_algorithm<T>::get_type(ref,
		ctx.directory, callback, ctx.object);
	return true;
}


template rx_result rx_get_simple_type<struct_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename struct_type::smart_ptr>&&)> callback, rx_context ctx);
template rx_result rx_get_simple_type<variable_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename variable_type::smart_ptr>&&)> callback, rx_context ctx);
template rx_result rx_get_simple_type<source_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename source_type::smart_ptr>&&)> callback, rx_context ctx);
template rx_result rx_get_simple_type<filter_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename filter_type::smart_ptr>&&)> callback, rx_context ctx);
template rx_result rx_get_simple_type<event_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename event_type::smart_ptr>&&)> callback, rx_context ctx);
template rx_result rx_get_simple_type<mapper_type>(const item_reference& ref
	, std::function<void(rx_result_with<typename mapper_type::smart_ptr>&&)> callback, rx_context ctx);



template<class T>
rx_result rx_delete_type(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx)
{
	model::algorithms::types_model_algorithm<T>::delete_type(ref,
		ctx.directory, callback, ctx.object);
	return true;
}

template rx_result rx_delete_type<object_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_type<domain_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_type<application_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_type<port_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);



template<class T>
rx_result rx_delete_simple_type(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx)
{
	model::algorithms::simple_types_model_algorithm<T>::delete_type(ref,
		ctx.directory, callback, ctx.object);
	return true;
}


template rx_result rx_delete_simple_type<struct_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_simple_type<variable_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_simple_type<source_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_simple_type<filter_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_simple_type<event_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);
template rx_result rx_delete_simple_type<mapper_type>(const item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);


}
}
}


