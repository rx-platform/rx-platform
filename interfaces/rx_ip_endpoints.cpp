

/****************************************************************************
*
*  interfaces\rx_ip_endpoints.cpp
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


// rx_ip_endpoints
#include "interfaces/rx_ip_endpoints.h"



namespace interfaces {

namespace ip_endpoints {

// Class interfaces::ip_endpoints::rx_udp_endpoint 

rx_udp_endpoint::rx_udp_endpoint()
{
}



rx_result rx_udp_endpoint::open (const string_type& addr, uint16_t port)
{
	return "Nisam jos implementirao";
}

rx_protocol_result_t rx_udp_endpoint::send_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_result rx_udp_endpoint::close ()
{
	return "Nisam jos implementirao";
}


// Class interfaces::ip_endpoints::udp_port 

udp_port::udp_port()
{
}



rx_result udp_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = physical_port::initialize_runtime(ctx);
	if (result)
	{		
	}
	return result;
}

rx_result udp_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	rx_result result;

	result = physical_port::deinitialize_runtime(ctx);
	return result;
}

rx_result udp_port::start_runtime (runtime::runtime_start_context& ctx)
{
	auto result = physical_port::start_runtime(ctx);
	if (result)
	{
		udp_socket_ = rx_create_reference<socket_holder_t>(this);
		string_type addr = ctx.structure.get_root()->get_local_as<string_type>("Bind.IPAddress", "");
		uint16_t port = ctx.structure.get_root()->get_local_as<uint16_t>("Bind.IPPort", 0);
		if (!udp_socket_->bind_socket_tcpip_4(port, addr, rx_gate::instance().get_infrastructure().get_io_pool()->get_pool()))
		{
			result.register_error("Unable to bind socket");
		}
	}
	return result;
}

rx_result udp_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
	rx_result result;
	udp_socket_->close();
	udp_socket_->disconnect();
	udp_socket_ = rx_reference<socket_holder_t>::null_ptr;
	result = physical_port::stop_runtime(ctx);
	return result;
}


} // namespace ip_endpoints
} // namespace interfaces

