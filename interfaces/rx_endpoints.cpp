

/****************************************************************************
*
*  interfaces\rx_endpoints.cpp
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

#include "rx_configuration.h"
#include "system/runtime/rx_objbase.h"

// rx_endpoints
#include "interfaces/rx_endpoints.h"

#include "system/server/rx_server.h"
#include "system/runtime/rx_blocks.h"


namespace interfaces {

namespace io_endpoints {

// Class interfaces::io_endpoints::rx_io_manager 

rx_io_manager::rx_io_manager()
{
}


rx_io_manager::~rx_io_manager()
{
}



rx_result rx_io_manager::initialize (hosting::rx_platform_host* host, io_manager_data_t& data)
{
	auto result = rx_init_protocols(nullptr);
	return result == RX_PROTOCOL_OK ? rx_result(true) : rx_result(rx_get_error_text(result));
}

rx_result rx_io_manager::deinitialize ()
{
	auto result = rx_deinit_protocols();
	return result == RX_PROTOCOL_OK ? rx_result(true) : rx_result(rx_get_error_text(result));
}

rx_result rx_io_manager::start (hosting::rx_platform_host* host, const io_manager_data_t& data)
{
	return true;
}

rx_result rx_io_manager::stop ()
{
	return true;
}


// Class interfaces::io_endpoints::rx_io_endpoint 

rx_io_endpoint::rx_io_endpoint()
{
}


rx_io_endpoint::~rx_io_endpoint()
{
}



// Class interfaces::io_endpoints::physical_port 

physical_port::physical_port()
      : my_endpoints_(nullptr)
{
}



} // namespace io_endpoints
} // namespace interfaces


