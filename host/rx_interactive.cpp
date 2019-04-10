

/****************************************************************************
*
*  host\rx_interactive.cpp
*
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

// adding command line parsing library
// see <https://github.com/jarro2783/cxxopts>
#include "third-party/cxxopts/include/cxxopts.hpp"
#include "rx_interactive_version.h"
#include "terminal/rx_terminal_style.h"
#include "system/hosting/rx_yaml.h"

// rx_host
#include "system/hosting/rx_host.h"
// rx_interactive
#include "host/rx_interactive.h"

#include "system/constructors/rx_construct.h"
#include "api/rx_meta_api.h"

//#define INTERACTIVE_HOST_INFO "Interactive Console Host Ver 0.9.2"


namespace host {

namespace interactive {

// Class host::interactive::interactive_console_host 

interactive_console_host::interactive_console_host (hosting::rx_host_storages& storage)
      : exit_(false)
	, hosting::rx_platform_host(storage)
{
	//startup_script_ = "test run meta/construct-wide\n";
	//startup_script_ = "test run -a\n";
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
	std::cout << "Initializing rx-platform...";
	auto result = rx_platform::rx_gate::instance().initialize(this, config);
	if (result)	
	{
		std::cout << ANSI_STATUS_OK "\r\n";
		HOST_LOG_INFO("Main", 999, "Starting Rx Engine...");
		std::cout << "Starting rx-platform...";
		result = rx_platform::rx_gate::instance().start(this, config);
		if (result)
		{
			std::cout << ANSI_STATUS_OK "\r\n";
			interactive_console_client interactive(this);

			std::cout << "Starting interactive console...";
			std::cout << ANSI_STATUS_OK "\r\n";
			result = interactive.run_interactive(config);
			if (!result)
			{
				std::cout << ANSI_STATUS_ERROR "\r\nError running Interactive Console:\r\n";
				rx_dump_error_result(std::cout, result);
			}
			std::cout << "Stopping rx-platform...";
			result = rx_platform::rx_gate::instance().stop();
			if(result)
				std::cout << ANSI_STATUS_OK "\r\n";
			else
			{
				std::cout << ANSI_STATUS_ERROR "\r\nError stopping rx-platform:\r\n";
				rx_dump_error_result(std::cout, result);
			}

		}
		else
		{
			std::cout << ANSI_STATUS_ERROR "\r\nError starting rx-platform:\r\n";
			rx_dump_error_result(std::cout, result);
		}
		std::cout << "De-initializing rx-platform...";
		result = rx_platform::rx_gate::instance().deinitialize();
		if (result)
			std::cout << ANSI_STATUS_OK "\r\n";
		else
		{
			std::cout << ANSI_STATUS_ERROR "\r\nError deinitialize rx-platform:\r\n";
			rx_dump_error_result(std::cout, result);
		}
	}
	else
	{
		std::cout << ANSI_STATUS_ERROR "\r\nError initializing rx-platform:\r\n";
		rx_dump_error_result(std::cout, result);
	}

	HOST_LOG_INFO("Main", 999, "Closing console...");
}

void interactive_console_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_interactive_info());
	rx_platform_host::get_host_info(hosts);
}

void interactive_console_host::server_started_event ()
{
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

void interactive_console_host::get_host_objects (std::vector<rx_platform::runtime::object_runtime_ptr>& items)
{
}

void interactive_console_host::get_host_types (std::vector<rx_platform::meta::object_type_ptr>& items)
{
	//items.push_back(rx_create_reference<meta::object_types::port_class>(meta::object_types::object_type_creation_data{ RX_INTERACTIVE_TYPE_NAME, RX_INTERACTIVE_TYPE_ID, RX_PHYSICAL_PORT_TYPE_ID, true }));
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
		auto storage = get_user_storage();
		if (storage)
		{
			sys_handle_t file = 0;// storage->get_host_console_script_file(file_name);
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

bool interactive_console_host::parse_command_line (int argc, char* argv[], rx_platform::configuration_data_t& config)
{
	
	cxxopts::Options options("rx-interactive", "");

	add_command_line_options(options, config);

	try
	{
		auto result = options.parse(argc, argv);
		if (result.count("help"))
		{

			restore_console();

			std::cout << options.help({ "" });
			std::cout << "\r\n\r\n";

			// don't execute
			return false;
		}
		else if (result.count("version"))
		{

			string_type version = rx_gate::instance().get_rx_version();

			std::cout << "\r\n" ANSI_COLOR_GREEN ANSI_COLOR_BOLD
				<< version << ANSI_COLOR_RESET "\r\n\r\n";
			
			restore_console();
			
			// don't execute
			return false;
		}
		return true;
	}
	catch (std::exception& ex)
	{
		std::cout << ANSI_STATUS_ERROR "\r\nError parsing command line:\r\n"
			<< ex.what() << "\r\n";

		restore_console();

		return false;
	}
}

int interactive_console_host::console_main (int argc, char* argv[])
{
	rx_result ret = setup_console(argc, argv);

	std::cout << "\r\n"
		<< ANSI_COLOR_GREEN ANSI_COLOR_BOLD "rx-platform" ANSI_COLOR_RESET " Interactive Console Host"
		<< "\r\n======================================\r\n";

	rx_platform::configuration_data_t config;
	ret = parse_command_line(argc, argv, config); 
	if (ret)
	{		
		rx_platform::hosting::simplified_yaml_reader reader;
		std::cout << "Reading configuration file...";
		ret = read_config_file(reader, config);
		if (ret)
		{			
			std::cout << ANSI_STATUS_OK "\r\n";
			rx_thread_data_t tls = rx_alloc_thread_data();
			string_type server_name = get_default_name();

			std::cout << "Initializing OS interface...";
			rx_initialize_os(config.runtime_data.real_time, tls, server_name.c_str());
			std::cout << ANSI_STATUS_OK "\r\n";

			std::cout << "Starting log...";
			ret = rx::log::log_object::instance().start(config.general.test_log);			
			if (ret)
			{
				std::cout << ANSI_STATUS_OK "\r\n";

				std::cout << "Initializing storages...";
				ret = initialize_storages(config);
				if (ret)
				{
					std::cout << ANSI_STATUS_OK "\r\n";

					HOST_LOG_INFO("Main", 999, "Starting Console Host...");
					// execute main loop of the console host
					console_loop(config);
					HOST_LOG_INFO("Main", 999, "Console Host exited.");
					
					deinitialize_storages();
				}
				else
				{
					std::cout << ANSI_STATUS_ERROR "\r\nError initializing storages\r\n";
					rx_dump_error_result(std::cout, ret);
				}
				rx::log::log_object::instance().deinitialize();
			}

			rx_deinitialize_os();
		}
		else
		{
			std::cout << ANSI_STATUS_ERROR "\r\nError reading configuration file\r\n";
			rx_dump_error_result(std::cout, ret);
		}
		restore_console();
	}
	std::cout << "\r\n";
	return ret ? 0 : -1;
}

rx_result interactive_console_host::setup_console (int argc, char* argv[])
{
	return true;
}

rx_result interactive_console_host::restore_console ()
{
	return true;
}

string_type interactive_console_host::get_interactive_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_HOST_NAME, RX_HOST_MAJOR_VERSION, RX_HOST_MINOR_VERSION, RX_HOST_BUILD_NUMBER);
	}
	return ret;
}


// Class host::interactive::interactive_console_client 

string_type interactive_console_client::license_message = RX_LICENSE_MESSAGE;

interactive_console_client::interactive_console_client (hosting::rx_platform_host* host)
      : host_(host),
        exit_(false)
	, security_context_(pointers::_create_new)
	, console_client(runtime::objects::port_creation_data {
		RX_INTERACTIVE_NAME
		, RX_INTERACTIVE_ID
		, RX_CONSOLE_TYPE_ID
		, RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_PLUGINS_NAME RX_DIR_DELIMETER_STR RX_NS_HOST_NAME
		, rx_system_application() })
{
	security_context_->login();

	/*auto result = api::meta::rx_create_port(
		RX_INTERACTIVE_NAME
		, RX_INTERACTIVE_TYPE_NAME
		, nullptr
		,
	)*/
	
	/*auto directory = rx_gate::instance().get_root_directory()->get_sub_directory(RX_NS_SYS_NAME "/" RX_NS_OBJ_NAME);
	if(directory)
		directory->add_item(smart_this()->get_item_ptr());*/
}


