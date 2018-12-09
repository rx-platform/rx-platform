

/****************************************************************************
*
*  system\server\rx_endpoints.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"

#include "rx_configuration.h"

// rx_endpoints
#include "system/server/rx_endpoints.h"

#include "system/server/rx_server.h"


namespace rx_platform {

namespace io_endpoints {

// Class rx_platform::io_endpoints::rx_io_manager 

rx_io_manager::rx_io_manager()
	: server_object(objects::object_creation_data{ RX_IO_MANAGER_NAME, RX_IO_MANAGER_ID, RX_IO_MANAGER_TYPE_ID, true,  objects::application_runtime_ptr::null_ptr, objects::domain_runtime_ptr::null_ptr })
{
}


rx_io_manager::~rx_io_manager()
{
}



uint32_t rx_io_manager::initialize (hosting::rx_platform_host* host, io_manager_data_t& data)
{
	return RX_OK;
}

uint32_t rx_io_manager::deinitialize ()
{
	return RX_OK;
}

uint32_t rx_io_manager::start (hosting::rx_platform_host* host, const io_manager_data_t& data)
{
	return RX_OK;
}

uint32_t rx_io_manager::stop ()
{
	return RX_OK;
}


// Class rx_platform::io_endpoints::rx_io_endpoint 

rx_io_endpoint::rx_io_endpoint()
{
}


rx_io_endpoint::~rx_io_endpoint()
{
}



} // namespace io_endpoints
} // namespace rx_platform
