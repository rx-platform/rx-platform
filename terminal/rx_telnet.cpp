

/****************************************************************************
*
*  terminal\rx_telnet.cpp
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


// rx_telnet
#include "terminal/rx_telnet.h"



namespace rx_internal {

namespace terminal {

namespace term_transport {
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


char g_server_telnet_idetification[] = { IAC, WILL, TELNET_ECHO,
IAC, WILL, SUPPRESS_GO_AHEAD };  /* IAC DO LINEMODE */
//IAC, SB, LINEMODE, 1, 0, IAC, SE /* IAC SB LINEMODE MODE 0 IAC SE */};
#define TELENET_IDENTIFICATION_SIZE sizeof(g_server_telnet_idetification)// has to be done here, don't ask why

} // anonymous 


// Class rx_internal::terminal::term_transport::telnet_transport 

telnet_transport::telnet_transport()
      : send_echo_(false),
        telnet_state_(telnet_parser_idle)
	, vt100_(false)
{
	rx_protocol_stack_entry* mine_entry = this;

	mine_entry->downward = nullptr;
	mine_entry->upward = nullptr;

	mine_entry->send_function = nullptr;
	mine_entry->sent_function = nullptr;
	mine_entry->received_function = &telnet_transport::received_function;

	mine_entry->connected_function = nullptr;

	mine_entry->close_function = nullptr;
	mine_entry->closed_function = nullptr;

	mine_entry->allocate_packet_function = nullptr;
	mine_entry->free_packet_function = nullptr;
}



rx_protocol_result_t telnet_transport::received_function (rx_protocol_stack_entry* reference, rx_const_packet_buffer* buffer)
{
	telnet_transport* self = reinterpret_cast<telnet_transport*>(reference);
	string_type to_echo;
	string_array lines;
	size_t i = 0;

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
			result = RX_PROTOCOL_BUFFER_SIZE_ERROR;
		else
			result = rx_move_packet_down(reference, &send_buffer);
		if (result == RX_PROTOCOL_OK)
			send_buffer.detach(nullptr);
	}
	if (result == RX_PROTOCOL_OK && !lines.empty())
	{
		for (const auto& one : lines)
		{
			rx_const_packet_buffer up_buffer{ (const uint8_t*)one.c_str(), 0, one.size() };
			rx_init_const_packet_buffer(&up_buffer, one.c_str(), one.size());
			result = rx_move_packet_up(reference, &up_buffer);
			if (result != RX_PROTOCOL_OK)
				break;
		}
	}
	return result;
}

rx_protocol_stack_entry* telnet_transport::bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	sent_func_ = sent_func;
	received_func_ = received_func;
	return this;
}

bool telnet_transport::handle_telnet (const char ch, string_type& to_echo)
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
			}
		}
		break;
	case telnet_parser_had_will:
		switch (ch)
		{
		case TELNET_ECHO:
			to_echo = { IAC, DONT, TELNET_ECHO };
			return true;
		}
		break;
	case telnet_parser_had_wont:
		switch (ch)
		{
		case TELNET_ECHO:
			to_echo = { IAC, DONT, TELNET_ECHO };
			return true;
		}
		break;
	case telnet_parser_had_do:
		switch (ch)
		{
		case TELNET_ECHO:
			send_echo_ = true;
			to_echo = { IAC, WILL, TELNET_ECHO };
			return true;
		}
		break;
	case telnet_parser_had_dont:
		switch (ch)
		{
		case TELNET_ECHO:
			send_echo_ = false;
			to_echo = { IAC, WONT, TELNET_ECHO };
			return true;
		}
		break;
	case telnet_parser_had_sb:
		break;
	case telnet_parser_had_sb2:
		break;
	};
	return false;
	size_t idx = 0;
	char* buff = NULL;
	if (buff[0] == IAC)
	{
		if (buff[1] >= WILL)
		{
			if (buff[2] == TELNET_ECHO)
			{
				if (buff[1] == DONT)
				{
					if (send_echo_)
					{
						send_echo_ = false;
						to_echo = { IAC, WONT, TELNET_ECHO };
					}
				}
				else if (buff[1] == DO)
				{
					if (!send_echo_)
					{
						send_echo_ = true;
						to_echo = { IAC, WILL, TELNET_ECHO };
					}
				}
			}
			idx += 3;
		}
		else if (buff[1] == SB)
		{
			idx += 2;
			int i = 2;
			while (buff[i] != SE)
			{
				i++;
				idx++;
			}
		}
		else
		{
			switch (buff[1])
			{
			case BRK:
			case IP:
			case EL:
				/*cancel_current_ = true;
				send_string_response("\r\nCanceling...\r\n", false);*/
				break;
			case AYT:
				//cancel_current_ = true;
				to_echo = "\r\nHello!\r\n";
				break;
			}
			idx += 2;
		}
	}
}

bool telnet_transport::char_received (const char ch, bool eof, string_type& to_echo, string_type& line)
{
	if (telnet_state_ != telnet_parser_idle || ch == IAC)
	{// handle telnet
		return handle_telnet(ch, to_echo);
	}
	else
	{
		return vt100_.char_received(ch, eof, to_echo, line);
	}
}


// Class rx_internal::terminal::term_transport::telnet_transport_port 

telnet_transport_port::telnet_transport_port()
{
}



} // namespace term_transport
} // namespace terminal
} // namespace rx_internal

