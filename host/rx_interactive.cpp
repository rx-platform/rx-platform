

/****************************************************************************
*
*  host\rx_interactive.cpp
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

#include "rx_interactive_version.h"

// rx_interactive
#include "host/rx_interactive.h"

#include "system/constructors/rx_construct.h"

//#define INTERACTIVE_HOST_INFO "Interactive Console Host Ver 0.9.2"


namespace host {

namespace interactive {

// Class host::interactive::interactive_console_host 

interactive_console_host::interactive_console_host (rx_platform::hosting::rx_platform_storage::smart_ptr storage)
      : exit_(false)
	, hosting::rx_platform_host(storage)
{
	startup_script_ = "test run lib/values/r/n";
}


interactive_console_host::~interactive_console_host()
{
}



void interactive_console_host::console_loop (configuration_data_t& config)
{

	rx_platform::hosting::host_security_context::smart_ptr sec_ctx(pointers::_create_new);
	sec_ctx->login();

	security::security_auto_context dummy(sec_ctx);

	if (!config.managment_data.telnet_port)// set to the last default if not set
		config.managment_data.telnet_port = 12345;
	if (config.runtime_data.genereal_pool_size < 0)
		config.runtime_data.genereal_pool_size = 2;
	if (config.runtime_data.io_pool_size <= 0)// has to have at least one
		config.runtime_data.io_pool_size = 1;
	if (config.runtime_data.workers_pool_size < 0)
		config.runtime_data.workers_pool_size = 2;


	HOST_LOG_INFO("Main", 999, "Initializing Rx Engine...");

	if (RX_OK == rx_platform::rx_gate::instance().initialize(this, config))
	{

		HOST_LOG_INFO("Main", 999, "Starting Rx Engine...");

		if (RX_OK == rx_platform::rx_gate::instance().start(this, config))
		{
			interactive_console_client interactive(this);

			interactive.run_interactive();

			rx_platform::rx_gate::instance().stop();
		}
		rx_platform::rx_gate::instance().deinitialize();
	}

	HOST_LOG_INFO("Main", 999, "Closing console...");
}

void interactive_console_host::get_host_info (string_array& hosts)
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_HOST_NAME, RX_HOST_MAJOR_VERSION, RX_HOST_MINOR_VERSION, RX_HOST_BUILD_NUMBER);
	}
	hosts.emplace_back(ret);
	rx_platform_host::get_host_info(hosts);
}

void interactive_console_host::server_started_event ()
{
	printf("\r\nStarting rx-platform...\r\n==========================================\r\n");
}

bool interactive_console_host::shutdown (const string_type& msg)
{
	security::security_context_ptr ctx = security::active_security();
	std::cout << "\r\n" ANSI_COLOR_RED "SHUTDOWN" ANSI_COLOR_RESET " initiated by " ANSI_COLOR_GREEN << ctx->get_full_name();
	std::cout << ANSI_COLOR_RESET "\r\n";
	std::cout << "Message:" << msg << "\r\n";
	std::cout.flush();
	exit_ = true;
	rx_gate::instance().get_host()->break_host("");
return true;
}

bool interactive_console_host::exit () const
{
	return exit_;
}

void interactive_console_host::get_host_objects (std::vector<rx_platform::objects::object_runtime_ptr>& items)
{
}

void interactive_console_host::get_host_classes (std::vector<rx_platform::meta::object_class_ptr>& items)
{
	//items.push_back(rx_create_reference<meta::object_defs::port_class>(meta::object_defs::object_type_creation_data{ RX_INTERACTIVE_TYPE_NAME, RX_INTERACTIVE_TYPE_ID, RX_PHYSICAL_PORT_TYPE_ID, true }));
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
				file_name = "platform script one.rxs";
				auto storage = get_storage();
				if (storage)
				{
					sys_handle_t file = storage->get_host_console_script_file(file_name);
					if (file)
					{
						memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer(pointers::_create_new);
						if (buffer->fill_with_file_content(file))
						{
							out << "file loaded in memory...\r\n";
							out << "Running file script:" << file_name;
							out << "\r\n=====================================\r\n";

							while (!buffer->eof())
							{
								string_type line;
								buffer->read_line(line);
								out << ANSI_COLOR_GREEN ">>>" ANSI_COLOR_RESET << line << "\r\n";
							}

							out << "=====================================\r\nScript done.\r\n";
							ret = true;
						}
						else
						{
							err << "error reading file content\r\n";
						}
						rx_file_close(file);
						return ret;
					}
					else
					{
						err << "error opening file\r\n";
					}
				}
			}
			else
			{
				err << "Unknown command:" ANSI_COLOR_YELLOW << command << ANSI_COLOR_RESET << ".\r\n";
			}
			return ret;
}

bool interactive_console_host::write_stdout (const string_type& lines)
{
			return write_stdout(lines.c_str(), lines.size());
}

string_type interactive_console_host::get_startup_script ()
{
			return startup_script_;
}

std::vector<ETH_interface> interactive_console_host::get_ETH_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
			std::vector<ETH_interface> ret;
			return ret;
}

std::vector<IP_interface> interactive_console_host::get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
			std::vector<IP_interface> ret;
			return ret;
}


// Class host::interactive::interactive_console_client 

interactive_console_client::interactive_console_client (interactive_console_host* host)
      : host_(host),
        exit_(false)
			, security_context_(pointers::_create_new)
			, console_client(objects::port_creation_data { RX_INTERACTIVE_NAME, RX_INTERACTIVE_ID, RX_CONSOLE_TYPE_ID, rx_system_application() })
{
	security_context_->login();
	auto directory = rx_gate::instance().get_root_directory()->get_sub_directory(RX_NS_SYS_NAME "/" RX_NS_OBJ_NAME);
	if(directory)
		directory->add_item(smart_this()->get_item_ptr());
}


interactive_console_client::~interactive_console_client()
{
}



const string_type& interactive_console_client::get_console_name ()
{
	static string_type temp("Interactive");
	return temp;
}

void interactive_console_client::run_interactive ()
{

	security::security_auto_context dummy(security_context_);

	host_->write_stdout("\
\r\n\
Copyright(C) 2018  Dusan Ciric\r\n\r\n\
This program comes with ABSOLUTELY NO WARRANTY; for details type `license'.\r\n\
This is free software, and you are welcome to redistribute it\r\n\
under certain conditions; type `license' for details.\r\n\
\r\n" 
	);

	string_type temp;
	get_wellcome(temp);
	temp += "\r\n";
	host_->write_stdout(temp);

	temp.clear();
	get_prompt(temp);
	host_->write_stdout(temp);

	string_type chars;

	std::array<char, 0x100> buffer;

	size_t count;
	
	while (!exit_ && !host_->exit())
	{
		count = 0;
		host_->read_stdin(buffer, count);

		if (std::cin.fail())
		{
			std::cin.clear();
			if (host_->is_canceling())
			{
				memory::buffer_ptr out_buffer(pointers::_create_new);
				memory::buffer_ptr err_buffer(pointers::_create_new);

				cancel_command(out_buffer, err_buffer, security_context_);
			}
			if (rx_gate::instance().is_shutting_down())
				break;

			continue;
		}

		if (is_postponed())
			continue;

		temp.clear();
		for (size_t i = 0; i < count; i++)
		{

			vt100_transport_.char_received(buffer[i], false, temp, [this](const string_type& line)
			{
				memory::buffer_ptr out_buffer(pointers::_create_new);
				memory::buffer_ptr err_buffer(pointers::_create_new);

				do_command(string_type(line), out_buffer, err_buffer, security_context_);
			});
		}
		if (!temp.empty() && 
			!rx_platform::rx_gate::instance().is_shutting_down())
			host_->write_stdout(temp);

	}
	security_context_->logout();
}

security::security_context::smart_ptr interactive_console_client::get_current_security_context ()
{
	return security_context_;
}

void interactive_console_client::exit_console ()
{
	rx_platform::rx_gate::instance().shutdown("Interactive Shutdown");
}

void interactive_console_client::process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer)
{
	if (!result)
	{
		if (!err_buffer->empty())
			host_->write_stdout(err_buffer->pbase(), err_buffer->get_size());
		if (!exit_ && !host_->exit())
		{
			host_->write_stdout(
				"\r\n" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "\r\n"
			);
		}
		if (!out_buffer->empty())
			host_->write_stdout(out_buffer->pbase(), out_buffer->get_size());
	}
	else
	{

		if (!out_buffer->empty())
			host_->write_stdout(out_buffer->pbase(), out_buffer->get_size());

	}

	if (!rx_gate::instance().is_shutting_down())
	{
		string_type temp;
		get_prompt(temp);
		host_->write_stdout(temp);
	}
	//rx_gate::instance().get_host()->break_host("");
}

bool interactive_console_client::readed (const void* data, size_t count, rx_thread_handle_t destination)
{
	return false;
}


// Class host::interactive::interactive_security_context 

interactive_security_context::interactive_security_context()
{
	user_name_ = "interactive";
	full_name_ = user_name_ + "@";
	full_name_ += location_;
	port_ = "internal";
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

