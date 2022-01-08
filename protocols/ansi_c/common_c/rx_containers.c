

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_containers.c
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


// rx_containers
#include "protocols/ansi_c/common_c/rx_containers.h"

#include "protocols/ansi_c/internal_c/rx_internal_impl.h"

RX_COMMON_API rx_protocol_result_t rx_init_queue(rx_packet_queue* queue, size_t initial_capacity)
{
	rx_protocol_result_t result;

	result = g_memory.alloc_function((void**)&(queue->buffers), initial_capacity * sizeof(rx_packet_buffer));
	if (result != RX_PROTOCOL_OK)
		return result;
	queue->capacity = initial_capacity;
	queue->size = 0;

	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_deinit_queue(rx_packet_queue* queue)
{
	rx_protocol_result_t result;
	size_t i;

	for (i = 0; i < queue->size; i++)
	{
		result = g_memory.free_buffer_function(queue->buffers[i].buffer_ptr, queue->buffers[i].capacity);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	return g_memory.free_function(queue->buffers, queue->capacity * sizeof(rx_packet_buffer));
}

RX_COMMON_API int rx_queue_empty(const rx_packet_queue* queue)
{
	return queue->size == 0 ? 1 : 0;
}
rx_protocol_result_t rx_handle_queue_resize(rx_packet_queue* queue, size_t to_add)
{
	rx_protocol_result_t result;
	void* temp_buffer;
	size_t new_capacity;

	if (queue->capacity < queue->size + to_add)
	{
		new_capacity = (queue->capacity << 1);
		while (new_capacity < queue->size + to_add)
			new_capacity <<= 1;
		result = g_memory.alloc_function(&temp_buffer, new_capacity * sizeof(rx_packet_buffer));
		if (result != RX_PROTOCOL_OK)
			return result;
		memcpy(temp_buffer, queue->buffers, queue->size * sizeof(rx_packet_buffer));
		result = g_memory.free_function(queue->buffers, queue->capacity);
		queue->buffers = temp_buffer;
		queue->capacity = new_capacity;
		if (result != RX_PROTOCOL_OK)
			return result;
	}

	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_enqueue(rx_packet_queue* queue, const rx_packet_buffer* buffer)
{
	rx_protocol_result_t result;

	result = rx_handle_queue_resize(queue, 1);
	if (result != RX_PROTOCOL_OK)
		return result;
	queue->buffers[queue->size] = *buffer;
	queue->size++;

	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_dequeue(rx_packet_queue* queue, rx_packet_buffer* buffer)
{
	size_t to_move;

	if (queue->size == 0)
		return RX_PROTOCOL_EMPTY;
	*buffer = queue->buffers[0];
	queue->size--;
	to_move = queue->size * sizeof(rx_packet_buffer);
	if (to_move > 0)
	{
		memmove(&queue->buffers[0], &queue->buffers[1], to_move);
	}

	return RX_PROTOCOL_OK;
}



RX_COMMON_API rx_protocol_result_t rx_init_stack(rx_packet_stack* stack, size_t initial_capacity)
{
	rx_protocol_result_t result;

	result = g_memory.alloc_function((void**)&(stack->buffers), initial_capacity * sizeof(rx_packet_buffer));
	if (result != RX_PROTOCOL_OK)
		return result;
	stack->capacity = initial_capacity;
	stack->size = 0;

	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_deinit_stack(rx_packet_stack* stack)
{
	rx_protocol_result_t result;
	size_t i;

	for (i = 0; i < stack->size; i++)
	{
		result = g_memory.free_buffer_function(stack->buffers[i].buffer_ptr, stack->buffers[i].capacity);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	return g_memory.free_function(stack->buffers, stack->capacity * sizeof(rx_packet_buffer));
}

RX_COMMON_API int rx_stack_empty(const rx_packet_stack* stack)
{
	return stack->size == 0 ? 1 : 0;
}
rx_protocol_result_t rx_handle_stack_resize(rx_packet_stack* stack, size_t to_add)
{
	rx_protocol_result_t result;
	void* temp_buffer;
	size_t new_capacity;

	if (stack->capacity < stack->size + to_add)
	{
		new_capacity = (stack->capacity << 1);
		while (new_capacity < stack->size + to_add)
			new_capacity <<= 1;
		result = g_memory.alloc_function(&temp_buffer, new_capacity * sizeof(rx_packet_buffer));
		if (result != RX_PROTOCOL_OK)
			return result;
		memcpy(temp_buffer, stack->buffers, stack->size * sizeof(rx_packet_buffer));
		result = g_memory.free_function(stack->buffers, stack->capacity);
		stack->buffers = temp_buffer;
		stack->capacity = new_capacity;
		if (result != RX_PROTOCOL_OK)
			return result;
	}

	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_push(rx_packet_stack* stack, const rx_packet_buffer* buffer)
{
	rx_protocol_result_t result;

	result = rx_handle_stack_resize(stack, 1);
	if (result != RX_PROTOCOL_OK)
		return result;
	stack->buffers[stack->size] = *buffer;
	stack->size++;

	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_pop(rx_packet_stack* stack, rx_packet_buffer* buffer)
{
	if (stack->size == 0)
		return RX_PROTOCOL_EMPTY;
	stack->size--;
	*buffer = stack->buffers[stack->size];
	return RX_PROTOCOL_OK;
}




