

/****************************************************************************
*
*  terminal\rx_con_programs.cpp
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

#include "system/server/rx_server.h"

// rx_con_programs
#include "terminal/rx_con_programs.h"

#include "sys_internal/rx_async_functions.h"
#include "rx_terminal_style.h"
#include "rx_terminal_version.h"
#include "rx_commands.h"
#include "api/rx_platform_api.h"
#include "rx_con_commands.h"


namespace rx_internal {

namespace terminal {

namespace console {

namespace script {

// Class rx_internal::terminal::console::script::console_program 

console_program::console_program ()
{
}


console_program::~console_program()
{
}



bool console_program::parse_line (const string_type& line, std::ostream& out, std::ostream& err, sl_runtime::program_context* context)
{
	console_program_context* ctx = static_cast<console_program_context*>(context);
	std::istringstream in(line);
	string_type first;
	in >> first;
	if (ctx->is_canceled())
	{
		err << "Canceled execution!\r\n";
		return false;
	}
	if (!first.empty())
	{
		if (line != "\t")
		{
			// regular command handling here
			server_command_base_ptr command = terminal::commands::server_command_manager::instance()->get_command_by_name(first);
			if (command)
			{
				if (!command->console_execute(in, out, err, ctx)
					&& ctx->get_result())// don raise another error, only if needed!!!
					return false;
			}
			else
			{
				err << "Syntax Error!\r\nCommand:" << first << " not existing!";
				err.flush();
				return false;
			}
		}
		else
		{
			suggestions_type suggestions;
			terminal::commands::server_command_manager::instance()->register_suggestions("", suggestions);
		}

	}
	return true;
}

std::unique_ptr<sl_runtime::program_context> console_program::create_program_context (sl_runtime::program_context* parent_context, sl_runtime::sl_program_holder* holder)
{
	RX_ASSERT(false);// had to place here because of current implementation, will be changed later
	return std::unique_ptr<sl_runtime::program_context>();
}

void console_program::process_program (sl_runtime::program_context* context, const rx_time& now, bool debug)
{
	console_program_context* ctx = (console_program_context*)context;
	sl_runtime::sl_script::sl_script_program::process_program(context, now, debug);
}


// Class rx_internal::terminal::console::script::console_program_context 

console_program_context::console_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, const string_type& current_directory)
      : current_directory_(current_directory),
        canceled_(false),
        terminal_width_(80),
        terminal_height_(24)
    , sl_runtime::sl_script::script_program_context(parent, holder)
{
}


console_program_context::~console_program_context()
{
}



void console_program_context::set_instruction_data (rx_struct_ptr data)
{
	instructions_data_.emplace(get_current_line(), data);
}

bool console_program_context::is_canceled ()
{
	return canceled_.exchange(false, std::memory_order_relaxed);
}

void console_program_context::cancel_execution ()
{
	canceled_ = true;
}


string_type console_program_context::get_current_directory ()
{
  return current_directory_;
}

void console_program_context::set_current_directory (string_type value)
{
  current_directory_ = value;
}


} // namespace script
} // namespace console
} // namespace terminal
} // namespace rx_internal

