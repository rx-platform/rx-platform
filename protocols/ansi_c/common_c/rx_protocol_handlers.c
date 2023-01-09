

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_protocol_handlers.c
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


// rx_protocol_handlers
#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"

#include "protocols/ansi_c/internal_c/rx_internal_impl.h"

rx_protocol_result_t rx_init_protocols(struct rx_hosting_functions* memory)
{
	if (memory)
	{
		g_memory = *memory;
		if (g_memory.alloc_buffer_function == NULL)
			g_memory.alloc_buffer_function = rx_c_alloc_buffer;
		if (g_memory.realloc_buffer_function == NULL)
			g_memory.realloc_buffer_function = rx_c_realloc_buffer;
		if (g_memory.alloc_function == NULL)
			g_memory.alloc_function = rx_c_alloc_buffer;
		if (g_memory.free_buffer_function == NULL)
			g_memory.free_buffer_function = rx_c_free_buffer;
		if (g_memory.free_function == NULL)
			g_memory.free_function = rx_c_free_buffer;
	}
	else
	{
		g_memory.alloc_buffer_function = rx_c_alloc_buffer;
		g_memory.realloc_buffer_function = rx_c_realloc_buffer;
		g_memory.alloc_function = rx_c_alloc_buffer;
		g_memory.free_buffer_function = rx_c_free_buffer;
		g_memory.free_function = rx_c_free_buffer;
	}
	return RX_PROTOCOL_OK;
}
rx_protocol_result_t rx_deinit_protocols()
{
	return RX_PROTOCOL_OK;
}


RX_COMMON_API void rx_init_stack_entry(struct rx_protocol_stack_endpoint* stack, void* user_data)
{
	memset(stack, 0, sizeof(struct rx_protocol_stack_endpoint));
	stack->user_data = user_data;
}

RX_COMMON_API rx_protocol_result_t rx_push_stack(struct rx_protocol_stack_endpoint* where_to, struct rx_protocol_stack_endpoint* what)
{
	struct rx_protocol_stack_endpoint* bottom = NULL;
	if (where_to->upward || what->downward)
	{
		return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
	}
	else
	{
		where_to->upward = what;
		what->downward = where_to;
		// notify everybody
		bottom = where_to;
		while (bottom->downward != NULL)
			bottom = bottom->downward;		
		while (bottom != NULL)
		{
			if (bottom->stack_changed_function)
				bottom->stack_changed_function(bottom);
			bottom = bottom->upward;
		}
		return RX_PROTOCOL_OK;
	}
}
RX_COMMON_API rx_protocol_result_t rx_pop_stack(struct rx_protocol_stack_endpoint* what)
{
	if (what->downward)
	{
		if (what->downward->upward)
		{
			what->downward->upward = NULL;
			what->downward = NULL;
			return RX_PROTOCOL_OK;
		}
	}
	return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
}

RX_COMMON_API recv_protocol_packet rx_create_recv_packet(rx_packet_id_type id, rx_const_packet_buffer* buffer, rx_address_reference_type from, rx_address_reference_type to)
{
	recv_protocol_packet ret;

	ret.id = id;
	ret.buffer = buffer;
	ret.from = from;
	ret.to = to;
	ret.from_addr = NULL;
	ret.to_addr = NULL;
	ret.priority = 0;
	return ret;
}

RX_COMMON_API send_protocol_packet rx_create_send_packet(rx_packet_id_type id, rx_packet_buffer* buffer, rx_address_reference_type from, rx_address_reference_type to)
{
	send_protocol_packet ret;
	ret.id = id;
	ret.buffer = buffer;
	ret.from = from;
	ret.to = to;
	ret.from_addr = NULL;
	ret.to_addr = NULL;
	ret.priority = 0;
	return ret;
}

