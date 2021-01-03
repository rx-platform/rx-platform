

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_protocol_handlers.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_protocol_handlers_h
#define rx_protocol_handlers_h 1


#include "rx_protocol_errors.h"
#include "rx_containers.h"
#include "rx_protocol_address.h"


#ifdef __cplusplus
extern "C" {
#endif



typedef uint32_t rx_packet_id_type;
typedef uint32_t rx_address_reference_type;


typedef struct rx_recv_protocol_packet_def
{
	const protocol_address* from_addr;
	const protocol_address* to_addr;
	rx_address_reference_type from;
	rx_address_reference_type to;
	rx_const_packet_buffer* buffer;
	rx_packet_id_type id;

} recv_protocol_packet;

typedef rx_protocol_result_t(*rx_received_function_type)(
	struct rx_protocol_stack_endpoint* reference
	, recv_protocol_packet packet);

typedef struct rx_send_protocol_packet_def
{
	const protocol_address* from_addr;
	const protocol_address* to_addr;
	rx_address_reference_type from;
	rx_address_reference_type to;
	rx_packet_buffer* buffer;
	rx_packet_id_type id;

} send_protocol_packet;

// session 
typedef struct rx_session_def
{
	const protocol_address* local_addr;
	const protocol_address* remote_addr;
	rx_address_reference_type local_ref;
	rx_address_reference_type remote_ref;
	const struct rx_session_def* next;

} rx_session;


#define UINT8_ADDR_SIZE sizeof(struct protocol_address_def)
#define UINT8_ADDRS_SIZE (2*UINT8_ADDR_SIZE)
recv_protocol_packet rx_create_recv_packet(rx_packet_id_type id, rx_const_packet_buffer* buffer, rx_address_reference_type from, rx_address_reference_type to);
send_protocol_packet rx_create_send_packet(rx_packet_id_type id, rx_packet_buffer* buffer, rx_address_reference_type from, rx_address_reference_type to);
rx_session rx_create_session(const protocol_address* local, const protocol_address* remote, rx_address_reference_type local_ref, rx_address_reference_type remote_ref, const struct rx_session_def* next);

typedef void(*rx_stack_changed_function_type)(
	struct rx_protocol_stack_endpoint* reference);

typedef rx_protocol_result_t(*rx_send_function_type)(
	struct rx_protocol_stack_endpoint* reference
	, send_protocol_packet packet);

typedef rx_protocol_result_t(*rx_ack_function_type)(
	struct rx_protocol_stack_endpoint* reference
	, rx_packet_id_type id
	, rx_protocol_result_t result);

typedef rx_protocol_result_t(*rx_close_function_type)(
	struct rx_protocol_stack_endpoint* reference
	, rx_protocol_result_t result);

typedef void(*rx_closed_function_type)(
	struct rx_protocol_stack_endpoint* reference
	, rx_protocol_result_t reason);


typedef rx_protocol_result_t(*rx_connect_function_type)(
	struct rx_protocol_stack_endpoint* reference
	, struct rx_session_def* session);

typedef rx_protocol_result_t(*rx_connected_function_type)(
	struct rx_protocol_stack_endpoint* reference
	, struct rx_session_def* session);

typedef rx_protocol_result_t(*rx_disconnect_function_type)(
	struct rx_protocol_stack_endpoint* reference
	, struct rx_session_def* session
	, rx_protocol_result_t reason);

typedef rx_protocol_result_t(*rx_disconnected_function_type)(
	struct rx_protocol_stack_endpoint* reference
	, struct rx_session_def* session
	, rx_protocol_result_t reason);

typedef rx_protocol_result_t(*rx_allocate_packet_type)(
	struct rx_protocol_stack_endpoint* reference
	, rx_packet_buffer* buffer
	, size_t size);
typedef rx_protocol_result_t(*rx_modify_packet_type)(
	struct rx_protocol_stack_endpoint* reference
	, rx_packet_buffer* buffer);

typedef rx_protocol_result_t(*rx_add_reference_type)(
	struct rx_protocol_stack_endpoint* reference);

typedef rx_protocol_result_t(*rx_release_reference_type)(
	struct rx_protocol_stack_endpoint* reference);


typedef intptr_t security_data_t;

// stack
struct rx_protocol_stack_endpoint
{
	rx_send_function_type send_function;
	rx_ack_function_type ack_function;

	rx_received_function_type received_function;

	rx_close_function_type close_function;
	rx_closed_function_type closed_function;

	rx_connect_function_type connect_function;
	rx_connected_function_type connected_function;
	rx_disconnect_function_type disconnect_function;
	rx_disconnected_function_type disconnected_function;

	rx_allocate_packet_type allocate_packet_function;
	rx_modify_packet_type free_packet_function;

	rx_add_reference_type add_reference_func;
	rx_release_reference_type release_reference_func;
	
	struct rx_protocol_stack_endpoint* upward;
	struct rx_protocol_stack_endpoint* downward;
	rx_stack_changed_function_type stack_changed_function;

	void* user_data;
	security_data_t identity;

};

void rx_init_stack_entry(struct rx_protocol_stack_endpoint* stack, void* user_data);


rx_protocol_result_t rx_connect(struct rx_protocol_stack_endpoint* stack, struct rx_session_def* session);
rx_protocol_result_t rx_disconnect(struct rx_protocol_stack_endpoint* stack, struct rx_session_def* session, rx_protocol_result_t reason);
rx_protocol_result_t rx_close(struct rx_protocol_stack_endpoint* stack, rx_protocol_result_t reason);

rx_protocol_result_t rx_move_packet_up(struct rx_protocol_stack_endpoint* stack, recv_protocol_packet packet);
rx_protocol_result_t rx_move_packet_down(struct rx_protocol_stack_endpoint* stack, send_protocol_packet packet);
rx_protocol_result_t rx_notify_ack(struct rx_protocol_stack_endpoint* stack, rx_packet_id_type id, rx_protocol_result_t result);

rx_protocol_result_t rx_notify_connected(struct rx_protocol_stack_endpoint* stack, struct rx_session_def* session);
rx_protocol_result_t rx_notify_disconnected(struct rx_protocol_stack_endpoint* stack, struct rx_session_def* session, rx_protocol_result_t reason);
void rx_notify_closed(struct rx_protocol_stack_endpoint* stack, rx_protocol_result_t reason);

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


rx_protocol_result_t rx_push_stack(struct rx_protocol_stack_endpoint* where_to, struct rx_protocol_stack_endpoint* what);
rx_protocol_result_t rx_pop_stack(struct rx_protocol_stack_endpoint* what);



#ifdef __cplusplus
}
#endif


#endif
