

/****************************************************************************
*
*  sys_internal\rx_directory_messages.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_directory_messages
#include "sys_internal/rx_directory_messages.h"

#include "sys_internal/rx_internal_protocol.h"


namespace rx_internal {

namespace rx_protocol {

namespace messages {

namespace directory_messages {

// Class rx_internal::rx_protocol::messages::directory_messages::rx_make_directory_request 

string_type rx_make_directory_request::type_name = "mkdirReq";

rx_message_type_t rx_make_directory_request::type_id = rx_make_directory_request_id;


rx_result rx_make_directory_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("path", path.c_str()))
		return "Error serializing path";
	return true;
}

rx_result rx_make_directory_request::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("path", path))
		return "Error reading path";
	return true;
}

message_ptr rx_make_directory_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	auto ret = ctx.safe_directory()->add_sub_directory(path);
	if (!ret)
	{
		auto ret_value = std::make_unique<error_message>(ret, 17, request_id);
		return ret_value;
	}
	else
	{
		auto response = std::make_unique<rx_make_directory_response>();
		return response;
	}
}

const string_type& rx_make_directory_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_make_directory_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::directory_messages::rx_remove_directory_request 

string_type rx_remove_directory_request::type_name = "rmdirReq";

rx_message_type_t rx_remove_directory_request::type_id = rx_remove_directory_request_id;


rx_result rx_remove_directory_request::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("path", path.c_str()))
		return "Error serializing path";
	return true;
}

rx_result rx_remove_directory_request::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("path", path))
		return "Error reading path";
	return true;
}

message_ptr rx_remove_directory_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
	auto ret_value = std::make_unique<error_message>("Jebi ga nije jos gotovo!"s, 17, request_id);
	return ret_value;
}

const string_type& rx_remove_directory_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_remove_directory_request::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::directory_messages::rx_make_directory_response 

string_type rx_make_directory_response::type_name = "mkdirResp";

rx_message_type_t rx_make_directory_response::type_id = rx_make_directory_response_id;


rx_result rx_make_directory_response::serialize (base_meta_writer& stream) const
{
	return true;
}

rx_result rx_make_directory_response::deserialize (base_meta_reader& stream)
{
	return true;
}

const string_type& rx_make_directory_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_make_directory_response::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::directory_messages::rx_remove_directory_response 

string_type rx_remove_directory_response::type_name = "rmdirResp";

rx_message_type_t rx_remove_directory_response::type_id = rx_remove_directory_response_id;


rx_result rx_remove_directory_response::serialize (base_meta_writer& stream) const
{
	return true;
}

rx_result rx_remove_directory_response::deserialize (base_meta_reader& stream)
{
	return true;
}

const string_type& rx_remove_directory_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_remove_directory_response::get_type_id ()
{
  return type_id;

}


} // namespace directory_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal

