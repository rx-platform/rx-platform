

/****************************************************************************
*
*  interfaces\rx_full_duplex_packet.cpp
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


// rx_full_duplex_packet
#include "interfaces/rx_full_duplex_packet.h"

#include "interfaces/rx_endpoints.h"
#include "interfaces/rx_port_stack_active.h"
using namespace rx::io;


namespace rx_internal {

namespace interfaces {

namespace ports_lib {
template <typename addrT>
typename duplex_port_adapter<addrT>::key_type duplex_port_adapter<addrT>::get_key_from_addresses_initiator(const addrT& local_address, const addrT& remote_address)
{
	return remote_address;
}
template <typename addrT>
typename duplex_port_adapter<addrT>::listener_key_type duplex_port_adapter<addrT>::get_key_from_addresses_listener(const addrT& local_address, const addrT& remote_address)
{
	return local_address;
}
template <typename addrT>
std::pair<bool, typename duplex_port_adapter<addrT>::key_type> duplex_port_adapter<addrT>::get_key_from_packet_initiator(const recv_protocol_packet& packet)
{
	addrT local, remote;
	if (!remote.parse(packet.from_addr))
		return { false, key_type() };
	if (!local.parse(packet.to_addr))
		return { false, key_type() };
	return { true, remote };
}

template <typename addrT>
std::pair<bool, typename duplex_port_adapter<addrT>::listener_key_type> duplex_port_adapter<addrT>::get_key_from_packet_listener(const recv_protocol_packet& packet)
{
	addrT local, remote;
	if (!remote.parse(packet.from_addr))
		return { false, listener_key_type() };
	if (!local.parse(packet.to_addr))
		return { false, listener_key_type() };
	return { true, { local, remote } };
}
template <typename addrT>
typename duplex_port_adapter<addrT>::listener_key_type duplex_port_adapter<addrT>::get_key_from_listener(const listener_data_type<addrT>& session_data)
{
	return { session_data.local_addr, session_data.remote_addr };
}
template <typename addrT>
void duplex_port_adapter<addrT>::fill_send_packet(send_protocol_packet& packet, const initiator_data_type<addrT>& session_data)
{
	packet.from_addr = &session_data.local_addr;
	packet.to_addr = &session_data.remote_addr;
}
template <typename addrT>
void duplex_port_adapter<addrT>::fill_send_packet(send_protocol_packet& packet, const listener_data_type<addrT>& session_data)
{
	packet.from_addr = &session_data.local_addr;
	packet.to_addr = &session_data.remote_addr;
}
template <typename addrT>
std::pair<typename duplex_port_adapter<addrT>::key_type, std::unique_ptr<initiator_data_type<addrT> >> duplex_port_adapter<addrT>::create_initiator_data(const protocol_address* local_address, const protocol_address* remote_address)
{
	std::unique_ptr<initiator_data_type<addrT> > data = std::make_unique<initiator_data_type<addrT> >();
	auto result = data->local_addr.parse(local_address);
	if (!result)
	{
		data.reset();
		return { key_type(), std::move(data) };
	}
	result = data->remote_addr.parse(remote_address);
	if (!result || data->remote_addr.is_null())
	{
		data.reset();
		return { key_type(), std::move(data) };
	}
	addrT key = data->remote_addr;
	return { key, std::move(data) };
}

template <typename addrT>
std::pair<typename duplex_port_adapter<addrT>::listener_key_type, std::unique_ptr<listener_data_type<addrT> >> duplex_port_adapter<addrT>::create_listener_data(const protocol_address* local_address, const protocol_address* remote_address)
{
	std::unique_ptr<listener_data_type<addrT> > data = std::make_unique<listener_data_type<addrT> >();
	auto result = data->local_addr.parse(local_address);
	if (!result || data->local_addr.is_null())
	{
		data.reset();
		return { listener_key_type(), std::move(data) };
	}
	result = data->remote_addr.parse(remote_address);
	if (!result)
	{
		data.reset();
		return { listener_key_type(), std::move(data) };
	}
	listener_key_type key = { data->local_addr, data->remote_addr };
	return { key, std::move(data) };
}

template <typename addrT>
std::unique_ptr<listener_instance<addrT> > duplex_port_adapter<addrT>::create_listener_instance(const protocol_address* local_address, const protocol_address* remote_address)
{
	std::unique_ptr<listener_instance<addrT> > data = std::make_unique<listener_instance<addrT> >();
	/*auto result = data->local_addr.parse(local_address);
	if (!result)
	{
		data.reset();
	}
	else
	{
		result = data->remote_addr.parse(remote_address);
		if (!result)
		{
			data.reset();
		}
	}*/
	return data;
}


// Class rx_internal::interfaces::ports_lib::byte_routing_port 


void byte_routing_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
	if (local_addr.is_null())
	{
		auto addr = binder_data.get_value("Bind.Address");
		uint8_t addr_val = 0;
		if (addr.is_unassigned())
			addr_val = addr.extract_static(0);
		if (addr_val != 0)
		{
			io::numeric_address<uint8_t> num_addr(addr_val);
			local_addr = &num_addr;
		}
	}
	if (remote_addr.is_null())
	{
		auto addr = binder_data.get_value("Connect.Address");
		uint8_t addr_val = 0;
		if (addr.is_unassigned())
			addr_val = addr.extract_static(addr_val);
		if (addr_val != 0)
		{
			io::numeric_address<uint8_t> num_addr(addr_val);
			remote_addr = &num_addr;
		}
	}
}


// Parameterized Class rx_internal::interfaces::ports_lib::full_duplex_addr_packet_port 

template <typename addrT>
full_duplex_addr_packet_port<addrT>::full_duplex_addr_packet_port()
      : state_(port_state_inactive),
        session_timeout_(2000)
{
}



template <typename addrT>
rx_protocol_stack_endpoint* full_duplex_addr_packet_port<addrT>::construct_initiator_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
	return &initiator_endpoint_;
}

template <typename addrT>
rx_protocol_stack_endpoint* full_duplex_addr_packet_port<addrT>::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
	if (state_ == port_state_inactive)
		return nullptr;
	std::unique_ptr<listener_instance_t > listener_inst = address_adapter_type::create_listener_instance(local_address, remote_address);
	if (!listener_inst)
		return nullptr;
	rx_protocol_stack_endpoint* stack_ep = &listener_inst->stack_endpoint;

	listener_inst->my_port = this;
	

	rx_init_stack_entry(&listener_inst->stack_endpoint, listener_inst.get());
	
	listener_inst->stack_endpoint.received_function = [](rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
	{
		listener_instance<addrT>* me = (listener_instance<addrT>*)reference->user_data;
		me->my_port->received_packet();
		return me->route_listeners_packet(packet);
	};

	listener_inst->stack_endpoint.closed_function = [](rx_protocol_stack_endpoint* reference, rx_protocol_result_t result)
	{
		listener_instance<addrT>* me = (listener_instance<addrT>*)reference->user_data;
		me->listener_closed_received(result);
		me->my_port->unbind_stack_endpoint(&me->stack_endpoint);
	};
	/*listener_inst->stack_endpoint.connected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session)
	{
		listener_instance<addrT>* me = (listener_instance<addrT>*)reference->user_data;
		return me->listener_connected_received(session);
	};*/
	listener_inst->stack_endpoint.allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
	{
		listener_instance<addrT>* me = (listener_instance<addrT>*)entry->user_data;
		auto result = me->my_port->alloc_io_buffer();
		if (result)
		{
			result.value().detach(buffer);
			return RX_PROTOCOL_OK;
		}
		else
		{
			return RX_PROTOCOL_OUT_OF_MEMORY;
		}
	};
	listener_inst->stack_endpoint.release_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
	{
		listener_instance<addrT>* me = (listener_instance<addrT>*)entry->user_data;
		rx_io_buffer temp;
		temp.attach(buffer);
		me->my_port->release_io_buffer(std::move(temp));

		return RX_PROTOCOL_OK;
	};
	locks::auto_lock_t _(&routing_lock_);
	listener_endpoints_.emplace(stack_ep, std::move(listener_inst));
	return stack_ep;
}

template <typename addrT>
void full_duplex_addr_packet_port<addrT>::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
	locks::auto_lock_t _(&routing_lock_);
	auto it = listener_endpoints_.find(what);
	if (it != listener_endpoints_.end())
	{
		listener_endpoints_.erase(it);
	}
	else
	{
		auto itd = listening_.find(what);
		if (itd != listening_.end())
		{
			listening_.erase(itd);
		}
	}
}

template <typename addrT>
rx_result full_duplex_addr_packet_port<addrT>::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
	if (state_ == port_state_inactive)
		return "Port not started!";
	std::pair<listener_key_type, std::unique_ptr<listener_data_t> > listener_data = address_adapter_type::create_listener_data(local_address, remote_address);
	if (!listener_data.second)
		return "Unable to parse address:";

	return true;
}

template <typename addrT>
rx_result_with<port_connect_result> full_duplex_addr_packet_port<addrT>::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
	if (state_ == port_state_inactive)
		return "Port not started!";

	std::pair<key_type, std::unique_ptr<initiator_data_t> > initiator_data = address_adapter_type::create_initiator_data(local_address, remote_address);
	if (!initiator_data.second)
		return "Unable to parse address";
	rx_protocol_stack_endpoint* stack_ep = &initiator_data.second->stack_endpoint;
	initiator_data.second->my_port = this;

	rx_init_stack_entry(stack_ep, initiator_data.second.get());
	///////////////////////////////////////////////////////////////
	// here is the code for filling in the client session stuff
	stack_ep->send_function = [](rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
		{
			initiator_data_t* me = (initiator_data_t*)reference->user_data;
			address_adapter_type::fill_send_packet(packet, *me);
			me->my_port->sent_packet();
			return rx_move_packet_down(&me->my_port->initiator_endpoint_, packet);
		};
	stack_ep->close_function = [] (struct rx_protocol_stack_endpoint* reference, rx_protocol_result_t result) -> rx_protocol_result_t
		{
			rx_notify_closed(reference, RX_PROTOCOL_OK);
			rx_protocol_result_t ret = RX_PROTOCOL_OK;
			initiator_data_t* me = (initiator_data_t*)reference->user_data;
			if (me->my_port)
			{
				key_type key = address_adapter_type::get_key_from_addresses_initiator(me->local_addr, me->remote_addr);
				ret = me->my_port->remove_initiator(key);
			}
			return ret;
		};
	stack_ep->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
		{
			initiator_data_t* whose = reinterpret_cast<initiator_data_t*>(entry->user_data);
			whose->my_port->unbind_stack_endpoint(entry);
		};
	stack_ep->allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
		{
			initiator_data_t* whose = (initiator_data_t*)entry->user_data;
			auto result = whose->my_port->alloc_io_buffer();
			if (result)
			{
				result.value().detach(buffer);
				return RX_PROTOCOL_OK;
			}
			else
			{
				return RX_PROTOCOL_OUT_OF_MEMORY;
			}
		};
	stack_ep->release_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
		{
			initiator_data_t* whose = (initiator_data_t*)entry->user_data;
			rx_io_buffer temp;
			temp.attach(buffer);
			whose->my_port->release_io_buffer(std::move(temp));

			return RX_PROTOCOL_OK;
		};


	locks::auto_lock_t _(&routing_lock_);
	if(initiators_.find(initiator_data.first) != initiators_.end())
		return "Address already occupied!";

	initiators_.emplace(std::move(initiator_data));

	return port_connect_result(stack_ep, state_ == current_port_state::port_state_connected);
}

template <typename addrT>
void full_duplex_addr_packet_port<addrT>::stack_assembled ()
{
	if (state_ & port_state_wait_connecting)
		connect(nullptr, nullptr);
	if (state_ & port_state_wait_listening)
		listen(nullptr, nullptr);
}

template <typename addrT>
void full_duplex_addr_packet_port<addrT>::stack_disassembled ()
{
	current_port_state new_state = port_state_inactive;
	if (state_ & port_connector)
		new_state = new_state | port_state_wait_connecting;
	if (state_ & port_listener)
		new_state = new_state | port_state_wait_listening;
	state_ = new_state;
}

template <typename addrT>
rx_result full_duplex_addr_packet_port<addrT>::stop_passive ()
{
	return true;
}

template <typename addrT>
rx_result full_duplex_addr_packet_port<addrT>::initialize_runtime (runtime_init_context& ctx)
{
	rx_result result = status.initialize(ctx);

	auto one_result = session_timeout_.bind("Options.SessionTimeout", ctx);
	if (!one_result)
		result.register_error("Error connecting Options.SessionTimeout:"s + one_result.errors_line());

	return result;
}

template <typename addrT>
rx_result full_duplex_addr_packet_port<addrT>::start_runtime (runtime_start_context& ctx)
{
	if (state_ != port_state_inactive)
		return "Wrong port state!!!";
	current_port_state new_state = port_state_inactive;
	rx_init_stack_entry(&initiator_endpoint_, this);
	
	initiator_endpoint_.received_function = [](rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
		{
			full_duplex_addr_packet_port<addrT>* me = (full_duplex_addr_packet_port<addrT>*)reference->user_data;
			me->received_packet();
			return me->route_initiator_packet(packet);
		};
	initiator_endpoint_.connected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session)
		{
			full_duplex_addr_packet_port<addrT>* me = (full_duplex_addr_packet_port<addrT>*)reference->user_data;
			return me->initiator_connected_received(session);
		};
	initiator_endpoint_.allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
		{
			full_duplex_addr_packet_port<addrT>* whose = (full_duplex_addr_packet_port<addrT>*)entry->user_data;
			auto result = whose->alloc_io_buffer();
			if (result)
			{
				result.value().detach(buffer);
				return RX_PROTOCOL_OK;
			}
			else
			{
				return RX_PROTOCOL_OUT_OF_MEMORY;
			}
		};
	initiator_endpoint_.release_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
		{
			full_duplex_addr_packet_port<addrT>* whose = (full_duplex_addr_packet_port<addrT>*)entry->user_data;
			rx_io_buffer temp;
			temp.attach(buffer);
			whose->release_io_buffer(std::move(temp));

			return RX_PROTOCOL_OK;
		};
	if (ctx.get_item_static("Options.Initiator", false))
		new_state = new_state | port_state_wait_connecting;
	if (ctx.get_item_static("Options.Listener", false))
		new_state = new_state | port_state_wait_listening;
	state_ = new_state;
	return true;
}

template <typename addrT>
rx_result full_duplex_addr_packet_port<addrT>::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}

template <typename addrT>
rx_protocol_result_t full_duplex_addr_packet_port<addrT>::route_initiator_packet (recv_protocol_packet packet)
{
	auto key = address_adapter_type::get_key_from_packet_initiator(packet);
	auto key_list = address_adapter_type::get_key_from_packet_listener(packet);
	if (!key.first)
		return RX_PROTOCOL_INVALID_ADDR;
	rx_protocol_stack_endpoint* endpoint = nullptr;
	locks::auto_lock_t _(&routing_lock_);
	endpoint = find_initiators_endpoint(key.second);
	if (endpoint)
	{
		return rx_move_packet_up(endpoint, packet);
	}
	else
	{
		for (auto& intance : listener_endpoints_)
		{
			endpoint = intance.second->find_listener_endpoint(key_list.second, packet.to_addr, packet.from_addr);
			if (endpoint)
				break;
		}
		if (endpoint)
		{
			return rx_move_packet_up(endpoint, packet);
		}
		else
		{

			auto ep = construct_listener_endpoint(packet.to_addr, packet.from_addr);
			if (ep)
			{
				rx_result result = add_stack_endpoint(ep, packet.to_addr, packet.from_addr);
				if (result)
				{
					return rx_move_packet_up(ep, packet);
				}
				else
				{

					locks::auto_lock_t _(&routing_lock_);
					listener_endpoints_.erase(ep);
					return RX_PROTOCOL_INVALID_ADDR;
				}
			}
			else
			{
				return RX_PROTOCOL_INVALID_ADDR;
			}
		}
	}
}

template <typename addrT>
rx_protocol_result_t full_duplex_addr_packet_port<addrT>::initiator_connected_received (rx_session* session)
{
	addrT local, remote;
	if (!remote.parse(session->remote_addr))
		return RX_PROTOCOL_INVALID_ADDR;
	if (!local.parse(session->local_addr))
		return RX_PROTOCOL_INVALID_ADDR;
	state_ = current_port_state::port_state_connected;
	if (remote.is_null())
	{// notify everyone
		rx_protocol_result_t proto_result = RX_PROTOCOL_OK;
		locks::auto_lock_t _(&routing_lock_);
		for (auto& one : initiators_)
		{
			rx_session one_session = rx_create_session(&one.second->local_addr, &one.second->remote_addr, session->local_ref, session->remote_ref, session);
			proto_result = rx_notify_connected(&one.second->stack_endpoint, &one_session);
			if (proto_result != RX_PROTOCOL_OK)
				break;
		}
		return proto_result;
	}
	else
	{// notify the one
		rx_protocol_stack_endpoint* endpoint = nullptr;
		locks::auto_lock_t _(&routing_lock_);
		endpoint = find_initiators_endpoint(remote);
		if (endpoint)
		{
			return rx_notify_connected(endpoint, session);
		}
		else
		{
			return RX_PROTOCOL_INVALID_ADDR;
		}
	}

}

template <typename addrT>
rx_protocol_stack_endpoint* full_duplex_addr_packet_port<addrT>::find_initiators_endpoint (const key_type& key)
{
	auto it_cli = initiators_.find(key);
	if (it_cli != initiators_.end())
	{
		return &it_cli->second->stack_endpoint;
	}
	else
	{
		it_cli = initiators_.find(key_type());
		if (it_cli != initiators_.end())
			return &it_cli->second->stack_endpoint;
	}
	return nullptr;
}

template <typename addrT>
rx_protocol_result_t full_duplex_addr_packet_port<addrT>::remove_initiator (const key_type& key)
{
	auto it_cli = initiators_.find(key);
	if (it_cli != initiators_.end())
	{
		initiators_.erase(it_cli);
		return RX_PROTOCOL_OK;
	}
	else
	{
		return RX_PROTOCOL_INVALID_ADDR;
	}
}

template <typename addrT>
void full_duplex_addr_packet_port<addrT>::received_packet ()
{
	status.received_packet();
}

template <typename addrT>
void full_duplex_addr_packet_port<addrT>::sent_packet ()
{
	status.sent_packet();
}


// Parameterized Class rx_internal::interfaces::ports_lib::initiator_data_type 

template <typename addrT>
initiator_data_type<addrT>::initiator_data_type()
{
	ITF_LOG_DEBUG("initiator_endpoint", 200, "Routing initiator endpoint created.");
}


template <typename addrT>
initiator_data_type<addrT>::~initiator_data_type()
{
	ITF_LOG_DEBUG("initiator_endpoint", 200, "Routing initiator endpoint destroyed.");
}



// Class rx_internal::interfaces::ports_lib::ip4_routing_port 


void ip4_routing_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
	if (local_addr.is_null())
	{
		auto addr = binder_data.get_value("Bind.IPAddress");
		auto port = binder_data.get_value("Bind.IPPort");
		string_type addr_str;
		uint16_t port_val = 0;
		if (!addr.is_null())
		{
			addr_str = addr.extract_static(""s);
			addr_str = rx_gate::instance().resolve_ip4_alias(addr_str);
		}
		if (port.is_unassigned())
			port_val = port.extract_static(port_val);
		if (!addr_str.empty() || port_val != 0)
		{
			io::ip4_address ip_addr(addr_str, port_val);
			if (ip_addr.is_valid())
				local_addr = &ip_addr;
		}
	}
	if (remote_addr.is_null())
	{
		auto addr = binder_data.get_value("Connect.IPAddress");
		auto port = binder_data.get_value("Connect.IPPort");
		string_type addr_str;
		uint16_t port_val = 0;
		if (!addr.is_null())
		{
			addr_str = addr.extract_static(""s);
			addr_str = rx_gate::instance().resolve_ip4_alias(addr_str);
		}
		if (port.is_unassigned())
			port_val = port.extract_static(port_val);
		if (!addr_str.empty() || port_val != 0)
		{
			io::ip4_address ip_addr(addr_str, port_val);
			if (ip_addr.is_valid())
				remote_addr = &ip_addr;
		}
	}
}


// Parameterized Class rx_internal::interfaces::ports_lib::listener_data_type 

template <typename addrT>
listener_data_type<addrT>::listener_data_type()
      : my_instance(nullptr),
        binded(false)
{
	ITF_LOG_DEBUG("listener_data_type", 200, "Routing listener endpoint created.");
}


template <typename addrT>
listener_data_type<addrT>::~listener_data_type()
{
	ITF_LOG_DEBUG("listener_data_type", 200, "Routing listener endpoint destroyed.");
}



// Parameterized Class rx_internal::interfaces::ports_lib::listener_instance 

template <typename addrT>
listener_instance<addrT>::listener_instance()
{
	ITF_LOG_DEBUG("listener_instance", 200, "Routing listener instance endpoint created.");
}


template <typename addrT>
listener_instance<addrT>::~listener_instance()
{
	ITF_LOG_DEBUG("listener_instance", 200, "Routing listener instance endpoint destroyed.");
}



template <typename addrT>
rx_protocol_result_t listener_instance<addrT>::route_listeners_packet (recv_protocol_packet packet)
{
	auto key = address_adapter_type::get_key_from_packet_listener(packet);
	if (!key.first)
		return RX_PROTOCOL_INVALID_ADDR;
	rx_protocol_stack_endpoint* endpoint = nullptr;
	locks::auto_lock_t _(&routing_lock_);
	endpoint = find_listener_endpoint(key.second, packet.to_addr, packet.from_addr);
	if (endpoint)
	{
		return rx_move_packet_up(endpoint, packet);
	}
	else
	{
		return RX_PROTOCOL_INVALID_ADDR;
	}
}

template <typename addrT>
rx_protocol_result_t listener_instance<addrT>::listener_connected_received (rx_session* session)
{
	addrT local, remote;
	if (!remote.parse(session->remote_addr))
		return RX_PROTOCOL_INVALID_ADDR;
	if (!local.parse(session->local_addr))
		return RX_PROTOCOL_INVALID_ADDR;

	if (local.is_null())
	{// notify everyone
		rx_protocol_result_t proto_result = RX_PROTOCOL_OK;
		locks::auto_lock_t _(&routing_lock_);
		for (auto& one : listeners_)
		{
			if (one.second->binded)
			{
				rx_session one_session = rx_create_session(&one.second->local_addr, &one.second->remote_addr, session->local_ref, session->remote_ref, session);
				proto_result = rx_notify_connected(&one.second->stack_endpoint, &one_session);
				if (proto_result != RX_PROTOCOL_OK)
					break;
			}
		}
		return proto_result;
	}
	else
	{// notify the one
		rx_protocol_stack_endpoint* endpoint = nullptr;
		locks::auto_lock_t _(&routing_lock_);
		endpoint = find_listener_endpoint(listener_key_type(local, remote), &local, &remote);
		if (endpoint)
		{
			return rx_notify_connected(endpoint, session);
		}
		else
		{
			return RX_PROTOCOL_INVALID_ADDR;
		}
	}
}

template <typename addrT>
void listener_instance<addrT>::listener_closed_received (rx_protocol_result_t result)
{
	std::vector<listener_data_t*> listeners;
	{
		locks::auto_lock_t _(&routing_lock_);
		if (!listeners_.empty())
		{
			listeners.reserve(listeners_.size());
			for (auto & one : listeners_)
			{
				if(one.second->binded)
					listeners.push_back(one.second);
			}
		}
	}
	if (!listeners.empty())
	{
		for (auto& one : listeners)
		{
			rx_session session = rx_create_session(&one->local_addr, &one->remote_addr, 0, 0, nullptr);
			rx_notify_disconnected(&one->stack_endpoint, &session, result);
			rx_close(&one->stack_endpoint, result);
		}
	}
}

template <typename addrT>
rx_protocol_stack_endpoint* listener_instance<addrT>::find_listener_endpoint (const listener_key_type& key, const protocol_address* local_address, const protocol_address* remote_address)
{
	auto it_cli = listeners_.find(key);
	if (it_cli != listeners_.end())
	{
		if (!it_cli->second->binded)
		{
			auto result = my_port->add_stack_endpoint(&it_cli->second->stack_endpoint, local_address, remote_address);
			if (!result)
			{
				return nullptr;
			}
			else
			{
				ITF_LOG_DEBUG("listener_instance", 200, "Routing listener endpoint binded.");
				it_cli->second->binded = true;
				return &it_cli->second->stack_endpoint;
			}
		}
		else
		{
			return &it_cli->second->stack_endpoint;
		}
	}
	else
	{
		addrT rmt;
		rmt.parse(remote_address);
		if (rmt.is_null())
			return nullptr;

		std::unique_ptr<listener_data_t> new_listener = std::make_unique<listener_data_t>();
		new_listener->my_port = my_port;
		new_listener->my_instance = this;
		new_listener->local_addr.parse(local_address);
		new_listener->remote_addr.parse(remote_address);

		rx_protocol_stack_endpoint* new_endpoint = &new_listener->stack_endpoint;
		rx_init_stack_entry(new_endpoint, new_listener.get());
		new_endpoint->send_function = [](rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
		{
			listener_data_t* me = (listener_data_t*)reference->user_data;
			address_adapter_type::fill_send_packet(packet, *me);
			me->my_port->sent_packet();
			return rx_move_packet_down(&me->my_instance->stack_endpoint, packet);
		};
		new_endpoint->connected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session)
		{
			listener_data_t* me = (listener_data_t*)reference->user_data;
			return me->my_instance->listener_connected_received(session);
		};
		new_endpoint->allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
		{
			listener_data_t* me = (listener_data_t*)entry->user_data;
			auto result = me->my_instance->my_port->alloc_io_buffer();
			if (result)
			{
				result.value().detach(buffer);
				return RX_PROTOCOL_OK;
			}
			else
			{
				return RX_PROTOCOL_OUT_OF_MEMORY;
			}
		};
		new_endpoint->release_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
		{
			listener_data_t* me = (listener_data_t*)entry->user_data;
			rx_io_buffer temp;
			temp.attach(buffer);
			me->my_instance->my_port->release_io_buffer(std::move(temp));

			return RX_PROTOCOL_OK;
		};
		new_endpoint->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
		{
			listener_data_t* whose = reinterpret_cast<listener_data_t*>(entry->user_data);
			whose->my_instance->remove_listener(address_adapter_type::get_key_from_listener(*whose));
			whose->my_port->unbind_stack_endpoint(entry);
		};
		auto listener_raw = new_listener.get();
		//locks::auto_lock_t _(&routing_lock_); !!already locked in route_listeners_packet or listener_connected_received
		listeners_.emplace(key, listener_raw);
		auto emplace_result = my_port->listening_.emplace(new_endpoint, std::move(new_listener));
		if (!emplace_result.second)
		{
			new_endpoint = nullptr;
		}
		else
		{
			listener_raw->binded = true;
			auto result = my_port->add_stack_endpoint(new_endpoint, local_address, remote_address);
			if (!result)
			{
				listener_raw->binded = false;
				new_endpoint = nullptr;
			}
			else
			{
				ITF_LOG_DEBUG("listener_instance", 200, "Routing listener endpoint binded.");
			}

		}
		return new_endpoint;

	}
	return nullptr;
}

template <typename addrT>
rx_protocol_result_t listener_instance<addrT>::remove_listener (const listener_key_type& key)
{
	locks::auto_lock_t _(&routing_lock_);
	auto it_cli = listeners_.find(key);
	if (it_cli != listeners_.end())
	{
		listeners_.erase(it_cli);
		return RX_PROTOCOL_OK;
	}
	else
	{
		return RX_PROTOCOL_INVALID_ADDR;
	}
}


// Class rx_internal::interfaces::ports_lib::mac_routing_port 


void mac_routing_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
	if (local_addr.is_null())
	{
		auto addr = binder_data.get_value("Bind.Address");
		byte_string addr_val;
		if (addr.is_byte_string())
			addr_val = addr.extract_static(byte_string{});
		if (addr_val.size()==MAC_ADDR_SIZE)
		{
			io::mac_address mac_addr(addr_val);
			local_addr = &mac_addr;
		}
	}
	if (remote_addr.is_null())
	{
		auto addr = binder_data.get_value("Connect.Address");
		byte_string addr_val;
		if (addr.is_byte_string())
			addr_val = addr.extract_static(byte_string{});
		if (addr_val.size() == MAC_ADDR_SIZE)
		{
			io::mac_address mac_addr(addr_val);
			remote_addr = &mac_addr;
		}
	}
}


// Class rx_internal::interfaces::ports_lib::word_routing_port 


void word_routing_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
	if (local_addr.is_null())
	{
		auto addr = binder_data.get_value("Bind.Address");
		uint16_t addr_val = 0;
		if (addr.is_unassigned())
			addr_val = addr.extract_static(0);
		if (addr_val != 0)
		{
			io::numeric_address<uint16_t> num_addr(addr_val);
			local_addr = &num_addr;
		}
	}
	if (remote_addr.is_null())
	{
		auto addr = binder_data.get_value("Connect.Address");
		uint16_t addr_val = 0;
		if (addr.is_unassigned())
			addr_val = addr.extract_static(addr_val);
		if (addr_val != 0)
		{
			io::numeric_address<uint16_t> num_addr(addr_val);
			remote_addr = &num_addr;
		}
	}
}


// Class rx_internal::interfaces::ports_lib::string_routing_port 


void string_routing_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
	if (local_addr.is_null())
	{
		auto addr = binder_data.get_value("Bind.Address");
		string_type addr_val;
		if (addr.is_string())
			addr_val = addr.extract_static(string_type{});
		if (addr_val.size() == MAC_ADDR_SIZE)
		{
			io::string_address str_addr(addr_val);
			local_addr = &str_addr;
		}
	}
	if (remote_addr.is_null())
	{
		auto addr = binder_data.get_value("Connect.Address");
		string_type addr_val;
		if (addr.is_byte_string())
			addr_val = addr.extract_static(string_type{});
		if (addr_val.size() == MAC_ADDR_SIZE)
		{
			io::string_address str_addr(addr_val);
			remote_addr = &str_addr;
		}
	}
}


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal

template class rx_internal::interfaces::ports_lib::full_duplex_addr_packet_port<rx::io::ip4_address>;
template class rx_internal::interfaces::ports_lib::full_duplex_addr_packet_port<rx::io::numeric_address<uint8_t> >;
template class rx_internal::interfaces::ports_lib::full_duplex_addr_packet_port<rx::io::numeric_address<uint16_t> >;
template class rx_internal::interfaces::ports_lib::full_duplex_addr_packet_port<rx::io::mac_address >;
template class rx_internal::interfaces::ports_lib::full_duplex_addr_packet_port<rx::io::string_address >;
