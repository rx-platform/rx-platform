

/****************************************************************************
*
*  api\rx_meta_api.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
	const rx_item_reference& ref
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
	rx_platform::meta::meta_data& meta_info, typename typeT::instance_data_t instance_data
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_get_runtime(const rx_item_reference& ref
	, std::function<void(rx_result_with<typename typeT::RTypePtr>&&)> callback, rx_context ctx);

template<class typeT>
rx_result rx_create_type(
	const string_type& name // type's path
	, const rx_item_reference& base_reference // base reference
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
	, const rx_item_reference& base_reference // base reference
	, typename typeT::smart_ptr prototype // prototype
	, namespace_item_attributes attributes // required attributes
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx);

template<class typeT>
rx_result rx_update_simple_type(typename typeT::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<typename typeT::smart_ptr>&&)> callback
	, rx_context ctx);

rx_result rx_create_relation_type(
	const string_type& name // type's path
	, const rx_item_reference& base_reference // base reference
	, object_types::relation_type::smart_ptr prototype // prototype
	, namespace_item_attributes attributes // required attributes
	, std::function<void(rx_result_with<typename object_types::relation_type::smart_ptr>&&)> callback
	, rx_context ctx);

rx_result rx_update_relation_type(object_types::relation_type::smart_ptr prototype, bool increment_version
	, std::function<void(rx_result_with<object_types::relation_type::smart_ptr>&&)> callback
	, rx_context ctx);


rx_result rx_save_item(
	const string_type& name // item's path
	, std::function<void(rx_result&&)> callback
	, rx_context ctx);

template<class T>
rx_result rx_get_type(const rx_item_reference& ref
	, std::function<void(rx_result_with<typename T::smart_ptr>&&)> callback, rx_context ctx);


template<class T>
rx_result rx_get_simple_type(const rx_item_reference& ref
	, std::function<void(rx_result_with<typename T::smart_ptr>&&)> callback, rx_context ctx);

rx_result rx_get_relation_type(const rx_item_reference& ref
	, std::function<void(rx_result_with<object_types::relation_type::smart_ptr>&&)> callback, rx_context ctx);



template<class T>
rx_result rx_delete_type(const rx_item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);


template<class T>
rx_result rx_delete_simple_type(const rx_item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);

rx_result rx_delete_relation_type(const rx_item_reference& ref
	, std::function<void(rx_result&&)> callback, rx_context ctx);

template<class resultT, class refT, class... Args>
rx_result rx_do_with_items(const std::vector<rx_namespace_item>& items, std::function<resultT(Args...)> verb, std::function<void(resultT)> callback, rx_context ctx);

}
}
}




#endif
