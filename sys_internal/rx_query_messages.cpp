

/****************************************************************************
*
*  sys_internal\rx_query_messages.cpp
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


// rx_query_messages
#include "sys_internal/rx_query_messages.h"

#include "sys_internal/rx_internal_protocol.h"
#include "system/serialization/rx_ser.h"
#include "api/rx_namespace_api.h"
#include "api/rx_meta_api.h"
#include "system/server/rx_server.h"
#include "model/rx_meta_internals.h"


namespace sys_internal {

namespace rx_protocol {

namespace messages {

namespace query_messages {

// Class sys_internal::rx_protocol::messages::query_messages::browse_request_message 

string_type browse_request_message::type_name = "brwReq";

rx_message_type_t browse_request_message::type_id = rx_browse_request_id;


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
	auto request_id = this->request_id;
	auto result = api::ns::rx_list_directory(path, filter,
		[request_id, port](rx_result_with<api::ns::directory_browse_result>&& result) mutable
		{
			if (result)
			{
				auto response = std::make_unique<browse_response_message>();
				for (const auto one : result.value().directories)
				{
					response->items.emplace_back(one->get_type_id(), one->meta_info());
				}
				for (const auto one : result.value().items)
				{
					response->items.emplace_back(one->get_type_id(), one->meta_info());
				}
				response->request_id = request_id;
				port->data_processed(std::move(response));

			}
			else
			{
				auto ret_value = std::make_unique<error_message>(result, 14, request_id);
				port->data_processed(std::move(ret_value));
			}

		}, ctx);

	if (!result)
	{
		auto ret_value = std::make_unique<error_message>(result, 13, request_id);
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

rx_message_type_t browse_request_message::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::query_messages::browse_response_message 

string_type browse_response_message::type_name = "brwResp";

rx_message_type_t browse_response_message::type_id = rx_browse_response_id;


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
	while (!stream.array_end())
	{
		rx_item_type type;
		meta::meta_data one;
		auto result = one.deserialize_meta_data(stream, 0, type);
		if (!result)
		{
			result.register_error("Error serializing meta item "s + one.get_name());
			return result;
		}
		items.emplace_back(type, std::move(one));
	}

	return true;
}

const string_type& browse_response_message::get_type_name ()
{
  return type_name;

}

rx_message_type_t browse_response_message::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::query_messages::get_type_request 

string_type get_type_request::type_name = "getTypeReq";

rx_message_type_t get_type_request::type_id = rx_get_type_request_id;


rx_result get_type_request::serialize (base_meta_writer& stream) const
{
	if (stream.is_string_based())
	{
		if (!stream.write_string("type", rx_item_type_name(item_type)))
			return false;
	}
	else
	{
		if (!stream.write_byte("type", item_type))
			return false;
	}
	auto result = reference.serialize_reference("target", stream);
	if (!result)
		return result;

	return result;
}

rx_result get_type_request::deserialize (base_meta_reader& stream)
{
	if (stream.is_string_based())
	{
		string_type temp;
		if (!stream.read_string("type", temp))
			return false;
		item_type = rx_parse_type_name(temp);
		if (item_type >= rx_item_type::rx_first_invalid)
			return temp + " is invalid type name";
	}
	else
	{
		uint8_t temp;
		if (!stream.read_byte("type", temp))
			return false;
		if (temp >= rx_item_type::rx_first_invalid)
			return "Invalid type";
		item_type = (rx_item_type)temp;
	}
	auto result = reference.deserialize_reference("target", stream);
	if (!result)
		return result;

	return result;
}

message_ptr get_type_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	switch (item_type)
	{
	case rx_item_type::rx_object_type:
		return do_job(ctx, port, tl::type2type<object_types::object_type>());
	case rx_item_type::rx_domain_type:
		return do_job(ctx, port, tl::type2type<object_types::domain_type>());
	case rx_item_type::rx_port_type:
		return do_job(ctx, port, tl::type2type<object_types::port_type>());
	case rx_item_type::rx_application_type:
		return do_job(ctx, port, tl::type2type<object_types::application_type>());

	case rx_item_type::rx_struct_type:
		return do_simple_job(ctx, port, tl::type2type<basic_types::struct_type>());
	case rx_item_type::rx_variable_type:
		return do_simple_job(ctx, port, tl::type2type<basic_types::variable_type>());
	case rx_item_type::rx_source_type:
		return do_simple_job(ctx, port, tl::type2type<basic_types::source_type>());
	case rx_item_type::rx_filter_type:
		return do_simple_job(ctx, port, tl::type2type<basic_types::filter_type>());
	case rx_item_type::rx_event_type:
		return do_simple_job(ctx, port, tl::type2type<basic_types::event_type>());
	case rx_item_type::rx_mapper_type:
		return do_simple_job(ctx, port, tl::type2type<basic_types::mapper_type>());
	default:
		{
			auto ret_value = std::make_unique<error_message>(rx_item_type_name(item_type) + " is unknown type", 15, request_id);
			return ret_value;
		}
	}
}

const string_type& get_type_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t get_type_request::get_type_id ()
{
  return type_id;

}

template<typename T>
message_ptr get_type_request::do_job(api::rx_context ctx, rx_protocol_port_ptr port, tl::type2type<T>)
{
	auto request_id = this->request_id;
	rx_node_id id = rx_node_id::null_id;

	auto callback = [request_id, port](rx_result_with<typename T::smart_ptr>&& result) mutable
	{
		if (result)
		{
			auto response = std::make_unique<get_type_response<T> >();
			response->item = result.value();
			response->request_id = request_id;
			port->data_processed(std::move(response));

		}
		else
		{
			auto ret_value = std::make_unique<error_message>(result, 14, request_id);
			port->data_processed(std::move(ret_value));
		}

	};

	rx_result result = api::meta::rx_get_type<T>(reference, callback, ctx);

	if (!result)
	{
		return std::make_unique<error_message>(result, 13, request_id);
	}
	else
	{
		// just return we send callback
		return message_ptr();
	}
}
template<typename T>
message_ptr get_type_request::do_simple_job(api::rx_context ctx, rx_protocol_port_ptr port, tl::type2type<T>)
{
	auto request_id = this->request_id;
	rx_node_id id = rx_node_id::null_id;

	auto callback = [request_id, port](rx_result_with<typename T::smart_ptr>&& result) mutable
	{
		if (result)
		{
			auto response = std::make_unique<get_type_response<T> >();
			response->item = result.value();
			response->request_id = request_id;
			port->data_processed(std::move(response));

		}
		else
		{
			auto ret_value = std::make_unique<error_message>(result, 14, request_id);
			port->data_processed(std::move(ret_value));
		}

	};

	rx_result result = api::meta::rx_get_simple_type<T>(reference, callback, ctx);

	if (!result)
	{
		auto ret_value = std::make_unique<error_message>(result, 13, request_id);
		return ret_value;
	}
	else
	{
		// just return we send callback
		return message_ptr();
	}
}
// Parameterized Class sys_internal::rx_protocol::messages::query_messages::get_type_response 

template <class itemT>
string_type get_type_response<itemT>::type_name = "getTypeResp";

template <class itemT>
uint16_t get_type_response<itemT>::type_id = rx_get_type_response_id;


template <class itemT>
const string_type& get_type_response<itemT>::get_type_name ()
{
  return type_name;

}

template <class itemT>
rx_message_type_t get_type_response<itemT>::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::query_messages::query_request_message 

string_type query_request_message::type_name = "queryReq";

rx_message_type_t query_request_message::type_id = rx_query_request_id;


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
	auto request_id = this->request_id;
	rx_node_id id = rx_node_id::null_id;

	auto callback = [request_id, port](rx_result_with<api::query_result>&& result) mutable
	{
		if (result)
		{
			auto response = std::make_unique<query_response_message>();
			
			for (const auto one : result.value().items)
			{
				response->items.emplace_back(one.type, one.data);
			}
			response->request_id = request_id;
			port->data_processed(std::move(response));
		}
		else
		{
			auto ret_value = std::make_unique<error_message>(result, 14, request_id);
			port->data_processed(std::move(ret_value));
		}

	};

	rx_result result = api::ns::rx_query_model(queries, callback, ctx);

	if (!result)
	{
		auto ret_value = std::make_unique<error_message>(result, 14, request_id);
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

rx_message_type_t query_request_message::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::query_messages::query_response_message 

string_type query_response_message::type_name = "queryResp";

rx_message_type_t query_response_message::type_id = rx_query_response_id;


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
		rx_item_type type;
		meta::meta_data one;
		auto result = one.deserialize_meta_data(stream, 0, type);
		if (!result)
		{
			result.register_error("Error serializing meta item "s + one.get_name());
			return result;
		}
		items.emplace_back(type, std::move(one));
	}

	return true;
}

const string_type& query_response_message::get_type_name ()
{
  return type_name;

}

rx_message_type_t query_response_message::get_type_id ()
{
  return type_id;

}


// Parameterized Class sys_internal::rx_protocol::messages::query_messages::type_response_message 


template <class itemT>
rx_result type_response_message<itemT>::serialize (base_meta_writer& stream) const
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	auto result = item->serialize_definition(stream, STREAMING_TYPE_TYPE);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

template <class itemT>
rx_result type_response_message<itemT>::deserialize (base_meta_reader& stream)
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	auto result = item->deserialize_definition(stream, STREAMING_TYPE_TYPE);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return result;
}


// Class sys_internal::rx_protocol::messages::query_messages::get_runtime_request 

string_type get_runtime_request::type_name = "getRuntimeReq";

rx_message_type_t get_runtime_request::type_id = rx_get_runtime_request_id;


rx_result get_runtime_request::serialize (base_meta_writer& stream) const
{
	if (stream.is_string_based())
	{
		if (!stream.write_string("type", rx_item_type_name(item_type)))
			return false;
	}
	else
	{
		if (!stream.write_byte("type", item_type))
			return false;
	}
	auto result = reference.serialize_reference("target", stream);
	if (!result)
		return result;

	return result;
}

rx_result get_runtime_request::deserialize (base_meta_reader& stream)
{
	if (stream.is_string_based())
	{
		string_type temp;
		if (!stream.read_string("type", temp))
			return false;
		item_type = rx_parse_type_name(temp);
		if (item_type >= rx_item_type::rx_first_invalid)
			return temp + " is invalid type name";
	}
	else
	{
		uint8_t temp;
		if (!stream.read_byte("type", temp))
			return false;
		if (temp >= rx_item_type::rx_first_invalid)
			return "Invalid type";
		item_type = (rx_item_type)temp;
	}
	auto result = reference.deserialize_reference("target", stream);
	if (!result)
		return result;

	return result;
}

message_ptr get_runtime_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	switch (item_type)
	{
	case rx_item_type::rx_object:
		return do_job(ctx, port, tl::type2type<object_types::object_type>());
	case rx_item_type::rx_domain:
		return do_job(ctx, port, tl::type2type<object_types::domain_type>());
	case rx_item_type::rx_port:
		return do_job(ctx, port, tl::type2type<object_types::port_type>());
	case rx_item_type::rx_application:
		return do_job(ctx, port, tl::type2type<object_types::application_type>());
	default:
		{
			auto ret_value = std::make_unique<error_message>(rx_item_type_name(item_type) + " is unknown type", 15, request_id);
			return ret_value;
		}
	}
}

const string_type& get_runtime_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t get_runtime_request::get_type_id ()
{
  return type_id;

}


template<typename T>
message_ptr get_runtime_request::do_job(api::rx_context ctx, rx_protocol_port_ptr port, tl::type2type<T>)
{
	auto request_id = this->request_id;
	rx_node_id id = rx_node_id::null_id;

	auto callback = [request_id, port](rx_result_with<typename T::RTypePtr>&& result) mutable
	{
		if (result)
		{
			auto response = std::make_unique<get_runtime_response<T> >();
			response->item = result.value();
			response->request_id = request_id;
			port->data_processed(std::move(response));

		}
		else
		{
			auto ret_value = std::make_unique<error_message>(result, 14, request_id);
			port->data_processed(std::move(ret_value));
		}

	};

	rx_result result = api::meta::rx_get_runtime<T>(reference, callback, ctx);

	if (!result)
	{
		auto ret_value = std::make_unique<error_message>(result, 13, request_id);
		return ret_value;
	}
	else
	{
		// just return we send callback
		return message_ptr();
	}
}
// Parameterized Class sys_internal::rx_protocol::messages::query_messages::runtime_response_message 


template <class itemT>
rx_result runtime_response_message<itemT>::serialize (base_meta_writer& stream) const
{
	if (!stream.start_object("item"))
		return "Error starting item object";
	
	auto result = item->serialize(stream, STREAMING_TYPE_OBJECT);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

template <class itemT>
rx_result runtime_response_message<itemT>::deserialize (base_meta_reader& stream)
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	auto result = item->deserialize(stream, STREAMING_TYPE_OBJECT);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return result;
}


// Parameterized Class sys_internal::rx_protocol::messages::query_messages::get_runtime_response 

template <class itemT>
string_type get_runtime_response<itemT>::type_name = "getRuntimeResp";

template <class itemT>
rx_message_type_t get_runtime_response<itemT>::type_id = rx_get_runtime_response_id;


template <class itemT>
const string_type& get_runtime_response<itemT>::get_type_name ()
{
  return type_name;

}

template <class itemT>
rx_message_type_t get_runtime_response<itemT>::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::query_messages::browse_runtime_request 

string_type browse_runtime_request::type_name = "brwRuntimeReq";

rx_message_type_t browse_runtime_request::type_id = rx_browse_runtime_request_id;


rx_result browse_runtime_request::serialize (base_meta_writer& stream) const
{
	if (stream.is_string_based())
	{
		if (!stream.write_string("type", rx_item_type_name(item_type)))
			return "Error serializing type string";
	}
	else
	{
		if (!stream.write_byte("type", item_type))
			return "Error serializing type binary";
	}
	if (!stream.write_id("id", id))
		return "Error serializing id";
	if (!stream.write_string("path", path))
		return "Error serializing path";
	if (!stream.write_string("filter", filter))
		return "Error serializing filter";
	return true;
}

rx_result browse_runtime_request::deserialize (base_meta_reader& stream)
{
	if (stream.is_string_based())
	{
		string_type temp;
		if (!stream.read_string("type", temp))
			return false;
		item_type = rx_parse_type_name(temp);
		if (item_type >= rx_item_type::rx_first_invalid)
			return temp + " is invalid type name";
	}
	else
	{
		uint8_t temp;
		if (!stream.read_byte("type", temp))
			return false;
		if (temp >= rx_item_type::rx_first_invalid)
			return "Invalid type";
		item_type = (rx_item_type)temp;
	}
	if (!stream.read_id("id", id))
		return "Error reading id";
	if (!stream.read_string("path", path))
		return "Error reading path";
	if (!stream.read_string("filter", filter))
		return "Error reading filter";
	return true;
}

message_ptr browse_runtime_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	switch (item_type)
	{
	case rx_item_type::rx_application:
		return do_concrete_job(ctx, port, tl::type2type<meta::object_types::application_type>());
	case rx_item_type::rx_domain:
		return do_concrete_job(ctx, port, tl::type2type<meta::object_types::domain_type>());
	case rx_item_type::rx_port:
		return do_concrete_job(ctx, port, tl::type2type<meta::object_types::port_type>());
	case rx_item_type::rx_object:
		return do_concrete_job(ctx, port, tl::type2type<meta::object_types::object_type>());
	default:
		{
			auto ret_value = std::make_unique<error_message>("Browse not valid for this type of item!"s, 14, request_id);
			return ret_value;
		}
	}
}

const string_type& browse_runtime_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t browse_runtime_request::get_type_id ()
{
  return type_id;

}

template<typename typeT>
message_ptr browse_runtime_request::do_concrete_job(api::rx_context ctx, rx_protocol_port_ptr port, tl::type2type<typeT>)
{
	auto request_id = this->request_id;
	auto result = api::ns::rx_list_runtime(id, path, filter,
		[request_id, port](rx_result_with<api::runtime_browse_result>&& result) mutable
		{
			if (result)
			{
				auto response = std::make_unique<browse_runtime_response_message>();
				response->items = std::move(result.value().items);
				response->request_id = request_id;
				port->data_processed(std::move(response));

			}
			else
			{
				auto ret_value = std::make_unique<error_message>(result, 14, request_id);
				port->data_processed(std::move(ret_value));
			}

		}, ctx, tl::type2type<typeT>());

	if (!result)
	{
		auto ret_value = std::make_unique<error_message>(result, 13, request_id);
		return ret_value;
	}
	else
	{
		// just return we send callback
		return message_ptr();
	}
}
// Class sys_internal::rx_protocol::messages::query_messages::browse_runtime_response_message 

string_type browse_runtime_response_message::type_name = "brwRuntimeResp";

rx_message_type_t browse_runtime_response_message::type_id = rx_browse_runtime_response_id;


rx_result browse_runtime_response_message::serialize (base_meta_writer& stream) const
{
	if (!stream.start_array("items", items.size()))
		return "Error serializing array of items";
	for (const auto& one : items)
	{
		if (!stream.start_object("item"))
			return "Error starting item object";
		if (!stream.write_byte("type", (uint8_t)one.type))
			return false;
		if (!stream.write_string("name", one.name))
			return "Error reading item name";
		if (!stream.write_string("path", one.full_path))
			return "Error reading item path";
		if (!stream.end_object())
			return "Error ending item object";
	}
	if (!stream.end_array())
		return "Error serializing array of items at the end";

	return true;
}

rx_result browse_runtime_response_message::deserialize (base_meta_reader& stream)
{
	if (!stream.start_array("items"))
		return "Error reading array of items";
	items.clear();
	while (!stream.array_end())
	{
		runtime_item_attribute one;

		if (!stream.start_object("item"))
			return "Error starting item object";
		uint8_t temp;
		if (!stream.read_byte("type", temp))
			return false;
		if (temp >= rx_item_type::rx_first_invalid)
			return "Invalid type";
		one.type = (rx_attribute_type)temp;
		if(!stream.read_string("name", one.name))
			return "Error reading item name";
		if (!stream.read_string("path", one.full_path))
			return "Error reading item path";

		if (!stream.end_object())
			return "Error ending item object";

		items.emplace_back(std::move(one));
	}

	return true;
}

const string_type& browse_runtime_response_message::get_type_name ()
{
  return type_name;

}

rx_message_type_t browse_runtime_response_message::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::query_messages::get_code_info_request 

string_type get_code_info_request::type_name = "codeReq";

rx_message_type_t get_code_info_request::type_id = rx_code_request_id;


rx_result get_code_info_request::serialize (base_meta_writer& stream) const
{
	if (stream.is_string_based())
	{
		if (!stream.write_string("type", rx_item_type_name(item_type)))
			return false;
	}
	else
	{
		if (!stream.write_byte("type", item_type))
			return false;
	}
	auto result = reference.serialize_reference("target", stream);
	if (!result)
		return result;

	return result;
}

rx_result get_code_info_request::deserialize (base_meta_reader& stream)
{
	if (stream.is_string_based())
	{
		string_type temp;
		if (!stream.read_string("type", temp))
			return false;
		item_type = rx_parse_type_name(temp);
		if (item_type >= rx_item_type::rx_first_invalid)
			return temp + " is invalid type name";
	}
	else
	{
		uint8_t temp;
		if (!stream.read_byte("type", temp))
			return false;
		if (temp >= rx_item_type::rx_first_invalid)
			return "Invalid type";
		item_type = (rx_item_type)temp;
	}
	auto result = reference.deserialize_reference("target", stream);
	if (!result)
		return result;

	return result;
}

message_ptr get_code_info_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	rx_directory_resolver directories{};
	meta_data data;
	rx_result_with<rx_node_id> result;
	switch (item_type)
	{
	case rx_item_type::rx_object:
		result = api::ns::rx_resolve_runtime_reference(reference, directories, tl::type2type<object_types::object_type>());
		break;
	case rx_item_type::rx_domain:
		result = api::ns::rx_resolve_runtime_reference(reference, directories, tl::type2type<object_types::domain_type>());
		break;
	case rx_item_type::rx_port:
		result = api::ns::rx_resolve_runtime_reference(reference, directories, tl::type2type<object_types::port_type>());
		break;
	case rx_item_type::rx_application:
		result = api::ns::rx_resolve_runtime_reference(reference, directories, tl::type2type<object_types::application_type>());
		break;
	default:
		{
			auto ret_value = std::make_unique<error_message>(rx_item_type_name(item_type) + " is unknown type", 15, request_id);
			return ret_value;
		}
	}
	if(!result)
	{
		auto ret_value = std::make_unique<error_message>(result, 15, request_id);
		return ret_value;
	}
	auto data_result = model::platform_types_manager::instance().get_types_resolver().get_item_data(result.value(), data);
	if (data_result == rx_item_type::rx_invalid_type)
	{
		auto ret_value = std::make_unique<error_message>(rx_item_type_name(item_type) + " can't be found in types resolver!", 15, request_id);
		return ret_value;
	}
	auto item_ptr = rx_gate::instance().get_root_directory()->get_sub_item(data.get_full_path());
	if (!item_ptr)
	{
		auto ret_value = std::make_unique<error_message>(rx_item_type_name(item_type) + " can't be found in root directory!", 15, request_id);
		return ret_value;
	}
	string_type info;
	item_ptr->code_info_to_string(info);

	auto ret_msg = std::make_unique<get_code_info_response_message>();
	ret_msg->request_id = request_id;
	ret_msg->code_info = info;

	return ret_msg;
}

const string_type& get_code_info_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t get_code_info_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::query_messages::get_code_info_response_message 

string_type get_code_info_response_message::type_name = "codeResp";

rx_message_type_t get_code_info_response_message::type_id = rx_code_response_id;


rx_result get_code_info_response_message::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("code", code_info))
		return "Error writing code info";

	return true;
}

rx_result get_code_info_response_message::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("code", code_info))
		return "Error reading code info";
	
	return true;
}

const string_type& get_code_info_response_message::get_type_name ()
{
  return type_name;

}

rx_message_type_t get_code_info_response_message::get_type_id ()
{
  return type_id;

}


} // namespace query_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal

