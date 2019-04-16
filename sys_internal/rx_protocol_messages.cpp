

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


// Class sys_internal::rx_protocol::messages::browse_request_message 

string_type browse_request_message::type_name = "brwReq";


rx_result browse_request_message::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("path", path.c_str()))
		return "Error serializing path";
	if (!stream.write_string("filter", filter.c_str()))
		return "Error serializing filter";
	return true;
}

rx_result browse_request_message::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("path", path))
		return "Error reading path";
	if (!stream.read_string("filter", filter))
		return "Error reading filter";
	return true;
}

message_ptr browse_request_message::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	auto requestId = this->reguestId;
	auto result = api::ns::rx_list_directory(path, filter,
		[requestId, port] (rx_result_with<api::ns::directory_browse_result>&& result) mutable
		{
			if (result)
			{
				auto response = std::make_unique<browse_response_message>();
				for (const auto one : result.value().directories)
				{
					response->items.emplace_back(one->get_type_name(), one->meta_info());
				}
				for (const auto one : result.value().items)
				{
					response->items.emplace_back(one->get_type_name(), one->meta_info());
				}
				response->reguestId = requestId;
				port->data_processed(std::move(response));

			}
			else
			{
				auto ret_value = std::make_unique<error_message>();
				for (const auto& one : result.errors())
					ret_value->errorMessage += one;
				ret_value->errorCode = 14;
				ret_value->reguestId = requestId;
				port->data_processed(std::move(ret_value));
			}

		}, ctx);

	if (!result)
	{
		auto ret_value = std::make_unique<error_message>();
		for (const auto& one : result.errors())
			ret_value->errorMessage += one;
		ret_value->errorCode = 13;
		ret_value->reguestId = requestId;
		return ret_value;
	}
	else
	{
		// just return we send callback
		return message_ptr();
	}
}

const string_type& browse_request_message::get_type_name ()
{
  return type_name;

}


// Class sys_internal::rx_protocol::messages::error_message 

string_type error_message::type_name = "error";


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


// Class sys_internal::rx_protocol::messages::browse_response_message 

string_type browse_response_message::type_name = "brwResp";


rx_result browse_response_message::serialize (base_meta_writer& stream) const
{
	if (!stream.start_array("items", items.size()))
		return "Error serializing array of items";
	for (const auto& one : items)
	{
		auto result = one.second.serialize_meta_data(stream, 0, one.first);
		if (!result)
		{
			result.register_error("Error serializing meta item "s + one.second.get_name());
			return result;
		}
	}
	if (!stream.end_array())
		return "Error serializing array of items at the end";
	
	return true;
}

rx_result browse_response_message::deserialize (base_meta_reader& stream)
{
	if (!stream.start_array("items"))
		return "Error reading array of items";
	items.clear();
	while(!stream.array_end())
	{
		string_type type;
		meta::meta_data one;
		auto result = one.deserialize_meta_data(stream, 0, type);
		if (!result)
		{
			result.register_error("Error serializing meta item "s + one.get_name());
			return result;
		}
		items.emplace_back(std::move(type), std::move(one));
	}

	return true;
}

const string_type& browse_response_message::get_type_name ()
{
  return type_name;

}


// Class sys_internal::rx_protocol::messages::rx_request_message 

rx_request_message::registered_messages_type rx_request_message::registered_messages_;


rx_result_with<request_message_ptr> rx_request_message::create_request_from_json (const string_type& data)
{
	// read the header first
	serialization::json_reader reader;
	if (!reader.parse_data(data))
		return "Error parsing Json request";

	string_type msgType;
	int requestId;

	if (!reader.start_object("header"))
		return "Header object missing";
	if (!reader.read_int("requestId", requestId))
		return "No request Id";
	if (!reader.read_string("msgType", msgType))
		return "No message type";
	if (!reader.end_object())
		return "Error closing header object";

	auto result = create_request_message(msgType);
	if (!result)
		return result;

	result.value()->reguestId = requestId;

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
	auto it = registered_messages_.find(type);
	if (it == registered_messages_.end())
		return type + " is unknown message type!";
	else
		return it->second();
}

