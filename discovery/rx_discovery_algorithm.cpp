

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
// rx_discovery_comm
#include "discovery/rx_discovery_comm.h"
// rx_discovery_algorithm
#include "discovery/rx_discovery_algorithm.h"

#include "lib/rx_ser_bin.h"
#include "rx_discovery_transactions.h"
#include "rx_discovery_protocol.h"
#include "interfaces/rx_ethernet.h"


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


// Class rx_internal::discovery::peer_discovery_algorithms


rx_result peer_discovery_algorithms::start_register (discovery_register& who, hosting::rx_platform_host* host, const configuration_data_t& config)
{
	auto ips = interfaces::ethernet::get_ip_addresses(nullptr);
	who.identity_ = rx_uuid::create_new();
	who.my_data_.instance = config.instance.name;
	who.my_data_.node = rx_gate::instance().get_node_name();
	who.my_data_.id = who.identity_;
	who.system_addresses_ = ips;
	who.comm_point_ = rx_create_reference<discovery_point>();
	who.comm_point_->my_register_ = &who;
	auto port = config.instance.port;
	if (port == 0)
		port = 0x7ABC;

	who.comm_point_->default_port_ = port;
	if (!config.instance.group.empty())
		who.comm_point_->multicast_address_ = io::ip4_address(config.instance.group, port);
	who.multicast_address_ = who.comm_point_->multicast_address_;
	who.comm_point_->activate();
	return true;
}

void peer_discovery_algorithms::stop_register (discovery_register& who)
{
	if (who.comm_point_)// just on the safe side
	{
		who.comm_point_->deactivate();
		who.comm_point_ = discovery_point::smart_ptr::null_ptr;
	}
}

bool peer_discovery_algorithms::create_dafult_query (discovery_register& who, base_meta_writer& writer)
{
	return create_master_list_mine(who, writer, RX_CURRENT_DISCOVERY_VERSION);
}

bool peer_discovery_algorithms::create_fallback_register (discovery_register& who, base_meta_writer& writer)
{
	if (who.my_data_.serialize(writer, "own", RX_CURRENT_DISCOVERY_VERSION))
	{
		return true;
	}
	return false;
}

bool peer_discovery_algorithms::parse_new_master_when_active (discovery_register& who, base_meta_writer& writer, base_meta_reader& reader, const rx_uuid_t& id, io::ip4_address from, uint32_t version)
{
	discovered_peer_data data;
	if (data.deserialize(reader, "own", version))
	{
		if (data.addresses.empty())
		{
			auto it = who.comm_point_->registered_masters_.find(id);
			if (it != who.comm_point_->registered_masters_.end())
			{
				DISCOVERY_LOG_TRACE("discovery_point", 200, "Erased Discovery Master from "s
					+ it->second.binded.to_string() + " [" + it->first.to_string() + "].");
				who.comm_point_->registered_masters_.erase(id);
				std::scoped_lock _(who.register_lock_);

				who.peers_.erase(id);
			}
		}
		else
		{

			if (reader.start_array("others"))
			{
				bool success = true;
				std::vector< discovered_peer_data> others;
				while (!reader.array_end())
				{
					discovered_peer_data temp;
					if (!temp.deserialize(reader, "peer", version))
					{
						success = false;
						break;
					}
					else
					{
						if (!who.is_this_you(temp.id))
							others.push_back(std::move(temp));
					}
				}
				if (success)
				{
					if (create_master_list_mine(who, writer, version))
					{
						discovery_point::peer_data temp;
						temp.last_heard = rx_get_tick_count();
						temp.binded = from;
						temp.version = std::min(version, (uint32_t)RX_CURRENT_DISCOVERY_VERSION);
						who.comm_point_->registered_masters_[id] = temp;

						DISCOVERY_LOG_TRACE("discovery_point", 200, "Registered new Discovery Master from "s
							+ from.to_string() + "[" + rx_uuid(id).to_string() + "].");

						who.peers_[id] = others;
						who.peers_[id].push_back(data);

						return true;
					}
				}
			}
		}
	}
	return false;
}

