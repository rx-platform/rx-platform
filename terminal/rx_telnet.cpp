

/****************************************************************************
*
*  terminal\rx_telnet.cpp
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

// rx_telnet
#include "terminal/rx_telnet.h"

#include "rx_console.h"


namespace rx_internal {

namespace terminal {

namespace term_ports {
namespace
{



#define IAC             ((char)0xff)
#define DONT            ((char)0xfe)
#define DO            ((char)0xfd)
#define WONT              ((char)0xfc)
#define WILL            ((char)0xfb)

#define SE	((char)240)//	End of sub-negotiation parameters.
#define NOP	((char)241)//	No operation
#define DM	((char)242)//	Data mark.Indicates the position of
#define LINEMODE  ((char)34)
/*					a Sync event within the data stream.This
					should always be accompanied by a TCP
					urgent notification.*/
#define BRK	((char)243)//	Break.Indicates that the "break"
					//or "attention" key was hit.
#define IP	((char)244)//	Suspend, interrupt or abort the process
							//to which the NVT is connected.
#define AO	((char)245)//	Abort output.Allows the current process
							/*to run to completion but do not send
							its output to the user.*/
#define AYT	((char)246)//	Are you there ? Send back to the NVT some
							//visible evidence that the AYT was received.
#define EC	((char)247)//	Erase character.The receiver should delete
							/*the last preceding undeleted
							character from the data stream.*/
#define EL	((char)248)//	Erase line.Delete characters from the data
							//stream back to but not including the previous CRLF.
#define GA	((char)249)//	Go ahead.Used, under certain circumstances,
							/*to tell the other end that it can transmit.*/
#define SB	((char)250)//	Sub-negotiation of the indicated option follows.


#define TELNET_ECHO            ((char)0x01)
#define SUPPRESS_GO_AHEAD ((char)0x03)
#define TERMINAL_TYPE ((char)24)
#define NAWS ((char)31)
#define TERMINAL_SPEED ((char)32)
#define NEW_ENVIRON ((char)39)
#define SLE ((char)45)


/*
const char* get_IAC_name(char code)
{
	static char buffer[0x20];
	switch (code)
	{
	case TELNET_ECHO:
		return "ECHO";
	case SUPPRESS_GO_AHEAD:
		return "SUPPRESS_GO_AHEAD";
	case TERMINAL_TYPE:
		return "TERMINAL_TYPE";
	case TERMINAL_SPEED:
		return "TERMINAL_SPEED";
	case NEW_ENVIRON:
		return "NEW_ENVIRON";
	default:
		{
			snprintf(buffer, 0x20, "%d", (int)(uint8_t)code);
			return buffer;
		}
	}
}

const char* get_IAC_what(char code)
{
	static char buffer[0x20];
	switch (code)
	{
	case DONT:
		return "DONT";
	case DO:
		return "DO";
	case WONT:
		return "WONT";
	case WILL:
		return "WILL";
	default:
		{
			snprintf(buffer, 0x20, "%d", (int)(uint8_t)code);
			return buffer;
		}
	}
}

char g_password_prompt[] = "Enter Password:";
*/

char g_server_telnet_idetification[] = { IAC, WILL, TELNET_ECHO,
IAC, WILL, SUPPRESS_GO_AHEAD, IAC, DO, NAWS };  /* IAC DO LINEMODE */
//IAC, SB, LINEMODE, 1, 0, IAC, SE /* IAC SB LINEMODE MODE 0 IAC SE */};
#define TELENET_IDENTIFICATION_SIZE sizeof(g_server_telnet_idetification)// has to be done here, don't ask why

} // anonymous


// Class rx_internal::terminal::term_ports::telnet_transport 

telnet_transport::telnet_transport()
      : send_echo(false),
        telnet_state_(telnet_parser_idle)
{
}


telnet_transport::~telnet_transport()
{
}



bool telnet_transport::handle_telnet (const char ch, string_type& to_echo, string_type& line)
{
	switch (telnet_state_)
	{
	case telnet_parser_idle:
		if (ch == IAC)
		{
			telnet_state_ = telnet_parser_had_escape;
			return true;
		}
		break;
	case telnet_parser_had_escape:
		{
			switch (ch)
			{
			case WILL:
				telnet_state_ = telnet_parser_had_will;
				return true;
			case WONT:
				telnet_state_ = telnet_parser_had_wont;
				return true;
			case DO:
				telnet_state_ = telnet_parser_had_do;
				return true;
			case DONT:
				telnet_state_ = telnet_parser_had_dont;
				return true;
			case SB:
				telnet_state_ = telnet_parser_had_sb;
				sub_neg_data_.clear();
				return true;
			}
		}
		break;
	case telnet_parser_had_will:
		switch (ch)
		{
		case TELNET_ECHO:
			to_echo = { IAC, DONT, TELNET_ECHO };
			telnet_state_ = telnet_parser_idle;
			return true;
		case NAWS:
			to_echo = { IAC, DO, NAWS };
			telnet_state_ = telnet_parser_idle;
			return true;
		default:
			telnet_state_ = telnet_parser_idle;
			return false;
		}
		break;
	case telnet_parser_had_wont:
		switch (ch)
		{
		case TELNET_ECHO:
			to_echo = { IAC, DONT, TELNET_ECHO };
			telnet_state_ = telnet_parser_idle;
			return true;
		case NAWS:
			to_echo = { IAC, DONT, NAWS };
			telnet_state_ = telnet_parser_idle;
			return true;
		default:
			telnet_state_ = telnet_parser_idle;
			return false;
		}
		break;
	case telnet_parser_had_do:
		switch (ch)
		{
		case TELNET_ECHO:
			send_echo = true;
			to_echo = { IAC, WILL, TELNET_ECHO };
			telnet_state_ = telnet_parser_idle;
			return true;
		case NAWS:
			send_echo = true;
			to_echo = { IAC, WILL, NAWS };
			telnet_state_ = telnet_parser_idle;
			return true;
		default:
			telnet_state_ = telnet_parser_idle;
			return false;
		}
		break;
	case telnet_parser_had_dont:
		switch (ch)
		{
		case TELNET_ECHO:
			send_echo = false;
			to_echo = { IAC, WONT, TELNET_ECHO };
			telnet_state_ = telnet_parser_idle;
			return true;
		case NAWS:
			send_echo = false;
			to_echo = { IAC, WONT, NAWS };
			telnet_state_ = telnet_parser_idle;
			return true;
		default:
			telnet_state_ = telnet_parser_idle;
			return false;
		}
		break;
	case telnet_parser_had_sb:
		if (ch == IAC)
		{
			parse_negotiation(line);
			telnet_state_ = telnet_parser_had_sb2;
		}
		else
		{
			sub_neg_data_.push_back(std::byte(ch));
		}
		break;
	case telnet_parser_had_sb2:
		if (ch == SE)
		{
			telnet_state_ = telnet_parser_idle;
		}
		else
		{
			telnet_state_ = telnet_parser_had_sb;
		}
		break;
	};
	return false;
}

bool telnet_transport::char_received (const char ch, bool eof, string_type& to_echo, string_type& line)
{
	if (telnet_state_ != telnet_parser_idle || ch == IAC)
	{// handle telnet
		return handle_telnet(ch, to_echo, line);
	}
	else
	{
		line += ch;
		return false;
	}
}

void telnet_transport::parse_negotiation (string_type& line)
{
	if (sub_neg_data_.size() >= 5)
	{
		if (sub_neg_data_[0] == std::byte{ NAWS })
		{
			uint16_t width = ((std::to_integer<uint16_t>(sub_neg_data_[1])) << 8) | std::to_integer<uint16_t>(sub_neg_data_[2]);
			uint16_t height = ((std::to_integer<uint16_t>(sub_neg_data_[3])) << 8) | std::to_integer<uint16_t>(sub_neg_data_[4]);
			std::ostringstream ss;
			ss	<< "\033]T "
				<< width << " " << height
				<< "\033\\";
			line += ss.str();
		}
	}
}


// Class rx_internal::terminal::term_ports::telnet_transport_port 

telnet_transport_port::telnet_transport_port()
{
	construct_func = [this]()
	{
		auto rt = std::make_unique<telnet_transport_endpoint>(this);
		auto entry = rt->bind([this](int64_t count)
			{
			},
			[this](int64_t count)
			{
			});
		return construct_func_type::result_type{ entry, std::move(rt) };
	};
}



// Class rx_internal::terminal::term_ports::telnet_transport_endpoint 

telnet_transport_endpoint::telnet_transport_endpoint (runtime::items::port_runtime* port)
      : port_(port)
{
	CONSOLE_LOG_TRACE("telnet_transport", 900, "Telnet Transport endpoint created.");
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &telnet_transport_endpoint::received_function;
	stack_entry_.connected_function = &telnet_transport_endpoint::connected_function;
}


telnet_transport_endpoint::~telnet_transport_endpoint()
{
	CONSOLE_LOG_TRACE("telnet_transport", 900, "Telnet Transport endpoint destroyed.");
}



rx_protocol_stack_endpoint* telnet_transport_endpoint::bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	sent_func_ = sent_func;
	received_func_ = received_func;
	return &stack_entry_;
}

rx_protocol_result_t telnet_transport_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	telnet_transport_endpoint* self = reinterpret_cast<telnet_transport_endpoint*>(reference->user_data);
	string_type to_echo;
	string_type to_send;
	size_t i = 0;
	auto buffer = packet.buffer;

	if (buffer->size)
		to_send.reserve(buffer->size);

	for (; i < buffer->size; i++)
	{
		self->telnet_.char_received((char)buffer->buffer_ptr[i], i == buffer->size - 1, to_echo, to_send);
	}
	rx_protocol_result_t result = RX_PROTOCOL_OK;
	if (self->telnet_.send_echo && !to_echo.empty())
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
	if (result == RX_PROTOCOL_OK && !to_send.empty())
	{
		rx_const_packet_buffer up_buffer { };
		rx_init_const_packet_buffer(&up_buffer, to_send.c_str(), to_send.size());

		recv_protocol_packet packet = rx_create_recv_packet(0, &up_buffer, 0, 0);

		result = rx_move_packet_up(reference, packet);
	}
	return result;
}

rx_protocol_result_t telnet_transport_endpoint::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
	telnet_transport_endpoint* self = reinterpret_cast<telnet_transport_endpoint*>(reference->user_data);
	auto send_buffer = self->port_->alloc_io_buffer();
	if (!send_buffer)
	{
		return RX_PROTOCOL_OUT_OF_MEMORY;
	}
	else
	{

		send_buffer.value().write(g_server_telnet_idetification, TELENET_IDENTIFICATION_SIZE);
		rx_protocol_result_t result = rx_move_packet_down(reference, rx_create_send_packet(0, &send_buffer.value(), 0, 0));
		if (result == RX_PROTOCOL_OK)
		{
			result = rx_notify_connected(reference, session);
		}
		self->port_->release_io_buffer(send_buffer.move_value());
		return result;
	}
}


} // namespace term_ports
} // namespace terminal
} // namespace rx_internal

