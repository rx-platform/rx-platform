

/****************************************************************************
*
*  terminal\rx_vt100.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


// rx_vt100
#include "terminal/rx_vt100.h"

#include "rx_terminal_version.h"
#include "system/meta/rx_useful_queries.h"
#include "api/rx_namespace_api.h"
#include "rx_term_table.h"
using namespace rx;
#define VT100_CURCOR_UP ""


namespace rx_internal {

namespace terminal {

namespace term_ports {

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

// Class rx_internal::terminal::term_ports::vt100_endpoint 

vt100_endpoint::vt100_endpoint (runtime::items::port_runtime* port, security::security_guard_ptr guard, bool to_echo)
      : state_(parser_normal),
        current_idx_(string_type::npos),
        password_mode_(false),
        history_it_(history_.begin()),
        had_first_(false),
        opened_brackets_(0),
        send_echo(to_echo),
        port_(port)
{
	CONSOLE_LOG_TRACE("vt100_endpoint", 900, "VT-100 Endpoint created.");
	rx_init_stack_entry(&stack_entry, this);
	stack_entry.received_function = &vt100_endpoint::received_function;
	stack_entry.connected_function = &vt100_endpoint::connected_function;
	console_program_ = rx_create_reference<console::console_runtime>(port_->get_executer(),
		[this](bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done)
		{
			process_result(result, out_buffer, err_buffer, done);
		}, guard);
	pull_timer_ = rx_timer_ptr::null_ptr;
}


vt100_endpoint::~vt100_endpoint()
{
	CONSOLE_LOG_TRACE("vt100_endpoint", 900, "VT-100 Endpoint destroyed.");
	if (pull_timer_)
		pull_timer_->cancel();
}



bool vt100_endpoint::move_cursor_left ()
{
	if (current_idx_ != string_type::npos && !current_line_.empty() && current_idx_ > 0)
	{
		current_idx_--;
		if(current_idx_==0)
			current_idx_ = string_type::npos;
		return true;
	}
	return false;
}

bool vt100_endpoint::move_cursor_right ()
{
	if (current_idx_ != string_type::npos && !current_line_.empty() && current_idx_<current_line_.size())
	{
		current_idx_++;
		if (current_idx_ == current_line_.size())
			current_idx_ = string_type::npos;
		return true;
	}
	return false;
}

bool vt100_endpoint::char_received (const char ch, bool eof, string_type& to_echo, string_type& line)
{
	if (ch == '\003')
	{// this is cancel
		state_ = parser_normal;
		line = "\003";
		return false;
	}
	switch (state_)
	{
	case parser_normal:
		return char_received_normal(ch, eof, to_echo,line);
	case parser_in_end_line:
		return char_received_in_end_line(ch, to_echo,line);
	case parser_had_escape:
		return char_received_had_escape(ch, to_echo);
	case parser_had_bracket:
		return char_received_had_bracket(ch, to_echo);
	case parser_had_os_command:
		return char_received_had_os(ch, to_echo);
	case parser_had_bracket_number:
		return char_received_had_bracket_number(ch, to_echo);
	}
	return false;
}

bool vt100_endpoint::char_received_normal (const char ch, bool eof, string_type& to_echo, string_type& line)
{
	switch (ch)
	{
	case '\x1b':
		state_ = parser_had_escape;
		break;
	case '\b':
	case '\x7f':
		if (current_idx_ > 0 && !current_line_.empty())
		{
			if (current_idx_ != string_type::npos)
			{
				to_echo += "\x08\033[1P";
				current_line_.erase(current_idx_ - 1, 1);
				move_cursor_left();
			}
			else
			{
				current_line_.pop_back();
				to_echo += "\x08 \x08";
			}
		}
		break;
	case '\n':
	case '\r':
		to_echo += "\r\n";

		if (opened_brackets_ <= 0)
		{
			opened_brackets_ = 0;
			line = current_line_;

			current_line_.clear();
			current_idx_ = string_type::npos;
			if (!eof)
				state_ = parser_in_end_line;
		}
		else
			current_line_ = current_line_ + ch;
		break;
	case '\t':
		{
			if (!current_line_.empty() && eof)
			{
				string_type what;
				auto idx = current_line_.rfind(' ');
				if (idx != string_type::npos)
				{
					what = current_line_.substr(idx + 1);
					if (!what.empty())
					{
						auto query = std::make_shared<meta::queries::ns_suggetions_query>();
						query->suggested_path = what;
						api::rx_context ctx;
						ctx.active_path = console_program_->get_current_directory();
						ctx.object = smart_this();

						rx_result_with_callback<api::query_result> callback(ctx.object,
							[this, what](rx_result_with<api::query_result>&& result) mutable
							{
								if (result)
								{
									bool had_options = false;
									// trim what
									auto idx = what.find_first_not_of("./");
									if (idx != string_type::npos)
										what = what.substr(idx);
									if (result.value().items.size() > 0)
									{
										string_type suggested;
										if (result.value().items.size() == 1)
										{
											suggested = result.value().items[0].data.get_full_path();

											idx = suggested.rfind(what);
											if (idx != string_type::npos)
											{
												suggested = suggested.substr(idx + what.size());
											}
											else
											{
												return;
											}
										}
										else
										{
											string_array suggestions;
											string_type temp_suggested;
											for (auto one : result.value().items)
											{
												temp_suggested = one.data.get_full_path();

												idx = temp_suggested.rfind(what);
												if (idx != string_type::npos)
												{
													temp_suggested = temp_suggested.substr(idx + what.size());
													if (temp_suggested.empty())
														return;// we have exact match
													else
														suggestions.emplace_back(temp_suggested);
												}
												else
												{
													return;
												}
											}



											size_t idx = 0;
											bool done = false;
											while(!done)
											{
												bool had_one = false;
												for (auto one : suggestions)
												{
													if (one.size() <= idx)
													{
														done = true;
														break;
													}
													if (!had_one)
													{
														suggested.push_back(one[idx]);
														had_one = true;
													}
													else
													{
														if (suggested[idx] != one[idx])
														{
															done = true;
															suggested.pop_back();
															break;
														}
													}
												}
												idx++;
											}
											if (suggested.empty())
											{
												std::ostringstream strs;
												strs << "\r\n";
												rx_row_type row;
												for (auto& one : suggestions)
													row.emplace_back(what + one);
												rx_dump_large_row(row, strs, 80);
												string_type prompt;
												get_prompt(prompt);
												strs << prompt;
												strs << current_line_;
												suggested = strs.str();
												had_options = true;
											}
										}
										if (!suggested.empty())
										{
											auto sbuff_res = port_->alloc_io_buffer();
											if (sbuff_res)
											{
												if(!had_options)
													current_line_ += suggested;
												io::rx_io_buffer sbuffer = sbuff_res.move_value();
												sbuffer.write_chars(suggested);
												rx_move_packet_down(&stack_entry, rx_create_send_packet(0, &sbuffer, 0, 0));
											}
										}
									}
								}
								else
								{
								}

							});

						rx_result result = api::ns::rx_query_model({ query }, std::move(callback), ctx);

					}
				}
			}
		}
		break;
	default:
		if (ch >= 0x20)
		{
			if (current_idx_ != string_type::npos)
			{
				current_line_.insert(current_idx_, 1, ch);
				to_echo += "\033[1@";
				to_echo += ch;
				move_cursor_right();
				//to_echo.append("\033C");
			}
			else
			{
				if (ch == '{')
				{
					opened_brackets_++;
				}
				else if (ch == '}')
				{
					opened_brackets_--;
				}
				current_line_ = current_line_ + ch;
				to_echo = to_echo + (password_mode_ ? '*' : ch);
			}
		}
	}
	return true;
}

bool vt100_endpoint::char_received_in_end_line (char ch, string_type& to_echo, string_type& line)
{
	state_ = parser_normal;
	switch (ch)
	{
	case '\r':
	case '\n':
		break;
	default:
		current_line_.clear();
		current_idx_ = string_type::npos;
		return char_received_normal(ch, true, to_echo, line);
	}
	return true;
}

bool vt100_endpoint::char_received_had_escape (const char ch, string_type& to_echo)
{
	switch (ch)
	{
	case '[':
		state_ = parser_had_bracket;
		break;
	case '\\':// ST command
		state_ = parser_normal;
		return false;
	case ']':
		state_ = parser_had_os_command;
		break;
	case 'D':
		if (current_idx_ != 0 && !current_line_.empty())
		{
			to_echo += "\033[1G";
			current_idx_ = 0;
		}
		state_ = parser_normal;
		break;
	case 'C':
		if (current_idx_ != string_type::npos && !current_line_.empty())
		{
			char buff[0x10];
			sprintf(buff, "\033[%dG", (int)current_line_.size());
			to_echo += buff;
			current_idx_ = string_type::npos;
		}
		state_ = parser_normal;
		break;
	default:
		state_ = parser_normal;
		return false;
	}
	return true;
}

bool vt100_endpoint::char_received_had_bracket (char ch, string_type& to_echo)
{
	switch (ch)
	{
	case 'A':
		state_ = parser_normal;
		return move_history_up(to_echo);
		break;
	case 'B':
		state_ = parser_normal;
		return move_history_down(to_echo);
		break;
	case 'D':
		if (move_cursor_left())
			to_echo.append("\033[D");
		state_ = parser_normal;
		break;
	case 'C':
		if (move_cursor_right())
			to_echo.append("\033[C");
		state_ = parser_normal;
		break;
	case 'H':
		while (move_cursor_left())
			to_echo.append("\033[D");
		state_ = parser_normal;
		break;
	case 'F':
		while (move_cursor_right())
			to_echo.append("\033[C");
		state_ = parser_normal;
		break;
	default:
		state_ = parser_normal;
		return false;
	}
	return true;
}

bool vt100_endpoint::char_received_had_os (char ch, string_type& to_echo)
{
	switch (ch)
	{
	case '\x1b':
		state_ = parser_had_escape;
		{
			std::istringstream ss(os_command_);
			string_type comm;
			ss >> comm;
			if (comm == "T")
			{
				int w, h;
				ss >> w;
				ss >> h;
				if (console_program_)
					console_program_->set_terminal_size(w, h);
			}
			os_command_.clear();
		}
		break;
	default:
		os_command_ += ch;
		return false;
	}
	return true;
}

bool vt100_endpoint::char_received_had_bracket_number (const char ch, string_type& to_echo)
{
	return false;
}

void vt100_endpoint::add_to_history (const string_type& line)
{
	history_it_ = history_.insert(history_it_,line);
	had_first_ = false;
}

bool vt100_endpoint::move_history_up (string_type& to_echo)
{
	if (had_first_ && history_it_ != history_.end())
	{
		string_type line(*history_it_);
		if (history_it_ != history_.begin())
			history_it_--;
		else
			had_first_ = true;
		to_echo += "\033[1G\033[M";
		to_echo += line;
		current_idx_ = string_type::npos;
		current_line_ = line;
		return true;
	}
	return false;
}

bool vt100_endpoint::move_history_down (string_type& to_echo)
{
	if (history_it_ != history_.end())
	{
		string_type line(*history_it_);
		history_it_++;
		to_echo += "\033[1G\033[M";
		to_echo += line;
		current_idx_ = string_type::npos;
		current_line_ = line;
	}
	return true;
}

void vt100_endpoint::set_echo (bool val)
{
	send_echo = val;
}

rx_protocol_result_t vt100_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	vt100_endpoint* self = reinterpret_cast<vt100_endpoint*>(reference->user_data);
	string_type to_echo;
	string_array lines_buffers;
	string_array::reverse_iterator current_line_it;
	size_t i = 0;
	auto buffer = packet.buffer;

	for (; i < buffer->size; i++)
	{
		string_type line;
		auto is_normal = self->char_received((char)buffer->buffer_ptr[i], i == buffer->size - 1, to_echo, line);
		if (!line.empty())
		{
			if (!is_normal)
			{// this is cancel, or tab, or something like that so leave it up
				if (!lines_buffers.empty())
					(*current_line_it) += RX_LINE_END;// handle previous lines
				lines_buffers.emplace_back(std::move(line));// no new line
				current_line_it = lines_buffers.rbegin();
			}
			if (lines_buffers.empty())
			{
				lines_buffers.emplace_back(std::move(line) + RX_LINE_END);
				current_line_it = lines_buffers.rbegin();
			}
			else
			{
				(*current_line_it) += ((std::move(line) + RX_LINE_END));
				current_line_it = lines_buffers.rbegin();
			}
		}
		//lines_buffer += (std::move(line) + RX_LINE_END);
	}
	rx_protocol_result_t result = RX_PROTOCOL_OK;
	if (self->send_echo && !to_echo.empty())
	{
		auto send_buffer = self->port_->alloc_io_buffer();
		if (!send_buffer)
		{
			result = RX_PROTOCOL_OUT_OF_MEMORY;
		}
		else
		{
			auto temp = send_buffer.value().write_chars(to_echo);
			if (!temp)
			{
				result = RX_PROTOCOL_BUFFER_SIZE_ERROR;
			}
			else
			{
				send_protocol_packet down = rx_create_send_packet(packet.id, &send_buffer.value(), 0, 0);
				result = rx_move_packet_down(reference, down);
			}
			self->port_->release_io_buffer(send_buffer.move_value());
		}
	}
	if (result == RX_PROTOCOL_OK && !lines_buffers.empty())
	{
		if (self->console_program_)
		{
			if (lines_buffers.size() > 1)
			{
				self->do_commands(std::move(lines_buffers), rx_create_reference<security::unathorized_security_context>());
			}
			else
			{
				self->do_command(std::move(lines_buffers[0]), rx_create_reference<security::unathorized_security_context>());
			}
		}
		lines_buffers.clear();
	}
	return result;
}



#define ANSI_CUR_SAVE_POS "\x1b[s"
#define ANSI_CUR_RESTORE_POS "\x1b[u"

#define ANSI_CUR_SAVE "\x1b" "7"
#define ANSI_CUR_RESTORE "\x1b" "8"

rx_protocol_result_t vt100_endpoint::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
	vt100_endpoint* self = reinterpret_cast<vt100_endpoint*>(reference->user_data);
	memory::buffer_ptr out_buffer(pointers::_create_new);
	memory::buffer_ptr err_buffer(pointers::_create_new);

	std::ostream out(out_buffer.unsafe_ptr());
	out << ANSI_COLOR_BOLD ANSI_COLOR_YELLOW ">>>" ANSI_COLOR_RESET "Hello!\r\n";
	out << ANSI_COLOR_BOLD ANSI_COLOR_YELLOW ">>>" ANSI_COLOR_RESET "Connecting terminal as ";
	out << ANSI_COLOR_BOLD ANSI_COLOR_GREEN
		<< security::active_security()->get_full_name()
		<< ANSI_COLOR_RESET;
	out << "...\r\n";

	string_type msg;
	self->get_wellcome(msg);

	out << msg;
	self->process_result(true, out_buffer, err_buffer, true);

	self->pull_timer_ = self->port_->create_timer_function([self]()
		{

			/*auto send_buffer = self->port_->alloc_io_buffer();
			if (send_buffer)
			{
				string_type str_prefix = ANSI_CUR_SAVE ANSI_COLOR_BOLD ANSI_COLOR_GREEN ANSI_CUR(0,0);
				string_type str_postfix = ANSI_COLOR_RESET ANSI_CUR_RESTORE;
				send_buffer.value().write_chars(str_prefix);
				send_buffer.value().write_chars("Jeeeee!!!");
				send_buffer.value().write_chars(str_postfix);

				send_protocol_packet packet = rx_create_send_packet(0, &send_buffer.value(), 0, 0);
				auto result = rx_move_packet_down(&self->stack_entry, packet);

				self->port_->release_io_buffer(send_buffer.move_value());
			}*/
			 
		});

	self->pull_timer_->start(200);
	return RX_PROTOCOL_OK;
}

bool vt100_endpoint::do_command (string_type&& line, security::security_context_ptr ctx)
{
	port_->send_function(
		[ctx, this](string_type&& line)
		{
			synchronized_do_command(std::move(line), ctx);
		}, std::move(line));
	return true;
}

bool vt100_endpoint::do_commands (string_array&& lines, security::security_context_ptr ctx)
{
	port_->send_function(
		[ctx, this](string_array&& lines)
		{
			for (const auto& captured_line : lines)
				synchronized_do_command(captured_line, ctx);
		}, std::move(lines));
	return true;
}

void vt100_endpoint::synchronized_cancel_command (security::security_context_ptr ctx)
{
}

void vt100_endpoint::synchronized_do_command (const string_type& in_line, security::security_context_ptr ctx)
{
	// string end line
	string_type line(in_line);
	while (!line.empty() && (*line.rbegin() == '\r' || *line.rbegin() == '\n'))
		line.pop_back();

	if (line.empty())
	{
		auto out_buffer = rx_create_reference< memory::std_buffer_type>();
		std::ostream out(out_buffer.unsafe_ptr());
		out << "\r\n";
		process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
	}
	else if (line[0] == '@')
	{// this is host command
		auto out_buffer = rx_create_reference< memory::std_buffer_type>();
		auto err_buffer = rx_create_reference< memory::std_buffer_type>();
		bool ret = rx_platform::rx_gate::instance().do_host_command(line.substr(1), out_buffer, err_buffer, ctx);
		process_result(ret, out_buffer, err_buffer, true);
	}
	else if (line == "exit")
	{
		rx_close(&stack_entry, RX_PROTOCOL_OK);
	}
	else if (line == "hello")
	{
		auto out_buffer = rx_create_reference< memory::std_buffer_type>();
		std::ostream out(out_buffer.unsafe_ptr());
		out << "Hello to you too!!!\r\nNow from VT-100 Endpoint...";
		process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
	}
	else if (line == "term")
	{
		auto out_buffer = rx_create_reference< memory::std_buffer_type>();
		std::ostream out(out_buffer.unsafe_ptr());

		out << ANSI_COLOR_GREEN "$>" ANSI_COLOR_RESET "Terminal Information:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
		out << "Version: " << get_terminal_info() << "\r\n";
		out << "Instance Name: " << rx_gate::instance().get_instance_name() << "\r\n";
		out << "Node Name: " << rx_get_node_name() << "\r\n";
		port_->fill_code_info(out, "vt100_endpoint");
		out << "\r\n";
		process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
	}
	else if (line == "host")
	{
		auto out_buffer = rx_create_reference< memory::std_buffer_type>();
		auto err_buffer = rx_create_reference< memory::std_buffer_type>();
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
		auto out_buffer = rx_create_reference< memory::std_buffer_type>();
		std::ostream out(out_buffer.unsafe_ptr());
		out << "Storage Information:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
		rx_gate::instance().get_host()->dump_storage_references(out);
		out << "\r\n";
		process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
	}
	else if (line == "welcome")
	{
		auto out_buffer = rx_create_reference< memory::std_buffer_type>();
		std::ostream out(out_buffer.unsafe_ptr());

		string_type msg;
		get_wellcome(msg);

		out << msg;
		process_result(true, out_buffer, memory::buffer_ptr::null_ptr, true);
	}
	else if (console_program_)
	{
		console_program_->do_command(line, ctx);
	}
	else
	{
		auto out_buffer = rx_create_reference< memory::std_buffer_type>();
		auto err_buffer = rx_create_reference< memory::std_buffer_type>();
		std::ostream out(out_buffer.unsafe_ptr());
		std::ostream err(err_buffer.unsafe_ptr());
		err << "Error while parsing the line:"
			<< line;
		out << RX_NULL_ITEM_NAME;
		out << "\r\n";
		process_result(false, out_buffer, err_buffer, true);
	}
}

void vt100_endpoint::process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done)
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
				auto result = rx_move_packet_down(&stack_entry, packet);
			}

			port_->release_io_buffer(send_buffer.move_value());
		}

	}
}

string_type vt100_endpoint::get_terminal_info ()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_TERM_NAME, RX_TERM_MAJOR_VERSION, RX_TERM_MINOR_VERSION, RX_TERM_BUILD_NUMBER);
	}
	return ret;
}

void vt100_endpoint::get_prompt (string_type& prompt)
{
	if (console_program_)
		console_program_->get_prompt(prompt);
	prompt += "\r\n" ANSI_RX_USER;
	prompt += security::active_security()->get_full_name();
	prompt += ":" ANSI_COLOR_RESET;
	prompt += ">";
}

void vt100_endpoint::get_wellcome (string_type& wellcome)
{
	wellcome.clear();
	std::ostringstream ss;
	ss << "\r\n";
	ss << g_console_welcome;
	ss << "\r\n\r\n         " ANSI_COLOR_BOLD ANSI_COLOR_GREEN;
	ss << get_terminal_info()
		<< ANSI_COLOR_RESET "\r\n\r\n";
	ss << ">Type "
		<< ANSI_COLOR_YELLOW "\"help\"" ANSI_COLOR_RESET ", "
		<< ANSI_COLOR_YELLOW "\"copyright\"" ANSI_COLOR_RESET ", or "
		<< ANSI_COLOR_YELLOW "\"license\"" ANSI_COLOR_RESET " for more information."
		<< "\r\n";
	wellcome = ss.str();
}

void vt100_endpoint::close_endpoint ()
{
	stack_entry.received_function = nullptr;
	if (pull_timer_)
	{
		pull_timer_->cancel();
		pull_timer_ = rx_timer_ptr::null_ptr;
	}
	if (console_program_)
		console_program_->reset();
}


// Class rx_internal::terminal::term_ports::vt100_port 

vt100_port::vt100_port()
{
	construct_func = [this]()
	{
		auto rt = rx_create_reference<vt100_endpoint>(this, security_guard_);
		return construct_func_type::result_type{ &rt->stack_entry, rt };
	};
}



void vt100_port::stack_assembled ()
{
	auto result = listen(nullptr, nullptr);
	if (!result)
	{
		// handle the fucking error!!!
		CONSOLE_LOG_WARNING("VT-100", 200, "Listen failed on (nullptr, nullptr):" + result.errors_line());
	}
}

rx_result vt100_port::initialize_runtime (runtime_init_context& ctx)
{
	security_guard_ = ctx.context->get_security_guard();
	return true;
}


} // namespace term_ports
} // namespace terminal
} // namespace rx_internal

