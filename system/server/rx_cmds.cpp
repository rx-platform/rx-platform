

/****************************************************************************
*
*  system\server\rx_cmds.cpp
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

#include "terminal/rx_terminal_version.h"

// rx_security
#include "lib/security/rx_security.h"
// rx_cmds
#include "system/server/rx_cmds.h"

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

// Class rx_platform::prog::server_command_base 

server_command_base::server_command_base (const string_type& name, const rx_node_id& id)
      : time_stamp_(rx_time::now()),
        console_name_(name),
        security_guard_(std::make_unique<security::security_guard>()),
        modified_time_(rx_time::now())
	, program_runtime(name, id, true)
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
	return (namespace_item_attributes)(namespace_item_execute_access);
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

bool server_command_base::generate_json (std::ostream& def, std::ostream& err) const
{
	return false;
}

platform_item_ptr server_command_base::get_item_ptr () const
{
	return rx_create_reference<sys_internal::internal_ns::rx_other_implementation<smart_ptr> >(smart_this());
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


// Class rx_platform::prog::console_program_context 

console_program_context::console_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, rx_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<console_client> client)
      : client_(client),
        out_std_(out.unsafe_ptr()),
        err_std_(err.unsafe_ptr()),
        current_directory_(current_directory),
        out_(out),
        err_(err),
        postponed_(0),
        canceled_(false),
        result_(false)
	, sl_runtime::sl_script::script_program_context(parent,holder, out.unsafe_ptr(), err.unsafe_ptr())
{
}


console_program_context::~console_program_context()
{
}



std::ostream& console_program_context::get_stdout ()
{
	return out_std_;
}

std::ostream& console_program_context::get_stderr ()
{
	return err_std_;
}

void console_program_context::send_results (bool result)
{
	postpone_done();
	if (client_)
		client_->process_event(result, get_out(), get_err(),true);
}

size_t console_program_context::get_possition () const
{
	return get_current_line();
}

bool console_program_context::is_postponed () const
{
	return postponed_ > 0;
}

bool console_program_context::postpone (uint32_t interval)
{
	postponed_++;
	stop_execution();
	if (interval)
	{
		buffer_ptr out_ptr = out_;
		buffer_ptr err_ptr = err_;
		rx_post_delayed_function<decltype(client_)>(
			[out_ptr, err_ptr](decltype(client_) client)
			{
				client->process_event(true, out_ptr, err_ptr, true);
			}
			, interval
			, client_
			, client_->get_executer()
			);
	}
	else
	{
		buffer_ptr out_ptr = out_;
		buffer_ptr err_ptr = err_;
		rx_post_function<decltype(client_)>(
			[out_ptr, err_ptr](decltype(client_) client)
			{
				client->process_event(true, out_ptr, err_ptr, true);
			}
			, client_
			, client_->get_executer()
		);
	}
	return true;
}

void console_program_context::set_instruction_data (rx_struct_ptr data)
{
	instructions_data_.emplace(get_possition(), data);
}

bool console_program_context::is_canceled ()
{
	return canceled_.exchange(false, std::memory_order_relaxed);
}

void console_program_context::postpone_done ()
{
	postponed_--;
}

void console_program_context::set_waiting ()
{
	postponed_++;
	stop_execution();
}

void console_program_context::cancel_execution ()
{
	canceled_ = true;
}

api::rx_context console_program_context::create_api_context ()
{
	api::rx_context ret;
	ret.directory = current_directory_;
	ret.object = client_;
	return ret;
}


// Class rx_platform::prog::server_console_program 

server_console_program::server_console_program (console_client::smart_ptr client, const string_type& name, const rx_node_id& id, bool system)
	: program_runtime(name, id, system)
	, console_(client)
{
}


server_console_program::~server_console_program()
{
}



// Class rx_platform::prog::console_client 

console_client::console_client()
      : current_context_(nullptr)
{
#ifdef _DEBUG
	current_directory_ = rx_platform::rx_gate::instance().get_root_directory()->get_sub_directory("world");// "_sys");
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
		, get_executer()
	);
	return true;
}

void console_client::get_prompt (string_type& prompt)
{
	prompt = "\r\n";
	prompt += current_directory_->meta_info().get_full_path();
	prompt += "\r\n" ANSI_RX_USER;
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
	if (current_program_)
		return true;
	else
		return false;
}

const string_type& console_client::get_console_terminal ()
{
	static string_type ret(get_terminal_info());
	return ret;
}

void console_client::synchronized_do_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	bool ret = false;
	RX_ASSERT(!current_program_);
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
	else if (line == "term")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		std::ostream err(out_buffer.unsafe_ptr());

		out << "Terminal Information:\r\n" RX_CONSOLE_HEADER_LINE "\r\n" ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET;
		out << "Info: " << get_console_terminal() << "\r\n" ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET "Console: ";
		out << get_console_name() << " Console\r\n";
		
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
	else if (line == "storage")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		std::ostream err(err_buffer.unsafe_ptr());

		out << "Storage Information:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
		string_type sys_info = rx_gate::instance().get_host()->get_system_storage()->get_storage_info();
		string_type sys_ref = rx_gate::instance().get_host()->get_system_storage()->get_storage_reference();
		string_type user_info = rx_gate::instance().get_host()->get_user_storage()->get_storage_info();
		string_type user_ref = rx_gate::instance().get_host()->get_user_storage()->get_storage_reference();
		string_type test_info = rx_gate::instance().get_host()->get_test_storage()->get_storage_info();
		string_type test_ref = rx_gate::instance().get_host()->get_test_storage()->get_storage_reference();
		out << ANSI_COLOR_GREEN "System Storage" ANSI_COLOR_RESET "\r\nReference: " << sys_ref << "\r\nVersion: "<< sys_info << "\r\n\r\n";
		out << ANSI_COLOR_GREEN "User Storage" ANSI_COLOR_RESET "\r\nReference: " << user_ref << "\r\nVersion: " << user_info << "\r\n\r\n";
		out << ANSI_COLOR_GREEN "Test Storage" ANSI_COLOR_RESET "\r\nReference: " << test_ref << "\r\nVersion: " << test_info << "\r\n";
		
		ret = true;
	}
	else if (line == "welcome")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		std::ostream err(out_buffer.unsafe_ptr());

		string_type msg;
		get_wellcome(msg);

		out << msg;

		ret = true;
	}
	else
	{
		rx_uuid id = rx_uuid::create_new();
		server_console_program::smart_ptr prog(smart_this(), id.to_string(), rx_node_id(id.uuid()), true);
		// create main program
		console_program* temp_prog = new console_program;
		auto context = new console_program_context(nullptr, &prog->my_program(), current_directory_, out_buffer, err_buffer, smart_this());
		temp_prog->load(line);
		prog->my_program().set_main_program(temp_prog, context);
		// set security context
		security::security_auto_context dummy(ctx);

		prog->my_program().process_program(context, rx_time::now(), false);
		ret = !context->has_error();
		if (ret)
		{
			if (context->is_postponed())
			{
				current_program_ = prog;
				current_context_ = context;
			}
			else
			{
				current_directory_ = context->get_current_directory();
			}
		}
	}
	if(!is_postponed())
		process_result(ret, out_buffer, err_buffer);
}

void console_client::process_event (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done)
{
	if (current_program_)
	{
		current_context_->postpone_done();
		auto context = current_context_;
		auto program = current_program_;
		program->my_program().process_program(current_context_, rx_time::now(), false);
		if (!context->is_postponed())
		{
			current_context_ = nullptr;
			current_program_ = server_console_program::smart_ptr::null_ptr;
			process_result(result, out_buffer, err_buffer);
		}
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
		, get_executer()
		);
	return true;
}

void console_client::synchronized_cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	if (current_program_)
	{// we are in a command
		current_context_->cancel_execution();
		process_event(true, out_buffer, err_buffer, true);
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
		, get_executer()
		);
	return true;
}

string_type console_client::get_terminal_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_TERM_NAME, RX_TERM_MAJOR_VERSION, RX_TERM_MINOR_VERSION, RX_TERM_BUILD_NUMBER);
	}
	return ret;
}

rx_result console_client::check_validity ()
{
	if (!current_directory_)
		return "No valid directory for Terminal";
	else
		return true;
}


// Class rx_platform::prog::server_script_host 

server_script_host::server_script_host (const script_def_t& definition)
{
}


server_script_host::~server_script_host()
{
}



// Class rx_platform::prog::console_program 

console_program::console_program()
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
	if (!first.empty())
	{
		server_command_base_ptr command = terminal::commands::server_command_manager::instance()->get_command_by_name(first);
		if (command)
		{
			if (!command->console_execute(in, out, err, ctx))
				return false;
		}
		else
		{
			err << "Syntax Error!\r\nCommand:" << first << " not existing!";
			err.flush();
			return false;
		}
	}
	if (ctx->is_canceled())
	{
		err << "Pending cancel for the current command:\r\n";
		err <<line;
		return false;
	}
	if (!ctx->is_postponed())
		ctx->next_line();

	return true;

}

sl_runtime::program_context* console_program::create_program_context (sl_runtime::program_context* parent_context, sl_runtime::sl_program_holder* holder)
{
	return new console_program_context(parent_context, holder, rx_directory_ptr::null_ptr, buffer_ptr::null_ptr, buffer_ptr::null_ptr, console_client::smart_ptr::null_ptr);
}


} // namespace prog
} // namespace rx_platform

