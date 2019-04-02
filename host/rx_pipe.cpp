

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


namespace host {

namespace pipe {

// Class host::pipe::rx_pipe_host 

rx_pipe_host::rx_pipe_host (hosting::rx_host_storages& storage)
      : exit_(false)
	, hosting::rx_platform_host(storage)
	, pipe_sender_("Pipe Writer", RX_DOMAIN_EXTERN)
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
		<< "rx-platform Pipe Child Console Host"
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

	cxxopts::Options options("rx-interactive", "");

	intptr_t read_handle;
	intptr_t write_handle;

	options.add_options()
		("input", "Handle of the input pipe for this child process", cxxopts::value<intptr_t>(read_handle))
		("output", "Handle of the output pipe for this child process", cxxopts::value<intptr_t>(write_handle))
		("r,real-time", "Force Real-time priority for process", cxxopts::value<bool>(config.runtime_data.real_time))
		("s,startup", "Startup script", cxxopts::value<string_type>(config.general.startup_script))
		("f,files", "File storage root folder", cxxopts::value<string_type>(config.namespace_data.user_storage_reference))
		("t,test", "Test storage root folder", cxxopts::value<string_type>(config.namespace_data.test_storage_reference))
		("y,system", "System storage root folder", cxxopts::value<string_type>(config.namespace_data.system_storage_reference))
		("n,name", "rx-platform Instance Name", cxxopts::value<string_type>(config.meta_configuration_data.instance_name))
		("l,log-test", "Test log at startup", cxxopts::value<bool>(config.general.test_log))
		("v,version", "Displays platform version")
		("code", "Force building platform system from code builders", cxxopts::value<bool>(config.namespace_data.build_system_from_code))
		("h,help", "Print help")
		;

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
			pipe_client_ = std::make_unique<anonymus_pipe_client>(pipes);


			pipe_sender_.start();

			rx_protocol_client protocol_client;
			protocol_client.reference = this;
			protocol_client.send_function = [] (void* reference, uint8_t* buffer, size_t buffer_size) -> rx_transport_result_t
			{
				rx_pipe_host* self = reinterpret_cast<rx_pipe_host*>(reference);

				byte_string data(buffer, &buffer[buffer_size - 1]);
				self->pipe_sender_.append(
					rx_create_reference<jobs::lambda_job<byte_string> >(
						[&](byte_string data)
						{
							self->pipe_client_->write_pipe(&data[0], data.size());
							std::cout << "Host sent " << (int)data.size() << "bytes\r\n";
						},
						data
					)
				);
				return RX_PROTOCOL_OK;
			};
			protocol_client.collected_callback = [](void* reference, uint8_t* buffer, size_t buffer_size) -> rx_transport_result_t
			{
				rx_pipe_host* self = reinterpret_cast<rx_pipe_host*>(reference);
				
				return RX_PROTOCOL_OK;
			};

			rx_memory_functions memory;
			memory.alloc_buffer_function = rx_c_alloc_buffer;
			memory.alloc_function = rx_c_alloc_buffer;
			memory.free_buffer_function = rx_c_free_buffer;
			memory.free_function = rx_c_free_buffer;

			rx_transport_result_t res = opcua_bin_init_server_transport(&transport_, &protocol_client, 0x10000, 0x10, 1, &memory);
			if (res == RX_PROTOCOL_OK)
			{
				std::cout << "OK\r\n";
				receive_loop(*pipe_client_);

				opcua_bin_deinit_transport(&transport_, &memory);
			}
			else
				std::cout << "ERROR\r\nError initializing transport protocol!\r\n";


			pipe_sender_.end();

			pipe_client_->close_pipe();
			pipe_client_.release();
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

void rx_pipe_host::receive_loop (anonymus_pipe_client& pipe)
{
	rx_result result;

	rx_memory_functions memory;
	memory.alloc_buffer_function = rx_c_alloc_buffer;
	memory.alloc_function = rx_c_alloc_buffer;
	memory.free_buffer_function = rx_c_free_buffer;
	memory.free_function = rx_c_free_buffer;

	while (!exit())
	{
		size_t size;
		void* buffer;
		result = pipe.read_pipe(buffer, size);
		if (!result)
		{
			std::cout << "Error reading pipe, exiting!\r\n";
			HOST_LOG_ERROR("rx_pipe_host", 900, "Error reading pipe, exiting!");
			break;
		}
		std::cout << "Host received " << size << " bytes\r\n";
		auto res = opcua_bin_bytes_received(&transport_, RX_PROTOCOL_OK, (uint8_t*)buffer, size, &memory);
		if (res != RX_PROTOCOL_OK)
		{
			std::cout << "Error code " << res << " returned by transport!\r\n";
			break;
		}
	}
}


// Class host::pipe::anonymus_pipe_client 

anonymus_pipe_client::anonymus_pipe_client (const pipe_client_t& pipes)
      : handles_(pipes)
	, buffer_(RX_PIPE_BUFFER_SIZE)
{
}


anonymus_pipe_client::~anonymus_pipe_client()
{
}



rx_result anonymus_pipe_client::write_pipe (const void* buffer, const size_t size)
{
	auto ret = rx_write_pipe_client(&handles_, buffer, size);
	if (ret == RX_OK)
		return true;
	else
		return "Error writing pipe";
}

rx_result anonymus_pipe_client::read_pipe (void*& buffer, size_t& size)
{
	size = RX_PIPE_BUFFER_SIZE;
	auto ret = rx_read_pipe_client(&handles_, buffer_.buffer(), &size);
	if (ret == RX_OK)
	{
		buffer = buffer_.buffer();
		return true;
	}
	else
		return "Error reading pipe";
}

rx_result anonymus_pipe_client::close_pipe ()
{
	rx_destry_client_side_pipe(&handles_);
	return true;
}


} // namespace pipe
} // namespace host

