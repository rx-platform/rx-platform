

/****************************************************************************
*
*  system\hosting\rx_host.cpp
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


// rx_security
#include "lib/security/rx_security.h"
// rx_plugin
#include "system/libraries/rx_plugin.h"
// rx_host
#include "system/hosting/rx_host.h"

#include "system/hosting/rx_yaml.h"
#include "system/server/rx_ns.h"
#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_plugin_manager.h"
#include "model/rx_meta_internals.h"
#include "terminal/rx_con_commands.h"


namespace rx_platform {

template<typename typeT>
rx_result register_host_constructor(const rx_node_id& id, std::function<typename typeT::RImplPtr()> f)
{
	return rx_internal::model::platform_types_manager::instance().get_type_repository<typeT>().register_constructor(id, f);
}
template rx_result register_host_constructor<meta::object_types::object_type>(const rx_node_id& id, std::function<rx_object_impl_ptr()> f);
template rx_result register_host_constructor<meta::object_types::domain_type>(const rx_node_id& id, std::function<rx_domain_impl_ptr()> f);
template rx_result register_host_constructor<meta::object_types::port_type>(const rx_node_id& id, std::function<rx_port_impl_ptr()> f);
template rx_result register_host_constructor<meta::object_types::application_type>(const rx_node_id& id, std::function<rx_application_impl_ptr()> f);

template<typename typeT>
rx_result register_host_simple_constructor(const rx_node_id& id, std::function<typename typeT::RTypePtr()> f)
{
	return rx_internal::model::platform_types_manager::instance().get_simple_type_repository<typeT>().register_constructor(id, f);
}
template rx_result register_host_simple_constructor<mapper_type>(const rx_node_id& id, std::function<runtime::mapper_runtime_ptr()> f);
template rx_result register_host_simple_constructor<source_type>(const rx_node_id& id, std::function<runtime::source_runtime_ptr()> f);
template rx_result register_host_simple_constructor<filter_type>(const rx_node_id& id, std::function<runtime::filter_runtime_ptr()> f);
template rx_result register_host_simple_constructor<variable_type>(const rx_node_id& id, std::function<runtime::variable_runtime_ptr()> f);
template rx_result register_host_simple_constructor<event_type>(const rx_node_id& id, std::function<runtime::event_runtime_ptr()> f);
template rx_result register_host_simple_constructor<struct_type>(const rx_node_id& id, std::function<runtime::struct_runtime_ptr()> f);

namespace hosting {
namespace
{
rx_result do_read_config_files(const rx_host_directories& host_directories, const string_type host_name, configuration_reader& reader, rx_platform::configuration_data_t& config)
{
	string_type platform_file_name("rx-platform.yml");
	string_type host_file_name(host_name + ".yml");
	rx_result ret = false;
	bool one_success = false;
	
	string_array paths{
			rx_combine_paths(host_directories.local_folder, host_file_name),
			rx_combine_paths(host_directories.local_folder, platform_file_name),
			rx_combine_paths(host_directories.user_config, host_file_name),
			rx_combine_paths(host_directories.user_config, platform_file_name),
			rx_combine_paths(host_directories.system_config, host_file_name),
			rx_combine_paths(host_directories.system_config, platform_file_name)
	};
	for (const auto& config_path : paths)
	{
		if (config_path.empty() || !rx_file_exsist(config_path.c_str()))
			continue;

		HOST_LOG_TRACE("rx_host", 500, "Reading configuration file "s + config_path);

		string_type settings_buff;
		rx_source_file file;
		if (file.open(config_path.c_str()))
		{
			ret = file.read_string(settings_buff);
			if (!ret)
			{
				ret = rx_result::create_from_last_os_error("error reading configuration file: "s + config_path);
			}
		}
		else
		{
			ret = rx_result::create_from_last_os_error("error opening configuration file: "s + config_path);
		}

		if (ret)
		{
			std::map<string_type, string_type> config_values;
			ret = reader.parse_configuration(settings_buff, config_values);
			if (ret)
			{
				for (auto& row : config_values)
				{
					if (row.first == "storage.system"
						&& config.storage.system_storage_reference.empty())
						config.storage.system_storage_reference = row.second;
					else if (row.first == "storage.user"
						&& config.storage.user_storage_reference.empty())
						config.storage.user_storage_reference = row.second;
					else if (row.first == "storage.test"
						&& config.storage.test_storage_reference.empty())
						config.storage.test_storage_reference = row.second;
					else if (row.first == "other.manuals" && config.other.manuals_path.empty())
						config.other.manuals_path = row.second;
					else if (row.first == "other.logs" && config.management.logs_directory.empty())
						config.management.logs_directory = row.second;
				}
				one_success = true;
			}
			else
			{
				ret.register_error("Error parsing config file "s + config_path);
			}
		}
		else
		{
			ret.register_error("Error reading config file "s + config_path);
		}
	}
	if (one_success)
	{
		return true;
	}
	else
		return ret;
}
}

// Class rx_platform::hosting::rx_platform_host 

rx_platform_host::rx_platform_host(const rx_platform_host &right)
      : parent_(nullptr)
{
	RX_ASSERT(false);
}

rx_platform_host::rx_platform_host (rx_host_storages& storage)
      : parent_(nullptr)
	, storages_(storage)
{
}


rx_platform_host::~rx_platform_host()
{
}


rx_platform_host & rx_platform_host::operator=(const rx_platform_host &right)
{
	RX_ASSERT(false);
	return *this;
}



void rx_platform_host::get_host_info (string_array& hosts)
{
}

void rx_platform_host::server_started_event ()
{
}

void rx_platform_host::server_stopping_event ()
{
}

bool rx_platform_host::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{

	std::ostream out(out_buffer.unsafe_ptr());
	out << "dummy implementation by base interface class\r\n";

	return true;
}

std::vector<ETH_interface> rx_platform_host::get_ETH_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	std::vector<ETH_interface> ret;
	return ret;
}

std::vector<IP_interface> rx_platform_host::get_IP_interfaces (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	std::vector<IP_interface> ret;
	return ret;
}

rx_result rx_platform_host::read_config_file (configuration_reader& reader, rx_platform::configuration_data_t& config)
{
	// fill paths
	rx_host_directories temp_directories;
	rx_result ret = fill_host_directories(temp_directories);
	if (!ret)
		return ret;

	ret = do_read_config_files(temp_directories, get_host_name(), reader, config);

	if (config.storage.system_storage_reference.empty())
		config.storage.system_storage_reference = temp_directories.system_storage;
	if (config.storage.user_storage_reference.empty())
		config.storage.user_storage_reference = temp_directories.user_storage;
	if (config.other.manuals_path.empty())
		config.other.manuals_path = temp_directories.manuals;

	manuals_path_ = config.other.manuals_path;

	

	return ret;
}

rx_result rx_platform_host::initialize_storages (rx_platform::configuration_data_t& config, const std::vector<library::rx_plugin_base*>& plugins)
{
	rx_result ret;
	if (config.storage.system_storage_reference.empty())
		ret = "No valid system storage reference!";
	else
		ret = storages_.system_storage->init_storage(config.storage.system_storage_reference, this);
	if (ret)
	{
		if (config.storage.user_storage_reference.empty())
			ret = "No valid user storage reference!";
		else
			ret = storages_.user_storage->init_storage(config.storage.user_storage_reference, this);
		if (ret)
		{
			if (storages_.test_storage && !config.storage.test_storage_reference.empty())
			{
				ret = storages_.test_storage->init_storage(config.storage.test_storage_reference, this);
				if (!ret)
				{
					ret.register_error("Error initializing test storage!");
				}
			}
			auto add_result = storages_.system_storage->get_storage(get_host_name(), this);
			if (add_result)
			{
				for (const auto& one : plugins)
				{
					add_result = storages_.system_storage->get_storage(one->get_plugin_name(), this);
					if (!add_result)
					{
						ret.register_error("Unable to initialize plugin storage for "s + one->get_plugin_name() + ".");
						ret.register_errors(add_result.errors());
						break;
					}
				}
			}
			else
			{
				ret.register_error("Unable to initialize host storage.");
				ret.register_errors(add_result.errors());
			}
		}
		else
		{
			ret.register_error("Error initializing user storage!");
		}
	}
	else
	{
		ret.register_error("Error initializing system storage!");
	}
	return ret;
}

void rx_platform_host::deinitialize_storages ()
{
	if(storages_.test_storage)
		storages_.test_storage->deinit_storage();
	storages_.user_storage->deinit_storage();
	storages_.system_storage->deinit_storage();
}

void rx_platform_host::add_command_line_options (command_line_options_t& options, rx_platform::configuration_data_t& config)
{
	options.add_options()
		("r,real-time", "Force Real-time priority for process", cxxopts::value<bool>(config.processor.real_time))
		("s,startup", "Startup script", cxxopts::value<string_type>(config.management.startup_script))
		("u,user", "User storage reference", cxxopts::value<string_type>(config.storage.user_storage_reference))
		("t,test", "Test storage reference", cxxopts::value<string_type>(config.storage.test_storage_reference))
		("y,system", "System storage reference", cxxopts::value<string_type>(config.storage.system_storage_reference))
		("n,name", "rx-platform Instance Name", cxxopts::value<string_type>(config.meta_configuration.instance_name))
		("log-test", "Test log at startup", cxxopts::value<bool>(config.management.test_log))
		("l,logs", "Location of the log files", cxxopts::value<string_type>(config.management.logs_directory))
		("v,version", "Displays platform version")
		("code", "Force building platform system from code builders", cxxopts::value<bool>(config.meta_configuration.build_system_from_code))
		("h,help", "Print help")
		;
}

rx_result rx_platform_host::register_plugins (std::vector<library::rx_plugin_base*>& plugins)
{
	rx_result ret;
	for (auto one : plugins)
	{
		ret = rx_internal::plugins::plugins_manager::instance().register_plugin(one);
	}
	return ret;
}

string_type rx_platform_host::get_manual (string_type what) const
{
	if (manuals_path_.empty())
		return "";
	string_type path = rx_combine_paths(manuals_path_, what + ".man");
	rx_source_file file;
	auto result = file.open(path.c_str());
	if (result)
	{
		string_type buffer;
		result = file.read_string(buffer);
		if (!result)
			return "";
		return buffer;
	}
	return "";
}

void rx_platform_host::print_offline_manual (const string_type& host, const rx_host_directories& dirs)
{
	rx_platform::configuration_data_t config;

	rx_platform::hosting::simplified_yaml_reader reader;
	auto result = do_read_config_files(dirs, host, reader, config);
	if (!result)
	{
		std::cout << "Error parsing config files";
		return;
	}
	string_type path = rx_combine_paths(config.other.manuals_path, "hosts/"s + host + ".man");
	rx_source_file file;
	string_type man;
	result = file.open(path.c_str());
	if (result)
	{
		result = file.read_string(man);
		if (!result)
		{
			std::cout << "Error reading file";
			return;
		}
	}
	else
	{
		std::cout << "Error reading file";
		return;
	}
	std::cout << man;
	std::cout << "\r\n";
}

rx_result_with<rx_storage_ptr> rx_platform_host::get_system_storage (const string_type& name)
{
	return storages_.system_storage->get_storage(name, this);
}

rx_result_with<rx_storage_ptr> rx_platform_host::get_user_storage (const string_type& name)
{
	return storages_.user_storage->get_storage(name, this);
}

rx_result_with<rx_storage_ptr> rx_platform_host::get_test_storage (const string_type& name)
{
	if (storages_.test_storage)
		return storages_.test_storage->get_storage(name, this);
	else
		return "Test storage not active.";
}

void rx_platform_host::dump_log_items (const log::log_events_type& items, std::ostream& out)
{
	rx_internal::terminal::console::console_commands::list_log_options options;
	options.list_level = false;
	options.list_code = false;
	options.list_library = false;
	options.list_source = false;
	options.list_dates = false;
	rx_internal::terminal::console::console_commands::log_command::dump_log_items(items, options, out, -1);
}


// Class rx_platform::hosting::configuration_reader 


} // namespace hosting
} // namespace rx_platform

