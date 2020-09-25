

/****************************************************************************
*
*  system\runtime\rx_port_stack_active.cpp
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


// rx_port_stack_active
#include "system/runtime/rx_port_stack_active.h"

#include "sys_internal/rx_async_functions.h"
#include "rx_runtime_holder.h"


namespace rx_platform {

namespace runtime {

namespace io_types {

// Class rx_platform::runtime::io_types::active_builder 


rx_result active_builder::bind_stack_endpoint (rx_port_ptr who, rx_protocol_stack_endpoint* what, const io::any_address& local_addr, const io::any_address& remote_addr)
{
	RX_ASSERT(who);
	if (who)
	{
		auto upper_port = who->get_instance_data().stack_data.passive_map.get_binded_port(&local_addr, &remote_addr);
		if (upper_port)
		{
			auto ep = upper_port->get_implementation()->create_endpoint();
			if (ep)
			{
				auto result = rx_push_stack(what, ep);
				if (result == RX_PROTOCOL_OK)
				{
					if (who->get_instance_data().behavior.active_behavior
						&& who->get_instance_data().behavior.active_behavior->is_endpoit_binder())
					{
						security::secured_scope _(what->identity);
						auto session = rx_create_session(&local_addr, &remote_addr, 0, 0, nullptr);
						rx_notify_connected(what, &session);
					}
					return true;
				}
				else
					return rx_protocol_error_message(result);
			}
			else
				return "Upper level port not created endpoint!";
		}
		else
			return "Nobody listens here!";
	}
	else
		return "E jebi ga!!!";
}


// Class rx_platform::runtime::io_types::extern_behavior 


bool extern_behavior::is_extern ()
{
	return true;
}

bool extern_behavior::is_endpoit_binder ()
{
	return true;
}


// Class rx_platform::runtime::io_types::passive_transport_behavior 


bool passive_transport_behavior::is_extern ()
{
	return false;
}

bool passive_transport_behavior::is_endpoit_binder ()
{
	return false;
}


// Class rx_platform::runtime::io_types::active_transport_behavior 


bool active_transport_behavior::is_extern ()
{
	return false;
}

bool active_transport_behavior::is_endpoit_binder ()
{
	return true;
}


// Class rx_platform::runtime::io_types::application_behavior 


bool application_behavior::is_extern ()
{
	return false;
}

bool application_behavior::is_endpoit_binder ()
{
	return false;
}


} // namespace io_types
} // namespace runtime
} // namespace rx_platform

