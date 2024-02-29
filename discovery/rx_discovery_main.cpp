

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
#include "rx_discovery_connections.h"
#include "api/rx_meta_api.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_internal {

namespace discovery {
namespace
{
peer_connections_manager g_connections_manager;

template<class typeT>
rx_result register_peer_type(peer_item_ptr what)
{
	auto result = model::algorithms::simple_types_model_algorithm<typeT>::create_peer_type_sync(what);

	return result;
}
template<class typeT>
rx_result unregister_peer_type(peer_item_ptr what)
{
	auto result = model::algorithms::simple_types_model_algorithm<typeT>::delete_peer_type_sync(what);

	return result;
}
template<class typeT>
rx_result register_peer_simple_type(peer_item_ptr what)
{
	auto result = model::algorithms::simple_types_model_algorithm<typeT>::create_peer_type_sync(what);

	return result;
}
template<class typeT>
rx_result unregister_peer_simple_type(peer_item_ptr what)
{
	auto result = model::algorithms::simple_types_model_algorithm<typeT>::delete_peer_type_sync(what);

	return result;
}/*
rx_result register_peer_data_type(peer_item_ptr what)
{
	auto result = model::platform_types_manager::instance().get_data_types_repository().register_peer_type(what);

	return result;
}*/
}

// Class rx_internal::discovery::discovery_manager

std::unique_ptr<discovery_manager> discovery_manager::g_obj;

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
	auto result = g_connections_manager.initialize(host, config, peers);
	if (!result)
		return result;

	return true;
}

rx_result discovery_manager::start (hosting::rx_platform_host* host, const configuration_data_t& config)
{
	auto result = peer_discovery_algorithms::start_register(peers_register_, host, config);
	if (!result)
		return result;

	result = g_connections_manager.start(host, config);
	if (!result)
		return result;

	return true;
}

void discovery_manager::stop ()
{
	g_connections_manager.stop();
	peer_discovery_algorithms::stop_register(peers_register_);
}

void discovery_manager::deinitialize ()
{
	g_connections_manager.deinitialize();
}

std::vector<peer_endpoint> discovery_manager::get_peers ()
{
	std::vector<peer_endpoint> ret;
#ifdef _DEBUG
	//ret.emplace_back("ensaco", io::ip4_address("192.168.9.20", 31420), "rx-platform/*");
#else
#endif
	return ret;
}

peer_connection_ptr discovery_manager::get_peer (const rx_uuid& id)
{
	return g_connections_manager.get_peer(id);
}

uint32_t discovery_manager::subscribe_to_port (std::function<void(uint16_t)> callback, rx_reference_ptr anchor)
{
	return peers_register_.comm_point_->subscribe_to_port(callback, anchor);
}

void discovery_manager::unsubscribe_from_port (uint32_t id)
{
	peers_register_.comm_point_->unsubscribe_from_port(id);
}

std::vector<discovery::discovered_peer_data> discovery_manager::get_peers_network ()
{
	return peer_discovery_algorithms::get_peers_network(peers_register_);
}

void discovery_manager::peer_registered (const rx_uuid& id, string_view_type instance, string_view_type node, const io::ip4_address& from)
{
	g_connections_manager.peer_registered(id, instance, node, from);
}

void discovery_manager::peer_unregistered (const rx_uuid& id)
{
	g_connections_manager.peer_unregistered(id);
}

io::ip4_address discovery_manager::get_ip4_network () const
{
	return peers_register_.get_multicast_address();
}

rx_uuid discovery_manager::get_network_id () const
{
	return peers_register_.get_identity();
}

template<class T>
rx_result add_object_to_configuration(rx_directory_ptr dir, typename T::instance_data_t&& data, data::runtime_values_data&& runtime_data, tl::type2type<T>
	, std::function<void(typename T::RTypePtr)> result_f = std::function<void(typename T::RTypePtr)>())
{
	string_type path = data.meta_info.get_full_path();
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
		create_result.register_error("Error creating "s + rx_item_type_name(T::RImplType::type_id) + " " + path);
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

peer_connection::peer_connection (string_view_type name, const io::ip4_address& addr)
	: endpoint_(string_type(name), addr)
{
}


peer_connection::~peer_connection()
{
}



rx_result peer_connection::build (const rx_uuid& id)
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_OBJ_NAME "/" RX_NS_PEER_OBJ_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir = ns::rx_directory_cache::instance().get_directory(full_path);
	rx_result result(true);
	if (dir)
	{
		string_type ns_peer_name(endpoint_.peer_name);
		for (auto& ch : ns_peer_name)
		{
			if (!rx_is_valid_name_character(ch))
				ch = '_';
		}

		runtime_data::port_runtime_data port_instance_data;

		port_instance_data.meta_info.name = string_type(RX_DISCOVERY_PROTOCOL_TCP_NAME_PREFIX) + ns_peer_name;
		port_instance_data.meta_info.id = rx_uuid::create_new();
		port_instance_data.meta_info.parent = RX_TCP_CLIENT_PORT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.meta_info.run_at = RX_OTHERS_STORAGE_NAME;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		string_type ip_addr;
		uint16_t ip_port;
		endpoint_.ip4.get_splitted(ip_addr, ip_port);
		port_instance_data.overrides.add_value_static("Connect.IPPort", ip_port);
		port_instance_data.overrides.add_value_static("Connect.IPAddress", ip_addr);
		ids_[0] = port_instance_data.meta_info.id;
		auto result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		port_instance_data = runtime_data::port_runtime_data();
		port_instance_data.meta_info.name = string_type(RX_DISCOVERY_PROTOCOL_TRANSPORT_NAME_PREFIX) + ns_peer_name;
		port_instance_data.meta_info.id = rx_uuid::create_new();
		port_instance_data.meta_info.parent = RX_OPCUA_CLIENT_TRANSPORT_PORT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.meta_info.run_at = RX_OTHERS_STORAGE_NAME;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("StackTop"
			, "./"s + RX_DISCOVERY_PROTOCOL_TCP_NAME_PREFIX + ns_peer_name);
		ids_[1] = port_instance_data.meta_info.id;
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		port_instance_data = runtime_data::port_runtime_data();
		port_instance_data.meta_info.name = string_type(RX_DISCOVERY_PROTOCOL_CHANNEL_NAME_PREFIX) + ns_peer_name;
		port_instance_data.meta_info.id = rx_uuid::create_new();
		port_instance_data.meta_info.parent = RX_OPCUA_SEC_NONE_CLIENT_PORT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.meta_info.run_at = RX_OTHERS_STORAGE_NAME;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("StackTop"
			, "./"s + RX_DISCOVERY_PROTOCOL_TRANSPORT_NAME_PREFIX + ns_peer_name);
		ids_[2] = port_instance_data.meta_info.id;
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		port_instance_data = runtime_data::port_runtime_data();
		port_instance_data.meta_info.name = string_type(RX_DISCOVERY_PROTOCOL_CONNECTION_NAME_PREFIX) + ns_peer_name;
		port_instance_data.meta_info.id = rx_uuid::create_new();
		port_instance_data.meta_info.parent = RX_RX_JSON_CLIENT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.meta_info.run_at = RX_OTHERS_STORAGE_NAME;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("StackTop"
			, "./"s + RX_DISCOVERY_PROTOCOL_CHANNEL_NAME_PREFIX + ns_peer_name);
		port_instance_data.overrides.add_value_static("Connect.Endpoint", endpoint_.get_url());
		ids_[3] = port_instance_data.meta_info.id;
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		meta::runtime_data::object_runtime_data instance_data;
		instance_data = meta::runtime_data::object_runtime_data();
		instance_data.meta_info.name = string_type(RX_DISCOVERY_CONNECTION_NAME_PREFIX) + ns_peer_name;
		instance_data.meta_info.id = id;
		instance_data.meta_info.parent = RX_PEER_CONNECTION_TYPE_ID;
		instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		instance_data.meta_info.path = full_path;
		instance_data.meta_info.run_at = RX_OTHERS_STORAGE_NAME;
		instance_data.instance_data.domain_ref = rx_node_id(RX_NS_SYSTEM_DOM_ID);
		instance_data.overrides.add_value_static("Endpoint.Name", endpoint_.peer_name);
		instance_data.overrides.add_value_static("Endpoint.Url", endpoint_.get_url());
		instance_data.overrides.add_value_static("Conn"
			, "./"s + RX_DISCOVERY_PROTOCOL_CONNECTION_NAME_PREFIX + ns_peer_name);

		ids_[4] = instance_data.meta_info.id;
		result = add_object_to_configuration(dir, std::move(instance_data), data::runtime_values_data(), tl::type2type<meta::object_types::object_type>());
	}
	return result;
}

