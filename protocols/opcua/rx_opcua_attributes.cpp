

/****************************************************************************
*
*  protocols\opcua\rx_opcua_attributes.cpp
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


// rx_opcua_attributes
#include "protocols/opcua/rx_opcua_attributes.h"

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;

#include "rx_opcua_server.h"
#include "rx_opcua_addr_space.h"


namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_attributes {

// Class protocols::opcua::requests::opcua_attributes::opcua_read_request 


rx_node_id opcua_read_request::get_binary_request_id ()
{
	return rx_node_id::opcua_standard_id(opcid_ReadRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_read_request::create_empty () const
{
	return std::make_unique<opcua_read_request>();
}

rx_result opcua_read_request::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream >> max_age;
	stream >> time_stamps;
	stream.deserialize_array(to_read);
	return true;
}

opcua_response_ptr opcua_read_request::do_job (opcua_server_endpoint_ptr ep)
{
	if(to_read.empty())
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NothingToDo);
	if (max_age < 0)
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_MaxAgeInvalid);
	auto addr_space = ep->get_address_space();
	if(addr_space==nullptr)
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_InternalError);

	auto ret_ptr = std::make_unique<opcua_read_response>(*this);

	addr_space->read_attributes(to_read, ret_ptr->results);

	return ret_ptr;
}


// Class protocols::opcua::requests::opcua_attributes::opcua_read_response 

opcua_read_response::opcua_read_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_read_response::get_binary_response_id ()
{
	return rx_node_id::opcua_standard_id(opcid_ReadResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_read_response::create_empty () const
{
	return std::make_unique<opcua_read_response>();
}

rx_result opcua_read_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream << results;
	stream << diagnostics_info;
	return true;
}


// Class protocols::opcua::requests::opcua_attributes::opcua_write_request 


rx_node_id opcua_write_request::get_binary_request_id ()
{
	return rx_node_id::opcua_standard_id(opcid_WriteRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_write_request::create_empty () const
{
	return std::make_unique<opcua_write_request>();
}

rx_result opcua_write_request::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream.deserialize_array(to_write);
	return true;
}

opcua_response_ptr opcua_write_request::do_job (opcua_server_endpoint_ptr ep)
{
	if (to_write.empty())
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NothingToDo);
	auto addr_space = ep->get_address_space();
	if (addr_space == nullptr)
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_InternalError);


	return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NotSupported);

	/*auto ret_ptr = std::make_unique<opcua_write_response>(*this);

	addr_space->write_attributes(to_write, ret_ptr->results);

	return ret_ptr;*/
}


// Class protocols::opcua::requests::opcua_attributes::opcua_write_response 

opcua_write_response::opcua_write_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_write_response::get_binary_response_id ()
{
	return rx_node_id::opcua_standard_id(opcid_WriteResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_write_response::create_empty () const
{
	return std::make_unique<opcua_write_response>();
}

rx_result opcua_write_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream << results;
	stream << diagnostics_info;
	return true;
}


} // namespace opcua_attributes
} // namespace requests
} // namespace opcua
} // namespace protocols

