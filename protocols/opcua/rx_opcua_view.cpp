

/****************************************************************************
*
*  protocols\opcua\rx_opcua_view.cpp
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


// rx_opcua_view
#include "protocols/opcua/rx_opcua_view.h"

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;

#include "rx_opcua_server.h"
#include "rx_opcua_addr_space.h"


namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_view {

// Class protocols::opcua::requests::opcua_view::opcua_browse_request 


rx_node_id opcua_browse_request::get_binary_request_id ()
{
	return rx_node_id::opcua_standard_id(opcid_BrowseRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_browse_request::create_empty () const
{
	return std::make_unique<opcua_browse_request>();
}

rx_result opcua_browse_request::deserialize_binary (binary::ua_binary_istream& stream)
{
	view.deserialize(stream);
	stream >> max_references;
	stream.deserialize_array(to_browse);
	return true;
}

opcua_response_ptr opcua_browse_request::do_job (opcua_server_endpoint_ptr ep)
{
	if(to_browse.empty())
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NothingToDo);

	auto addr_space = ep->get_address_space();
	if (addr_space == nullptr)
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_InternalError);

	std::unique_ptr< opcua_browse_response> ret_msg = std::make_unique<opcua_browse_response>(*this);

	std::vector<browse_result_internal> results;
	results.reserve(to_browse.size());
	addr_space->browse(view, to_browse, results);
	ret_msg->results.reserve(results.size());
	for (auto& one : results)
	{
		opcua_browse_result one_result;
		one_result.status_code = one.status_code;
		one_result.references = std::move(one.references);

		ret_msg->results.push_back(std::move(one_result));
	}
	return ret_msg;
}


// Class protocols::opcua::requests::opcua_view::opcua_browse_response 

opcua_browse_response::opcua_browse_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_browse_response::get_binary_response_id ()
{
	return rx_node_id::opcua_standard_id(opcid_BrowseResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_browse_response::create_empty () const
{
	return std::make_unique<opcua_browse_response>();
}

rx_result opcua_browse_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream.serialize_array(results);
	stream << diagnostics_info;
	return true;
}


// Class protocols::opcua::requests::opcua_view::opcua_translate_request 


rx_node_id opcua_translate_request::get_binary_request_id ()
{
	return rx_node_id::opcua_standard_id(opcid_TranslateBrowsePathsToNodeIdsRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_translate_request::create_empty () const
{
	return std::make_unique<opcua_translate_request>();
}

rx_result opcua_translate_request::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream.deserialize_array(browse_paths);
	return true;
}

opcua_response_ptr opcua_translate_request::do_job (opcua_server_endpoint_ptr ep)
{
	if (browse_paths.empty())
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NothingToDo);

	auto addr_space = ep->get_address_space();
	if (addr_space == nullptr)
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_InternalError);

	std::unique_ptr<opcua_translate_response> ret_msg = std::make_unique<opcua_translate_response>(*this);

	ret_msg->results.reserve(browse_paths.size());
	addr_space->translate(browse_paths, ret_msg->results, addr_space);
	return ret_msg;
}


// Class protocols::opcua::requests::opcua_view::opcua_translate_response 

opcua_translate_response::opcua_translate_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_translate_response::get_binary_response_id ()
{
	return rx_node_id::opcua_standard_id(opcid_TranslateBrowsePathsToNodeIdsResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_translate_response::create_empty () const
{
	return std::make_unique<opcua_translate_response>();
}

rx_result opcua_translate_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream.serialize_array(results);
	stream << diagnostics_info;
	return true;
}


} // namespace opcua_view
} // namespace requests
} // namespace opcua
} // namespace protocols

