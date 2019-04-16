

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_protocol_base.h
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


#ifndef rx_protocol_base_h
#define rx_protocol_base_h 1


#include "rx_protocol_errors.h"
#include "rx_containers.h"
#include "rx_session_map.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef rx_protocol_result_t(*rx_send_function_type)(
	struct rx_protocol_stack_entry* reference
	, protocol_endpoint* end_point
	, rx_packet_buffer* buffer);

typedef rx_protocol_result_t(*rx_sent_function_type)(
	struct rx_protocol_stack_entry* reference
	, protocol_endpoint* end_point
	, rx_protocol_result_t result);

typedef rx_protocol_result_t(*rx_received_function_type)(
	struct rx_protocol_stack_entry* reference
	, protocol_endpoint* end_point
	, rx_const_packet_buffer* buffer);


typedef rx_protocol_result_t(*rx_close_function_type)(
	struct rx_protocol_stack_entry* reference
	, rx_protocol_result_t result);

typedef rx_protocol_result_t(*rx_closed_function_type)(
	struct rx_protocol_stack_entry* reference
	, rx_protocol_result_t result);

typedef rx_protocol_result_t(*rx_allocate_packet_type)(
	struct rx_protocol_stack_entry* reference
	, rx_packet_buffer* buffer
	, size_t size);
typedef rx_protocol_result_t(*rx_modify_packet_type)(
	struct rx_protocol_stack_entry* reference
	, rx_packet_buffer* buffer);

typedef rx_protocol_result_t(*rx_add_reference_type)(
	struct rx_protocol_stack_entry* reference);

typedef rx_protocol_result_t(*rx_release_reference_type)(
	struct rx_protocol_stack_entry* reference);

// stack
struct rx_protocol_stack_entry
{
	rx_send_function_type send_function;
	rx_sent_function_type sent_function;

	rx_received_function_type received_function;

	rx_close_function_type close_function;
	rx_closed_function_type closed_function;

	rx_allocate_packet_type allocate_packet_function;
	rx_modify_packet_type free_packet_function;

	rx_add_reference_type add_reference_func;
	rx_release_reference_type release_reference_func;
	
	struct rx_protocol_stack_entry* upward;
	struct rx_protocol_stack_entry* downward;

};


rx_protocol_result_t rx_move_packet_down(struct rx_protocol_stack_entry* stack, protocol_endpoint* end_point, rx_packet_buffer* buffer);
rx_protocol_result_t rx_move_packet_up(struct rx_protocol_stack_entry* stack, protocol_endpoint* end_point, rx_const_packet_buffer* buffer);
rx_protocol_result_t rx_move_result_up(struct rx_protocol_stack_entry* stack, protocol_endpoint* end_point, rx_protocol_result_t result);


typedef rx_protocol_result_t(*rx_alloc_buffer_type)(void** buffer, size_t buffer_size);
typedef rx_protocol_result_t(*rx_free_buffer_type)(void* buffer, size_t buffer_size);

typedef uint32_t (*rx_protocol_atomic_inc_type)(volatile uint32_t* val);
typedef uint32_t (*rx_protocol_atomic_dec_type)(volatile uint32_t* val);


struct rx_hosting_functions
{
	rx_alloc_buffer_type alloc_function;
	rx_free_buffer_type free_function;

	rx_alloc_buffer_type alloc_buffer_function;
	rx_free_buffer_type free_buffer_function;

	rx_protocol_atomic_inc_type atomic_inc_function;
	rx_protocol_atomic_inc_type atomic_dec_function;

};

rx_protocol_result_t rx_init_protocols(struct rx_hosting_functions* memory);
rx_protocol_result_t rx_deinit_protocols();

rx_protocol_result_t rx_push_stack(struct rx_protocol_stack_entry* where_to, struct rx_protocol_stack_entry* what);
rx_protocol_result_t rx_pop_stack(struct rx_protocol_stack_entry* what);



#ifdef __cplusplus
}
#endif


#endif
