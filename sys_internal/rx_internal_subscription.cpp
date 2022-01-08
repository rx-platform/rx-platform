

/****************************************************************************
*
*  sys_internal\rx_internal_subscription.cpp
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


// rx_internal_subscription
#include "sys_internal/rx_internal_subscription.h"

#include "rx_internal_protocol.h"


namespace rx_internal {

namespace rx_protocol {

namespace messages {

namespace subscription_messages {

// Class rx_internal::rx_protocol::messages::subscription_messages::create_subscription_request 

string_type create_subscription_request::type_name = "createSubsReq";

rx_message_type_t create_subscription_request::type_id = rx_create_subscription_request_id;


rx_result create_subscription_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.write_uint("rate", publish_rate))
		return stream.get_error();
	if (!stream.write_uint("keepalive", keep_alive_period))
		return stream.get_error();
	if (!stream.write_bool("active", active))
		return stream.get_error();
	if (!stream.write_byte("priority", priority))
		return stream.get_error();
	return true;
}

rx_result create_subscription_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.read_uint("rate", publish_rate))
		return stream.get_error();
	if (!stream.read_uint("keepalive", keep_alive_period))
		return stream.get_error();
	if (!stream.read_bool("active", active))
		return stream.get_error();
	if (!stream.read_byte("priority", priority))
		return stream.get_error();
	return true;
}

message_ptr create_subscription_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	subscription_data data;
	data.active = active;
	data.subscription_id = subscription_id;
	data.keep_alive_period = keep_alive_period;
	data.publish_rate = publish_rate;
	data.priority = priority;
	auto result = conn->connect_subscription(data);
	if (result)
	{
		auto response = std::make_unique<create_subscriptions_response>();
		response->request_id = request_id;
		response->subscription_id = data.subscription_id;
		response->revised_keep_alive_period = data.keep_alive_period;
		response->revised_publish_rate = data.publish_rate;
		return response;
	}
	else
	{
		auto ret_value = std::make_unique<error_message>(result, 13, request_id);
		return ret_value;
	}
}

const string_type& create_subscription_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t create_subscription_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::subscription_messages::create_subscriptions_response 

string_type create_subscriptions_response::type_name = "createSubsResp";

rx_message_type_t create_subscriptions_response::type_id = rx_create_subscription_response_id;


rx_result create_subscriptions_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.write_uint("rate", revised_publish_rate))
		return stream.get_error();
	if (!stream.write_uint("keepalive", revised_keep_alive_period))
		return stream.get_error();
	return true;
}

rx_result create_subscriptions_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.read_uint("rate", revised_publish_rate))
		return stream.get_error();
	if (!stream.read_uint("keepalive", revised_keep_alive_period))
		return stream.get_error();
	return true;
}

const string_type& create_subscriptions_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t create_subscriptions_response::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::subscription_messages::delete_subscription_request 

string_type delete_subscription_request::type_name = "delSubsReq";

rx_message_type_t delete_subscription_request::type_id = rx_delete_subscription_request_id;


rx_result delete_subscription_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	return true;
}

rx_result delete_subscription_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	return true;
}

message_ptr delete_subscription_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	auto result = conn->delete_subscription(subscription_id);
	if (result)
	{
		auto response = std::make_unique<delete_subscription_response>();
		response->subscription_id = subscription_id;
		return response;
	}
	else
	{
		auto ret_value = std::make_unique<error_message>(result, 13, request_id);
		return ret_value;
	}
}

const string_type& delete_subscription_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t delete_subscription_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::subscription_messages::update_subscription_request 

string_type update_subscription_request::type_name = "updSubsReq";

rx_message_type_t update_subscription_request::type_id = rx_update_subscription_request_id;


rx_result update_subscription_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.write_uint("rate", publish_rate))
		return stream.get_error();
	if (!stream.write_uint("keepalive", keep_alive_period))
		return stream.get_error();
	if (!stream.write_bool("active", active))
		return stream.get_error();
	if (!stream.write_byte("priority", priority))
		return stream.get_error();
	return true;
}

rx_result update_subscription_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.read_uint("rate", publish_rate))
		return stream.get_error();
	if (!stream.read_uint("keepalive", keep_alive_period))
		return stream.get_error();
	if (!stream.read_bool("active", active))
		return stream.get_error();
	if (!stream.read_byte("priority", priority))
		return stream.get_error();
	return true;
}

message_ptr update_subscription_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	return std::make_unique<error_message>(RX_NOT_IMPLEMENTED, 9, request_id);
}

const string_type& update_subscription_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t update_subscription_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::subscription_messages::delete_subscription_response 

string_type delete_subscription_response::type_name = "delSubsResp";

rx_message_type_t delete_subscription_response::type_id = rx_delete_subscription_response_id;


rx_result delete_subscription_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	return true;
}

rx_result delete_subscription_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	return true;
}

const string_type& delete_subscription_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t delete_subscription_response::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::subscription_messages::update_subscription_response 

string_type update_subscription_response::type_name = "updSubsResp";

rx_message_type_t update_subscription_response::type_id = rx_update_subscription_response_id;


rx_result update_subscription_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.write_uint("rate", revised_publish_rate))
		return stream.get_error();
	if (!stream.write_uint("keepalive", revised_keep_alive_period))
		return stream.get_error();
	return true;
}

rx_result update_subscription_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.read_uint("rate", revised_publish_rate))
		return stream.get_error();
	if (!stream.read_uint("keepalive", revised_keep_alive_period))
		return stream.get_error();
	return true;
}

const string_type& update_subscription_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t update_subscription_response::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::subscription_messages::subscription_items_change 

string_type subscription_items_change::type_name = "subsItemsNotif";

rx_message_type_t subscription_items_change::type_id = rx_subscription_items_notification_id;


rx_result subscription_items_change::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.start_array("items", items.size()))
		return stream.get_error();
	for (const auto one : items)
	{
		if (!stream.start_object("item"))
			return stream.get_error();

		if (!stream.write_uint("handle", one.handle))
			return stream.get_error();
		if(!stream.write_value("value", one.value))
			return stream.get_error();

		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();
	return true;
}

rx_result subscription_items_change::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.start_array("items"))
		return stream.get_error();
	while(!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		update_item temp;
		if (!stream.read_uint("handle", temp.handle))
			return stream.get_error();
		if (!stream.read_value("value", temp.value))
			return stream.get_error();

		items.emplace_back(std::move(temp));

		if (!stream.end_object())
			return stream.get_error();
	}
	return true;
}

const string_type& subscription_items_change::get_type_name ()
{
  return type_name;

}

rx_message_type_t subscription_items_change::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::subscription_messages::subscription_write_done 

string_type subscription_write_done::type_name = "subsWriteDone";

rx_message_type_t subscription_write_done::type_id = rx_subscription_write_done_id;


rx_result subscription_write_done::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id))
		return stream.get_error();
	if (!stream.write_uint("transId", transaction_id))
		return stream.get_error();
	if (!stream.start_array("results", results.size()))
		return stream.get_error();
	for (auto&& one : results)
	{
		if (!stream.start_object("result"))
			return stream.get_error();

		if (!stream.write_uint("handle", std::get<0>(one)))
			return stream.get_error();

		if (!stream.write_uint("errCode", std::get<1>(one)))
			return stream.get_error();

		if (!stream.write_string("errMsg", std::move(std::get<2>(one)).c_str()))
			return stream.get_error();

		if (!stream.end_object())
			return stream.get_error();
	}
	if (!stream.end_array())
		return stream.get_error();
	return true;
}

rx_result subscription_write_done::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return stream.get_error();
	subscription_id = temp;
	if (!stream.read_uint("transId", transaction_id))
		return stream.get_error();
	if (!stream.start_array("results"))
		return stream.get_error();
	while (!stream.array_end())
	{
		result_type one;
		if (!stream.start_object("result"))
			return stream.get_error();

		if (!stream.read_uint("handle", std::get<0>(one)))
			return stream.get_error();

		if (!stream.read_uint("errCode", std::get<1>(one)))
			return stream.get_error();

		if (!stream.read_string("errMsg", std::get<2>(one)))
			return stream.get_error();

		if (!stream.end_object())
			return stream.get_error();
		results.emplace_back(one);
	}
	return true;
}

const string_type& subscription_write_done::get_type_name ()
{
  return type_name;

}

rx_message_type_t subscription_write_done::get_type_id ()
{
  return type_id;

}

void subscription_write_done::add_result (runtime_handle_t handle, rx_result&& result)
{
	uint32_t error_code = 0;
	string_type error_text;
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
	results.emplace_back(result_type{ handle, error_code, error_text });
}


} // namespace subscription_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal

