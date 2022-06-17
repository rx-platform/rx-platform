

/****************************************************************************
*
*  protocols\opcua\rx_opcua_requests.cpp
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


// rx_opcua_requests
#include "protocols/opcua/rx_opcua_requests.h"

#include "rx_opcua_discovery.h"
#include "rx_opcua_session.h"
#include "rx_opcua_attributes.h"
#include "rx_opcua_view.h"

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;

#include "rx_opcua_server.h"


namespace protocols {

namespace opcua {

namespace requests {

// Class protocols::opcua::requests::opcua_requests_repository 


opcua_requests_repository& opcua_requests_repository::instance ()
{
	static opcua_requests_repository g_obj;
	return g_obj;
}

void opcua_requests_repository::init_requests ()
{
	// discovery service set
	opcua_request_ptr ptr = std::make_unique<opcua_discovery::opcua_get_endpoints>();
	register_request(std::move(ptr));
	ptr = std::make_unique<opcua_discovery::opcua_find_servers>();
	register_request(std::move(ptr));
	// session service set
	ptr = std::make_unique<opcua_session::opcua_create_session>();
	register_request(std::move(ptr));
	ptr = std::make_unique<opcua_session::opcua_activate_session>();
	register_request(std::move(ptr));
	ptr = std::make_unique<opcua_session::opcua_close_session>();
	register_request(std::move(ptr));
	// attributes service set
	ptr = std::make_unique<opcua_attributes::opcua_read_request>();
	register_request(std::move(ptr));
	// view service set
	ptr = std::make_unique<opcua_view::opcua_browse_request>();
	register_request(std::move(ptr));
}

rx_result opcua_requests_repository::register_request (opcua_request_ptr req)
{
	rx_node_id req_id = req->get_binary_request_id();
	auto it = registered_requests_.find(req_id);
	if (it == registered_requests_.end())
	{
		registered_requests_.emplace(req_id, std::move(req));
		return true;
	}
	else
	{
		RX_ASSERT(false);
		return "Already registered";
	}
}

opcua_request_ptr opcua_requests_repository::get_request (const rx_node_id& id) const
{
	auto it = registered_requests_.find(id);
	if (it != registered_requests_.end())
	{
		return it->second->create_empty();
	}
	else
	{
		return std::make_unique<opcua_unsupported_request>(id);
	}
}

void opcua_requests_repository::init_responses ()
{
	// discovery service set
	opcua_response_ptr ptr = std::make_unique<opcua_discovery::opcua_get_endpoints_response>();
	register_response(std::move(ptr));
	ptr = std::make_unique<opcua_discovery::opcua_find_servers_response>();
	register_response(std::move(ptr));
	// session service set
	ptr = std::make_unique<opcua_session::opcua_create_session_response>();
	register_response(std::move(ptr));
	ptr = std::make_unique<opcua_session::opcua_activate_session_response>();
	register_response(std::move(ptr));
	ptr = std::make_unique<opcua_session::opcua_close_session_response>();
	register_response(std::move(ptr));
	// attributes service set
	ptr = std::make_unique<opcua_attributes::opcua_read_response>();
	register_response(std::move(ptr));
}

rx_result opcua_requests_repository::register_response (opcua_response_ptr resp)
{
	rx_node_id req_id = resp->get_binary_response_id();
	auto it = registered_responses_.find(req_id);
	if (it == registered_responses_.end())
	{
		registered_responses_.emplace(req_id, std::move(resp));
		return true;
	}
	else
	{
		RX_ASSERT(false);
		return "Already registered";
	}
}

opcua_response_ptr opcua_requests_repository::get_response (const rx_node_id& id) const
{
	auto it = registered_responses_.find(id);
	if (it != registered_responses_.end())
	{
		return it->second->create_empty();
	}
	else
	{
		return opcua_response_ptr();
	}
}


// Class protocols::opcua::requests::opcua_service_fault 

opcua_service_fault::opcua_service_fault (const opcua_request_base& req, uint32_t result_code)
	: opcua_response_base(req)
{
	result = result_code;
}



rx_node_id opcua_service_fault::get_binary_response_id ()
{
	return rx_node_id(opcid_ServiceFault_Encoding_DefaultBinary, 0);
}

opcua_response_ptr opcua_service_fault::create_empty () const
{
	return std::make_unique<opcua_service_fault>();
}

rx_result opcua_service_fault::serialize_binary (binary::ua_binary_ostream& stream) const
{
	return true;
}

rx_result opcua_service_fault::deserialize_binary (binary::ua_binary_istream& stream)
{
	return true;
}


// Class protocols::opcua::requests::opcua_unsupported_request 

opcua_unsupported_request::opcua_unsupported_request (const rx_node_id& id)
      : request_type_id_(id)
{
}



rx_node_id opcua_unsupported_request::get_binary_request_id ()
{
	return rx_node_id::null_id;
}

opcua_request_ptr opcua_unsupported_request::create_empty () const
{
	return opcua_request_ptr();
}

rx_result opcua_unsupported_request::deserialize_binary (binary::ua_binary_istream& stream)
{
	// we do not understand this request so no point in deserializing 
	return true;
}

opcua_response_ptr opcua_unsupported_request::do_job (opcua_server_endpoint_ptr ep)
{
	auto ret_ptr = std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NotSupported);
	return ret_ptr;
}


// Class protocols::opcua::requests::opcua_request_base 


rx_result opcua_request_base::deserialize_binary (binary::ua_binary_istream& stream)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result opcua_request_base::deserialize_header_binary (binary::ua_binary_istream& stream)
{
	stream >> authentication_token;
	stream >> timestamp;
	stream >> request_handle;
	stream >> diagnostics;
	stream >> audit_enrty_id;
	stream >> timeout;
	additional = stream.deserialize_extension([](const rx_node_id& id) {
		if (id.is_null())
			return std::make_unique<ua_extension>();
		else
			return opcua_extension_ptr();
		});
	return true;
}

rx_result opcua_request_base::serialize_binary (binary::ua_binary_ostream& stream)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result opcua_request_base::serialize_header_binary (binary::ua_binary_ostream& stream)
{
	return RX_NOT_IMPLEMENTED;
}


// Class protocols::opcua::requests::opcua_response_base 

opcua_response_base::opcua_response_base (const opcua_request_base& req)
      : request_id(0),
        request_handle(0),
        result(0)
{
	timestamp = rx_time::now();
	request_handle = req.request_handle;
	request_id = req.request_id;
}



rx_result opcua_response_base::deserialize_binary (binary::ua_binary_istream& stream)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result opcua_response_base::deserialize_header_binary (binary::ua_binary_istream& stream)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result opcua_response_base::serialize_binary (binary::ua_binary_ostream& stream) const
{
	return RX_NOT_IMPLEMENTED;
}

rx_result opcua_response_base::serialize_header_binary (binary::ua_binary_ostream& stream)
{
	stream << timestamp;
	stream << request_handle;
	stream << result;
	string_array strings;
	diagnostics.fill_diagnostics_strings(strings, 0xffffffff);
	stream << diagnostics;
	stream << strings;
	stream.serialize_extension(nullptr);
	return true;
}


} // namespace requests
} // namespace opcua
} // namespace protocols

