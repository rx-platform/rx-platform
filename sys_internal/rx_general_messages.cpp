

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
#include "terminal/rx_console.h"


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
    if (!stream.write_time("started", start_time))
        return "Unable to write start time";
    if (!stream.write_time("current", current_time))
        return "Unable to write current time";
    if (!stream.write_string("platform", platform))
        return "Unable to write platform";
    if (!stream.write_string("lib", library))
        return "Unable to write lib";
    if (!stream.write_string("term", terminal))
        return "Unable to write term";
    if (!stream.write_string("comp", compiler))
        return "Unable to write comp";
    if (!stream.write_string("platform_host", platform_host))
        return "Unable to write platform_host";
    if (!stream.write_string("os_host", os_host))
        return "Unable to write os_host";
    if (!stream.write_string("os_itf", os_itf))
        return "Unable to write os_itf";
    if (!stream.write_string("os", os))
        return "Unable to write os";
    if (!stream.write_string("cpu", cpu))
        return "Unable to write cpu";
    if (!stream.write_string("memory", memory))
        return "Unable to write memory";
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
    response->instance = rx_gate::instance().get_rx_name();
    response->node = rx_gate::instance().get_host()->get_default_name();
    response->platform = rx_gate::instance().get_rx_version();
    response->library = rx_gate::instance().get_lib_version();
    response->os_itf = rx_gate::instance().get_hal_version();
    response->terminal = terminal::console::console_runtime::get_terminal_info();
    response->compiler = rx_gate::instance().get_comp_version();
    std::ostringstream out3;
    out3 << rx_gate::instance().get_os_info() << " [PID:" << rx_gate::instance().get_pid() << "]";
    response->os = out3.str();
    string_array hosts;
    rx_gate::instance().get_host()->get_host_info(hosts);
    if (hosts.size() > 0)
        response->os_host = hosts[0];
    if (hosts.size() > 1)
        response->platform_host = hosts[1];
    response->start_time = rx_gate::instance().get_started();
    response->current_time = rx_time::now();
    /////////////////////////////////////////////////////////////////////////
    // Processor
    char buff[0x100];
    size_t cpu_count = 1;
    std::ostringstream out1;
    rx_collect_processor_info(buff, sizeof(buff) / sizeof(buff[0]), &cpu_count);
    out1 << "CPU: " << buff
        << (rx_big_endian ? "; Big-endian" : "; Little-endian");
    response->cpu = out1.str();
    
    /////////////////////////////////////////////////////////////////////////
    // memory
    size_t total = 0;
    size_t free = 0;
    size_t process = 0;
    std::ostringstream out2;
    rx_collect_memory_info(&total, &free, &process);
    out2 << "Memory: Total "
        << (int)(total / 1048576ull)
        << "MiB / Free "
        << (int)(free / 1048576ull)
        << "MiB / Process "
        << (int)(process / 1024ull)
        << "KiB \r\n";
    /////////////////////////////////////////////////////////////////////////
    out2 << "Page size: " << (int)rx_os_page_size() << " bytes";
    response->memory = out2.str();
    
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

