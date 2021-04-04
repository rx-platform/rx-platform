

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_packet_buffer.c
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


// rx_packet_buffer
#include "protocols/ansi_c/common_c/rx_packet_buffer.h"

#include "protocols/ansi_c/internal_c/rx_internal_impl.h"

rx_protocol_result_t rx_init_const_packet_buffer(rx_const_packet_buffer* buffer, const void* data, size_t size)
{
	// force it through!!!
	buffer->buffer_ptr = data;
	*((size_t*)&buffer->size) = size;
	buffer->next_read = 0;

	return RX_PROTOCOL_OK;
}
rx_protocol_result_t rx_init_const_from_packet_buffer(rx_const_packet_buffer* buffer, const rx_packet_buffer* from)
{
	// force it through!!!
	buffer->buffer_ptr = from->buffer_ptr;
	*((size_t*)&buffer->size) = from->size;
	buffer->next_read = 0;

	return RX_PROTOCOL_OK;
}
rx_protocol_result_t rx_init_packet_buffer(rx_packet_buffer* buffer, size_t initial_capacity, struct rx_protocol_stack_endpoint* whose)
{
	rx_protocol_result_t result;

	while (whose)
	{
		if (whose->allocate_packet_function
			&& whose->free_packet_function)// we're in problem if this is not 
		{
			result = whose->allocate_packet_function(whose, buffer, initial_capacity);
			buffer->whose = whose;
			
			return result;
		}
		else
			whose = whose->downward;
	}

	result = g_memory.alloc_buffer_function((void**)&(buffer->buffer_ptr), initial_capacity);
	buffer->whose = NULL;

	if (result != RX_PROTOCOL_OK)
		return result;
	buffer->capacity = initial_capacity;
	buffer->size = 0;

	return RX_PROTOCOL_OK;
}
rx_protocol_result_t rx_deinit_packet_buffer(rx_packet_buffer* buffer)
{
	if (buffer->whose && buffer->whose->free_packet_function)
		return buffer->whose->free_packet_function(buffer->whose, buffer);
	else
		return g_memory.free_function(buffer->buffer_ptr, buffer->capacity);
}

rx_protocol_result_t rx_reinit_packet_buffer(rx_packet_buffer* buffer)
{
	buffer->size = 0;
	return RX_PROTOCOL_OK;
}
rx_protocol_result_t rx_reinit_const_packet_buffer(rx_const_packet_buffer* buffer)
{
	buffer->next_read = 0;
	return RX_PROTOCOL_OK;
}

size_t rx_get_packet_usable_data(const rx_packet_buffer* buffer)
{
	return buffer->size;
}
void* rx_get_packet_data_at(rx_packet_buffer* buffer, size_t pos, rx_protocol_result_t* result)
{
	if (pos < buffer->size)
	{
		*result = RX_PROTOCOL_OK;
		return &buffer->buffer_ptr[pos];
	}
	else
	{
		*result = RX_PROTOCOL_BUFFER_SIZE_ERROR;
		return NULL;
	}
}
size_t rx_get_packet_available_data(const rx_const_packet_buffer* buffer)
{
	return buffer->size - buffer->next_read;
}
int rx_buffer_eof(const rx_const_packet_buffer* buffer)
{
	assert(buffer->next_read <= buffer->size);
	return buffer->next_read == buffer->size;
}

rx_protocol_result_t rx_handle_buffer_resize(rx_packet_buffer* buffer, size_t to_add)
{
	rx_protocol_result_t result;
	void* temp_ptr;
	rx_packet_buffer temp_buffer;
	size_t new_capacity;

	if (buffer->capacity < buffer->size + to_add)
	{
		new_capacity = (buffer->capacity << 1);
		if (new_capacity == 0)
			new_capacity = sizeof(void*);
		while (new_capacity < buffer->size + to_add)
			new_capacity <<= 1;
		if (buffer->whose && buffer->whose->allocate_packet_function)
		{
			result = buffer->whose->allocate_packet_function(buffer->whose, &temp_buffer , new_capacity);
			if (result != RX_PROTOCOL_OK)
				return result;
			memcpy(temp_buffer.buffer_ptr, buffer->buffer_ptr, buffer->size);
			if (buffer->whose && buffer->whose->free_packet_function)
				result = buffer->whose->free_packet_function(buffer->whose, buffer);
			else
				assert(0);
			buffer->buffer_ptr = temp_buffer.buffer_ptr;
			buffer->capacity = new_capacity;
		}
		else
		{
			result = g_memory.alloc_function(&temp_ptr, new_capacity);
			if (result != RX_PROTOCOL_OK)
				return result;
			memcpy(temp_ptr, buffer->buffer_ptr, buffer->size);
			result = g_memory.free_function(buffer->buffer_ptr, buffer->capacity);
			buffer->buffer_ptr = temp_ptr;
			buffer->capacity = new_capacity;
		}
		if (result != RX_PROTOCOL_OK)
			return result;
	}

	return RX_PROTOCOL_OK;
}
void* rx_alloc_from_packet(rx_packet_buffer* buffer, size_t size, rx_protocol_result_t* result)
{
	void* ret = NULL;

	*result = rx_handle_buffer_resize(buffer, size);
	if (*result != RX_PROTOCOL_OK)
		return ret;
	ret = &buffer->buffer_ptr[buffer->size];
	buffer->size += size;

	return ret;
}
rx_protocol_result_t rx_push_to_packet(rx_packet_buffer* buffer, const void* buffer_ptr, size_t size)
{
	void* temp;
	rx_protocol_result_t result;

	temp = rx_alloc_from_packet(buffer, size, &result);
	if(temp)
		memcpy(temp, buffer_ptr, size);

	return result;
}

const void* rx_get_from_packet(rx_const_packet_buffer* buffer, size_t size, rx_protocol_result_t* result)
{
	const void* ret = NULL;

	if (buffer->next_read + size > buffer->size)
		*result = RX_PROTOCOL_INSUFFICIENT_DATA;
	else
		*result = RX_PROTOCOL_OK;

	ret = &buffer->buffer_ptr[buffer->next_read];
	buffer->next_read += size;

	return ret;

}
rx_protocol_result_t rx_pop_from_packet(rx_const_packet_buffer* buffer, void* buffer_ptr, size_t size)
{
	const void* temp;
	rx_protocol_result_t result;

	temp = rx_get_from_packet(buffer, size, &result);
	if(temp)
		memcpy(buffer_ptr, temp, size);

	return result;
}





