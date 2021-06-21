

/****************************************************************************
*
*  terminal\rx_console.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_console
#include "terminal/rx_console.h"
// soft_plc
#include "soft_logic/soft_plc.h"

#include "sys_internal/rx_async_functions.h"
#include "rx_terminal_style.h"
#include "rx_terminal_version.h"
#include "rx_commands.h"
#include "api/rx_platform_api.h"
#include "rx_con_commands.h"


namespace rx_internal {

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

#define WELCOME_SPACES "  "

char g_console_welcome[] = ANSI_COLOR_YELLOW "\r\n\r\n\
                            ,p@@Np,\r\n\
                       ,gg@@@@@@@@@@@@@gg,\r\n\
                 ,g@@@@@/   \\@@N 'V |@@@@@@@@g,\r\n\
           ,gg@@@@@@@@@K I$\\ 'K@M\\  g@@@@@@@@@@@@Np,\r\n\
      ,g@@@@@@@@@@@@@@K  fM'  ]@@@  \\@@@@@@@@@@@@@@@@@@@g\r\n\
     '%@@@@@@@@@@@@@@K   g,  %@@@@'   \\@@@@@@@@@@@@@@@@@@P\r\n\
     gpR*N@@@@@@@@@@K   $@@b  %@P/  A   'B@@@@@@@@@@@R'Nmg\r\n\
    JK  gP''%B@@@@@K   $@@@b   @P  /@$   'B@@@@@@@@P'*%w- $\r\n\
   _@P  @     g@%@/   $@@@@@   \\C  g@@@\\   $@@@N@,     ]P Rw,\r\n\
  %Q-,gM'   ]| _,gP*@@@@@@@@@@@@@@@@@@@@@@@@**w,  ]P    *g, ]@\r\n\
           |[  ]|  *'N@@@@@@@@@@@@@@@@N'^     ]|  |_      ***\r\n\
          pP'  ]      ;pP'*B@@@@@@@@@*''Nw     ]|  '*mg\r\n\
          'Nwg**      @  ]@'<'N@@P''*w   ]r      '%gg*\r\n\
                      $. ]|          |[  $p\r\n\
                   pP'  ,@            @p  *'Np\r\n\
                   '%wgP'              '*Ng@'\r\n\
" ANSI_COLOR_RESET;


char g_console_welcome_old[] = ANSI_COLOR_YELLOW "\
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

// Class rx_internal::terminal::console::console_runtime 

console_runtime::console_runtime (runtime::items::port_runtime* port)
      : program_context_(nullptr),
        executer_(-1),
        port_(port)
{
	CONSOLE_LOG_TRACE("console_runtime", 900, "Console endpoint created.");
#ifdef _DEBUG
	current_directory_ = rx_platform::rx_gate::instance().get_root_directory()->get_sub_directory("world");// "_sys");
#else
	current_directory_ = rx_platform::rx_gate::instance().get_root_directory()->get_sub_directory("world");
#endif
	executer_ = port_->get_executer();
}


console_runtime::~console_runtime()
{
	CONSOLE_LOG_TRACE("console_runtime", 900, "Console endpoint destroyed.");
}



bool console_runtime::do_command (string_type&& line, security::security_context_ptr ctx)
{
	string_type captured_line(std::move(line));
	rx_post_function_to(get_executer(), smart_this(),
		[captured_line, ctx, this]()
		{
			synchronized_do_command(captured_line, ctx);
		});
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
	wellcome.clear();
	std::ostringstream ss;
	ss << "\r\n";
	ss << g_console_welcome;	
	ss << "\r\n\r\n         " ANSI_COLOR_BOLD ANSI_COLOR_GREEN;
	ss << get_console_terminal()
		<< ANSI_COLOR_RESET "\r\n\r\n" ;
	ss << ">Type " 
		<< ANSI_COLOR_YELLOW "\"help\"" ANSI_COLOR_RESET ", "
		<< ANSI_COLOR_YELLOW "\"copyright\"" ANSI_COLOR_RESET ", or "
		<< ANSI_COLOR_YELLOW "\"license\"" ANSI_COLOR_RESET " for more information."
		<< "\r\n";
	wellcome = ss.str();
}

string_type console_runtime::get_console_terminal ()
{
	return get_terminal_info();
}

void console_runtime::process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done)
{
	string_type prompt;
	bool exit = true;
	if (!rx_gate::instance().is_shutting_down() && done)
	{
		exit = false;
		get_prompt(prompt);
	}
	size_t size = out_buffer->get_size() + (err_buffer ? err_buffer->get_size() : 0) + prompt.size();
	if (size)
	{
		auto send_buffer = port_->alloc_io_buffer();
		if (send_buffer)
		{
			if (!result)
			{
				if (!out_buffer->empty())
					send_buffer.value().write(out_buffer->pbase(), out_buffer->get_size());
				if (!exit)
				{
					send_buffer.value().write_chars(
						"\r\n" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "\r\n"
					);
				}
				if (!err_buffer->empty())
					send_buffer.value().write(err_buffer->pbase(), err_buffer->get_size());
			}
			else
			{
				if (!out_buffer->empty())
					send_buffer.value().write(out_buffer->pbase(), out_buffer->get_size());
			}
			if (!prompt.empty())
				send_buffer.value().write_chars(prompt);

			if (send_buffer.value().size > 0)
			{
				send_protocol_packet packet = rx_create_send_packet(0, &send_buffer.value(), 0, 0);
				auto result = rx_move_packet_down(&stack_entry_, packet);
			}

			port_->release_io_buffer(send_buffer.move_value());
		}

	}
}

void console_runtime::synchronized_do_command (const string_type& line, security::security_context_ptr ctx)
{
	if (!context_ownership_)
	{
		if (line == "\003")
		{// this is cancel
			synchronized_cancel_command(ctx);
			return;
		}
	}
	else
	{
		/*if (line == "\t")
		{

			commands::suggestions_type suggestions;
			terminal::commands::server_command_manager::instance()->register_suggestions("", suggestions);

		}*/

		if (line.empty())
		{
			auto out_buffer = create_buffer();
			std::ostream out(out_buffer.unsafe_ptr());
			out << "\r\n";
			process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
		}
		else if (line[0] == '@')
		{// this is console command
			auto out_buffer = create_buffer();
			auto err_buffer = create_buffer();
			bool ret = rx_platform::rx_gate::instance().do_host_command(line.substr(1), out_buffer, err_buffer, ctx);
			process_result(ret, out_buffer, err_buffer, true);
		}
		else if (line == "exit")
		{
			rx_close(&stack_entry_, RX_PROTOCOL_OK);
		}
		else if (line == "hello")
		{
			auto out_buffer = create_buffer();
			std::ostream out(out_buffer.unsafe_ptr());
			out << "Hello to you too!!!";
			process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
		}
		else if (line == "term")
		{
			auto out_buffer = create_buffer();
			std::ostream out(out_buffer.unsafe_ptr());

			out << ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET "Terminal Information:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
			out << "Version: " << get_console_terminal() << "\r\n";
			port_->fill_code_info(out, "jebiga");
			out << "\r\n";
			process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
		}
		else if (line == "host")
		{
			auto out_buffer = create_buffer();
			auto err_buffer = create_buffer();
			std::ostream out(out_buffer.unsafe_ptr());
			std::ostream err(err_buffer.unsafe_ptr());

			string_type man = rx_gate::instance().get_host()->get_host_manual();

			out << man << "\r\n";

			out << "Hosts stack details:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
			hosting::hosts_type hosts;
			rx_gate::instance().get_host()->get_host_info(hosts);
			for (const auto& one : hosts)
			{
				out << ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET << one << "\r\n";
			}
			process_result(true, out_buffer, err_buffer, true);
		}
		else if (line == "storage")
		{
			auto out_buffer = create_buffer();
			std::ostream out(out_buffer.unsafe_ptr());
			out << "Storage Information:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
			rx_gate::instance().get_host()->dump_storage_references(out);
			out << "\r\n";
			process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
		}
		else if (line == "welcome")
		{
			auto out_buffer = create_buffer();
			std::ostream out(out_buffer.unsafe_ptr());

			string_type msg;
			get_wellcome(msg);

			out << msg;
			process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
		}
		else
		{
			// create main program
			auto program = std::make_unique<script::console_program>();
			program->load(line);
			program_holder_.set_main_program(std::move(program));
			program_context_->init_scan();
			program_context_ = nullptr;
			std::unique_ptr<sl_runtime::program_context> temp_ctx(context_ownership_.release());
			program_holder_.start_program(program_executer_.get(), std::move(temp_ctx));
		}
	}
}

void console_runtime::process_event (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done)
{
	if (stack_entry_.received_function)// if we're connected
	{
		if (!program_context_)
		{
			if (done)
			{
				context_ownership_ = std::move(program_holder_.stop_program());
				RX_ASSERT(context_ownership_);
				if (context_ownership_)
				{
					program_context_ = static_cast<script::console_program_context*>(context_ownership_.get());
					if(result)
						current_directory_ = program_context_->get_current_directory();
				}
			}
			process_result(result, out_buffer, err_buffer, done);
		}
	}
}

bool console_runtime::cancel_command (security::security_context_ptr ctx)
{
	rx_post_function_to(get_executer(), smart_this(),
		[ctx, this]()
		{
			synchronized_cancel_command(ctx);
		});
	return true;
}

void console_runtime::synchronized_cancel_command (security::security_context_ptr ctx)
{
	if (!program_context_)
	{// we are in a command
		// !!!CONTODO!!!
		//current_context_->cancel_execution();
		//process_event(false, out_buffer, err_buffer, true);
	}
	else
	{// nothing to cancel!!!
		auto err_buffer = create_buffer();
		std::ostream err(err_buffer.unsafe_ptr());
		err << "\r\nThere is nothing to cancel...";
		process_result(false, memory::buffer_ptr::null_ptr, err_buffer, true);
	}
}

void console_runtime::get_security_error (string_type& txt, sec_error_num_t err_number)
{
	if (err_number == 0)
	{
		txt = g_console_unauthorized;
	}
}

bool console_runtime::do_commands (string_array&& lines, security::security_context_ptr ctx)
{
	rx_post_function_to(get_executer(), smart_this(),
		[ctx, this](string_array&& lines)
		{
			for (const auto& captured_line : lines)
				synchronized_do_command(captured_line, ctx);
		}, std::move(lines));
	return true;
}

string_type console_runtime::get_terminal_info ()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
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

rx_protocol_stack_endpoint* console_runtime::bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &console_runtime::received_function;
	stack_entry_.connected_function = &console_runtime::connected_function;

	// program executer
	program_executer_ = std::make_unique<console_runtime_program_executer>(&program_holder_, smart_this()
		, security::active_security());
	// create program context
	context_ownership_ = std::make_unique<console_runtime_program_context>(nullptr
		, &program_holder_, current_directory_
		, create_buffer(), create_buffer()
		, smart_this());
	program_context_ = static_cast<script::console_program_context*>(context_ownership_.get());

	return &stack_entry_;
}

rx_protocol_result_t console_runtime::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	console_runtime* self = reinterpret_cast<console_runtime*>(reference->user_data);
	string_type line;
	runtime::io_types::rx_const_io_buffer buff(packet.buffer);
	auto result = buff.read_chars(line);
	if (result)
	{
		auto idx = line.find(RX_LINE_END_CH);
		if (idx != string_type::npos)
		{// we have more then one line at once
			string_array lines;
			size_t idx_first = 0;
			do
			{
				lines.emplace_back(line.substr(idx_first, idx - idx_first));
				idx_first = idx;
				idx_first++;
				idx = line.find(RX_LINE_END_CH, idx + 1);
			} while (idx != string_type::npos);
			
			self->do_commands(std::move(lines), rx_create_reference<security::unathorized_security_context>());
		}
		else
		{
			self->do_command(std::move(line), rx_create_reference<security::unathorized_security_context>());
		}
	}
	return RX_PROTOCOL_OK;
}

rx_protocol_result_t console_runtime::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
	console_runtime* self = reinterpret_cast<console_runtime*>(reference->user_data);
	memory::buffer_ptr out_buffer(pointers::_create_new);
	memory::buffer_ptr err_buffer(pointers::_create_new);

	std::ostream out(out_buffer.unsafe_ptr()); 
	out << ANSI_COLOR_BOLD ANSI_COLOR_YELLOW ">>>" ANSI_COLOR_RESET "Hello!\r\n";
	out << ANSI_COLOR_BOLD ANSI_COLOR_YELLOW ">>>" ANSI_COLOR_RESET "Connecting terminal as ";
	out << ANSI_COLOR_BOLD ANSI_COLOR_GREEN
		<< security::active_security()->get_full_name()
		<< ANSI_COLOR_RESET;
	out << "...\r\n";
	self->process_result(true, out_buffer, err_buffer, true);
	self->do_command("welcome", rx_create_reference<security::unathorized_security_context>());
	return RX_PROTOCOL_OK;
}

void console_runtime::close_endpoint ()
{
	stack_entry_.received_function = nullptr;
	program_executer_.reset();
}

memory::buffer_ptr console_runtime::create_buffer ()
{
	memory::buffer_ptr out_buffer(pointers::_create_new);
	return out_buffer;
}


// Class rx_internal::terminal::console::console_port 

console_port::console_port()
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<console_runtime>(this);
		auto entry =  rt->bind_endpoint([this](int64_t count)
			{
			},
			[this](int64_t count)
			{
			});
		return construct_func_type::result_type{ entry, rt };
	};
}



void console_port::stack_assembled ()
{
	auto result = listen(nullptr, nullptr);
}


// Class rx_internal::terminal::console::console_runtime_program_executer 

console_runtime_program_executer::console_runtime_program_executer (sl_runtime::sl_program_holder* program, rx_reference<console_runtime> host, security::security_context_ptr sec_context)
      : host_(host),
        program_context_(nullptr),
        sec_context_(sec_context)
	, sl_runtime::program_executer(program)
{
}



void console_runtime_program_executer::start_program (uint32_t rate, std::unique_ptr<sl_runtime::program_context>&& context)
{
	program_context_ = static_cast<script::console_program_context*>(context.get());
	program_executer::start_program(rate, std::move(context));
	do_scan();
}

std::unique_ptr<sl_runtime::program_context> console_runtime_program_executer::stop_program ()
{
	std::unique_ptr<sl_runtime::program_context>&& ret = sl_runtime::program_executer::stop_program();
	program_context_ = nullptr;
	return std::move(ret);
}

void console_runtime_program_executer::schedule_scan (uint32_t interval)
{
	if (interval)
	{
		rx_post_delayed_function(host_, interval,
			[this]()
			{
				do_scan();
			});
	}
	else
	{
		rx_post_function(host_,
			[this]()
			{
				do_scan();
			});
	}
}

void console_runtime_program_executer::do_scan ()
{
	security::secured_scope _(sec_context_);
	program_scan();
}


// Class rx_internal::terminal::console::console_runtime_program_context 

console_runtime_program_context::console_runtime_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, rx_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<console_runtime> runtime)
		: host_(runtime)
		, out_(out)
		, err_(err)
		, out_std_(out_.unsafe_ptr())
		, err_std_(err_.unsafe_ptr())
		, console_program_context(parent, holder, current_directory)
{
}

console_runtime_program_context::console_runtime_program_context (console_runtime_program_context&& right)
	: host_(right.host_)
	, out_(std::move(right.out_))
	, err_(std::move(right.err_))
	, out_std_(out_.unsafe_ptr())
	, err_std_(err_.unsafe_ptr())
	, console_program_context(right.parent_, right.get_program_holder(), right.get_current_directory())
{
}


console_runtime_program_context::~console_runtime_program_context()
{
}



std::ostream& console_runtime_program_context::get_stdout ()
{
	return out_std_;
}

std::ostream& console_runtime_program_context::get_stderr ()
{
	return err_std_;
}

api::rx_context console_runtime_program_context::create_api_context ()
{
	api::rx_context ret;
	ret.directory = get_current_directory();
	ret.object = host_;
	return ret;
}

void console_runtime_program_context::send_results (bool result, bool done)
{

	host_->process_event(result, out_, err_, done);
	out_->reinit();
	err_->reinit();
}


} // namespace console
} // namespace terminal
} // namespace rx_internal

