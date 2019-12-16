

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
#include "rx_interactive_config.h"
#include "terminal/rx_terminal_style.h"
#include "system/hosting/rx_yaml.h"

// rx_host
#include "system/hosting/rx_host.h"
// rx_interactive
#include "host/rx_interactive.h"

#include "api/rx_meta_api.h"

#define SAFE_ANSI_STATUS_ERROR (supports_ansi() ? ANSI_STATUS_ERROR : "ERROR")
#define SAFE_ANSI_STATUS_OK (supports_ansi() ? ANSI_STATUS_OK : "OK")

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



rx_result interactive_console_host::console_loop (configuration_data_t& config, std::vector<library::rx_plugin_base*>& plugins)
{
	rx_platform::hosting::host_security_context::smart_ptr sec_ctx(pointers::_create_new);
	rx_result login_result = sec_ctx->login();
	if (!login_result)
	{
		std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError while trying to login as a host:\r\n";
		rx_dump_error_result(std::cout, login_result);
	}

	security::security_auto_context dummy(sec_ctx);

	if (!config.management.telnet_port)// set to the last default if not set
		config.management.telnet_port = 12345;
	if (config.processor.genereal_pool_size < 0)
		config.processor.genereal_pool_size = 2;
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
	std::cout << "Initializing rx-platform...";
	result = rx_platform::rx_gate::instance().initialize(this, config);
	if (result)	
	{
		std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
		HOST_LOG_INFO("Main", 999, "Starting Rx Engine...");
		std::cout << "Starting rx-platform...";
		result = rx_platform::rx_gate::instance().start(this, config);
		if (result)
		{
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

			terminal::rx_vt100::vt100_transport_port vt100_port;
			vt100_port.bind_port();

			terminal::console::console_runtime console;
			console.bind_port();

			rx_protocol_result_t res = rx_push_stack(interactive_port_->get_stack_entry(), vt100_port.get_stack_entry());
			res = rx_push_stack(vt100_port.get_stack_entry(), console.get_stack_entry());

			std::cout << "Starting interactive console...";
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
			result = interactive_port_->run_interactive(this);

			if (!result)
			{
				std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError running Interactive Console:\r\n";
				rx_dump_error_result(std::cout, result);
			}
			std::cout << "Stopping rx-platform...";
			result = rx_platform::rx_gate::instance().stop();
			if(result)
				std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
			else
			{
				std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError stopping rx-platform:\r\n";
				rx_dump_error_result(std::cout, result);
			}
		}
		else
		{
			std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError starting rx-platform:\r\n";
			rx_dump_error_result(std::cout, result);
		}
		std::cout << "De-initializing rx-platform...";
		result = rx_platform::rx_gate::instance().deinitialize();
		if (result)
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
		else
		{
			std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError deinitialize rx-platform:\r\n";
			rx_dump_error_result(std::cout, result);
		}
	}
	else
	{
		std::cout << SAFE_ANSI_STATUS_ERROR << "\r\nError initializing rx-platform:\r\n";
		rx_dump_error_result(std::cout, result);
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

			// fill paths
			hosting::rx_host_directories host_directories;
			rx_result fill_result = fill_host_directories(host_directories);
			if (!fill_result)
			{
				std::cout << "\r\nERROR\r\n";
			}
			rx_platform_host::print_offline_manual(RX_INTERACTIVE_HOST, host_directories);
			
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

int interactive_console_host::console_main (int argc, char* argv[], std::vector<library::rx_plugin_base*>& plugins)
{
	rx_result ret = setup_console(argc, argv);

	rx_platform::configuration_data_t config;
	ret = parse_command_line(argc, argv, config); 
	if (ret)
	{		
		rx_platform::hosting::simplified_yaml_reader reader;
		std::cout << "Reading configuration file...";
		ret = read_config_file(reader, config);
		if (ret)
		{			
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
			rx_thread_data_t tls = rx_alloc_thread_data();
			string_type server_name = get_default_name();

			std::cout << "Initializing OS interface...";
			rx_initialize_os(config.processor.real_time, tls, server_name.c_str());
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";
			
			std::cout << "\r\n";
			if(supports_ansi())
				std::cout << ANSI_COLOR_GREEN ANSI_COLOR_BOLD;
			std::cout << "rx-platform "
				<< rx_gate::instance().get_rx_version();
			if (supports_ansi())
				std::cout << ANSI_COLOR_RESET;
			std::cout << "\r\n\r\n";;
				
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
			ret = rx::log::log_object::instance().start(config.management.test_log);
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
					if (!ret)
					{// we had error, dump log

						std::cout << "\r\n\r\nStartup log entries:\r\n";

						log::log_query_type query;
						log::log_events_type events;
						query.type = log::rx_log_query_type::error_level;
						rx::log::log_object::instance().read_cache(query, events);
						
						hosting::rx_platform_host::dump_log_items(events, std::cout);

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
		restore_console();
	}
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
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_INTERACTIVE_HOST_NAME, RX_INTERACTIVE_HOST_MAJOR_VERSION, RX_INTERACTIVE_HOST_MINOR_VERSION, RX_INTERACTIVE_HOST_BUILD_NUMBER);
	}
	return ret;
}

rx_result interactive_console_host::build_host (rx_directory_ptr root)
{
	return true;
}

string_type interactive_console_host::get_host_manual () const
{
	return rx_platform_host::get_manual("hosts/" RX_INTERACTIVE_HOST);
}

string_type interactive_console_host::get_host_name ()
{
	return RX_INTERACTIVE_HOST;
}


// Class host::interactive::interactive_console_client 


rx_result interactive_console_client::run_interactive (configuration_data_t& config)
{

	auto result = check_validity();
	if (!result)
		return result;



	string_type temp_script(config.management.startup_script);
	if(!temp_script.empty())
		temp_script += "\r\n";
	
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

				//cancel_command(out_buffer, err_buffer, security_context_);

				continue;
			}

			if (std::cin.fail())
			{
				std::cin.clear();
				if (host_->is_canceling())
				{
					memory::buffer_ptr out_buffer(pointers::_create_new);
					memory::buffer_ptr err_buffer(pointers::_create_new);

					//cancel_command(out_buffer, err_buffer, security_context_);
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

			//do_commands(std::move(lines), out_buffer, err_buffer, security_context_);
		}

	}
	//security_context_->logout();

	return true;
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


// Class host::interactive::interactive_console_port 

interactive_console_port::interactive_console_port (hosting::rx_platform_host* host)
      : endpoint_(host)
{
}



rx_result interactive_console_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = physical_port::initialize_runtime(ctx);
	if (result)
	{
		result = endpoint_.open([this](size_t count)
			{
				update_sent_counters(count);
			});
	}
	return result;
}

rx_result interactive_console_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	auto result = physical_port::deinitialize_runtime(ctx);
	if (result)
	{
	}
	return result;
}

rx_result interactive_console_port::start_runtime (runtime::runtime_start_context& ctx)
{
	auto result = physical_port::start_runtime(ctx);
	if (result)
	{
	}
	return result;
}

rx_result interactive_console_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
	auto result = physical_port::stop_runtime(ctx);
	if (result)
	{
	}
	return result;
}

rx_protocol_stack_entry* interactive_console_port::get_stack_entry ()
{
	return &endpoint_;
}

rx_result interactive_console_port::run_interactive (hosting::rx_platform_host* host)
{

	interactive_security_context::smart_ptr sec_ctx(pointers::_create_new);
	rx_result login_result = sec_ctx->login();


	security::security_auto_context dummy(sec_ctx);

	host->write_stdout(RX_LICENSE_MESSAGE);

	auto result = endpoint_.run_interactive([this](size_t count)
		{
			update_received_counters(count);

		});


	sec_ctx->logout();

	return result;
}


// Class host::interactive::interactive_console_endpoint 

interactive_console_endpoint::interactive_console_endpoint (hosting::rx_platform_host* host)
      : host_(host)
	, std_out_sender_("Standard Out Writer", RX_DOMAIN_EXTERN)
{
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
			ITF_LOG_ERROR("rx_pipe_host", 900, "Error reading pipe, exiting!");
			break;
		}
		if (count > 0)
		{
			rx_init_const_packet_buffer(&buffer, &in_buffer[0], count);
			received_func(buffer.size);
			auto res = rx_move_packet_up(this, nullptr, &buffer);
			if (res != RX_PROTOCOL_OK)
			{
				std::cout << "Error code " << (int)res << " returned by stack!\r\n";
				break;
			}
		}
	}
	return true;
}

rx_protocol_result_t interactive_console_endpoint::send_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer)
{
	interactive_console_endpoint* self = reinterpret_cast<interactive_console_endpoint*>(reference);

	self->std_out_sender_.append(
		rx_create_reference<jobs::lambda_job<rx_packet_buffer> >(
			[self](rx_packet_buffer buffer)
			{
				auto result = self->host_->write_stdout(buffer.buffer_ptr, buffer.size);
				if (result)
				{
					self->sent_func_(buffer.size);
					rx_move_result_up(self, NULL, RX_PROTOCOL_OK);
				}
				else
				{
					std::cout << "Error sending data to std out\r\n";
				}
				rx_deinit_packet_buffer(&buffer);
			},
			*buffer
				)
	);

	return RX_PROTOCOL_OK;
}

void interactive_console_endpoint::close ()
{
	std_out_sender_.end();
}

rx_result interactive_console_endpoint::open (std::function<void(int64_t)> sent_func)
{
	rx_protocol_stack_entry* mine_entry = this;

	sent_func_ = sent_func;
	std_out_sender_.start();
	mine_entry->downward = nullptr;
	mine_entry->upward = nullptr;

	mine_entry->send_function = &interactive_console_endpoint::send_function;
	mine_entry->sent_function = nullptr;

	mine_entry->received_function = nullptr;

	mine_entry->connected_function = nullptr;

	mine_entry->close_function = nullptr;
	mine_entry->closed_function = nullptr;


	mine_entry->allocate_packet_function = nullptr;
	mine_entry->free_packet_function = nullptr;

	return true;
}


} // namespace interactive
} // namespace host

