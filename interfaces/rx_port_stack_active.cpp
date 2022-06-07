

/****************************************************************************
*
*  interfaces\rx_port_stack_active.cpp
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


// rx_port_stack_active
#include "interfaces/rx_port_stack_active.h"

#include "sys_internal/rx_async_functions.h"
#include "system/runtime/rx_runtime_holder.h"
#include "interfaces/rx_endpoints.h"

namespace rx_platform
{
rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();
}


namespace rx_internal {

namespace interfaces {

namespace port_stack {

namespace stack_active {

// Class rx_internal::interfaces::port_stack::stack_active::active_builder 


rx_result active_builder::bind_stack_endpoint (rx_port_ptr who, rx_protocol_stack_endpoint* what, const io::any_address& local_addr, const io::any_address& remote_addr)
{
	if (who->get_instance_data().behavior.active_behavior->is_endpoint_user())
	{
		security::secured_scope _(what->identity);

		auto ep = who->get_implementation()->construct_listener_endpoint(&local_addr, &remote_addr);

		if (ep)
		{
			auto result = rx_push_stack(what, ep);
			if (result == RX_PROTOCOL_OK)
			{
				auto reg_result = who->get_instance_data().stack_data.active_map.register_endpoint(ep, rx_port_ptr::null_ptr, who);
				if (reg_result)
				{
					std::ostringstream ss;
					ss << "Port "
						<< who->meta_info().get_full_path()
						<< " accepted connection ["
						<< local_addr.to_string()
						<< ","
						<< remote_addr.to_string()
						<< "] as "
						<< security::active_security()->get_full_name();

					ITF_LOG_INFO("active_builder", 900, ss.str());
				}
				else
				{
					std::ostringstream ss;
					ss << "Port "
						<< who->meta_info().get_full_path()
						<< " error binding at ["
						<< local_addr.to_string()
						<< ","
						<< remote_addr.to_string()
						<< "]";
					rx_dump_error_result(ss, reg_result);
					ITF_LOG_ERROR("active_builder", 500, ss.str());
					who->get_implementation()->destroy_endpoint(what);
					RX_ASSERT(false);
					return reg_result;
				}
				return true;
			}
			else
			{
				return rx_protocol_error_message(result);
			}
		}
		else
			return "Error constructing endpoint!";
	}
	else
	{
		auto upper_port = who->get_instance_data().stack_data.passive_map.get_binded_port(&local_addr, &remote_addr);
		if (upper_port)
		{
			security::secured_scope _(what->identity);

			auto ep = who->get_implementation()->construct_listener_endpoint(&local_addr, &remote_addr);

			if (ep)
			{
				auto result = rx_push_stack(what, ep);
				if (result == RX_PROTOCOL_OK)
				{
					auto reg_result = bind_stack_endpoint(upper_port, ep, local_addr, remote_addr);
					if (reg_result)
					{
						reg_result = who->get_instance_data().stack_data.active_map.register_endpoint(ep, upper_port, who);
						if (reg_result)
						{
							std::ostringstream ss;
							ss << "Port "
								<< who->meta_info().get_full_path()
								<< " accepted connection ["
								<< local_addr.to_string()
								<< ","
								<< remote_addr.to_string()
								<< "] as "
								<< security::active_security()->get_full_name();

							if (who->get_instance_data().behavior.active_behavior
								&& who->get_instance_data().behavior.active_behavior->is_endpoit_binder())
							{
								ITF_LOG_INFO("active_builder", 900, ss.str());
								auto session = rx_create_session(&local_addr, &remote_addr, 0, 0, nullptr);
								rx_notify_connected(what, &session);
							}
							else
							{
								ITF_LOG_TRACE("active_builder", 900, ss.str());
							}
						}
						else
						{
							std::ostringstream ss;
							ss << "Port "
								<< who->meta_info().get_full_path()
								<< " error binding at ["
								<< local_addr.to_string()
								<< ","
								<< remote_addr.to_string()
								<< "]";
							rx_dump_error_result(ss, reg_result);
							ITF_LOG_ERROR("active_builder", 500, ss.str());
							who->get_implementation()->destroy_endpoint(what);
							RX_ASSERT(false);
							return reg_result;
						}
					}
					else
					{
						who->get_implementation()->destroy_endpoint(ep);
						return reg_result;
					}
					return true;
				}
				else
				{
					return rx_protocol_error_message(result);
				}
			}
			else
				return "Error constructing endpoint!";
		}
		else
			return "Nobody listens here!";
	}
}

rx_result active_builder::add_stack_endpoint (rx_port_ptr who, rx_protocol_stack_endpoint* what, const io::any_address& local_addr, const io::any_address& remote_addr)
{
	auto upper_port = who->get_instance_data().stack_data.passive_map.get_binded_port(&local_addr, &remote_addr);
	if (upper_port)
	{
		security::secured_scope _(what->identity);

		auto reg_result = bind_stack_endpoint(upper_port, what, local_addr, remote_addr);
		if (reg_result)
		{
			reg_result = who->get_instance_data().stack_data.active_map.register_endpoint(what, upper_port, who);
			if (reg_result)
			{
				std::ostringstream ss;
				ss << "Port "
					<< who->meta_info().get_full_path()
					<< " accepted connection ["
					<< local_addr.to_string()
					<< ","
					<< remote_addr.to_string()
					<< "] as "
					<< security::active_security()->get_full_name();

				if (who->get_instance_data().behavior.active_behavior
					&& who->get_instance_data().behavior.active_behavior->is_endpoit_binder())
				{
					ITF_LOG_INFO("active_builder", 900, ss.str());
					auto session = rx_create_session(&local_addr, &remote_addr, 0, 0, nullptr);
					rx_notify_connected(what, &session);
				}
				else
				{
					ITF_LOG_TRACE("active_builder", 900, ss.str());
				}
			}
			else
			{
				std::ostringstream ss;
				ss << "Port "
					<< who->meta_info().get_full_path()
					<< " error binding at ["
					<< local_addr.to_string()
					<< ","
					<< remote_addr.to_string()
					<< "]";
				rx_dump_error_result(ss, reg_result);
				ITF_LOG_ERROR("active_builder", 500, ss.str());
				who->get_implementation()->destroy_endpoint(what);
				RX_ASSERT(false);
				return reg_result;
			}
		}
		else
		{
			std::ostringstream ss;
			ss << "Port "
				<< who->meta_info().get_full_path()
				<< " error binding at ["
				<< local_addr.to_string()
				<< ","
				<< remote_addr.to_string()
				<< "]";
			rx_dump_error_result(ss, reg_result);
			ITF_LOG_ERROR("active_builder", 500, ss.str());
			who->get_implementation()->destroy_endpoint(what);
			return reg_result;
		}
		return true;

	}
	else
	{
		return "Nobody listens here!";
	}
}

rx_result active_builder::register_routing_endpoint (rx_port_ptr who, rx_protocol_stack_endpoint* what)
{
	auto reg_result = who->get_instance_data().stack_data.active_map.register_endpoint(what, who, who);
	if (reg_result)
	{
		std::ostringstream ss;
		ss << "Port "
			<< who->meta_info().get_full_path()
			<< " registered routing endpoint.";
		ITF_LOG_TRACE("active_builder", 900, ss.str());
		return true;
	}
	else
	{
		std::ostringstream ss;
		ss << "Port "
			<< who->meta_info().get_full_path()
			<< " error registering routing endpoint.";
		rx_dump_error_result(ss, reg_result);
		ITF_LOG_ERROR("active_builder", 500, ss.str());
		who->get_implementation()->destroy_endpoint(what);
		RX_ASSERT(false);
		return reg_result;
	}
}

rx_result active_builder::unbind_stack_endpoint (rx_port_ptr who, rx_protocol_stack_endpoint* what)
{
	auto result = who->get_instance_data().stack_data.active_map.unregister_endpoint(what, who);
	if (result)
		who->get_implementation()->destroy_endpoint(what);
	return result;
}

void active_builder::close_all_endpoints (rx_port_ptr who)
{
	return who->get_instance_data().stack_data.active_map.close_all_endpoints();
}

rx_result active_builder::register_connection_endpoints (rx_port_ptr who, rx_protocol_stack_endpoint* what, rx_port_ptr upper_port, rx_protocol_stack_endpoint* upper, const io::any_address& local_addr, const io::any_address& remote_addr)
{
	security::secured_scope _(what->identity);

	auto reg_result = who->get_instance_data().stack_data.active_map.register_endpoint(what, upper_port, who);
	if (reg_result)
	{
		std::ostringstream ss;
		ss << "Port "
			<< who->meta_info().get_full_path()
			<< " connecting ["
			<< local_addr.to_string()
			<< ","
			<< remote_addr.to_string()
			<< "] as "
			<< security::active_security()->get_full_name();
			ITF_LOG_TRACE("active_builder", 900, ss.str());
	}
	else
	{
		std::ostringstream ss;
		ss << "Port "
			<< who->meta_info().get_full_path()
			<< " error binding at ["
			<< local_addr.to_string()
			<< ","
			<< remote_addr.to_string()
			<< "]";
		rx_dump_error_result(ss, reg_result);
		ITF_LOG_ERROR("active_builder", 500, ss.str());
		RX_ASSERT(false);
		return reg_result;
	}
	if (upper)
	{
		auto result = rx_push_stack(what, upper);
		if (result != RX_PROTOCOL_OK)
			return rx_protocol_error_message(result);
	}

	return true;
}

rx_result active_builder::disconnect_stack_endpoint (rx_port_ptr who, rx_protocol_stack_endpoint* what)
{
	auto result = who->get_instance_data().stack_data.active_map.unregister_endpoint(what, who);
	if (result)
		who->get_implementation()->destroy_endpoint(what);
	return result;
}


// Class rx_internal::interfaces::port_stack::stack_active::active_transport_behavior 


bool active_transport_behavior::is_extern ()
{
	return false;
}

bool active_transport_behavior::is_endpoit_binder ()
{
	return true;
}

bool active_transport_behavior::is_endpoint_user ()
{
	return true;
}


// Class rx_internal::interfaces::port_stack::stack_active::application_behavior 


bool application_behavior::is_extern ()
{
	return false;
}

bool application_behavior::is_endpoit_binder ()
{
	return false;
}

bool application_behavior::is_endpoint_user ()
{
	return true;
}


// Class rx_internal::interfaces::port_stack::stack_active::passive_transport_behavior 


bool passive_transport_behavior::is_extern ()
{
	return false;
}

bool passive_transport_behavior::is_endpoit_binder ()
{
	return false;
}

bool passive_transport_behavior::is_endpoint_user ()
{
	return false;
}


// Class rx_internal::interfaces::port_stack::stack_active::extern_behavior 


bool extern_behavior::is_extern ()
{
	return true;
}

bool extern_behavior::is_endpoit_binder ()
{
	return true;
}

bool extern_behavior::is_endpoint_user ()
{
	return false;
}


} // namespace stack_active
} // namespace port_stack
} // namespace interfaces
} // namespace rx_internal