rx_result peer_connection::destroy ()
{
	if (ids_[0]) // tcp
	{
		rx_platform::api::meta::rx_delete_runtime<meta::object_types::port_type>(
			rx_item_reference(ids_[0]), rx_result_callback(smart_this(), [](rx_result result)
			{

			}));
		ids_[0] = rx_node_id();
	}
	if (ids_[1])  // transport
	{
		rx_platform::api::meta::rx_delete_runtime<meta::object_types::port_type>(
			rx_item_reference(ids_[1]), rx_result_callback(smart_this(), [](rx_result result)
			{

			}));
		ids_[1] = rx_node_id();
	}
	if (ids_[2]) // sec channel
	{
		rx_platform::api::meta::rx_delete_runtime<meta::object_types::port_type>(
			rx_item_reference(ids_[2]), rx_result_callback(smart_this(), [](rx_result result)
			{

			}));
		ids_[2] = rx_node_id();
	}
	if (ids_[3]) // app client
	{
		rx_platform::api::meta::rx_delete_runtime<meta::object_types::port_type>(
			rx_item_reference(ids_[3]), rx_result_callback(smart_this(), [](rx_result result)
			{

			}));
		ids_[3] = rx_node_id();
	}
	if (ids_[4]) // peer object
	{
		rx_platform::api::meta::rx_delete_runtime<meta::object_types::object_type>(
			rx_item_reference(ids_[4]), rx_result_callback(smart_this(), [](rx_result result)
			{

			}));
		ids_[4] = rx_node_id();
	}
	return true;
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
	rx_post_function_to(RX_DOMAIN_META, smart_this(), [this]()
		{
			std::vector<peer_item_ptr> to_remove;
			{
				std::scoped_lock _(items_lock_);
				for (auto& one : cached_items_)
				{
					to_remove.push_back(one.second);
				}
				cached_items_.clear();
				cached_paths_.clear();
			}
			for (auto one : to_remove)
			{
				remove_peer_item_sync(one);
			}
		});

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
	rx_post_function_to(RX_DOMAIN_META, smart_this(), [this](peer_item_ptr item)
		{
			rx_result result = RX_NOT_VALID_TYPE;

			switch (item->type)
			{
				case rx_application:
					{
					}
					break;
				case rx_application_type:
					{
					}
					break;
				case rx_domain:
					{
					}
					break;
				case rx_domain_type:
					{
					}
					break;
				case rx_object:
					{
					}
					break;
				case rx_object_type:
					{
					}
					break;
				case rx_port:
					{
					}
					break;
				case rx_port_type:
					{
					}
					break;
				case rx_struct_type:
					{
						result = register_peer_simple_type<meta::basic_types::struct_type>(item);
					}
					break;
				case rx_variable_type:
					{
						result = register_peer_simple_type<meta::basic_types::variable_type>(item);
					}
					break;
				case rx_source_type:
					{
						result = register_peer_simple_type<meta::basic_types::source_type>(item);
					}
					break;
				case rx_filter_type:
					{
						result = register_peer_simple_type<meta::basic_types::filter_type>(item);
					}
					break;
				case rx_event_type:
					{
						result = register_peer_simple_type<meta::basic_types::event_type>(item);
					}
					break;
				case rx_mapper_type:
					{
						result = register_peer_simple_type<meta::basic_types::mapper_type>(item);
					}
					break;
				case rx_relation_type:
					{
					}
					break;
				case rx_program_type:
					{
						result = register_peer_simple_type<meta::basic_types::program_type>(item);
					}
					break;
				case rx_method_type:
					{
						result = register_peer_simple_type<meta::basic_types::method_type>(item);
					}
					break;
				case rx_data_type:
					{
					}
					break;
				case rx_display_type:
					{
					}
					break;
                default:
                    {
                    }
                    break;
			}
			if (result)
			{
				std::scoped_lock _(items_lock_);
				cached_paths_.insert_or_assign(item->meta.get_full_path(), item);
				cached_items_.insert_or_assign(item->meta.id, std::move(item));
			}
		}, item);

}

