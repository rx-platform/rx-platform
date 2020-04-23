

/****************************************************************************
*
*  sys_internal\rx_general_messages.cpp
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


// rx_general_messages
#include "sys_internal/rx_general_messages.h"

#include "sys_internal/rx_internal_protocol.h"
#include "system//server/rx_server.h"


namespace rx_internal {

namespace rx_protocol {

namespace messages {

namespace general_messages {

// Class rx_internal::rx_protocol::messages::general_messages::rx_system_info_response 

string_type rx_system_info_response::type_name = "infoResp";

rx_message_type_t rx_system_info_response::type_id = rx_info_response_id;


rx_result rx_system_info_response::serialize (base_meta_writer& stream) const
{
    if (!stream.start_object("general"))
        return "Unable to start object general";
    if(!stream.write_string("instance", instance))
        return "Unable to write instance";
    if (!stream.write_string("node", node))
        return "Unable to write node";
    if (!stream.write_string("platform", platform))
        return "Unable to write platform";
    if (!stream.write_string("lib", library))
        return "Unable to write lib";
    if (!stream.write_string("os_itf", os_itf))
        return "Unable to write os_itf";
    if (!stream.write_string("os", os))
        return "Unable to write os";
    if (!stream.end_object())
        return "Unable to end object general";
    return true;
}

rx_result rx_system_info_response::deserialize (base_meta_reader& stream)
{
    if (!stream.start_object("general"))
        return "Unable to start object general";
    if (!stream.read_string("instance", instance))
        return "Unable to read instance";
    if (!stream.read_string("node", node))
        return "Unable to read node";
    if (!stream.read_string("platform", platform))
        return "Unable to read platform";
    if (!stream.read_string("lib", library))
        return "Unable to read lib";
    if (!stream.read_string("os_itf", os_itf))
        return "Unable to read os_itf";
    if (!stream.read_string("os", os))
        return "Unable to read os";
    if (!stream.end_object())
        return "Unable to end object general";
    return true;
}

const string_type& rx_system_info_response::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_system_info_response::get_type_id ()
{
  return type_id;

}


// Class rx_internal::rx_protocol::messages::general_messages::rx_system_info_request 

string_type rx_system_info_request::type_name = "infoReq";

rx_message_type_t rx_system_info_request::type_id = rx_info_request_id;


rx_result rx_system_info_request::serialize (base_meta_writer& stream) const
{
    return true;
}

rx_result rx_system_info_request::deserialize (base_meta_reader& stream)
{
    return true;
}

message_ptr rx_system_info_request::do_job (api::rx_context ctx, rx_protocol_connection_ptr conn)
{
    auto response = std::make_unique<rx_system_info_response>();
    response->request_id = request_id;
    response->platform = rx_gate::instance().get_rx_version();
    response->library = rx_gate::instance().get_lib_version();
    response->os_itf = rx_gate::instance().get_hal_version();
    response->os = rx_gate::instance().get_os_info();
    return response;
}

const string_type& rx_system_info_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t rx_system_info_request::get_type_id ()
{
  return type_id;

}


} // namespace general_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal

