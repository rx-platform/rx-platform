

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_packet_buffer.h
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


#ifndef rx_packet_buffer_h
#define rx_packet_buffer_h 1


#include "rx_protocol_errors.h"


#ifdef __cplusplus
extern "C" {
#endif

// this is a very long way forward declaration!!!
struct rx_protocol_stack_endpoint;


typedef struct rx_packet_buffer_type
{
	uint8_t* buffer_ptr;
	size_t size;
	size_t capacity;
	size_t front_capacity;
	uint8_t* mem_ptr;

} rx_packet_buffer;


// initialization
RX_COMMON_API rx_protocol_result_t rx_init_packet_buffer(rx_packet_buffer* buffer, size_t initial_capacity, size_t initial_front_capacity);
RX_COMMON_API rx_protocol_result_t rx_deinit_packet_buffer(rx_packet_buffer* buffer);
RX_COMMON_API rx_protocol_result_t rx_reinit_packet_buffer(rx_packet_buffer* buffer);
// writing to buffer
RX_COMMON_API void* rx_alloc_from_packet(rx_packet_buffer* buffer, size_t size, rx_protocol_result_t* result);
RX_COMMON_API rx_protocol_result_t rx_push_to_packet(rx_packet_buffer* buffer, const void* buffer_ptr, size_t size);
// writing to buffer front
RX_COMMON_API void* rx_alloc_from_packet_front(rx_packet_buffer* buffer, size_t size, rx_protocol_result_t* result);
RX_COMMON_API rx_protocol_result_t rx_push_to_packet_front(rx_packet_buffer* buffer, const void* buffer_ptr, size_t size);
// data for "write-to"
RX_COMMON_API size_t rx_get_packet_usable_data(const rx_packet_buffer* buffer);


typedef struct rx_const_packet_buffer_type
{
	uint8_t const* buffer_ptr;
	const size_t size;
	size_t next_read;

} rx_const_packet_buffer;

RX_COMMON_API rx_protocol_result_t rx_reinit_const_packet_buffer(rx_const_packet_buffer* buffer);
RX_COMMON_API rx_protocol_result_t rx_init_const_packet_buffer(rx_const_packet_buffer* buffer, const void* data, size_t size);
RX_COMMON_API rx_protocol_result_t rx_init_const_from_packet_buffer(rx_const_packet_buffer* buffer, const rx_packet_buffer* from);
// reading from const buffer
RX_COMMON_API const void* rx_get_from_packet(rx_const_packet_buffer* buffer, size_t size, rx_protocol_result_t* result);
RX_COMMON_API rx_protocol_result_t rx_pop_from_packet(rx_const_packet_buffer* buffer, void* buffer_ptr, size_t size);
// reading statuses
RX_COMMON_API size_t rx_get_packet_available_data(const rx_const_packet_buffer* buffer);
RX_COMMON_API int rx_buffer_eof(const rx_const_packet_buffer* buffer);



#ifdef __cplusplus
}
#endif


#endif
