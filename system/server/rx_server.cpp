

/****************************************************************************
*
*  system\server\rx_server.cpp
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

#include "lib/rx_lib_version.h"

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


namespace rx_platform {

rx_domain_ptr rx_system_domain()
{
	return rx_gate::instance().get_manager().get_system_domain();
}
rx_application_ptr rx_system_application()
{
	return rx_gate::instance().get_manager().get_system_app();
}

rx_domain_ptr rx_unassigned_domain()
{
	return rx_gate::instance().get_manager().get_unassigned_domain();
}
rx_application_ptr rx_unassigned_application()
{
	return rx_gate::instance().get_manager().get_unassigned_app();
}

// Class rx_platform::rx_gate 

rx_gate* rx_gate::g_instance = nullptr;

rx_gate::rx_gate()
      : host_(nullptr),
        started_(rx_time::now()),
        pid_(0),
        security_guard_(std::make_unique<security::security_guard>()),
        shutting_down_(false),
        platform_status_(rx_platform_initializing)
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
	io_manager_ = std::make_unique<interfaces::io_endpoints::rx_io_manager>();
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

rx_result rx_gate::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
#ifdef PYTHON_SUPPORT
	python::py_script* python = &python::py_script::instance();
	scripts_.emplace(python->get_definition().name, python);
#endif	
	host_ = host;
	rx_name_ = data.meta_configuration_data.instance_name.empty() ? host_->get_default_name() : data.meta_configuration_data.instance_name;

	auto result = infrastructure_.initialize(host, data.runtime_data);
	if (result)
	{
		result = manager_.initialize(host, data.managment_data);
		if (result)
		{
			result = io_manager_->initialize(host, data.io_manager_data);
			if (result)
			{
				auto build_result =	sys_internal::builders::rx_platform_builder::buid_platform(host, data.namespace_data);

				if (build_result)
				{
					root_ = build_result.value();

					for (auto one : scripts_)
						one.second->initialize();

					result = model::platform_types_manager::instance().initialize(host, data.meta_configuration_data);
					if(!result)
						result.register_error("Error initializing platform types manager!");
				}
				else
				{
					result = build_result.errors();
					result.register_error("Error building platform!");
				}
				if(!result)
				{
					io_manager_->deinitialize();
					manager_.deinitialize();
					infrastructure_.deinitialize();
					io_manager_->deinitialize();
				}
			}
			else
			{
				manager_.deinitialize();
				infrastructure_.deinitialize();
				result.register_error("Error initializing I/O manager!");
			}
		}
		else
		{
			infrastructure_.deinitialize();
			result.register_error("Error initializing platform manager!");
		}
	}
	else
	{
		result.register_error("Error initializing platform runtime!");
	}
	if (result)
		platform_status_ = rx_platform_starting;
	return result;
}

rx_result rx_gate::deinitialize ()
{
	
	for (auto one : scripts_)
		one.second->deinitialize();

	model::platform_types_manager::instance().deinitialize();

	io_manager_->deinitialize();
	manager_.deinitialize();
	infrastructure_.deinitialize();
	return RX_OK;
}

rx_result rx_gate::start (hosting::rx_platform_host* host, const configuration_data_t& data)
{
	auto result = infrastructure_.start(host, data.runtime_data);
	if (result)
	{
		result = manager_.start(host, data.managment_data);
		if (result)
		{
			result = model::platform_types_manager::instance().start(host, data.meta_configuration_data);
			if (result)
			{
				platform_status_ = rx_platform_running;
				host->server_started_event();
				return true;
			}
			else
			{
				infrastructure_.stop();
				manager_.stop();
				result.register_error("Error starting platform types manager!");
			}
		}
		else
		{
			infrastructure_.stop();
			result.register_error("Error starting platform manager!");
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
	platform_status_ = rx_platform_stopping;
	model::platform_types_manager::instance().stop();
	manager_.stop();
	infrastructure_.stop();
	platform_status_ = rx_platform_deinitializing;
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

void rx_gate::interface_bind ()
{
}

void rx_gate::interface_release ()
{
}

bool rx_gate::read_log (const log::log_query_type& query, log::log_events_type& result)
{
	return log::log_object::instance().read_cache(query, result);
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


} // namespace rx_platform

