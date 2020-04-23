

/****************************************************************************
*
*  interfaces\rx_endpoints.cpp
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

#include "rx_configuration.h"
#include "system/runtime/rx_objbase.h"

// rx_endpoints
#include "interfaces/rx_endpoints.h"

#include "system/server/rx_server.h"
#include "system/runtime/rx_blocks.h"
#include "model/rx_meta_internals.h"
#include "rx_ip_endpoints.h"
#include "sys_internal/rx_async_functions.h"
#include "terminal/rx_commands.h"


namespace rx_internal {

namespace interfaces {

namespace io_endpoints {

// Class rx_internal::interfaces::io_endpoints::rx_io_manager 

rx_io_manager::rx_io_manager()
{
}


rx_io_manager::~rx_io_manager()
{
}



rx_result rx_io_manager::initialize (hosting::rx_platform_host* host, io_manager_data_t& data)
{
	auto result_c = rx_init_protocols(nullptr);
	rx_result result = result_c == RX_PROTOCOL_OK ? rx_result(true) : rx_result(rx_get_error_text(result_c));
	if (result)
	{
		// register I/O constructors
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_UDP_PORT_TYPE_ID, [] {
				return rx_create_reference<ip_endpoints::udp_port>();
			});
        /*if (!result)
        {
            
        }*/
        // register I/O constructors
        result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
            RX_TCP_SERVER_PORT_TYPE_ID, [] {
                return rx_create_reference<ip_endpoints::tcp_server_port>();
            });

        rx_internal::terminal::commands::server_command_manager::instance()->register_internal_commands();
	}
	return result;
}

void rx_io_manager::deinitialize ()
{
	rx_deinit_protocols();
}

rx_result rx_io_manager::start (hosting::rx_platform_host* host, const io_manager_data_t& data)
{
	return true;
}

void rx_io_manager::stop ()
{
}


// Class rx_internal::interfaces::io_endpoints::rx_io_endpoint 

rx_io_endpoint::rx_io_endpoint()
{
}


rx_io_endpoint::~rx_io_endpoint()
{
}



} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal

