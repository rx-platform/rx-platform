

/****************************************************************************
*
*  protocols\opcua\rx_opcua_mapping.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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
#include "system/server/rx_log.h"


namespace protocols {

namespace opcua {
void opcua_split_url(const string_type& url, string_type& addr, string_type& path)
{
    size_t idx = url.find("://");
    if (idx != string_type::npos)
        idx += 3;
    else
        idx = 0;
    idx = url.find('/', idx);
    if (idx == string_type::npos)
    {
        addr = url;
        path = "";
    }
    else
    {
        addr = url.substr(0, idx);
        path = url.substr(idx + 1);
    }
}

namespace opcua_transport {

// Class protocols::opcua::opcua_transport::opcua_transport_endpoint 

opcua_transport_endpoint::opcua_transport_endpoint (opcua_transport_port* port)
      : port_(port)
{
    OPCUA_LOG_DEBUG("opcua_transport_endpoint", 200, "OPC UA server endpoint created.");
    opcua_transport_protocol_type* mine_entry = this;
    rx_protocol_result_t res = opcua_bin_init_server_transport(mine_entry);
    if (res == RX_PROTOCOL_OK)
    {
        mine_entry->stack_entry.received_function = &opcua_transport_endpoint::received_function;
        mine_entry->stack_entry.send_function = &opcua_transport_endpoint::send_function;
    }
}


opcua_transport_endpoint::~opcua_transport_endpoint()
{
    OPCUA_LOG_DEBUG("opcua_transport_endpoint", 200, "OPC UA server endpoint destroyed.");
}



rx_protocol_stack_endpoint* opcua_transport_endpoint::bind ()
{
    return &stack_entry;
}

rx_protocol_result_t opcua_transport_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    opcua_transport_endpoint* self = reinterpret_cast<opcua_transport_endpoint*>(reference->user_data);
    self->port_->status.received_packet();
    return opcua_bin_bytes_received(reference, packet);
}

rx_protocol_result_t opcua_transport_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    opcua_transport_endpoint* self = reinterpret_cast<opcua_transport_endpoint*>(reference->user_data);
    self->port_->status.sent_packet();
    return rx_move_packet_down(reference, packet);
}


// Class protocols::opcua::opcua_transport::opcua_transport_port 

std::map<rx_node_id, opcua_transport_port::smart_ptr> opcua_transport_port::runtime_instances;

opcua_transport_port::opcua_transport_port()
{
    construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
    {
        auto rt = std::make_unique<opcua_transport_endpoint>(this);
        auto entry = rt->bind();
        return construct_func_type::result_type{ entry, std::move(rt) };
    };
}



rx_result opcua_transport_port::initialize_runtime (runtime_init_context& ctx)
{
    auto result = status.initialize(ctx);
    return result;
}


// Class protocols::opcua::opcua_transport::opcua_client_transport_endpoint 

opcua_client_transport_endpoint::opcua_client_transport_endpoint (opcua_client_transport_port* port)
      : port_(port)
{
    OPCUA_LOG_DEBUG("opcua_client_transport_endpoint", 200, "OPC UA client endpoint created.");
    opcua_transport_protocol_type* mine_entry = this;
    rx_protocol_result_t res = opcua_bin_init_client_transport(mine_entry);
    if (res == RX_PROTOCOL_OK)
    {
        mine_entry->stack_entry.received_function = &opcua_client_transport_endpoint::received_function;
        mine_entry->stack_entry.send_function = &opcua_client_transport_endpoint::send_function;
        mine_entry->stack_entry.connected_function = &opcua_client_transport_endpoint::connected_function;
        //mine_entry->stack_entry.disconnected_function = &opcua_client_transport_endpoint::disconnected_function;
    }
}


opcua_client_transport_endpoint::~opcua_client_transport_endpoint()
{
    OPCUA_LOG_DEBUG("opcua_client_transport_endpoint", 200, "OPC UA client endpoint destroyed.");
}



rx_protocol_stack_endpoint* opcua_client_transport_endpoint::bind ()
{
    return &stack_entry;
}

rx_protocol_result_t opcua_client_transport_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    opcua_client_transport_endpoint* self = reinterpret_cast<opcua_client_transport_endpoint*>(reference->user_data);
    self->port_->status.received_packet();
    return opcua_bin_bytes_received(reference, packet);
}

rx_protocol_result_t opcua_client_transport_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    opcua_client_transport_endpoint* self = reinterpret_cast<opcua_client_transport_endpoint*>(reference->user_data);
    self->port_->status.sent_packet();
    return rx_move_packet_down(reference, packet);
}

rx_protocol_result_t opcua_client_transport_endpoint::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
    return opcua_bin_client_connected(reference, session);
}

rx_protocol_result_t opcua_client_transport_endpoint::disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
{
    return RX_PROTOCOL_OK;
}


// Class protocols::opcua::opcua_transport::opcua_client_transport_port 

std::map<rx_node_id, opcua_client_transport_port::smart_ptr> opcua_client_transport_port::runtime_instances;

opcua_client_transport_port::opcua_client_transport_port()
{
    construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
    {
        auto rt = std::make_unique<opcua_client_transport_endpoint>(this);
        
        io::string_address temp;
        if (temp.parse(remote_address))
        {
            string_type temp_str = temp.get_address();
            if (!temp_str.empty())
            {
                char* temp_char = (char*)rx_heap_alloc(temp_str.size() + 1);
                strcpy(temp_char, temp_str.c_str());
                rt->endpoint_url = (const char*)temp_char;
            }
        }
        auto entry = rt->bind();
        return construct_func_type::result_type{ entry, std::move(rt) };
    };
}



rx_result opcua_client_transport_port::initialize_runtime (runtime_init_context& ctx)
{
    auto result = status.initialize(ctx);
    return result;
}


} // namespace opcua_transport
} // namespace opcua
} // namespace protocols

