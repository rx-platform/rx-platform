

/****************************************************************************
*
*  discovery\rx_discovery_algorithm.cpp
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


// rx_discovery_main
#include "discovery/rx_discovery_main.h"
// rx_discovery_algorithm
#include "discovery/rx_discovery_algorithm.h"

#include "rx_discovery_transactions.h"
#include "system/runtime/rx_holder_algorithms.h"


namespace rx_internal {

namespace discovery {
bool should_be_cached(rx_item_type type)
{
	return type != rx_directory;
}

// Class rx_internal::discovery::peer_directory_algorithm 

peer_directory_algorithm::peer_directory_algorithm()
{
}


peer_directory_algorithm::~peer_directory_algorithm()
{
}



rx_result peer_directory_algorithm::start_algorithm (runtime::runtime_start_context& ctx, rx_object_impl_ptr obj, peer_connection_ptr whose)
{
	return true;
}

rx_result peer_directory_algorithm::stop_algorithm (runtime::runtime_stop_context& ctx, peer_connection_ptr whose)
{
	return true;
}

void peer_directory_algorithm::client_connected (peer_connection_ptr whose)
{
	do_browse(whose, "/world");
}

void peer_directory_algorithm::client_disconnected (peer_connection_ptr whose)
{
}

void peer_directory_algorithm::item_changed (const rx_node_id& id, const string_type& path, peer_connection_ptr whose)
{
}

void peer_directory_algorithm::do_browse (peer_connection_ptr whose, const string_type& path)
{
	auto brw = std::make_unique<browse_peer_transaction>([this](query_result_items_type& items, peer_connection_ptr whose)
		{
			for (const auto& one : items)
			{
				if (one.first == rx_item_type::rx_directory)
					do_browse(whose, one.second.get_full_path());
			}
			for (const auto& one : items)
			{
				if (should_be_cached(one.first))
				{
					peer_item_ptr item = rx_create_reference<peer_item>(one.first, one.second, whose);
					whose->add_peer_item(std::move(item));
				}
			}

		}, whose);
	brw->path = path;
	auto ret = whose->send_request(std::move(brw), 2000);
}


// Class rx_internal::discovery::peer_refresh_algorithm 

peer_refresh_algorithm::peer_refresh_algorithm()
{
}


peer_refresh_algorithm::~peer_refresh_algorithm()
{
}



rx_result peer_refresh_algorithm::start_algorithm (runtime::runtime_start_context& ctx, rx_object_impl_ptr obj, peer_connection_ptr whose)
{
	return true;
}

rx_result peer_refresh_algorithm::stop_algorithm (runtime::runtime_stop_context& ctx, peer_connection_ptr whose)
{
	return true;
}

void peer_refresh_algorithm::client_connected (peer_connection_ptr whose)
{
}

void peer_refresh_algorithm::client_disconnected (peer_connection_ptr whose)
{
}

void peer_refresh_algorithm::item_changed (const rx_node_id& id, const string_type& path, peer_connection_ptr whose)
{
	auto msg = std::make_unique<query_peer_transaction>([this](query_result_items_type& items, peer_connection_ptr whose)
		{
			for (const auto& one : items)
			{
				if (should_be_cached(one.first))
				{
					peer_item_ptr item = rx_create_reference<peer_item>(one.first, one.second, whose);
					whose->add_peer_item(std::move(item));
				}
			}

		}, whose);
	auto query = std::make_shared<meta::queries::translate_query>();
	query->items.emplace_back(id);
	msg->queries.emplace_back(std::move(query));
	auto ret = whose->send_request(std::move(msg), 2000);
}


} // namespace discovery
} // namespace rx_internal

