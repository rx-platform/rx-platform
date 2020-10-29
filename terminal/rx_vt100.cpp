

/****************************************************************************
*
*  terminal\rx_vt100.cpp
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


// rx_vt100
#include "terminal/rx_vt100.h"

#include "rx_con_commands.h"
#define VT100_CURCOR_UP ""


namespace rx_internal {

namespace terminal {

namespace term_transport {

// Class rx_internal::terminal::term_transport::vt100_transport 

vt100_transport::vt100_transport (runtime::items::port_runtime* port, bool to_echo)
      : state_(parser_normal),
        current_idx_(string_type::npos),
        password_mode_(false),
        history_it_(history_.begin()),
        had_first_(false),
        opened_brackets_(0),
        send_echo_(to_echo),
        port_(port)
{
	std::ostringstream ss;
	ss << "Port "
		<< " created VT100 Transport Endpoint.";
	CONSOLE_LOG_TRACE("vt100_transport", 900, ss.str());
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &vt100_transport::received_function;
}



bool vt100_transport::move_cursor_left ()
{
	if (current_idx_ == string_type::npos && !current_line_.empty())
	{
		current_idx_ = current_line_.size() - 1;
		return true;
	}
	if (current_idx_ > 0)
	{
		current_idx_--;
		return true;
	}
	return false;
}

bool vt100_transport::move_cursor_right ()
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

bool vt100_transport::char_received (const char ch, bool eof, string_type& to_echo, string_type& line)
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
	case parser_had_bracket_number:
		return char_received_had_bracket_number(ch, to_echo);
	}
	return false;
}

bool vt100_transport::char_received_normal (const char ch, bool eof, string_type& to_echo, string_type& line)
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
		string_type offered;//!! = m_consumer->line_asked(current_line_);
		if (!offered.empty())
		{

			to_echo += "\033[1G\033[M";
			to_echo += offered;
			current_idx_ = string_type::npos;
			current_line_ = offered;
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

bool vt100_transport::char_received_in_end_line (char ch, string_type& to_echo, string_type& line)
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

bool vt100_transport::char_received_had_escape (const char ch, string_type& to_echo)
{
	switch (ch)
	{
	case '[':
		state_ = parser_had_bracket;
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

bool vt100_transport::char_received_had_bracket (char ch, string_type& to_echo)
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

bool vt100_transport::char_received_had_bracket_number (const char ch, string_type& to_echo)
{
	return false;
}

void vt100_transport::add_to_history (const string_type& line)
{
	history_it_ = history_.insert(history_it_,line);
	had_first_ = false;
}

bool vt100_transport::move_history_up (string_type& to_echo)
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

bool vt100_transport::move_history_down (string_type& to_echo)
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

rx_protocol_result_t vt100_transport::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	vt100_transport* self = reinterpret_cast<vt100_transport*>(reference->user_data);
	string_type to_echo;
	string_array lines;
	size_t i = 0;
	auto buffer = packet.buffer;

	for (; i < buffer->size; i++)
	{
		string_type line;
		self->char_received((char)buffer->buffer_ptr[i], i == buffer->size - 1, to_echo, line);
		if (!line.empty())
			lines.emplace_back(std::move(line));
	}
	rx_protocol_result_t result = RX_PROTOCOL_OK;
	if (self->send_echo_ && !to_echo.empty())
	{
		runtime::io_types::rx_io_buffer send_buffer(to_echo.size(), reference);
		auto temp = send_buffer.write_chars(to_echo);
		if (!temp)
		{
			result = RX_PROTOCOL_BUFFER_SIZE_ERROR;
		}
		else
		{
			send_protocol_packet down = rx_create_send_packet(packet.id, &send_buffer, 0, 0);
			result = rx_move_packet_down(reference, down);
		}
		if (result == RX_PROTOCOL_OK)
			send_buffer.detach(nullptr);
	}
	if (result == RX_PROTOCOL_OK && !lines.empty())
	{
		for (const auto& one : lines)
		{
			rx_const_packet_buffer up_buffer{ (const uint8_t*)one.c_str(), 0, one.size() };
			rx_init_const_packet_buffer(&up_buffer, one.c_str(), one.size());

			recv_protocol_packet packet = rx_create_recv_packet(0, &up_buffer, 0, 0);
			result = rx_move_packet_up(reference, packet);
			if (result != RX_PROTOCOL_OK)
				break;
		}
	}
	return result;
}

rx_protocol_stack_endpoint* vt100_transport::bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	sent_func_ = sent_func;
	received_func_ = received_func;
	return &stack_entry_;
}

void vt100_transport::set_echo (bool val)
{
}


// Class rx_internal::terminal::term_transport::vt100_transport_port 

vt100_transport_port::vt100_transport_port()
{
	construct_func = [this]()
	{
		auto rt = std::make_unique<vt100_transport>(this);
		auto entry = rt->bind([this](int64_t count)
			{
			},
			[this](int64_t count)
			{
			});
		return construct_func_type::result_type{ entry, std::move(rt) };
	};
}



} // namespace term_transport
} // namespace terminal
} // namespace rx_internal



// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
	

#endif
