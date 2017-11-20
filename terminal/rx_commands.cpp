

/****************************************************************************
*
*  terminal\rx_commands.cpp
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

#include "system/server/rx_server.h"
#include "terminal/rx_telnet.h"
#include "terminal/rx_general_commands.h"
#include "testing/rx_test.h"


namespace terminal {

namespace commands {

// Class terminal::commands::server_command 

server_command::server_command (const string_type& console_name)
 : server::prog::server_command_base(console_name,ns::namespace_item_system)
{
}


server_command::~server_command()
{
}



void server_command::virtual_bind ()
{
  bind();

}

void server_command::virtual_release ()
{
  release();

}

namespace_item_attributes server_command::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_command | namespace_item_execute | namespace_item_read_access | namespace_item_system);
}

bool server_command::generate_json (std::ostream& def, std::ostream& err) const
{
	return true;
}


// Class terminal::commands::server_command_manager 

server_command_manager::server_command_manager()
	: objects::server_object(NS_RX_COMMANDS_MANAGER_NAME, RX_COMMANDS_MANAGER_ID)
{
}


server_command_manager::~server_command_manager()
{
}



void server_command_manager::register_command (server_command_base_ptr cmd)
{
	string_type con_name = cmd->get_console_name();
	item_lock();
	_registred_commands.emplace(con_name, cmd);
	item_unlock();
}

void server_command_manager::register_internal_commands ()
{

	// console commands
	register_command(rx_create_reference<echo_server_command>());
	register_command(rx_create_reference<console::console_commands::dir_command>());
	register_command(rx_create_reference<console::console_commands::ls_command>());
	register_command(rx_create_reference<console::console_commands::cd_command>());
	register_command(rx_create_reference<console::console_commands::info_command>());
	register_command(rx_create_reference<console::console_commands::code_command>());
	register_command(rx_create_reference<console::console_commands::rx_name_command>());
	register_command(rx_create_reference<console::console_commands::cls_command>());
	register_command(rx_create_reference<console::console_commands::shutdown_command>());
	register_command(rx_create_reference<console::console_commands::log_command>());
	register_command(rx_create_reference<console::console_commands::sec_command>());
	register_command(rx_create_reference<console::console_commands::time_command>());
	register_command(rx_create_reference<console::console_commands::sleep_command>());
	register_command(rx_create_reference<console::console_commands::def_command>());
	register_command(rx_create_reference<console::console_commands::phyton_command>());
	register_command(rx_create_reference<testing::test_command>());
}

server_command_base_ptr server_command_manager::get_command_by_name (const string_type& name)
{

	server_command_base_ptr command;
	item_lock();
	auto it = _registred_commands.find(name);
	if (it != _registred_commands.end())
	{
		command = it->second;
	}
	item_unlock();
	return command;
}

server_command_manager::smart_ptr server_command_manager::instance ()
{
	return rx_server::instance().get_manager().get_commands_manager().cast_to<server_command_manager::smart_ptr>();
}

namespace_item_attributes server_command_manager::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system | namespace_item_object);
}

void server_command_manager::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "_CommandsManager";
	has_own_code_info = true;
}

void server_command_manager::get_commands (std::vector<command_ptr>& sub_items) const
{
	item_lock();
	sub_items.reserve(_registred_commands.size());
	for (const auto& one : _registred_commands)
		sub_items.emplace_back(one.second);
	item_unlock();
}

bool server_command_manager::get_help (std::ostream& out, std::ostream& err)
{
	
	out << "Printing help, well the beginig of making help :)\r\n";
	out << RX_CONSOLE_HEADER_LINE "\r\n";
	out << "This is a list of commands:\r\n";

	for (const auto& one : _registred_commands)
	{
		out << "\r\n" ANSI_COLOR_YELLOW;
		out <<one.first;
		out << ANSI_COLOR_RESET "\r\n" << one.second->get_help()<<"\r\n";
	}

	out << "\r\n\r\nThis is acctually code comment dump :)\r\n";
	out << "Don't know what to tell you, try reading the reference...\r\n";

	return true;
}


// Class terminal::commands::echo_server_command 

echo_server_command::echo_server_command()
  : server_command("echo")
{
}


echo_server_command::~echo_server_command()
{
}



bool echo_server_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	// just copy from one steam to another
	if (!in.eof())
	{
		std::copy(std::istreambuf_iterator<char>(in),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(out));
	}
	else
		out << "echo";
	return true;
}


} // namespace commands
} // namespace terminal

