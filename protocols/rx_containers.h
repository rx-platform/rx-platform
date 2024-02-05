

/****************************************************************************
*
*  protocols\rx_containers.h
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


#ifndef rx_containers_h
#define rx_containers_h 1


#include "rx_protocol_errors.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct rx_transport_buffer_type
{
	uint8_t* buffer_ptr;
	size_t size;

} rx_transport_buffer;

typedef struct rx_transport_queue_type
{
	rx_transport_buffer* buffers;
	size_t size;
	size_t capacity;
} rx_transport_queue;


typedef struct rx_transport_packet_collector_type
{
	rx_transport_buffer buffer;
	size_t capacity;

} rx_transport_packet_collector;

typedef rx_transport_result_t(*rx_alloc_buffer_type)(void** buffer, size_t buffer_size);
typedef rx_transport_result_t(*rx_free_buffer_type)(void* buffer, size_t buffer_size);


typedef struct rx_memory_functions_type
{
	rx_alloc_buffer_type alloc_function;
	rx_free_buffer_type free_function;

	rx_alloc_buffer_type alloc_buffer_function;
	rx_free_buffer_type free_buffer_function;

} rx_memory_functions;



rx_transport_result_t rx_init_queue(rx_transport_queue* queue, size_t initial_capacity, rx_memory_functions* memory);
rx_transport_result_t rx_deinit_queue(rx_transport_queue* queue, rx_memory_functions* memory);
int rx_queue_empty(const rx_transport_queue* queue);
rx_transport_result_t rx_push_to_queue(rx_transport_queue* queue, uint8_t* buffer_ptr, size_t size, rx_memory_functions* memory);
rx_transport_result_t rx_pop_from_queue(rx_transport_queue* queue, uint8_t** buffer_ptr, size_t* size);

rx_transport_result_t rx_init_packet_collector(rx_transport_packet_collector* collector, size_t initial_capacity, rx_memory_functions* memory);
rx_transport_result_t rx_deinit_packet_collector(rx_transport_packet_collector* collector, rx_memory_functions* memory);
int rx_collector_empty(const rx_transport_packet_collector* collector);
rx_transport_result_t rx_push_to_packet_collector_end(rx_transport_packet_collector* collector, uint8_t* buffer_ptr, size_t size, rx_memory_functions* memory);
rx_transport_result_t rx_get_collected_packet(rx_transport_packet_collector* collector, uint8_t** buffer_ptr, size_t* size);
rx_transport_result_t rx_reinit_packet_collector(rx_transport_packet_collector* collector);


rx_transport_result_t rx_c_alloc_buffer(void** buffer, size_t buffer_size);
rx_transport_result_t rx_c_free_buffer(void* buffer, size_t buffer_size);


#ifdef __cplusplus
}
#endif


#endif
