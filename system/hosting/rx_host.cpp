

/****************************************************************************
*
*  system\hosting\rx_host.cpp
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


// rx_security
#include "security/rx_security.h"
// rx_plugin
#include "system/libraries/rx_plugin.h"
// rx_host
#include "system/hosting/rx_host.h"

#include "sys_internal/rx_security/rx_platform_security.h"
#include "system/hosting/rx_yaml.h"
#include "system/server/rx_ns.h"
#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_plugin_manager.h"
#include "model/rx_meta_internals.h"
#include "terminal/rx_con_commands.h"
#include "model/rx_model_algorithms.h"
#include "sys_internal/rx_storage_build.h"
#include "terminal/rx_term_table.h"
#include "system/server/rx_file_helpers.h"
using namespace rx;


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


template<typename typeT>
rx_result register_host_type(typename typeT::smart_ptr what)
{
	if (what->meta_info.created_time.is_null())
		what->meta_info.created_time = rx_time::now();

	if (what->meta_info.modified_time.is_null())
		what->meta_info.modified_time = rx_time::now();

	auto result = rx_internal::model::algorithms::types_model_algorithm<typeT>::create_type_sync(what);
	if (!result)
		return result.errors();
	else
		return true;
}
template rx_result register_host_type<object_type>(rx_object_type_ptr what);
template rx_result register_host_type<domain_type>(rx_domain_type_ptr what);
template rx_result register_host_type<port_type>(rx_port_type_ptr what);
template rx_result register_host_type<application_type>(rx_application_type_ptr what);

template<typename typeT>
rx_result register_host_simple_type(typename typeT::smart_ptr what)
{
	if (what->meta_info.created_time.is_null())
		what->meta_info.created_time = rx_time::now();

	if (what->meta_info.modified_time.is_null())
		what->meta_info.modified_time = rx_time::now();

	auto result = rx_internal::model::algorithms::simple_types_model_algorithm<typeT>::create_type_sync(what);
	if (!result)
		return result.errors();
	else
		return true;
}
template rx_result register_host_simple_type<mapper_type>(mapper_type::smart_ptr what);
template rx_result register_host_simple_type<source_type>(source_type::smart_ptr what);
template rx_result register_host_simple_type<filter_type>(filter_type::smart_ptr what);
template rx_result register_host_simple_type<variable_type>(variable_type::smart_ptr what);
template rx_result register_host_simple_type<event_type>(event_type::smart_ptr what);
template rx_result register_host_simple_type<struct_type>(struct_type::smart_ptr what);


rx_result register_host_relation_type(relation_type_ptr what)
{

	if (what->meta_info.created_time.is_null())
		what->meta_info.created_time = rx_time::now();

	if (what->meta_info.modified_time.is_null())
		what->meta_info.modified_time = rx_time::now();

	auto result = rx_internal::model::algorithms::relation_types_algorithm::create_type_sync(what);
	if (!result)
		return result.errors();
	else
		return true;
}
rx_result register_host_data_type(data_type_ptr what)
{

	if (what->meta_info.created_time.is_null())
		what->meta_info.created_time = rx_time::now();

	if (what->meta_info.modified_time.is_null())
		what->meta_info.modified_time = rx_time::now();

	auto result = rx_internal::model::algorithms::data_types_model_algorithm::create_type_sync(what);
	if (!result)
		return result.errors();
	else
		return true;
}

template<typename typeT>
rx_result register_host_runtime(const typename typeT::instance_data_t& instance_data, const data::runtime_values_data* data)
{
	auto result = rx_internal::model::algorithms::runtime_model_algorithm<typeT>::create_runtime_sync(
		typename typeT::instance_data_t(instance_data)
		, data ? data::runtime_values_data(*data) : data::runtime_values_data());
	if (!result)
		return result.errors();
	else
		return true;
}
template rx_result register_host_runtime<object_type>(const object_type::instance_data_t& instance_data, const data::runtime_values_data* data);
template rx_result register_host_runtime<domain_type>(const domain_type::instance_data_t& instance_data, const data::runtime_values_data* data);
template rx_result register_host_runtime<port_type>(const port_type::instance_data_t& instance_data, const data::runtime_values_data* data);
template rx_result register_host_runtime<application_type>(const application_type::instance_data_t& instance_data, const data::runtime_values_data* data);

namespace hosting {
namespace
{
bool get_bool_value(const string_type val)
{
    if(val=="True"
        || val=="true"
        || val == "T"
        || val == "t"
        || val == "TRUE"
        || val =="1"
        || val == "Yes"
        || val == "yes"
        || val == "YES"
        || val == "Y")
        return true;
    else if(val=="False"
        || val=="false"
        || val == "F"
        || val == "f"
        || val == "FALSE"
        || val =="0"
        || val == "No"
        || val == "no"
        || val == "NO"
        || val == "N")
        return false;
    // for now this is allowed
    return false;
}

size_t str_to_mem(const char* str)
{
	size_t len = 0;
	size_t multiply = 0;
	int radix = 10;
	if (str)
	{
		len = strlen(str);
	}
	if (len == 0)
		return 0;
	if (len > 1)
	{
		switch (str[len - 1])
		{
			case 'G':
				multiply += 10;
			case 'M':
				multiply += 10;
			case 'K':
				multiply += 10;
			default:
				;
		}
	}
	if (len > 3)
	{
		if (memcmp(str, "0x", 2) == 0)
		{
			radix = 0x10;
			str += 2;
		}

	}
	char* end_ptr = nullptr;
	return strtoul(str, &end_ptr, radix) << multiply;
}

void read_base_config_options(const std::map<string_type, string_type>& options, rx_platform::configuration_data_t& config)
{
	for (auto& row : options)
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
		else if (row.first == "instance.name"
			&& config.instance.name.empty())
			config.instance.name = row.second;
		else if (row.first == "other.manuals" && config.other.manuals_path.empty())
			config.other.manuals_path = row.second;
		else if (row.first == "http.resources" && config.other.http_path.empty())
			config.other.http_path = row.second;
		else if (row.first == "http.user" && config.other.http_user.empty())
			config.other.http_user = row.second;
		else if (row.first == "upy.resources" && config.other.upy_path.empty())
			config.other.upy_path = row.second;
		else if (row.first == "upy.user" && config.other.upy_user.empty())
			config.other.upy_user = row.second;
		else if (row.first == "rx.security" && config.other.rx_security.empty())
			config.other.rx_security = row.second;
		else if (row.first.size() > 8 && row.first.substr(0, 5) == "x509.")
			config.other.certificates.emplace(row);
		else if (row.first == "http.port" && config.other.http_port == 0)
			config.other.http_port = atoi(row.second.c_str());
		else if (row.first == "mqtt.broker_port" && config.other.mqtt_port == 0)
			config.other.mqtt_port = atoi(row.second.c_str());
		else if (row.first == "mqtt.broker_addr" && config.other.mqtt_address.empty())
			config.other.mqtt_address = row.second;
		else if (row.first == "opc.port" && config.other.opcua_port == 0)
			config.other.opcua_port = atoi(row.second.c_str());
		else if (row.first == "instance.port" && config.instance.port == 0)
			config.instance.port = atoi(row.second.c_str());
		else if (row.first == "instance.group" && config.instance.group.empty())
			config.instance.group = row.second;
		else if (row.first == "rx.port" && config.other.rx_port == 0)
			config.other.rx_port = atoi(row.second.c_str());
		else if (row.first == "rx.wsport" && config.other.rx_wsport == 0)
			config.other.rx_wsport = atoi(row.second.c_str());
		else if (row.first == "processor.realtime" && !config.processor.real_time && get_bool_value(row.second))
			config.processor.real_time = true;
		else if (row.first == "processor.nohdtimer" && !config.processor.no_hd_timer && get_bool_value(row.second))
			config.processor.no_hd_timer = true;
		else if (row.first == "heap.initial" && config.heap.initial_heap_size == 0)
			config.heap.initial_heap_size = str_to_mem(row.second.c_str());
		else if (row.first == "heap.alloc" && config.heap.heap_alloc_size == 0)
			config.heap.heap_alloc_size = str_to_mem(row.second.c_str());
		else if (row.first == "heap.trigger" && config.heap.heap_trigger == 0)
			config.heap.heap_trigger = atoi(row.second.c_str());
		else if (row.first == "heap.bucket" && config.heap.heap_bucket_capacity == 0)
			config.heap.heap_bucket_capacity = atoi(row.second.c_str());
		else if (row.first == "instance.code" && !config.build_system_from_code && get_bool_value(row.second))
			config.build_system_from_code = true;
		else if (row.first.size() > 8 && row.first.substr(0, 6) == "world.")
			config.user_storages.emplace(row);
		else if (row.first.size() > 6 && row.first.substr(0, 4) == "sys.")
			config.system_storages.emplace(row);

		else if (row.first == "log.level" && config.log.log_level < 0)
			config.log.log_level = atoi(row.second.c_str());
		else if (row.first == "log.cachesize")
			config.log.cache_size = atoi(row.second.c_str());
		else if (row.first == "log.directory" && config.log.directory.empty())
			config.log.directory = row.second;
	}
}
}

// Class rx_platform::hosting::rx_platform_host 

rx_platform_host::rx_platform_host(const rx_platform_host &right)
      : parent_(nullptr)
{
	RX_ASSERT(false);
}

rx_platform_host::rx_platform_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
      : parent_(nullptr)
{
	for (auto one : storages)
		storages_.storage_types.emplace(one->get_reference_prefix(), one);
}


rx_platform_host::~rx_platform_host()
{
}


rx_platform_host & rx_platform_host::operator=(const rx_platform_host &right)
{
	RX_ASSERT(false);
	return *this;
}



rx_result rx_platform_host::parse_config_files (rx_platform::configuration_data_t& config)
{
	// fill paths
	rx_result ret = fill_host_directories(host_directories_);
	if (!ret)
		return ret;


	if (!host_directories_.copyright_file.empty())
	{
		sys_handle_t file = rx_file(host_directories_.copyright_file.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
		if (file)
		{
			uint64_t size = 0;
			if (RX_OK == rx_file_get_size(file, &size) && size > 0)
			{
				copyright_cache_.assign((size_t)size, ' ');
				if (RX_OK != rx_file_read(file, &copyright_cache_[0], (uint32_t)size, nullptr))
				{
					copyright_cache_.clear();
				}
			}
			rx_file_close(file);
		}
	}
	lic_path_ = host_directories_.license_file;

	simplified_yaml_reader reader;

	string_type platform_file_name("rx-platform.yml");
	string_type host_file_name(get_host_name() + ".yml");
	bool one_success = false;

	string_array paths{
			rx_combine_paths(host_directories_.local_folder, host_file_name),
			rx_combine_paths(host_directories_.local_folder, platform_file_name),
			rx_combine_paths(host_directories_.user_config, host_file_name),
			rx_combine_paths(host_directories_.user_config, platform_file_name),
			rx_combine_paths(host_directories_.system_config, host_file_name),
			rx_combine_paths(host_directories_.system_config, platform_file_name)
	};
	for (const auto& config_path : paths)
	{
		if (config_path.empty() || !rx_file_exsist(config_path.c_str()))
			continue;


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
				one_success = true;
				read_config_options(config_values, config);
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

		if (config.storage.system_storage_reference.empty())
			config.storage.system_storage_reference = host_directories_.system_storage;
		if (config.storage.user_storage_reference.empty())
		{
			string_type def = get_default_user_storage();
			if(!def.empty())
				config.storage.user_storage_reference = def;
			else
				config.storage.user_storage_reference = host_directories_.user_storage;
		}
		if (config.other.manuals_path.empty())
			config.other.manuals_path = host_directories_.manuals;
		if (config.other.http_path.empty())
			config.other.http_path = host_directories_.http;
		if (config.other.upy_path.empty())
			config.other.upy_path = host_directories_.upy;

		if (config.log.log_level < 0 || config.log.log_level>5)
			config.log.log_level = 1;

		if (config.log.directory.empty())
			config.log.directory = get_default_log_directory();

		manuals_path_ = config.other.manuals_path;


		if (config.instance.name.empty())
			config.instance.name = get_host_instance();

		return true;
	}
	else
		return ret;
}

void rx_platform_host::read_config_options (const std::map<string_type, string_type>& options, rx_platform::configuration_data_t& config)
{
	read_base_config_options(options, config);
}

bool rx_platform_host::parse_command_line (int argc, char* argv[], const char* help_name, rx_platform::configuration_data_t& config)
{
	config.other.http_port = 0;
	config.other.mqtt_port = 0;
	config.other.opcua_port = 0;
	config.instance.port = 0;
	config.other.rx_port = 0;
	config.other.rx_wsport = 0;


	cxxopts::Options options(help_name, "");

	add_command_line_options(options, config);

	// had to do stuff, but if we do not have actual command line
	// it must be safe to send 0, nullptr as command line arguments
	// parsers tend do misbehave if send those arguments
	if (argv == nullptr || argc == 0)
		return true;// no command line options, so skip it!

	try
	{
		auto result = options.parse(argc, argv);
		if (result.count("help"))
		{
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

			// fill paths
			hosting::rx_host_directories host_directories_;
			rx_result fill_result = fill_host_directories(host_directories_);
			if (!fill_result)
			{
				std::cout << "\r\nERROR\r\n";
			}
			rx_platform_host::print_offline_manual(help_name, host_directories_);

			std::ostringstream ss;
			ss << options.help({ "" });
			std::cout << ss.str();

			// don't execute
			return false;
		}
		else if (result.count("version"))
		{
			std::cout << SAFE_ANSI_STATUS_OK << "\r\n";

			string_type version = rx_gate::instance().get_rx_version();

			std::cout << "\r\n" ANSI_COLOR_GREEN ANSI_COLOR_BOLD
				<< version << ANSI_COLOR_RESET;

			// don't execute
			return false;
		}
		return true;
	}
	catch (std::exception& ex)
	{
		std::cout << ANSI_STATUS_ERROR "\r\nError parsing command line:\r\n"
			<< ex.what() << "\r\n";


		return false;
	}
}

void rx_platform_host::add_command_line_options (command_line_options_t& options, rx_platform::configuration_data_t& config)
{
	options.add_options()
		("io", "I/O pool thread pool size", cxxopts::value<int>(config.processor.io_pool_size))
		("workers", "Workers pool thread pool size", cxxopts::value<int>(config.processor.workers_pool_size))
		("slow", "Slow pool thread pool size", cxxopts::value<int>(config.processor.slow_pool_size))
		("unassigned", "Use dedicated unassigned thread", cxxopts::value<bool>(config.processor.has_unassigned_pool))
		("calculation", "Use dedicated calculation timer", cxxopts::value<bool>(config.processor.has_calculation_timer))
		("no-hd-timer", "Do not use high definition timer", cxxopts::value<bool>(config.processor.no_hd_timer))
		("r,real-time", "Force Real-time priority for process", cxxopts::value<bool>(config.processor.real_time))
		("heap-initial", "Initial Heap size", cxxopts::value<size_t>(config.heap.initial_heap_size))
		("heap-alloc", "Heap Allocation size", cxxopts::value<size_t>(config.heap.heap_alloc_size))
		("heap-trigger", "Heap Trigger for additional allocation [1-1023]", cxxopts::value<size_t>(config.heap.heap_trigger))

		("s,startup", "Startup script", cxxopts::value<string_type>(config.management.startup_script))
		("u,user", "User storage reference", cxxopts::value<string_type>(config.storage.user_storage_reference))
		("test", "Test storage reference", cxxopts::value<string_type>(config.storage.test_storage_reference))
		("system", "System storage reference", cxxopts::value<string_type>(config.storage.system_storage_reference))
		("n,name", "{rx-platform} Instance Name", cxxopts::value<string_type>(config.instance.name))
		("port", "UDP/IP port for rx-protocol server to listen to", cxxopts::value<uint16_t>(config.instance.port))
		("group", "IP address of rx-protocol multi-cast group", cxxopts::value<string_type>(config.instance.group))
		("plugin", "Load just named plugin", cxxopts::value<string_type>(config.instance.plugin))
		("http-path", "Location of the http resource files", cxxopts::value<string_type>(config.other.http_path))
		("http", "Location of the http user resource files", cxxopts::value<string_type>(config.other.http_user))
		("http-port", "TCP/IP port for web server to listen to", cxxopts::value<uint16_t>(config.other.http_port))
		("upy-path", "Location of the micropython files", cxxopts::value<string_type>(config.other.upy_path))
		("upy", "Location of the micropython user program files", cxxopts::value<string_type>(config.other.upy_user))
		("opc-port", "TCP/IP port for OPCUA binary server to listen to", cxxopts::value<uint16_t>(config.other.opcua_port))

		("rx-port", "TCP/IP port for platform protocol", cxxopts::value<uint16_t>(config.other.rx_port))
		("security", "Default security provider for {rx-platform}", cxxopts::value<string_type>(config.other.rx_security))

		("log-test", "Test log at startup", cxxopts::value<bool>(config.log.test_log))
		("log-size", "Log size for in process log cache", cxxopts::value<uint32_t>(config.log.cache_size))
		("l,log-dir", "Log directory for file logging", cxxopts::value<string_type>(config.log.directory))
		("log-level", "Log level for for file logging", cxxopts::value<int32_t>(config.log.log_level))

		("debug" , "Force debugging options thorough the platform", cxxopts::value<bool>(config.management.debug))
		("v,version", "Displays platform version")
		("code", "Force building platform system from code builders", cxxopts::value<bool>(config.build_system_from_code))
		("h,help", "Print help")
		;
}

void rx_platform_host::get_host_info (hosts_type& hosts)
{
}

void rx_platform_host::server_started_event ()
{
	if (startup_log_)
		startup_log_->started();
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

string_type rx_platform_host::get_default_user_storage () const
{
  return "";

}

rx_result rx_platform_host::initialize_storages (rx_platform::configuration_data_t& config, const std::vector<library::rx_plugin_base*>& plugins)
{
	rx_result result;

	HOST_LOG_INFO("Base", 999, "Initializing storages");

	std::vector<configured_storage_t> storages_to_create;

	for (const auto& strg : config.user_storages)
	{
		auto idx = strg.second.find('@');
		configured_storage_t arg;
		arg.name = (idx != string_type::npos) ? strg.second.substr(0, idx) : "";
		arg.reference = (idx != string_type::npos) ? strg.second.substr(idx + 1) : strg.second;
		arg.storage_id = strg.first;
		storages_to_create.push_back(std::move(arg));
	}
	for (const auto& strg : config.system_storages)
	{
		auto idx = strg.second.find('@');
		configured_storage_t arg;
		arg.name = (idx != string_type::npos) ? strg.second.substr(0, idx) : "";
		arg.reference = (idx != string_type::npos) ? strg.second.substr(idx + 1) : strg.second;
		arg.storage_id = strg.first;
		storages_to_create.push_back(std::move(arg));
	}

	auto fss = read_config_files("rx-fss.yml");
	for (const auto& one : fss)
	{
		for (const auto& strg : one)
		{
			auto idx = strg.second.find('@');
			configured_storage_t arg;
			arg.name = (idx != string_type::npos) ? strg.second.substr(0, idx) : "";
			arg.reference = (idx != string_type::npos) ? strg.second.substr(idx + 1) : strg.second;
			arg.storage_id = strg.first;
			storages_to_create.push_back(std::move(arg));
		}
	}

	if (config.storage.system_storage_reference.empty())
	{
		result = "No valid system storage reference!";
	}
	else
	{
		result = init_storage("sys", config.storage.system_storage_reference);

	}
	if (result)
	{
		get_system_storage("sys");
		if (config.storage.user_storage_reference.empty())
		{
			result = "No valid user storage reference!";
		}
		else
		{
			result = init_storage("usr", config.storage.user_storage_reference);
			if (!result)
			{
				result.register_error("Error initializing user storage:"s + result.errors_line());
			}
			else
			{
				get_user_storage();
			}
		}
		if (result)
		{
			if (!config.storage.test_storage_reference.empty())
			{
				result = init_storage("tst", config.storage.test_storage_reference);
				if (!result)
				{
					result.register_error("Error initializing test storage!");
				}
				else
				{
					get_test_storage();
				}
			}
			auto add_result = storages_.registered_connections["sys"]->get_storage(get_host_name(), this);
			if (add_result)
			{
				for (const auto& one : plugins)
				{
					add_result = storages_.registered_connections["sys"]->get_storage(one->get_plugin_name(), this);
					if (!add_result)
					{
						result.register_error("Unable to initialize plugin storage for "s + one->get_plugin_name() + ".");
						result.register_errors(add_result.errors());
						break;
					}
				}
				if (add_result)
				{
					for (auto& configured : storages_to_create)
					{

						result = init_storage(configured.storage_id, configured.reference);
						if (!result)
							break;
						add_result = storages_.registered_connections[configured.storage_id]->get_storage(configured.name, this);
						if (!add_result)
						{
							result.register_error("Unable to initialize configured storage "s + configured.storage_id + ".");
							result.register_errors(add_result.errors());
							break;
						}
						configured_storage_t to_add(configured);
						to_add.connection = add_result.value();
						storages_.configured_storages.emplace_back(std::move(to_add));
					}
				}
			}
			else
			{
				result.register_error("Unable to initialize host storage.");
				result.register_errors(add_result.errors());
			}
		}
		else
		{
			result.register_error("Error initializing user storage!");
		}
	}
	else
	{
		result.register_error("Error initializing system storage!");
	}

	if (result)
	{
		get_test_storage();

		std::vector<rx_roles_storage_item_ptr> storage_roles;

		for (auto& one : storages_.registered_connections)
			one.second->list_storage_roles(storage_roles);
		result = rx_internal::rx_security::platform_security::instance().initialize_roles(std::move(storage_roles));
		if (!result)
			return result;
	}
	return result;
}

void rx_platform_host::deinitialize_storages ()
{
	for (auto& one : storages_.registered_connections)
		one.second->deinit_connection();
}

rx_result rx_platform_host::init_storage (const string_type& name, const string_type& full_reference)
{
	string_type type;
	string_type reference;
	if (full_reference == "empty")
	{
		auto storage_ptr = rx_create_reference<storage_base::rx_empty_storage_connection>();
		storages_.registered_connections.emplace(name, std::move(storage_ptr));
		std::ostringstream ss;
		ss << "Initialized storage [" << name
			<< "] as empty!";
		HOST_LOG_WARNING("Base", 200, ss.str());

		return true;
	}
	else
	{
		auto result = storage_base::split_storage_reference(full_reference, type, reference);
		if (result)
		{
			auto its = storages_.storage_types.find(type);
			if (its != storages_.storage_types.end())
			{
				auto temp_ptr = storages_.storage_types[type]->construct_storage_connection();
				RX_ASSERT(temp_ptr);
				if (temp_ptr)
				{
					result = temp_ptr->init_connection(reference, this);
					if (result)
					{
						storages_.registered_connections.emplace(name, std::move(temp_ptr));
						if (result)
						{
							std::ostringstream ss;
							ss << "Initialized storage [" << name
								<< "] with reference: " << full_reference;
							HOST_LOG_INFO("Base", 999, ss.str());
						}
					}
				}
				else
				{
					result = "Storage type "s + type + " can not create storage connection.";
				}
			}
			else
			{
				result = type + " is not valid storage type.";
			}
		}
		return result;
	}
}

rx_result rx_platform_host::register_plugins (std::vector<library::rx_plugin_base*>& plugins)
{
	rx_gate::instance().set_host(this);
	static std::vector<std::unique_ptr<library::rx_dynamic_plugin> > dynamic_plugins;
	startup_log_ = rx_create_reference<startup_log_subscriber>();
	rx_platform::log::log_object::instance().register_subscriber(startup_log_);

	string_array libs;
	fill_plugin_libs(libs);
	for (auto& one : libs)
	{
		HOST_LOG_INFO("Base", 999, "Trying to load library "s + one + "...");
		auto one_plugin = std::make_unique< library::rx_dynamic_plugin>(one);
		auto result = one_plugin->load_plugin();
		if (result)
		{
			HOST_LOG_INFO("Base", 999, "Loaded library "s + one);
			plugins.emplace_back(one_plugin.get());
			dynamic_plugins.emplace_back(std::move(one_plugin));
		}
		else
		{
			HOST_LOG_ERROR("Base", 999, "Error loading library "s + one + ". " + result.errors_line());
		}
	}
	rx_platform::api::bind_plugins_dynamic_api();
	rx_result ret = true;
	for (auto one : plugins)
	{
		ret = rx_internal::plugins::plugins_manager::instance().register_plugin(one);
		if (!ret)
		{
			HOST_LOG_ERROR("Base", 999, "Error registering plugin "s + one->get_plugin_name() + ". " + ret.errors_line());
		}
	}
	return true;
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

string_type rx_platform_host::get_host_instance ()
{
	return get_host_name();
}

void rx_platform_host::print_offline_manual (const string_type& host, const rx_host_directories& dirs)
{
	rx_platform::configuration_data_t config;

	auto result = parse_config_files(config);
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
			std::cout << "Error reading file" << result.errors_line();
			return;
		}
	}
	else
	{
		std::cout << "Error reading file" << result.errors_line();
		return;
	}
	std::cout << man;
	std::cout << "\r\n";
}

rx_result_with<rx_storage_ptr> rx_platform_host::get_system_storage (const string_type& name)
{
	return storages_.registered_connections["sys"]->get_storage(name, this);
}

rx_result_with<rx_storage_ptr> rx_platform_host::get_user_storage (const string_type& name)
{
	return storages_.registered_connections["usr"]->get_storage(name, this);
}

rx_result_with<rx_storage_ptr> rx_platform_host::get_test_storage (const string_type& name)
{
	auto it = storages_.registered_connections.find("tst");
	if(it != storages_.registered_connections.end())
		return it->second->get_storage(name, this);
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

void rx_platform_host::dump_storage_references (std::ostream& out)
{
	out << "\r\n";
	for (const auto& one : storages_.registered_connections)
	{
		out << one.first << ":["
			<< one.second->get_storage_info() << "]\r\n"
			<< one.second->get_storage_reference() << "\r\n\r\n";

		auto conns = one.second->get_mounted_storages();
		rx_table_type table(conns.size() + 1);


		table[0].emplace_back("Name");
		table[0].emplace_back("Mount Point");

		size_t idx = 1;
		for (auto& one : conns)
		{
			table[idx].emplace_back(one.first.empty() ? "default" : one.first);
			table[idx].emplace_back(one.second.empty() ? RX_NULL_ITEM_NAME : one.second);
			idx++;
		}

		rx_dump_table(table, out, true, false);

		out << "\r\n\r\n";

	}
}

const string_type& rx_platform_host::get_license ()
{
	static string_type lic_cahce;
	if (lic_cahce.empty())
	{
		sys_handle_t file = rx_file(lic_path_.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
		if (file)
		{
			uint64_t size = 0;
			if (RX_OK == rx_file_get_size(file, &size) && size > 0)
			{
				lic_cahce.assign((size_t)size, ' ');
				if (RX_OK != rx_file_read(file, &lic_cahce[0], (uint32_t)size, nullptr))
				{
					lic_cahce.clear();
				}
			}
			rx_file_close(file);
		}
	}
	return lic_cahce;
}

const string_type& rx_platform_host::get_copyright ()
{
	return copyright_cache_;
}

void rx_platform_host::host_started ()
{
	startup_log_->started();
}

void rx_platform_host::dump_startup_log (std::ostream& out)
{
	std::cout << "Startup log entries:\r\n";

	log::log_query_type query;
	query.count = 10000;
	log::log_events_type events;
	query.type = log::rx_log_error_level;
	if(startup_log_)
		startup_log_->read_log(query, events);

	hosting::rx_platform_host::dump_log_items(events, out);
}

std::vector<std::map<string_type, string_type> > rx_platform_host::read_config_files (const string_type& file_name)
{
	string_array paths{
			rx_combine_paths(host_directories_.local_folder, file_name),
			rx_combine_paths(host_directories_.user_config, file_name),
			rx_combine_paths(host_directories_.system_config, file_name)
	};
	std::vector<std::map<string_type, string_type> > options;
	for (const auto& config_path : paths)
	{
		if (config_path.empty() || !rx_file_exsist(config_path.c_str()))
			continue;

		HOST_LOG_TRACE("rx_host", 500, "Reading configuration file "s + config_path);

		string_type settings_buff;
		rx_source_file file;
		rx_result ret;

		simplified_yaml_reader reader;

		if (file.open(config_path.c_str()))
		{
			ret  = file.read_string(settings_buff);
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
				options.emplace_back(std::move(config_values));
			}
			else
			{
				options.clear();
				break;
			}
		}
		else
		{
			HOST_LOG_ERROR("rx_host", 500, ret.errors_line());
		}
	}
	return options;
}

void rx_platform_host::fill_plugin_libs (string_array& paths)
{
}

std::vector<std::pair<string_type, rx_storage_ptr> > rx_platform_host::get_configured_storages ()
{
	std::vector<std::pair<string_type, rx_storage_ptr> > ret;
	for (auto one : storages_.configured_storages)
	{
		if (one.storage_id.find("world.") == 0)
		{
			string_type path(one.storage_id);
			std::replace_if(path.begin(), path.end(), [](char ch) ->bool { return ch == '.'; }, '/');
			string_type path_full = "/" + std::move(path);
			ret.emplace_back(path_full, one.connection);
		}
		else if (one.storage_id.find("sys.") == 0)
		{
			string_type path(one.storage_id);
			std::replace_if(path.begin(), path.end(), [](char ch) ->bool { return ch == '.'; }, '/');
			string_type path_full = "/" + std::move(path);
			ret.emplace_back(path_full, one.connection);
		}
		else
		{
			;// jbg do nothing
		}
	}
	return ret;
}

rx_result rx_platform_host::register_storage_type (const string_type& prefix, storage_base::rx_platform_storage_type* what)
{
	auto it = storages_.storage_types.find(prefix);
	if (it != storages_.storage_types.end())
		return RX_INVALID_ARGUMENT;

	storages_.storage_types.emplace(prefix, what);
	
	return true;
}

rx_result rx_platform_host::start_log (rx_platform::configuration_data_t& config)
{
	std::vector<log::log_subscriber::smart_ptr> subscribers;
	subscribers.push_back(rx_create_reference< log::cache_log_subscriber>(config.log.cache_size));
	if (!config.log.directory.empty())
	{
		string_type file_path;
		char last = *config.log.directory.rbegin();
		if (last == '/' || last == '\\')
		{
			file_path = config.log.directory.substr(0, config.log.directory.size() - 1);
		}
		else
		{
			file_path = config.log.directory;
		}
		file_path += "/runtime_log.log";
		subscribers.push_back(rx_create_reference<log::file_log_subscriber>(file_path, (log::log_event_type)config.log.log_level));
	}
	auto ret = rx_platform::log::log_object::instance().start(config.log.test_log, subscribers);
	return ret;
}

string_type rx_platform_host::get_default_log_directory () const
{
  return "";

}


// Class rx_platform::hosting::configuration_reader 


// Class rx_platform::hosting::host_platform_builder 


// Class rx_platform::hosting::startup_log_subscriber 


void startup_log_subscriber::log_event (log::log_event_type event_type, const string_type& library, const string_type& source, uint16_t level, const string_type& user, const string_type& code, const string_type& message, rx_time when)
{
	if (started_)
		return;

	log::log_event_data one = { event_type,library,source,level,code,message,when, user };

	locks::auto_lock_t _(&pending_lock_);
	pending_events_.emplace_back(std::move(one));
}

rx_result startup_log_subscriber::read_log (const log::log_query_type& query, log::log_events_type& result)
{
	locks::auto_lock_t _(&pending_lock_);
	for (const auto& one : pending_events_)
	{
		if (one.is_included(query))
			result.data.emplace_back(one);
	}
	result.succeeded = true;
	return true;
}

string_type startup_log_subscriber::get_name () const
{
	return "startup";
}

void startup_log_subscriber::started ()
{
	started_ = true;
}


} // namespace hosting
} // namespace rx_platform

