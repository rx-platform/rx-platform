

/****************************************************************************
*
*  terminal\rx_console.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_console
#include "terminal/rx_console.h"

#include "protocols/ansi_c/internal_c/rx_internal_impl.h"
#include "system/server/rx_async_functions.h"
#include "rx_terminal_style.h"
#include "rx_terminal_version.h"
#include "rx_commands.h"


namespace terminal {

namespace console {

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

// Class terminal::console::console_endpoint 


rx_result console_endpoint::open (const string_type& addr, uint16_t port)
{
	rx_protocol_stack_entry* mine_entry = this;

	mine_entry->downward = nullptr;
	mine_entry->upward = nullptr;

	mine_entry->send_function = nullptr;
	mine_entry->sent_function = nullptr;
	mine_entry->received_function = (rx_received_function_type)&console_endpoint::received_function;

	mine_entry->close_function = nullptr;
	mine_entry->closed_function = nullptr;

	mine_entry->allocate_packet_function = nullptr;
	mine_entry->free_packet_function = nullptr;
	


	return true;
}

rx_result console_endpoint::close ()
{
	return true;
}

rx_protocol_result_t console_endpoint::received_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	console_endpoint* self = reinterpret_cast<console_endpoint*>(reference);
	if (self->my_console_)
	{
		string_type line;
		runtime::io_types::rx_const_io_buffer buff(buffer);
		auto result = buff.read_chars(line);
		if (result)
		{
			memory::buffer_ptr out_buffer(pointers::_create_new);
			memory::buffer_ptr err_buffer(pointers::_create_new);
			self->my_console_->do_command(std::move(line), out_buffer, err_buffer, rx_create_reference<security::unathorized_security_context>());
		}
	}
	return RX_PROTOCOL_OK;
}

void console_endpoint::bind (console_runtime* console, std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	my_console_ = console;
	received_func_ = received_func;
	rx_protocol_stack_entry* mine_entry = this;

	mine_entry->downward = nullptr;
	mine_entry->upward = nullptr;

	mine_entry->send_function = nullptr;
	mine_entry->sent_function = nullptr;
	mine_entry->received_function = &console_endpoint::received_function;

	mine_entry->connected_function = nullptr;

	mine_entry->close_function = nullptr;
	mine_entry->closed_function = nullptr;

	mine_entry->allocate_packet_function = nullptr;
	mine_entry->free_packet_function = nullptr;
}

rx_result console_endpoint::write (runtime::io_types::rx_io_buffer& what)
{
	rx_packet_buffer buff;
	what.detach(&buff);
	auto result = rx_move_packet_down(this, NULL, &buff);
	if (result == RX_PROTOCOL_OK)
		return true;
	else
		return rx_protocol_error_message(result);
}


// Class terminal::console::console_runtime 

console_runtime::console_runtime()
      : current_context_(nullptr)
{
#ifdef _DEBUG
	current_directory_ = rx_platform::rx_gate::instance().get_root_directory()->get_sub_directory("world");// "_sys");
#else
	current_directory_ = rx_platform::rx_gate::instance().get_root_directory()->get_sub_directory("world");
#endif
}


console_runtime::~console_runtime()
{
}



bool console_runtime::do_command (string_type&& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	string_type captured_line(std::move(line));
	rx_post_function<smart_ptr>(
		[captured_line, out_buffer, err_buffer, ctx](smart_ptr sended_this)
		{
			sended_this->synchronized_do_command(captured_line, out_buffer, err_buffer, ctx);
		}
		, smart_this()
			, get_executer()
			);
	return true;
}

void console_runtime::get_prompt (string_type& prompt)
{
	prompt = "\r\n";
	prompt += current_directory_->meta_info().get_full_path();
	prompt += "\r\n" ANSI_RX_USER;
	prompt += security::active_security()->get_full_name();
	prompt += ":" ANSI_COLOR_RESET;
	prompt += ">";
}

void console_runtime::get_wellcome (string_type& wellcome)
{
	wellcome = g_console_welcome;
	wellcome += ANSI_COLOR_BOLD ANSI_COLOR_GREEN ">>>> Running ";
	wellcome += get_console_terminal();
	wellcome += "\r\n";
	wellcome += "";
	wellcome += RX_CONSOLE_HEADER_LINE "\r\n" ANSI_COLOR_RESET;
}

bool console_runtime::is_postponed () const
{
	if (current_program_)
		return true;
	else
		return false;
}

const string_type& console_runtime::get_console_terminal ()
{
	static string_type ret(get_terminal_info());
	return ret;
}

void console_runtime::process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer)
{
	string_type prompt;
	bool exit = true;
	if (!rx_gate::instance().is_shutting_down())
	{
		exit = false;
		get_prompt(prompt);
	}
	size_t size = out_buffer->get_size() + err_buffer->get_size() + prompt.size();
	if (size)
	{
		runtime::io_types::rx_io_buffer send_buffer(size, &endpoint_);
		if (!result)
		{
			if (!out_buffer->empty())
				send_buffer.write(out_buffer->pbase(), out_buffer->get_size());
			if (!exit)
			{
				send_buffer.write_chars(
					"\r\n" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "\r\n"
				);
			}
			if (!err_buffer->empty())
				send_buffer.write(err_buffer->pbase(), err_buffer->get_size());
		}
		else
		{
			if (!out_buffer->empty())
				send_buffer.write(out_buffer->pbase(), out_buffer->get_size());
		}
		if(!prompt.empty())
			send_buffer.write_chars(prompt);
		endpoint_.write(send_buffer);

	}
}

void console_runtime::synchronized_do_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
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
		rx_platform::rx_gate::instance().shutdown("Interactive Shutdown");
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

		out << ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET "Terminal Information:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
		out << "Version: " << get_console_terminal() << "\r\n";
		fill_code_info(out, "jebiga");
		out << "\r\n";

		ret = true;
	}
	else if (line == "host")
	{
		std::ostream out(out_buffer.unsafe_ptr());
		std::ostream err(err_buffer.unsafe_ptr());

		string_type man = rx_gate::instance().get_host()->get_host_manual();

		out << man << "\r\n";

		out << "Hosts stack details:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
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
		string_type sys_info = rx_gate::instance().get_host()->get_storages().system_storage->get_storage_info();
		string_type sys_ref = rx_gate::instance().get_host()->get_storages().system_storage->get_storage_reference();
		string_type user_info = rx_gate::instance().get_host()->get_storages().user_storage->get_storage_info();
		string_type user_ref = rx_gate::instance().get_host()->get_storages().user_storage->get_storage_reference();
		string_type test_info = rx_gate::instance().get_host()->get_storages().test_storage->get_storage_info();
		string_type test_ref = rx_gate::instance().get_host()->get_storages().test_storage->get_storage_reference();
		out << ANSI_COLOR_GREEN "System Storage" ANSI_COLOR_RESET "\r\nReference: " << sys_ref << "\r\nVersion: " << sys_info << "\r\n\r\n";
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
	if (!is_postponed())
		process_result(ret, out_buffer, err_buffer);
}

void console_runtime::process_event (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done)
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

bool console_runtime::cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
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

void console_runtime::synchronized_cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
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

void console_runtime::get_security_error (string_type& txt, sec_error_num_t err_number)
{
	if (err_number == 0)
	{
		txt = g_console_unauthorized;
	}
}

bool console_runtime::do_commands (string_array&& lines, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	string_array captured_lines(std::move(lines));
	rx_post_function<smart_ptr>(
		[captured_lines, out_buffer, err_buffer, ctx](smart_ptr sended_this)
		{
			for (const auto& captured_line : captured_lines)
				sended_this->synchronized_do_command(captured_line, out_buffer, err_buffer, ctx);
		}
		, smart_this()
			, get_executer()
			);
	return true;
}

string_type console_runtime::get_terminal_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_TERM_NAME, RX_TERM_MAJOR_VERSION, RX_TERM_MINOR_VERSION, RX_TERM_BUILD_NUMBER);
	}
	return ret;
}

rx_result console_runtime::check_validity ()
{
	if (!current_directory_)
		return "No valid directory for Terminal";
	else
		return true;
}

void console_runtime::bind_port ()
{
	endpoint_.bind(this, [this](int64_t count)
		{
			update_sent_counters(count);
		},
		[this](int64_t count)
		{
			update_received_counters(count);
		});
}

rx_protocol_stack_entry* console_runtime::get_stack_entry ()
{
	return &endpoint_;
}

rx_result console_runtime::start_console ()
{
	return true;
}


// Class terminal::console::console_program_context 

console_program_context::console_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, rx_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<console_runtime> client)
      : client_(client),
        out_std_(out.unsafe_ptr()),
        err_std_(err.unsafe_ptr()),
        current_directory_(current_directory),
        out_(out),
        err_(err),
        postponed_(0),
        canceled_(false),
        result_(false),
        one_more_time_(false)
	, sl_runtime::sl_script::script_program_context(parent, holder, out.unsafe_ptr(), err.unsafe_ptr())
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
		client_->process_event(result, get_out(), get_err(), true);
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

bool console_program_context::one_more_time ()
{
	one_more_time_ = true;
	return true;
}

bool console_program_context::should_next_line ()
{
	if (one_more_time_)
	{
		one_more_time_ = false;
		return false;
	}
	return true;
}


// Class terminal::console::console_program 

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
		err << line;
		return false;
	}
	if (ctx->should_next_line())
	{
		ctx->next_line();
	}

	return true;
}

sl_runtime::program_context* console_program::create_program_context (sl_runtime::program_context* parent_context, sl_runtime::sl_program_holder* holder)
{
	return new console_program_context(parent_context, holder
		, rx_directory_ptr::null_ptr, buffer_ptr::null_ptr
		, buffer_ptr::null_ptr, console_runtime::smart_ptr::null_ptr);
}


// Class terminal::console::server_console_program 

server_console_program::server_console_program (console_runtime::smart_ptr client, const string_type& name, const rx_node_id& id, bool system)
	: program_runtime(name, id, system)
	, console_(client)
{
}


server_console_program::~server_console_program()
{
}



} // namespace console
} // namespace terminal

