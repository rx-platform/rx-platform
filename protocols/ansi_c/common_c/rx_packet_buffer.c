

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_packet_buffer.c
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


// rx_packet_buffer
#include "protocols/ansi_c/common_c/rx_packet_buffer.h"

#include "protocols/ansi_c/internal_c/rx_internal_impl.h"

#ifdef _DEBUG
#define HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer)  \
	assert(buffer->capacity + buffer->front_capacity >= buffer->size + (buffer->buffer_ptr - buffer->mem_ptr));\

#else
#define HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer) 

#endif


RX_COMMON_API rx_protocol_result_t rx_init_packet_buffer(rx_packet_buffer* buffer, size_t initial_capacity, size_t initial_front_capacity)
{
	rx_protocol_result_t result;

	result = g_memory.alloc_buffer_function((void**)&(buffer->mem_ptr), initial_capacity + initial_front_capacity);

	if (result != RX_PROTOCOL_OK)
		return result;
	buffer->buffer_ptr = buffer->mem_ptr + initial_front_capacity;
	buffer->capacity = initial_capacity;
	buffer->front_capacity = initial_front_capacity;
	buffer->size = 0;

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_deinit_packet_buffer(rx_packet_buffer* buffer)
{
	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);
	return g_memory.free_function(buffer->mem_ptr, buffer->capacity + buffer->front_capacity);
}

RX_COMMON_API rx_protocol_result_t rx_reinit_packet_buffer(rx_packet_buffer* buffer)
{
	buffer->size = 0;
	buffer->buffer_ptr = buffer->mem_ptr + buffer->front_capacity;

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	return RX_PROTOCOL_OK;
}
RX_COMMON_API size_t rx_get_packet_usable_data(const rx_packet_buffer* buffer)
{
	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	return buffer->size;
}




rx_protocol_result_t rx_handle_buffer_back_resize(rx_packet_buffer* buffer, size_t to_add)
{
	rx_protocol_result_t result;
	uint8_t* temp_ptr;
	size_t new_capacity;
	size_t front_reserve = buffer->buffer_ptr - buffer->mem_ptr;

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	if (buffer->capacity + buffer->front_capacity < buffer->size + to_add + front_reserve)
	{
		new_capacity = (buffer->capacity << 1);
		if (new_capacity == 0)
			new_capacity = sizeof(void*);
		while (new_capacity + buffer->front_capacity < buffer->size + to_add + front_reserve)
			new_capacity <<= 1;

		temp_ptr = buffer->mem_ptr;
		if (temp_ptr == NULL)
		{
			result = g_memory.alloc_buffer_function(&temp_ptr, new_capacity + buffer->front_capacity);
		}
		else
		{
			result = g_memory.realloc_buffer_function(&temp_ptr, buffer->capacity + new_capacity);
		}
		if (result != RX_PROTOCOL_OK)
			return result;
		buffer->mem_ptr = temp_ptr;
		buffer->buffer_ptr = buffer->mem_ptr + front_reserve;
		buffer->capacity = new_capacity;
	}

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	return RX_PROTOCOL_OK;
}

rx_protocol_result_t rx_handle_buffer_front_resize(rx_packet_buffer* buffer, size_t to_add)
{
	rx_protocol_result_t result;
	uint8_t* temp_ptr;
	size_t new_capacity, diff_capacity;
	size_t front_reserve = buffer->buffer_ptr - buffer->mem_ptr;

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	if (to_add > front_reserve)
	{
		new_capacity = (buffer->front_capacity << 1);
		if (new_capacity == 0)
			new_capacity = sizeof(void*);
		diff_capacity = new_capacity - buffer->front_capacity;
		front_reserve += diff_capacity;
		while (to_add > front_reserve)
		{
			diff_capacity = new_capacity;
			new_capacity <<= 1;
			diff_capacity = new_capacity - diff_capacity;
			front_reserve += diff_capacity;
		}
		temp_ptr = buffer->mem_ptr;
		if (temp_ptr == NULL)
		{
			result = g_memory.alloc_buffer_function(&temp_ptr, buffer->capacity + new_capacity);
			if (result != RX_PROTOCOL_OK)
				return result;
		}
		else
		{
			// for backward we have to do the copy stuff
			result = g_memory.alloc_buffer_function(&temp_ptr, buffer->capacity + new_capacity);
			if (result != RX_PROTOCOL_OK)
				return result;
			if (buffer->size)
				memcpy(temp_ptr + front_reserve, buffer->buffer_ptr, buffer->size);
			g_memory.free_buffer_function(buffer->mem_ptr, buffer->capacity + buffer->front_capacity);
		}
		
		front_reserve -= to_add;
		buffer->mem_ptr = temp_ptr;
		buffer->buffer_ptr = buffer->mem_ptr + front_reserve;
		buffer->front_capacity = new_capacity;
	}
	else
	{
		buffer->buffer_ptr -= to_add;
	}

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	return RX_PROTOCOL_OK;
}
RX_COMMON_API void* rx_alloc_from_packet(rx_packet_buffer* buffer, size_t size, rx_protocol_result_t* result)
{
	void* ret = NULL;

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	*result = rx_handle_buffer_back_resize(buffer, size);
	if (*result != RX_PROTOCOL_OK)
		return ret;
	ret = &buffer->buffer_ptr[buffer->size];
	buffer->size += size;


	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	return ret;
}
RX_COMMON_API rx_protocol_result_t rx_push_to_packet(rx_packet_buffer* buffer, const void* buffer_ptr, size_t size)
{
	void* temp;
	rx_protocol_result_t result;


	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	temp = rx_alloc_from_packet(buffer, size, &result);
	if(temp)
		memcpy(temp, buffer_ptr, size);


	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	return result;
}


RX_COMMON_API void* rx_alloc_from_packet_front(rx_packet_buffer* buffer, size_t size, rx_protocol_result_t* result)
{
	void* ret = NULL;

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	*result = rx_handle_buffer_front_resize(buffer, size);
	if (*result != RX_PROTOCOL_OK)
		return ret;
	ret = buffer->buffer_ptr;
	buffer->size += size;

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	return ret;
}
RX_COMMON_API rx_protocol_result_t rx_push_to_packet_front(rx_packet_buffer* buffer, const void* buffer_ptr, size_t size)
{
	void* temp;
	rx_protocol_result_t result;

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	temp = rx_alloc_from_packet_front(buffer, size, &result);
	if (temp)
		memcpy(temp, buffer_ptr, size);

	HANDLE_PACKET_BUFFER_SANITY_CHECK(buffer);

	return result;
}


RX_COMMON_API rx_protocol_result_t rx_reinit_const_packet_buffer(rx_const_packet_buffer* buffer)
{
	buffer->next_read = 0;
	return RX_PROTOCOL_OK;
}

RX_COMMON_API rx_protocol_result_t rx_init_const_packet_buffer(rx_const_packet_buffer* buffer, const void* data, size_t size)
{
	// force it through!!!
	buffer->buffer_ptr = data;
	*((size_t*)&buffer->size) = size;
	buffer->next_read = 0;

	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_init_const_from_packet_buffer(rx_const_packet_buffer* buffer, const rx_packet_buffer* from)
{
	// force it through!!!
	buffer->buffer_ptr = from->buffer_ptr;
	*((size_t*)&buffer->size) = from->size;
	buffer->next_read = 0;

	return RX_PROTOCOL_OK;
}

RX_COMMON_API size_t rx_get_packet_available_data(const rx_const_packet_buffer* buffer)
{
	return buffer->size - buffer->next_read;
}
RX_COMMON_API int rx_buffer_eof(const rx_const_packet_buffer* buffer)
{
	assert(buffer->next_read <= buffer->size);
	return buffer->next_read == buffer->size;
}

RX_COMMON_API const void* rx_get_from_packet(rx_const_packet_buffer* buffer, size_t size, rx_protocol_result_t* result)
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
RX_COMMON_API rx_protocol_result_t rx_pop_from_packet(rx_const_packet_buffer* buffer, void* buffer_ptr, size_t size)
{
	const void* temp;
	rx_protocol_result_t result;

	temp = rx_get_from_packet(buffer, size, &result);
	if(temp)
		memcpy(buffer_ptr, temp, size);

	return result;
}





