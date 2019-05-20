

/****************************************************************************
*
*  sys_internal\rx_plugin_manager.cpp
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


// rx_plugin_manager
#include "sys_internal/rx_plugin_manager.h"



namespace sys_internal {

namespace plugins {

// Class sys_internal::plugins::plugins_manager 

plugins_manager::plugins_manager()
{
}

plugins_manager::plugins_manager(const plugins_manager &right)
{
	RX_ASSERT(false);
}


plugins_manager::~plugins_manager()
{
}


plugins_manager & plugins_manager::operator=(const plugins_manager &right)
{
	RX_ASSERT(false);
	return *this;
}



plugins_manager& plugins_manager::instance ()
{
	static plugins_manager g_obj;
	return g_obj;
}

bool plugins_manager::check_class (rx::pointers::code_behind_definition_t* cd)
{
	return this->definitions_.find(cd) != definitions_.end();
}

rx_result plugins_manager::register_plugin (rx_platform::library::rx_plugin_base* what)
{
	plugins_.emplace_back(what);
	return true;
}


// Class sys_internal::plugins::plugin_command 

plugin_command::plugin_command()
	: terminal::commands::server_command("plugin")
{
}


plugin_command::~plugin_command()
{
}



bool plugin_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx)
{
	string_type sub_command;
	in >> sub_command;
	if (sub_command == "list")
	{
		const auto& plugins = plugins_manager::instance().get_plugins();
		for (const auto& one : plugins)
		{
			out << ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET;
			out << one->get_plugin_info() << "\r\n";
		}
	}
	else
	{
		if (sub_command.empty())
			err << "Specify a sub-command.";
		else
			err << sub_command << " is unknown sub-command!";
		return false;
	}
	return true;
}


} // namespace plugins
} // namespace sys_internal

