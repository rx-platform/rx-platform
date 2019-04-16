

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



rx_result rx_delete_object(
	const string_type& name // item's path
	, std::function<void(rx_result&&)> callback
	, rx_context ctx);


rx_result rx_create_object(
	const string_type& name // item's path
	, const string_type& type_name // type's path
	, data::runtime_values_data* init_data  // initialization data
	, namespace_item_attributes attributes // required attributes
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback
	, rx_context ctx);

rx_result rx_create_port(
	const string_type& name // item's path
	, const string_type& type_name // type's path
	, data::runtime_values_data* init_data  // initialization data
	, namespace_item_attributes attributes // required attributes
	, std::function<void(rx_result_with<rx_port_ptr>&&)> callback
	, rx_context ctx);


rx_result rx_create_prototype(
	const string_type& name // item's path
	, const rx_node_id& instance_id // prototype's id
	, const string_type& type_name  // type's path
	, std::function<void(rx_result_with<rx_object_ptr>&&)> callback
	, rx_context ctx);


rx_result rx_create_object_type(
	const string_type& name // type's path
	, const string_type& base_name // base type's path
	, rx_object_type_ptr prototype // prototype
	, namespace_item_attributes attributes // required attributes
	, std::function<void(rx_result_with<rx_object_type_ptr>&&)> callback
	, rx_context ctx);


rx_result rx_save_item(
	const string_type& name // item's path
	, std::function<void(rx_result&&)> callback
	, rx_context ctx);


rx_result rx_get_derived_object_types(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<query_result>&&)> callback
	, rx_context ctx);

rx_result rx_get_derived_domain_types(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<query_result>&&)> callback
	, rx_context ctx);

rx_result rx_get_derived_application_types(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<query_result>&&)> callback
	, rx_context ctx);

rx_result rx_get_derived_port_types(
	const rx_node_id& id // item's id
	, const string_type name
	, std::function<void(rx_result_with<query_result>&&)> callback
	, rx_context ctx);

}
}
}




#endif