interactive_console_client::~interactive_console_client()
{
}



const string_type& interactive_console_client::get_console_name ()
{
	static string_type temp("Interactive");
	return temp;
}

rx_result interactive_console_client::run_interactive (configuration_data_t& config)
{

	auto result = check_validity();
	if (!result)
		return result;

	security::security_auto_context dummy(security_context_);


	string_type temp_script(config.general.startup_script);
	if(!temp_script.empty())
		temp_script += "\r\n";

	host_->write_stdout(license_message);

	string_type temp;
	get_wellcome(temp);
	temp += "\r\n";
	host_->write_stdout(temp);

	temp.clear();
	get_prompt(temp);
	host_->write_stdout(temp);
	
	std::array<char, 0x100> buffer;

	size_t count;
	
	while (!exit_ && !host_->exit())
	{
		if (temp_script.empty())
		{
			count = 0;
			host_->read_stdin(buffer, count);

			if (count == 1 && buffer[0] == 3)
			{
				memory::buffer_ptr out_buffer(pointers::_create_new);
				memory::buffer_ptr err_buffer(pointers::_create_new);

				cancel_command(out_buffer, err_buffer, security_context_);

				continue;
			}

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
		}
		else
		{
			count = temp_script.size();
			if (count > buffer.size())
			{
				memcpy(&buffer[0], temp_script.c_str(), buffer.size());
				count = buffer.size();
				temp_script = temp_script.substr(count);
			}
			else
			{
				memcpy(&buffer[0], temp_script.c_str(), count);
				temp_script.clear();
			}
		}

		if (is_postponed())
			continue;

		temp.clear();
		string_array lines;
		for (size_t i = 0; i < count; i++)
		{

			vt100_transport_.char_received(buffer[i], false, temp, [&lines, this](const string_type& line)
			{
				lines.push_back(line);
			});
		}
		if (!temp.empty() && 
			!rx_platform::rx_gate::instance().is_shutting_down())
			host_->write_stdout(temp);

		if (!lines.empty())
		{

			memory::buffer_ptr out_buffer(pointers::_create_new);
			memory::buffer_ptr err_buffer(pointers::_create_new);

			do_commands(std::move(lines), out_buffer, err_buffer, security_context_);
		}

	}
	security_context_->logout();

	return true;
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
		if (!out_buffer->empty())
			host_->write_stdout(out_buffer->pbase(), out_buffer->get_size());
		if (!exit_ && !host_->exit())
		{
			host_->write_stdout(
				"\r\n" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "\r\n"
			);
		}
		if (!err_buffer->empty())
			host_->write_stdout(err_buffer->pbase(), err_buffer->get_size());
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

