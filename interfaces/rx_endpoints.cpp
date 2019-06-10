

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
#include "model/rx_meta_internals.h"
#include "rx_ip_endpoints.h"


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
	auto result_c = rx_init_protocols(nullptr);
	rx_result result = result_c == RX_PROTOCOL_OK ? rx_result(true) : rx_result(rx_get_error_text(result_c));
	if (result)
	{
		// register I/O constructors
		model::platform_types_manager::instance().internal_get_type_cache<port_type>().register_constructor(
			RX_UDP_PORT_TYPE_ID, [] {
				return rx_create_reference<ip_endpoints::udp_port>();
			});
	}
	return result;
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
      : my_endpoints_(nullptr),
        rx_bytes_item_(0),
        tx_bytes_item_(0)
{
}



rx_result physical_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = port_runtime::initialize_runtime(ctx);
	if (result)
	{
		auto bind_result = ctx.tags->bind_item("Status.RxBytes", ctx);
		if (bind_result)
			rx_bytes_item_ = bind_result.value();
		else
			RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.RxBytes");

		bind_result = ctx.tags->bind_item("Status.TxBytes", ctx);
		if (bind_result)
			tx_bytes_item_ = bind_result.value();
		else
			RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Status.TxBytes");
	}
	return result;
}

void physical_port::update_received_counters (size_t count)
{
	if (rx_bytes_item_)
	{
		auto current = get_runtime().get_binded_as<int64_t>(rx_bytes_item_, 0);
		current += count;
		get_runtime().set_binded_as<int64_t>(rx_bytes_item_, std::move(current));
	}
	update_received_packets(1);
}


} // namespace io_endpoints
} // namespace interfaces

