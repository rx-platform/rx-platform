

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_containers.h
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


#include "rx_packet_buffer.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct rx_packet_queue_type
{
	rx_packet_buffer* buffers;
	size_t size;
	size_t capacity;

} rx_packet_queue;

rx_protocol_result_t rx_init_queue(rx_packet_queue* queue, size_t initial_capacity);
rx_protocol_result_t rx_deinit_queue(rx_packet_queue* queue);
int rx_queue_empty(const rx_packet_queue* queue);
rx_protocol_result_t rx_enqueue(rx_packet_queue* queue, const rx_packet_buffer* buffer);
rx_protocol_result_t rx_dequeue(rx_packet_queue* queue, rx_packet_buffer* buffer);


typedef struct rx_packet_stack_type
{
	rx_packet_buffer* buffers;
	size_t size;
	size_t capacity;

} rx_packet_stack;

rx_protocol_result_t rx_init_stack(rx_packet_stack* stack, size_t initial_capacity);
rx_protocol_result_t rx_deinit_stack(rx_packet_stack* stack);
int rx_stack_empty(const rx_packet_stack* stack);
rx_protocol_result_t rx_push(rx_packet_stack* stack, const rx_packet_buffer* buffer);
rx_protocol_result_t rx_pop(rx_packet_stack* stack, rx_packet_buffer* buffer);




#ifdef __cplusplus
}
#endif


#endif
