

/****************************************************************************
*
*  terminal\rx_console.cpp
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


// rx_console
#include "terminal/rx_console.h"

#include "sys_internal/rx_async_functions.h"
#include "rx_terminal_style.h"
#include "rx_commands.h"
#include "api/rx_platform_api.h"
#include "rx_con_commands.h"


namespace rx_internal {

namespace terminal {

namespace console {

// Class rx_internal::terminal::console::console_runtime 

console_runtime::console_runtime (rx_thread_handle_t executer, console_runtime_callback_t callback, security::security_guard_ptr guard)
      : executer_(executer),
        callback_(callback),
        term_width_(80),
        term_height_(24),
        security_guard_(guard)
{
	CONSOLE_LOG_DEBUG("console_runtime", 900, "Console endpoint created.");
#ifdef _DEBUG
	current_directory_ = "/world";// "_sys";
#else
	current_directory_ = "/world";
#endif
}


console_runtime::~console_runtime()
{
	CONSOLE_LOG_DEBUG("console_runtime", 900, "Console endpoint destroyed.");
}



void console_runtime::do_command (const string_type& line, security::security_context_ptr ctx)
{
	if (program_context_)
	{
		if (line == "\003")
		{// this is cancel
			cancel_command(ctx);
			return;
		}
	}
	else
	{
		// create main program
		current_program_ = rx_create_reference<console_runtime_program>();
		current_program_->load(line);
		// create context
		program_context_ = std::make_unique<console_runtime_program_context>(nullptr
				, current_program_, current_directory_
				, rx_create_reference<memory::std_buffer_type>()
				, rx_create_reference<memory::std_buffer_type>()
				, smart_this());

		program_context_->init_scan();
		program_context_->set_terminal_width(term_width_);
		program_context_->set_terminal_height(term_height_);

		runtime::runtime_process_context* ctx = nullptr;
		current_program_->process_program(program_context_.get(), *ctx);

	}
}

void console_runtime::process_event (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done)
{
	if (program_context_ && callback_)
	{
		if (done && result)
		{
			current_directory_ = program_context_->get_current_directory();
		}
		callback_(result, out_buffer, err_buffer, done);
		if (done)
		{
			program_context_.reset();
		}
	}
}

bool console_runtime::cancel_command (security::security_context_ptr ctx)
{
	security::secured_scope _(ctx);
	if (program_context_)
	{// we are in a command
		// !!!CONTODO!!!
		//current_context_->cancel_execution();
		//process_event(false, out_buffer, err_buffer, true);
	}
	else
	{// nothing to cancel!!!
		if (callback_)
		{
			auto err_buffer = rx_create_reference<memory::std_buffer_type>();
			std::ostream err(err_buffer.unsafe_ptr());
			err << "\r\nThere is nothing to cancel...";
			callback_(false, memory::buffer_ptr::null_ptr, err_buffer, true);
			return true;
		}
	}
	return false;
}

rx_result console_runtime::check_validity ()
{
	if (current_directory_.empty())
		return "No valid directory for Terminal";
	else
		return true;
}

void console_runtime::reset ()
{
}

void console_runtime::get_prompt (string_type& prompt)
{
	prompt += "\r\n";
	prompt += current_directory_;
}

void console_runtime::set_terminal_size (int width, int height)
{
	term_width_ = width;
	term_height_ = height;
}

void console_runtime::process_program (bool continue_scan)
{
	if (current_program_ && program_context_)
	{
		runtime::runtime_process_context* ctx = nullptr;
		current_program_->process_program(program_context_.get(), *ctx);
	}
}

security::security_guard_ptr console_runtime::get_security_guard ()
{
	return security_guard_;
}


// Class rx_internal::terminal::console::console_runtime_program_context 

console_runtime_program_context::console_runtime_program_context (program_context* parent, script::console_program_ptr runtime, const string_type& current_directory, buffer_ptr out, buffer_ptr err, rx_reference<console_runtime> host)
		: host_(host)
		, out_(out)
		, err_(err)
		, out_std_(out_.unsafe_ptr())
		, err_std_(err_.unsafe_ptr())
		, console_program_context(parent, runtime, current_directory, host->get_security_guard())
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
	ret.active_path = get_current_directory();
	ret.object = host_;
	return ret;
}

void console_runtime_program_context::send_results (bool result, bool done)
{
	host_->process_event(result, out_, err_, done);
	/*out_->reinit();
	err_->reinit();*/
}

void console_runtime_program_context::process_program (bool continue_scan)
{
	host_->process_program(continue_scan);
}

bool console_runtime_program_context::schedule_scan (uint32_t interval)
{
	RX_ASSERT(false);
	if (interval)
	{
		rx_post_delayed_function(interval, host_,
			[this]()
			{
			//	do_scan();
			});
	}
	else
	{
		rx_post_function(host_,
			[this]()
			{
			//	do_scan();
			});
	}
	return true;
}


// Class rx_internal::terminal::console::console_runtime_program 

console_runtime_program::console_runtime_program ()
{
}


console_runtime_program::~console_runtime_program()
{
}



std::unique_ptr<logic::program_context> console_runtime_program::create_program_context (logic::program_context* parent_context, security::security_guard_ptr guard)
{
	return std::make_unique<console_runtime_program_context>(parent_context
		, smart_this(), ""
		, rx_create_reference<memory::std_buffer_type>()
		, rx_create_reference<memory::std_buffer_type>()
		, console_runtime::smart_ptr());
}


} // namespace console
} // namespace terminal
} // namespace rx_internal

