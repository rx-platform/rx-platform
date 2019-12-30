

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

#define VT100_CURCOR_UP ""


namespace terminal {

namespace rx_vt100 {

// Class terminal::rx_vt100::vt100_transport 

vt100_transport::vt100_transport()
      : state_(parser_normal),
        current_idx_(string_type::npos),
        password_mode_(false),
        history_it_(history_.begin()),
        had_first_(false),
        opened_brackets_(0)
{
}


vt100_transport::~vt100_transport()
{
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

bool vt100_transport::char_received (const char ch, bool eof, string_type& to_echo, std::function<void(string_type)> received_line_callback)
{
	switch (state_)
	{
	case parser_normal:
		return char_received_normal(ch, eof, to_echo,received_line_callback);
	case parser_in_end_line:
		return char_received_in_end_line(ch, to_echo,received_line_callback);
	case parser_had_escape:
		return char_received_had_escape(ch, to_echo);
	case parser_had_bracket:
		return char_received_had_bracket(ch, to_echo);
	case parser_had_bracket_number:
		return char_received_had_bracket_number(ch, to_echo);
	}
	return false;
}

bool vt100_transport::char_received_normal (const char ch, bool eof, string_type& to_echo, std::function<void(string_type)> received_line_callback)
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
			received_line_callback(current_line_);

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

bool vt100_transport::char_received_in_end_line (char ch, string_type& to_echo, std::function<void(string_type)> received_line_callback)
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
		return char_received_normal(ch, true, to_echo, received_line_callback);
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

rx_protocol_result_t vt100_transport::send_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer)
{
	// just pass througth
	return rx_move_packet_down(reference, end_point, buffer);
}

rx_protocol_result_t vt100_transport::received_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	vt100_transport* self = reinterpret_cast<vt100_transport*>(reference);
	string_type to_echo;
	string_array lines;
	for (size_t i = 0; i < buffer->size; i++)
	{
		self->char_received((char)buffer->buffer_ptr[i], i == buffer->size - 1, to_echo, [&lines](string_type line)
			{
				lines.emplace_back(line);
			});
	}
	rx_protocol_result_t result = RX_PROTOCOL_OK;
	if (!to_echo.empty())
	{
		runtime::io_types::rx_io_buffer send_buffer(to_echo.size(), reference);
		auto temp = send_buffer.write_chars(to_echo);
		if (!temp)
			result = RX_PROTOCOL_BUFFER_SIZE_ERROR;
		else
			result = rx_move_packet_down(reference, end_point, &send_buffer);
		if (result == RX_PROTOCOL_OK)
			send_buffer.detach(nullptr);
	}
	if (result == RX_PROTOCOL_OK && !lines.empty())
	{
		for (const auto& one : lines)
		{
			rx_const_packet_buffer up_buffer{ (const uint8_t*)one.c_str(), 0, one.size() };
			rx_init_const_packet_buffer(&up_buffer, one.c_str(), one.size());
			result = rx_move_packet_up(reference, NULL, &up_buffer);
			if (result != RX_PROTOCOL_OK)
				break;
		}
	}
	return result;
}

void vt100_transport::bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	sent_func_ = sent_func;
	received_func_ = received_func;
	rx_protocol_stack_entry* mine_entry = this;

	mine_entry->downward = nullptr;
	mine_entry->upward = nullptr;

	mine_entry->send_function = nullptr;
	mine_entry->sent_function = nullptr;
	mine_entry->received_function = &vt100_transport::received_function;

	mine_entry->connected_function = nullptr;

	mine_entry->close_function = nullptr;
	mine_entry->closed_function = nullptr;

	mine_entry->allocate_packet_function = nullptr;
	mine_entry->free_packet_function = nullptr;
}


// Class terminal::rx_vt100::dummy_transport 

dummy_transport::dummy_transport()
{
}


dummy_transport::~dummy_transport()
{
}



void dummy_transport::line_received (const string_type& line)
{
}

void dummy_transport::do_stuff ()
{
}


// Class terminal::rx_vt100::vt100_transport_port 

vt100_transport_port::vt100_transport_port()
{
}



rx_protocol_stack_entry* vt100_transport_port::get_stack_entry ()
{
	return &endpoint_;
}

void vt100_transport_port::bind_port ()
{
	endpoint_.bind([this](int64_t count)
		{
			update_sent_counters(count);
		},
		[this](int64_t count)
		{
			update_received_counters(count);
		});
}


} // namespace rx_vt100
} // namespace terminal

