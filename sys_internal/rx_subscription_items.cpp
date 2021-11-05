

/****************************************************************************
*
*  sys_internal\rx_subscription_items.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_subscription_items
#include "sys_internal/rx_subscription_items.h"

#include "sys_internal/rx_internal_protocol.h"
#include "system/serialization/rx_ser.h"


namespace rx_internal {

namespace rx_protocol {

namespace messages {

namespace items_messages {

// Class rx_internal::rx_protocol::messages::items_messages::add_items_request 

string_type add_items_request::type_name = "addItemsReq";

rx_message_type_t add_items_request::type_id = rx_add_items_request_id;


rx_result add_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.start_array("items", items.size()))
		return stream.get_error();
	for (const auto& one : items)
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		auto one_result = one.serialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to serialize item");
			return one_result;
		}
		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();
	return true;
}

rx_result add_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.start_array("items"))
		return stream.get_error();
	while (!stream.array_end())
	{
		add_item_data one;
		if (!stream.start_object("item"))
			return stream.get_error();
		auto one_result = one.deserialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to deserialize item");
			return one_result;
		}
		if (!stream.end_object())
			return stream.get_error();
		items.emplace_back(one);
	}
	return true;
}

message_ptr add_items_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
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
	auto result = conn->add_items(subscription_id, subs_items, results);
	if (result)
	{
		auto response = std::make_unique<add_items_response>();
		response->request_id = request_id;
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


// Class rx_internal::rx_protocol::messages::items_messages::remove_items_request 

string_type remove_items_request::type_name = "removeItemsReq";

rx_message_type_t remove_items_request::type_id = rx_remove_items_request_id;


rx_result remove_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	return true;
}

rx_result remove_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.start_array("items"))
		return stream.get_error();
	while (!stream.array_end())
	{
		runtime_handle_t item;
		if (!stream.read_uint("handle",item))
			return stream.get_error();
		items.emplace_back(item);
	}
	return true;
}

message_ptr remove_items_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	std::vector<rx_result> results;
	auto result = conn->remove_items(subscription_id, std::move(items), results);
	if (result)
	{
		auto response = std::make_unique<remove_items_response>();
		response->subscription_id = subscription_id;
		response->request_id = request_id;
		for (auto& one : results)
			response->results.emplace_back(std::move(one));
		return response;
	}
	return std::make_unique<error_message>(result, 9, request_id);
}

const string_type& remove_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t remove_items_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::items_messages::execute_item_request 

string_type execute_item_request::type_name = "execItemReq";

rx_message_type_t execute_item_request::type_id = rx_execute_item_request_id;


rx_result execute_item_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	return true;
}

rx_result execute_item_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	return true;
}

message_ptr execute_item_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	return std::make_unique<error_message>(RX_NOT_IMPLEMENTED, 9, request_id);
}

const string_type& execute_item_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t execute_item_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::items_messages::add_items_response 

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
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.start_array("results", results.size()))
		return stream.get_error();
	for (const auto& one : results)
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		auto one_result = one.serialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to serialize item");
			return one_result;
		}
		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();
	return true;
}

rx_result add_items_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.start_array("results"))
		return stream.get_error();
	while (!stream.array_end())
	{
		add_item_result_data one;
		if (!stream.start_object("item"))
			return stream.get_error();
		auto one_result = one.deserialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to deserialize item");
			return one_result;
		}
		if (!stream.end_object())
			return stream.get_error();
		results.emplace_back(one);
	}
	return true;
}


// Class rx_internal::rx_protocol::messages::items_messages::remove_items_response 

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


// Class rx_internal::rx_protocol::messages::items_messages::write_items_request 

string_type write_items_request::type_name = "writeItemsReq";

rx_message_type_t write_items_request::type_id = rx_write_items_request_id;


rx_result write_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.write_uint("trans_id", transaction_id))
		return stream.get_error();
	if (!stream.start_array("values", values.size()))
		return stream.get_error();
	for (const auto& one : values)
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		if (!stream.write_uint("handle", one.first))
			return stream.get_error();
		if (!stream.write_value("handle", one.second))
			return stream.get_error();
		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();
	return true;
}

rx_result write_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.read_uint("trans_id", transaction_id))
		return stream.get_error();
	if (!stream.start_array("values"))
		return stream.get_error();
	while (!stream.array_end())
	{
		std::pair<runtime_handle_t, rx_simple_value> one;
		if (!stream.start_object("item"))
			return stream.get_error();
		if (!stream.read_uint("handle", one.first))
			return stream.get_error();
		if (!one.second.deserialize("value", stream))
			return stream.get_error();
		if (!stream.end_object())
			return stream.get_error();
		values.emplace_back(one);
	}
	return true;
}

const string_type& write_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t write_items_request::get_type_id ()
{
  return type_id;

}

message_ptr write_items_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	if (values.empty())
	{
		return std::make_unique<error_message>("Nothing to do!"s, 19, request_id);
	}
	std::vector<rx_result> results;
	results.reserve(values.size());
	auto result = conn->write_items(subscription_id, transaction_id, std::move(values), results);
	if (result)
	{
		auto response = std::make_unique<write_items_response>();
		response->request_id = request_id;
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


// Class rx_internal::rx_protocol::messages::items_messages::write_items_response 

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


// Class rx_internal::rx_protocol::messages::items_messages::execute_item_response 

string_type execute_item_response::type_name = "execItemResp";

rx_message_type_t execute_item_response::type_id = rx_execute_item_response_id;


rx_result execute_item_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	return true;
}

rx_result execute_item_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
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


// Class rx_internal::rx_protocol::messages::items_messages::modify_items_response 

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


// Class rx_internal::rx_protocol::messages::items_messages::modify_items_request 

string_type modify_items_request::type_name = "modifyItemsReq";

rx_message_type_t modify_items_request::type_id = rx_modify_items_request_id;


rx_result modify_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	return true;
}

rx_result modify_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	return true;
}

message_ptr modify_items_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	return std::make_unique<error_message>(RX_NOT_IMPLEMENTED, 9, request_id);
}

const string_type& modify_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t modify_items_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::items_messages::read_items_request 

string_type read_items_request::type_name = "readItemsReq";

rx_message_type_t read_items_request::type_id = rx_read_items_request_id;


rx_result read_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	return true;
}

rx_result read_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	return true;
}

message_ptr read_items_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	return std::make_unique<error_message>(RX_NOT_IMPLEMENTED, 9, request_id);
}

const string_type& read_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t read_items_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::items_messages::read_items_response 

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


// Class rx_internal::rx_protocol::messages::items_messages::subscription_items_response 


rx_result subscription_items_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.start_array("results", results.size()))
		return stream.get_error();
	for (const auto& one : results)
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		auto one_result = one.serialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to serialize item");
			return one_result;
		}
		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();
	return true;
}

rx_result subscription_items_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.start_array("results"))
		return stream.get_error();
	while (!stream.array_end())
	{
		item_result_data one;
		if (!stream.start_object("item"))
			return stream.get_error();
		auto one_result = one.deserialize(stream);
		if (!one_result)
		{
			one_result.register_error("Unable to deserialize item");
			return one_result;
		}
		if (!stream.end_object())
			return stream.get_error();
		results.emplace_back(one);
	}
	return true;
}


// Class rx_internal::rx_protocol::messages::items_messages::add_item_data 


rx_result add_item_data::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uint("client", client_handle))
		return stream.get_error();
	if (!stream.write_string("path", path.c_str()))
		return stream.get_error();
	if (!stream.write_bool("active", active))
		return stream.get_error();
	if (!stream.write_byte("trigger", (uint8_t)trigger_type))
		return stream.get_error();
	return true;
}

rx_result add_item_data::deserialize (base_meta_reader& stream)
{
	if (!stream.read_uint("client", client_handle))
		return stream.get_error();
	if (!stream.read_string("path", path))
		return stream.get_error();
	if (!stream.read_bool("active", active))
		return stream.get_error();
	uint8_t temp;
	if (!stream.read_byte("trigger", temp))
		return stream.get_error();
	if(temp>(uint8_t)max_trigger_type)
		return stream.get_error() + "Trigger type out of bounds";
	trigger_type = (subscription_trigger_type)temp;
	return true;
}


// Class rx_internal::rx_protocol::messages::items_messages::modify_item_data 


// Class rx_internal::rx_protocol::messages::items_messages::add_item_result_data 

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
		return stream.get_error();
	if (!stream.write_uint("errCode", error_code))
		return stream.get_error();
	if (!stream.write_string("errMsg", error_text.c_str()))
		return stream.get_error();
	return true;
}

rx_result add_item_result_data::deserialize (base_meta_reader& stream)
{
	if (!stream.read_uint("handle", handle))
		return stream.get_error();
	if (!stream.read_uint("errCode", error_code))
		return stream.get_error();
	if (!stream.read_string("errMsg", error_text))
		return stream.get_error();
	return true;
}


// Class rx_internal::rx_protocol::messages::items_messages::item_result_data 

item_result_data::item_result_data (rx_result&& result)
{
	if (result)
	{
		error_code = 0;
	}
	else
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
}



rx_result item_result_data::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uint("errCode", error_code))
		return stream.get_error();
	if (!stream.write_string("errMsg", error_text.c_str()))
		return stream.get_error();
	return true;
}

rx_result item_result_data::deserialize (base_meta_reader& stream)
{
	if (!stream.read_uint("errCode", error_code))
		return stream.get_error();
	if (!stream.read_string("errMsg", error_text))
		return stream.get_error();
	return true;
}


} // namespace items_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal

