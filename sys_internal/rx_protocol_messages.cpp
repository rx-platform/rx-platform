

/****************************************************************************
*
*  sys_internal\rx_protocol_messages.cpp
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


// rx_internal_protocol
#include "sys_internal/rx_internal_protocol.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"

#include "system/serialization/rx_ser_json.h"
#include "api/rx_namespace_api.h"
#include "api/rx_meta_api.h"
#include "system/server/rx_server.h"

#include "rx_query_messages.h"
#include "rx_set_messages.h"
#include "rx_internal_subscription.h"
#include "rx_subscription_items.h"
#include "rx_directory_messages.h"
#include "rx_general_messages.h"
#include "rx_trans_messages.h"

using namespace rx_platform;


namespace rx_internal {

namespace rx_protocol {

namespace messages {

// Class rx_internal::rx_protocol::messages::rx_message_base 

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

void rx_message_base::deinit_messages ()
{
	rx_request_message::deinit_request_messages();
}


// Class rx_internal::rx_protocol::messages::error_message 

string_type error_message::type_name = "error";

rx_message_type_t error_message::type_id = rx_error_message_id;


rx_result error_message::serialize (base_meta_writer& stream) const
{
	if (!stream.write_uint("errCode", error_code))
		return stream.get_error();
	if (!stream.write_string("errMsg", error_text.c_str()))
		return stream.get_error();
	return true;
}

rx_result error_message::deserialize (base_meta_reader& stream)
{
	if (!stream.read_uint("errCode", error_code))
		return stream.get_error();
	if (!stream.read_string("errMsg", error_text))
		return stream.get_error();
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


// Class rx_internal::rx_protocol::messages::rx_request_message 

rx_request_message::registered_string_messages_type rx_request_message::registered_string_messages_;

rx_request_message::registered_messages_type rx_request_message::registered_messages_;


rx_result_with<request_message_ptr> rx_request_message::create_request_from_stream (rx_request_id_t& request_id, base_meta_reader& reader)
{
	string_type msgType;

	if (!reader.start_object("header"))
		return reader.get_error();
	if (!reader.read_int("requestId", request_id))
		return reader.get_error();
	if (!reader.read_string("msgType", msgType))
		return reader.get_error();
	if (!reader.end_object())
		return reader.get_error();

	auto result = create_request_message(msgType);
	if (!result)
		return result;

	result.value()->request_id = request_id;

	if (!reader.start_object("body"))
		return reader.get_error();

	auto ser_result = result.value()->deserialize(reader);
	if (!ser_result)
		return ser_result.errors();

	if (!reader.end_object())
		return reader.get_error();

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
	registered_messages_.emplace(general_messages::rx_system_info_request::type_id, [] { return std::make_unique<general_messages::rx_system_info_request>(); });
	registered_messages_.emplace(query_messages::get_code_info_request::type_id, [] { return std::make_unique<query_messages::get_code_info_request>(); });
	registered_messages_.emplace(query_messages::browse_request_message::type_id, [] { return std::make_unique<query_messages::browse_request_message>(); });
	registered_messages_.emplace(query_messages::browse_runtime_request::type_id, [] { return std::make_unique<query_messages::browse_runtime_request>(); });
	registered_messages_.emplace(query_messages::query_request_message::type_id, [] { return std::make_unique<query_messages::query_request_message>(); });
	registered_messages_.emplace(query_messages::get_type_request::type_id, [] { return std::make_unique<query_messages::get_type_request>(); });
	registered_messages_.emplace(query_messages::get_runtime_request::type_id, [] { return std::make_unique<query_messages::get_runtime_request>(); });
	registered_messages_.emplace(rx_connection_context_request::type_id, [] { return std::make_unique<rx_connection_context_request>(); });
	registered_messages_.emplace(set_messages::delete_type_request::type_id, [] { return std::make_unique<set_messages::delete_type_request>(); });
	registered_messages_.emplace(set_messages::set_type_request::type_id, [] { return std::make_unique<set_messages::set_type_request>(); });
	registered_messages_.emplace(set_messages::update_type_request::type_id, [] { return std::make_unique<set_messages::update_type_request>(); });
	registered_messages_.emplace(set_messages::delete_runtime_request::type_id, [] { return std::make_unique<set_messages::delete_runtime_request>(); });
	registered_messages_.emplace(set_messages::set_runtime_request::type_id, [] { return std::make_unique<set_messages::set_runtime_request>(); });
	registered_messages_.emplace(set_messages::update_runtime_request::type_id, [] { return std::make_unique<set_messages::update_runtime_request>(); });
	registered_messages_.emplace(set_messages::prototype_runtime_request::type_id, [] { return std::make_unique<set_messages::prototype_runtime_request>(); });
	registered_messages_.emplace(set_messages::read_runtime_request::type_id, [] { return std::make_unique<set_messages::read_runtime_request>(); });
	registered_messages_.emplace(set_messages::rx_update_directory_request_message::type_id, [] { return std::make_unique<set_messages::rx_update_directory_request_message>(); });
	// directory messages
	registered_messages_.emplace(directory_messages::rx_make_directory_request::type_id, [] { return std::make_unique<directory_messages::rx_make_directory_request>(); });
	registered_messages_.emplace(directory_messages::rx_remove_directory_request::type_id, [] { return std::make_unique<directory_messages::rx_remove_directory_request>(); });
	// subscription messages
	registered_messages_.emplace(subscription_messages::create_subscription_request::type_id, [] { return std::make_unique<subscription_messages::create_subscription_request>(); });
	registered_messages_.emplace(subscription_messages::update_subscription_request::type_id, [] { return std::make_unique<subscription_messages::update_subscription_request>(); });
	registered_messages_.emplace(subscription_messages::delete_subscription_request::type_id, [] { return std::make_unique<subscription_messages::delete_subscription_request>(); });
	// subscription items messages
	registered_messages_.emplace(items_messages::add_items_request::type_id, [] { return std::make_unique<items_messages::add_items_request>(); });
	registered_messages_.emplace(items_messages::modify_items_request::type_id, [] { return std::make_unique<items_messages::modify_items_request>(); });
	registered_messages_.emplace(items_messages::remove_items_request::type_id, [] { return std::make_unique<items_messages::remove_items_request>(); });
	registered_messages_.emplace(items_messages::read_items_request::type_id, [] { return std::make_unique<items_messages::read_items_request>(); });
	registered_messages_.emplace(items_messages::write_items_request::type_id, [] { return std::make_unique<items_messages::write_items_request>(); });
	registered_messages_.emplace(items_messages::execute_item_request::type_id, [] { return std::make_unique<items_messages::execute_item_request>(); });
	// special messages
	registered_messages_.emplace(rx_keep_alive_message::type_id, [] { return std::make_unique<rx_keep_alive_message>(); });

	registered_string_messages_.emplace(general_messages::rx_system_info_request::type_name, [] { return std::make_unique<general_messages::rx_system_info_request>(); });
	registered_string_messages_.emplace(query_messages::get_code_info_request::type_name, [] { return std::make_unique<query_messages::get_code_info_request>(); });
	registered_string_messages_.emplace(query_messages::browse_request_message::type_name, [] { return std::make_unique<query_messages::browse_request_message>(); });
	registered_string_messages_.emplace(query_messages::browse_runtime_request::type_name, [] { return std::make_unique<query_messages::browse_runtime_request>(); });
	registered_string_messages_.emplace(query_messages::query_request_message::type_name, [] { return std::make_unique<query_messages::query_request_message>(); });
	registered_string_messages_.emplace(query_messages::get_type_request::type_name, [] { return std::make_unique<query_messages::get_type_request>(); });
	registered_string_messages_.emplace(query_messages::get_runtime_request::type_name, [] { return std::make_unique<query_messages::get_runtime_request>(); });
	registered_string_messages_.emplace(rx_connection_context_request::type_name, [] { return std::make_unique<rx_connection_context_request>(); });
	registered_string_messages_.emplace(set_messages::delete_type_request::type_name, [] { return std::make_unique<set_messages::delete_type_request>(); });
	registered_string_messages_.emplace(set_messages::set_type_request::type_name, [] { return std::make_unique<set_messages::set_type_request>(); });
	registered_string_messages_.emplace(set_messages::update_type_request::type_name, [] { return std::make_unique<set_messages::update_type_request>(); });
	registered_string_messages_.emplace(set_messages::delete_runtime_request::type_name, [] { return std::make_unique<set_messages::delete_runtime_request>(); });
	registered_string_messages_.emplace(set_messages::set_runtime_request::type_name, [] { return std::make_unique<set_messages::set_runtime_request>(); });
	registered_string_messages_.emplace(set_messages::update_runtime_request::type_name, [] { return std::make_unique<set_messages::update_runtime_request>(); });
	registered_string_messages_.emplace(set_messages::prototype_runtime_request::type_name, [] { return std::make_unique<set_messages::prototype_runtime_request>(); });
	registered_string_messages_.emplace(set_messages::read_runtime_request::type_name, [] { return std::make_unique<set_messages::read_runtime_request>(); });
	registered_string_messages_.emplace(set_messages::rx_update_directory_request_message::type_name, [] { return std::make_unique<set_messages::rx_update_directory_request_message>(); });
	// directory messages
	registered_string_messages_.emplace(directory_messages::rx_make_directory_request::type_name, [] { return std::make_unique<directory_messages::rx_make_directory_request>(); });
	registered_string_messages_.emplace(directory_messages::rx_remove_directory_request::type_name, [] { return std::make_unique<directory_messages::rx_remove_directory_request>(); });
	// subscription messages
	registered_string_messages_.emplace(subscription_messages::create_subscription_request::type_name, [] { return std::make_unique<subscription_messages::create_subscription_request>(); });
	registered_string_messages_.emplace(subscription_messages::update_subscription_request::type_name, [] { return std::make_unique<subscription_messages::update_subscription_request>(); });
	registered_string_messages_.emplace(subscription_messages::delete_subscription_request::type_name, [] { return std::make_unique<subscription_messages::delete_subscription_request>(); });
	// subscription items messages
	registered_string_messages_.emplace(items_messages::add_items_request::type_name, [] { return std::make_unique<items_messages::add_items_request>(); });
	registered_string_messages_.emplace(items_messages::modify_items_request::type_name, [] { return std::make_unique<items_messages::modify_items_request>(); });
	registered_string_messages_.emplace(items_messages::remove_items_request::type_name, [] { return std::make_unique<items_messages::remove_items_request>(); });
	registered_string_messages_.emplace(items_messages::read_items_request::type_name, [] { return std::make_unique<items_messages::read_items_request>(); });
	registered_string_messages_.emplace(items_messages::write_items_request::type_name, [] { return std::make_unique<items_messages::write_items_request>(); });
	registered_string_messages_.emplace(items_messages::execute_item_request::type_name, [] { return std::make_unique<items_messages::execute_item_request>(); });
	// special messages
	registered_string_messages_.emplace(rx_keep_alive_message::type_name, [] { return std::make_unique<rx_keep_alive_message>(); });

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

void rx_request_message::deinit_request_messages ()
{
	meta::queries::rx_query::deinit_query_types();
	registered_messages_.clear();
	registered_string_messages_.clear();
}


// Class rx_internal::rx_protocol::messages::rx_connection_context_request 

string_type rx_connection_context_request::type_name = "connCtxReq";

rx_message_type_t rx_connection_context_request::type_id = rx_connection_context_request_id;


rx_result rx_connection_context_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("dir", directory.c_str()))
		return stream.get_error();
	if (!stream.write_string("app", application.c_str()))
		return stream.get_error();
	if (!stream.write_string("domain", domain.c_str()))
		return stream.get_error();
	if (!stream.write_version("sversion", stream_version))
		return stream.get_error();
	return true;
}

rx_result rx_connection_context_request::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("dir", directory))
		return stream.get_error();
	if (!stream.read_string("app", application))
		return stream.get_error();
	if (!stream.read_string("domain", domain))
		return stream.get_error();
	if (!stream.read_version("sversion", stream_version))
		return stream.get_error();
	return true;
}

message_ptr rx_connection_context_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	return conn->set_context(ctx, *this);
}

const string_type& rx_connection_context_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_connection_context_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::rx_connection_context_response 

string_type rx_connection_context_response::type_name = "connCtxResp";

rx_message_type_t rx_connection_context_response::type_id = rx_connection_context_response_id;


rx_result rx_connection_context_response::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("dir", directory.c_str()))
		return stream.get_error();
	if (!stream.write_string("app", application.c_str()))
		return stream.get_error();
	if (!stream.write_string("domain", domain.c_str()))
		return stream.get_error();
	if (!stream.write_id("appid", application_id))
		return stream.get_error();
	if (!stream.write_id("domainid", domain_id))
		return stream.get_error();
	if (!stream.write_version("sversion", stream_version))
		return stream.get_error();
	return true;
}

rx_result rx_connection_context_response::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("dir", directory))
		return stream.get_error();
	if (!stream.read_string("app", application))
		return stream.get_error();
	if (!stream.read_string("domain", domain))
		return stream.get_error();
	if (!stream.read_id("appid", application_id))
		return stream.get_error();
	if (!stream.read_id("domainid", domain_id))
		return stream.get_error();
	if (!stream.read_version("sversion", stream_version))
		return stream.get_error();
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


// Class rx_internal::rx_protocol::messages::rx_keep_alive_message 

string_type rx_keep_alive_message::type_name = "keep-alive";

rx_message_type_t rx_keep_alive_message::type_id = rx_keep_alive_message_id;


rx_result rx_keep_alive_message::serialize (base_meta_writer& stream) const
{
	return true;
}

rx_result rx_keep_alive_message::deserialize (base_meta_reader& stream)
{
	return true;
}

const string_type& rx_keep_alive_message::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_keep_alive_message::get_type_id ()
{
  return type_id;

}

message_ptr rx_keep_alive_message::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	auto ret_msg = std::make_unique<rx_keep_alive_message>();
	ret_msg->request_id = request_id;
	return ret_msg;
}


// Class rx_internal::rx_protocol::messages::rx_connection_notify_message 

string_type rx_connection_notify_message::type_name = "connectionNotify";

rx_message_type_t rx_connection_notify_message::type_id = rx_connection_notify_id;


rx_result rx_connection_notify_message::serialize (base_meta_writer& stream) const
{
	if (!stream.write_id("changed_id", changed_id))
		return stream.get_error();
	if (!stream.write_string("changed_path", changed_path.c_str()))
		return stream.get_error();
	return true;
}

const string_type& rx_connection_notify_message::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_connection_notify_message::get_type_id ()
{
  return type_id;

}

rx_result rx_connection_notify_message::deserialize (base_meta_reader& stream)
{
	if (!stream.read_id("changed_id", changed_id))
		return stream.get_error();
	if (!stream.read_string("changed_path", changed_path))
		return stream.get_error();
	return true;
}


} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal

