

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

// Class sys_internal::rx_protocol::messages::items_messages::modify_items_response 

string_type modify_items_response::type_name = "modifyItemsResp";

rx_message_type_t modify_items_response::type_id = rx_modify_items_response_id;


rx_result modify_items_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result modify_items_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

const string_type& modify_items_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t modify_items_response::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::write_items_response 

string_type write_items_response::type_name = "writeItemsResp";

rx_message_type_t write_items_response::type_id = rx_write_items_response_id;


rx_result write_items_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result write_items_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

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


// Class sys_internal::rx_protocol::messages::items_messages::remove_items_response 

string_type remove_items_response::type_name = "removeItemsResp";

rx_message_type_t remove_items_response::type_id = rx_remove_items_response_id;


rx_result remove_items_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result remove_items_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

const string_type& remove_items_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t remove_items_response::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::add_items_response 

string_type add_items_response::type_name = "addItemsResp";

rx_message_type_t add_items_response::type_id = rx_add_items_response_id;


rx_result add_items_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result add_items_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

const string_type& add_items_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t add_items_response::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::items_messages::add_items_request 

string_type add_items_request::type_name = "addItemsReq";

rx_message_type_t add_items_request::type_id = rx_add_items_request_id;


rx_result add_items_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result add_items_request::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

message_ptr add_items_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	return std::make_unique<error_message>("Not implemented!!!"s, 9, request_id);
}

const string_type& add_items_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t add_items_request::get_type_id ()
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


rx_result read_items_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uuid("id", subscription_id.uuid()))
		return "error writing subscription id";
	return true;
}

rx_result read_items_response::deserialize (base_meta_reader& stream)
{
	rx_uuid_t temp;
	if (!stream.read_uuid("id", temp))
		return "error reading subscription id";
	subscription_id = temp;
	return true;
}

const string_type& read_items_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t read_items_response::get_type_id ()
{
  return type_id;

}


} // namespace items_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal

