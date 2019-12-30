

/****************************************************************************
*
*  host\rx_pipe.cpp
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

#include "rx_pipe_version.h"

// rx_pipe
#include "host/rx_pipe.h"

// adding command line parsing library
// see <https://github.com/jarro2783/cxxopts>
#include "third-party/cxxopts/include/cxxopts.hpp"
#include "system/server/rx_server.h"
#include "system/hosting/rx_yaml.h"
#include "sys_internal/rx_internal_protocol.h"
#include "rx_pipe_config.h"
#include "api/rx_meta_api.h"


namespace host {

namespace pipe {

// Class host::pipe::rx_pipe_host 

rx_pipe_host::rx_pipe_host (hosting::rx_host_storages& storage)
      : exit_(false),
        dump_start_log_(false),
        dump_storage_references_(false),
        debug_stop_(false)
	, hosting::rx_platform_host(storage)
	, stdout_log_(rx_create_reference< rx_pipe_stdout_log_subscriber>())
{
	//opcua_bin_init_client_transport(&transport_, 0x1000, 10);
	stdout_log_->show_traces = false;
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

bool rx_pipe_host::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx)
{
	return true;
}

bool rx_pipe_host::break_host (const string_type& msg)
{
	return true;
}

int rx_pipe_host::pipe_main (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins)
{
	
	rx_platform::configuration_data_t config;
	pipe_client_t pipes;
	memzero(&pipes, sizeof(pipes));
	rx_result ret = parse_command_line(argc, argv, config, pipes);
	if (ret)
	{
		if (debug_stop_)
		{
			std::cout << "Press <ENTER> to continue...\r\n";
			string_type dummy;
			std::getline(std::cin, dummy);
		}
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
			rx_initialize_os(config.processor.real_time, tls, server_name.c_str());
			std::cout << "OK\r\n";

			std::cout << "\r\n"
				<< "rx-platform "
				<< rx_gate::instance().get_rx_version()
				<< "\r\n\r\n";
			string_array hosts;
			get_host_info(hosts);
			bool first = true;
			for (const auto& one : hosts)
			{
				std::cout << one;
				if (first)
				{
					std::cout << " [PID:"
						<< rx_pid
						<< "]\r\n";
					first = false;
				}
				else
				{
					std::cout << "\r\n";
				}
			}
			std::cout << "========================================================\r\n\r\n";
			std::cout << "Starting log...";
			rx::log::log_object::instance().register_subscriber(stdout_log_);
			ret = rx::log::log_object::instance().start(config.management.test_log);
			if (ret)
			{
				std::cout << "OK\r\n";
				char buff[0x20];
				sprintf(buff, "%d", rx_gate::instance().get_pid());
				HOST_LOG_INFO("Main", 900, "rx-platform running on PID "s + buff);

				std::cout << "Registering plug-ins...";
				ret = register_plugins(plugins);
				if (ret)
				{
					std::cout << "OK\r\n";
					std::cout << "Initializing storages...";
					ret = initialize_storages(config, plugins);

					if (ret)
					{
						std::cout << "OK\r\n";

						HOST_LOG_INFO("Main", 999, "Starting Console Host...");
						// execute main loop of the console host
						pipe_loop(config, pipes, plugins);
						HOST_LOG_INFO("Main", 999, "Console Host exited.");

						deinitialize_storages();
					}
					else
					{
						std::cout << "ERROR\r\nError initializing storages\r\n";
						rx_dump_error_result(std::cout, ret);
					}
				}
				else
				{
					std::cout << "ERROR\r\nError registering plug-ins\r\n";
					rx_dump_error_result(std::cout, ret);
				}
				rx::log::log_object::instance().deinitialize();
			}

			rx::log::log_object::instance().unregister_subscriber(stdout_log_);

			rx_deinitialize_os();
		}
		else
		{
			std::cout << "ERROR\r\nError reading configuration file\r\n";
			rx_dump_error_result(std::cout, ret);
		}
	}
	std::cout << "\r\n";

	if (debug_stop_)
	{
		std::cout << "Press <ENTER> to continue...\r\n";
		string_type dummy;
		std::getline(std::cin, dummy);
	}
	return ret ? 0 : -1;
}

string_type rx_pipe_host::get_pipe_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_PIPE_HOST_NAME, RX_PIPE_HOST_MAJOR_VERSION, RX_PIPE_HOST_MINOR_VERSION, RX_PIPE_HOST_BUILD_NUMBER);
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

	intptr_t read_handle(0);
	intptr_t write_handle(0);

	options.add_options()
		("input", "Handle of the input pipe for this child process", cxxopts::value<intptr_t>(read_handle))
		("output", "Handle of the output pipe for this child process", cxxopts::value<intptr_t>(write_handle))
		("startlog", "Dump starting log", cxxopts::value<bool>(dump_start_log_))
		("storageref", "Dump storage references", cxxopts::value<bool>(dump_storage_references_))
		("debug", "Wait keyboard hit on start", cxxopts::value<bool>(debug_stop_))
		("trace", "Dump traces in standard output", cxxopts::value<bool>(stdout_log_->show_traces))
		
		;

	add_command_line_options(options, config);

	try
	{
		auto result = options.parse(argc, argv);
		if (result.count("help"))
		{
			// fill paths
			hosting::rx_host_directories host_directories;
			rx_result fill_result = fill_host_directories(host_directories);
			if (!fill_result)
			{
				std::cout << "\r\nERROR\r\n";
			}
			rx_platform_host::print_offline_manual(RX_PIPE_HOST, host_directories);

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
				<< "\r\nUse --help for more datails"
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

void rx_pipe_host::pipe_loop (configuration_data_t& config, const pipe_client_t& pipes, std::vector<library::rx_plugin_base*>& plugins)
{
	rx_platform::hosting::host_security_context::smart_ptr sec_ctx(pointers::_create_new);
	auto result = sec_ctx->login();

	security::security_auto_context dummy(sec_ctx);

	if (!config.management.telnet_port)// set to the last default if not set
		config.management.telnet_port = 12345;
	if (config.processor.genereal_pool_size < 0)
		config.processor.genereal_pool_size = 2;
	if (config.processor.io_pool_size <= 0)// has to have at least one
		config.processor.io_pool_size = 1;
	if (config.processor.workers_pool_size < 0)
		config.processor.workers_pool_size = 2;


	result = rx_platform::register_host_constructor<object_types::port_type>(rx_node_id(RX_LOCAL_PIPE_TYPE_ID, 2), [this] {
		return pipe_port_;
		});
	if (!result)
		HOST_LOG_WARNING("Main", 999, "Error registering local pipe port constructor:"s + result.errors()[0]);

	pipe_port_ = rx_create_reference<local_pipe_port>(pipes);

	HOST_LOG_INFO("Main", 999, "Initializing Rx Engine...");
	std::cout << "Initializing rx-platform...";
	result = rx_platform::rx_gate::instance().initialize(this, config);
	if (result)
	{
		auto json = rx_create_reference< sys_internal::rx_protocol::rx_protocol_port>();


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
				result = pipe_port_->open();
				if (result)
				{
					res = rx_push_stack(pipe_port_->get_stack_entry(), &transport_.protocol_stack_entry);

					
					res = rx_push_stack(&transport_.protocol_stack_entry, json->get_stack_entry());

					std::cout << "OK\r\n";

					if (dump_storage_references_)
					{
						string_type sys_info = get_storages().system_storage->get_storage_info();
						string_type sys_ref = get_storages().system_storage->get_storage_reference();
						string_type user_info = get_storages().user_storage->get_storage_info();
						string_type user_ref = get_storages().user_storage->get_storage_reference();

						std::cout << "\r\nStorage Information:\r\n============================\r\n";
						std::cout << "System Storage: " << sys_info << "\r\n";
						std::cout << "System Reference: " << sys_ref << "\r\n";
						std::cout << "User Storage: " << user_info << "\r\n";
						std::cout << "User Reference: " << user_ref << "\r\n";
						if (get_storages().test_storage)
						{
							string_type test_info = get_storages().test_storage->get_storage_info();
							string_type test_ref = get_storages().test_storage->get_storage_reference();
							std::cout << "Test Storage: " << test_info << "\r\n";
							std::cout << "Test Reference: " << test_ref << "\r\n";
						}
					}
					if(dump_start_log_)
						std::cout << "\r\nStartup log:\r\n============================\r\n";
					stdout_log_->release_log(dump_start_log_);

					auto user = security::active_security()->get_full_name();

					std::cout << "\r\nEnternig loop as "
						<< user
						<< "\r\n.";
					for (auto i = 0; i < 29; i++)
					{
						std::cout << ".";
						rx_ms_sleep(15);
					}
					std::cout << ".\r\n";

					std::cout << ">";


					pipe_port_->receive_loop();

					stdout_log_->suspend_log();

					std::cout << "Exited loop....\r\n";

					pipe_port_->close();

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

rx_result rx_pipe_host::build_host (rx_directory_ptr root)
{
	return true;
}

string_type rx_pipe_host::get_host_manual () const
{
	return rx_platform_host::get_manual("hosts/" RX_PIPE_HOST);
}

string_type rx_pipe_host::get_host_name ()
{
	return RX_PIPE_HOST;
}

rx_result rx_pipe_host::register_hosts ()
{
	auto result = rx_gate::instance().register_constructor<meta::object_types::port_type>(
		rx_node_id(RX_LOCAL_PIPE_TYPE_ID, 2), [] {
			return rx_create_reference<sys_internal::rx_protocol::rx_protocol_port>();
		});
	return result;
}


// Class host::pipe::rx_pipe_stdout_log_subscriber 

rx_pipe_stdout_log_subscriber::rx_pipe_stdout_log_subscriber()
      : running_(false),
        show_traces(false)
{
}



void rx_pipe_stdout_log_subscriber::log_event (log::log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& code, const string_type& message, rx_time when)
{
	log::log_event_data one = { event_type,library,source,level,code,message,when };
	if (!show_traces && event_type == log::log_event_type::trace)
		return;
	if (running_)
	{
		one.dump_to_stream_simple(std::cout);	}
	else
	{
		locks::auto_slim_lock _(&pending_lock_);
		pending_events_.emplace_back(std::move(one));
	}
}

void rx_pipe_stdout_log_subscriber::release_log (bool dump_previous)
{
	running_ = true;
	if (dump_previous)
	{
		locks::auto_slim_lock _(&pending_lock_);
		for (const auto& one : pending_events_)
		{
			one.dump_to_stream_simple(std::cout);
		}
		pending_events_.clear();
	}
}

void rx_pipe_stdout_log_subscriber::suspend_log ()
{
	running_ = false;
}


} // namespace pipe
} // namespace host