RX_COMMON_API rx_session rx_create_session(const protocol_address* local, const protocol_address* remote, rx_address_reference_type local_ref, rx_address_reference_type remote_ref, const struct rx_session_def* next)
{
	rx_session ret;
	ret.local_addr = local;
	ret.remote_addr = remote;
	ret.local_ref = local_ref;
	ret.remote_ref = remote_ref;
	ret.next = next;

	return ret;
}
RX_COMMON_API rx_protocol_result_t rx_connect(struct rx_protocol_stack_endpoint* stack, struct rx_session_def* session)
{
	while (stack)
	{
		if (stack->connect_function)
		{
			return stack->connect_function(stack->downward, session);
		}
		stack = stack->downward;
	}
	return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
}

RX_COMMON_API rx_protocol_result_t rx_disconnect(struct rx_protocol_stack_endpoint* stack, struct rx_session_def* session, rx_protocol_result_t reason)
{
	while (stack != NULL)
	{
		if (stack->disconnect_function)
		{
			return stack->disconnect_function(stack->downward, session, reason);
		}
		if (stack->downward == NULL)
			break;
		stack = stack->downward;
	}
	if (stack)// just in case :)
	{
		// we didn't find it so do the default thing
		if (stack->disconnected_function)
		{
			stack->disconnected_function(stack, session, reason);
		}
		return rx_notify_disconnected(stack, session, reason);
	}
	return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
}

RX_COMMON_API rx_protocol_result_t rx_close(struct rx_protocol_stack_endpoint* stack, rx_protocol_result_t reason)
{
	while (stack != NULL)
	{
		if (stack->close_function)
		{
			return stack->close_function(stack, reason);
		}
		if (stack->downward == NULL)
			break;
		stack = stack->downward;
	}
	if (stack)// just in case :)
	{		
		// we didn't find it so do the default thing
		rx_notify_closed(stack, reason);
	}
	return RX_PROTOCOL_OK;
}


RX_COMMON_API rx_protocol_result_t rx_move_packet_down(struct rx_protocol_stack_endpoint* stack, send_protocol_packet packet)
{
	while (stack->downward != NULL)
	{
		if (stack->downward->send_function)
		{
			return stack->downward->send_function(stack->downward, packet);
		}
		stack = stack->downward;
	}
	return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
}
RX_COMMON_API rx_protocol_result_t rx_move_packet_up(struct rx_protocol_stack_endpoint* stack, recv_protocol_packet packet)
{
	while (stack->upward != NULL)
	{
		if (stack->upward->received_function)
		{
			return stack->upward->received_function(stack->upward, packet);
		}
		stack = stack->upward;
	}
	return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
}
RX_COMMON_API rx_protocol_result_t rx_notify_ack(struct rx_protocol_stack_endpoint* stack, rx_packet_id_type id, rx_protocol_result_t result)
{
	while (stack->upward != NULL)
	{
		if (stack->upward->ack_function)
		{
			return stack->upward->ack_function(stack->upward, id, result);
		}
		stack = stack->upward;
	}
	return RX_PROTOCOL_OK;// o.k. to ignore the result
}
RX_COMMON_API rx_protocol_result_t rx_notify_connected(struct rx_protocol_stack_endpoint* stack, struct rx_session_def* session)
{
	while (stack->upward != NULL)
	{
		if (stack->upward->connected_function)
		{
			return stack->upward->connected_function(stack->upward, session);
		}
		stack = stack->upward;
	}
	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_notify_disconnected(struct rx_protocol_stack_endpoint* stack, struct rx_session_def* session, rx_protocol_result_t reason)
{
	while (stack->upward != NULL)
	{
		if (stack->upward->disconnected_function)
		{
			return stack->upward->disconnected_function(stack->upward, session, reason);
		}
		stack = stack->upward;
	}
	return RX_PROTOCOL_OK;
}
RX_COMMON_API void rx_notify_closed(struct rx_protocol_stack_endpoint* stack, rx_protocol_result_t reason)
{
	struct rx_protocol_stack_endpoint* temp_stack;
	temp_stack = stack;
	while (stack != NULL)
	{
		temp_stack = stack;
		stack = stack->upward;
	}
	stack = temp_stack;
	while (stack != NULL)
	{
		temp_stack = stack->downward;
		rx_pop_stack(stack);
		if (stack->closed_function)
		{
			stack->closed_function(stack, reason);
		}
		stack = temp_stack;
	}
}


