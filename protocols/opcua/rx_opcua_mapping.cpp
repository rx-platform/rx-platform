

/****************************************************************************
*
*  protocols\opcua\rx_opcua_mapping.cpp
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


// rx_opcua_mapping
#include "protocols/opcua/rx_opcua_mapping.h"

#include "protocols/ansi_c/opcua_c/rx_opcua_transport.h"


namespace protocols {

namespace opcua {

// Class protocols::opcua::opcua_transport_endpoint 

opcua_transport_endpoint::opcua_transport_endpoint()
{

    opcua_transport_protocol_type* mine_entry = this;
    rx_protocol_result_t res = opcua_bin_init_pipe_transport(mine_entry, 0x10000, 0x10);
    if (res == RX_PROTOCOL_OK)
    {
        mine_entry->protocol_stack_entry.received_function = &opcua_transport_endpoint::received_function;
        mine_entry->protocol_stack_entry.send_function = &opcua_transport_endpoint::send_function;
    }
}



rx_protocol_result_t opcua_transport_endpoint::received_function (rx_protocol_stack_entry* reference,const protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
    opcua_transport_endpoint* self = reinterpret_cast<opcua_transport_endpoint*>(reference);
    self->received_func_((int64_t)rx_get_packet_available_data(buffer));
    return opcua_bin_bytes_received(reference, end_point, buffer);
}

void opcua_transport_endpoint::bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
    sent_func_ = sent_func;
    received_func_ = received_func;
}

rx_protocol_result_t opcua_transport_endpoint::send_function (rx_protocol_stack_entry* reference,const protocol_endpoint* end_point, rx_packet_buffer* buffer)
{
    opcua_transport_endpoint* self = reinterpret_cast<opcua_transport_endpoint*>(reference);
    self->sent_func_((int64_t)rx_get_packet_usable_data(buffer));
    return opcua_bin_bytes_send(reference, end_point, buffer);
}


// Class protocols::opcua::opcua_transport_port 

opcua_transport_port::opcua_transport_port()
{
    bind_port();
}



rx_protocol_stack_entry* opcua_transport_port::create_stack_entry ()
{
    return &endpoint_.protocol_stack_entry;
}

void opcua_transport_port::bind_port ()
{
    endpoint_.bind([this](int64_t count)
        {
            update_sent_counters(count);
        },
        [this](int64_t count)
        {
            update_received_counters(count);
        });
}


} // namespace opcua
} // namespace protocols

