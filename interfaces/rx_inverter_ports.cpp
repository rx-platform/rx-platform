

/****************************************************************************
*
*  interfaces\rx_inverter_ports.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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

#include "system/runtime/rx_runtime_helpers.h"
using namespace rx_platform::runtime;

// rx_inverter_ports
#include "interfaces/rx_inverter_ports.h"

#include "rx_endpoints.h"


namespace rx_internal {

namespace interfaces {

namespace ports_lib {

// Class rx_internal::interfaces::ports_lib::listener_to_initiator_port 

listener_to_initiator_port::listener_to_initiator_port()
      : state_(port_state_inactive)
{
}


listener_to_initiator_port::~listener_to_initiator_port()
{
}



void listener_to_initiator_port::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
}

rx_result listener_to_initiator_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    return RX_NOT_SUPPORTED;
}

void listener_to_initiator_port::stack_assembled ()
{
}

void listener_to_initiator_port::stack_disassembled ()
{
}

rx_result listener_to_initiator_port::stop_passive ()
{
    return true;
}


// Class rx_internal::interfaces::ports_lib::initiator_to_listener_port 

initiator_to_listener_port::initiator_to_listener_port()
      : state_(port_state_inactive)
{

	rx_init_stack_entry(&initiator_endpoint_, this);
	initiator_endpoint_.send_function = [](rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
	{
		initiator_to_listener_port* me = (initiator_to_listener_port*)reference->user_data;
		return me->move_packet_down(packet);
	};
	initiator_endpoint_.allocate_packet = [](rx_protocol_stack_endpoint* entry, rx_packet_buffer* buffer)->rx_protocol_result_t
	{
		initiator_to_listener_port* whose = (initiator_to_listener_port*)entry->user_data;
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
		initiator_to_listener_port* whose = (initiator_to_listener_port*)entry->user_data;
		io::rx_io_buffer temp;
		temp.attach(buffer);
		whose->release_io_buffer(std::move(temp));

		return RX_PROTOCOL_OK;
	};
}


initiator_to_listener_port::~initiator_to_listener_port()
{
}



void initiator_to_listener_port::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
	if (active_endpoint_ && &active_endpoint_->stack_endpoint == what)
		active_endpoint_.reset();
}

rx_result_with<port_connect_result> initiator_to_listener_port::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{

	return port_connect_result(&initiator_endpoint_, (bool)active_endpoint_);
}

void initiator_to_listener_port::stack_assembled ()
{
	listen(nullptr, nullptr);
}

void initiator_to_listener_port::stack_disassembled ()
{
	current_port_state new_state = state_;
	new_state = new_state | port_state_wait_listening;
	state_ = new_state;
}

rx_result initiator_to_listener_port::stop_passive ()
{
    return true;
}

rx_result initiator_to_listener_port::start_runtime (runtime_start_context& ctx)
{
	state_ = port_state_wait_listening | port_state_wait_connecting;
	return true;
}

rx_result initiator_to_listener_port::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}

rx_protocol_stack_endpoint* initiator_to_listener_port::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    if (active_endpoint_)
    {
        return NULL;
    }
    else
    {
        active_endpoint_ = std::make_unique<i2l_listener_endpoint>(this);
        return &active_endpoint_->stack_endpoint;
    }
}

rx_protocol_result_t initiator_to_listener_port::move_packet_down (send_protocol_packet packet)
{
	if (active_endpoint_)
	{
		return rx_move_packet_down(&active_endpoint_->stack_endpoint, packet);
	}
	else
	{
		return RX_PROTOCOL_DISCONNECTED;
	}
}

rx_protocol_result_t initiator_to_listener_port::move_packet_up (recv_protocol_packet packet)
{
	return rx_move_packet_up(&initiator_endpoint_, packet);
}

rx_protocol_result_t initiator_to_listener_port::listener_connected (rx_session* session)
{
	return rx_notify_connected(&initiator_endpoint_, session);
}

rx_protocol_result_t initiator_to_listener_port::listener_disconnected (rx_session* session, rx_protocol_result_t reason)
{
	return rx_notify_disconnected(&initiator_endpoint_, session, reason);
}

void initiator_to_listener_port::listener_closed (rx_protocol_result_t reason)
{
}


// Class rx_internal::interfaces::ports_lib::i2l_listener_endpoint 

i2l_listener_endpoint::i2l_listener_endpoint (initiator_to_listener_port* port)
      : my_port_(port)
{
	ITF_LOG_DEBUG("i2l_listener_endpoint", 200, "I2L listener endpoint created.");
	rx_init_stack_entry(&stack_endpoint, this);
	stack_endpoint.connected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session)
	{
		i2l_listener_endpoint* self = reinterpret_cast<i2l_listener_endpoint*>(reference->user_data);
		return self->connected(session);
	};
	stack_endpoint.received_function = [](rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
	{
		i2l_listener_endpoint* self = reinterpret_cast<i2l_listener_endpoint*>(reference->user_data);
		return self->received_packet(packet);
	};
	stack_endpoint.disconnected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
	{
		i2l_listener_endpoint* self = reinterpret_cast<i2l_listener_endpoint*>(reference->user_data);
		return self->disconnected(session, reason);
	};
	stack_endpoint.closed_function = [](rx_protocol_stack_endpoint* reference, rx_protocol_result_t reason)
	{
		i2l_listener_endpoint* self = reinterpret_cast<i2l_listener_endpoint*>(reference->user_data);
		self->closed(reason);
		self->my_port_->unbind_stack_endpoint(reference);
	};
}


i2l_listener_endpoint::~i2l_listener_endpoint()
{
	ITF_LOG_DEBUG("i2l_listener_endpoint", 200, "I2L listener endpoint destroyed.");
}



rx_protocol_result_t i2l_listener_endpoint::connected (rx_session* session)
{
	return my_port_->listener_connected(session);
}

rx_protocol_result_t i2l_listener_endpoint::received_packet (recv_protocol_packet packet)
{
	return my_port_->move_packet_up(packet);
}

rx_protocol_result_t i2l_listener_endpoint::disconnected (rx_session* session, rx_protocol_result_t reason)
{
	return my_port_->listener_disconnected(session, reason);
	return RX_PROTOCOL_OK;
}

void i2l_listener_endpoint::closed (rx_protocol_result_t reason)
{
	my_port_->listener_closed(reason);
}


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal

