

/****************************************************************************
*
*  host\rx_simple_host.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform}
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"

#include "rx_simple_version.h"

// rx_simple_host
#include "host/rx_simple_host.h"

#include "system/hosting/rx_yaml.h"
#include "terminal/rx_terminal_style.h"


namespace host {

namespace simple {

// Class host::simple::simple_platform_host 

simple_platform_host::simple_platform_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
      : exit_(false),
        debug_break_(false)
	, hosting::rx_platform_host(storages)
{
}


simple_platform_host::~simple_platform_host()
{
}



void simple_platform_host::get_host_info (hosts_type& hosts)
{
	hosts.emplace_back(get_simple_info());
	rx_platform_host::get_host_info(hosts);
}

void simple_platform_host::server_started_event ()
{
}

bool simple_platform_host::shutdown (const string_type& msg)
{
	exit_ = true;
	rx_gate::instance().get_host()->break_host("");
	return true;
}

bool simple_platform_host::exit () const
{
	return exit_;
}

bool simple_platform_host::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx)
{
	return true;
}

bool simple_platform_host::break_host (const string_type& msg)
{

	return true;
}

int simple_platform_host::initialize_platform (int argc, char* argv[], log::log_subscriber::smart_ptr log_subscriber, synchronize_callback_t sync_callback, std::vector<library::rx_plugin_base*>& plugins)
{
	rx_thread_data_t tls = rx_alloc_thread_data();

	debug_break_ = false;



	rx_result ret = true;// parse_command_line(argc, argv, config);
	if (ret)
	{
		if (debug_break_)
		{
			std::cout << "Press <ENTER> to continue...\r\n";
			string_type dummy;
			std::getline(std::cin, dummy);
		}
		rx_platform::hosting::simplified_yaml_reader reader;
		std::cout << "Reading configuration file...";
		ret = read_config_file(reader, config_);
		if (ret)
		{
			std::cout << "OK\r\n";
			string_type server_name = get_default_name();



			std::cout << "Initializing OS interface...";
			rx_initialize_os(config_.processor.real_time, tls, server_name.c_str());
			std::cout << "OK\r\n";
			std::cout << "\r\n"
				<< "{rx-platform} "
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
			if (log_subscriber)
				rx::log::log_object::instance().register_subscriber(log_subscriber);
			ret = rx::log::log_object::instance().start(config_.management.test_log);
			if (ret)
			{

				std::cout << "OK\r\n";
				char buff[0x20];
				sprintf(buff, "%d", rx_gate::instance().get_pid());
				HOST_LOG_INFO("Main", 900, "{rx-platform} running on PID "s + buff);

				std::cout << "Registering plug-ins...";
				ret = register_plugins(plugins);
				if (ret)
				{

					std::cout << "OK\r\n";

					std::cout << "Initializing storages...";
					ret = initialize_storages(config_, plugins);
					if (ret)
					{

						std::cout << "OK\r\n";

						HOST_LOG_INFO("Main", 999, "Starting Simple Host...");

						if (!config_.management.telnet_port)// set to the last default if not set
							config_.management.telnet_port = 12345;
						if (config_.processor.io_pool_size <= 0)// has to have at least one
							config_.processor.io_pool_size = 1;
						if (config_.processor.workers_pool_size < 0)
							config_.processor.workers_pool_size = 2;

						// initialize extern ui thread
						thread_synchronizer_.init_callback(sync_callback);
						config_.processor.extern_executer = &thread_synchronizer_;

						ret = register_constructors();
						if (ret)
						{
							HOST_LOG_INFO("Main", 999, "Initializing Rx Engine...");
							std::cout << "Initializing rx-platform...";
							auto result = rx_platform::rx_gate::instance().initialize(this, config_);
							if (result)
							{
								host_security_context_ = result.value();

								std::cout << "OK\r\n";
								return 1;
							}
							else
							{
								std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError initializing rx-platform:\r\n";
								rx_dump_error_result(std::cout, result);
							}
						}
						else
						{
							HOST_LOG_WARNING("Main", 999, "Error registering standard console port constructor:"s + ret.errors()[0]);
						}
					}
					else
					{
						std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError initializing storages\r\n";
						rx_dump_error_result(std::cout, ret);
					}
				}
				else
				{
					std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError registering plug-ins\r\n";
					rx_dump_error_result(std::cout, ret);
				}
				rx::log::log_object::instance().deinitialize();
				
			}

			rx_deinitialize_os();
		}
		else
		{
			std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError reading configuration file\r\n";
			rx_dump_error_result(std::cout, ret);
		}
	}
	thread_synchronizer_.deinit_callback();// just in case
	return 0;
}

string_type simple_platform_host::get_simple_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_SIMPLE_HOST_NAME, RX_SIMPLE_HOST_MAJOR_VERSION, RX_SIMPLE_HOST_MINOR_VERSION, RX_SIMPLE_HOST_BUILD_NUMBER);
	}
	return ret;
}

bool simple_platform_host::is_canceling () const
{
	return false;
}

bool simple_platform_host::read_stdin (std::array<char,0x100>& chars, size_t& count)
{
	return false;
}

bool simple_platform_host::write_stdout (const void* data, size_t size)
{
	return true;
}

int simple_platform_host::deinitialize_platform ()
{

	auto result = remove_simple_thread_security();

	std::cout << "Stopping rx-platform...";
	result = rx_platform::rx_gate::instance().stop();
	if (result)
		std::cout << "OK\r\n";
	else
	{
		std::cout << "ERROR\r\nError stopping rx-platform:\r\n";
		rx_dump_error_result(std::cout, result);
	}
	if (debug_break_)
	{
		std::cout << "Press <ENTER> to continue...\r\n";
		string_type dummy;
		std::getline(std::cin, dummy);
	}
	return 1;
}

rx_result simple_platform_host::set_simple_thread_security ()
{
	return true;// just use the host security context
}

rx_result simple_platform_host::remove_simple_thread_security ()
{
	return true;
}

string_type simple_platform_host::just_parse_command_line (int argc, char* argv[], rx_platform::configuration_data_t& config)
{
	string_type server_name;

	std::cout << "\r\n"
		<< "{rx-platform} Simple Host"
		<< "\r\n======================================\r\n";

	bool ret = parse_command_line(argc, argv, config);
	if (ret)
	{
		rx_platform::hosting::simplified_yaml_reader reader;
		std::cout << "Reading configuration file...";
		ret = read_config_file(reader, config);
		if (ret)
		{
			std::cout << "OK\r\n";
			server_name = get_default_name();
		}
	}
	return server_name;
}

int simple_platform_host::parse_command_line (int argc, char* argv[], rx_platform::configuration_data_t& config)
{
	return true;
}

rx_result simple_platform_host::build_host (hosting::host_platform_builder& builder)
{
	return true;
}

string_type simple_platform_host::get_host_manual () const
{
	return rx_platform_host::get_manual("hosts/rx-simple");
}

int simple_platform_host::start_platform ()
{
	HOST_LOG_INFO("Main", 999, "Starting Rx Engine...");
	std::cout << "Starting rx-platform...";
	auto result = rx_platform::rx_gate::instance().start(this, config_);
	if (result)
	{
		result = set_simple_thread_security();
		if (result)
		{
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

			//if (dump_storage_references_)
			{
				dump_storage_references(std::cout);
				
			}

			std::cout << "\r\nrx-platform Comm Module is running.";
			std::cout << "\r\n=================================\r\n";
			return 1;
		}
		std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError starting rx-platform:\r\n";
		rx_dump_error_result(std::cout, result);
	}
	else
	{
		std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError starting rx-platform:\r\n";
		rx_dump_error_result(std::cout, result);
	}
	std::cout << "De-initializing rx-platform...";
	if(host_security_context_)
		result = rx_platform::rx_gate::instance().deinitialize(host_security_context_);
	if (result)
		std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
	else
	{
		std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError deinitialize rx-platform:\r\n";
		rx_dump_error_result(std::cout, result);
	}
	return 0;
}

int simple_platform_host::stop_platform ()
{
	std::cout << "De-initializing rx-platform...";
	rx_result result;
	if (host_security_context_)
		result = rx_platform::rx_gate::instance().deinitialize(host_security_context_);
	if (result)
		std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
	else
	{
		std::cout << "ERROR\r\nError deinitialize rx-platform:\r\n";
		rx_dump_error_result(std::cout, result);
	}
	thread_synchronizer_.deinit_callback();

	HOST_LOG_INFO("Main", 999, "Console Host exited.");

	deinitialize_storages();
	rx_deinitialize_os();

	return 1;
}

string_type simple_platform_host::get_host_name ()
{
	return RX_SIMPLE_HOST;
}

rx_result simple_platform_host::register_constructors ()
{
	return true;
}


// Class host::simple::simple_security 


// Class host::simple::rx_thread_synchronizer 


void rx_thread_synchronizer::append (jobs::job_ptr pjob)
{
	synchronize_callback_(pjob);
}

void rx_thread_synchronizer::init_callback (synchronize_callback_t callback)
{
}

void rx_thread_synchronizer::deinit_callback ()
{
}


} // namespace simple
} // namespace host

