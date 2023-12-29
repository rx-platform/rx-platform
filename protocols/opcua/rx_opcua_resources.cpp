

/****************************************************************************
*
*  protocols\opcua\rx_opcua_resources.cpp
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


// rx_opcua_resources
#include "protocols/opcua/rx_opcua_resources.h"

#include "rx_opcua_requests.h"


namespace protocols {

namespace opcua {

// Class protocols::opcua::opcua_resources_repository 


opcua_resources_repository& opcua_resources_repository::instance ()
{
	static opcua_resources_repository g_obj;
	return g_obj;
}

rx_result opcua_resources_repository::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
	requests::opcua_requests_repository::instance().init_requests();
	requests::opcua_requests_repository::instance().init_responses();
	return true;
}

void opcua_resources_repository::deinitialize ()
{
}

rx_result opcua_resources_repository::register_server (const string_type& port_path, const application_description& data)
{
	opcua_server_endpoint_data storage_data;
	storage_data.description = data;

	locks::auto_lock_t _(&resources_lock_);
	auto it_ports = server_endpoints_.find(port_path);
	if (it_ports == server_endpoints_.end())
	{
		return RX_INVALID_PATH;
	}
	else
	{
		auto it_ep = it_ports->second.second.find(data.ep_bind);
		if (it_ep == it_ports->second.second.end())
		{
			it_ports->second.second.emplace(data.ep_bind, std::move(storage_data));
			return true;
		}
		else
		{
			return RX_ALREADY_CONNECTED;
		}
	}
}

void opcua_resources_repository::unregister_server (const string_type& port_path, const string_type& path)
{
	locks::auto_lock_t _(&resources_lock_);
	auto it_ports = server_endpoints_.find(port_path);
	if (it_ports != server_endpoints_.end())
	{
		it_ports->second.second.erase(path);
	}
}

application_descriptions_type opcua_resources_repository::find_servers (const string_type& ep_url, const string_type& port_path)
{
	application_descriptions_type apps;
	{
		locks::auto_lock_t _(&resources_lock_);
		auto it_port = server_endpoints_.find(port_path);
		if (it_port != server_endpoints_.end())
		{
			for (const auto& ep : it_port->second.second)
			{
				application_description temp(ep.second.description);
				apps.push_back(std::move(temp));
			}
		}
	}
	for (auto& app : apps)
	{
		app.discovery_urls.push_back(ep_url);
	}
	return apps;
}

rx_result opcua_resources_repository::register_endpoint (const string_type& port_path, registered_endpoint_data data)
{
	locks::auto_lock_t _(&resources_lock_);
	auto it_ports = server_endpoints_.find(port_path);
	if (it_ports == server_endpoints_.end())
	{
		server_endpoints_[port_path].first = std::move(data);
		return true;
	}
	return RX_ALREADY_CONNECTED;
}

void opcua_resources_repository::unregister_endpoint (const string_type& port_path)
{
	locks::auto_lock_t _(&resources_lock_);
	auto it_ports = server_endpoints_.find(port_path);
	if (it_ports != server_endpoints_.end())
	{
		server_endpoints_.erase(it_ports);
	}
}

void opcua_resources_repository::fill_endpoint_description (endpoint_description& descr, const string_type& port_path)
{
	locks::auto_lock_t _(&resources_lock_);
	auto it_ports = server_endpoints_.find(port_path);
	if (it_ports != server_endpoints_.end())
	{
		descr.certificate = it_ports->second.first.certificate;
		descr.security_mode = it_ports->second.first.security_mode;
		descr.policy_uri = it_ports->second.first.policy_uri;
		descr.security_level = it_ports->second.first.security_level;
		descr.transport_profile_uri = it_ports->second.first.transport_profile_uri;
	}
	else
	{
		descr.security_mode = security_mode_t::none;
		descr.policy_uri = "http://opcfoundation.org/UA/SecurityPolicy#None";
		descr.security_level = 0;
		descr.transport_profile_uri = "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";
	}
}


// Class protocols::opcua::opcua_security_channel 


// Class protocols::opcua::opcua_session 


} // namespace opcua
} // namespace protocols

