

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


namespace server {

// Class server::rx_server 

rx_server* rx_server::g_instance = nullptr;

rx_server::rx_server()
      : _host(nullptr),
        _started(rx_time::now())
{
	char buff[0x100];
	rx_collect_system_info(buff, 0x100);
	_os_info = buff;

	_rx_version =  " Ver ";
	sprintf(buff,"%s Ver %d.%d.%d", RX_SERVER_NAME, RX_SERVER_MAJOR_VERSION, RX_SERVER_MINOR_VERSION, RX_SERVER_BUILD_NUMBER);
	{
		ASSIGN_MODULE_VERSION(_rx_version, buff);
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


rx_server::~rx_server()
{
}



rx_server& rx_server::instance ()
{
	if (g_instance == nullptr)
		g_instance = new rx_server();
	return *g_instance;
}

void rx_server::cleanup ()
{
	RX_ASSERT(g_instance);
	delete g_instance;
	g_instance = nullptr;
}

dword rx_server::initialize (host::rx_server_host* host, configuration_data_t& data)
{
	python::py_script* python = &python::py_script::instance();
	_scripts.emplace(python->get_definition().name, python);
	
	_host = host;
	_host_info = host->get_host_info();

	if (_runtime.initialize(host, data.runtime_data))
	{
		if (_manager.initialize(host, data.managment_data))
		{
			sys_internal::internal_ns::root_server_directory::initialize(host,data.namespace_data);
			_root = sys_internal::internal_ns::root_server_directory::smart_ptr(pointers::_create_new);

			for (auto one : _scripts)
				one.second->initialize();
			
			return RX_OK;
		}
		else
		{
			_runtime.deinitialize();
		}
	}
	return RX_ERROR;
}

dword rx_server::deinitialize ()
{
	
	for (auto one : _scripts)
		one.second->deinitialize();

	_manager.deinitialize();
	_runtime.deinitialize();
	return RX_OK;
}

dword rx_server::start (host::rx_server_host* host, const configuration_data_t& data)
{
	if (_runtime.start(host, data.runtime_data))
	{
		if (_manager.start(host, data.managment_data))
		{
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

dword rx_server::stop ()
{
	_manager.stop();
	_runtime.stop();
	return RX_OK;
}

server_directory_ptr rx_server::get_root_directory ()
{
	return _root;
}

bool rx_server::shutdown (const string_type& msg, std::ostream& err)
{
	if (!dword_check_premissions(0, 0))
	{
		err << "Access Denied!";
		return false;
	}
	_host->shutdown(msg);
	return true;
}

void rx_server::interface_bind ()
{
}

void rx_server::interface_release ()
{
}

bool rx_server::read_log (const log::log_query_type& query, log::log_events_type& result)
{
	return log::log_object::instance().read_cache(query, result);
}


} // namespace server

