

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


#include "terminal/ansi_codes.h"
#define RX_ANSI_PROGRAM_LINE ANSI_COLOR_YELLOW ANSI_COLOR_BOLD "{"
#define RX_ANSI_PROGRAM_LINE_END "}" ANSI_COLOR_RESET


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



std::unique_ptr<logic::program_context> console_program::create_program_context (logic::program_context* parent_context)
{
	RX_ASSERT(false);// had to place here because of current implementation, will be changed later
	return std::unique_ptr<logic::program_context>();
}

void console_program::load (const string_type& text)
{
	size_t count = text.size();
	size_t idx = 0;
	int blocks_count = 0;
	string_type current_line;
	while (idx < count)
	{
		if (blocks_count == 0)
		{// regular line
			switch (text[idx])
			{
			case '\r':
			case '\n':
				if (!current_line.empty())
				{
					lines_.push_back(current_line);
					current_line.clear();
				}
				break;
			case '{':
				blocks_count++;
			default:
				current_line += text[idx];
			}
			idx++;
		}
		else
		{
			switch (text[idx])
			{
			case '}':
				blocks_count--;
				current_line += text[idx];
				break;
			case '{':
				blocks_count++;
			default:
				current_line += text[idx];
			}
			idx++;
		}
	}
	if (!current_line.empty())
		lines_.push_back(current_line);
}

void console_program::process_program (logic::program_context* context, runtime::runtime_process_context& rt_context)
{
	console_program_context* ctx = (console_program_context*)context;
	size_t total_lines = lines_.size();
	size_t current_line = ctx->get_current_line();
	string_type label;

	std::ostream& out(ctx->get_stdout());
	std::ostream& err(ctx->get_stderr());

	while (!ctx->waiting_ && ctx->get_current_line() < total_lines)
	{
		if (!parse_line(lines_[current_line], out, err, ctx))
		{
			ctx->raise_error();
			break;
		}
		else
		{
			ctx->next_line();
		}
	}
	ctx->send_results(ctx->get_result(), !ctx->waiting_);
}

bool console_program::parse_line (const string_type& line, std::ostream& out, std::ostream& err, console_program_context* context)
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


// Class rx_internal::terminal::console::script::console_program_context 

console_program_context::console_program_context (program_context* parent, console_program_ptr runtime, const string_type& current_directory)
      : current_directory_(current_directory),
        canceled_(false),
        terminal_width_(80),
        terminal_height_(24),
        current_line_(0),
        waiting_(false),
        error_(false)
    , program_context(parent, runtime)
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

void console_program_context::raise_error ()
{
	if (!error_)
	{
		reset_waiting();
		error_ = true;
		get_stderr() << "\r\nError in line " RX_ANSI_PROGRAM_LINE << current_line_ + 1 << RX_ANSI_PROGRAM_LINE_END "\r\n";
	}
	else
	{
		// this should not be happening
		// anyway just do the waiting stuff as code,
		// as bad as it is, depends on it
		RX_ASSERT(false);
		reset_waiting();
	}
}

bool console_program_context::get_result () const
{
	return !error_;
}

void console_program_context::set_waiting ()
{
	waiting_ = true;
}

void console_program_context::reset_waiting ()
{
	waiting_ = false;
}

size_t console_program_context::next_line ()
{
	current_line_++;
	return current_line_;
}

void console_program_context::continue_scan ()
{
	waiting_ = false;
	process_program(true);
}

void console_program_context::init_scan ()
{
	current_line_ = 0;
	error_ = false;
}


string_type console_program_context::get_current_directory ()
{
  return current_directory_;
}

void console_program_context::set_current_directory (string_type value)
{
  current_directory_ = value;
}

size_t console_program_context::get_current_line () const
{
  return current_line_;
}


} // namespace script
} // namespace console
} // namespace terminal
} // namespace rx_internal

