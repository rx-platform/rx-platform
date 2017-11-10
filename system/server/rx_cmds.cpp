

/****************************************************************************
*
*  system\server\rx_cmds.cpp
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


// rx_cmds
#include "system/server/rx_cmds.h"
// rx_security
#include "lib/security/rx_security.h"

#include "system/server/rx_server.h"
#include "terminal/rx_telnet.h"
#include "terminal/rx_commands.h"
using namespace server;
using namespace terminal::commands;


#define SCRIPT_LINE_LENGTH 0x400



namespace server {

namespace prog {


/*
char g_console_welcome[] = "\r\n\
       ____   __  __\r\n\
      |  _ \\  \\ \\/ /\r\n\
      | |_) |  \\  / \r\n\
      |  _ <   /  \\ \r\n\
      |_| \\_\\ /_/\\_\\\r\n\
 \r\n\
";
*/

char g_console_welcome[] = ANSI_COLOR_YELLOW "\
        ____________________\r\n\
       / ____     _  __    /\\\r\n\
      / / __ \\   | |/ /   / /\\\r\n\
     / / /_/ /   |   /   / /\\\r\n\
    / / _, _/   /   |   / /\\\r\n\
   / /_/ |_|   /_/|_|  / /\\\r\n\
  /___________________/ /\\\r\n\
  \\___________________\\/\\\r\n\
   \\ \\ \\ \\ \\ \\ \\ \\ \\ \\ \\\r\n\
 \r\n\
" ANSI_COLOR_RESET;


// Class server::prog::program_context_base 

program_context_base::program_context_base (server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory)
      : _root(root_context),
        _holder(holder),
        _current_directory(current_directory)
{
}


program_context_base::~program_context_base()
{
}



bool program_context_base::is_postponed () const
{
	return false;
}


// Class server::prog::server_command_base 

server_command_base::server_command_base (const string_type& console_name, ns::namespace_item_attributes attributes)
      : _console_name(console_name)
  //!!, rx_server_item(console_name, (ns::namespace_item_attributes)(attributes | ns::namespace_item_execute), "COMMAND   ",rx_time::now())
{
}


server_command_base::~server_command_base()
{
}



const string_type& server_command_base::get_console_name (std::istream& in, std::ostream& out, std::ostream& err)
{
	return _console_name;
}

void server_command_base::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "_ServerCommand";
	has_own_code_info = true;
	console = _console_name;
}

string_type server_command_base::get_type_name () const
{
  // generated from ROSE!!!
  static string_type type_name = "COMMAND";
  return type_name;


}

void server_command_base::get_value (values::rx_value& val) const
{
	values::rx_value temp;
	temp.set_time(rx_time::now());
	temp.set_quality(RX_GOOD_QUALITY);
	val = temp;
}

void server_command_base::item_lock ()
{
}

void server_command_base::item_unlock ()
{
}

void server_command_base::item_lock () const
{
}

void server_command_base::item_unlock () const
{
}

namespace_item_attributes server_command_base::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_system_const_value | namespace_item_execute| namespace_item_test_case);
}

const string_type& server_command_base::get_item_name () const
{
	return get_console_name();
}


// Class server::prog::server_program_base 

server_program_base::server_program_base()
{
}


server_program_base::~server_program_base()
{
}



// Class server::prog::program_executer_base 

program_executer_base::program_executer_base()
{
}


program_executer_base::~program_executer_base()
{
}



// Class server::prog::server_program_holder 

server_program_holder::server_program_holder (program_executer_ptr executer)
      : _executer(executer)
{
}


server_program_holder::~server_program_holder()
{
}



// Class server::prog::console_program_context 

console_program_context::console_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err)
      : _current_line(0),
        _out(out),
        _err(err),
        _out_std(out.unsafe_ptr()),
        _err_std(err.unsafe_ptr())
  , prog::program_context_base(holder, root_context,current_directory)

{
}


console_program_context::~console_program_context()
{
}



size_t console_program_context::next_line ()
{
	_current_line++;
	return _current_line;
}

std::ostream& console_program_context::get_stdout ()
{
	return _out_std;
}

std::ostream& console_program_context::get_stderr ()
{
	return _err_std;
}

console_program_context::smart_ptr console_program_context::create_console_sub_context ()
{
	console_program_context::smart_ptr ctx_ret = console_program_context::smart_ptr(get_holder(),smart_this()
		, get_current_directory(), _out, _err);
	return ctx_ret;
}


// Class server::prog::server_console_program 

server_console_program::server_console_program (std::istream& in)
{
	while (!in.eof())
	{
		char temp[SCRIPT_LINE_LENGTH];
		in.getline(temp, SCRIPT_LINE_LENGTH);
		_lines.emplace_back(temp);
	}
}

