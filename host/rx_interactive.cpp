

/****************************************************************************
*
*  host\rx_interactive.cpp
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

#include <iostream>
#include "system/server/rx_server.h"

// rx_interactive
#include "host/rx_interactive.h"

#include "system/constructors/rx_construct.h"
#include "lib/rx_log.h"


namespace host {

namespace interactive {

// Class host::interactive::interactive_console_host 

interactive_console_host::interactive_console_host()
      : _exit(false),
        _testBool(true)
{
}


interactive_console_host::~interactive_console_host()
{
}



void interactive_console_host::main (interactive_console_host& host)
{

	host_security_context::smart_ptr sec_ctx(pointers::_create_new);
	sec_ctx->login();

	security::security_auto_context dummy(sec_ctx);

	server::configuration_data_t config;
	config.managment_data.telnet_port = 12345;
	config.runtime_data.genereal_pool_size = 2;
	config.runtime_data.io_pool_size = 4;
	config.runtime_data.workers_pool_size = 2;


	HOST_LOG_INFO("Main", 999, "Initializing Rx Engine...");

	if (RX_OK == server::rx_server::instance().initialize(&host, config))
	{

		HOST_LOG_INFO("Main", 999, "Starting Rx Engine...");

		if (RX_OK == server::rx_server::instance().start(&host, config))
		{
			interactive_console_client interactive;

			interactive.run_interactive(host);

			server::rx_server::instance().stop();
		}
		server::rx_server::instance().deinitialize();
	}


	printf("Closing console...\r\n");

	std::cout.flush();

	std::cin.clear();

}

const string_type& interactive_console_host::get_host_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, "Interactive Console Host Ver 0.8.2");
	}
	return ret;
}

void interactive_console_host::server_started_event ()
{
	printf("\r\nStarting RX Engine...\r\n==========================================\r\n");
}

bool interactive_console_host::shutdown (const string_type& msg)
{
	security::security_context_ptr ctx = security::active_security();
	std::cout << "\r\n" ANSI_COLOR_RED "SHUTDOWN" ANSI_COLOR_RESET " initiated by " ANSI_COLOR_GREEN << ctx->get_full_name();
	std::cout << ANSI_COLOR_RESET "\r\n";
	std::cout.flush();
	_exit = true;

	return true;
}

bool interactive_console_host::exit () const
{
	return _exit;
}

void interactive_console_host::get_host_objects (std::vector<server::objects::object_runtime_ptr>& items)
{
	constructors::user_object_constructor constructor;
//	server::objects::user_object::smart_ptr test(pointers::_create_new);
	server::objects::user_object::smart_ptr test("test_object", 55);
	test->register_const_value("testBool", _testBool);
	items.push_back(test);
}

void interactive_console_host::get_host_classes (std::vector<server::meta::object_class_ptr>& items)
{
	server::meta::object_class_ptr test("test_class", 55, true);
	test->register_const_value("testBool", false);
	items.push_back(test);
}

bool interactive_console_host::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx)
{
	std::ostream out(out_buffer.unsafe_ptr());
	std::stringstream in(line);
	std::ostream err(err_buffer.unsafe_ptr());

	bool ret = false;

	string_type command;
	in >> command;
	if (command == "frun")
	{
		string_type file_name;
		in >> file_name;
		file_name = "D:\\RX\\Native\\Source/host/scripts/platform script one.rxs";
		sys_handle_t file = rx_file(file_name.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
		if (file)
		{
			memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer(pointers::_create_new);
			if (buffer->fill_with_file_content(file))
			{
				out << "file loadad in memory...\r\n";
				out << "Running file script:" << file_name;
				out << "\r\n=====================================\r\n";

				while (!buffer->eof())
				{
					string_type line;
					buffer->read_line(line);
					out << ANSI_COLOR_GREEN ">" ANSI_COLOR_RESET << line << "\r\n";
				}

				out << "=====================================\r\nScript done.\r\n";
				ret = true;
			}
			else
			{
				err << "error reading file contet\r\n";
			}
			rx_file_close(file);
			return ret;
		}
		else
		{
			err << "error oppening file\r\n";
		}
	}
	else
	{
		err << "Unknown command:" ANSI_COLOR_YELLOW << command << ANSI_COLOR_RESET << ".\r\n";
	}
	return ret;
}


// Class host::interactive::interactive_console_client 

interactive_console_client::interactive_console_client()
      : _exit(false)
  , _security_context(pointers::_create_new)
{
	_security_context->login();
}


interactive_console_client::~interactive_console_client()
{
}



const string_type& interactive_console_client::get_console_name ()
{
	static string_type ret("Process Interactive Console");
	return ret;
}

void interactive_console_client::run_interactive (const interactive_console_host& host)
{

	security::security_auto_context dummy(_security_context);

	string_type temp;
	get_wellcome(temp);
	std::cout << temp<<"\r\n";


	while (!_exit && !host.exit())
	{

		while (is_postponed())
		{
			rx_msleep(50);
		}

		temp.clear();
		get_prompt(temp);

		std::cout << temp;

		string_type line;
		std::getline(std::cin, line);

		if(std::cin.fail())
		{
            std::cin.clear();
		}
		if (!line.empty())
		{
			memory::buffer_ptr out_buffer(pointers::_create_new);
			memory::buffer_ptr err_buffer(pointers::_create_new);

			if (!do_command(line, out_buffer, err_buffer,*_security_context))
			{
				if (!err_buffer->empty())
					std::cout.write((const char*)err_buffer->pbase(), err_buffer->get_size());
				if (!_exit && !host.exit())
				{
					std::cout << "\r\n";
					std::cout << ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET;
					std::cout << "\r\n";
				}
				if (!out_buffer->empty())
					std::cout.write((const char*)out_buffer->pbase(), out_buffer->get_size());
			}
			else
			{

				if (!out_buffer->empty())
					std::cout.write((const char*)out_buffer->pbase(), out_buffer->get_size());

				//std::cout << "\r\n";
			}

		}

	}
	_security_context->logout();
}

void interactive_console_client::virtual_bind ()
{
}

void interactive_console_client::virtual_release ()
{
}

security::security_context::smart_ptr interactive_console_client::get_current_security_context ()
{
	return _security_context;
}

void interactive_console_client::exit_console ()
{
	_exit = true;
}


// Class host::interactive::interactive_security_context 

interactive_security_context::interactive_security_context()
{
	_user_name = "interactive";
	_full_name = _user_name + "@";
	_full_name += _location;
	_port = "internal";
}


interactive_security_context::~interactive_security_context()
{
}



bool interactive_security_context::has_console () const
{
  return true;

}

bool interactive_security_context::is_system () const
{
  return true;

}

bool interactive_security_context::is_interactive () const
{
  return true;

}


} // namespace interactive
} // namespace host