bool peer_discovery_algorithms::parse_new_fallback_when_active (discovery_register& who, base_meta_writer& writer, base_meta_reader& reader, const rx_uuid_t& id, io::ip4_address from, uint32_t version, bool& broadcast)
{
	discovered_peer_data data;
	if (data.deserialize(reader, "own", version))
	{
		if (data.addresses.empty())
		{
			DISCOVERY_LOG_TRACE("discovery_point", 200, "Erased new Discovery Follower from "s + from.to_string() + " [" + rx_uuid(id).to_string() + "]");

			discovery_manager::instance().peer_unregistered(data.id);

			who.comm_point_->registered_followers_.erase(id);
			std::scoped_lock _(who.register_lock_);
			who.peers_.erase(id);
			who.followers_.erase(id);

			broadcast = true;
		}
		else
		{
			broadcast = false;
			discovery_point::peer_data temp;
			temp.last_heard = rx_get_tick_count();
			temp.binded = from;
			temp.version = std::min(version, (uint32_t)RX_CURRENT_DISCOVERY_VERSION);

			std::scoped_lock _(who.register_lock_);
			if (who.peers_[id].empty())
			{

				discovery_manager::instance().peer_registered(data.id, data.instance, data.node, from);
				who.peers_[id].push_back(data);
				who.followers_[id].push_back(std::move(data));

				if (create_master_list_full(who, writer, version))
				{
					who.comm_point_->registered_followers_[id] = temp;
					DISCOVERY_LOG_TRACE("discovery_point", 200, "Registered new Discovery Follower from "s + from.to_string() + " [" + rx_uuid(id).to_string() + "]");
					broadcast = true;
					return true;
				}
			}
			else
			{
				who.comm_point_->registered_followers_[id] = temp;
				if (who.peers_[id][0] != data)
				{
					who.peers_[id][0] = data;
					who.followers_[id][0] = std::move(data);
					broadcast = true;

				}
				if (create_master_list_full(who, writer, version))
				{
					return true;
				}
			}
		}

	}
	return false;
}

bool peer_discovery_algorithms::parse_master_list_when_active (discovery_register& who, base_meta_reader& reader, const rx_uuid_t& id, io::ip4_address from, uint32_t version)
{
	discovered_peer_data data;
	if (data.deserialize(reader, "own", version))
	{
		if (reader.start_array("others"))
		{
			bool success = true;
			std::vector< discovered_peer_data> others;
			while (!reader.array_end())
			{
				discovered_peer_data temp;
				if (!temp.deserialize(reader, "peer", version))
				{
					success = false;
					break;
				}
				else
				{
					if(!who.is_this_you(temp.id))
						others.push_back(std::move(temp));
				}
			}
			if (success)
			{

				discovery_point::peer_data temp;
				temp.last_heard = rx_get_tick_count();
				temp.binded = from;
				temp.version = std::min(version, (uint32_t)RX_CURRENT_DISCOVERY_VERSION);
				auto it = who.comm_point_->registered_masters_.find(id);
				if (it == who.comm_point_->registered_masters_.end())
				{
					DISCOVERY_LOG_TRACE("discovery_point", 200, "Registered new Discovery Master from "s
						+ from.to_string() + "[" + rx_uuid(id).to_string() + "].");
				}
				who.comm_point_->registered_masters_[id] = temp;
				std::scoped_lock _(who.register_lock_);

				who.peers_[id] = others;
				who.peers_[id].push_back(data);
			}
		}
	}
	return false;
}

bool peer_discovery_algorithms::parse_master_list_when_inactive (discovery_register& who, base_meta_reader& reader, const rx_uuid_t& id, io::ip4_address from, uint32_t version)
{
	discovered_peer_data data;
	if (data.deserialize(reader, "own", version))
	{
		if (reader.start_array("others"))
		{
			bool success = true;
			std::vector< discovered_peer_data> others;
			while (!reader.array_end())
			{
				discovered_peer_data temp;
				if (!temp.deserialize(reader, "peer", version))
				{
					success = false;
					break;
				}
				else
				{
					if(!who.is_this_you(temp.id))
						others.push_back(std::move(temp));
				}
			}
			if (success)
			{

				discovery_point::peer_data temp;
				temp.last_heard = rx_get_tick_count();
				temp.binded = from;
				temp.version = std::min(version, (uint32_t)RX_CURRENT_DISCOVERY_VERSION);
				who.comm_point_->registered_masters_[id] = temp;
				std::scoped_lock _(who.register_lock_);
				who.peers_[id] = others;
				who.peers_[id].push_back(data);
				who.comm_point_->state_ = discovery_point::discovery_state::fallback_active;

			}
		}
	}
	return false;
}

void peer_discovery_algorithms::recreate_own_data (discovery_register& who)
{
	who.my_data_.addresses.clear();
	if (who.system_port_)
	{
		who.my_data_.id = who.identity_;
		char port_buff[0x10];
		sprintf(port_buff, ":%d", (int)who.system_port_);
		for (const auto& one : who.system_addresses_)
		{
			discovered_network_point pt = one;
			pt.address += port_buff;
			who.my_data_.addresses.push_back(std::move(pt));
		}
	}
}

bool peer_discovery_algorithms::create_master_list_mine (discovery_register& who, base_meta_writer& writer, uint32_t version)
{
	if (who.my_data_.serialize(writer, "own", version))
	{
		if (writer.start_array("others", who.followers_.size()))
		{
			bool success = true;
			std::scoped_lock _(who.register_lock_);
			for (const auto& from : who.followers_)
			{
				for (const auto& peer : from.second)
				{
					if (!peer.serialize(writer, "peer", version))
					{
						success = false;
						break;
					}
				}
			}
			if (success)
			{
				if (writer.end_array())
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool peer_discovery_algorithms::create_master_list_full (discovery_register& who, base_meta_writer& writer, uint32_t version)
{
	if (who.my_data_.serialize(writer, "own", RX_CURRENT_DISCOVERY_VERSION))
	{
		if (writer.start_array("others", who.peers_.size()))
		{
			bool success = true;
			std::scoped_lock _(who.register_lock_);
			for (const auto& from : who.peers_)
			{
				for (const auto& peer : from.second)
				{
					if (!peer.serialize(writer, "peer", RX_CURRENT_DISCOVERY_VERSION))
					{
						success = false;
						break;
					}
				}
			}
			if (success)
			{
				if (writer.end_array())
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool peer_discovery_algorithms::do_time_checks_master (discovery_register& who, rx_timer_ticks_t ticks, rx_timer_ticks_t timeout)
{
	auto it = who.comm_point_->registered_masters_.begin();
	while (it != who.comm_point_->registered_masters_.end())
	{
		if (it->second.last_heard + timeout < ticks)
		{// remove this one

			DISCOVERY_LOG_TRACE("discovery_point", 200, "Erased Discovery Master from "s + it->first.to_string() + ", timeouted.");
			std::scoped_lock _(who.register_lock_);
			who.peers_.erase(it->first);
			it = who.comm_point_->registered_masters_.erase(it);
		}
		else
		{
			it++;
		}
	}
	it = who.comm_point_->registered_followers_.begin();
	while (it != who.comm_point_->registered_followers_.end())
	{
		if (it->second.last_heard + timeout < ticks)
		{// remove this one

			DISCOVERY_LOG_TRACE("discovery_point", 200, "Erased Discovery Follower from "s + it->first.to_string() + ", timeouted.");


			discovery_manager::instance().peer_unregistered(it->first);

			std::scoped_lock _(who.register_lock_);
			who.peers_.erase(it->first);
			who.followers_.erase(it->first);
			it = who.comm_point_->registered_followers_.erase(it);
		}
		else
		{
			it++;
		}
	}
	return true;
}

bool peer_discovery_algorithms::do_time_checks_follower (discovery_register& who, rx_timer_ticks_t ticks, rx_timer_ticks_t timeout)
{
	RX_ASSERT(who.comm_point_->registered_masters_.size() < 2);
	RX_ASSERT(who.comm_point_->registered_followers_.empty());
	auto it = who.comm_point_->registered_masters_.begin();
	while (it != who.comm_point_->registered_masters_.end())
	{
		if (it->second.last_heard + timeout < ticks)
		{// remove this one
			DISCOVERY_LOG_TRACE("discovery_point", 200, "Erased Discovery Master from "s
				+ it->second.binded.to_string() + " [" + it->first.to_string() + "], timeouted.");
			std::scoped_lock _(who.register_lock_);
			who.peers_.erase(it->first);
			it = who.comm_point_->registered_masters_.erase(it);
			return false;
		}
		else
		{
			it++;
		}
	}
	return true;
}

std::vector<discovery::discovered_peer_data> peer_discovery_algorithms::get_peers_network (discovery_register& who)
{
	std::vector<discovery::discovered_peer_data> data;
	std::scoped_lock _(who.register_lock_);
	data.push_back(who.my_data_);
	for (const auto& from : who.peers_)
	{
		for (const auto& peer : from.second)
		{
			data.push_back(peer);
		}
	}
	return data;
}

bool peer_discovery_algorithms::create_unregister (discovery_register& who, base_meta_writer& writer, uint32_t version)
{
	std::scoped_lock _(who.register_lock_);
	who.my_data_.addresses.clear();
	if (who.my_data_.serialize(writer, "own", RX_CURRENT_DISCOVERY_VERSION))
	{
		return true;
	}
	return false;
}


} // namespace discovery
} // namespace rx_internal

