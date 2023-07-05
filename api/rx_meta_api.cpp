

/****************************************************************************
*
*  api\rx_meta_api.cpp
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

#include "rx_platform.h"

// rx_meta_api
#include "api/rx_meta_api.h"

#include "system/server/rx_ns.h"
#include "system/runtime/rx_objbase.h"
#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "sys_internal/rx_async_functions.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/runtime/rx_runtime_holder.h"
#include "system/meta/rx_runtime_data.h"

namespace rx_platform
{
namespace api
{
namespace meta
{

template<class typeT>
rx_result rx_delete_runtime(const rx_item_reference& ref
	, rx_result_callback&& callback)
{
	rx_internal::model::algorithms::runtime_model_algorithm<typeT>::delete_runtime(
		ref, std::move(callback));
	return true;
}
template rx_result rx_delete_runtime<object_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_runtime<port_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_runtime<domain_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_runtime<application_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);

template<class typeT>
rx_result rx_create_runtime(
	typename typeT::instance_data_t instance_data
	, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	instance_data.meta_info = create_meta_for_new(instance_data.meta_info);
	rx_internal::model::algorithms::runtime_model_algorithm<typeT>::create_runtime(std::move(instance_data)
		, data::runtime_values_data()
		, std::move(callback));
	return true;
}

template rx_result rx_create_runtime<object_type>(runtime_data::object_runtime_data instance_data
	, rx_result_with_callback<rx_object_ptr>&& callback);
template rx_result rx_create_runtime<domain_type>(runtime_data::domain_runtime_data instance_data
	, rx_result_with_callback<rx_domain_ptr>&& callback);
template rx_result rx_create_runtime<application_type>(runtime_data::application_runtime_data instance_data
	, rx_result_with_callback<rx_application_ptr>&& callback);
template rx_result rx_create_runtime<port_type>(runtime_data::port_runtime_data instance_data
	, rx_result_with_callback<rx_port_ptr>&& callback);


template<class typeT>
rx_result rx_update_runtime(
	typename typeT::instance_data_t instance_data, rx_update_runtime_data update_data
	, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_internal::model::algorithms::runtime_model_algorithm<typeT>::update_runtime(
		std::move(instance_data), update_data, std::move(callback));
	return true;
}

template rx_result rx_update_runtime<object_type>(runtime_data::object_runtime_data instance_data, rx_update_runtime_data update_data
	, rx_result_with_callback<rx_object_ptr>&& callback);
template rx_result rx_update_runtime<domain_type>(runtime_data::domain_runtime_data instance_data, rx_update_runtime_data update_data
	, rx_result_with_callback<rx_domain_ptr>&& callback);
template rx_result rx_update_runtime<application_type>(runtime_data::application_runtime_data instance_data, rx_update_runtime_data update_data
	, rx_result_with_callback<rx_application_ptr>&& callback);
template rx_result rx_update_runtime<port_type>(runtime_data::port_runtime_data instance_data, rx_update_runtime_data update_data
	, rx_result_with_callback<rx_port_ptr>&& callback);


template<class typeT>
rx_result rx_create_prototype(typename typeT::instance_data_t instance_data
	, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_internal::model::algorithms::runtime_model_algorithm<typeT>::create_prototype(std::move(instance_data), std::move(callback));
	return true;
}
template rx_result rx_create_prototype<object_type>(object_type::instance_data_t instance_data
	, rx_result_with_callback<rx_object_ptr>&& callback);
template rx_result rx_create_prototype<domain_type>(domain_type::instance_data_t instance_data
	, rx_result_with_callback<rx_domain_ptr>&& callback);
template rx_result rx_create_prototype<application_type>(application_type::instance_data_t instance_data
	, rx_result_with_callback<rx_application_ptr>&& callback);
template rx_result rx_create_prototype<port_type>(port_type::instance_data_t instance_data
	, rx_result_with_callback<rx_port_ptr>&& callback);


template<class typeT>
rx_result rx_get_runtime(const rx_item_reference& ref
	, rx_result_with_callback<typename typeT::RTypePtr>&& callback)
{
	rx_internal::model::algorithms::runtime_model_algorithm<typeT>::get_runtime(ref, std::move(callback));
	return true;
}

template rx_result rx_get_runtime<object_type>(const rx_item_reference& ref
	, rx_result_with_callback<rx_object_ptr>&& callback);
template rx_result rx_get_runtime<application_type>(const rx_item_reference& ref
	, rx_result_with_callback<rx_application_ptr>&& callback);
template rx_result rx_get_runtime<domain_type>(const rx_item_reference& ref
	, rx_result_with_callback<rx_domain_ptr>&& callback);
template rx_result rx_get_runtime<port_type>(const rx_item_reference& ref
	, rx_result_with_callback<rx_port_ptr>&& callback);


template<class typeT>
rx_result rx_create_type(
	typename typeT::smart_ptr prototype
	, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_internal::model::algorithms::types_model_algorithm<typeT>::create_type(prototype, std::move(callback));
	return true;
}
template rx_result rx_create_type<object_type>(object_type::smart_ptr prototype
	, rx_result_with_callback<object_type::smart_ptr>&&);
template rx_result rx_create_type<domain_type>(domain_type::smart_ptr prototype
	, rx_result_with_callback<domain_type::smart_ptr>&& callback);
template rx_result rx_create_type<port_type>(port_type::smart_ptr prototype
	, rx_result_with_callback<port_type::smart_ptr>&& callback);
template rx_result rx_create_type<application_type>(application_type::smart_ptr prototype
	, rx_result_with_callback<application_type::smart_ptr>&& callback);

template<class typeT>
rx_result rx_update_type(typename typeT::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_internal::model::algorithms::types_model_algorithm<typeT>::update_type(prototype, std::move(update_data), std::move(callback));
	return true;
}
template rx_result rx_update_type<object_type>(object_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<object_type::smart_ptr>&&);
template rx_result rx_update_type<domain_type>(domain_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<domain_type::smart_ptr>&&);
template rx_result rx_update_type<port_type>(port_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<port_type::smart_ptr>&& callback);
template rx_result rx_update_type<application_type>(application_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<application_type::smart_ptr>&& callback);


template<class typeT>
rx_result rx_create_simple_type(typename typeT::smart_ptr prototype
	, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	prototype->meta_info = create_meta_for_new(prototype->meta_info);
	rx_internal::model::algorithms::simple_types_model_algorithm<typeT>::create_type(prototype, std::move(callback));
	return true;
}
template rx_result rx_create_simple_type<struct_type>(struct_type::smart_ptr prototype
	, rx_result_with_callback<struct_type::smart_ptr>&& callback);
template rx_result rx_create_simple_type<variable_type>(variable_type::smart_ptr prototype
	, rx_result_with_callback<variable_type::smart_ptr>&& callback);

template rx_result rx_create_simple_type<source_type>(source_type::smart_ptr prototype
	, rx_result_with_callback<source_type::smart_ptr>&& callback);
template rx_result rx_create_simple_type<filter_type>(filter_type::smart_ptr prototype
	, rx_result_with_callback<filter_type::smart_ptr>&& callback);
template rx_result rx_create_simple_type<event_type>(event_type::smart_ptr prototype
	, rx_result_with_callback<event_type::smart_ptr>&& callback);

template rx_result rx_create_simple_type<mapper_type>(mapper_type::smart_ptr prototype
	, rx_result_with_callback<mapper_type::smart_ptr>&& callback);

template rx_result rx_create_simple_type<program_type>(program_type::smart_ptr prototype
	, rx_result_with_callback<program_type::smart_ptr>&& callback);
template rx_result rx_create_simple_type<method_type>(method_type::smart_ptr prototype
	, rx_result_with_callback<method_type::smart_ptr>&& callback);
template rx_result rx_create_simple_type<display_type>(display_type::smart_ptr prototype
	, rx_result_with_callback<display_type::smart_ptr>&& callback);



template<class typeT>
rx_result rx_update_simple_type(typename typeT::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<typename typeT::smart_ptr>&& callback)
{
	rx_internal::model::algorithms::simple_types_model_algorithm<typeT>::update_type(
		prototype, std::move(update_data), std::move(callback));
	return true;
}
template rx_result rx_update_simple_type<struct_type>(struct_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<struct_type::smart_ptr>&& callback);
template rx_result rx_update_simple_type<variable_type>(variable_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<variable_type::smart_ptr>&& callback);

template rx_result rx_update_simple_type<source_type>(source_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<source_type::smart_ptr>&& callback);
template rx_result rx_update_simple_type<filter_type>(filter_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<filter_type::smart_ptr>&& callback);
template rx_result rx_update_simple_type<event_type>(event_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<event_type::smart_ptr>&& callback);

template rx_result rx_update_simple_type<mapper_type>(mapper_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<mapper_type::smart_ptr>&& callback);

template rx_result rx_update_simple_type<program_type>(program_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<program_type::smart_ptr>&& callback);
template rx_result rx_update_simple_type<method_type>(method_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<method_type::smart_ptr>&& callback);
template rx_result rx_update_simple_type<display_type>(display_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<display_type::smart_ptr>&& callback);



rx_result rx_create_relation_type(typename relation_type::smart_ptr prototype // prototype
	, rx_result_with_callback<typename relation_type::smart_ptr>&& callback)
{
	prototype->meta_info = create_meta_for_new(prototype->meta_info);
	rx_internal::model::algorithms::relation_types_algorithm::create_type(prototype, std::move(callback));
	return true;
}

rx_result rx_update_relation_type(typename relation_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<typename relation_type::smart_ptr>&& callback)
{
	rx_internal::model::algorithms::relation_types_algorithm::update_type(
		prototype, std::move(update_data), std::move(callback));
	return true;
}


rx_result rx_create_data_type(typename data_type::smart_ptr prototype // prototype
	, rx_result_with_callback<typename data_type::smart_ptr>&& callback)
{
	prototype->meta_info = create_meta_for_new(prototype->meta_info);
	rx_internal::model::algorithms::data_types_model_algorithm::create_type(prototype, std::move(callback));
	return true;
}

rx_result rx_update_data_type(typename data_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<typename data_type::smart_ptr>&& callback)
{
	rx_internal::model::algorithms::data_types_model_algorithm::update_type(
		prototype, std::move(update_data), std::move(callback));
	return true;
}



rx_result recursive_save_directory(rx_directory_ptr dir)
{
	platform_directories_type dirs;
	platform_items_type items;
	dir->list_content(dirs, items, "");
	for (auto& dir : dirs)
	{
		auto result = recursive_save_directory(dir);
		if (!result)
			return result;
	}
	for (auto& item : items)
	{
		auto temp_result = rx_internal::model::algorithms::get_platform_item_sync(item.get_type(), item.get_meta().id);
		if (!temp_result)
			return "Error retrieving "s + item.get_meta().get_full_path() + " from it's parent's directory.";
		auto result = temp_result.value()->save();
		if (!result)
			return result;
	}
	return true;
}
// save functionalities
rx_result save_item_helper(string_type path)
{
	// check to see if we are to save whole directory?
	auto dir_ptr = rx_gate::instance().get_directory(path);
	if (dir_ptr)
	{// this is a directory do recursive save!!!
		return recursive_save_directory(dir_ptr);
	}
	else
	{// just plain item save
		auto item = rx_gate::instance().get_namespace_item(path);
		if (!item)
		{
			return path + " does not exists!";
		}
		auto temp_result = rx_internal::model::algorithms::get_platform_item_sync(item.get_type(), item.get_meta().id);
		if (!temp_result)
			return "Error retrieving "s + item.get_meta().get_full_path() + " from it's parent's directory.";
		auto result = temp_result.value()->save();
		return result;
	}
}

rx_result rx_save_item(const string_type& path
	, rx_result_callback&& callback)
{
	rx_post_function_to(RX_DOMAIN_META, callback.get_anchor(), [](string_type path, rx_result_callback&& callback)
		{
			auto result = save_item_helper(path);
			callback(std::move(result));
		}
		, string_type(path), std::move(callback));

	return true;
}

template<class T>
rx_result rx_get_type(const rx_item_reference& ref
	, rx_result_with_callback<typename T::smart_ptr>&& callback)
{
	rx_internal::model::algorithms::types_model_algorithm<T>::get_type(ref, std::move(callback));
	return true;
}

template rx_result rx_get_type<object_type>(const rx_item_reference& ref
	, rx_result_with_callback<object_type::smart_ptr>&& callback);
template rx_result rx_get_type<domain_type>(const rx_item_reference& ref
	, rx_result_with_callback<domain_type::smart_ptr>&& callback);
template rx_result rx_get_type<application_type>(const rx_item_reference& ref
	, rx_result_with_callback<application_type::smart_ptr>&& callback);
template rx_result rx_get_type<port_type>(const rx_item_reference& ref
	, rx_result_with_callback<port_type::smart_ptr>&& callback);



template<class T>
rx_result rx_get_simple_type(const rx_item_reference& ref
	, rx_result_with_callback<typename T::smart_ptr>&& callback)
{
	rx_internal::model::algorithms::simple_types_model_algorithm<T>::get_type(ref, std::move(callback));
	return true;
}


template rx_result rx_get_simple_type<struct_type>(const rx_item_reference& ref
	, rx_result_with_callback<struct_type::smart_ptr>&& callback);
template rx_result rx_get_simple_type<variable_type>(const rx_item_reference& ref
	, rx_result_with_callback<variable_type::smart_ptr>&& callback);

template rx_result rx_get_simple_type<source_type>(const rx_item_reference& ref
	, rx_result_with_callback<source_type::smart_ptr>&& callback);
template rx_result rx_get_simple_type<filter_type>(const rx_item_reference& ref
	, rx_result_with_callback<filter_type::smart_ptr>&& callback);
template rx_result rx_get_simple_type<event_type>(const rx_item_reference& ref
	, rx_result_with_callback<event_type::smart_ptr>&& callback);
template rx_result rx_get_simple_type<mapper_type>(const rx_item_reference& ref
	, rx_result_with_callback<mapper_type::smart_ptr>&& callback);

template rx_result rx_get_simple_type<program_type>(const rx_item_reference& ref
	, rx_result_with_callback<program_type::smart_ptr>&& callback);
template rx_result rx_get_simple_type<method_type>(const rx_item_reference& ref
	, rx_result_with_callback<method_type::smart_ptr>&& callback);
template rx_result rx_get_simple_type<display_type>(const rx_item_reference& ref
	, rx_result_with_callback<display_type::smart_ptr>&& callback);

rx_result rx_get_relation_type(const rx_item_reference& ref
	, rx_result_with_callback<relation_type::smart_ptr>&& callback)
{
	rx_internal::model::algorithms::relation_types_algorithm::get_type(ref, std::move(callback));
	return true;
}

rx_result rx_get_data_type(const rx_item_reference& ref
	, rx_result_with_callback<data_type::smart_ptr>&& callback)
{
	rx_internal::model::algorithms::data_types_model_algorithm::get_type(ref, std::move(callback));
	return true;
}

template<class T>
rx_result rx_delete_type(const rx_item_reference& ref
	, rx_result_callback&& callback)
{
	rx_internal::model::algorithms::types_model_algorithm<T>::delete_type(ref, std::move(callback));
	return true;
}

template rx_result rx_delete_type<object_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_type<domain_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_type<application_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_type<port_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);



template<class T>
rx_result rx_delete_simple_type(const rx_item_reference& ref
	, rx_result_callback&& callback)
{
	rx_internal::model::algorithms::simple_types_model_algorithm<T>::delete_type(ref, std::move(callback));
	return true;
}


template rx_result rx_delete_simple_type<struct_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_simple_type<variable_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_simple_type<source_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_simple_type<filter_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_simple_type<event_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_simple_type<mapper_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_simple_type<program_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_simple_type<method_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);
template rx_result rx_delete_simple_type<display_type>(const rx_item_reference& ref
	, rx_result_callback&& callback);


rx_result rx_delete_relation_type(const rx_item_reference& ref
	, rx_result_callback&& callback)
{
	rx_internal::model::algorithms::relation_types_algorithm::delete_type(ref, std::move(callback));
	return true;
}

rx_result rx_delete_data_type(const rx_item_reference& ref
	, rx_result_callback&& callback)
{
	rx_internal::model::algorithms::data_types_model_algorithm::delete_type(ref, std::move(callback));
	return true;
}


struct item_transaction_data
{
	size_t current_idx;
	bool done = false;
	operator bool()
	{
		return !done;
	}
};

}
}
}


