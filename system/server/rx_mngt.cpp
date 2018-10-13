

/****************************************************************************
*
*  system\server\rx_mngt.cpp
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


// rx_commands
#include "terminal/rx_commands.h"
// rx_mngt
#include "system/server/rx_mngt.h"

#include "lib/rx_lib.h"
#include "system/server/rx_server.h"
#include "terminal/rx_telnet.h"
#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_internal_objects.h"
#include "classes/rx_meta.h"

using namespace rx_platform;
using namespace terminal::commands;
using namespace rx;


namespace rx_platform {

namespace mngt {

// Class rx_platform::mngt::server_manager 

server_manager::server_manager()
      : telnet_port_(0)
{
	commands_manager_ = rx_create_reference<server_command_manager>();
}


server_manager::~server_manager()
{
}



void server_manager::virtual_bind ()
{
}

void server_manager::virtual_release ()
{
}

uint32_t server_manager::initialize (hosting::rx_platform_host* host, managment_data_t& data)
{
	data.manager_internal_data = new mngt::manager_initialization_context;
	telnet_port_ = data.telnet_port;
	commands_manager_.cast_to<server_command_manager::smart_ptr>()->register_internal_commands();
	unassigned_domain_ = rx_create_reference<sys_internal::sys_objects::unssigned_domain>();
	unassigned_app_ = rx_create_reference<sys_internal::sys_objects::unassigned_application>();
	system_app_ = rx_create_reference<sys_internal::sys_objects::system_application>();
	system_domain_ = rx_create_reference<sys_internal::sys_objects::system_domain>();
	return RX_OK;
}

uint32_t server_manager::deinitialize ()
{
	return RX_OK;
}

uint32_t server_manager::start (hosting::rx_platform_host* host, const managment_data_t& data)
{
	if (telnet_port_)
	{
		telnet_listener_ = rx_create_reference<terminal::console::server_telnet_socket>();
		telnet_listener_->start_tcpip_4(rx_gate::instance().get_runtime().get_io_pool()->get_pool(), telnet_port_);
	}
	for (auto& one : data.manager_internal_data->get_to_register())
	{
		commands_manager_.cast_to<server_command_manager::smart_ptr>()->register_command(one);
	}
	return RX_OK;
}

uint32_t server_manager::stop ()
{
	if (telnet_listener_)
	{
		telnet_listener_->stop();
		telnet_listener_ = terminal::console::server_telnet_socket::smart_ptr::null_ptr;
	}
	return RX_OK;
}

void server_manager::get_directories (server_directories_type& dirs)
{
}

meta::object_class_ptr server_manager::get_object_class (const rx_node_id& id)
{
	auto& cache = model::internal_classes_manager::instance().get_type_cache<meta::object_defs::object_class>();
	auto def = cache.get_class_definition(id);
	//model::relations_hash_data data;
	return def;
}

template<class clsT>
typename clsT::smart_ptr server_manager::get_class(const rx_node_id& id)
{
	auto& cache = model::internal_classes_manager::instance().get_type_cache<clsT>();
	auto def = cache.get_class_definition(id);
	return def;
}
template object_class_ptr server_manager::get_class<object_class>(const rx_node_id& id);


template struct_class_ptr server_manager::get_class<struct_class>(const rx_node_id& id);
// Class rx_platform::mngt::manager_initialization_context 

manager_initialization_context::manager_initialization_context()
{
}

manager_initialization_context::manager_initialization_context(const manager_initialization_context &right)
{
	RX_ASSERT(false);
}


manager_initialization_context::~manager_initialization_context()
{
}


manager_initialization_context & manager_initialization_context::operator=(const manager_initialization_context &right)
{
	RX_ASSERT(false);
	return *this;
}



void manager_initialization_context::register_command (server_command_base_ptr cmd)
{
	to_register_.emplace_back(cmd);
}


} // namespace mngt
} // namespace rx_platform

