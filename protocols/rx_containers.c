

/****************************************************************************
*
*  protocols\rx_containers.c
*
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


// rx_containers
#include "protocols/rx_containers.h"


rx_transport_result_t rx_c_alloc_buffer(void** buffer, size_t buffer_size)
{
	*buffer = malloc(buffer_size);
	if (buffer)
	{
		return RX_PROTOCOL_OK;
	}
	else
	{
		return RX_PROTOCOL_OUT_OF_MEMORY;
	}
}
rx_transport_result_t rx_c_free_buffer(void* buffer, size_t buffer_size)
{
	free(buffer);
	return RX_PROTOCOL_OK;
}


rx_transport_result_t rx_init_queue(rx_transport_queue* queue, size_t initial_capacity, rx_memory_functions* memory)
{
	rx_transport_result_t result;

	result = memory->alloc_function((void**)&(queue->buffers), initial_capacity * sizeof(rx_transport_buffer));
	if (result != RX_PROTOCOL_OK)
		return result;
	queue->capacity = initial_capacity;
	queue->size = 0;

	return RX_PROTOCOL_OK;
}
rx_transport_result_t rx_deinit_queue(rx_transport_queue* queue, rx_memory_functions* memory)
{
	rx_transport_result_t result;
	size_t i;

	for (i = 0; i < queue->size; i++)
	{
		result = memory->free_buffer_function(queue->buffers[i].buffer_ptr, queue->buffers[i].size);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	return memory->free_function(queue->buffers, queue->capacity * sizeof(rx_transport_buffer));
}

int rx_queue_empty(const rx_transport_queue* queue)
{
	return queue->size == 0 ? 0 : 1;
}
rx_transport_result_t rx_handle_queue_resize(rx_transport_queue* queue, size_t to_add, rx_memory_functions* memory)
{
	rx_transport_result_t result;
	void* temp_buffer;
	size_t new_capacity;

	if (queue->capacity < queue->size + to_add)
	{
		new_capacity = (queue->capacity << 1);
		while (new_capacity < queue->size + to_add)
			new_capacity <<= 1;
		result = memory->alloc_function(&temp_buffer, new_capacity * sizeof(rx_transport_buffer));
		if (result != RX_PROTOCOL_OK)
			return result;
		memcpy(temp_buffer, queue->buffers, queue->size * sizeof(rx_transport_buffer));
		result = memory->free_function(queue->buffers, queue->capacity);
		queue->buffers = temp_buffer;
		queue->capacity = new_capacity;
		if (result != RX_PROTOCOL_OK)
			return result;
	}

	return RX_PROTOCOL_OK;
}
rx_transport_result_t rx_push_to_queue(rx_transport_queue* queue, uint8_t* buffer_ptr, size_t size, rx_memory_functions* memory)
{
	rx_transport_result_t result;

	result = rx_handle_queue_resize(queue, 1, memory);
	if (result != RX_PROTOCOL_OK)
		return result;
	queue->buffers[queue->size].buffer_ptr = buffer_ptr;
	queue->buffers[queue->size].size = size;
	queue->size++;

	return RX_PROTOCOL_OK;
}
rx_transport_result_t rx_pop_from_queue(rx_transport_queue* queue, uint8_t** buffer_ptr, size_t* size)
{
	size_t to_move;

	if (queue->size == 0)
		return RX_PROTOCOL_EMPTY;
	*buffer_ptr = queue->buffers[0].buffer_ptr;
	*size = queue->buffers[0].size;
	queue->size--;
	to_move = queue->size * sizeof(rx_transport_buffer);
	if (to_move > 0)
	{
		memmove(&queue->buffers[0], &queue->buffers[1], to_move);
	}

	return RX_PROTOCOL_OK;
}



rx_transport_result_t rx_init_packet_collector(rx_transport_packet_collector* collector, size_t initial_capacity, rx_memory_functions* memory)
{
	rx_transport_result_t result;

	result = memory->alloc_buffer_function((void**)&(collector->buffer), initial_capacity);
	if (result != RX_PROTOCOL_OK)
		return result;
	collector->capacity = initial_capacity;
	collector->buffer.size = 0;

	return RX_PROTOCOL_OK;
}
rx_transport_result_t rx_deinit_packet_collector(rx_transport_packet_collector* collector, rx_memory_functions* memory)
{
	return memory->free_function(collector->buffer.buffer_ptr, collector->capacity);
}
int rx_collector_empty(const rx_transport_packet_collector* collector)
{
	return collector->buffer.size == 0 ? 0 : 1;
}
rx_transport_result_t rx_handle_collector_resize(rx_transport_packet_collector* collector, size_t to_add, rx_memory_functions* memory)
{
	rx_transport_result_t result;
	void* temp_buffer;
	size_t new_capacity;

	if (collector->capacity < collector->buffer.size + to_add)
	{
		new_capacity = (collector->capacity << 1);
		while (new_capacity < collector->buffer.size + to_add)
			new_capacity <<= 1;
		result = memory->alloc_function(&temp_buffer, new_capacity);
		if (result != RX_PROTOCOL_OK)
			return result;
		memcpy(temp_buffer, collector->buffer.buffer_ptr, collector->buffer.size);
		result = memory->free_function(collector->buffer.buffer_ptr, collector->capacity);
		collector->buffer.buffer_ptr = temp_buffer;
		collector->capacity = new_capacity;
		if (result != RX_PROTOCOL_OK)
			return result;
	}

	return RX_PROTOCOL_OK;
}
rx_transport_result_t rx_push_to_packet_collector_end(rx_transport_packet_collector* collector, uint8_t* buffer_ptr, size_t size, rx_memory_functions* memory)
{
	rx_transport_result_t result;

	result = rx_handle_collector_resize(collector, size, memory);
	if (result != RX_PROTOCOL_OK)
		return result;
	memcpy(&collector->buffer.buffer_ptr[collector->buffer.size], buffer_ptr, size);
	collector->buffer.size += size;

	return RX_PROTOCOL_OK;
}
rx_transport_result_t rx_get_collected_packet(rx_transport_packet_collector* collector, uint8_t** buffer_ptr, size_t* size)
{
	if (collector->buffer.size == 0)
		return RX_PROTOCOL_EMPTY;
	*buffer_ptr = collector->buffer.buffer_ptr;
	*size = collector->buffer.size;
	return RX_PROTOCOL_OK;
}
rx_transport_result_t rx_reinit_packet_collector(rx_transport_packet_collector* collector)
{
	collector->buffer.size = 0;
	return RX_PROTOCOL_OK;
}


