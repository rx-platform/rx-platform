

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
	: server_object(runtime::objects::object_creation_data{
	RX_IO_MANAGER_NAME
	, RX_IO_MANAGER_ID
	, RX_IO_MANAGER_TYPE_ID
	, true
	, RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_OBJ_NAME RX_DIR_DELIMETER_STR RX_NS_SYSTEM_OBJ_NAME RX_DIR_DELIMETER_STR RX_IO_MANAGER_NAME
	, rx_application_ptr::null_ptr
	, rx_domain_ptr::null_ptr 
	})
{
}


rx_io_manager::~rx_io_manager()
{
}



uint32_t rx_io_manager::initialize (hosting::rx_platform_host* host, io_manager_data_t& data)
{
	auto result = rx_init_protocols(nullptr);
	return RX_OK;
}

uint32_t rx_io_manager::deinitialize ()
{
	auto result = rx_deinit_protocols();
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


// Class interfaces::io_endpoints::physical_port 

physical_port::physical_port (objects::port_creation_data&& data)
      : my_endpoints_(nullptr)
	, port_runtime(std::move(data))
{
}



// Class interfaces::io_endpoints::rx_io_endpoint 

rx_io_endpoint::rx_io_endpoint()
{
}


rx_io_endpoint::~rx_io_endpoint()
{
}



// Class interfaces::io_endpoints::rx_io_buffer 

rx_io_buffer::rx_io_buffer()
{
	memzero(this, sizeof(rx_packet_buffer));
}

rx_io_buffer::rx_io_buffer (size_t initial_capacity, rx_protocol_stack_entry* stack_entry)
{
	auto result = rx_init_packet_buffer(this, initial_capacity, stack_entry);
	if (result != RX_PROTOCOL_OK)
	{
		memzero(this, sizeof(rx_packet_buffer));
	}
}


rx_io_buffer::~rx_io_buffer()
{
	if (buffer_ptr)
		rx_deinit_packet_buffer(this);
}



bool rx_io_buffer::valid () const
{
	return (buffer_ptr != nullptr);
}

void rx_io_buffer::attach (rx_packet_buffer* buffer)
{
	if (buffer_ptr)
		rx_deinit_packet_buffer(this);
	if (buffer)
		memcpy(this, buffer, sizeof(rx_packet_buffer));
	else
		memzero(this, sizeof(rx_packet_buffer));
}

void rx_io_buffer::detach (rx_packet_buffer* buffer)
{
	if (buffer)
		memcpy(buffer, this, sizeof(rx_packet_buffer));
	memzero(this, sizeof(rx_packet_buffer));
}

void rx_io_buffer::release ()
{
	if (buffer_ptr)
		rx_deinit_packet_buffer(this);
	memzero(this, sizeof(rx_packet_buffer));
}

rx_io_buffer::rx_io_buffer(rx_io_buffer&& right) noexcept
{
	if(right.buffer_ptr)
		memcpy(this, &right, sizeof(rx_packet_buffer));
	else
		memzero(this, sizeof(rx_packet_buffer));
}
rx_io_buffer& rx_io_buffer::operator=(rx_io_buffer&& right) noexcept
{
	if (buffer_ptr)
		rx_deinit_packet_buffer(this);
	if (right.buffer_ptr)
		memcpy(this, &right, sizeof(rx_packet_buffer));
	else
		memzero(this, sizeof(rx_packet_buffer));

	return *this;
}
} // namespace io_endpoints
} // namespace interfaces

