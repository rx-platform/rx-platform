

/****************************************************************************
*
*  terminal\rx_telnet.cpp
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


// rx_telnet
#include "terminal/rx_telnet.h"

using namespace rx;
using namespace rx_platform;
using namespace rx_platform::ns;
using namespace std::string_literals;


#include "system/server/rx_server.h"



#define MY_PASSWORD "rxtx"


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
#define TELENET_RECIVE_TIMEOUT 600000




namespace terminal {

namespace console {

// Class terminal::console::server_telnet_socket 

server_telnet_socket::server_telnet_socket()
{
}


server_telnet_socket::~server_telnet_socket()
{
}



io::tcp_socket_std_buffer::smart_ptr server_telnet_socket::make_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher, rx_thread_handle_t destination)
{
	//TODOIO
	//telnet_client::smart_ptr ret = telnet_client::smart_ptr(handle, addr,local_addr, dispatcher);
	//ret->socket()->set_receive_timeout(TELENET_RECIVE_TIMEOUT);
	//return ret;

	return io::tcp_socket_std_buffer::smart_ptr::null_ptr;
}


// Class terminal::console::telnet_client 

telnet_client::telnet_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher)
      : security_context_(*addr,*local_addr),
        send_echo_(true),
        cancel_current_(false),
        verified_(false),
        exit_(false)
	//TODOIO
  //, io::tcp_socket_std_buffer(handle, addr,local_addr, dispatcher)
	, console_client(runtime::port_creation_data{ "telnet_stuff",55,56, rx_system_application() })
{
	vt100_parser_.set_password_mode(true);
}


telnet_client::~telnet_client()
{
}



bool telnet_client::on_startup (rx_thread_handle_t destination)
{
	buffer_ptr buffer = get_free_buffer();// dummy empty buffer
	buffer->push_data(g_server_telnet_idetification, TELENET_IDENTIFICATION_SIZE);
	buffer->push_data(g_password_prompt, sizeof(g_password_prompt));
	write(buffer);
	return true;
}

void telnet_client::on_shutdown (rx_thread_handle_t destination)
{
	security_context_->logout();
}

buffer_ptr telnet_client::get_free_buffer ()
{
	locks::auto_slim_lock dummy(&buffers_lock_);
	if (buffers_.empty())
		return buffer_ptr(pointers::_create_new);
	else
	{
		buffer_ptr ret(buffers_.top());
		buffers_.pop();
		return ret;
	}
}

bool telnet_client::new_recive (const char* buff, size_t& idx)
{

	if (!is_postponed() && (buff[0] == 13 || buff[0] == 3))
	{
		idx++;
		if (verified_ && send_echo_)
		{
			buffer_ptr buff = get_free_buffer();
			buff->push_data("\r\n", 2);
			write(buff);
		}
		std::string temp = receiving_string_;
		receiving_string_.clear();

		if (!verified_)
		{
			verified_ = true;

			if (temp == MY_PASSWORD)
			{
				security_context_->login();

				security::security_auto_context dummy(security_context_);

				buffer_ptr buff = get_free_buffer();

				string_type wellcome;
				get_wellcome(wellcome);
				buff->push_line(ANSI_CLS ANSI_CUR_HOME);
				buff->push_line(wellcome);
				string_type prompt;
				get_prompt(prompt);
				buff->push_string(prompt);

				write(buff);

			}
			else
			{
				buffer_ptr buff = get_free_buffer();

				string_type temp_str;
				get_wellcome(temp_str);
				buff->push_line(ANSI_CLS ANSI_CUR_HOME);
				buff->push_line(temp_str);
				get_security_error(temp_str, 0);
				buff->push_line(temp_str);
				string_type prompt;
				get_prompt(prompt);
				buff->push_string(prompt);

				write(buff);
			}

		}
		else
		{

			security::security_auto_context dummy(security_context_);

			buffer_ptr out_buffer = get_free_buffer();
			std::ostream out(out_buffer.unsafe_ptr());
			buffer_ptr err_buffer = get_free_buffer();
			std::ostream err(err_buffer.unsafe_ptr());

			if (buff[0] == 3)
			{// Ctrl+C
			 // cancel current stuff
				cancel_command(out_buffer, err_buffer, security_context_);

				write(err_buffer);
				write(out_buffer);

				return true;
			}

			if (temp.empty())
			{
				string_type prompt;
				get_prompt(prompt);
				out << prompt;

				write(err_buffer);
				write(out_buffer);

				return true;
			}



			//		do_command(temp, out_buffer, err_buffer,security_context_);


		}

		return true;
	}

	char echo_buff[4];
	int size_of_echo_buff = 0;

	if (buff[0] == IAC)
	{
		//ATLTRACE(L);
		if (buff[1] >= WILL)
		{
			if (buff[2] == TELNET_ECHO)
			{
				if (buff[1] == DONT)
				{
					if (send_echo_)
					{
						send_echo_ = false;
						char resp[] = { IAC, WONT, TELNET_ECHO };
						buffer_ptr buff = get_free_buffer();
						buff->push_data(resp, 3);

						write(buff);
					}
				}
				else if (buff[1] == DO)
				{
					if (!send_echo_)
					{
						send_echo_ = true;
						char resp[] = { IAC, WILL, TELNET_ECHO };
						buffer_ptr buff = get_free_buffer();
						buff->push_data(resp, 3);
						write(buff);
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
				cancel_current_ = true;
				send_string_response("\r\nCanceling...\r\n", false);
				break;
			case AYT:
				cancel_current_ = true;
				send_string_response("\r\nHello!\r\n");
				break;
			}
			idx += 2;
		}
	}
	else
	{
		//printf("received %d\r\n",(int)buff[0]);
		if (buff[0] == 8 || buff[0] == 0x7f) // backspace ( delete on linux )
		{
			// backspace pressed
			if (receiving_string_.size() > 0)
			{
				// cut off one char
				receiving_string_ = receiving_string_.substr(0, receiving_string_.size() - 1);
				echo_buff[0] = 8;
				echo_buff[1] = 32;
				echo_buff[2] = 8;
				size_of_echo_buff = 3;
			}
		}
		// enter pressed
		else if (buff[0] == 13)
		{
			echo_buff[0] = 13;
			echo_buff[1] = 10;
			size_of_echo_buff = 2;
		}
		// Ctrl+C handled up so do it
		else if (buff[0] == 0x3)
		{
			RX_ASSERT(is_postponed());
			// has to be because of first if in function
			if (is_postponed())
			{// cancel current

				security::security_auto_context dummy(security_context_);

				buffer_ptr out_buffer = get_free_buffer();
				buffer_ptr err_buffer = get_free_buffer();


				cancel_command(out_buffer, err_buffer, security_context_);

				write(err_buffer);
				write(out_buffer);
			}
		}
		else if (buff[0] >= 0x20)// no special characters
		{

			// ordinary char pressed
			if (!verified_)
			{
				if (buff[0] >= 0x20)// no special characters
				{
					// echo a '*'
					strcpy(echo_buff, "*");
					size_of_echo_buff = 1;
					// add to result buffer
					receiving_string_ += buff[0];
				}
			}
			else
			{
				// echo the pressed char
				memcpy(echo_buff, buff, 1);
				size_of_echo_buff = 1;
				// add to result buffer

				receiving_string_ += buff[0];
			}
		}
		idx++;

		// echo
		if (send_echo_ && size_of_echo_buff)
		{
			buffer_ptr buff = get_free_buffer();
			buff->push_data(echo_buff, size_of_echo_buff);
			write(buff);
		}
	}

	return true;
}

void telnet_client::send_string_response (const string_type& line, bool with_prompt)
{
	buffer_ptr buff = get_free_buffer();
	buff->push_line(line);
	if (with_prompt)
	{
		string_type prompt;
		get_prompt(prompt);
		buff->push_string(prompt);
	}
	write(buff);
}

const string_type& telnet_client::get_console_name ()
{
	static string_type temp("Telnet");
	return temp;
}

void telnet_client::exit_console ()
{
	exit_ = true;
}

bool telnet_client::readed (const void* data, size_t count, rx_thread_handle_t destination)
{

	if (count == 0)
	{
		RX_ASSERT(0);
		return false;
	}

	const char* buff = static_cast<const char*>(data);
	size_t idx = 0;
	char echo_buff[4];
	int size_of_echo_buff = 0;

	if (buff[0] != IAC)
	{
		// our buffer
		string_type to_send;
		std::vector<string_type> lines;

		if (count > 1)
		{
			for (size_t i = idx; i < count - 1; i++)
			{
				if (buff[i] == 0x3)
				{
					cancel_current();
				}
				else
				{
					vt100_parser_.char_received(buff[i], false, to_send
						, [&](const string_type& line)
					{
						lines.emplace_back(line);
					});
				}
			}
		}
		if (buff[count - 1] == 0x3)
		{
			cancel_current();
		}
		else
		{
			vt100_parser_.char_received(buff[count - 1], true, to_send
				, [&](const string_type& line)
			{
				lines.emplace_back(line);
			});
		}

		if (!to_send.empty())
		{
			auto buff = get_free_buffer();
			buff->push_string(to_send);
			write(buff);
		}
		if (!lines.empty())
		{
			lines_received(std::move(lines));
		}
	}
	else //if (buff[0] == IAC)!!!
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
						char resp[] = { IAC, WONT, TELNET_ECHO };
						buffer_ptr buff = get_free_buffer();
						buff->push_data(resp, 3);

						write(buff);
					}
				}
				else if (buff[1] == DO)
				{
					if (!send_echo_)
					{
						send_echo_ = true;
						char resp[] = { IAC, WILL, TELNET_ECHO };
						buffer_ptr buff = get_free_buffer();
						buff->push_data(resp, 3);
						write(buff);
					}
				}
				idx += 3;
			}
		}
		else if (buff[1] == DO)
		{
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
				cancel_current_ = true;
				send_string_response("\r\nCanceling...\r\n", false);
				break;
			case AYT:
				cancel_current_ = true;
				send_string_response("\r\nHello!\r\n");
				break;
			}
			idx += 2;
		}

		// echo
		if (size_of_echo_buff)
		{
			buffer_ptr buff = get_free_buffer();
			buff->push_data(echo_buff, size_of_echo_buff);
			write(buff);
		}

		return true;

	}
	return true;

}

void telnet_client::release_buffer (buffer_ptr what)
{
	locks::auto_slim_lock dummy(&buffers_lock_);
	what->reinit();
	buffers_.push(what);
}

bool telnet_client::get_next_line (string_type& line)
{
	return true;
}

void telnet_client::process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer)
{
	std::ostream out(out_buffer.unsafe_ptr());
	std::ostream err(err_buffer.unsafe_ptr());

	if (result)
	{// success, out buffer is active

		if (is_postponed())
		{// async stuff, nothing to do
			locks::auto_slim_lock dummy(&buffers_lock_);
			buffers_.push(err_buffer);
			buffers_.push(out_buffer);
		}
		else
		{
			if (exit_)
			{
				write(out_buffer);
				write(buffer_ptr::null_ptr);// exit the loop
			}
			else
			{
				string_type prompt;
				get_prompt(prompt);
				out << prompt;

				write(err_buffer);
				write(out_buffer);
			}
		}
	}
	else
	{// error
		if (exit_ || err_buffer->empty())
		{// exit command, close socket
			write(buffer_ptr::null_ptr);
		}
		else
		{
			err << "\r\n" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "\r\n";
			string_type prompt;
			get_prompt(prompt);
			out << prompt;

			write(err_buffer);
			write(out_buffer);
		}
	}
}

void telnet_client::lines_received (string_array&& lines)
{
	if (!verified_)
	{
		string_array captured_lines(std::move(lines));
		rx_post_function<smart_ptr>(
			[captured_lines](smart_ptr sended_this)
		{
			sended_this->verified_ = true;
			sended_this->vt100_parser_.set_password_mode(false);

			for (auto&& captured_line : captured_lines)
			{
				if (captured_line == MY_PASSWORD)
				{
					sended_this->security_context_->login();

					security::security_auto_context dummy(sended_this->security_context_);

					buffer_ptr buff = sended_this->get_free_buffer();

					string_type wellcome;
					sended_this->get_wellcome(wellcome);
					buff->push_line(ANSI_CLS ANSI_CUR_HOME);
					buff->push_line(wellcome);
					string_type prompt;
					sended_this->get_prompt(prompt);
					buff->push_string(prompt);

					sended_this->write(buff);

				}
				else
				{
					buffer_ptr buff = sended_this->get_free_buffer();

					string_type temp_str;
					sended_this->get_wellcome(temp_str);
					buff->push_line(ANSI_CLS ANSI_CUR_HOME);
					buff->push_line(temp_str);
					sended_this->get_security_error(temp_str, 0);
					buff->push_line(temp_str);
					string_type prompt;
					sended_this->get_prompt(prompt);
					buff->push_string(prompt);

					sended_this->write(buff);
				}
			}
		}
			, smart_this()
			, get_executer()
			);

	}
	else
	{
		security::security_auto_context dummy(security_context_);

		buffer_ptr out_buffer = get_free_buffer();
		buffer_ptr err_buffer = get_free_buffer();

		do_commands(std::move(lines), out_buffer, err_buffer, security_context_);
	}
}

void telnet_client::cancel_current ()
{
	
	security::security_auto_context dummy(security_context_);

	buffer_ptr out_buffer = get_free_buffer();
	buffer_ptr err_buffer = get_free_buffer();

	cancel_command(out_buffer, err_buffer, security_context_);
}


// Class terminal::console::telnet_security_context 

telnet_security_context::telnet_security_context()
{
}

telnet_security_context::telnet_security_context (const sockaddr_in& addr, const sockaddr_in& local_addr)
{
	location_ = inet_ntoa(addr.sin_addr);
	port_ = "telnet@TCP/IP[";
	port_ += inet_ntoa(local_addr.sin_addr);
	port_ += "]";
	user_name_ += "console";
	full_name_ = "console@" + location_;

}


telnet_security_context::~telnet_security_context()
{
}



bool telnet_security_context::has_console () const
{
  return true;

}

bool telnet_security_context::is_system () const
{
  return true;

}


} // namespace console
} // namespace terminal

