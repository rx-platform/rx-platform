

/****************************************************************************
*
*  sys_internal\rx_protocol_messages.cpp
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


#include "pch.h"


// rx_internal_protocol
#include "sys_internal/rx_internal_protocol.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"

#include "system/serialization/rx_ser.h"
#include "api/rx_namespace_api.h"
#include "api/rx_meta_api.h"
#include "system/server/rx_server.h"

#include "rx_query_messages.h"
#include "rx_set_messages.h"

using namespace rx_platform;


namespace sys_internal {

namespace rx_protocol {

namespace messages {

// Class sys_internal::rx_protocol::messages::rx_message_base 

rx_message_base::rx_message_base()
{
}


rx_message_base::~rx_message_base()
{
}



rx_result rx_message_base::init_messages ()
{
	auto result = rx_request_message::init_request_messages();
	if (!result)
		return result;

	return result;
}


// Class sys_internal::rx_protocol::messages::error_message 

string_type error_message::type_name = "error";

rx_message_type_t error_message::type_id = rx_error_message_id;


rx_result error_message::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uint("errCode", errorCode))
		return "Error serializing error code";
	if (!stream.write_string("errMsg", errorMessage.c_str()))
		return "Error serializing error message";
	return true;
}

rx_result error_message::deserialize (base_meta_reader& stream)
{
	if (!stream.read_uint("errCode", errorCode))
		return "Error reading error code";
	if (!stream.read_string("errCode", errorMessage))
		return "Error reading error message";
	return true;
}

const string_type& error_message::get_type_name ()
{
  return type_name;

}

rx_message_type_t error_message::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::rx_request_message 

rx_request_message::registered_string_messages_type rx_request_message::registered_string_messages_;

rx_request_message::registered_messages_type rx_request_message::registered_messages_;


rx_result_with<request_message_ptr> rx_request_message::create_request_from_json (const string_type& data, rx_request_id_t& request_id)
{
	// read the header first
	serialization::json_reader reader;
	if (!reader.parse_data(data))
		return "Error parsing Json request";

	string_type msgType;

	if (!reader.start_object("header"))
		return "Header object missing";
	if (!reader.read_int("requestId", request_id))
		return "No request Id";
	if (!reader.read_string("msgType", msgType))
		return "No message type";
	if (!reader.end_object())
		return "Error closing header object";

	auto result = create_request_message(msgType);
	if (!result)
		return result;

	result.value()->request_id = request_id;

	if (!reader.start_object("body"))
		return "Body object missing";

	auto ser_result = result.value()->deserialize(reader);
	if (!ser_result)
		return ser_result.errors();

	if (!reader.end_object())
		return "Error closing body object";

	return result;
}

rx_result_with<request_message_ptr> rx_request_message::create_request_message (const string_type& type)
{
	auto it = registered_string_messages_.find(type);
	if (it == registered_string_messages_.end())
		return type + " is unknown message type!";
	else
		return it->second();
}

rx_result rx_request_message::init_request_messages ()
{
	registered_messages_.emplace(query_messages::browse_request_message::type_id, [] { return std::make_unique<query_messages::browse_request_message>(); });
	registered_messages_.emplace(query_messages::query_request_message::type_id, [] { return std::make_unique<query_messages::query_request_message>(); });
	registered_messages_.emplace(query_messages::get_type_request::type_id, [] { return std::make_unique<query_messages::get_type_request>(); });
	registered_messages_.emplace(query_messages::get_object_request::type_id, [] { return std::make_unique<query_messages::get_object_request>(); });
	registered_messages_.emplace(rx_connection_context_request::type_id, [] { return std::make_unique<rx_connection_context_request>(); });
	registered_messages_.emplace(set_messages::set_type_request::type_id, [] { return std::make_unique<set_messages::set_type_request>(); });
	registered_messages_.emplace(set_messages::update_type_request::type_id, [] { return std::make_unique<set_messages::update_type_request>(); });

	registered_string_messages_.emplace(query_messages::browse_request_message::type_name, [] { return std::make_unique<query_messages::browse_request_message>(); });
	registered_string_messages_.emplace(query_messages::query_request_message::type_name, [] { return std::make_unique<query_messages::query_request_message>(); });
	registered_string_messages_.emplace(query_messages::get_type_request::type_name, [] { return std::make_unique<query_messages::get_type_request>(); });
	registered_string_messages_.emplace(query_messages::get_object_request::type_name, [] { return std::make_unique<query_messages::get_object_request>(); });
	registered_string_messages_.emplace(rx_connection_context_request::type_name, [] { return std::make_unique<rx_connection_context_request>(); });
	registered_string_messages_.emplace(set_messages::set_type_request::type_name, [] { return std::make_unique<set_messages::set_type_request>(); });
	registered_string_messages_.emplace(set_messages::update_type_request::type_name, [] { return std::make_unique<set_messages::update_type_request>(); });
	
	auto ret = meta::queries::rx_query::init_query_types();

	return ret;
}

rx_result_with<request_message_ptr> rx_request_message::create_request_message (rx_message_type_t type)
{
	auto it = registered_messages_.find(type);
	if (it == registered_messages_.end())
		return type + " is unknown message type!";
	else
		return it->second();
}


// Class sys_internal::rx_protocol::messages::rx_connection_context_request 

string_type rx_connection_context_request::type_name = "connCtxReq";

rx_message_type_t rx_connection_context_request::type_id = rx_connection_context_request_id;


rx_result rx_connection_context_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("dir", directory))
		return "Error writing directory";
	if (!stream.write_string("app", application))
		return "Error writing application";
	if (!stream.write_string("domain", domain))
		return "Error writing domain";
	return true;
}

rx_result rx_connection_context_request::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("dir", directory))
		return "Error reading directory";
	if (!stream.read_string("app", application))
		return "Error reading application";
	if (!stream.read_string("domain", domain))
		return "Error reading domain";
	return true;
}

message_ptr rx_connection_context_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	auto request_id = this->request_id;
	if (!directory.empty())
	{
		auto result = port->set_current_directory(directory);
		if (!result)
		{
			auto ret_value = std::make_unique<error_message>();
			for (const auto& one : result.errors())
				ret_value->errorMessage += one;
			ret_value->errorCode = 13;
			ret_value->request_id = request_id;
			return ret_value;
		}
	}
	auto response = std::make_unique<rx_connection_context_response>();
	response->directory = port->get_current_directory_path();
	return response;
}

const string_type& rx_connection_context_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_connection_context_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::rx_connection_context_response 

string_type rx_connection_context_response::type_name = "connCtxResp";

rx_message_type_t rx_connection_context_response::type_id = rx_connection_context_response_id;


rx_result rx_connection_context_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("dir", directory))
		return "Error writing directory";
	if (!stream.write_string("app", application))
		return "Error writing application";
	if (!stream.write_string("domain", domain))
		return "Error writing domain";
	if (!stream.write_id("appid", application_id))
		return "Error writing application id";
	if (!stream.write_id("domainid", domain_id))
		return "Error writing domain id";
	return true;
}

rx_result rx_connection_context_response::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("dir", directory))
		return "Error reading directory";
	if (!stream.read_string("app", application))
		return "Error reading application";
	if (!stream.read_string("domain", domain))
		return "Error reading domain";
	if (!stream.read_id("appid", application_id))
		return "Error reading application";
	if (!stream.read_id("domainid", domain_id))
		return "Error reading domain id";
	return true;
}

const string_type& rx_connection_context_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_connection_context_response::get_type_id ()
{
  return type_id;

}


} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal



