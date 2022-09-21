

/****************************************************************************
*
*  protocols\opcua\rx_opcua_security.cpp
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


// rx_opcua_security
#include "protocols/opcua/rx_opcua_security.h"

#include "protocols/opcua/rx_opcua_mapping.h"


namespace protocols {

namespace opcua {

namespace opcua_transport {

// Class protocols::opcua::opcua_transport::opcua_sec_none_endpoint 

opcua_sec_none_endpoint::opcua_sec_none_endpoint (opcua_sec_none_port* port)
      : port_(port)
{
    OPCUA_LOG_DEBUG("opcua_sec_none_endpoint", 200, "OPC UA server Security None endpoint created.");
    opcua_sec_none_protocol_type* mine_entry = this;

    rx_protocol_result_t res = opcua_bin_init_sec_none_server_transport(mine_entry);
    if (res == RX_PROTOCOL_OK)
    {
        mine_entry->stack_entry.received_function = &opcua_sec_none_endpoint::received_function;
        mine_entry->stack_entry.send_function = &opcua_sec_none_endpoint::send_function;
        mine_entry->transport_connected = &opcua_sec_none_endpoint::transport_connected;
    }
}


opcua_sec_none_endpoint::~opcua_sec_none_endpoint()
{
    OPCUA_LOG_DEBUG("opcua_sec_none_endpoint", 200, "OPC UA server Security None endpoint destroyed.");
}



rx_protocol_result_t opcua_sec_none_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    return opcua_bin_sec_none_bytes_received(reference, packet);
}

rx_protocol_result_t opcua_sec_none_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    return opcua_bin_sec_none_bytes_send(reference, packet);
}

rx_protocol_result_t opcua_sec_none_endpoint::transport_connected (rx_protocol_stack_endpoint* reference, const protocol_address* local_address, const protocol_address* remote_address)
{
    opcua_sec_none_endpoint* me = reinterpret_cast<opcua_sec_none_endpoint*>(reference->user_data);
    auto result = me->port_->stack_endpoint_connected(reference, local_address, remote_address);
    if (!result)
    {
        std::ostringstream ss;
        ss << "Error binding connected endpoint ";
        ss << result.errors_line();
        OPCUA_LOG_ERROR("opcua_sec_none_endpoint", 200, ss.str().c_str());
        return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
    }
    return RX_PROTOCOL_OK;
}


// Class protocols::opcua::opcua_transport::opcua_sec_none_port 

std::map<rx_node_id, opcua_sec_none_port::smart_ptr> opcua_sec_none_port::runtime_instances;

opcua_sec_none_port::opcua_sec_none_port()
{
    construct_func = [this]()
    {
        auto rt = std::make_unique<opcua_sec_none_endpoint>(this);
        return construct_func_type::result_type{ &rt->stack_entry, std::move(rt) };
    };
}



// Class protocols::opcua::opcua_transport::opcua_sec_none_client_port 

std::map<rx_node_id, opcua_sec_none_client_port::smart_ptr> opcua_sec_none_client_port::runtime_instances;

opcua_sec_none_client_port::opcua_sec_none_client_port()
{
    construct_func = [this]()
    {
        auto rt = std::make_unique<opcua_sec_none_client_endpoint>(this);
        return construct_func_type::result_type{ &rt->stack_entry, std::move(rt) };
    };
}



// Class protocols::opcua::opcua_transport::opcua_sec_none_client_endpoint 

opcua_sec_none_client_endpoint::opcua_sec_none_client_endpoint (opcua_sec_none_client_port* port)
      : port_(port)
{
    OPCUA_LOG_DEBUG("opcua_sec_none_client_endpoint", 200, "OPC UA server Security None client endpoint created.");
    opcua_sec_none_protocol_type* mine_entry = this;

    rx_protocol_result_t res = opcua_bin_init_sec_none_client_transport(mine_entry);
    if (res == RX_PROTOCOL_OK)
    {
        mine_entry->stack_entry.received_function = &opcua_sec_none_client_endpoint::received_function;
        mine_entry->stack_entry.send_function = &opcua_sec_none_client_endpoint::send_function;
    }
}


opcua_sec_none_client_endpoint::~opcua_sec_none_client_endpoint()
{
    OPCUA_LOG_DEBUG("opcua_sec_none_client_endpoint", 200, "OPC UA server Security None client endpoint destroyed.");
}



rx_protocol_result_t opcua_sec_none_client_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    return opcua_bin_sec_none_bytes_received(reference, packet);
}

rx_protocol_result_t opcua_sec_none_client_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    return opcua_bin_sec_none_bytes_send(reference, packet);
}


} // namespace opcua_transport
} // namespace opcua
} // namespace protocols

