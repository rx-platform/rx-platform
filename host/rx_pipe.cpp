

/****************************************************************************
*
*  host\rx_pipe.cpp
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

#include "rx_pipe_version.h"

// rx_pipe
#include "host/rx_pipe.h"

// adding command line parsing library
// see <https://github.com/jarro2783/cxxopts>
#include "third-party/cxxopts/include/cxxopts.hpp"
#include "system/server/rx_server.h"
#include "system/hosting/rx_yaml.h"
#include "sys_internal/rx_internal_protocol.h"


namespace host {

namespace pipe {

// Class host::pipe::rx_pipe_host 

rx_pipe_host::rx_pipe_host (hosting::rx_host_storages& storage)
      : exit_(false)
	, hosting::rx_platform_host(storage)
{
}


rx_pipe_host::~rx_pipe_host()
{
}



void rx_pipe_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_pipe_info());
	rx_platform_host::get_host_info(hosts);
}

void rx_pipe_host::server_started_event ()
{
}

bool rx_pipe_host::shutdown (const string_type& msg)
{
	exit_ = true;
	return true;
}

bool rx_pipe_host::exit () const
{
	return exit_;
}

void rx_pipe_host::get_host_objects (std::vector<rx_platform::runtime::object_runtime_ptr>& items)
{
}

void rx_pipe_host::get_host_types (std::vector<rx_platform::meta::object_type_ptr>& items)
{
}

bool rx_pipe_host::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx)
{
	return true;
}

bool rx_pipe_host::break_host (const string_type& msg)
{
	return true;
}

int rx_pipe_host::pipe_main (int argc, char* argv[])
{
	std::cout << "\r\n"
		<< "rx-platform " 
		<< rx_gate::instance().get_rx_version()
		<< "\r\n\r\n"
		<< get_pipe_info()
		<< "\r\n======================================\r\n";


	rx_platform::configuration_data_t config;
	pipe_client_t pipes;
	memzero(&pipes, sizeof(pipes));
	rx_result ret = parse_command_line(argc, argv, config, pipes);
	if (ret)
	{
		rx_platform::hosting::simplified_yaml_reader reader;
		std::cout << "Reading configuration file...";
		ret = read_config_file(reader, config);
		if (ret)
		{
			std::cout << "OK\r\n";
			rx_thread_data_t tls = rx_alloc_thread_data();
			string_type server_name = get_default_name();

			//config.namespace_data.build_system_from_code = true;

			std::cout << "Initializing OS interface...";
			rx_initialize_os(config.runtime_data.real_time, tls, server_name.c_str());
			std::cout << "OK\r\n";

			std::cout << "Starting log...";
			ret = rx::log::log_object::instance().start(config.general.test_log);
			if (ret)
			{
				std::cout << "OK\r\n";

				std::cout << "Initializing storages...";
				ret = initialize_storages(config);

				if (ret)
				{
					std::cout << "OK\r\n";

					HOST_LOG_INFO("Main", 999, "Starting Console Host...");
					// execute main loop of the console host
					pipe_loop(config, pipes);
					HOST_LOG_INFO("Main", 999, "Console Host exited.");

					deinitialize_storages();
				}
				else
				{
					std::cout << "ERROR\r\nError initializing storages\r\n";
					rx_dump_error_result(std::cout, ret);
				}
				rx::log::log_object::instance().deinitialize();
			}

			rx_deinitialize_os();
		}
		else
		{
			std::cout << "ERROR\r\nError reading configuration file\r\n";
			rx_dump_error_result(std::cout, ret);
		}
	}
	std::cout << "\r\n";
	return ret ? 0 : -1;
}

string_type rx_pipe_host::get_pipe_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_HOST_NAME, RX_HOST_MAJOR_VERSION, RX_HOST_MINOR_VERSION, RX_HOST_BUILD_NUMBER);
	}
	return ret;
}

bool rx_pipe_host::is_canceling () const
{
	return false;
}

bool rx_pipe_host::read_stdin (std::array<char,0x100>& chars, size_t& count)
{
	return false;
}

bool rx_pipe_host::write_stdout (const void* data, size_t size)
{
	return false;
}

bool rx_pipe_host::parse_command_line (int argc, char* argv[], rx_platform::configuration_data_t& config, pipe_client_t& pipes)
{

	cxxopts::Options options("rx-pipe", "");

	intptr_t read_handle;
	intptr_t write_handle;

	options.add_options()
		("input", "Handle of the input pipe for this child process", cxxopts::value<intptr_t>(read_handle))
		("output", "Handle of the output pipe for this child process", cxxopts::value<intptr_t>(write_handle))
		;

	add_command_line_options(options, config);

	try
	{
		auto result = options.parse(argc, argv);
		if (result.count("help"))
		{
			std::cout << options.help({ "" });
			std::cout << "\r\n\r\n";

			// don't execute
			return false;
		}
		else if (result.count("version"))
		{

			string_type version = rx_gate::instance().get_rx_version();

			std::cout << "\r\n"
				<< version << "\r\n\r\n";

			// don't execute
			return false;
		}
		else if (read_handle == 0 || write_handle == 0)
		{
			std::cout << "\r\nThis is a child process and I/O handles have to be supplied"
				<< "\r\nUse --input and --output options to specify handles."
				<< "\r\nExiting...";

			return false;
		}
		pipes.client_read = (sys_handle_t)read_handle;
		pipes.client_write = (sys_handle_t)write_handle;
		return true;
	}
	catch (std::exception& ex)
	{
		std::cout <<  "\r\nError parsing command line:\r\n"
			<< ex.what() << "\r\n";

		return false;
	}
}

void rx_pipe_host::pipe_loop (configuration_data_t& config, const pipe_client_t& pipes)
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
		std::cout << "OK\r\n";
		HOST_LOG_INFO("Main", 999, "Starting Rx Engine...");
		std::cout << "Starting rx-platform...";
		result = rx_platform::rx_gate::instance().start(this, config);
		if (result)
		{
			std::cout << "OK\r\n";
			std::cout << "Starting pipe communication...";

			rx_protocol_result_t res = opcua_bin_init_pipe_transport(&transport_, 0x10000, 0x10);
			
			if (res == RX_PROTOCOL_OK)
			{
				result = pipes_.open(pipes);
				if (result)
				{
					res = rx_push_stack(&pipes_, &transport_.protocol_stack_entry);

					auto json = rx_create_reference< sys_internal::rx_protocol::rx_protocol_port>();
					res = rx_push_stack(&transport_.protocol_stack_entry, json->get_stack_entry());

					std::cout << "OK\r\n";
					
					string_type sys_info = get_system_storage()->get_storage_info();
					string_type sys_ref = get_system_storage()->get_storage_reference();
					string_type user_info = get_user_storage()->get_storage_info();
					string_type user_ref = get_user_storage()->get_storage_reference();
					string_type test_info = get_test_storage()->get_storage_info();
					string_type test_ref = get_test_storage()->get_storage_reference();

					std::cout << "Storage Information:\r\n============================\r\n";
					std::cout << "System Storage: " << sys_info << "\r\n";
					std::cout << "System Reference: " << sys_ref << "\r\n";
					std::cout << "User Storage: " << user_info << "\r\n";
					std::cout << "User Reference: " << user_ref << "\r\n";
					std::cout << "Test Storage: " << test_info << "\r\n";
					std::cout << "Test Reference: " << test_ref << "\r\n";

					pipes_.receive_loop();

					pipes_.close();

					opcua_bin_deinit_transport(&transport_);
				}
				else
				{
					std::cout << "ERROR\r\nError initializing pipe endpoint!\r\n";
					rx_dump_error_result(std::cout, result);
				}
			}
			else
				std::cout << "ERROR\r\nError initializing transport protocol!\r\n";


			
			std::cout << "Stopping rx-platform...";
			result = rx_platform::rx_gate::instance().stop();
			if (result)
				std::cout << "OK\r\n";
			else
			{
				std::cout << "ERROR\r\nError deinitialize rx-platform:\r\n";
				rx_dump_error_result(std::cout, result);
			}
		}
		else
		{
			std::cout << "ERROR\r\nError starting rx-platform:\r\n";
			rx_dump_error_result(std::cout, result);
		}
		std::cout << "De-initializing rx-platform...";
		result = rx_platform::rx_gate::instance().deinitialize();
		if (result)
			std::cout << "OK\r\n";
		else
		{
			std::cout << "ERROR\r\nError deinitialize rx-platform:\r\n";
			rx_dump_error_result(std::cout, result);
		}
	}
	else
	{
		std::cout << "ERROR\r\nError initializing rx-platform:\r\n";
		rx_dump_error_result(std::cout, result);
	}

	HOST_LOG_INFO("Main", 999, "Closing console...");
}


} // namespace pipe
} // namespace host

