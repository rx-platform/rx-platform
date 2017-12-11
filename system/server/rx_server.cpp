

/****************************************************************************
*
*  system\server\rx_server.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "stdafx.h"

#include "lib/rx_lib_version.h"

// rx_server
#include "system/server/rx_server.h"

#include "sys_internal/rx_internal_ns.h"
#include "system/python/py_support.h"
#include "classes/rx_meta.h"


namespace rx_platform {

// Class rx_platform::rx_gate 

rx_gate* rx_gate::g_instance = nullptr;

rx_gate::rx_gate()
      : _host(nullptr),
        _started(rx_time::now()),
        _pid(0),
        _shutting_down(false)
	, _security_guard(pointers::_create_new)
{
	char buff[0x100];
	rx_collect_system_info(buff, 0x100);
	_os_info = buff;


	_pid = rx_pid;

	{
		ASSIGN_MODULE_VERSION(_rx_version, RX_SERVER_NAME, RX_SERVER_MAJOR_VERSION, RX_SERVER_MINOR_VERSION, RX_SERVER_BUILD_NUMBER);
	}
	_rx_name = rx_get_server_name();
	_lib_version = g_lib_version;
	_hal_version = g_ositf_version;
	
	

	sprintf(buff, "%s %d.%d.%d",
		RX_COMPILER_NAME,
		RX_COMPILER_VERSION,
		RX_COMPILER_MINOR,
		RX_COMPILER_BUILD);
	_comp_version = buff;

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

uint32_t rx_gate::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
	python::py_script* python = &python::py_script::instance();
	_scripts.emplace(python->get_definition().name, python);
	
	_host = host;

	if (_runtime.initialize(host, data.runtime_data))
	{
		if (_manager.initialize(host, data.managment_data))
		{
			sys_internal::internal_ns::root_server_directory::initialize(host,data.namespace_data);
			_root = rx_create_reference<sys_internal::internal_ns::root_server_directory>();

			for (auto one : _scripts)
				one.second->initialize();

			model::internal_classes_manager::instance().initialize(host, data.meta_data);
			
			return RX_OK;
		}
		else
		{
			_runtime.deinitialize();
		}
	}
	return RX_ERROR;
}

uint32_t rx_gate::deinitialize ()
{
	
	for (auto one : _scripts)
		one.second->deinitialize();

	model::internal_classes_manager::instance().deinitialize();

	_manager.deinitialize();
	_runtime.deinitialize();
	return RX_OK;
}

uint32_t rx_gate::start (hosting::rx_platform_host* host, const configuration_data_t& data)
{
	if (_runtime.start(host, data.runtime_data))
	{
		if (_manager.start(host, data.managment_data))
		{
			model::internal_classes_manager::instance().start(host, data.meta_data);

			host->server_started_event();

			return RX_OK;
		}
		else
		{
			_runtime.stop();
		}
	}
	return RX_ERROR;
}

uint32_t rx_gate::stop ()
{
	model::internal_classes_manager::instance().stop();
	_manager.stop();
	_runtime.stop();
	return RX_OK;
}

server_directory_ptr rx_gate::get_root_directory ()
{
	return _root;
}

bool rx_gate::shutdown (const string_type& msg)
{
	if (!_security_guard->check_premissions(security::rx_security_delete_access, security::rx_security_ext_null))
	{
		return false;
	}
	_shutting_down = true;
	_host->shutdown(msg);
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
	if (!_security_guard->check_premissions(security::rx_security_execute_access, security::rx_security_ext_null))
	{
		return _host->do_host_command(line, out_buffer, err_buffer, ctx);
	}
	else
	{
		err_buffer->push_line(ANSI_COLOR_RED RX_ACCESS_DENIED ANSI_COLOR_RESET "\r\n");
		return false;
	}
}


} // namespace rx_platform

