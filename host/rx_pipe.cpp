

/****************************************************************************
*
*  host\rx_pipe.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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

#include "rx_pipe_version.h"

// rx_pipe
#include "host/rx_pipe.h"

// adding command line parsing library
// see <https://github.com/jarro2783/cxxopts>
#include "third-party/cxxopts/include/cxxopts.hpp"
#include "system/server/rx_server.h"
#include "system/hosting/rx_yaml.h"
#include "rx_pipe_config.h"
#include "api/rx_meta_api.h"
#include "terminal/rx_terminal_style.h"
#include "sys_internal/rx_security/rx_platform_security.h"
#include "interfaces/rx_io.h"
#include "system/server/rx_directory_cache.h"


namespace host {

namespace pipe {
namespace
{
// helper data for pipe host and command line options
const intptr_t c_invalid_handle = (intptr_t)(-1);

intptr_t read_handle(c_invalid_handle);
intptr_t write_handle(c_invalid_handle);
bool use_std = false;

bool do_trace_level = false;
bool do_info_level = false;
bool do_warning_level = false;
bool do_error_level = false;


pipe_client_t pipes;

}

// Class host::pipe::rx_pipe_host 

rx_pipe_host::rx_pipe_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
      : exit_(false),
        dump_start_log_(false),
        dump_storage_references_(false),
        debug_stop_(false)
	, hosting::rx_platform_host(storages)
	, stdout_log_(rx_create_reference< rx_pipe_stdout_log_subscriber>())
{
	//opcua_bin_init_client_transport(&transport_, 0x1000, 10);
	RX_ASSERT(stdout_log_->log_query == log::rx_log_normal_level);// just in case, i got a hunch ;)
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
	rx_thread_data_t tls = rx_alloc_thread_data();

	rx_result ret = setup_console(argc, argv);
	stdout_log_->set_supports_ansi(supports_ansi());

	rx_platform::configuration_data_t config;
	memzero(&pipes, sizeof(pipes));
	std::cout << "Parsing command line...";
	ret = parse_command_line(argc, argv, "rx-pipe", config);
	if (ret)
	{
		std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

		if (config.management.debug)
			stdout_log_->log_query = log::rx_log_debug_level;
		else if (do_trace_level)
			stdout_log_->log_query = log::rx_log_trace_level;
		else if (do_info_level)
			stdout_log_->log_query = log::rx_log_normal_level;
		else if (do_warning_level)
			stdout_log_->log_query = log::rx_log_warining_level;
		else if (do_error_level)
			stdout_log_->log_query = log::rx_log_error_level;
		else
			stdout_log_->log_query = log::rx_log_normal_level;

		if (use_std)
		{
			sys_handle_t in, out, err;
			get_stdio_handles(in, out, err);
			read_handle = (intptr_t)in;
			write_handle = (intptr_t)err;
		}
		if (read_handle == c_invalid_handle || write_handle == c_invalid_handle)
		{
			std::cout << "\r\nThis is a child process and I/O handles have to be supplied"
				<< "\r\nUse --input and --output or --std options to specify handles."
				<< "\r\nUse --help for more details"
				<< "\r\n\r\nExiting...\r\n";

			restore_console();

			return false;
		}
		pipes.client_read = (sys_handle_t)read_handle;
		pipes.client_write = (sys_handle_t)write_handle;

		if (debug_stop_)
		{
			std::cout << "Press <ENTER> to continue...\r\n";
			string_type dummy;
			std::getline(std::cin, dummy);
		}
		std::cout << "Reading configuration file...";
		ret = parse_config_files(config);
		if (ret)
		{
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

			//config.namespace_data.build_system_from_code = true;
			
			std::cout << "Initializing OS interface...";
			rx_initialize_os(config.processor.real_time, !config.processor.no_hd_timer, tls, config.management.debug);
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

			
			std::cout << "\r\n";
			if (supports_ansi())
				std::cout << ANSI_COLOR_GREEN ANSI_COLOR_BOLD;
			std::cout << "{rx-platform} "
				<< rx_gate::instance().get_rx_version();
			if (supports_ansi())
				std::cout << ANSI_COLOR_RESET;
			std::cout << "\r\n\r\n";

			std::cout << "Instance Name:";
			if (supports_ansi())
				std::cout << ANSI_COLOR_GREEN ANSI_COLOR_BOLD;
			std::cout << config.instance.name << "\r\n";
			if (supports_ansi())
				std::cout << ANSI_COLOR_RESET;
			std::cout << "Node Name:";
			if (supports_ansi())
				std::cout << ANSI_COLOR_GREEN ANSI_COLOR_BOLD;
			std::cout << rx_get_node_name() << "\r\n\r\n";
			if (supports_ansi())
				std::cout << ANSI_COLOR_RESET;

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
			rx_platform::log::log_object::instance().register_subscriber(stdout_log_);
			ret = start_log(config);
			if (ret)
			{
				std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
				std::cout << "Initializing security...";
				ret = rx_internal::rx_security::platform_security::instance().initialize(this, config);
				if (ret)
				{
					char buff[0x20];
					sprintf(buff, "%d", rx_gate::instance().get_pid());
					HOST_LOG_INFO("Main", 900, "{rx-platform} running on PID "s + buff);

					std::cout << "Registering plug-ins...";
					ret = register_plugins(plugins);
					if (ret)
					{
						std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
						std::cout << "Initializing storages...";
						ret = initialize_storages(config, plugins);

						if (ret)
						{
							std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

							HOST_LOG_INFO("Main", 999, "Starting Console Host...");
							// execute main loop of the console host
							pipe_loop(config, pipes, plugins);
							HOST_LOG_INFO("Main", 999, "Console Host exited.");

							deinitialize_storages();
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
					rx_internal::rx_security::platform_security::instance().deinitialize();

				}
				else
				{
					std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError initializing security:\r\n";
					rx_dump_error_result(std::cout, ret);
				}
				rx_platform::log::log_object::instance().deinitialize();
			}

			rx_platform::log::log_object::instance().unregister_subscriber(stdout_log_);

			rx_deinitialize_os();
		}
		else
		{
			std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError reading configuration file\r\n";
			rx_dump_error_result(std::cout, ret);
		}
	}
	std::cout << "\r\n";
	rx_internal::interfaces::io_endpoints::dispatcher_subscriber::deinitialize();
	rx_platform::threads::thread::deinitialize();
	restore_console();

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
	static char ret[0x60] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_PIPE_HOST_NAME, RX_PIPE_HOST_MAJOR_VERSION, RX_PIPE_HOST_MINOR_VERSION, RX_PIPE_HOST_BUILD_NUMBER);
	}
	return ret;
}

bool rx_pipe_host::is_canceling () const
{
	return false;
}

void rx_pipe_host::pipe_loop (configuration_data_t& config, const pipe_client_t& pipes, std::vector<library::rx_plugin_base*>& plugins)
{

	if (!config.management.telnet_port)// set to the last default if not set
		config.management.telnet_port = 12345;
	if (config.processor.io_pool_size <= 0)// has to have at least one
		config.processor.io_pool_size = 1;
	if (config.processor.workers_pool_size < 0)
		config.processor.workers_pool_size = 2;


	rx_result result = rx_platform::register_host_constructor<object_types::port_type>(rx_node_id(RX_LOCAL_PIPE_TYPE_ID, 2), [this] {
		return pipe_port_;
		});
	if (!result)
		HOST_LOG_WARNING("Main", 999, "Error registering local pipe port constructor:"s + result.errors()[0]);

	pipe_port_ = rx_create_reference<local_pipe_port>(pipes, this);

	HOST_LOG_INFO("Main", 999, "Initializing Rx Engine...");
	std::cout << "Initializing {rx-platform}...";
	auto sec_result = rx_platform::rx_gate::instance().initialize(this, config);
	if (sec_result)
	{
		std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
		HOST_LOG_INFO("Main", 999, "Starting Rx Engine...");
		std::cout << "Starting {rx-platform} ...";
		result = rx_platform::rx_gate::instance().start(this, config);
		if (result)
		{
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
			std::cout << "Starting pipe communication...";

			result = pipe_port_->receive_loop(this);
			if (!result)
			{
				std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError running Local Pipe:\r\n";
				rx_dump_error_result(std::cout, result);
			}
			stdout_log_->suspend_log();

			std::cout << "Stopping {rx-platform} ...";
			result = rx_platform::rx_gate::instance().stop();
			if (result)
				std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
			else
			{
				std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError deinitialize {rx-platform}:\r\n";
				rx_dump_error_result(std::cout, result);
			}
		}
		else
		{
			std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError starting {rx-platform}:\r\n";
			rx_dump_error_result(std::cout, result);
		}
		std::cout << "De-initializing {rx-platform} ...";
		result = rx_platform::rx_gate::instance().deinitialize();
		if (result)
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
		else
		{
			std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError deinitialize {rx-platform}:\r\n";
			rx_dump_error_result(std::cout, result);
		}
	}
	else
	{
		std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError initializing {rx-platform}:\r\n";
		rx_dump_error_result(std::cout, sec_result);
	}
	HOST_LOG_INFO("Main", 999, "Closing console...");
}

rx_result rx_pipe_host::build_host (hosting::host_platform_builder& builder)
{
#ifndef RX_MIN_MEMORY

	auto dir_result = rx_gate::instance().add_directory(builder.host_root + RX_DIR_DELIMETER_STR "types");

	auto detail_struct_type = rx_create_reference<meta::basic_types::struct_type>();
	detail_struct_type->meta_info.name = RX_LOCAL_PIPE_DETAILS_TYPE_NAME;
	detail_struct_type->meta_info.id = rx_node_id(RX_LOCAL_PIPE_DETAILS_TYPE_ID, 2);
	detail_struct_type->meta_info.parent = rx_node_id(RX_CLASS_STRUCT_BASE_ID);
	detail_struct_type->meta_info.path = "/sys/host/types";
	detail_struct_type->meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	detail_struct_type->complex_data.register_const_value_static<int64_t>("InPipe", -1);
	detail_struct_type->complex_data.register_const_value_static<int64_t>("OutPipe", -1);

	auto result = register_host_simple_type<meta::basic_types::struct_type>(detail_struct_type);
	if (!result)
	{
		result.register_error("Unable to register " RX_LOCAL_PIPE_DETAILS_TYPE_NAME " struct type.");
		return result;
	}

	rx_port_type_ptr interactive_port_type = rx_create_reference<meta::object_types::port_type>();
	interactive_port_type->meta_info.name = RX_LOCAL_PIPE_TYPE_NAME;
	interactive_port_type->meta_info.id = rx_node_id(RX_LOCAL_PIPE_TYPE_ID, 2);
	interactive_port_type->meta_info.parent = rx_node_id(RX_EXTERNAL_PORT_TYPE_ID);
	interactive_port_type->meta_info.path = "/sys/host/types";
	interactive_port_type->meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	interactive_port_type->complex_data.register_struct("Pipe", rx_node_id(RX_LOCAL_PIPE_DETAILS_TYPE_ID, 2));

	result = register_host_type<meta::object_types::port_type>(interactive_port_type);
	if (!result)
	{
		result.register_error("Unable to register " RX_LOCAL_PIPE_TYPE_NAME " port type.");
		return result;
	}


	meta::runtime_data::application_runtime_data app_inst_data;
	app_inst_data.meta_info.name = RX_HOST_APP_NAME;
	app_inst_data.meta_info.id = rx_node_id(RX_HOST_APP_ID);
	app_inst_data.meta_info.parent = rx_node_id(RX_HOST_APP_TYPE_ID);
	app_inst_data.meta_info.path = "/sys/host";
	app_inst_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	app_inst_data.instance_data.priority = rx_domain_priority::normal;
	app_inst_data.instance_data.processor = 0;

	result = register_host_runtime<meta::object_types::application_type>(app_inst_data, nullptr);
	if (!result)
	{
		result.register_error("Unable to register " RX_HOST_APP_NAME " application runtime.");
		return result;
	}


	meta::runtime_data::domain_runtime_data domain_inst_data;
	domain_inst_data.meta_info.name = RX_HOST_DOMAIN_NAME;
	domain_inst_data.meta_info.id = rx_node_id(RX_HOST_DOMAIN_ID);
	domain_inst_data.meta_info.parent = rx_node_id(RX_HOST_DOMAIN_TYPE_ID);
	domain_inst_data.meta_info.path = "/sys/host";
	domain_inst_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	domain_inst_data.instance_data.app_ref = rx_node_id(RX_HOST_APP_ID);
	domain_inst_data.instance_data.priority = rx_domain_priority::normal;
	domain_inst_data.instance_data.processor = -1;

	result = register_host_runtime<meta::object_types::domain_type>(domain_inst_data, nullptr);
	if (!result)
	{
		result.register_error("Unable to register " RX_HOST_DOMAIN_NAME " domain runtime.");
		return result;
	}

	runtime_data::object_runtime_data instance_data;
	instance_data = runtime_data::object_runtime_data();
	instance_data.meta_info.name = RX_HOST_OBJECT_NAME;
	instance_data.meta_info.id = RX_HOST_OBJ_ID;
	instance_data.meta_info.parent = RX_NS_HOST_TYPE_ID;
	instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
	instance_data.meta_info.path = "/sys/host";
	instance_data.instance_data.domain_ref = rx_node_id(RX_HOST_DOMAIN_ID);
	result = register_host_runtime<meta::object_types::object_type>(instance_data, nullptr);
	if (!result)
	{
		result.register_error("Unable to register " RX_HOST_OBJECT_NAME " application runtime.");
		return result;
	}


	meta::runtime_data::port_runtime_data inst_data;
	inst_data.meta_info.name = RX_LOCAL_PIPE_NAME;
	inst_data.meta_info.id = rx_node_id(RX_LOCAL_PIPE_ID, 2);
	inst_data.meta_info.parent = rx_node_id(RX_LOCAL_PIPE_TYPE_ID, 2);
	inst_data.meta_info.path = "/sys/host";
	inst_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	inst_data.instance_data.app_ref = rx_node_id(RX_HOST_APP_ID);

	result = register_host_runtime<meta::object_types::port_type>(inst_data, nullptr);
	if (!result)
	{
		result.register_error("Unable to register " RX_LOCAL_PIPE_NAME " port runtime.");
		return result;
	}

	inst_data = meta::runtime_data::port_runtime_data();
	inst_data.meta_info.name = RX_LOCAL_PIPE_TRANSPORT_NAME;
	inst_data.meta_info.id = rx_node_id(RX_LOCAL_PIPE_TRANSPORT_ID, 2);
	inst_data.meta_info.parent = rx_node_id(RX_OPCUA_TRANSPORT_PORT_TYPE_ID);
	inst_data.meta_info.path = "/sys/host";
	inst_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	inst_data.instance_data.app_ref = rx_node_id(RX_HOST_APP_ID);

	inst_data.overrides.add_value_static("StackTop", RX_LOCAL_PIPE_NAME);

	result = register_host_runtime<meta::object_types::port_type>(inst_data, nullptr);
	if (!result)
	{
		result.register_error("Unable to register " RX_LOCAL_PIPE_TRANSPORT_NAME " port runtime.");
		return result;
	}

	inst_data = meta::runtime_data::port_runtime_data();
	inst_data.meta_info.name = RX_LOCAL_PIPE_CHANNEL_NAME;
	inst_data.meta_info.id = rx_node_id(RX_LOCAL_PIPE_CHANNEL_ID, 2);
	inst_data.meta_info.parent = rx_node_id(RX_OPCUA_SEC_NONE_PORT_TYPE_ID);
	inst_data.meta_info.path = "/sys/host";
	inst_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	inst_data.instance_data.app_ref = rx_node_id(RX_HOST_APP_ID);

	inst_data.overrides.add_value_static("StackTop", RX_LOCAL_PIPE_TRANSPORT_NAME);

	result = register_host_runtime<meta::object_types::port_type>(inst_data, nullptr);
	if (!result)
	{
		result.register_error("Unable to register " RX_LOCAL_PIPE_CHANNEL_NAME " port runtime.");
		return result;
	}

	inst_data = meta::runtime_data::port_runtime_data();
	inst_data.meta_info.name = RX_LOCAL_PIPE_PROTOCOL_NAME;
	inst_data.meta_info.id = rx_node_id(RX_LOCAL_PIPE_PROTOCOL_ID, 2);
	inst_data.meta_info.parent = rx_node_id(RX_RX_JSON_TYPE_ID);
	inst_data.meta_info.path = "/sys/host";
	inst_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	inst_data.instance_data.app_ref = rx_node_id(RX_HOST_APP_ID);

	inst_data.overrides.add_value_static("StackTop", RX_LOCAL_PIPE_CHANNEL_NAME);

	result = register_host_runtime<meta::object_types::port_type>(inst_data, nullptr);
	if (!result)
	{
		result.register_error("Unable to register " RX_LOCAL_PIPE_PROTOCOL_NAME " port runtime.");
		return result;
	}

	return true;
#else
	return "Not implemented for small memory footprint";
#endif
}

string_type rx_pipe_host::get_host_manual () const
{
	return rx_platform_host::get_manual("hosts/" RX_PIPE_HOST);
}

string_type rx_pipe_host::get_host_name ()
{
	return RX_PIPE_HOST;
}

void rx_pipe_host::restore_console ()
{
}

rx_result rx_pipe_host::setup_console (int argc, char* argv[])
{
	return true;
}

void rx_pipe_host::pipe_run_result (rx_result result)
{
	if (result)
	{
		std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

		if (dump_storage_references_)
		{
			dump_storage_references(std::cout);
		}
		std::cout << "\r\n";
		if (dump_start_log_)
			std::cout << "\r\nStartup log START"
			<< "\r\n=============================================\r\n";
		stdout_log_->release_log(dump_start_log_);
		if (dump_start_log_)
			std::cout
			<< "=============================================\r\n"
			<< "Startup log END\r\n\r\n";
	}
	else
	{
		std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError running Local Pipe:\r\n";
		rx_dump_error_result(std::cout, result);
		std::cout << "\r\n";
		stdout_log_->release_log(true);
		std::cout << "\r\n";
	}
	host_started();
}

void rx_pipe_host::add_command_line_options (hosting::command_line_options_t& options, rx_platform::configuration_data_t& config)
{
	rx_platform_host::add_command_line_options(options, config);

	options.add_options()
		("i,input", "Handle of the input pipe for this child process", cxxopts::value<intptr_t>(read_handle))
		("o,output", "Handle of the output pipe for this child process", cxxopts::value<intptr_t>(write_handle))
		("std", "Use stdin and stderr for communication", cxxopts::value<bool>(use_std))
		("startlog", "Dump starting log", cxxopts::value<bool>(dump_start_log_))
		("storageref", "Dump storage references", cxxopts::value<bool>(dump_storage_references_))
		("wait", "Wait interactive on process startup and exit", cxxopts::value<bool>(debug_stop_))
		("t,trace", "Show trace level content all but debug events are listed to standard output", cxxopts::value<bool>(do_trace_level))
		("info", "Show info level content all but debug and trace events are listed to standard output", cxxopts::value<bool>(do_info_level))
		("w,warning", "Show warning level content only warning error and critical events are listed to standard output", cxxopts::value<bool>(do_warning_level))
		("e,error", "Show error level content only error and critical events are listed  to standard output", cxxopts::value<bool>(do_error_level))
		;
}

void rx_pipe_host::read_config_options (const std::map<string_type, string_type>& options, rx_platform::configuration_data_t& config)
{
	rx_platform_host::read_config_options(options, config);
}

const char* get_log_type_string(log::log_event_type type)
{
	switch (type)
	{
	case log::log_event_type::info:
		return ANSI_RX_LOG_INFO		" INFO     " ANSI_COLOR_RESET;
	case log::log_event_type::warning:
		return ANSI_RX_LOG_WARNING	" WARNING  " ANSI_COLOR_RESET;
	case log::log_event_type::error:
		return ANSI_RX_LOG_ERROR	" ERROR    " ANSI_COLOR_RESET;
	case log::log_event_type::critical:
		return ANSI_RX_LOG_CRITICAL	" CRITICAL " ANSI_COLOR_RESET;
	case log::log_event_type::debug:
		return ANSI_RX_LOG_DEBUG	" DEBUG    " ANSI_COLOR_RESET;
	case log::log_event_type::trace:
		return ANSI_RX_LOG_TRACE	" TRACE    " ANSI_COLOR_RESET;
	default:
		return "***UNKNOWN***";
	}
}
// Class host::pipe::rx_pipe_stdout_log_subscriber 

rx_pipe_stdout_log_subscriber::rx_pipe_stdout_log_subscriber (bool supports_ansi)
      : running_(false),
        supports_ansi_(false),
        log_query(log::rx_log_normal_level)
{
}



void rx_pipe_stdout_log_subscriber::log_event (log::log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when)
{
	log::log_event_data one = { event_type,library,source,level,code,message,when, user };
	log::log_query_type query;
	query.type = log_query;
	if (!one.is_included(query))
		return;
	if (running_)
	{
		if (supports_ansi_)
		{
			std::cout << one.when.get_string(false)
				<< get_log_type_string(one.event_type)
				<< one.message << "\r\n";
		}
		else
		{
			one.dump_to_stream_simple(std::cout);
		}
	}
	else
	{
		locks::auto_lock_t _(&pending_lock_);
		pending_events_.emplace_back(std::move(one));
	}
}

void rx_pipe_stdout_log_subscriber::release_log (bool dump_previous)
{
	if (dump_previous)
	{

		locks::auto_lock_t _(&pending_lock_);
		for (const auto& one : pending_events_)
		{
			if (supports_ansi_)
			{
				std::cout << one.when.get_string(false)
					<< get_log_type_string(one.event_type)
					<< one.message << "\r\n";
			}
			else
			{
				one.dump_to_stream_simple(std::cout);
			}
		}
		pending_events_.clear();
	}
	running_ = true;	
}

void rx_pipe_stdout_log_subscriber::suspend_log ()
{
	running_ = false;
}

rx_result rx_pipe_stdout_log_subscriber::read_log (const log::log_query_type& query, log::log_events_type& result)
{
	return RX_NOT_SUPPORTED;
}

string_type rx_pipe_stdout_log_subscriber::get_name () const
{
	return "stdout";
}


} // namespace pipe
} // namespace host

