

/****************************************************************************
*
*  protocols\opcua\rx_opcua_mapping.cpp
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


// rx_opcua_mapping
#include "protocols/opcua/rx_opcua_mapping.h"

#include "protocols/ansi_c/opcua_c/rx_opcua_transport.h"


namespace protocols {

namespace opcua {

// Class protocols::opcua::opcua_transport_endpoint 

opcua_transport_endpoint::opcua_transport_endpoint (runtime::items::port_runtime* port)
      : port_(port)
{
    OPCUA_LOG_DEBUG("tcp_server_endpoint", 200, "OPC UA server endpoint created.");
    opcua_transport_protocol_type* mine_entry = this;
    rx_protocol_result_t res = opcua_bin_init_pipe_transport(mine_entry, 0x10000, 0x10);
    if (res == RX_PROTOCOL_OK)
    {
        mine_entry->stack_entry.received_function = &opcua_transport_endpoint::received_function;
        mine_entry->stack_entry.send_function = &opcua_transport_endpoint::send_function;
    }
}


opcua_transport_endpoint::~opcua_transport_endpoint()
{
    OPCUA_LOG_DEBUG("tcp_server_endpoint", 200, "OPC UA server endpoint destroyed.");
}



rx_protocol_stack_endpoint* opcua_transport_endpoint::bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
    sent_func_ = sent_func;
    received_func_ = received_func;
    return &stack_entry;
}

rx_protocol_result_t opcua_transport_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    opcua_transport_endpoint* self = reinterpret_cast<opcua_transport_endpoint*>(reference->user_data);
    self->received_func_((int64_t)rx_get_packet_available_data(packet.buffer));
    return opcua_bin_bytes_received(reference, packet);
}

rx_protocol_result_t opcua_transport_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    opcua_transport_endpoint* self = reinterpret_cast<opcua_transport_endpoint*>(reference->user_data);
    self->sent_func_((int64_t)rx_get_packet_usable_data(packet.buffer));
    return opcua_bin_bytes_send(reference, packet);
}


// Class protocols::opcua::opcua_transport_port 

std::map<rx_node_id, opcua_transport_port::smart_ptr> opcua_transport_port::runtime_instances;

opcua_transport_port::opcua_transport_port()
{
    construct_func = [this]()
    {
        auto rt = std::make_unique<opcua_transport_endpoint>(this);
        auto entry = rt->bind([this](int64_t count)
            {
            },
            [this](int64_t count)
            {
            });
        return construct_func_type::result_type{ entry, std::move(rt) };
    };
}



} // namespace opcua
} // namespace protocols

