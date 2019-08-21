

/****************************************************************************
*
*  sys_internal\rx_subscription_items.cpp
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


// rx_subscription_items
#include "sys_internal/rx_subscription_items.h"

#include "sys_internal/rx_internal_protocol.h"
#include "system/serialization/rx_ser.h"


namespace sys_internal {

namespace rx_protocol {

namespace messages {

namespace items_messages {

// Class sys_internal::rx_protocol::messages::items_messages::add_items_request 

string_type add_items_request::type_name = "addItemsReq";

rx_message_type_t add_items_request::type_id = rx_add_items_request_id;


rx_result add_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	if (!stream.start_array("items", items.size()))
		return "Unable to start items array";
	for (const auto& one : items)
	{
		if (!stream.start_object("item"))
			return "Unable to start item";
		auto one_result = one.serialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to serialize item");
			return one_result;
		}
		if (!stream.end_object())
			return "Unable to end item";
	}
	if (!stream.end_array())
		return "Unable to end items array";
	return true;
}

rx_result add_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	if (!stream.start_array("items"))
		return "Unable to start items array";
	while (!stream.array_end())
	{
		add_item_data one;
		if (!stream.start_object("item"))
			return "Unable to start item";
		auto one_result = one.deserialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to deserialize item");
			return one_result;
		}
		if (!stream.end_object())
			return "Unable to end item";
		items.emplace_back(one);
	}
	return true;
}

message_ptr add_items_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	std::vector<subscription_item_data> subs_items;
	subs_items.reserve(items.size());
	for (const auto& one : items)
	{
		subscription_item_data temp;
		temp.local_handle = 0;
		temp.client_handle = one.client_handle;
		temp.path = one.path;
		temp.active = one.active;
		temp.trigger_type = one.trigger_type;
		subs_items.emplace_back(std::move(temp));
	}
	std::vector<rx_result_with<runtime_handle_t> > results;
	auto result = port->add_items(subscription_id, subs_items, results);
	if (result)
	{
		auto response = std::make_unique<add_items_response>();
		response->subscription_id = subscription_id;
		response->results.reserve(results.size());
		for (auto& one : results)
		{
			response->results.emplace_back(std::move(one));
		}
		return response;
	}
	else
	{
		auto ret_value = std::make_unique<error_message>(result, 13, request_id);
		return ret_value;
	}
}

const string_type& add_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t add_items_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::remove_items_request 

string_type remove_items_request::type_name = "removeItemsReq";

rx_message_type_t remove_items_request::type_id = rx_remove_items_request_id;


rx_result remove_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result remove_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

message_ptr remove_items_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return std::make_unique<error_message>("Not implemented!!!"s, 9, request_id);
}

const string_type& remove_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t remove_items_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::execute_item_request 

string_type execute_item_request::type_name = "execItemReq";

rx_message_type_t execute_item_request::type_id = rx_execute_item_request_id;


rx_result execute_item_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result execute_item_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

message_ptr execute_item_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return std::make_unique<error_message>("Not implemented!!!"s, 9, request_id);
}

const string_type& execute_item_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t execute_item_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::add_items_response 

string_type add_items_response::type_name = "addItemsResp";

rx_message_type_t add_items_response::type_id = rx_add_items_response_id;


const string_type& add_items_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t add_items_response::get_type_id ()
{
  return type_id;

}

rx_result add_items_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	if (!stream.start_array("results", results.size()))
		return "Unable to start results array";
	for (const auto& one : results)
	{
		if (!stream.start_object("item"))
			return "Unable to start item";
		auto one_result = one.serialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to serialize item");
			return one_result;
		}
		if (!stream.end_object())
			return "Unable to end item";
	}
	if (!stream.end_array())
		return "Unable to end results array";
	return true;
}

rx_result add_items_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	if (!stream.start_array("results"))
		return "Unable to start results array";
	while (!stream.array_end())
	{
		add_item_result_data one;
		if (!stream.start_object("item"))
			return "Unable to start item";
		auto one_result = one.deserialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to deserialize item");
			return one_result;
		}
		if (!stream.end_object())
			return "Unable to end item";
		results.emplace_back(one);
	}
	return true;
}


// Class sys_internal::rx_protocol::messages::items_messages::remove_items_response 

string_type remove_items_response::type_name = "removeItemsResp";

rx_message_type_t remove_items_response::type_id = rx_remove_items_response_id;


const string_type& remove_items_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t remove_items_response::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::write_items_request 

string_type write_items_request::type_name = "writeItemsReq";

rx_message_type_t write_items_request::type_id = rx_write_items_request_id;


rx_result write_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result write_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

message_ptr write_items_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return std::make_unique<error_message>("Not implemented!!!"s, 9, request_id);
}

const string_type& write_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t write_items_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::write_items_response 

string_type write_items_response::type_name = "writeItemsResp";

rx_message_type_t write_items_response::type_id = rx_write_items_response_id;


const string_type& write_items_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t write_items_response::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::execute_item_response 

string_type execute_item_response::type_name = "execItemResp";

rx_message_type_t execute_item_response::type_id = rx_execute_item_response_id;


rx_result execute_item_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result execute_item_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

const string_type& execute_item_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t execute_item_response::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::modify_items_response 

string_type modify_items_response::type_name = "modifyItemsResp";

rx_message_type_t modify_items_response::type_id = rx_modify_items_response_id;


const string_type& modify_items_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t modify_items_response::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::modify_items_request 

string_type modify_items_request::type_name = "modifyItemsReq";

rx_message_type_t modify_items_request::type_id = rx_modify_items_request_id;


rx_result modify_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result modify_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

message_ptr modify_items_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return std::make_unique<error_message>("Not implemented!!!"s, 9, request_id);
}

const string_type& modify_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t modify_items_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::read_items_request 

string_type read_items_request::type_name = "readItemsReq";

rx_message_type_t read_items_request::type_id = rx_read_items_request_id;


rx_result read_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result read_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

message_ptr read_items_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return std::make_unique<error_message>("Not implemented!!!"s, 9, request_id);
}

const string_type& read_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t read_items_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::read_items_response 

string_type read_items_response::type_name = "readItemsResp";

rx_message_type_t read_items_response::type_id = rx_read_items_response_id;


const string_type& read_items_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t read_items_response::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::subscription_items_response 


rx_result subscription_items_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	if (!stream.start_array("results", results.size()))
		return "Unable to start results array";
	for (const auto& one : results)
	{
		if (!stream.start_object("item"))
			return "Unable to start item";
		auto one_result = one.serialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to serialize item");
			return one_result;
		}
		if (!stream.end_object())
			return "Unable to end item";
	}
	if (!stream.end_array())
		return "Unable to end results array";
	return true;
}

rx_result subscription_items_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	if (!stream.start_array("results"))
		return "Unable to start results array";
	while (!stream.array_end())
	{
		item_result_data one;
		if (!stream.start_object("item"))
			return "Unable to start item";
		auto one_result = one.deserialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to deserialize item");
			return one_result;
		}
		if (!stream.end_object())
			return "Unable to end item";
		results.emplace_back(one);
	}
	return true;
}


// Class sys_internal::rx_protocol::messages::items_messages::add_item_data 


rx_result add_item_data::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uint("client", client_handle))
		return "Unable to write client_handle";
	if (!stream.write_string("path", path))
		return "Unable to write path";
	if (!stream.write_bool("active", active))
		return "Unable to write active";
	if (!stream.write_byte("trigger", (uint8_t)trigger_type))
		return "Unable to write trigger type";
	return true;
}

rx_result add_item_data::deserialize (base_meta_reader& stream)
{
	if (!stream.read_uint("client", client_handle))
		return "Unable to read client_handle";
	if (!stream.read_string("path", path))
		return "Unable to read path";
	if (!stream.read_bool("active", active))
		return "Unable to read active";
	uint8_t temp;
	if (!stream.read_byte("trigger", temp))
		return "Unable to read active";
	if(temp>(uint8_t)max_trigger_type)
		return "Trigger type out of bounds";
	trigger_type = (subscription_trigger_type)temp;
	return true;
}


// Class sys_internal::rx_protocol::messages::items_messages::modify_item_data 


// Class sys_internal::rx_protocol::messages::items_messages::add_item_result_data 

add_item_result_data::add_item_result_data (rx_result_with<runtime_handle_t>&& result)
{
	if (result)
	{
		handle = result.value();
		error_code = 0;
	}
	else
	{
		handle = 0;
		error_code = 99;
		bool first = true;
		for (const auto& one : result.errors())
		{
			if (first)
				first = false;
			else
				error_text += ", ";
			error_text += one;
		}
	}
}



rx_result add_item_result_data::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uint("handle", handle))
		return "Unable to write handle";
	if (!stream.write_uint("errCode", error_code))
		return "Unable to write error code";
	if (!stream.write_string("errMsg", error_text))
		return "Unable to write error message";
	return true;
}

rx_result add_item_result_data::deserialize (base_meta_reader& stream)
{
	if (!stream.read_uint("handle", handle))
		return "Unable to read handle";
	if (!stream.read_uint("errCode", error_code))
		return "Unable to read error code";
	if (!stream.read_string("errMsg", error_text))
		return "Unable to read error message";
	return true;
}


// Class sys_internal::rx_protocol::messages::items_messages::item_result_data 

item_result_data::item_result_data (rx_result&& result)
{
	error_code = 98;
	bool first = true;
	for (const auto& one : result.errors())
	{
		if (first)
			first = false;
		else
			error_text += ", ";
		error_text += one;
	}
}



rx_result item_result_data::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uint("errCode", error_code))
		return "Unable to write error code";
	if (!stream.write_string("errMsg", error_text))
		return "Unable to write error message";
	return true;
}

rx_result item_result_data::deserialize (base_meta_reader& stream)
{
	if (!stream.read_uint("errCode", error_code))
		return "Unable to read error code";
	if (!stream.read_string("errMsg", error_text))
		return "Unable to read error message";
	return true;
}


} // namespace items_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal

