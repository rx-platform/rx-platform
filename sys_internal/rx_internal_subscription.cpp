

/****************************************************************************
*
*  sys_internal\rx_internal_subscription.cpp
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


// rx_internal_subscription
#include "sys_internal/rx_internal_subscription.h"

#include "rx_internal_protocol.h"


namespace sys_internal {

namespace rx_protocol {

namespace messages {

namespace subscription_messages {

// Class sys_internal::rx_protocol::messages::subscription_messages::update_subscription_response 

string_type update_subscription_response::type_name = "updSubsResp";

rx_message_type_t update_subscription_response::type_id = rx_update_subscription_response_id;


rx_result update_subscription_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	if (!stream.write_uint("rate", revised_publish_rate))
		return "error writing publish rate";
	if (!stream.write_uint("keepalive", revised_keep_alive_period))
		return "error writing keep alive period";
	return true;
}

rx_result update_subscription_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	if (!stream.read_uint("rate", revised_publish_rate))
		return "error reading publish rate";
	if (!stream.read_uint("keepalive", revised_keep_alive_period))
		return "error reading keep alive period";
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


// Class sys_internal::rx_protocol::messages::subscription_messages::delete_subscription_response 

string_type delete_subscription_response::type_name = "delSubsResp";

rx_message_type_t delete_subscription_response::type_id = rx_delete_subscription_response_id;


rx_result delete_subscription_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result delete_subscription_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
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


// Class sys_internal::rx_protocol::messages::subscription_messages::create_subscriptions_response 

string_type create_subscriptions_response::type_name = "createSubsResp";

rx_message_type_t create_subscriptions_response::type_id = rx_create_subscription_response_id;


rx_result create_subscriptions_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	if (!stream.write_uint("rate", revised_publish_rate))
		return "error writing publish rate";
	if (!stream.write_uint("keepalive", revised_keep_alive_period))
		return "error writing keep alive period";
	return true;
}

rx_result create_subscriptions_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	if (!stream.read_uint("rate", revised_publish_rate))
		return "error reading publish rate";
	if (!stream.read_uint("keepalive", revised_keep_alive_period))
		return "error reading keep alive period";
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


// Class sys_internal::rx_protocol::messages::subscription_messages::create_subscription_request 

string_type create_subscription_request::type_name = "createSubsReq";

rx_message_type_t create_subscription_request::type_id = rx_create_subscription_request_id;


rx_result create_subscription_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	if (!stream.write_uint("rate", publish_rate))
		return "error writing publish rate";
	if (!stream.write_uint("keepalive", keep_alive_period))
		return "error writing keep alive period";
	if (!stream.write_bool("active", active))
		return "error writing active";
	if (!stream.write_byte("priority", priority))
		return "error writing active";
	return true;
}

rx_result create_subscription_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	if (!stream.read_uint("rate", publish_rate))
		return "error reading publish rate";
	if (!stream.read_uint("keepalive", keep_alive_period))
		return "error reading keep alive period";
	if (!stream.read_bool("active", active))
		return "error reading active";
	if (!stream.read_byte("priority", priority))
		return "error reading active";
	return true;
}

message_ptr create_subscription_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return std::make_unique<error_message>("Not implemented!!!"s, 9, request_id);
}

const string_type& create_subscription_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t create_subscription_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::subscription_messages::update_subscription_request 

string_type update_subscription_request::type_name = "updSubsReq";

rx_message_type_t update_subscription_request::type_id = rx_update_subscription_request_id;


rx_result update_subscription_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	if (!stream.write_uint("rate", publish_rate))
		return "error writing publish rate";
	if (!stream.write_uint("keepalive", keep_alive_period))
		return "error writing keep alive period";
	if (!stream.write_bool("active", active))
		return "error writing active";
	if (!stream.write_byte("priority", priority))
		return "error writing active";
	return true;
}

rx_result update_subscription_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error writing subscription id";
	subscription_id = temp;
	if (!stream.read_uint("rate", publish_rate))
		return "error writing publish rate";
	if (!stream.read_uint("keepalive", keep_alive_period))
		return "error writing keep alive period";
	if (!stream.read_bool("active", active))
		return "error writing active";
	if (!stream.read_byte("priority", priority))
		return "error writing active";
	return true;
}

message_ptr update_subscription_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return std::make_unique<error_message>("Not implemented!!!"s, 9, request_id);
}

const string_type& update_subscription_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t update_subscription_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::subscription_messages::delete_subscription_request 

string_type delete_subscription_request::type_name = "delSubsReq";

rx_message_type_t delete_subscription_request::type_id = rx_delete_subscription_request_id;


rx_result delete_subscription_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result delete_subscription_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

message_ptr delete_subscription_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return std::make_unique<error_message>("Not implemented!!!"s, 9, request_id);
}

const string_type& delete_subscription_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t delete_subscription_request::get_type_id ()
{
  return type_id;

}


} // namespace subscription_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal

