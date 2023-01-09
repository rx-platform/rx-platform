

/****************************************************************************
*
*  protocols\opcua\rx_monitoreditem_set.cpp
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


// rx_monitoreditem_set
#include "protocols/opcua/rx_monitoreditem_set.h"

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;
#include "rx_opcua_params.h"
using namespace protocols::opcua::common;

#include "rx_opcua_server.h"
#include "rx_opcua_client.h"


namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_monitoreditem {

// Class protocols::opcua::requests::opcua_monitoreditem::opcua_create_mon_items_request 

opcua_create_mon_items_request::opcua_create_mon_items_request (uint32_t req_id, uint32_t req_handle)
      : subscription_id(0)
	, opcua_request_base(req_id, req_handle)
{
}



rx_node_id opcua_create_mon_items_request::get_binary_request_id ()
{
	return rx_node_id::opcua_standard_id(opcid_CreateMonitoredItemsRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_create_mon_items_request::create_empty () const
{
	return std::make_unique<opcua_create_mon_items_request>();
}

rx_result opcua_create_mon_items_request::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream >> subscription_id;
	stream >> timestamps_to_return;
	stream.deserialize_array(to_create);
	return true;
}

opcua_response_ptr opcua_create_mon_items_request::do_job (opcua_server_endpoint_ptr ep)
{
	if (to_create.empty())
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NothingToDo);

	auto subs_obj = ep->get_subscriptions();

	auto ret_ptr = std::make_unique<opcua_create_mon_items_response>(*this);
	ret_ptr->results.reserve(to_create.size());
	for (auto& one : to_create)
	{
		ret_ptr->results.push_back(subs_obj->create_monitored_item(subscription_id, timestamps_to_return, one));
	}
	return ret_ptr;
}

rx_result opcua_create_mon_items_request::serialize_binary (binary::ua_binary_ostream& stream)
{
	stream << subscription_id;
	stream << timestamps_to_return;
	stream.serialize_array(to_create);
	return true;
}


// Class protocols::opcua::requests::opcua_monitoreditem::opcua_create_mon_items_response 

opcua_create_mon_items_response::opcua_create_mon_items_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_create_mon_items_response::get_binary_response_id ()
{
	return rx_node_id::opcua_standard_id(opcid_CreateMonitoredItemsResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_create_mon_items_response::create_empty () const
{
	return std::make_unique<opcua_create_mon_items_response>();
}

rx_result opcua_create_mon_items_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream.serialize_array(results);
	stream << diagnostics_info;
	return true;
}

rx_result opcua_create_mon_items_response::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream.deserialize_array(results);
	stream >> diagnostics_info;
	return true;
}

rx_result opcua_create_mon_items_response::process_response (opcua_client_endpoint_ptr ep)
{
	return ep->create_items_response(results);
}


// Class protocols::opcua::requests::opcua_monitoreditem::opcua_delete_mon_items_request 

opcua_delete_mon_items_request::opcua_delete_mon_items_request (uint32_t req_id, uint32_t req_handle)
      : subscription_id(0)
	, opcua_request_base(req_id, req_handle)
{
}



rx_node_id opcua_delete_mon_items_request::get_binary_request_id ()
{
	return rx_node_id::opcua_standard_id(opcid_DeleteMonitoredItemsRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_delete_mon_items_request::create_empty () const
{
	return std::make_unique<opcua_delete_mon_items_request>();
}

rx_result opcua_delete_mon_items_request::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream >> subscription_id;
	stream >> to_delete;
	return true;
}

opcua_response_ptr opcua_delete_mon_items_request::do_job (opcua_server_endpoint_ptr ep)
{
	if (to_delete.empty())
		return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NothingToDo);

	auto subs_obj = ep->get_subscriptions();

	auto ret_ptr = std::make_unique<opcua_delete_mon_items_response>(*this);
	ret_ptr->results.reserve(to_delete.size());
	for (auto& one : to_delete)
	{
		ret_ptr->results.push_back(subs_obj->delete_monitored_item(subscription_id, one));
	}
	return ret_ptr;
}

rx_result opcua_delete_mon_items_request::serialize_binary (binary::ua_binary_ostream& stream)
{
	stream << subscription_id;
	stream << to_delete;
	return true;
}


// Class protocols::opcua::requests::opcua_monitoreditem::opcua_delete_mon_items_response 

opcua_delete_mon_items_response::opcua_delete_mon_items_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_delete_mon_items_response::get_binary_response_id ()
{
	return rx_node_id::opcua_standard_id(opcid_DeleteMonitoredItemsResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_delete_mon_items_response::create_empty () const
{
	return std::make_unique<opcua_delete_mon_items_response>();
}

rx_result opcua_delete_mon_items_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream << results;
	stream << diagnostics_info;
	return true;
}

rx_result opcua_delete_mon_items_response::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream >> results;
	stream >> diagnostics_info;
	return true;
}

rx_result opcua_delete_mon_items_response::process_response (opcua_client_endpoint_ptr ep)
{
	return RX_NOT_IMPLEMENTED;
}


} // namespace opcua_monitoreditem
} // namespace requests
} // namespace opcua
} // namespace protocols

