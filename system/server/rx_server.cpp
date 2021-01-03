

/****************************************************************************
*
*  system\server\rx_server.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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

#include "lib/rx_lib_version.h"

// rx_cmds
#include "system/server/rx_cmds.h"
// rx_endpoints
#include "interfaces/rx_endpoints.h"
// rx_server
#include "system/server/rx_server.h"

#include "sys_internal/rx_internal_ns.h"
#include "sys_internal/rx_internal_builders.h"
#include "system/python/py_support.h"
#include "api/rx_meta_api.h"
#include "model/rx_meta_internals.h"
#include "interfaces/rx_endpoints.h"
#include "sys_internal/rx_inf.h"
#include "sys_internal/rx_security/rx_platform_security.h"
#include "sys_internal/rx_async_functions.h"


namespace rx_platform {

// Class rx_platform::rx_gate 

rx_gate* rx_gate::g_instance = nullptr;

rx_gate::rx_gate()
      : host_(nullptr),
        started_(rx_time::now()),
        pid_(0),
        security_guard_(std::make_unique<security::security_guard>()),
        shutting_down_(false),
        platform_status_(rx_platform_status::initializing)
{
	char buff[0x100];
	rx_collect_system_info(buff, 0x100);
	os_info_ = buff;
	pid_ = rx_pid;
	{
		ASSIGN_MODULE_VERSION(rx_version_, RX_SERVER_NAME, RX_SERVER_MAJOR_VERSION, RX_SERVER_MINOR_VERSION, RX_SERVER_BUILD_NUMBER);
	}
	auto sname = rx_get_server_name();
	if(sname)
		rx_name_ = sname;
	lib_version_ = g_lib_version;
	rx_init_hal_version();
	hal_version_ = g_ositf_version;	

	sprintf(buff, "%s %d.%d.%d",
		RX_COMPILER_NAME,
		RX_COMPILER_VERSION,
		RX_COMPILER_MINOR,
		RX_COMPILER_BUILD);
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

void rx_gate::cleanup ()
{
	RX_ASSERT(g_instance);
	delete g_instance;
	g_instance = nullptr;
}

rx_result_with<security::security_context_ptr> rx_gate::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
#ifdef PYTHON_SUPPORT
	python::py_script* python = &python::py_script::instance();
	scripts_.emplace(python->get_definition().name, python);
#endif	
	host_ = host;
	rx_name_ = data.meta_configuration.instance_name.empty() ? host_->get_default_name() : data.meta_configuration.instance_name;

	security::security_context_ptr host_ctx = rx_create_reference<rx_internal::rx_security::host_security_context>(host_->get_host_name(), rx_name_);
	auto result = host_ctx->login();
	if (result)
	{
		result = rx_internal::infrastructure::server_runtime::instance().initialize(host, data.processor, data.io);
		if (result)
		{
			result = io_manager_->initialize(host, data.io);
			if (result)
			{
				auto root = rx_create_reference<rx_internal::internal_ns::platform_root>();
				root_ = root;
				auto build_result = rx_internal::builders::rx_platform_builder::build_platform(host, data.storage, data.meta_configuration, root);

				if (build_result)
				{
					for (auto one : scripts_)
						one.second->initialize();

					result = rx_internal::model::platform_types_manager::instance().initialize(host, data.meta_configuration);
					if (!result)
						result.register_error("Error initializing platform types manager!");
				}
				else
				{
					result = build_result.errors();
					result.register_error("Error building platform!");
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
	}
	else
	{
		result.register_error("Error impersonating host!");
	}
	if (result)
	{
		platform_status_ = rx_platform_status::starting;
		return host_ctx;
	}
	else
	{
		return result.errors();
	}
}

rx_result rx_gate::deinitialize (security::security_context_ptr sec_ctx)
{
	
	for (auto one : scripts_)
		one.second->deinitialize();

	rx_internal::model::platform_types_manager::instance().deinitialize();

	io_manager_->deinitialize();
	rx_internal::infrastructure::server_runtime::instance().deinitialize();

	sec_ctx->logout();

	return RX_OK;
}

rx_result rx_gate::start (hosting::rx_platform_host* host, const configuration_data_t& data)
{
	auto result = rx_internal::infrastructure::server_runtime::instance().start(host, data.processor, data.io);
	if (result)
	{
		result = rx_internal::model::platform_types_manager::instance().start(host, data.meta_configuration);
		if (result)
		{
			platform_status_ = rx_platform_status::running;
			host->server_started_event();
			return true;
		}
		else
		{
			rx_internal::infrastructure::server_runtime::instance().stop();
			result.register_error("Error starting platform types manager!");
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
	rx_internal::model::platform_types_manager::instance().stop();
	rx_internal::infrastructure::server_runtime::instance().stop();
	platform_status_ = rx_platform_status::deinitializing;
	return true;
}

rx_directory_ptr rx_gate::get_root_directory ()
{
	return root_;
}

bool rx_gate::shutdown (const string_type& msg)
{
	if (!security_guard_->check_premissions(security::rx_security_delete_access, security::rx_security_ext_null))
	{
		return false;
	}
	shutting_down_ = true;
	host_->shutdown(msg);
	return true;
}

bool rx_gate::read_log (const string_type& log, const log::log_query_type& query, std::function<void(rx_result_with<log::log_events_type>&&)> callback)
{
	auto current_executer = rx_thread_context();
	return log::log_object::instance().read_log(log, query, [current_executer, callback](rx_result_with<log::log_events_type>&& result)
		{
			rx_post_function_to(current_executer, rx_reference_ptr(), callback, std::move(result));
		});
}

bool rx_gate::do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx)
{
	if (!security_guard_->check_premissions(security::rx_security_execute_access, security::rx_security_ext_null))
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

template <class typeT>
rx_result rx_gate::register_constructor(const rx_node_id& id, std::function<typename typeT::RImplPtr()> f)
{
	if (platform_status_ == rx_platform_status::initializing)
		return rx_internal::model::platform_types_manager::instance().get_type_repository<typeT>().register_constructor(id, f);
	else
		return "Wrong platform status for constructor registration!";
}

template <class typeT>
rx_result rx_gate::register_constructor_internal(const rx_node_id& id, std::function<typename typeT::RImplPtr()> f)
{
	if (platform_status_ == rx_platform_status::initializing)
		return rx_internal::model::platform_types_manager::instance().get_type_repository<typeT>().register_constructor(id, f);
	else
		return "Wrong platform status for constructor registration!";
}

template rx_result rx_gate::register_constructor<object_type>(const rx_node_id& id, std::function<object_type::RImplPtr()> f);
template rx_result rx_gate::register_constructor<port_type>(const rx_node_id& id, std::function<port_type::RImplPtr()> f);
template rx_result rx_gate::register_constructor<domain_type>(const rx_node_id& id, std::function<domain_type::RImplPtr()> f);
template rx_result rx_gate::register_constructor<application_type>(const rx_node_id& id, std::function<application_type::RImplPtr()> f);
} // namespace rx_platform

