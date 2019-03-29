

/****************************************************************************
*
*  host\rx_gui_host.cpp
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

#include "rx_gui_version.h"
#include "terminal/rx_terminal_style.h"

// rx_gui_host
#include "host/rx_gui_host.h"

#include "system/hosting/rx_yaml.h"


namespace host {

namespace gui {

// Class host::gui::gui_platform_host 

gui_platform_host::gui_platform_host (hosting::rx_host_storages& storage)
      : exit_(false)
	, hosting::rx_platform_host(storage)
{
}


gui_platform_host::~gui_platform_host()
{
}



void gui_platform_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_gui_info());
	rx_platform_host::get_host_info(hosts);
}

void gui_platform_host::server_started_event ()
{
}

bool gui_platform_host::shutdown (const string_type& msg)
{
	exit_ = true;
	rx_gate::instance().get_host()->break_host("");
	return true;
}

bool gui_platform_host::exit () const
{
	return exit_;
}

void gui_platform_host::get_host_objects (std::vector<rx_platform::runtime::object_runtime_ptr>& items)
{
}

void gui_platform_host::get_host_types (std::vector<rx_platform::meta::object_type_ptr>& items)
{
}

bool gui_platform_host::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx)
{
	return true;
}

bool gui_platform_host::break_host (const string_type& msg)
{

	return true;
}

int gui_platform_host::gui_initialize (int argc, char* argv[], log::log_subscriber::smart_ptr log_subscriber, synchronize_callback_t sync_callback)
{
	std::cout << "\r\n"
		<< ANSI_COLOR_GREEN ANSI_COLOR_BOLD "rx-platform" ANSI_COLOR_RESET " Interactive Console Host"
		<< "\r\n======================================\r\n";

	rx_platform::configuration_data_t config;
	bool ret = true;// parse_command_line(argc, argv, config);
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
			if (log_subscriber)
				rx::log::log_object::instance().register_subscriber(log_subscriber);
			ret = rx::log::log_object::instance().start(config.general.test_log);
			if (ret)
			{
				std::cout << ANSI_STATUS_OK "\r\n";

				std::cout << "Initializing storages...";
				ret = initialize_storages(config);
				if (ret)
				{
					std::cout << ANSI_STATUS_OK "\r\n";

					HOST_LOG_INFO("Main", 999, "Starting GUI Host...");


					host_security_context_ = rx_create_reference<rx_platform::hosting::host_security_context>();
					host_security_context_->login();

					host_security_context_->impersonate();

					if (!config.managment_data.telnet_port)// set to the last default if not set
						config.managment_data.telnet_port = 12345;
					if (config.runtime_data.genereal_pool_size < 0)
						config.runtime_data.genereal_pool_size = 2;
					if (config.runtime_data.io_pool_size <= 0)// has to have at least one
						config.runtime_data.io_pool_size = 1;
					if (config.runtime_data.workers_pool_size < 0)
						config.runtime_data.workers_pool_size = 2;

					// init exter ui thread
					thread_synchronizer_.init_callback(sync_callback);
					config.runtime_data.extern_executer = &thread_synchronizer_;


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
							set_gui_thread_security();
							return 1;
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
	}
	thread_synchronizer_.deinit_callback();// just in case
	return 0;
}

string_type gui_platform_host::get_gui_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_HOST_NAME, RX_HOST_MAJOR_VERSION, RX_HOST_MINOR_VERSION, RX_HOST_BUILD_NUMBER);
	}
	return ret;
}

bool gui_platform_host::is_canceling () const
{
	return false;
}

bool gui_platform_host::read_stdin (std::array<char,0x100>& chars, size_t& count)
{
	return false;
}

bool gui_platform_host::write_stdout (const void* data, size_t size)
{
	return true;
}

int gui_platform_host::gui_deinitialize ()
{

	auto result = remove_gui_thread_security();

	std::cout << "Stopping rx-platform...";
	result = rx_platform::rx_gate::instance().stop();
	if (result)
		std::cout << ANSI_STATUS_OK "\r\n";
	else
	{
		std::cout << ANSI_STATUS_ERROR "\r\nError stopping rx-platform:\r\n";
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
	thread_synchronizer_.deinit_callback();

	HOST_LOG_INFO("Main", 999, "Console Host exited.");

	host_security_context_->revert();

	deinitialize_storages();
	rx_deinitialize_os();

	return 1;
}

rx_result gui_platform_host::gui_loop ()
{
	return true;
}

rx_result gui_platform_host::set_gui_thread_security ()
{
	RX_ASSERT(!user_security_context_);
	user_security_context_ = rx_create_reference<interactive::interactive_security_context>();
	auto result = user_security_context_->login();
	if(result)
		result = user_security_context_->impersonate();
	return result;
}

rx_result gui_platform_host::remove_gui_thread_security ()
{
	RX_ASSERT(user_security_context_);
	user_security_context_->revert();
	user_security_context_->logout();
	return true;
}


// Class host::gui::gui_thread_synchronizer 


void gui_thread_synchronizer::append (jobs::job_ptr pjob)
{
	synchronize_callback_(pjob);
}

void gui_thread_synchronizer::init_callback (synchronize_callback_t callback)
{
}

void gui_thread_synchronizer::deinit_callback ()
{
}


} // namespace gui
} // namespace host

