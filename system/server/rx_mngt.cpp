

/****************************************************************************
*
*  system\server\rx_mngt.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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


#include "stdafx.h"


// rx_commands
#include "terminal/rx_commands.h"
// rx_mngt
#include "system/server/rx_mngt.h"

#include "lib/rx_lib.h"
#include "system/server/rx_server.h"
#include "terminal/rx_telnet.h"
#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_internal_objects.h"

using namespace server;
using namespace terminal::commands;
using namespace rx;


namespace server {

namespace mngt {

// Class server::mngt::server_manager 

server_manager::server_manager()
      : _telnet_port(0)
{
	_commands_manager = server_command_manager::smart_ptr(pointers::_create_new);
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

dword server_manager::initialize (host::rx_server_host* host, managment_data_t& data)
{
	data.manager_internal_data = new mngt::manager_initialization_context;
	_telnet_port = data.telnet_port;
	_commands_manager.cast_to<server_command_manager::smart_ptr>()->register_internal_commands();
	_unassigned_domain = sys_internal::sys_objects::unssigned_domain::smart_ptr(pointers::_create_new);
	_unassigned_app = sys_internal::sys_objects::unassigned_application::smart_ptr(pointers::_create_new);
	_system_app = sys_internal::sys_objects::system_application::smart_ptr(pointers::_create_new);
	_system_domain = sys_internal::sys_objects::system_domain::smart_ptr(pointers::_create_new);
	return RX_OK;
}

dword server_manager::deinitialize ()
{
	return RX_OK;
}

dword server_manager::start (host::rx_server_host* host, const managment_data_t& data)
{
	if (_telnet_port)
	{
		_telnet_listener = terminal::console::server_telnet_socket::smart_ptr(pointers::_create_new);
		_telnet_listener->start_tcpip_4(rx_server::instance().get_runtime().get_io_pool()->get_pool(), _telnet_port);
	}
	for (auto& one : data.manager_internal_data->get_to_register())
	{
		_commands_manager.cast_to<server_command_manager::smart_ptr>()->register_command(one);
	}
	return RX_OK;
}

dword server_manager::stop ()
{
	if (_telnet_listener)
	{
		_telnet_listener->stop();
		_telnet_listener = terminal::console::server_telnet_socket::smart_ptr::null_ptr;
	}
	return RX_OK;
}

void server_manager::get_directories (server_directories_type& dirs)
{
}


// Class server::mngt::manager_initialization_context 

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
	_to_register.emplace_back(cmd);
}


} // namespace mngt
} // namespace server

