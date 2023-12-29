

/****************************************************************************
*
*  sys_internal\rx_plugin_manager.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


// rx_plugin_manager
#include "sys_internal/rx_plugin_manager.h"



namespace rx_internal {

namespace plugins {
namespace
{
plugins_manager* g_obj = nullptr;
}

// Class rx_internal::plugins::plugins_manager 

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
	if (g_obj == nullptr)
		g_obj = new plugins_manager();
	return *g_obj;
}

bool plugins_manager::check_class (rx::pointers::code_behind_definition_t* cd)
{
	return this->definitions_.find(cd) != definitions_.end();
}

rx_result plugins_manager::register_plugin (rx_platform::library::rx_plugin_base* what)
{
	auto result = what->bind_plugin();
	if (result)
	{
		auto info = what->get_plugin_info();

		PLUGIN_LOG_INFO("plugins_manager", 900, "Registering plugin "s + info.plugin_version);
		result = what->init_plugin();
		if (result)
		{
			plugins_.emplace_back(what);
			PLUGIN_LOG_INFO("plugins_manager", 900, "Registered plugin "s + info.plugin_version);
		}
		else
		{
			for (const auto& one : result.errors())
				PLUGIN_LOG_ERROR("plugins_manager", 900, one);
			PLUGIN_LOG_ERROR("plugins_manager", 900, "Error registering plugin "s + info.plugin_version);
		}
	}
	else
	{
		for (const auto& one : result.errors())
			PLUGIN_LOG_ERROR("plugins_manager", 900, one);
		PLUGIN_LOG_ERROR("plugins_manager", 900, "Error registering plugin");
	}
	return result;
}

void plugins_manager::deinitialize ()
{
	delete this;
}


// Class rx_internal::plugins::plugin_command 

plugin_command::plugin_command()
	: terminal::commands::server_command("plugin")
{
}


plugin_command::~plugin_command()
{
}



bool plugin_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
{
	string_type sub_command;
	in >> sub_command;
	if (sub_command.empty())
	{
		const auto& plugins = plugins_manager::instance().get_plugins();
		for (const auto& one : plugins)
		{
			auto info = one->get_plugin_info();
			out << ANSI_COLOR_GREEN ANSI_COLOR_BOLD "$>" ;
			out << one->get_plugin_name() 
				<< ANSI_COLOR_RESET " [" << info.plugin_version << "]\r\n";
		}
	}
	else
	{
		const rx_platform::library::rx_plugin_base* plugin = nullptr;
		const auto& plugins = plugins_manager::instance().get_plugins();
		for (const auto& one : plugins)
		{
			if (sub_command == one->get_plugin_name())
			{
				plugin = one;
				break;
			}
		}
		if (plugin)
		{
			auto info = plugin->get_plugin_info();

			out << ANSI_COLOR_GREEN ANSI_COLOR_BOLD "$>";
			out << plugin->get_plugin_name()
				<< ANSI_COLOR_RESET "\r\n";
			out << info.plugin_version << "\r\n";
			out << "Platform: " << info.platform_version << "\r\n";
			out << "Library: " << info.lib_version << "\r\n";
			out << "Compiler: " << info.comp_version << "\r\n";
			out << "ABI: " << (info.abi_version.empty() ? "-" : info.abi_version) << "\r\n";
			out << "Common: " << (info.common_version.empty() ? "-" : info.common_version) << "\r\n";

		}
		else
		{
			err << sub_command << " is unknown plugin!";
			return false;
		}
	}
	return true;
}


} // namespace plugins
} // namespace rx_internal

