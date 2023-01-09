

/****************************************************************************
*
*  interfaces\rx_endpoints.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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

#include "rx_configuration.h"
#include "system/runtime/rx_objbase.h"

// rx_endpoints
#include "interfaces/rx_endpoints.h"

#include "system/server/rx_server.h"
#include "system/runtime/rx_blocks.h"
#include "model/rx_meta_internals.h"
#include "rx_tcp_server.h"
#include "rx_udp.h"
#include "rx_serial.h"
#include "rx_ethernet.h"
#include "rx_inverter_ports.h"
#include "rx_tcp_client.h"
#include "sys_internal/rx_async_functions.h"
#include "terminal/rx_commands.h"
#include "rx_io_relations.h"
#include "system/runtime/rx_runtime_holder.h"
#include "system/runtime/rx_process_context.h"
#include "rx_port_instance.h"
#include "rx_port_stack_construction.h"
#include "rx_port_stack_passive.h"
#include "rx_port_stack_active.h"
#include "interfaces/rx_full_duplex_packet.h"
#include "interfaces/rx_transaction_limiter.h"
#include "interfaces/rx_stxetx.h"
#include "protocols/opcua/rx_opcua_resources.h"


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



rx_result rx_io_manager::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
	auto alias_result = host->read_config_files("rx-ip4network.yml");
	for (const auto& one : alias_result)
		for (const auto& alias : one)
			ip4_aliases_[alias.first] = alias.second;

	alias_result = host->read_config_files("rx-serial.yml");
	for (const auto& one : alias_result)
		for (const auto& alias : one)
			serial_aliases_[alias.first] = alias.second;

	alias_result = host->read_config_files("rx-ethernet.yml");
	for (const auto& one : alias_result)
		for (const auto& alias : one)
			ethernet_aliases_[alias.first] = alias.second;

	rx_result result = protocols::opcua::opcua_resources_repository::instance().initialize(host, data);

	if (result)
	{
        // register port stack relation
        result = model::platform_types_manager::instance().get_relations_repository().register_constructor(
            RX_NS_PORT_STACK_ID, [] {
                return rx_create_reference<port_stack_relation>();
            });

		result = model::platform_types_manager::instance().get_relations_repository().register_constructor(
			RX_NS_PORT_REF_ID, [] {
				return rx_create_reference<port_reference_relation>();
			});

        // register behavior creators

		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_behavior(
			RX_EXTERNAL_PORT_TYPE_ID, [] {
				rx_internal::sys_runtime::runtime_core::runtime_data::port_behaviors ret;
				ret.build_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_build::assemble_sender>();
				ret.passive_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_passive::listen_connect_subscriber>();
				ret.active_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_active::extern_behavior>();
				return ret;
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_behavior(
			RX_TRANSPORT_PORT_TYPE_ID, [] {
				rx_internal::sys_runtime::runtime_core::runtime_data::port_behaviors ret;
				ret.build_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_build::assemble_ignorant>();
				ret.passive_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_passive::passive_ignorant>();
				ret.active_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_active::passive_transport_behavior>();
				return ret;
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_behavior(
			RX_CONN_TRANSPORT_PORT_TYPE_ID, [] {
				rx_internal::sys_runtime::runtime_core::runtime_data::port_behaviors ret;
				ret.build_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_build::assemble_ignorant>();
				ret.passive_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_passive::passive_ignorant>();
				ret.active_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_active::connection_transport_behavior>();
				return ret;
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_behavior(
			RX_ROUTED_TRANSPORT_PORT_TYPE_ID, [] {
				rx_internal::sys_runtime::runtime_core::runtime_data::port_behaviors ret;
				ret.build_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_build::assemble_sender_subscriber>();
				ret.passive_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_passive::full_router>();
				ret.active_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_active::active_transport_behavior>();
				return ret;
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_behavior(
			RX_APPLICATION_PORT_TYPE_ID, [] {
				rx_internal::sys_runtime::runtime_core::runtime_data::port_behaviors ret;
				ret.build_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_build::assemble_subscriber>();
				ret.passive_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_passive::listen_connect_sender>();
				ret.active_behavior = std::make_unique<rx_internal::interfaces::port_stack::stack_active::application_behavior>();
				return ret;
			});

		// register I/O constructors
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_UDP_PORT_TYPE_ID, [] {
				return rx_create_reference<ip_endpoints::udp_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_SERIAL_PORT_TYPE_ID, [] {
				return rx_create_reference<serial::serial_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_ETHERNET_PORT_TYPE_ID, [] {
				return rx_create_reference<ethernet::ethernet_port>();
			});
        result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
            RX_TCP_SERVER_PORT_TYPE_ID, [] {
                return rx_create_reference<ip_endpoints::tcp_server_port>();
            });
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_TCP_HTTP_PORT_TYPE_ID, [] {
				return rx_create_reference<ip_endpoints::system_http_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_TCP_RX_PORT_TYPE_ID, [] {
				return rx_create_reference<ip_endpoints::system_rx_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
				RX_TCP_OPCUA_PORT_TYPE_ID, [] {
					return rx_create_reference<ip_endpoints::system_opcua_port>();
				});
        result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
            RX_TCP_CLIENT_PORT_TYPE_ID, [] {
                return rx_create_reference<ip_endpoints::tcp_client_port>();
            });
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_IP4_ROUTER_TYPE_ID, [] {
				return rx_create_reference<rx_internal::interfaces::ports_lib::ip4_routing_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_BYTE_ROUTER_TYPE_ID, [] {
				return rx_create_reference<rx_internal::interfaces::ports_lib::byte_routing_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_WORD_ROUTER_TYPE_ID, [] {
				return rx_create_reference<rx_internal::interfaces::ports_lib::word_routing_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_MAC_ROUTER_TYPE_ID, [] {
				return rx_create_reference<rx_internal::interfaces::ports_lib::mac_routing_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_TRANS_LIMITER_TYPE_ID, [] {
				return rx_create_reference<rx_internal::interfaces::ports_lib::transaction_limiter_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_LISTENER_TO_INITIATOR_PORT_TYPE_ID, [] {
				return rx_create_reference<rx_internal::interfaces::ports_lib::listener_to_initiator_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_INITIATOR_TO_LISTENER_PORT_TYPE_ID, [] {
				return rx_create_reference<rx_internal::interfaces::ports_lib::initiator_to_listener_port>();
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_STXETX_TYPE_ID, [] {
				return rx_create_reference<rx_internal::interfaces::ports_lib::stxetx_port>();
			});

        rx_internal::terminal::commands::server_command_manager::instance()->register_internal_commands();
	}
	return result;
}

void rx_io_manager::deinitialize ()
{
	rx_internal::terminal::commands::server_command_manager::instance()->clear();
}

rx_result rx_io_manager::start (hosting::rx_platform_host* host, const io_manager_data_t& data)
{
	return true;
}

void rx_io_manager::stop ()
{
}

string_type rx_io_manager::resolve_ip4_alias (const string_type& what) const
{
	auto it = ip4_aliases_.find(what);
	if (it != ip4_aliases_.end())
		return it->second;
	else
		return what;
}

string_type rx_io_manager::resolve_serial_alias (const string_type& what) const
{
	auto it = serial_aliases_.find(what);
	if (it != serial_aliases_.end())
		return it->second;
	else
		return what;
}

string_type rx_io_manager::resolve_ethernet_alias (const string_type& what) const
{
	auto it = ethernet_aliases_.find(what);
	if (it != ethernet_aliases_.end())
		return it->second;
	else
		return what;
}


} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal

