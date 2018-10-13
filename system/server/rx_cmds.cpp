

/****************************************************************************
*
*  system\server\rx_cmds.cpp
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

#include "terminal/rx_terminal_version.h"

// rx_cmds
#include "system/server/rx_cmds.h"
// rx_security
#include "lib/security/rx_security.h"

#include "system/server/rx_server.h"
#include "terminal/rx_telnet.h"
#include "terminal/rx_commands.h"
#include "sys_internal/rx_internal_ns.h"
using namespace rx_platform;
using namespace terminal::commands;


#define SCRIPT_LINE_LENGTH 0x400



namespace rx_platform {

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


char g_console_unauthorized[] = ANSI_COLOR_RED "You are unauthorized!" ANSI_COLOR_RESET "\r\n;";

// Class rx_platform::prog::program_context_base 

program_context_base::program_context_base (server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<server_program_base> program)
      : root_(root_context),
        holder_(holder),
        program_(program),
        current_directory_(current_directory),
        out_(out),
        err_(err),
        postponed_(false),
        canceled_(false)
{
}


program_context_base::~program_context_base()
{
}



bool program_context_base::is_postponed () const
{
	return postponed_;
}

bool program_context_base::postpone (uint32_t interval)
{
	postponed_ = true;
	if (interval)
	{
		rx_platform::rx_gate::instance().get_runtime().append_timer_job(
			jobs::lambda_timer_job<rx_reference<program_context_base> >::smart_ptr(
				[](rx_platform::prog::program_context_base_ptr context) mutable
				{
					if(!context->is_canceled())
						context->return_control();
				},
				smart_this()), interval);
	}
	else
		rx_platform::rx_gate::instance().get_runtime().append_job(
			jobs::lambda_job<rx_reference<program_context_base> >::smart_ptr(
				[](rx_platform::prog::program_context_base_ptr context) mutable
				{
					if (!context->is_canceled())
						context->return_control();
				},
				smart_this()));
	return true;
}

bool program_context_base::return_control (bool done)
{
	postponed_ = false;
	bool ret = program_->process_program(smart_this(), rx_time::now(), false);
	if (!postponed_)
	{
		send_results(ret);
	}
	return ret;
}

void program_context_base::send_results (bool result)
{
}

void program_context_base::set_instruction_data (rx_struct_ptr data)
{
	instructions_data_.emplace(get_possition(), data);
}

bool program_context_base::is_canceled ()
{
	return canceled_.exchange(false, std::memory_order_relaxed);
}

void program_context_base::cancel_execution ()
{
	canceled_.store(true, std::memory_order_relaxed);
	return_control(false);
}

bool program_context_base::should_run_again ()
{
	return !is_postponed();
}


// Class rx_platform::prog::server_command_base 

server_command_base::server_command_base (const string_type& console_name, ns::namespace_item_attributes attributes)
      : time_stamp_(rx_time::now()),
        console_name_(console_name),
        security_guard_(std::make_unique<security::security_guard>()),
        modified_time_(rx_time::now())

  //!!, rx_server_item(console_name, (ns::namespace_item_attributes)(attributes | ns::namespace_item_execute), "COMMAND   ",rx_time::now())
{
}


server_command_base::~server_command_base()
{
}



void server_command_base::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	has_own_code_info = true;
	console = get_console_name();
}

string_type server_command_base::get_type_name () const
{
	static string_type type_name = RX_CPP_COMMAND_TYPE_NAME;
	return type_name;
}

values::rx_value server_command_base::get_value () const
{
	values::rx_value temp;
	temp.assign_static(get_console_name(), time_stamp_);
	return temp;
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
	return (namespace_item_attributes)(namespace_item_system_const_value | namespace_item_execute_access | namespace_item_test_case);
}

bool server_command_base::console_execute (std::istream& in, std::ostream& out, std::ostream& err, console_program_context::smart_ptr ctx)
{
	if (security_guard_->check_premissions(security::rx_security_execute_access, security::rx_security_ext_null))
	{
		return do_console_command(in, out, err, ctx);
	}
	else
	{
		err << ANSI_COLOR_RED RX_ACCESS_DENIED ANSI_COLOR_RESET;
		return false;
	}
}

bool server_command_base::dword_check_premissions (security::security_mask_t mask, security::extended_security_mask_t extended_mask)
{
	return security_guard_->check_premissions(mask, extended_mask);
}

rx_time server_command_base::get_created_time () const
{
	return rx_gate::instance().get_started();
}

bool server_command_base::is_browsable () const
{
	return false;
}

bool server_command_base::generate_json (std::ostream& def, std::ostream& err) const
{
	return false;
}

platform_item_ptr server_command_base::get_item_ptr ()
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

string_type server_command_base::get_name () const
{
	return console_name_;
}

size_t server_command_base::get_size () const
{
	return 0;
}

bool server_command_base::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool server_command_base::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::prog::server_program_base 

server_program_base::server_program_base()
{
}


server_program_base::~server_program_base()
{
}



// Class rx_platform::prog::program_executer_base 

program_executer_base::program_executer_base()
{
}


program_executer_base::~program_executer_base()
{
}



// Class rx_platform::prog::server_program_holder 

server_program_holder::server_program_holder (program_executer_ptr executer)
      : executer_(executer)
{
}


server_program_holder::~server_program_holder()
{
}



// Class rx_platform::prog::console_program_context 

console_program_context::console_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<server_program_base> program, rx_virtual<console_client> client)
      : client_(client),
        current_line_(0),
        out_std_(out.unsafe_ptr()),
        err_std_(err.unsafe_ptr())
  , prog::program_context_base(holder, root_context,current_directory,out,err, program)

{
}


console_program_context::~console_program_context()
{
}



size_t console_program_context::next_line ()
{
	current_line_++;
	return current_line_;
}

std::ostream& console_program_context::get_stdout ()
{
	return out_std_;
}

std::ostream& console_program_context::get_stderr ()
{
	return err_std_;
}

console_program_context::smart_ptr console_program_context::create_console_sub_context ()
{
	console_program_context::smart_ptr ctx_ret = console_program_context::smart_ptr(get_holder(),smart_this()
		, get_current_directory(), get_out(), get_err(),get_program(),client_);
	return ctx_ret;
}

void console_program_context::send_results (bool result)
{
	if (client_)
		client_->process_event(result, get_out(), get_err(),true);
}

size_t console_program_context::get_possition () const
{
	return current_line_;
}


// Class rx_platform::prog::server_console_program 

server_console_program::server_console_program (std::istream& in)
{
	while (!in.eof())
	{
		char temp[SCRIPT_LINE_LENGTH];
		in.getline(temp, SCRIPT_LINE_LENGTH);
		lines_.emplace_back(temp);
	}
}

server_console_program::server_console_program (const string_vector& lines)
  : lines_(lines)
{
}

server_console_program::server_console_program (const string_type& line)
{
	lines_.emplace_back(line);
}


server_console_program::~server_console_program()
{
}



bool server_console_program::process_program (prog::program_context_ptr context, const rx_time& now, bool debug)
{

	console_program_context::smart_ptr ctx = context.cast_to<console_program_context::smart_ptr>();
	size_t total_lines = lines_.size();
	size_t current_line = ctx->get_current_line();
	string_type label;

	std::ostream& out(ctx->get_stdout());
	std::ostream& err(ctx->get_stderr());

	while (current_line < total_lines && ctx->should_run_again())
	{		
		label.clear();
		std::istringstream in(lines_[current_line]);
		string_type name;
		in >> name;
		if (!name.empty())
		{
			server_command_base_ptr command = terminal::commands::server_command_manager::instance()->get_command_by_name(name);
			if (command)
			{
				if (!command->console_execute(in, out, err,ctx))
					return false;
			}
			else
			{
				err << "Syntax Error!\r\nCommand:" << name << " not existing!";
				err.flush();
				return false;
			}
		}
		if (ctx->is_canceled())
		{
			err << "Pending cancel for the current command:\r\n";
			err << lines_[current_line];
			return false;
		}
		if(!ctx->is_postponed())
			current_line = ctx->next_line();

		
	}
	return true;
}

prog::program_context_ptr server_console_program::create_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_virtual<console_client> client)
{
	return console_program_context::smart_ptr(holder, root_context, current_directory,out,err,smart_this(),client);
}


// Class rx_platform::prog::console_client 

console_client::console_client (rx_thread_handle_t executer)
      : executer_(executer)
{
#ifdef _DEBUG
	current_directory_ = rx_platform::rx_gate::instance().get_root_directory()->get_sub_directory("_sys");
#else
	current_directory_ = rx_platform::rx_gate::instance().get_root_directory()->get_sub_directory("world");
#endif
}


console_client::~console_client()
{
}



bool console_client::do_command (string_type&& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	string_type captured_line(std::move(line));
	rx_post_function<smart_ptr>(
		[captured_line,out_buffer, err_buffer, ctx](smart_ptr sended_this)
		{
			sended_this->synchronized_do_command(captured_line, out_buffer, err_buffer, ctx);
		}
		, smart_this()
		,executer_
	);
	return true;
}

void console_client::get_prompt (string_type& prompt)
{
	prompt = "\r\n";
	prompt += current_directory_->get_path();
	if (current_object_)
	{
		prompt += RX_DIR_DELIMETER;
		prompt += ANSI_COLOR_BOLD ANSI_COLOR_YELLOW;
		prompt += current_object_->get_name();
		prompt += ANSI_COLOR_RESET;
	}
	prompt += "\r\n" ANSI_COLOR_GREEN;
	prompt += security::active_security()->get_full_name();
	prompt += ":" ANSI_COLOR_RESET;
	prompt += ">";
}

void console_client::get_wellcome (string_type& wellcome)
{
	wellcome = g_console_welcome;
	wellcome += ANSI_COLOR_BOLD ANSI_COLOR_GREEN ">>>> Running ";
	wellcome += get_console_name();
	wellcome += " Console...\r\n";
	wellcome += RX_CONSOLE_HEADER_LINE "\r\n" ANSI_COLOR_RESET;
}

bool console_client::is_postponed () const
{
	if (current_)
		return true;
	else
		return false;
}

const string_type& console_client::get_console_terminal ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_TERM_NAME, RX_TERM_MAJOR_VERSION, RX_TERM_MINOR_VERSION, RX_TERM_BUILD_NUMBER);
	}
	return ret;
}

void console_client::synchronized_do_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	bool ret = false;
	RX_ASSERT(!current_);
	if (line.empty())
	{
		std::ostream out(out_buffer.unsafe_ptr());
		out << "\r\n";
		ret = true;

	}
	else if (line[0] == '@')
	{// this is console command
		ret = rx_platform::rx_gate::instance().do_host_command(line.substr(1), out_buffer, err_buffer, ctx);
	}
	else if (line == "exit")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		out << "bye...\r\n";
		exit_console();
		ret = true;
	}
	else if (line == "hello")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		out << "Hello to you too!!!";
		ret = true;
	}
	else if (line == "help")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		std::ostream err(out_buffer.unsafe_ptr());
		ret = terminal::commands::server_command_manager::instance()->get_help(out, err);
	}
	else if (line == "term")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		std::ostream err(out_buffer.unsafe_ptr());

		out << "Terminal Information:\r\n" RX_CONSOLE_HEADER_LINE "\r\n" ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET;
		out << get_console_name() << " Console\r\n" ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET;
		out << get_console_terminal() << "\r\n";
		ret = true;
	}
	else if (line == "host")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		std::ostream err(err_buffer.unsafe_ptr());

		out << "Hosts Information:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
		string_array hosts;
		rx_gate::instance().get_host()->get_host_info(hosts);
		for (const auto& one : hosts)
		{
			out << ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET << one << "\r\n";
		}
		ret = true;
	}
	else
	{
		prog::server_console_program::smart_ptr temp_prog(line);

		security::security_auto_context dummy(ctx);

		prog::program_context_base_ptr ctx = temp_prog->create_program_context(
			prog::server_program_holder_ptr::null_ptr,
			prog::program_context_base_ptr::null_ptr,
			current_directory_,
			out_buffer,
			err_buffer,
			smart_this());
		ctx->set_current_directory(current_directory_);
		ctx->set_current_object(current_object_);
		ctx->set_current_item(current_item_);
		ret = temp_prog->process_program(ctx, rx_time::now(), false);
		if (ret)
		{
			if (ctx->is_postponed())
			{
				current_ = ctx;
			}
			else
			{
				current_directory_ = ctx->get_current_directory();
				current_item_ = ctx->get_current_item();
				current_object_ = ctx->get_current_object();
			}
		}
	}
	if(!current_)
		process_result(ret, out_buffer, err_buffer);
}

void console_client::process_event (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done)
{
	if (current_)
	{
		current_ = program_context_ptr::null_ptr;
		process_result(result, out_buffer, err_buffer);
	}
}

bool console_client::cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	rx_post_function<smart_ptr>(
		[out_buffer, err_buffer, ctx](smart_ptr sended_this)
		{
			sended_this->synchronized_cancel_command(out_buffer, err_buffer, ctx);
		}
		, smart_this()
		, executer_
		);
	return true;
}

void console_client::synchronized_cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	if (current_)
	{// we are in a command
		current_->cancel_execution();
		current_ = program_context_ptr::null_ptr;
	}
	else
	{// nothing to cancel!!!
		std::ostream err(err_buffer.unsafe_ptr());
		err << "\r\nThere is nothing to cancel...";
		process_result(false, out_buffer, err_buffer);
	}
}

void console_client::get_security_error (string_type& txt, sec_error_num_t err_number)
{
	if (err_number == 0)
	{
		txt = g_console_unauthorized;
	}
}

bool console_client::do_commands (string_array&& lines, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	string_array captured_lines(std::move(lines));
	rx_post_function<smart_ptr>(
		[captured_lines, out_buffer, err_buffer, ctx](smart_ptr sended_this)
		{
			for(const auto& captured_line : captured_lines)
				sended_this->synchronized_do_command(captured_line, out_buffer, err_buffer, ctx);
		}
		, smart_this()
		, executer_
		);
	return true;
}


// Class rx_platform::prog::server_script_program 

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
	return console_program_context::smart_ptr(holder, root_context, current_directory, out, err,smart_this(),prog::console_client::smart_ptr::null_ptr);
}


// Class rx_platform::prog::server_script_host 

server_script_host::server_script_host (const script_def_t& definition)
{
}


server_script_host::~server_script_host()
{
}



} // namespace prog
} // namespace rx_platform

