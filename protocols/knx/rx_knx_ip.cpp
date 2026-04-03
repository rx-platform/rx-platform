

/****************************************************************************
*
*  protocols\knx\rx_knx_ip.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


// rx_knx_ip
#include "protocols/knx/rx_knx_ip.h"



namespace protocols {

namespace knx {

// Class protocols::knx::knx_ip_port 

std::map<rx_node_id, knx_ip_port::smart_ptr> knx_ip_port::runtime_instances;

knx_ip_port::knx_ip_port()
      : keep_alive(0),
        connected_(false)
{
}


knx_ip_port::~knx_ip_port()
{
}



void knx_ip_port::stack_assembled ()
{
}

rx_result knx_ip_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result knx_ip_port::start_runtime (runtime::runtime_start_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result knx_ip_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

void knx_ip_port::register_source (const string_type& addr, knx_ip_source_ptr what)
{
	RX_ASSERT(false); // Not implemented
}

void knx_ip_port::unregister_source (const string_type& addr, knx_ip_source_ptr what)
{
    RX_ASSERT(false); // Not implemented
}

void knx_ip_port::connected ()
{
    RX_ASSERT(false); // Not implemented
}

void knx_ip_port::disconnected ()
{
    RX_ASSERT(false); // Not implemented
}


// Class protocols::knx::knx_ip_endpoint 

knx_ip_endpoint::knx_ip_endpoint (knx_ip_port_ptr port, rx_timer_ptr timer)
      : port_(port)
{
}


knx_ip_endpoint::~knx_ip_endpoint()
{
}



rx_protocol_stack_endpoint* knx_ip_endpoint::bind_endpoint ()
{
    return &stack_;
}

void knx_ip_endpoint::close_endpoint ()
{
	RX_ASSERT(false); // Not implemented
}

rx_port_impl_ptr knx_ip_endpoint::get_port ()
{
	return port_;
}

rx_protocol_result_t knx_ip_endpoint::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t knx_ip_endpoint::disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t knx_ip_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

void knx_ip_endpoint::timer_fired ()
{
    printf("$$Wery anoying stuff!!!\r\n");
}

rx_protocol_result_t knx_ip_endpoint::received_packet (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}


// Class protocols::knx::knx_ip_source 

knx_ip_source::knx_ip_source()
{
}


knx_ip_source::~knx_ip_source()
{
}



void knx_ip_source::port_connected (knx_ip_port_ptr port)
{
}

void knx_ip_source::port_disconnected (knx_ip_port_ptr port)
{
}


} // namespace knx
} // namespace protocols

