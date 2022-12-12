

/****************************************************************************
*
*  first_plugin\ether_test.cpp
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

#include "first_plugin_version.h"
#include "version/rx_code_macros.h"

// ether_test
#include "first_plugin/ether_test.h"



namespace ether {

// Class ether::ether_subscriber1_endpoint 

ether_subscriber1_endpoint::ether_subscriber1_endpoint (ether_subscriber1_port* port)
      : port_(port)
{
	RX_PLUGIN_LOG_DEBUG("ether_subscriber1_endpoint", 100, _rx_func_);
	rx_init_stack_entry(&stack_, this);
	stack_.received_function = [](rx_protocol_stack_endpoint* entry, recv_protocol_packet packet)->rx_protocol_result_t
	{
		ether_subscriber1_endpoint* whose = reinterpret_cast<ether_subscriber1_endpoint*>(entry->user_data);
		return whose->received(packet);
	};
}


ether_subscriber1_endpoint::~ether_subscriber1_endpoint()
{
	RX_PLUGIN_LOG_DEBUG("ether_subscriber1_endpoint", 100, _rx_func_);
}



rx_protocol_stack_endpoint* ether_subscriber1_endpoint::get_endpoint ()
{
	return &stack_;
}

void ether_subscriber1_endpoint::close_endpoint ()
{
}

rx_protocol_result_t ether_subscriber1_endpoint::received (recv_protocol_packet packet)
{
	return RX_PROTOCOL_OK;
}

void ether_subscriber1_endpoint::send_command (const string_type& val)
{
}


ether_subscriber1_port * ether_subscriber1_endpoint::get_port ()
{
  return port_;
}


// Class ether::ether_subscriber1_port 

ether_subscriber1_port::ether_subscriber1_port()
{
}


ether_subscriber1_port::~ether_subscriber1_port()
{
}



rx_result ether_subscriber1_port::initialize_port (rx_platform_api::rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("ether_subscriber1_port", 100, _rx_func_);
	return true;
}

rx_result ether_subscriber1_port::start_port (rx_platform_api::rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("ether_subscriber1_port", 100, _rx_func_);
	return true;
}

rx_result ether_subscriber1_port::stop_port ()
{
	RX_PLUGIN_LOG_DEBUG("ether_subscriber1_port", 100, _rx_func_);
	return true;
}

rx_result ether_subscriber1_port::deinitialize_port ()
{
	RX_PLUGIN_LOG_DEBUG("ether_subscriber1_port", 100, _rx_func_);
	return true;
}

rx_reference<ether_subscriber1_endpoint> ether_subscriber1_port::construct_endpoint ()
{
	return rx_create_reference<ether_subscriber1_endpoint>(this);
}


} // namespace ether

