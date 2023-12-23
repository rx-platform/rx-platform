

/****************************************************************************
*
*  protocols\opcua\rx_method_set.cpp
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


// rx_method_set
#include "protocols/opcua/rx_method_set.h"

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;

#include "rx_opcua_server.h"
#include "rx_opcua_addr_space.h"


namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_method {

// Class protocols::opcua::requests::opcua_method::opcua_call_request 


rx_node_id opcua_call_request::get_binary_request_id ()
{
	return rx_node_id::opcua_standard_id(opcid_CallRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_call_request::create_empty () const
{
	return std::make_unique<opcua_call_request>();
}

rx_result opcua_call_request::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream.deserialize_array(to_execute);
	return true;
}

opcua_response_ptr opcua_call_request::do_job (opcua_server_endpoint_ptr ep)
{
	if (to_execute.empty())
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NothingToDo);
	auto addr_space = ep->get_address_space();
	if (addr_space == nullptr)
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_InternalError);


	auto req = std::make_unique<opcua_call_request>();
	move_header_to(req.get());
	req->to_execute = std::move(to_execute);
	ep->queue_execute_request(std::move(req));

	return opcua_response_ptr();
}


// Class protocols::opcua::requests::opcua_method::opcua_call_response 

opcua_call_response::opcua_call_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_call_response::get_binary_response_id ()
{
	return rx_node_id::opcua_standard_id(opcid_CallResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_call_response::create_empty () const
{
	return std::make_unique<opcua_call_response>();
}

rx_result opcua_call_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream.serialize_array(results);
	stream << diagnostics_info;
	return true;
}


} // namespace opcua_method
} // namespace requests
} // namespace opcua
} // namespace protocols

