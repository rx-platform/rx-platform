

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_binary_sec.h
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


#ifndef rx_opcua_binary_sec_h
#define rx_opcua_binary_sec_h 1


#include "rx_opcua_common.h"



#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)
#pragma pack(1)

typedef struct opcua_security_simetric_header_def
{
	uint32_t channel_id;
	uint32_t token_id;

} opcua_security_simetric_header;

typedef struct opcua_sequence_header_def
{
	uint32_t sequence_number;
	uint32_t request_id;

} opcua_sequence_header;


typedef rx_protocol_result_t(*rx_opcua_transport_connected_t)(
	struct rx_protocol_stack_endpoint* stack, const protocol_address* local_address, const protocol_address* remote_address);




#pragma pack(pop)

// definition of transport struct
typedef struct opcua_sec_none_protocol_def
{
	struct rx_protocol_stack_endpoint stack_entry;
	// settings
	int server_side;
	// state
	enum opcua_transport_state current_state;
	opcua_acknowledge_message connection_data;
	uint32_t received_sequence_id;
	int last_chunk_received;
	uint32_t current_request_id;
	uint32_t current_sequence_id;

	uint32_t channel_id;
	uint32_t token_id;
	// receive collector
	rx_packet_buffer receive_buffer;

	rx_opcua_transport_connected_t transport_connected;

	protocol_address ep_address;

} opcua_sec_none_protocol_type;
// initialize and deinitialize of transport
rx_protocol_result_t opcua_bin_init_sec_none_client_transport(opcua_sec_none_protocol_type* transport);
rx_protocol_result_t opcua_bin_init_sec_none_server_transport(opcua_sec_none_protocol_type* transport);
rx_protocol_result_t opcua_bin_deinit_sec_none_transport(opcua_sec_none_protocol_type* transport);

rx_protocol_result_t opcua_bin_sec_none_client_connected(struct rx_protocol_stack_endpoint* reference, rx_session* session);

rx_protocol_result_t opcua_bin_sec_none_bytes_send(struct rx_protocol_stack_endpoint* reference, send_protocol_packet packet);
rx_protocol_result_t opcua_bin_sec_none_bytes_received(struct rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

rx_protocol_result_t opcua_sec_none_client_disconnected(struct rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason);



#ifdef __cplusplus
}
#endif


#endif
