

/****************************************************************************
*
*  discovery\rx_discovery_connections.cpp
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


// rx_discovery_connections
#include "discovery/rx_discovery_connections.h"

#include "sys_internal/rx_async_functions.h"
#include "model/rx_model_algorithms.h"


namespace rx_internal {

namespace discovery {
const char* instance_keywords[] = {
	"debug",
	"develop",
	"test",
	"conflict"
};

// Class rx_internal::discovery::peer_connections_manager

std::atomic<uint32_t> peer_connections_manager::g_next_id = RX_DISCOVERY_CONNECTION_ID_FIRST;

peer_connections_manager::peer_connections_manager()
{
}


peer_connections_manager::~peer_connections_manager()
{
}



rx_result peer_connections_manager::initialize (hosting::rx_platform_host* host, configuration_data_t& config, const std::vector<peer_endpoint>& peers)
{
	if (host->get_host_instance() != config.instance.name)
		my_instance_ = config.instance.name;

	for (auto&& one : peers)
	{
		//TODO
		//string_type name = one.peer_name;
		//auto peer_ptr = rx_create_reference<peer_connection>(std::move(one));
		//rx_result result = peer_ptr->build();
		//if (result)
		//{
		//	//connections_.emplace(std::move(name), std::move(peer_ptr));
		//}
		//else
		//{
		//	std::ostringstream ss;
		//	ss << "Error building peer connection <"
		//		<< name
		//		<< "> : "
		//		<< result.errors_line();
		//	DISCOVERY_LOG_ERROR("", 900, ss.str());
		//}
	}
	return true;
}

rx_result peer_connections_manager::start (hosting::rx_platform_host* host, const configuration_data_t& config)
{
	return true;
}

void peer_connections_manager::stop ()
{
}

void peer_connections_manager::deinitialize ()
{
}

void peer_connections_manager::peer_registered(const rx_uuid& id, string_view_type instance, string_view_type node, const io::ip4_address& from)
{
	if (is_peer(instance, node, from))
	{
		rx_post_function_to(RX_DOMAIN_META, rx_reference_ptr(), [this](rx_uuid id, string_type instance, string_type node, io::ip4_address from)
			{
				string_type name = instance + "@" + node;
				auto peer_ptr = rx_create_reference<peer_connection>(name, from);
				{
					std::scoped_lock _(connections_lock_);
					auto it = connections_.find(id);
					if (it != connections_.end())
						return;// this is over

					peer_connection_data new_data;
					new_data.instance = instance;
					new_data.node = node;
					new_data.connection = peer_ptr;
					connections_.emplace(id, std::move(new_data));
				}
				rx_result result = peer_ptr->build(id);
				if (result)
				{
					std::ostringstream ss;
					ss << "Built peer connection objects <"
						<< name;
					DISCOVERY_LOG_INFO("", 900, ss.str());
				}
				else
				{
					std::ostringstream ss;
					ss << "Error building peer connection <"
						<< name
						<< "> : "
						<< result.errors_line();
					DISCOVERY_LOG_ERROR("", 900, ss.str());
				}

			}, id, string_type(instance), string_type(node), from);
	}
}

void peer_connections_manager::peer_unregistered(const rx_uuid& id)
{
	rx_post_function_to(RX_DOMAIN_META, rx_reference_ptr(), [this](rx_uuid id)
		{
			string_type name;
			peer_connection_ptr peer;
			{
				std::scoped_lock _(connections_lock_);
				auto it = connections_.find(id);
				if (it == connections_.end())
					return;// this is over
				peer = it->second.connection;
				name = it->second.instance + "@" + it->second.node;

				connections_.erase(it);
			}

			auto result = peer->destroy();
			if (result)
			{
				std::ostringstream ss;
				ss << "Peer connection objects destroyed <"
					<< name;
				DISCOVERY_LOG_INFO("", 900, ss.str());
			}
			else
			{
				std::ostringstream ss;
				ss << "Error building peer connection <"
					<< name
					<< "> : "
					<< result.errors_line();
				DISCOVERY_LOG_ERROR("", 900, ss.str());
			}

		}, id);
}

peer_connection_ptr peer_connections_manager::get_peer(const rx_uuid& id)
{
	auto it = connections_.find(id);
	if (it != connections_.end())
		return it->second.connection;
	else
		return peer_connection_ptr();
}

bool peer_connections_manager::is_peer(string_view_type instance, string_view_type node, const io::ip4_address& from)
{
	if (!my_instance_.empty())
	{
		// check for special instance names
		for (size_t i = 0; i < sizeof(instance_keywords) / sizeof(instance_keywords[0]); i++)
		{
			if(instance_keywords[i]==instance)
				return false;
		}
		if(instance.size() > my_instance_.size()
			&& instance.substr(instance.size() - my_instance_.size()) == my_instance_
			|| instance.size() > my_instance_.size()
			&& instance == my_instance_)
			return true;
	}
	return false;
}


} // namespace discovery
} // namespace rx_internal

