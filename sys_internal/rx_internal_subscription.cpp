

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

// Class sys_internal::rx_protocol::messages::subscription_messages::create_subscriptions_response 

string_type create_subscriptions_response::type_name = "createSubsResp";

rx_message_type_t create_subscriptions_response::type_id = rx_create_subscription_response_id;


rx_result create_subscriptions_response::serialize (base_meta_writer& stream) const
{
	return "Not implemented!";
}

rx_result create_subscriptions_response::deserialize (base_meta_reader& stream)
{
	return "Not implemented!";
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
	return "Not implemented!";
}

rx_result create_subscription_request::deserialize (base_meta_reader& stream)
{
	return "Not implemented!";
}

message_ptr create_subscription_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return message_ptr();
}

const string_type& create_subscription_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t create_subscription_request::get_type_id ()
{
  return type_id;

}


} // namespace subscription_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal

