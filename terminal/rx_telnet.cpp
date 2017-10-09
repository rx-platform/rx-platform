

/****************************************************************************
*
*  terminal\rx_telnet.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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


#include "stdafx.h"


// rx_telnet
#include "terminal/rx_telnet.h"

#include "system/server/rx_server.h"
#include "system/python/py_support.h"


namespace terminal {

namespace console {


#define RX_CONSOLE_CHECKED "[X]"
#define RX_CONSOLE_UNCHECKED "[ ]"

#define MY_PASSWORD "rxtx"


#define IAC             ((char)0xff)
#define DONT            ((char)0xfe)
#define DO            ((char)0xfd)
#define WONT              ((char)0xfc)
#define WILL            ((char)0xfb)

#define SE	((char)240)//	End of subnegotiation parameters.
#define NOP	((char)241)//	No operation
#define DM	((char)242)//	Data mark.Indicates the position of
#define LINEMODE  ((char)34)
		/*					a Synch event within the data stream.This
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
#define SB	((char)250)//	Subnegotiation of the indicated option follows.


#define ECHO            ((char)0x01)
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
	case ECHO:
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
			snprintf(buffer, 0x20,"%d",(int)(byte)code);
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
			snprintf(buffer, 0x20, "%d", (int)(byte)code);
			return buffer;
		}
	}
}

char g_password_prompt[] = "Password:";


char g_server_telnet_idetification[] = { IAC, DONT, ECHO, IAC, WILL, ECHO,
IAC, DO, LINEMODE, IAC, WILL, SUPPRESS_GO_AHEAD };  /* IAC DO LINEMODE */
//IAC, SB, LINEMODE, 1, 0, IAC, SE /* IAC SB LINEMODE MODE 0 IAC SE */};
#define TELENET_IDENTIFICATION_SIZE sizeof(g_server_telnet_idetification)// has to be done here, don't ask why
#define TELENET_RECIVE_TIMEOUT 600000


#define BASIC_TERMINAL_DATA "\
Terminal Server Ver: 0.7.0"

struct table_cell_struct
{
	table_cell_struct(const string_type& vvalue)
		: value(vvalue)
	{
	}
	table_cell_struct(const string_type& vvalue, const string_type& vprefix, const string_type& vpostfix)
		: prefix(vprefix) , value(vvalue), postfix(vpostfix)
	{
	}
	string_type prefix;
	string_type value;
	string_type postfix;
};

typedef std::vector<table_cell_struct> row_type;
typedef std::vector<row_type> table_type;

void dump_large_row(row_type row, std::ostream& out,size_t console_width)
{
	if (row.empty())
		return;

	const size_t col_diff = 2;

	size_t count = row.size();

	std::vector<size_t> widths(count);
	for (size_t i = 0; i < count; i++)
	{
		widths[i] = row[i].value.size();
	}
	// first try to get how manny columns do we need
	size_t columns = count+1;
	std::vector<size_t> column_widths;
	size_t total_width = 1000000000ull;// i guess will be enought

	while (total_width>console_width)
	{
		columns--;
		column_widths.assign(columns, 0);
		size_t idx = 0;
		while (idx < count)
		{
			for (size_t i = 0; i < columns; i++)
			{
				size_t one_idx = idx + i;

				if (one_idx >= count)
					break;
				if (column_widths[i] < widths[one_idx])
				{
					column_widths[i] = widths[one_idx];
				}
			}
			idx += columns;
		}
		total_width = 0;
		for (size_t i = 0; i < columns; i++)
			total_width += (column_widths[i] + col_diff);
	}

	bool first = true;
	size_t idx = 0;
	while (idx < count)
	{
		if (first)
			first = false;
		else
			out << "\r\n";

		for (size_t i = 0; i < columns; i++)
		{
			size_t one_idx = idx + i;
			if (one_idx >= count)
				break;

			string_type rest(column_widths[i] + col_diff - row[one_idx].value.size(), ' ');

			if (!row[one_idx].prefix.empty())
				out << row[one_idx].prefix;
			out << row[one_idx].value;
			if (!row[one_idx].postfix.empty())
				out << row[one_idx].postfix;
			out << rest;
		}
		idx += columns;
	}

	out << "\r\n";
}

void dump_table(const table_type& table, std::ostream& out,bool column_names)
{
	if (table.empty())
		return;

	const size_t col_diff = 2;

	size_t columns_number = 0;
	for (const auto& row : table)
	{
		if (columns_number == 0)
			columns_number = row.size();
		else
		{
			if (!row.empty())
			{// we allow emty rows
				if (columns_number != row.size())
				{
					out << "Error in table format\r\n";
					RX_ASSERT(false);
					return;
				}
			}
		}
	}

	// o.k. we checked now so let's callculate columns width
	std::vector<size_t> widths(columns_number);
	for (const auto& row : table)
	{
		if (!row.empty())
		{
			for (size_t i = 0; i < columns_number; i++)
			{
				if (row[i].value.size() > widths[i])
					widths[i] = row[i].value.size();
			}
		}
	}
	// now we have all widths
	bool first = true;
	for (const auto& row : table)
	{
		if (first)
			first = false;
		else
			out << "\r\n";
		if (!row.empty())
		{
			for (size_t i = 0; i < columns_number; i++)
			{
				string_type rest(widths[i] + col_diff - row[i].value.size(), ' ');
				if (!row[i].prefix.empty())
					out << row[i].prefix;
				out << row[i].value;
				if (!row[i].postfix.empty())
					out << row[i].postfix;
				out << rest;
			}
		}
		if (column_names)
		{
			out << "\r\n";
			size_t total_width = 0;
			for (size_t i = 0; i < columns_number; i++)
				total_width += (widths[i] + col_diff);
			string_type rest(total_width, '=');
			out << rest;
			column_names = false;
		}
	}

	out << "\r\n";
}


// Class terminal::console::server_telnet_socket 

server_telnet_socket::server_telnet_socket()
{
}


server_telnet_socket::~server_telnet_socket()
{
}



io::tcp_socket_str_buffer::smart_ptr server_telnet_socket::make_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher, rx_thread_handle_t destination)
{
	telnet_client::smart_ptr ret = telnet_client::smart_ptr(handle, addr,local_addr, dispatcher);
	ret->set_receive_timeout(TELENET_RECIVE_TIMEOUT);
	//ret->set_receive_timeout(2000);
	return ret;
}


// Class terminal::console::telnet_client 

telnet_client::telnet_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher)
      : m_security_context(*addr,*local_addr),
        m_send_echo(false),
        m_cancel_current(false),
        m_verified(false),
        m_exit(false)
  , io::tcp_socket_str_buffer(handle, addr,local_addr, dispatcher)
{
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
	m_security_context->logout();
}

telnet_client::buffer_ptr telnet_client::get_free_buffer ()
{
	locks::auto_slim_lock dummy(&m_buffers_lock);
	if (m_buffers.empty())
		return buffer_ptr(pointers::_create_new);
	else
	{
		buffer_ptr ret(m_buffers.top());
		m_buffers.pop();
		return ret;
	}
}

bool telnet_client::new_recive (const char* buff, size_t& idx)
{

	if (!is_postponed() && (buff[0] == 13 || buff[0]==3))
	{
		idx++;
		if (m_verified && m_send_echo)
		{
			buffer_ptr buff = get_free_buffer();
			buff->push_data("\r\n", 2);
			send(buff);
		}
		std::string temp = m_receiving_string;
		m_receiving_string.clear();

		if (!m_verified)
		{
			m_verified = true;

			if (temp == MY_PASSWORD)
			{
				m_security_context->login();

				security::security_auto_context dummy(m_security_context);

				buffer_ptr buff = get_free_buffer();

				string_type wellcome;
				get_wellcome(wellcome);
				buff->push_line(ANSI_CLS ANSI_CUR_HOME);
				buff->push_line(wellcome);
				string_type prompt;
				get_prompt(prompt);
				buff->push_string(prompt);

				send(buff);

			}
			else
			{
				buffer_ptr buff = get_free_buffer();

				string_type wellcome;
				get_wellcome(wellcome);
				buff->push_line(ANSI_CLS ANSI_CUR_HOME);
				buff->push_line(wellcome);
				string_type prompt;
				get_prompt(prompt);
				buff->push_string(prompt);

				send(buff);
			}

		}
		else
		{

			security::security_auto_context dummy(m_security_context);

			buffer_ptr out_buffer = get_free_buffer();
			std::ostream out(out_buffer.unsafe_ptr());
			buffer_ptr err_buffer = get_free_buffer();
			std::ostream err(err_buffer.unsafe_ptr());

			if (buff[0] == 3)
			{// Ctrl+C
			 // canacel current stuff

				string_type prompt;
				get_prompt(prompt);

				out << "^C\r\n";
				out << prompt;

				send(out_buffer);
				send(err_buffer);

				return true;
			}

			if (temp.empty())
			{
				string_type prompt;
				get_prompt(prompt);
				out << prompt;


				send(out_buffer);

				send(err_buffer);

				return true;
			}



			bool ret = do_command(temp, out_buffer, err_buffer,*m_security_context);

			if (ret)
			{// success, out buffer is active

				if (is_postponed())
				{// async stuff, nothing to do
					locks::auto_slim_lock dummy(&m_buffers_lock);
					m_buffers.push(err_buffer);
					m_buffers.push(out_buffer);
					return true;
				}
				else
				{
					if (m_exit)
					{
						send(out_buffer);
						send(buffer_ptr::null_ptr);// exit the loop

					}
					else
					{
						string_type prompt;
						get_prompt(prompt);
						out << prompt;


						send(out_buffer);
						send(err_buffer);
					}
				}
			}
			else
			{// error
				if (err_buffer->empty())
				{// exit command, close socket
					return false;
				}
				err << "\r\n" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "\r\n";
				string_type prompt;
				get_prompt(prompt);
				err << prompt;

				send(out_buffer);
				send(err_buffer);
			}
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
			if (buff[2] == ECHO)
			{
				if (buff[1] == DONT)
				{
					if (m_send_echo)
					{
						m_send_echo = false;
						char resp[] = { IAC, WONT, ECHO };
						buffer_ptr buff = get_free_buffer();
						buff->push_data(resp, 3);

						send(buff);
					}
				}
				else if (buff[1] == DO)
				{
					if (!m_send_echo)
					{
						m_send_echo = true;
						char resp[] = { IAC, WILL, ECHO };
						buffer_ptr buff = get_free_buffer();
						buff->push_data(resp, 3);
						send(buff);
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
				m_cancel_current = true;
				send_string_response("\r\nCanceling...\r\n", false);
				break;
			case AYT:
				m_cancel_current = true;
				send_string_response("\r\nHello!\r\n");
				break;
			}
			idx += 2;
		}
	}
	else
	{
        //printf("received %d\r\n",(int)buff[0]);

		if (buff[0] == 8 || buff[0]==0x7f) // backspace ( delete on linux )
		{
			// backspace pressed
			if (m_receiving_string.size() > 0)
			{
				// cut off one char
				m_receiving_string = m_receiving_string.substr(0, m_receiving_string.size() - 1);
				echo_buff[0] = 8;
				echo_buff[1] = 32;
				echo_buff[2] = 8;
				size_of_echo_buff = 3;
			}
			/*else
			{
			return recive();
			}*/
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

				buffer_ptr out_buffer = get_free_buffer();
				std::ostream out(out_buffer.unsafe_ptr());
				buffer_ptr err_buffer = get_free_buffer();
				std::ostream err(err_buffer.unsafe_ptr());

				string_type prompt;
				get_prompt(prompt);

				out << "^C\r\n";
				out << prompt;

				send(out_buffer);
				send(err_buffer);
			}
		}
		else if (buff[0] >= 0x20)// no special characters
		{

			// ordinary char pressed
			if (!m_verified)
			{
				if (buff[0] >= 0x20)// no special characters
				{
					// echo a '*'
					strcpy(echo_buff, "*");
					size_of_echo_buff = 1;
					// add to result buffer
					m_receiving_string += buff[0];
				}
			}
			else
			{
				// echo the pressed char
				memcpy(echo_buff, buff, 1);
				size_of_echo_buff = 1;
				// add to result buffer

				m_receiving_string += buff[0];
			}
		}
		idx++;

		// echo
		if (m_send_echo && size_of_echo_buff)
		{
			buffer_ptr buff = get_free_buffer();
			buff->push_data(echo_buff, size_of_echo_buff);
			send(buff);
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
	send(buff);
}

const string_type& telnet_client::get_console_name ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret,BASIC_TERMINAL_DATA);
	}
	return ret;
}

void telnet_client::exit_console ()
{
	m_exit = true;
}

bool telnet_client::readed (const void* data, size_t count, rx_thread_handle_t destination)
{
	if (count == 0)
	{
        RX_ASSERT(0);
		return false;
	}

	const char* string = (const char*)data;
	size_t idx = 0;
	while (idx < count)
	{
		if (!new_recive(&string[idx], idx))
			return false;
	}
	return true;
}

void telnet_client::release_buffer (buffer_ptr what)
{
	locks::auto_slim_lock dummy(&m_buffers_lock);
	what->reinit();
	m_buffers.push(what);
}


// Class terminal::console::telnet_security_context 

telnet_security_context::telnet_security_context()
{
}

telnet_security_context::telnet_security_context (const sockaddr_in& addr, const sockaddr_in& local_addr)
{
	m_location = inet_ntoa(addr.sin_addr);
	m_port = "telnet@TCP/IP[";
	m_port += inet_ntoa(local_addr.sin_addr);
	m_port += "]";
	m_user_name += "console";
	m_full_name = "console@" + m_location;

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


namespace console_commands {
namespace
{

bool dump_info(std::ostream& out, rx_server_item::smart_ptr& item)
{
	string_type quality_stirng;
	values::rx_value val;
	item->get_value(val);
	string_type value_string;
	val.get_string(value_string);
	ns::fill_quality_string(val, quality_stirng);
	string_type attrs;
	ns::fill_namepsace_string(item->get_attributes(), attrs);
	string_type cls_name;
	bool has_code = false;
	string_type console;
	item->get_class_info(cls_name, console, has_code);


	string_type pera = g_complie_time;
	string_type temp = "aaa";
	out << "\r\nINFO" << "\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Name       : " << item->get_item_name() << "\r\n";
	out << "Full Path  : " << item->get_path() << "\r\n";
	if(!console.empty())
		out << "Console    : " << console << "\r\n";
	out << "Type       : " << item->get_type_name() << "\r\n";
	out << "Attributes : " << attrs << "\r\n\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Value      : " << value_string << "\r\n";
	out << "Quality	   : " << quality_stirng << "\r\n";
	out << "Time stamp : " << val.get_time().get_string() << "\r\n\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Class      : " << cls_name << "\r\n";
	out << "Has Code   : " << (has_code ? "true" : "false") << "\r\n";
	return true;
}

bool dump_items_on_console(row_type& row, bool list_attributes, bool list_qualities, bool list_timestamps, bool list_created, bool list_type, ns::rx_server_item::smart_ptr one)
{
	if ((one->get_attributes()&namespace_item_browsable) != 0)
		row.emplace_back(one->get_item_name(), ANSI_COLOR_BOLD ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
	else
		row.emplace_back(one->get_item_name());
	if (list_type)
	{
		row.emplace_back(one->get_type_name());
	}
	if (list_attributes)
	{
		string_type attrs;
		ns::fill_namepsace_string(one->get_attributes(), attrs);
		row.emplace_back(attrs);
	}
	if (list_qualities || list_timestamps)
	{
		values::rx_value val;
		one->get_value(val);

		if (list_qualities)
		{
			string_type quality_stirng;
			ns::fill_quality_string(val, quality_stirng);
			row.emplace_back(quality_stirng);
		}
		if (list_timestamps)
			row.emplace_back(val.get_time().get_string());
	}

	return true;
}


bool dump_dirs_on_console(row_type& row, bool list_attributes, bool list_qualities, bool list_timestamps, bool list_created, bool list_type, ns::rx_server_directory::smart_ptr one, const string_type& name)
{

	row.emplace_back(name,ANSI_COLOR_CYAN ANSI_COLOR_BOLD,ANSI_COLOR_RESET);
	if (list_type)
	{
		row.emplace_back(one->get_type_name());
	}
	if (list_attributes)
	{
		string_type attrs;
		ns::fill_namepsace_string(one->get_attributes(), attrs);
		row.emplace_back(attrs);
	}
	if (list_qualities || list_timestamps)
	{
		values::rx_value val;
		one->get_value(val);

		if (list_qualities)
		{
			string_type quality_stirng;
			ns::fill_quality_string(val, quality_stirng);
			row.emplace_back(quality_stirng);
		}
		if (list_timestamps)
			row.emplace_back(val.get_time().get_string());
	}

	return true;
}

void fill_context_attributes(security::security_context_ptr ctx,string_type& val)
{
	val.assign(3, '-');
	if (ctx->is_authenticated())
		val[0] = 'a';
	if (ctx->is_system())
		val[1] = 's';
	if (ctx->is_hosted())
		val[2] = 'h';
}

}

// Class terminal::console::console_commands::directory_command 

directory_command::directory_command (const string_type& console_name)
  : server_command(console_name)
{
}


directory_command::~directory_command()
{
}



bool directory_command::list_directory (std::ostream& out, std::ostream& err, const string_type& filter, bool list_attributes, bool list_qualities, bool list_timestamps, bool list_created, bool list_type, server_directory_ptr& directory)
{
	server_directories_type dirs;
	server_items_type items;
	directory->get_content(dirs, items, filter);

	size_t count = dirs.size() + items.size();

	table_type table(count + 1);


	table[0].emplace_back("Name");
	if (list_type)
		table[0].emplace_back("Type");
	if (list_attributes)
		table[0].emplace_back("Attributes");
	if (list_qualities)
		table[0].emplace_back("Quality");
	if (list_timestamps)
		table[0].emplace_back("Time Stamp");

	size_t idx = 1;
	for (auto& one : dirs)
	{
		dump_dirs_on_console(table[idx], list_attributes, list_qualities, list_timestamps, list_created, list_type, one, one->get_name());
		idx++;
	}
	for (auto& one : items)
	{
		dump_items_on_console(table[idx], list_attributes, list_qualities, list_timestamps, list_created, list_type, one);
		idx++;
	}

	dump_table(table, out, true);

	return true;
}

bool directory_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type filter;
	bool list_attributes = false;
	bool list_qualities = false;
	bool list_timestamps = false;
	bool list_created = false;
	bool list_type = true;
	while (!in.eof())
	{
		string_type opt;
		in >> opt;
		if (opt == "-q")
			list_qualities = true;
		if (opt == "-t")
			list_timestamps = true;
		if (opt == "-a")
			list_attributes = true;
		if (opt == "-c")
			list_created = true;
		if (opt == "-f")
		{
			list_attributes = true;
			list_qualities = true;
			list_timestamps = true;
			list_created = true;
		}
	}

    server_directory_ptr temp(ctx->get_current_directory());
	return list_directory(out, err, filter, list_attributes, list_qualities, list_timestamps, list_created, list_type, temp);
}


// Class terminal::console::console_commands::dir_command 

dir_command::dir_command()
  : directory_command("dir")
{
}


dir_command::~dir_command()
{
}



// Class terminal::console::console_commands::ls_command 

ls_command::ls_command()
  : directory_command("ls")
{
}


ls_command::~ls_command()
{
}



bool ls_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	if (in.eof())
	{// dump here
		string_type filter;
		server_directory_ptr temp(ctx->get_current_directory());
		server_directories_type dirs;
		server_items_type items;
		temp->get_content(dirs, items, filter);

		size_t count = dirs.size() + items.size();

		row_type row;
		row.reserve(count);

		for (auto& one : dirs)
		{
			row.emplace_back(one->get_name(), ANSI_COLOR_BOLD ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
		}
		for (auto& one : items)
		{
			if((one->get_attributes()&namespace_item_browsable)!=0)
				row.emplace_back(one->get_item_name(), ANSI_COLOR_BOLD ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
			else
				row.emplace_back(one->get_item_name(), ANSI_COLOR_BOLD, ANSI_COLOR_RESET);

		}
		dump_large_row(row, out, 60);
		return true;
	}
	else
	{
		return directory_command::do_console_command(in, out, err, ctx);
	}
}


// Class terminal::console::console_commands::cd_command 

cd_command::cd_command()
  : server_command("cd")
{
}


cd_command::~cd_command()
{
}



bool cd_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
    string_type path;
	in >> path;

    server_directory_ptr where=ctx->get_current_directory()->get_sub_directory(path);
	if (!where)
	{
		err << "Directory not found!\r\n";
		return false;
	}
	ctx->set_current_directory(where);
	return true;
}


// Class terminal::console::console_commands::info_command 

info_command::info_command()
  : directory_aware_command("info")
{
}


info_command::~info_command()
{
}



bool info_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type whose;
	if (!in.eof())
		in >> whose;
	if (!whose.empty())
	{
		server_directory_ptr dir = ctx->get_current_directory()->get_sub_directory(whose);
		if (dir)
		{
			dump_dir_info(out,dir);
		}
		else
		{
			server_item_ptr item = ctx->get_current_directory()->get_sub_item(whose);
			if (item)
			{
				dump_info(out, item);
			}
			else
			{
				err << "ERROR: unknown object name";
				return false;
			}
		}
	}
	else
	{
		dump_dir_info(out, ctx->get_current_directory());
	}
	return true;
}

bool info_command::dump_dir_info (std::ostream& out, server_directory_ptr directory)
{
	string_type quality_stirng;
	values::rx_value val;
	directory->get_value(val);
	ns::fill_quality_string(val, quality_stirng);
	string_type attrs;
	ns::fill_namepsace_string(directory->get_attributes(), attrs);
	string_type cls_name;
	bool has_code = false;
	string_type console;
	directory->get_class_info(cls_name, console, has_code);


	string_type pera = g_complie_time;
	string_type temp = "aaa";
	out << "\r\nINFO" << "\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Name       : " << directory->get_name(true) << "\r\n";
	out << "Full Path  : " << directory->get_path() << "\r\n";
	out << "Type       : " << directory->get_type_name() << "\r\n";
	out << "Attributes : " << attrs << "\r\n\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Value      : " << "<<null>>" << "\r\n";
	out << "Quality	   : " << quality_stirng << "\r\n";
	out << "Time stamp : " << val.get_time().get_string() << "\r\n\r\n";
	out << "Created    : " << directory->get_created().get_string() << "\r\n";
	out << "--------------------------------------------------------------------------------" << "\r\n";
	out << "Class      : " << cls_name << "\r\n";
	out << "Has Code   : " << (has_code ? "true" : "false") << "\r\n";
	return true;
}


// Class terminal::console::console_commands::code_command 

code_command::code_command()
  : directory_aware_command("code")
{
}


code_command::~code_command()
{
}



bool code_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type whose;
	if (!in.eof())
		in >> whose;
	if (!whose.empty())
	{
		server_directory_ptr dir = ctx->get_current_directory()->get_sub_directory(whose);
		if (dir)
		{
			dir->fill_code_info(out);
		}
		else
		{
			server_item_ptr item = ctx->get_current_directory()->get_sub_item(whose);
			if (item)
			{
				item->fill_code_info(out,whose);
			}
			else
			{
				err << "ERROR: unknown object name";
				return false;
			}
		}
	}
	else
	{
		ctx->get_current_directory()->fill_code_info(out);
	}
	return true;
}


// Class terminal::console::console_commands::rx_name_command 

rx_name_command::rx_name_command()
  : server_command("pname")
{
}


rx_name_command::~rx_name_command()
{
}



bool rx_name_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	out << "System Information\r\n";
	out << "========================================\r\n";
	out << "Engine Name: " << rx_server::instance().get_rx_name() << "\r\n";
	out << "Engine Version: " << rx_server::instance().get_rx_version() << "\r\n";
	out << "Library Version: " << rx_server::instance().get_lib_version() << "\r\n";
	out << "OS/HW Interface: " << rx_server::instance().get_hal_version() << "\r\n";
	out << "Compiler: " << rx_server::instance().get_comp_version() << "\r\n";

	out << "Host: " << rx_server::instance().get_host_info() << "\r\nOS: " << rx_server::instance().get_os_info() << "\r\n";

	
	/////////////////////////////////////////////////////////////////////////
	// Processor
	char buff[0x100];
	rx_collect_processor_info(buff, sizeof(buff) / sizeof(buff[0]));
	out << "CPU: " << buff 
		<< ( rx_big_endian ? " [BE]:" : " [LE]" )
		<< "\r\n";
	/////////////////////////////////////////////////////////////////////////
	// memory
	qword total = 0;
	qword free = 0;
	rx_collect_memory_info(&total, &free);	
	out << "Memory: Total " 
		<< (int)(total / 1048576ull) 
		<< "MB / Free "
		<< (int)(free / 1048576ull)  << "MB \r\n";
	/////////////////////////////////////////////////////////////////////////
	out << "Page size: " << (int)rx_os_page_size() << " bytes\r\n";

	return true;
}


// Class terminal::console::console_commands::cls_command 

cls_command::cls_command()
  : server_command("cls")
{
}


cls_command::~cls_command()
{
}



bool cls_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	out << ANSI_CLS ANSI_CUR_HOME;
	return true;
}


// Class terminal::console::console_commands::shutdown_command 

shutdown_command::shutdown_command()
  : server_command("shutdown")
{
}


shutdown_command::~shutdown_command()
{
}



bool shutdown_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type msg;
	in >> msg;
	rx_server::instance().shutdown(msg,err);
	return false;
}


// Class terminal::console::console_commands::log_command 

log_command::log_command()
	: server_command("log")
{
}


log_command::~log_command()
{
}



bool log_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type sub_command;
	in >> sub_command;
	if (sub_command.empty())
	{// "jbg prazno
		out << "Prazno je\r\n";
	}
	else if (sub_command == "test")
	{// testing stuff
		do_test_command(in, out, err, ctx);
	}
	else if (sub_command == "hist")
	{// testing stuff
		do_hist_command(in, out, err, ctx);
	}
	return true;
}

bool log_command::do_test_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{

	char buffer[0x100];

	snprintf(buffer, sizeof(buffer), "User %s performing log test."
		, security::active_security()->get_full_name().c_str());

	const char* line = buffer;
	CONSOLE_LOG_INFO("log", 900,line);
	out << line << "\r\n";

	double spans[4];

	for (size_t i = 0; i < sizeof(spans) / sizeof(spans[0]); i++)
	{
		snprintf(buffer, sizeof(buffer), "Console log test pass %d...", (int)i);
		rx::locks::event ev(false);
		qword first_tick = rx_get_us_ticks();
		RX_LOG_TEST(buffer, &ev);
		ev.wait_handle();
		qword second_tick = rx_get_us_ticks();
		double ms = (double)(second_tick - first_tick) / 1000.0;
		snprintf(buffer, sizeof(buffer), "Console log test %d passed. Delay time: %g ms...", (int)i, ms);
		CONSOLE_LOG_INFO("log",100,buffer);
		out << buffer << "\r\n";
		spans[i] = ms;
		rx_msleep(10);
	}

	double val = 0.0;
	size_t count = sizeof(spans) / sizeof(spans[0]);
	if (count > 1)
	{
		for (size_t i = 1; i < count; i++)
		{
			val += spans[i];
		}
		val = val / (double(count - 1));
	}
	else
		val = spans[0];
	snprintf(buffer, sizeof(buffer), "Average response time: %g ms...", val);
	line = buffer;
	CONSOLE_LOG_INFO("log", 900,line);
	out << line << "\r\n";

	snprintf(buffer, sizeof(buffer), "User %s log test completed."
		, security::active_security()->get_full_name().c_str());

	line = buffer;
	CONSOLE_LOG_INFO("log", 900,line);
	out << line << "\r\n";

	return true;
}

bool log_command::do_hist_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	log::log_query_type query;
	log::log_events_type result;
	auto ret = rx_server::instance().read_log(query, result);
	if (ret)
	{
		for (auto one : result)
		{
			one.dump_to_stream(out);
		}
	}
	return true;
}


// Class terminal::console::console_commands::sec_command 

sec_command::sec_command()
	: server_command("sec")
{
}


sec_command::~sec_command()
{
}



bool sec_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type sub_command;
	in >> sub_command;
	if (sub_command.empty())
	{// "jbg prazno
		out << "Prazno je\r\n";
	}
	else if (sub_command == "active")
	{// testing stuff
		do_active_command(in, out, err, ctx);
	}
	return true;
}

bool sec_command::do_active_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	std::vector<security::security_context_ptr> ctxs;
	security::security_manager::instance().get_active_contexts(ctxs);
	out << "Dumping active users:\r\n\r\n";

	size_t count = ctxs.size();
	table_type table(count+1);

	table[0].emplace_back("Id");
	table[0].emplace_back("User Name");
	table[0].emplace_back("Console");
	table[0].emplace_back("System");
	table[0].emplace_back("Port");

	rx_security_handle_t my_handle = security::active_security()->get_handle();

	for (size_t i=0; i<count; i++)
	{
		string_type name = ctxs[i]->get_full_name();
		rx_security_handle_t sec_handle = ctxs[i]->get_handle();
		string_type prefix;
		bool is_console = ctxs[i]->has_console();
		if (sec_handle == my_handle)
		{
			prefix = ANSI_COLOR_GREEN;
			name += "*";
		}
		else if (is_console)
			prefix = ANSI_COLOR_CYAN;

		char buff[0x20];
		snprintf(buff, sizeof(buff), "[%llu]",sec_handle);
		table[i + 1].emplace_back(buff);
		if (prefix.empty())
			table[i + 1].emplace_back(name);
		else
			table[i + 1].emplace_back(name, prefix,ANSI_COLOR_RESET);
		string_type temp;
		fill_context_attributes(ctxs[i], temp);
		//table[i + 1].emplace_back(temp);
		if (is_console)
			table[i + 1].emplace_back(RX_CONSOLE_CHECKED);
		else
			table[i + 1].emplace_back(RX_CONSOLE_UNCHECKED);
		if(ctxs[i]->is_system())
			table[i + 1].emplace_back(RX_CONSOLE_CHECKED);
		else
			table[i + 1].emplace_back(RX_CONSOLE_UNCHECKED);
		table[i + 1].emplace_back(ctxs[i]->get_port());
	}
	dump_table(table, out,true);

	return true;
}


// Class terminal::console::console_commands::time_command 

time_command::time_command()
	: server_command("time")
{
}


time_command::~time_command()
{
}



bool time_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	out << "Start Time\t: " << rx_server::instance().get_started().get_string() << " UTC\r\n";
	out << "Current Time\t: " << rx_time::now().get_string() << " UTC\r\n";
	return true;
}


// Class terminal::console::console_commands::sleep_command 

sleep_command::sleep_command()
	: server_command("sleep")
{
}


sleep_command::~sleep_command()
{
}



bool sleep_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	dword period = 0;
	in >> period;

	if (period == 0)
	{
		err << "Invalid period specified!";
		return false;
	}
	return true;
}


// Class terminal::console::console_commands::def_command 

def_command::def_command()
	: directory_aware_command("def")
{
}


def_command::~def_command()
{
}



bool def_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type whose;
	if (!in.eof())
		in >> whose;
	if (!whose.empty())
	{
		server_item_ptr item = ctx->get_current_directory()->get_sub_item(whose);
		if (item)
		{
			dump_object_definition(out,err, item);
		}
		else
		{
			err << "ERROR: unknown object name";
			return false;
		}
	}
	else
	{
		err << "ERROR: unknown object name";
		return false;
	}
	return true;
}

bool def_command::dump_object_definition (std::ostream& out, std::ostream& err, server_item_ptr item)
{
	bool ret = false;
	if (item)
	{
		ret =item->generate_json(out, err);
	}
	else
	{
		err << "Unknown Item!!!";
	}
	return ret;
}


// Class terminal::console::console_commands::directory_aware_command 

directory_aware_command::directory_aware_command (const string_type& console_name)
	: server_command(console_name)
{
}


directory_aware_command::~directory_aware_command()
{
}



// Class terminal::console::console_commands::phyton_command 

phyton_command::phyton_command()
	: server_command("python")
{
}


phyton_command::~phyton_command()
{
}



bool phyton_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type sub_command;
	in >> sub_command;
	if (sub_command.empty())
	{// "jbg prazno
		err << "It,s not over yet, be more paitent...";
		return false;
	}
	else if (sub_command == "version" || sub_command == "ver")
	{// testing stuff
		out << "Embedded Python Version\r\n======================================\r\n";
		python::py_script::instance().dump_script_information(out);
		out << "\r\n";
	}
	return true;
}


} // namespace console_commands
} // namespace console
} // namespace terminal