rx_result rx_request_message::init_request_messages ()
{
	registered_messages_.emplace(browse_request_message::type_name, [] { return std::make_unique<browse_request_message>(); });
	registered_messages_.emplace(query_request_message::type_name, [] { return std::make_unique<query_request_message>(); });
	
	auto ret = meta::queries::rx_query::init_query_types();

	return ret;
}


// Class sys_internal::rx_protocol::messages::query_response_message 

string_type query_response_message::type_name = "queryResp";


rx_result query_response_message::serialize (base_meta_writer& stream) const
{
	if (!stream.start_array("items", items.size()))
		return "Error serializing array of items";
	for (const auto& one : items)
	{
		auto result = one.second.serialize_meta_data(stream, 0, one.first);
		if (!result)
		{
			result.register_error("Error serializing meta item "s + one.second.get_name());
			return result;
		}
	}
	if (!stream.end_array())
		return "Error serializing array of items at the end";

	return true;
}

rx_result query_response_message::deserialize (base_meta_reader& stream)
{
	if (!stream.start_array("items"))
		return "Error reading array of items";
	items.clear();
	while (!stream.array_end())
	{
		string_type type;
		meta::meta_data one;
		auto result = one.deserialize_meta_data(stream, 0, type);
		if (!result)
		{
			result.register_error("Error serializing meta item "s + one.get_name());
			return result;
		}
		items.emplace_back(std::move(type), std::move(one));
	}

	return true;
}

const string_type& query_response_message::get_type_name ()
{
  return type_name;

}


// Class sys_internal::rx_protocol::messages::query_request_message 

string_type query_request_message::type_name = "queryReq";


rx_result query_request_message::serialize (base_meta_writer& stream) const
{
	if (!stream.start_array("queries", queries.size()))
		return "Error serializing queries array";
	for (const auto& one : queries)
	{
		auto ret = one->serialize(stream);
		if (!ret)
			return ret;
	}
	if (!stream.end_array())
		return "Error serializing queries array end";
	
	if (!stream.write_bool("intersection", intersection))
		return "Error serializing all";

	return true;
}

rx_result query_request_message::deserialize (base_meta_reader& stream)
{
	if (!stream.start_array("queries"))
		return "Error serializing queries array";
	while (!stream.array_end())
	{
		auto one = meta::queries::rx_query::create_query(stream);
		if (!one)
			return one.errors();
		queries.emplace_back(std::move(one.value()));
	}

	if (!stream.read_bool("intersection", intersection))
		return "Error serializing all";

	return true;
}

message_ptr query_request_message::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	auto requestId = this->reguestId;
	rx_node_id id = rx_node_id::null_id;

	auto callback = [requestId, port](rx_result_with<api::query_result>&& result) mutable
	{
		if (result)
		{
			auto response = std::make_unique<query_response_message>();

			for (const auto one : result.value().items)
			{
				response->items.emplace_back(one->get_type_name(), one->meta_info());
			}
			response->reguestId = requestId;
			port->data_processed(std::move(response));

		}
		else
		{
			auto ret_value = std::make_unique<error_message>();
			for (const auto& one : result.errors())
				ret_value->errorMessage += one;
			ret_value->errorCode = 14;
			ret_value->reguestId = requestId;
			port->data_processed(std::move(ret_value));
		}

	};

	rx_result result = api::ns::rx_query_model(queries, callback, ctx);

	if (!result)
	{
		auto ret_value = std::make_unique<error_message>();
		for (const auto& one : result.errors())
			ret_value->errorMessage += one;
		ret_value->errorCode = 13;
		ret_value->reguestId = requestId;
		return ret_value;
	}
	else
	{
		// just return we send callback
		return message_ptr();
	}
}

const string_type& query_request_message::get_type_name ()
{
  return type_name;

}


} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal

