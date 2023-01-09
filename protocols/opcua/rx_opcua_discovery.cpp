

/****************************************************************************
*
*  protocols\opcua\rx_opcua_discovery.cpp
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

#include "system/server/rx_server.h"
#include "system/server/rx_file_helpers.h"

// rx_opcua_discovery
#include "protocols/opcua/rx_opcua_discovery.h"

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;

#include "rx_opcua_server.h"


namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_discovery {

// Class protocols::opcua::requests::opcua_discovery::opcua_get_endpoints 


rx_node_id opcua_get_endpoints::get_binary_request_id ()
{
	return rx_node_id(opcid_GetEndpointsRequest_Encoding_DefaultBinary, 0);
}

opcua_request_ptr opcua_get_endpoints::create_empty () const
{
	return std::make_unique<opcua_get_endpoints>();
}

rx_result opcua_get_endpoints::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream >> endpoint;
	stream >> locale_ids;
	stream >> profile_uris;
	return true;
}

opcua_response_ptr opcua_get_endpoints::do_job (opcua_server_endpoint_ptr ep)
{
	auto ret_ptr = std::make_unique<opcua_get_endpoints_response>(*this);
	
	ret_ptr->endpoints.push_back(std::move(ep->get_endpoint_description(endpoint, false)));

	return ret_ptr;
}


// Class protocols::opcua::requests::opcua_discovery::opcua_get_endpoints_response 

opcua_get_endpoints_response::opcua_get_endpoints_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_get_endpoints_response::get_binary_response_id ()
{
	return rx_node_id(opcid_GetEndpointsResponse_Encoding_DefaultBinary, 0);
}

opcua_response_ptr opcua_get_endpoints_response::create_empty () const
{
	return std::make_unique<opcua_get_endpoints_response>();
}

rx_result opcua_get_endpoints_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream.serialize_array(endpoints);
	return true;
}


// Class protocols::opcua::requests::opcua_discovery::opcua_find_servers 


rx_node_id opcua_find_servers::get_binary_request_id ()
{
	return rx_node_id(opcid_FindServersRequest_Encoding_DefaultBinary, 0);
}

opcua_request_ptr opcua_find_servers::create_empty () const
{
	return std::make_unique<opcua_find_servers>();
}

rx_result opcua_find_servers::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream >> endpoint;
	stream >> locale_ids;
	stream >> server_uris;
	return true;
}

opcua_response_ptr opcua_find_servers::do_job (opcua_server_endpoint_ptr ep)
{
	auto ret_ptr = std::make_unique<opcua_find_servers_response>(*this);

	ret_ptr->servers.push_back(std::move(ep->get_application_description(endpoint)));

	return ret_ptr;
}


// Class protocols::opcua::requests::opcua_discovery::opcua_find_servers_response 

opcua_find_servers_response::opcua_find_servers_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_find_servers_response::get_binary_response_id ()
{
	return rx_node_id(opcid_FindServersResponse_Encoding_DefaultBinary, 0);
}

opcua_response_ptr opcua_find_servers_response::create_empty () const
{
	return std::make_unique<opcua_find_servers_response>();
}

rx_result opcua_find_servers_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream.serialize_array(servers);
	return true;
}


} // namespace opcua_discovery
} // namespace requests
} // namespace opcua
} // namespace protocols

