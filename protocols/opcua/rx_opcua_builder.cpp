

/****************************************************************************
*
*  protocols\opcua\rx_opcua_builder.cpp
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

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;

// rx_opcua_builder
#include "protocols/opcua/rx_opcua_builder.h"

namespace protocols {
namespace opcua {
namespace opcua_addr_space {

template<typename T>
rx_node_id get_type_id();


template<>
rx_node_id get_type_id<bool>()
{
	return rx_node_id::opcua_standard_id(opcid_Boolean);
}

template<typename T>
std::unique_ptr<opcua_property_node> create_standard_property_node(const string_type& name, uint32_t code, const T& val
	, const rx_node_id& type_id = rx_node_id::null_id, opc_access_level access=opc_access_level::current_read)
{
	auto node = std::make_unique<opcua_property_node>();
	node->browse_name.name = name;
	node->display_name.text = name;
	node->node_id = rx_node_id::opcua_standard_id(code);
	node->value.value = variant_type(val);
	if (type_id)
		node->data_type = type_id;
	else
		node->data_type = rx_node_id::opcua_standard_id(node->value.value.get_type());
	node->value_rank = -1;
	node->access_level = access;
	node->user_access_level = access;
	
	return node;
}

template<typename T>
std::unique_ptr<opcua_property_node> create_standard_property_node(const string_type& name, uint32_t code, const std::vector<T>& val
	, const rx_node_id& type_id = rx_node_id::null_id, opc_access_level access = opc_access_level::current_read)
{
	auto node = std::make_unique<opcua_property_node>();
	node->browse_name.name = name;
	node->display_name.text = name;
	node->node_id = rx_node_id::opcua_standard_id(code);
	node->value.value = variant_type(val);
	if (type_id)
		node->data_type = type_id;
	else
		node->data_type = rx_node_id::opcua_standard_id(node->value.value.get_type());
	node->value_rank = 1;
	node->access_level = access;
	node->user_access_level = access;
	
	return node;
}

rx_result build_standard_address_space_UANodeSet2(opcua_std_address_space& server);

rx_result build_standard_address_space(opcua_std_address_space& server, const string_type& server_uri)
{
	auto result = build_standard_address_space_UANodeSet2(server);
	if (!result)
		return result;

	/*auto node = create_standard_property_node("ServerArray", opcid_Server_ServerArray, string_array{
			server_uri
		});
	result = server.register_node(std::move(node));
	if (!result)
		return result;

	node = create_standard_property_node("NamespaceArray", opcid_Server_NamespaceArray, string_array{
			"http://opcfoundation.org/UA/",
			"http://rx-platform.org/UA/"
		});
	result = server.register_node(std::move(node));
	if (!result)
		return result;*/

	return result;
}


}// opcua_addr_space
}// opcua
}// protocols
