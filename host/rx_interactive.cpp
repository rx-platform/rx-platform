

/****************************************************************************
*
*  host\rx_interactive.cpp
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

// adding command line parsing library
// see <https://github.com/jarro2783/cxxopts>
#include "third-party/cxxopts/include/cxxopts.hpp"
#include "rx_interactive_version.h"
#include "rx_interactive_config.h"
#include "terminal/rx_terminal_style.h"
#include "system/hosting/rx_yaml.h"
#include "sys_internal/rx_security/rx_platform_security.h"
#include "sys_internal/rx_plugin_manager.h"
#include "interfaces/rx_io.h"
#include "system/server/rx_directory_cache.h"

// rx_interactive
#include "host/rx_interactive.h"

#include "api/rx_meta_api.h"

//#define INTERACTIVE_HOST_INFO "Interactive Console Host Ver 0.9.2"


namespace host {

namespace interactive {

// Class host::interactive::interactive_console_host 

interactive_console_host::interactive_console_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
      : exit_(false)
	, hosting::rx_platform_host(storages)
{
	//startup_script_ = "test run meta/construct-wide\n";
	//startup_script_ = "test run -a\n";
}


interactive_console_host::~interactive_console_host()
{
}



rx_result interactive_console_host::console_loop (configuration_data_t& config, std::vector<library::rx_plugin_base*>& plugins)
{

	if (!config.management.telnet_port)// set to the last default if not set
		config.management.telnet_port = 12345;
	if (config.processor.io_pool_size <= 0)// has to have at least one
		config.processor.io_pool_size = 1;
	if (config.processor.workers_pool_size < 0)
		config.processor.workers_pool_size = 2;

	auto result = rx_platform::register_host_constructor<object_types::port_type>(rx_node_id(RX_STD_IO_TYPE_ID, 3), [this] {
		return interactive_port_;
		});
	if (!result)
		HOST_LOG_WARNING("Main", 999, "Error registering standard console port constructor:"s + result.errors()[0]);

	interactive_port_ = rx_create_reference<interactive_console_port>(this);

	HOST_LOG_INFO("Main", 999, "Initializing Rx Engine...");
	std::cout << "Initializing {rx-platform}...";
	auto init_result = rx_platform::rx_gate::instance().initialize(this, config);
	if (init_result)
	{
		std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
		HOST_LOG_INFO("Main", 999, "Starting Rx Engine...");
		std::cout << "Starting {rx-platform}...";
		result = rx_platform::rx_gate::instance().start(this, config);
		if (result)
		{
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

			std::cout << "Starting interactive console...";

			result = interactive_port_->run_interactive(this);
			if (!result)
			{
				std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError running Interactive Console:\r\n";
				rx_dump_error_result(std::cout, result);
			}
			std::cout << "Stopping {rx-platform}...";
			result = rx_platform::rx_gate::instance().stop();
			if(result)
				std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
			else
			{
				std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError stopping {rx-platform}:\r\n";
				rx_dump_error_result(std::cout, result);
			}
		}
		else
		{
			std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError starting {rx-platform}:\r\n";
			rx_dump_error_result(std::cout, result);
		}
		std::cout << "De-initializing {rx-platform}...";
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
		rx_dump_error_result(std::cout, init_result);
		result = init_result.errors();
	}

	HOST_LOG_INFO("Main", 999, "Closing console...");

	return result;
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
	std::ostringstream ss;
	ss << "\r\n" ANSI_COLOR_RED "SHUTDOWN" ANSI_COLOR_RESET " initiated by " ANSI_COLOR_GREEN << ctx->get_full_name();
	ss << ANSI_COLOR_RESET "\r\n";
	ss << "Message:" << msg << "\r\n";
	write_stdout(ss.str().c_str());
	exit_ = true;
	rx_gate::instance().get_host()->break_host("");
	return true;
}

bool interactive_console_host::exit () const
{
	return exit_;
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
				//if (buffer->fill_with_file_content(file))
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
				//else
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

int interactive_console_host::console_main (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins)
{
	rx_thread_data_t tls = rx_alloc_thread_data();

	rx_result ret = setup_console(argc, argv);

	rx_platform::configuration_data_t config;
	std::cout << "Parsing command line...";
	ret = parse_command_line(argc, argv, "rx-interactive", config);
	if (ret)
	{
		std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
		std::cout << "Reading configuration file...";
		ret = parse_config_files(config);
		if (ret)
		{
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
			if (config.meta_configuration.instance_name.empty())
				config.meta_configuration.instance_name = "develop";

			std::cout << "Initializing OS interface...";
			rx_initialize_os(config.processor.real_time, !config.processor.no_hd_timer, tls, config.management.debug);
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

			std::cout << "\r\n";
			if(supports_ansi())
				std::cout << ANSI_COLOR_GREEN ANSI_COLOR_BOLD;
			std::cout << "{rx-platform} "
				<< rx_gate::instance().get_rx_version();
			if (supports_ansi())
				std::cout << ANSI_COLOR_RESET;
			std::cout << "\r\n\r\n";

			std::cout << "Instance Name:";
			if (supports_ansi())
				std::cout << ANSI_COLOR_GREEN ANSI_COLOR_BOLD;
			std::cout << config.meta_configuration.instance_name << "\r\n";
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
			ret = start_log(config);
			if (ret)
			{
				std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
				std::cout << "Initializing security...";
				ret = rx_internal::rx_security::platform_security::instance().initialize(this, config);
				if (ret)
				{
					std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

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
							ret = console_loop(config, plugins);
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
					rx_internal::plugins::plugins_manager::instance().deinitialize();
					rx_internal::rx_security::platform_security::instance().deinitialize();

				}
				else
				{
					std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError initializing security:\r\n";
					rx_dump_error_result(std::cout, ret);
				}
				rx_platform::log::log_object::instance().deinitialize();
			}

			if (!ret)
			{// we had error, dump log
				dump_startup_log(std::cout);
			}

			rx_deinitialize_os();
		}
		else
		{
			std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError reading configuration file\r\n";
			rx_dump_error_result(std::cout, ret);
		}
	}
	rx_internal::interfaces::io_endpoints::dispatcher_subscriber::deinitialize();
	rx_platform::threads::thread::deinitialize();
	restore_console();
	std::cout << "\r\n";
	return ret ? 0 : -1;
}

rx_result interactive_console_host::setup_console (int argc, char* argv[])
{
	return true;
}

void interactive_console_host::restore_console ()
{
}

string_type interactive_console_host::get_interactive_info ()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_INTERACTIVE_HOST_NAME, RX_INTERACTIVE_HOST_MAJOR_VERSION, RX_INTERACTIVE_HOST_MINOR_VERSION, RX_INTERACTIVE_HOST_BUILD_NUMBER);
	}
	return ret;
}

rx_result interactive_console_host::build_host (hosting::host_platform_builder& builder)
{
#ifndef RX_MIN_MEMORY
	auto dir_result = rx_gate::instance().add_directory(builder.host_root + RX_DIR_DELIMETER_STR "types");

	auto detail_struct_type = rx_create_reference<meta::basic_types::struct_type>();
	detail_struct_type->meta_info.name = RX_STD_IO_DETAILS_TYPE_NAME;
	detail_struct_type->meta_info.id = rx_node_id(RX_STD_IO_DETAILS_TYPE_ID, 3);
	detail_struct_type->meta_info.parent = rx_node_id(RX_CLASS_STRUCT_BASE_ID);
	detail_struct_type->meta_info.path = "/sys/host/types";
	detail_struct_type->meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	detail_struct_type->complex_data.register_const_value_static("supportsANSI", false);
	auto result = register_host_simple_type<meta::basic_types::struct_type>(detail_struct_type);
	if (!result)
	{
		result.register_error("Unable to register " RX_STD_IO_DETAILS_TYPE_NAME " struct type.");
		return result;
	}

	rx_port_type_ptr interactive_port_type = rx_create_reference<meta::object_types::port_type>();
	interactive_port_type->meta_info.name = RX_STD_IO_TYPE_NAME;
	interactive_port_type->meta_info.id = rx_node_id(RX_STD_IO_TYPE_ID , 3);
	interactive_port_type->meta_info.parent = rx_node_id(RX_EXTERNAL_PORT_TYPE_ID);
	interactive_port_type->meta_info.path = "/sys/host/types";
	interactive_port_type->meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	interactive_port_type->complex_data.register_struct("IODetails", rx_node_id(RX_STD_IO_DETAILS_TYPE_ID, 3));

	result = register_host_type<meta::object_types::port_type>(interactive_port_type);
	if (!result)
	{
		result.register_error("Unable to register " RX_STD_IO_TYPE_NAME " port type.");
		return result;
	}

	meta::runtime_data::application_runtime_data app_inst_data;
	app_inst_data.meta_info.name = RX_HOST_APP_NAME;
	app_inst_data.meta_info.id = RX_HOST_APP_ID;
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
	domain_inst_data.meta_info.id = RX_HOST_DOMAIN_ID;
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
	inst_data.meta_info.name = RX_STD_IO_NAME;
	inst_data.meta_info.id = rx_node_id(RX_STD_IO_ID, 3);
	inst_data.meta_info.parent = rx_node_id(RX_STD_IO_TYPE_ID, 3);
	inst_data.meta_info.path = "/sys/host";
	inst_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	inst_data.instance_data.app_ref = rx_node_id(RX_HOST_APP_ID);

	result = register_host_runtime<meta::object_types::port_type>(inst_data, nullptr);
	if (!result)
	{
		result.register_error("Unable to register " RX_STD_IO_NAME " port runtime.");
		return result;
	}

	inst_data = meta::runtime_data::port_runtime_data();
	inst_data.meta_info.name = RX_STD_VT100_NAME;
	inst_data.meta_info.id = rx_node_id(RX_STD_VT100_ID, 3);
	inst_data.meta_info.parent = rx_node_id(RX_VT00_TYPE_ID);
	inst_data.meta_info.path = "/sys/host";
	inst_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;

	inst_data.instance_data.app_ref = rx_node_id(RX_HOST_APP_ID);

	inst_data.overrides.add_value_static("StackTop", RX_STD_IO_NAME);

	result = register_host_runtime<meta::object_types::port_type>(inst_data, nullptr);
	if (!result)
	{
		result.register_error("Unable to register " RX_STD_VT100_NAME " port runtime.");
		return result;
	}


	return true;
#else
	return "Not implemented for small memory footprint";
#endif
}

string_type interactive_console_host::get_host_manual () const
{
	return rx_platform_host::get_manual("hosts/" RX_INTERACTIVE_HOST);
}

string_type interactive_console_host::get_host_name ()
{
	return RX_INTERACTIVE_HOST;
}

bool interactive_console_host::write_stdout (const string_type& lines)
{
	return write_stdout(lines.c_str(), lines.size());
}

void interactive_console_host::console_run_result (rx_result result)
{
	if (result)
	{
		std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
	}
	else
	{
		std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError running Interactive Console:\r\n";
		rx_dump_error_result(std::cout, result);
		std::cout << "\r\n";
		dump_startup_log(std::cout);
	}
	host_started();
}

void interactive_console_host::terminal_size_changed (int width, int height)
{
	interactive_port_->terminal_size_changed(width, height);
}

string_type interactive_console_host::get_default_user_storage () const
{
  return ".";

}

string_type interactive_console_host::get_default_log_directory () const
{
  return ".";

}


// Class host::interactive::interactive_console_port 

interactive_console_port::interactive_console_port (interactive_console_host* host)
      : listening_(false)
	, endpoint_(host, this)
{
}



rx_result interactive_console_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result interactive_console_port::run_interactive (interactive_console_host* host)
{
	auto ticks = rx_get_tick_count();
	rx_timer_ticks_t wait_period = 2000;
#ifdef _DEBUG
	wait_period = 2000000;
#endif
	while (!listening_ && !host->exit())
	{
		rx_ms_sleep(50);
		if ((rx_get_tick_count() - ticks) > wait_period)
		{
			break;
		}
	}
	if (!listening_)// if we break the loop?
	{
		host->console_run_result("Waiting for port creation timeouted.");
		return true;
	}

	host->console_run_result(true);

	host->write_stdout(RX_LICENSE_MESSAGE);

	add_stack_endpoint(&endpoint_.stack_entry_, nullptr, nullptr);
	auto result = endpoint_.run_interactive([this](int64_t count)
		{

		});

	return result;
}

rx_result interactive_console_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
	if (listening_)
	{
		RX_ASSERT(false);
		return "Already listening.";
	}
	auto result = endpoint_.open([this](int64_t count)
		{

		});
	if (!result)
		return result;

	listening_ = true;
	return true;
}

void interactive_console_port::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
}

rx_result interactive_console_port::stop_passive ()
{
	return true;
}

void interactive_console_port::terminal_size_changed (int width, int height)
{
	if (listening_)
		endpoint_.terminal_size_changed(width, height);
}


// Class host::interactive::interactive_console_endpoint 

interactive_console_endpoint::interactive_console_endpoint (interactive_console_host* host, interactive_console_port* my_port)
      : host_(host),
        my_port_(my_port)
	, std_out_sender_("Standard Out Writer", RX_DOMAIN_EXTERN)
{
	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.send_function = &interactive_console_endpoint::send_function;
	stack_entry_.close_function = [](rx_protocol_stack_endpoint* reference, rx_protocol_result_t reason)->rx_protocol_result_t
		{
			if(!rx_gate::instance().is_shutting_down())
				rx_platform::rx_gate::instance().shutdown("Interactive Shutdown");

			rx_notify_closed(reference, RX_PROTOCOL_OK);

			return RX_PROTOCOL_OK;
		};
	stack_entry_.closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
		{
			interactive_console_endpoint* whose = (interactive_console_endpoint*)entry->user_data;
			whose->my_port_->unbind_stack_endpoint(entry);
		};
}



rx_result interactive_console_endpoint::run_interactive (std::function<void(int64_t)> received_func)
{
	std::array<char, 0x100> in_buffer;

	size_t count;


	while (!host_->exit())
	{
		count = 0;

		rx_const_packet_buffer buffer{};

		bool result = host_->read_stdin(in_buffer, count);
		if (!result)
		{
			HOST_LOG_ERROR("interactive_console_endpoint", 900, "Error reading <stdin>, exiting!");
			break;
		}
		if (count > 0)
		{
			rx_init_const_packet_buffer(&buffer, &in_buffer[0], count);
			received_func(buffer.size);
			auto res = rx_move_packet_up(&stack_entry_, rx_create_recv_packet(0, &buffer, 0, 0));
			if (res != RX_PROTOCOL_OK)
			{
				std::ostringstream ss;
				ss << "Error code " << (int)res << "(" << rx_protocol_error_message(res) << ") returned by stack!\r\n";
				std::cout << ss.str();

				HOST_LOG_ERROR("interactive_console_endpoint", 900, ss.str());
				HOST_LOG_ERROR("interactive_console_endpoint", 900, "Error reading <stdin>, exiting!");
				break;
			}
		}
	}
	rx_session session = rx_create_session(nullptr, nullptr, 0, 0, nullptr);
	rx_notify_disconnected(&stack_entry_, &session, RX_PROTOCOL_OK);
	std_out_sender_.end();
	return true;
}

rx_protocol_result_t interactive_console_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	interactive_console_endpoint* self = reinterpret_cast<interactive_console_endpoint*>(reference->user_data);
	std::vector<uint8_t> captured(packet.buffer->buffer_ptr, packet.buffer->buffer_ptr + packet.buffer->size);
	auto packet_id = packet.id;

	auto job = rx_create_func_job(rx_reference_ptr(), [self, packet_id](std::vector<uint8_t>&& buffer)
		{
			auto result = self->host_->write_stdout(&buffer[0], buffer.size());
			if (result)
			{
				self->sent_func_(buffer.size());
				rx_notify_ack(&self->stack_entry_, packet_id, RX_PROTOCOL_OK);
			}
			else
			{
				std::cout << "Error sending data to std out\r\n";
			}
		}, std::move(captured));

	self->std_out_sender_.append(job);

	return RX_PROTOCOL_OK;
}

void interactive_console_endpoint::close ()
{
	host_->exit();
}

rx_result interactive_console_endpoint::open (std::function<void(int64_t)> sent_func)
{
	sent_func_ = sent_func;
	std_out_sender_.start();
	return true;
}

void interactive_console_endpoint::terminal_size_changed (int width, int height)
{
	char str_buff[0x20];
	int ret = sprintf(str_buff, "\033]T %d %d\033\\", width, height);
	if (ret > 0)
	{
		rx_const_packet_buffer buffer{};
		rx_init_const_packet_buffer(&buffer, str_buff, ret);
		rx_move_packet_up(&stack_entry_, rx_create_recv_packet(0, &buffer, 0, 0));
	}
}


} // namespace interactive
} // namespace host

