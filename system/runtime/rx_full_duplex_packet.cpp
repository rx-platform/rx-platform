

/****************************************************************************
*
*  system\runtime\rx_full_duplex_packet.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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
#include "system/runtime/rx_full_duplex_packet.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_lib {
template <typename addrT>
typename duplex_port_adapter<addrT>::key_type duplex_port_adapter<addrT>::get_key_from_addresses(const addrT& local_address, const addrT& remote_address)
{
	return remote_address;
}
template <typename addrT>
std::pair<bool, typename duplex_port_adapter<addrT>::key_type> duplex_port_adapter<addrT>::get_key_from_packet(const recv_protocol_packet& packet)
{
	addrT local, remote;
	if (!remote.parse(packet.from_addr))
		return { false, key_type() };
	if (!local.parse(packet.to_addr))
		return { false, key_type() };
	return { true, remote };
}
template <typename addrT>
void duplex_port_adapter<addrT>::fill_send_packet(send_protocol_packet& packet, const initiator_data_type<addrT>& session_data)
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
	if (!result)
	{
		data.reset();
		return { key_type(), std::move(data) };
	}
	addrT key = data->remote_addr;
	return { key, std::move(data) };
}

// Parameterized Class rx_platform::runtime::io_types::ports_lib::full_duplex_addr_packet_port 

template <typename addrT>
full_duplex_addr_packet_port<addrT>::full_duplex_addr_packet_port()
      : state_(port_state_inactive)
{
}



template <typename addrT>
rx_protocol_stack_endpoint* full_duplex_addr_packet_port<addrT>::construct_endpoint ()
{
	return &initiators_endpoint_;
}

template <typename addrT>
rx_result full_duplex_addr_packet_port<addrT>::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
	if (state_ == port_state_inactive)
		return "Port not started!";
	return RX_NOT_IMPLEMENTED;
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
			return rx_move_packet_down(&me->my_port->initiators_endpoint_, packet);
		};
	stack_ep->close_function = [] (struct rx_protocol_stack_endpoint* reference, rx_protocol_result_t result) -> rx_protocol_result_t
		{
			rx_notify_closed(reference, RX_PROTOCOL_OK);
			rx_protocol_result_t ret = RX_PROTOCOL_OK;
			initiator_data_t* me = (initiator_data_t*)reference->user_data;
			if (me->my_port)
			{
				key_type key = address_adapter_type::get_key_from_addresses(me->local_addr, me->remote_addr);
				ret = me->my_port->remove_initiator(key);
			}
			return ret;
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
rx_result full_duplex_addr_packet_port<addrT>::start_runtime (runtime_start_context& ctx)
{
	if (state_ != port_state_inactive)
		return "Wrong port state!!!";
	current_port_state new_state = port_state_inactive;
	rx_init_stack_entry(&initiators_endpoint_, this);
	/*initiators_endpoint_.send_function = [](rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
		{
			full_duplex_addr_packet_port<addrT>* me = (full_duplex_addr_packet_port<addrT>*)reference->user_data;
			return me->send_packet(packet);
		};*/
	initiators_endpoint_.received_function = [](rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
		{
			full_duplex_addr_packet_port<addrT>* me = (full_duplex_addr_packet_port<addrT>*)reference->user_data;
			return me->route_initiator_packet(packet);
		};
	initiators_endpoint_.connected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session)
		{
			full_duplex_addr_packet_port<addrT>* me = (full_duplex_addr_packet_port<addrT>*)reference->user_data;
			return me->initiator_connected_received(session);
		};
	if (ctx.get_item_static("Initiator", false))
		new_state = new_state | port_state_wait_connecting;
	if (ctx.get_item_static("Listener", false))
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
	auto key = address_adapter_type::get_key_from_packet(packet);
	if (!key.first)
		return RX_PROTOCOL_INVALID_ADDR;
	rx_protocol_stack_endpoint* endpoint = nullptr;
	locks::auto_lock_t _(&routing_lock_);
	endpoint = find_initiators_endpoint(key.second);
	if (endpoint)
		return rx_move_packet_up(endpoint, packet);
	else
		return RX_PROTOCOL_INVALID_ADDR;
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


// Parameterized Class rx_platform::runtime::io_types::ports_lib::initiator_data_type 


// Parameterized Class rx_platform::runtime::io_types::ports_lib::listener_data_type 


// Class rx_platform::runtime::io_types::ports_lib::ip4_routing_port 


void ip4_routing_port::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
}

void ip4_routing_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
	if (local_addr.is_null())
	{
		auto addr = binder_data.get_value("Bind.IPAddress");
		auto port = binder_data.get_value("Bind.IPPort");
		string_type addr_str;
		uint16_t port_val = 0;
		if (!addr.is_null())
			addr_str = addr.get_storage().get_string_value();
		if (!port.is_null())
			port_val = (uint16_t)port.get_storage().get_integer_value();
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
			addr_str = addr.get_storage().get_string_value();
		if (!port.is_null())
			port_val = (uint16_t)port.get_storage().get_integer_value();
		if (!addr_str.empty() || port_val != 0)
		{
			io::ip4_address ip_addr(addr_str, port_val);
			if (ip_addr.is_valid())
				remote_addr = &ip_addr;
		}
	}
}


// Class rx_platform::runtime::io_types::ports_lib::byte_routing_port 


void byte_routing_port::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
}

void byte_routing_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
	if (local_addr.is_null())
	{
		auto addr = binder_data.get_value("Bind.Address");
		uint8_t addr_val = 0;
		if (!addr.is_null())
			addr_val = (uint8_t)addr.get_storage().get_integer_value();
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
		if (!addr.is_null())
			addr_val = (uint8_t)addr.get_storage().get_integer_value();
		if (addr_val != 0)
		{
			io::numeric_address<uint8_t> num_addr(addr_val);
			remote_addr = &num_addr;
		}
	}
}


} // namespace ports_lib
} // namespace io_types
} // namespace runtime
} // namespace rx_platform

template class rx_platform::runtime::io_types::ports_lib::full_duplex_addr_packet_port<rx::io::ip4_address>;
template class rx_platform::runtime::io_types::ports_lib::full_duplex_addr_packet_port<rx::io::numeric_address<uint8_t> >;
