

/****************************************************************************
*
*  system\server\rx_server.cpp
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

#include "lib/rx_lib_version.h"

// rx_cmds
#include "system/server/rx_cmds.h"
// rx_endpoints
#include "interfaces/rx_endpoints.h"
// rx_server
#include "system/server/rx_server.h"

#include "rx_platform_version.h"
#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_internal_builders.h"
#include "system/python/py_support.h"
#include "api/rx_meta_api.h"
#include "model/rx_meta_internals.h"
#include "interfaces/rx_endpoints.h"
#include "sys_internal/rx_inf.h"
#include "sys_internal/rx_security/rx_platform_security.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "sys_internal/rx_async_functions.h"
#include "http_server/rx_http_server.h"
#include "system/server/rx_directory_cache.h"
#include "http_server/rx_http_server.h"
#include "upython/upython.h"
#include "discovery/rx_discovery_main.h"
#include "enterprise/rx_enterprise.h"
#include "system/runtime/rx_internal_objects.h"


namespace rx_platform {

// Class rx_platform::rx_gate 

rx_gate* rx_gate::g_instance = nullptr;

rx_gate::rx_gate()
      : host_(nullptr),
        started_(rx_time::now()),
        pid_(0),
        security_guard_(rx_create_reference<security::security_guard>(security::rx_security_read_access | security::rx_security_delete_access, "@gate")),
        shutting_down_(false),
        platform_status_(rx_platform_status::initializing)
{
	char buff[0x100];
	rx_collect_system_info(buff, 0x100);
	os_info_ = buff;
	pid_ = rx_pid;
	{
		buff[0] = '\0';
		ASSIGN_MODULE_VERSION(buff, RX_SERVER_NAME, RX_SERVER_MAJOR_VERSION, RX_SERVER_MINOR_VERSION, RX_SERVER_BUILD_NUMBER);
		rx_version_ = buff;
	}
	node_name_ = rx_get_node_name();

	lib_version_ = g_lib_version;
	abi_version_ = g_abi_version;
	common_version_ = g_common_version;
	rx_init_hal_version();
	hal_version_ = g_ositf_version;

	sprintf(buff, "%s %d.%d.%d",
		RX_COMPILER_NAME,
		RX_COMPILER_VERSION,
		RX_COMPILER_MINOR,
		RX_COMPILER_BUILD);

#ifdef _DEBUG
	strcat(buff, " [DEBUG]");
#endif
	comp_version_ = buff;

	// create io manager instance
	io_manager_ = std::make_unique<rx_internal::interfaces::io_endpoints::rx_io_manager>();
}


rx_gate::~rx_gate()
{
}



rx_gate& rx_gate::instance ()
{
	if (g_instance == nullptr)
		g_instance = new rx_gate();
	return *g_instance;
}

void rx_gate::set_host (hosting::rx_platform_host* host)
{
	host_ = host;
}

void rx_gate::cleanup ()
{
	ns::rx_directory_cache::instance().clear_cache();
	RX_ASSERT(g_instance);
	delete g_instance;
	g_instance = nullptr;
}

rx_result rx_gate::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
	configuration_ = data;
	rx_internal::discovery::discovery_manager::instance();// create discovery instance!!!!
#ifdef PYTHON_SUPPORT
	python::py_script* python = &python::py_script::instance();
	scripts_.emplace(python->get_definition().name, python);
#endif
	RX_ASSERT(host_ = host);
	host_ = host;
	rx_name_ = data.instance.name.empty() ? node_name_ : data.instance.name;

	auto result = rx_internal::infrastructure::server_runtime::instance().initialize(host, data);
	if (result)
	{
		result = io_manager_->initialize(host, data);
		if (result)
		{
			result = rx_internal::enterprise::enterprise_manager::instance().init_interfaces(data);
			if (!result)
			{
				result.register_error("Error initializing enterprise manager!");
				rx_internal::enterprise::enterprise_manager::instance().deinit_interfaces();
			}
			else
			{
				result = rx_internal::rx_http_server::http_server::instance().initialize(host, data);
				if (!result)
				{
					result.register_error("Error initializing http server!");
					rx_internal::rx_http_server::http_server::instance().deinitialize();
				}
				else
				{
					auto build_result = rx_internal::builders::rx_platform_builder::build_platform(host, data);

					if (build_result)
					{
						for (auto one : scripts_)
							one.second->initialize();

						result = rx_internal::discovery::discovery_manager::instance().initialize(host, data);
						if (result)
						{
							init_data_ = std::make_unique<initialize_data_type>();
							result = rx_internal::model::platform_types_manager::instance().initialize(host, data, *init_data_);
							if (!result)
							{
								result.register_error("Error initializing platform types manager!");
							}
						}
						else
						{
							result.register_error("Error initializing discovery manager!");
						}


					}
					else
					{
						result = build_result.errors();
						result.register_error("Error building platform!");
					}
				}
			}
			if (!result)
			{
				io_manager_->deinitialize();
				rx_internal::infrastructure::server_runtime::instance().deinitialize();
				io_manager_->deinitialize();
			}
		}
		else
		{
			rx_internal::infrastructure::server_runtime::instance().deinitialize();
			result.register_error("Error initializing I/O manager!");
		}
	}
	else
	{
		result.register_error("Error initializing platform runtime!");
	}

	if (result)
	{
		platform_status_ = rx_platform_status::starting;
		return true;
	}
	else
	{
		return result.errors();
	}
}

rx_result rx_gate::deinitialize ()
{

	for (auto one : scripts_)
		one.second->deinitialize();

	rx_internal::rx_http_server::http_server::instance().deinitialize();

	rx_internal::enterprise::enterprise_manager::instance().deinit_interfaces(); 
	
	rx_internal::discovery::discovery_manager::instance().deinitialize();

	rx_internal::model::platform_types_manager::instance().deinitialize();

	rx_internal::builders::rx_platform_builder::deinitialize();

	io_manager_->deinitialize();
	rx_internal::infrastructure::server_runtime::instance().deinitialize();

	cleanup();

	return RX_OK;
}

rx_result rx_gate::start (hosting::rx_platform_host* host, const configuration_data_t& data)
{

	security::secured_scope _(rx_platform::sys_objects::system_application::instance()->system_identity);
	auto result = rx_internal::infrastructure::server_runtime::instance().start(host, data.processor, data.io);
	if (result)
	{

#ifdef UPYTHON_SUPPORT
		result = python::upython::start_script(host, data);
#endif
		result = rx_internal::discovery::discovery_manager::instance().start(host, data);
		if (result)
		{
			if (init_data_)
			{
				network_ = rx_internal::discovery::discovery_manager::instance().get_ip4_network().to_string();
				network_id_ = rx_internal::discovery::discovery_manager::instance().get_network_id();
			
				result = rx_internal::model::platform_types_manager::instance().start(host, data, *init_data_);
				if (result)
				{
					init_data_.reset();// we do not need this anymore
					platform_status_ = rx_platform_status::running;
					host->server_started_event();
					return true;
				}
				else
				{
					rx_internal::discovery::discovery_manager::instance().stop();
					rx_internal::infrastructure::server_runtime::instance().stop();
					result.register_error("Error starting platform types manager!");
				}
			}
			else
			{
				result.register_error("Platform not started!");
			}
		}
		else
		{
			rx_internal::discovery::discovery_manager::instance().stop();
			rx_internal::infrastructure::server_runtime::instance().stop();
			result.register_error("Error starting discovery manager!");
		}
	}
	else
	{
		result.register_error("Error starting platform runtime!");
	}
	return result;
}

rx_result rx_gate::stop ()
{
	platform_status_ = rx_platform_status::stopping; 
	rx_internal::sys_runtime::platform_runtime_manager::instance().stop_all();
	rx_internal::model::platform_types_manager::instance().stop();
#ifdef UPYTHON_SUPPORT
	python::upython::stop_script();
#endif
	rx_internal::discovery::discovery_manager::instance().stop();
	rx_internal::infrastructure::server_runtime::instance().stop();
	platform_status_ = rx_platform_status::deinitializing;
	return true;
}

rx_directory_ptr rx_gate::get_directory (const string_type& path, ns::rx_directory_resolver* dirs)
{
	if (dirs != nullptr)
		return dirs->resolve_directory(path);
	else
		return ns::rx_directory_cache::instance().get_directory(path);
}

ns::rx_namespace_item rx_gate::get_namespace_item (const string_type& path, ns::rx_directory_resolver* dirs)
{
	if (dirs != nullptr)
		return dirs->resolve_item(path);
	else
	{
		string_type name;
		string_type dir_path;
		rx_split_path(path, dir_path, name);
		auto dir = ns::rx_directory_cache::instance().get_directory(dir_path);
		if (!dir)
			return ns::rx_namespace_item();
		else
			return dir->get_item(name);
	}
}

rx_result_with<rx_directory_ptr> rx_gate::add_directory (const string_type& path)
{
	return ns::rx_directory_cache::instance().add_directory(path);
}

bool rx_gate::shutdown (const string_type& msg)
{
	if (!security_guard_->check_permission(security::rx_security_delete_access))
	{
		return false;
	}
	shutting_down_ = true;
	std::ostringstream ss;
	ss << "Shutdown initiated with message ";
	ss << msg;
	HOST_LOG_INFO("rx_gate", 999, ss.str());
	host_->shutdown(msg);
	return true;
}

bool rx_gate::read_log (const string_type& log, const log::log_query_type& query, std::function<void(rx_result_with<log::log_events_type>&&)> callback)
{
	auto current_executer = rx_thread_context();
	return log::log_object::instance().read_log(log, query, [current_executer, callback = std::move(callback)](rx_result_with<log::log_events_type>&& result)
		{
			rx_post_function_to(current_executer, rx_reference_ptr(), std::move(callback), std::move(result));
		});
}

bool rx_gate::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	if (security_guard_->check_permission(security::rx_security_execute_access))
	{
		return host_->do_host_command(line, out_buffer, err_buffer, ctx);
	}
	else
	{
		err_buffer->push_line(ANSI_COLOR_RED RX_ACCESS_DENIED ANSI_COLOR_RESET "\r\n");
		return false;
	}
}

runtime_data_t rx_gate::get_cpu_data ()
{
	return rx_internal::infrastructure::server_runtime::instance().get_cpu_data();
}

string_type rx_gate::resolve_ip4_alias (const string_type& what) const
{
	return io_manager_->resolve_ip4_alias(what);
}

string_type rx_gate::resolve_ip6_alias (const string_type& what) const
{
	return what;
}

string_type rx_gate::resolve_serial_alias (const string_type& what) const
{
	return io_manager_->resolve_serial_alias(what);
}

string_type rx_gate::resolve_ethernet_alias (const string_type& what) const
{
	return io_manager_->resolve_ethernet_alias(what);
}

runtime_transaction_id_t rx_gate::get_new_unique_id ()
{
	return rx_internal::infrastructure::server_runtime::instance().get_ids_manager().get_new_unique_id();
	/*auto anchor = callback.get_anchor();

	auto storage = this->host_->get_user_storage();
	if (!storage)
		return false;

	rx_post_function_to(RX_DOMAIN_SLOW, anchor
		, [storage= storage.move_value()]  (callback::rx_any_callback<runtime_transaction_id_t>&& callback) mutable
		{
			auto ret = storage->get_new_unique_id();

			callback(std::move(ret));

		}, std::move(callback));

	return true;*/
}


} // namespace rx_platform

