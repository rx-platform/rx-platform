

/****************************************************************************
*
*  discovery\rx_discovery_main.cpp
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

#include "rx_discovery_configuration.h"
#include "system/meta/rx_runtime_data.h"
#include "system/server/rx_directory_cache.h"
#include "model/rx_model_algorithms.h"
#include "system/meta/rx_obj_types.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "rx_discovery_transactions.h"


namespace rx_internal {

namespace discovery {

// Class rx_internal::discovery::discovery_manager 

std::unique_ptr<discovery_manager> discovery_manager::g_obj;

std::atomic<uint32_t> discovery_manager::g_next_id = RX_DISCOVERY_CONNECTION_ID_FIRST;

discovery_manager::discovery_manager()
{
}


discovery_manager::~discovery_manager()
{
}



discovery_manager& discovery_manager::instance ()
{
	if (!g_obj)
		g_obj = std::make_unique<discovery_manager>();
	return *g_obj;
}

void discovery_manager::clear ()
{
	g_obj.reset();
}

rx_result discovery_manager::initialize (hosting::rx_platform_host* host, configuration_data_t& config)
{
	auto peers = get_peers();
	for (auto&& one : peers)
	{
		string_type name = one.peer_name;
		auto peer_ptr = rx_create_reference<peer_connection>(std::move(one));
		rx_result result = peer_ptr->build(host, config, g_next_id += 5);
		if (result)
		{
			connections_.emplace(std::move(name), std::move(peer_ptr));
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
	}
	return true;
}

void discovery_manager::deinitialize ()
{
}

std::vector<peer_endpoint> discovery_manager::get_peers ()
{
	std::vector<peer_endpoint> ret;
#ifdef _DEBUG
//	ret.emplace_back("ensaco", io::ip4_address("192.168.9.20", 31420), "rx-platform/*");
#else
#endif
	return ret;
}

peer_connection_ptr discovery_manager::get_peer (const string_type& name)
{
	auto it = connections_.find(name);
	if (it != connections_.end())
		return it->second;
	else
		return peer_connection_ptr();
}

template<class T>
rx_result add_object_to_configuration(rx_directory_ptr dir, typename T::instance_data_t&& data, data::runtime_values_data&& runtime_data, tl::type2type<T>
	, std::function<void(typename T::RTypePtr)> result_f = std::function<void(typename T::RTypePtr)>())
{
	data.meta_info = create_meta_for_new(data.meta_info);
	auto create_result = model::algorithms::runtime_model_algorithm<T>::create_runtime_sync(std::move(data)
		, std::move(runtime_data), true);

	if (create_result)
	{
		auto rx_type_item = create_result.value()->get_item_ptr();
		DISCOVERY_LOG_TRACE("code_objects", 100, ("Created "s + rx_item_type_name(T::RImplType::type_id) + " "s + rx_type_item->get_name()).c_str());
		if (result_f)
			result_f(create_result.value());
		return true;
	}
	else
	{
		create_result.register_error("Error creating "s + rx_item_type_name(T::RImplType::type_id) + " " + data.meta_info.get_full_path());
		for (const auto& one : create_result.errors())
			DISCOVERY_LOG_ERROR("code_objects", 900, one.c_str());
		//return create_result.errors();
		return true;
	}
}
// Class rx_internal::discovery::peer_endpoint 

peer_endpoint::peer_endpoint (string_type name, io::ip4_address addr, string_type ep)
	: peer_name(name)
	, ip4(std::move(addr))
	, path(std::move(ep))
{
}

peer_endpoint::peer_endpoint (string_type name, string_type addr, uint16_t port, string_type ep)
	: peer_name(name)
	, ip4(io::ip4_address(std::move(addr), port))
	, path(std::move(ep))
{
}



string_type peer_endpoint::get_url () const
{
	if (com_port.empty() && eth_port.empty())
	{
		std::ostringstream ss;
		ss << "tcp://";
		if (ip4.is_empty_ip4())
			ss << "127.0.0.1:31420";
		else
			ss << ip4.to_string();
		if (!path.empty())
			ss << '/' << path;
		return ss.str();
	}
	else if (!com_port.empty())
	{
		std::ostringstream ss;
		ss << "com://";
		ss << com_port;
		if (!path.empty())
			ss << '/' << path;
		return ss.str();
	}
	else if (!eth_port.empty())
	{
		std::ostringstream ss;
		ss << "eth://";
		ss << eth_port;
		if (!path.empty())
			ss << '/' << path;
		return ss.str();
	}
	else
	{
		return "";
	}
}


peer_endpoint::peer_endpoint(const peer_endpoint& right)
	: peer_name(right.peer_name)
	, ip4(right.ip4)
	, com_port(right.com_port)
	, eth_port(right.eth_port)
	, path(right.path)
{	
}
peer_endpoint::peer_endpoint(peer_endpoint&& right) noexcept
	: peer_name(std::move(right.peer_name))
	, ip4(std::move(right.ip4))
	, com_port(std::move(right.com_port))
	, eth_port(std::move(right.eth_port))
	, path(std::move(right.path))
{
}
// Class rx_internal::discovery::peer_connection 

peer_connection::peer_connection (peer_endpoint endpoint)
	: endpoint_(std::move(endpoint))
{
}


peer_connection::~peer_connection()
{
}



rx_result peer_connection::build (hosting::rx_platform_host* host, configuration_data_t& config, uint32_t id)
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_OBJ_NAME "/" RX_NS_PEER_OBJ_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir = ns::rx_directory_cache::instance().get_directory(full_path);
	rx_result result(true);
	if (dir)
	{
		runtime_data::port_runtime_data port_instance_data;

		port_instance_data.meta_info.name = string_type(RX_DISCOVERY_PROTOCOL_TCP_NAME_PREFIX) + endpoint_.peer_name;
		port_instance_data.meta_info.id = id + 1;
		port_instance_data.meta_info.parent = RX_TCP_CLIENT_PORT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		string_type ip_addr;
		uint16_t ip_port;
		endpoint_.ip4.get_splitted(ip_addr, ip_port);
		port_instance_data.overrides.add_value_static("Connect.IPPort", ip_port);
		port_instance_data.overrides.add_value_static("Connect.IPAddress", ip_addr);
		auto result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		port_instance_data.meta_info.name = string_type(RX_DISCOVERY_PROTOCOL_TRANSPORT_NAME_PREFIX) + endpoint_.peer_name;
		port_instance_data.meta_info.id = id + 2;
		port_instance_data.meta_info.parent = RX_OPCUA_CLIENT_TRANSPORT_PORT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("StackTop"
			, "./"s + RX_DISCOVERY_PROTOCOL_TCP_NAME_PREFIX + endpoint_.peer_name);
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());


		port_instance_data.meta_info.name = string_type(RX_DISCOVERY_PROTOCOL_CHANNEL_NAME_PREFIX) + endpoint_.peer_name;
		port_instance_data.meta_info.id = id + 3;
		port_instance_data.meta_info.parent = RX_OPCUA_SEC_NONE_CLIENT_PORT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("StackTop"
			, "./"s + RX_DISCOVERY_PROTOCOL_TRANSPORT_NAME_PREFIX + endpoint_.peer_name);
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());


		port_instance_data.meta_info.name = string_type(RX_DISCOVERY_PROTOCOL_CONNECTION_NAME_PREFIX) + endpoint_.peer_name;
		port_instance_data.meta_info.id = id + 4;
		port_instance_data.meta_info.parent = RX_RX_JSON_CLIENT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("StackTop"
			, "./"s + RX_DISCOVERY_PROTOCOL_CHANNEL_NAME_PREFIX + endpoint_.peer_name);
		port_instance_data.overrides.add_value_static("Connect.Endpoint", endpoint_.path);
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		meta::runtime_data::object_runtime_data instance_data;
		instance_data = meta::runtime_data::object_runtime_data();
		instance_data.meta_info.name = string_type(RX_DISCOVERY_CONNECTION_NAME_PREFIX) + endpoint_.peer_name;
		instance_data.meta_info.id = id;
		instance_data.meta_info.parent = RX_PEER_CONNECTION_TYPE_ID;
		instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		instance_data.meta_info.path = full_path;
		instance_data.instance_data.domain_ref = rx_node_id(RX_NS_SYSTEM_DOM_ID);
		instance_data.overrides.add_value_static("Endpoint.Name", endpoint_.peer_name);
		instance_data.overrides.add_value_static("Endpoint.Url", endpoint_.get_url());
		instance_data.overrides.add_value_static("Conn"
			, "./"s + RX_DISCOVERY_PROTOCOL_CONNECTION_NAME_PREFIX + endpoint_.peer_name);
		result = add_object_to_configuration(dir, std::move(instance_data), data::runtime_values_data(), tl::type2type<meta::object_types::object_type>());
	}
	return result;
}

rx_result peer_connection::send_request (rx_transaction_ptr trans, uint32_t timeout)
{
	if (my_port_)
	{
		return my_port_->send_request(std::move(trans), timeout);
	}
	return RX_NOT_CONNECTED;
}

bool peer_connection::port_connected (rx_protocol::rx_json_protocol_client_port::smart_ptr port)
{
	if (port)
	{
		my_port_ = port;
		my_port_->register_user(smart_this());
	}
	return true;
}

void peer_connection::port_disconnected ()
{
	if (my_port_)
	{
		my_port_->unregister_user(smart_this());
		my_port_ = rx_protocol::rx_json_protocol_client_port::smart_ptr::null_ptr;
	}
}

rx_result peer_connection::initialize_runtime (runtime_init_context& ctx, rx_reference<peer_object> object)
{
	object_ = object;
	return true;
}

rx_result peer_connection::deinitialize_runtime (runtime_deinit_context& ctx)
{
	object_ = peer_object::smart_ptr::null_ptr;
	return true;
}

rx_result peer_connection::start_runtime (runtime_start_context& ctx)
{
	peer_connection_ptr ptr = smart_this();
	// send client connected
	std::apply([&ptr, ctx, this](auto&&... args) mutable {(args.start_algorithm(ctx, object_, ptr), ...); }, algorithms_);
	return true;
}

rx_result peer_connection::stop_runtime (runtime_stop_context& ctx)
{
	peer_connection_ptr ptr = smart_this();
	// send client connected
	std::apply([&ptr, ctx, this](auto&&... args) mutable {(args.stop_algorithm(ctx, ptr), ...); }, algorithms_);
	return true;
}

void peer_connection::client_connected ()
{
	if (object_)
		object_->set_status(true, "");

	auto ptr = smart_this();
	// send client connected
	std::apply([&ptr](auto&&... args) {(args.client_connected(ptr), ...); }, algorithms_);
}

void peer_connection::client_disconnected ()
{
	if (object_)
		object_->set_status(false, "");

	auto ptr = smart_this();
	// send client connected
	std::apply([&ptr](auto&&... args) {(args.client_disconnected(ptr), ...); }, algorithms_);
}

void peer_connection::item_changed (const rx_node_id& id, const string_type& path)
{
	auto ptr = smart_this();
	// send client connected
	std::apply([&id, &path, &ptr](auto&&... args) {(args.item_changed(id, path, ptr), ...); }, algorithms_);
}

void peer_connection::add_peer_item (peer_item_ptr item)
{
	std::scoped_lock _(items_lock_);
	cached_paths_.insert_or_assign(item->meta.get_full_path(), item);
	cached_items_.insert_or_assign(item->meta.id, std::move(item));
}

void peer_connection::remove_peer_item (const rx_node_id& id)
{
	std::scoped_lock _(items_lock_);
	auto it = cached_items_.find(id);
	if (it != cached_items_.end())
	{
		cached_paths_.erase(it->second->meta.get_full_path());
		cached_items_.erase(it);
	}
}


// Class rx_internal::discovery::peer_object 

peer_object::peer_object()
      : online_(false),
        stream_version_("")
{
	resolver_user_.my_obj = this;
	DISCOVERY_LOG_DEBUG("peer_object", 200, "{rx-platform} discovery peer object created.");
}


peer_object::~peer_object()
{
	DISCOVERY_LOG_DEBUG("peer_object", 200, "{rx-platform} discovery peer object destroyed.");
}



rx_result peer_object::initialize_runtime (runtime_init_context& ctx)
{
	rx_result ret;
	connection_ = discovery_manager::instance().get_peer(ctx.get_item_static<string_type>("Endpoint.Name", ""));
	online_.bind("Status.Online", ctx);
	stream_version_.bind("Status.Version", ctx);
	if (connection_)
	{
		ret = connection_->initialize_runtime(ctx, smart_this());
	}
	return ret;
}

rx_result peer_object::deinitialize_runtime (runtime_deinit_context& ctx)
{
	rx_result ret;
	if (connection_)
	{
		ret = connection_->deinitialize_runtime(ctx);
	}
	connection_ = peer_connection_ptr::null_ptr;
	return ret;
}

rx_result peer_object::start_runtime (runtime_start_context& ctx)
{
	auto ret = ctx.register_relation_subscriber("Conn", &resolver_user_);
	if (!ret)
	{
		RUNTIME_LOG_WARNING("peer_connection", 900, "Error starting port registration "
			+ ctx.context->meta_info.get_full_path() + "." + ctx.path.get_current_path() + " " + ret.errors_line());
	}
	if (connection_)
	{
		ret = connection_->start_runtime(ctx);
	}
	return ret;
}

rx_result peer_object::stop_runtime (runtime_stop_context& ctx)
{
	rx_result ret;
	if (connection_)
	{
		ret = connection_->stop_runtime(ctx);
	}
	return ret;
}

bool peer_object::internal_port_connected (const platform_item_ptr& item)
{
	if (connection_)
	{
		auto result = rx_platform::get_runtime_instance<rx_protocol::rx_json_protocol_client_port>(item->meta_info().id);
		if (result)
		{
			connection_->port_connected(result.value());
			return true;
		}
	}
	return false;
}

void peer_object::internal_port_disconnected ()
{
	if (connection_)
	{
		connection_->port_disconnected();
	}
}

void peer_object::set_status (bool online, const string_type& ver)
{
	online_ = online;
	stream_version_ = ver;
}


} // namespace discovery
} // namespace rx_internal

