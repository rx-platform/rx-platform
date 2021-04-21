

/****************************************************************************
*
*  interfaces\rx_endpoints.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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
#include "rx_tcp_client.h"
#include "sys_internal/rx_async_functions.h"
#include "terminal/rx_commands.h"
#include "rx_io_relations.h"
#include "system/runtime/rx_runtime_holder.h"
#include "system/runtime/rx_process_context.h"
#include "system/runtime/rx_port_instance.h"
#include "system/runtime/rx_port_stack_construction.h"
#include "system/runtime/rx_port_stack_passive.h"
#include "system/runtime/rx_port_stack_active.h"


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
        // register port stack relation
        result = model::platform_types_manager::instance().get_relations_repository().register_constructor(
            RX_NS_PORT_STACK_ID, [] {
                return rx_create_reference<port_stack_relation>();
            });

        // register behavior creators

		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_behavior(
			RX_EXTERNAL_PORT_TYPE_ID, [] {
				runtime::items::port_behaviors ret;
				ret.build_behavior = std::make_unique<runtime::io_types::stack_build::assemble_sender>();
				ret.passive_behavior = std::make_unique<runtime::io_types::stack_passive::listen_connect_subscriber>();
				ret.active_behavior = std::make_unique<runtime::io_types::stack_active::extern_behavior>();
				return ret;
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_behavior(
			RX_TRANSPORT_PORT_TYPE_ID, [] {
				runtime::items::port_behaviors ret;
				ret.build_behavior = std::make_unique<runtime::io_types::stack_build::assemble_ignorant>();
				ret.passive_behavior = std::make_unique<runtime::io_types::stack_passive::passive_ignorant>();
				ret.active_behavior = std::make_unique<runtime::io_types::stack_active::passive_transport_behavior>();
				return ret;
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_behavior(
			RX_ROUTED_TRANSPORT_PORT_TYPE_ID, [] {
				runtime::items::port_behaviors ret;
				ret.build_behavior = std::make_unique<runtime::io_types::stack_build::assemble_sender_subscriber>();
				ret.passive_behavior = std::make_unique<runtime::io_types::stack_passive::full_router>();
				ret.active_behavior = std::make_unique<runtime::io_types::stack_active::active_transport_behavior>();
				return ret;
			});
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_behavior(
			RX_APPLICATION_PORT_TYPE_ID, [] {
				runtime::items::port_behaviors ret;
				ret.build_behavior = std::make_unique<runtime::io_types::stack_build::assemble_subscriber>();
				ret.passive_behavior = std::make_unique<runtime::io_types::stack_passive::listen_connect_sender>();
				ret.active_behavior = std::make_unique<runtime::io_types::stack_active::application_behavior>();
				return ret;
			});

		// register I/O constructors
		result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
			RX_UDP_PORT_TYPE_ID, [] {
				return rx_create_reference<ip_endpoints::udp_port>();
			});
        result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
            RX_TCP_SERVER_PORT_TYPE_ID, [] {
                return rx_create_reference<ip_endpoints::tcp_server_port>();
            });
        result = model::platform_types_manager::instance().get_type_repository<port_type>().register_constructor(
            RX_TCP_CLIENT_PORT_TYPE_ID, [] {
                return rx_create_reference<ip_endpoints::tcp_client_port>();
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


} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal

