

/****************************************************************************
*
*  api\rx_meta_api.h
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


#ifndef rx_meta_api_h
#define rx_meta_api_h 1




#include "rx_platform_api.h"
namespace rx_platform
{
namespace api
{
namespace meta
{

template<class typeT>
rx_result rx_delete_runtime(
	const string_type& name // item's path
	, std::function<void(rx_result&&)> callback
	, rx_context ctx);


template<class typeT>
rx_result rx_create_runtime(
	rx_platform::meta::meta_data& meta_info
	, data::runtime_values_data* init_data  // initialization data
	, typename typeT::instance_data_t instance_data
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_update_runtime(
	rx_platform::meta::meta_data& meta_info
	, data::runtime_values_data* init_data  // initialization data
	, typename typeT::instance_data_t instance_data, bool increment_version
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback
	, rx_context ctx);


template<class typeT>
rx_result rx_create_runtime_implicit(
	const string_type& name, const string_type& type_name
	, namespace_item_attributes attributes // required attributes
	, data::runtime_values_data* init_data  // initialization data
	, typename typeT::instance_data_t instance_data
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_create_prototype(
	const string_type& name // item's path
	, const rx_node_id& instance_id // prototype's id
	, const string_type& type_name  // type's path
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_get_runtime(const rx_node_id& id, const string_type name
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_context ctx);

template<class typeT>
rx_result rx_create_type(
	const string_type& name // type's path
	, const string_type& base_name // base type's path
	, typename typeT::smart_ptr prototype // prototype
	, namespace_item_attributes attributes // required attributes
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_update_type(typename typeT::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx);


template<class typeT>
rx_result rx_create_simple_type(
	const string_type& name // type's path
	, const string_type& base_name // base type's path
	, typename typeT::smart_ptr prototype // prototype
	, namespace_item_attributes attributes // required attributes
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_update_simple_type(typename typeT::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx);


rx_result rx_save_item(
	const string_type& name // item's path
	, std::function<void(rx_result&&)> callback
	, rx_context ctx);

template<class T>
rx_result rx_get_type(const rx_node_id& id, const string_type name
	, std::function<void(rx_result_with<typename T::smart_ptr>&&)> callback, rx_context ctx);


template<class T>
rx_result rx_get_simple_type(const rx_node_id& id, const string_type name
	, std::function<void(rx_result_with<typename T::smart_ptr>&&)> callback, rx_context ctx);

}
}
}




#endif
