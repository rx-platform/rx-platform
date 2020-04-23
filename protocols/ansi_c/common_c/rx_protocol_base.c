

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_protocol_base.c
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_protocol_base
#include "protocols/ansi_c/common_c/rx_protocol_base.h"

#include "protocols/ansi_c/internal_c/rx_internal_impl.h"

rx_protocol_result_t rx_init_protocols(struct rx_hosting_functions* memory)
{
	if (memory)
	{
		g_memory = *memory;
		if (g_memory.alloc_buffer_function == NULL)
			g_memory.alloc_buffer_function = rx_c_alloc_buffer;
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

rx_protocol_result_t rx_push_stack(struct rx_protocol_stack_entry* where_to, struct rx_protocol_stack_entry* what)
{
	if (where_to->upward || what->downward)
	{
		return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
	}
	else
	{
		where_to->upward = what;
		what->downward = where_to;
		return RX_PROTOCOL_OK;
	}
}
rx_protocol_result_t rx_pop_stack(struct rx_protocol_stack_entry* what)
{
	if (what->downward)
	{
		if (what->downward->upward)
		{
			what->downward->upward = NULL;
			what->downward = NULL;
		}
	}
	return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
}

rx_protocol_result_t rx_move_packet_down(struct rx_protocol_stack_entry* stack, rx_packet_buffer* buffer)
{
	while (stack->downward != NULL)
	{
		if (stack->downward->send_function)
		{
			return stack->downward->send_function(stack->downward, buffer);
		}
		stack = stack->downward;
	}
	return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
}
rx_protocol_result_t rx_move_packet_up(struct rx_protocol_stack_entry* stack, rx_const_packet_buffer* buffer)
{
	while (stack->upward != NULL)
	{
		if (stack->upward->received_function)
		{
			return stack->upward->received_function(stack->upward, buffer);
		}
		stack = stack->upward;
	}
	return RX_PROTOCOL_STACK_STRUCTURE_ERROR;
}
rx_protocol_result_t rx_move_result_up(struct rx_protocol_stack_entry* stack, rx_protocol_result_t result)
{
	while (stack->upward != NULL)
	{
		if (stack->upward->sent_function)
		{
			return stack->upward->sent_function(stack->upward, result);
		}
		stack = stack->upward;
	}
	return RX_PROTOCOL_OK;// o.k. to ignore the result
}
void rx_send_connected(struct rx_protocol_stack_entry* stack)
{
	while (stack->upward != NULL)
	{
		if (stack->upward->connected_function)
		{
			stack->upward->connected_function(stack->upward);
			return;
		}
		stack = stack->upward;
	}
}
void rx_send_close(struct rx_protocol_stack_entry* stack)
{
	while (stack->downward != NULL)
	{
		if (stack->downward->close_function)
		{
			stack->downward->close_function(stack->downward);
			return;
		}
		stack = stack->upward;
	}
}
rx_protocol_result_t rx_allocate_packet(struct rx_protocol_stack_entry* stack, rx_packet_buffer* buffer, size_t initial_capacity)
{
	if (stack)
	{
		while (stack->downward != NULL && stack->allocate_packet_function == NULL)
			stack = stack->downward;

		if (stack->allocate_packet_function != NULL)
			return stack->allocate_packet_function(stack, buffer, initial_capacity);
	}
	return g_memory.alloc_buffer_function((void**)&buffer->buffer_ptr, initial_capacity);
}

rx_protocol_result_t rx_free_packet(rx_packet_buffer* buffer)
{
	if (buffer->whose && buffer->whose->free_packet_function)
	{
		return buffer->whose->free_packet_function(buffer->whose, buffer);
	}
	else
	{
		return g_memory.free_buffer_function(buffer->buffer_ptr, buffer->capacity);
	}
}