void peer_connection::remove_peer_item (const rx_node_id& id)
{
	rx_post_function_to(RX_DOMAIN_META, smart_this(), [this](rx_node_id id)
		{
			peer_item_ptr to_remove;
			{
				std::scoped_lock _(items_lock_);
				auto it = cached_items_.find(id);
				if (it != cached_items_.end())
				{
					to_remove = it->second;
					cached_paths_.erase(it->second->meta.get_full_path());
					cached_items_.erase(it);
				}
			}
			if(to_remove)
				remove_peer_item_sync(to_remove);

		}, id);
}

void peer_connection::remove_peer_item_sync (peer_item_ptr item)
{
	rx_result result = RX_NOT_VALID_TYPE;
	switch (item->type)
	{
		case rx_application:
			{
			}
			break;
		case rx_application_type:
			{
			}
			break;
		case rx_domain:
			{
			}
			break;
		case rx_domain_type:
			{
			}
			break;
		case rx_object:
			{
			}
			break;
		case rx_object_type:
			{
			}
			break;
		case rx_port:
			{
			}
			break;
		case rx_port_type:
			{
			}
			break;
		case rx_struct_type:
			{
				result = unregister_peer_simple_type<meta::basic_types::struct_type>(item);
			}
			break;
		case rx_variable_type:
			{
				result = unregister_peer_simple_type<meta::basic_types::variable_type>(item);
			}
			break;
		case rx_source_type:
			{
				result = unregister_peer_simple_type<meta::basic_types::source_type>(item);
			}
			break;
		case rx_filter_type:
			{
				result = unregister_peer_simple_type<meta::basic_types::filter_type>(item);
			}
			break;
		case rx_event_type:
			{
				result = unregister_peer_simple_type<meta::basic_types::event_type>(item);
			}
			break;
		case rx_mapper_type:
			{
				result = unregister_peer_simple_type<meta::basic_types::mapper_type>(item);
			}
			break;
		case rx_relation_type:
			{
			}
			break;
		case rx_program_type:
			{
				result = unregister_peer_simple_type<meta::basic_types::program_type>(item);
			}
			break;
		case rx_method_type:
			{
				result = unregister_peer_simple_type<meta::basic_types::method_type>(item);
			}
			break;
		case rx_data_type:
			{
			}
			break;
		case rx_display_type:
			{
			}
			break;
        default:
            {
            }
            break;
	}
	if (!result)
	{
		DISCOVERY_LOG_ERROR("peer_connection", 800, "Error removing peer item "s + item->meta.get_full_path() + " - " + result.errors_line());
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
	if (!ctx.meta.id.is_guid())
		return RX_INTERNAL_ERROR;

	connection_ = discovery_manager::instance().get_peer(ctx.meta.id.get_uuid());
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

