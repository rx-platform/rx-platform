

/****************************************************************************
*
*  api\rx_meta_api.h
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


#ifndef rx_meta_api_h
#define rx_meta_api_h 1




#include "rx_platform_api.h"
#include "system/callbacks/rx_callback.h"

namespace rx_platform
{


namespace api
{
namespace meta
{

template<class typeT>
rx_result rx_delete_runtime(const rx_item_reference& ref
	, rx_result_callback&& callback);


template<class typeT>
rx_result rx_create_runtime(
	typename typeT::instance_data_t instance_data
	, rx_result_with_callback<typename typeT::RTypePtr>&& callback);

template<class typeT>
rx_result rx_update_runtime(
	typename typeT::instance_data_t instance_data, rx_update_runtime_data update_data
	, rx_result_with_callback<typename typeT::RTypePtr>&& callback);


template<class typeT>
rx_result rx_create_prototype(
	typename typeT::instance_data_t instance_data
	, rx_result_with_callback<typename typeT::RTypePtr>&& callback);

template<class typeT>
rx_result rx_get_runtime(const rx_item_reference& ref
	, rx_result_with_callback<typename typeT::RTypePtr>&& callback);

template<class typeT>
rx_result rx_create_type(
	typename typeT::smart_ptr prototype // prototype
	, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

template<class typeT>
rx_result rx_update_type(typename typeT::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<typename typeT::smart_ptr>&& callback);


template<class typeT>
rx_result rx_create_simple_type(
	typename typeT::smart_ptr prototype // prototype
	, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

template<class typeT>
rx_result rx_update_simple_type(typename typeT::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<typename typeT::smart_ptr>&& callback);

rx_result rx_create_relation_type(
	object_types::relation_type::smart_ptr prototype // prototype
	, rx_result_with_callback<object_types::relation_type::smart_ptr>&& callback);

rx_result rx_update_relation_type(object_types::relation_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<object_types::relation_type::smart_ptr>&& callback);


rx_result rx_create_data_type(
	basic_types::data_type::smart_ptr prototype // prototype
	, rx_result_with_callback<basic_types::data_type::smart_ptr>&& callback);

rx_result rx_update_data_type(basic_types::data_type::smart_ptr prototype, rx_update_type_data update_data
	, rx_result_with_callback<basic_types::data_type::smart_ptr>&& callback);


rx_result rx_save_item(const string_type& ref
	, rx_result_callback&& callback);

template<class T>
rx_result rx_get_type(const rx_item_reference& ref
	, rx_result_with_callback<typename T::smart_ptr>&& callback);


template<class T>
rx_result rx_get_simple_type(const rx_item_reference& ref
	, rx_result_with_callback<typename T::smart_ptr>&& callback);

rx_result rx_get_relation_type(const rx_item_reference& ref
	, rx_result_with_callback<object_types::relation_type::smart_ptr>&& callback);

rx_result rx_get_data_type(const rx_item_reference& ref
	, rx_result_with_callback<basic_types::data_type::smart_ptr>&& callback);


template<class T>
rx_result rx_delete_type(const rx_item_reference& ref
	, rx_result_callback&& callback);


template<class T>
rx_result rx_delete_simple_type(const rx_item_reference& ref
	, rx_result_callback&& callback);

rx_result rx_delete_relation_type(const rx_item_reference& ref
	, rx_result_callback&& callback);

rx_result rx_delete_data_type(const rx_item_reference& ref
	, rx_result_callback&& callback);

}
}
}




#endif