server_console_program::server_console_program (const string_vector& lines)
  : _lines(lines)
{
}

server_console_program::server_console_program (const string_type& line)
{
	_lines.emplace_back(line);
}


server_console_program::~server_console_program()
{
}



bool server_console_program::process_program (prog::program_context_ptr context, const rx_time& now, bool debug)
{

	console_program_context::smart_ptr ctx = context.cast_to<console_program_context::smart_ptr>();
	size_t total_lines = _lines.size();
	size_t current_line = ctx->get_current_line();
	string_type label;

	std::ostream& out(ctx->get_stdout());
	std::ostream& err(ctx->get_stderr());

	while (current_line < total_lines)
	{
		label.clear();
		std::istringstream in(_lines[current_line]);
		string_type name;
		in >> name;
		if (!name.empty())
		{
			server_command_base_ptr command = terminal::commands::server_command_manager::instance()->get_command_by_name(name);
			if (command)
			{
				if (!command->do_console_command(in, out, err,ctx))
					return false;
			}
			else
			{
				err << "Syntax Error!\r\nCommand:" << name << " not exsisting!";
				err.flush();
				return false;
			}
		}

		current_line = ctx->next_line();
	}
	return true;
}

prog::program_context_ptr server_console_program::create_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err)
{
	return console_program_context::smart_ptr(holder, root_context, current_directory,out,err);
}


// Class server::prog::console_client 

console_client::console_client()
{
#ifdef _DEBUG
	_current_directory = server::rx_server::instance().get_root_directory()->get_sub_directory("_sys/plugins/host");
#else
	_current_directory = server::rx_server::instance().get_root_directory()->get_sub_directory("world");
#endif
}


console_client::~console_client()
{
}



bool console_client::do_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx)
{
	RX_ASSERT(!_current);
	if (line.size() > 0 && line[0] == '@')
	{// this is console command
		return server::rx_server::instance().do_host_command(line.substr(1), out_buffer, err_buffer, ctx);
	}
	if (line == "exit")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		out << "bye...\r\n";
		exit_console();
		return true;
	}
	else if (line == "hello")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		out << "Hello to you too!!!";
		return true;
	}
	else if (line == "help")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		std::ostream err(out_buffer.unsafe_ptr());
		return terminal::commands::server_command_manager::instance()->get_help(out, err);
	}
	else
	{
		prog::server_console_program temp_prog(line);
		
		prog::program_context_base_ptr ctx = temp_prog.create_program_context(prog::server_program_holder_ptr::null_ptr, prog::program_context_base_ptr::null_ptr,_current_directory,out_buffer,err_buffer);
		ctx->set_current_directory(_current_directory);
		bool ret = temp_prog.process_program(ctx, rx_time::now(), false);
		if (ret)
		{
			if (ctx->is_postponed())
			{
				_current = ctx;
			}
			else
			{
				_current_directory = ctx->get_current_directory();
			}
		}
		return ret;
	}
}

void console_client::get_prompt (string_type& prompt)
{
	prompt = ANSI_COLOR_GREEN;
	prompt += security::active_security()->get_full_name();
	prompt += ":" ANSI_COLOR_RESET;
	prompt += _current_directory->get_path();
	prompt += ">";
}

void console_client::get_wellcome (string_type& wellcome)
{
	wellcome = g_console_welcome;
	wellcome += get_console_name();
	wellcome += "\r\n===========================================\r\n";
	wellcome += "\r\n>";
	wellcome += rx_server::instance().get_host_info();
	wellcome += "\r\n";
}

bool console_client::is_postponed () const
{
	if (_current)
		return true;
	else
		return false;
}


// Class server::prog::server_script_program 

server_script_program::server_script_program (std::istream& in)
{
}

server_script_program::server_script_program (const string_vector& lines)
{
}

server_script_program::server_script_program (const string_type& line)
{
}


server_script_program::~server_script_program()
{
}



bool server_script_program::process_program (prog::program_context_ptr context, const rx_time& now, bool debug)
{
	console_program_context::smart_ptr ctx = context.cast_to<console_program_context::smart_ptr>();
	ctx->get_stderr() << "Nema nicega ovde...";
	return false;
}

prog::program_context_ptr server_script_program::create_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err)
{
	return console_program_context::smart_ptr(holder, root_context, current_directory, out, err);
}


// Class server::prog::server_script_host 

server_script_host::server_script_host (const script_def_t& definition)
{
}


server_script_host::~server_script_host()
{
}



} // namespace prog
} // namespace server

