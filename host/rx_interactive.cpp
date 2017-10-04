

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

class my_periodic_job : public rx::jobs::periodic_job
{
    DECLARE_REFERENCE_PTR(my_periodic_job);
public:
    my_periodic_job()
    {
    }
    void process()
    {
        printf("Timer fired at %s\r\n",rx::rx_time(rx::time_stamp::now().rx_time).get_string().c_str());
    }
};

void test_timers()
{
    rx::threads::dispatcher_pool pool(2,"ime");
    rx::threads::timer tm;

    pool.run();
    tm.start();

    printf("Sleeping\r\n");

    my_periodic_job::smart_ptr job(rx::pointers::_create_new);
    tm.append_job(job,&pool,100,false);

    rx_msleep(1000);


    printf("Stopping\r\n");

    tm.stop();
    tm.wait_handle();
    pool.end();
}


namespace host {

namespace interactive {

// Class host::interactive::interactive_console_host 

interactive_console_host::interactive_console_host()
      : m_exit(false)
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
	static string_type ret=CREATE_MODULE_VERSION("Interactive Console Host Ver 0.8.0");
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
	m_exit = true;

	return true;
}

bool interactive_console_host::exit () const
{
	return m_exit;
}

void interactive_console_host::get_host_objects (std::vector<server::objects::object_runtime_ptr>& items)
{
	constructors::user_object_constructor constructor;
//	server::objects::user_object::smart_ptr test(pointers::_create_new);
	server::objects::user_object::smart_ptr test("test_object", 55);
	items.push_back(test);
}

void interactive_console_host::get_host_classes (std::vector<server::meta::object_class_ptr>& items)
{
	server::meta::object_class_ptr test("test_class", 55, true);
	test->register_const_value("testBool", false);
	items.push_back(test);
}


// Class host::interactive::interactive_console_client 

interactive_console_client::interactive_console_client()
      : m_exit(false)
  , m_security_context(pointers::_create_new)
{
	m_security_context->login();
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

	security::security_auto_context dummy(m_security_context);

	string_type temp;
	get_wellcome(temp);
	std::cout << temp<<"\r\n";


	while (!m_exit && !host.exit())
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

			if (!do_command(line, out_buffer, err_buffer,*m_security_context))
			{
				if (!err_buffer->empty())
					std::cout.write((const char*)err_buffer->pbase(), err_buffer->get_size());
				if (!m_exit && !host.exit())
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
	m_security_context->logout();
}

void interactive_console_client::virtual_bind ()
{
}

void interactive_console_client::virtual_release ()
{
}

security::security_context::smart_ptr interactive_console_client::get_current_security_context ()
{
	return m_security_context;
}

void interactive_console_client::exit_console ()
{
	m_exit = true;
}


// Class host::interactive::interactive_security_context 

interactive_security_context::interactive_security_context()
{
	m_user_name = "interactive";
	m_full_name = m_user_name + "@";
	m_full_name += m_location;
	m_port = "internal";
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


} // namespace interactive
} // namespace host

