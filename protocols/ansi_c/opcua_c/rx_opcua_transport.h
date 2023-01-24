

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_transport.h
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


#ifndef rx_opcua_transport_h
#define rx_opcua_transport_h 1


#include "rx_opcua_common.h"



#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)
#pragma pack(1)


typedef struct opcua_hello_message_def
{
	uint32_t protocol_version;
	uint32_t receive_buffer_size;
	uint32_t send_buffer_size;
	uint32_t max_message_size;
	uint32_t max_chunk_count;
	int32_t endpoint_url_size;
	//char endpoint_url[endpoint_url_size];

} opcua_hello_message;



typedef struct opcua_transport_error_def
{
	uint32_t error_code;
	int32_t reason_size;
	//char reason[reason_size];

} opcua_transport_error;


#pragma pack(pop)




// definition of transport struct
typedef struct opcua_transport_protocol_def
{
	struct rx_protocol_stack_endpoint stack_entry;
	// settings
	int server_side;
	// receive collector
	rx_packet_buffer transport_receive_buffer;

	struct packed_decoder_type packet_decoder;
	uint8_t header_buffer[sizeof(opcua_transport_header)];
	// state
	enum opcua_transport_state current_state;
	opcua_acknowledge_message connection_data;

	const char* endpoint_url;

} opcua_transport_protocol_type;
// initialize and deinitialize of transport
rx_protocol_result_t opcua_bin_init_client_transport(opcua_transport_protocol_type* transport);
rx_protocol_result_t opcua_bin_init_server_transport(opcua_transport_protocol_type* transport);
rx_protocol_result_t opcua_bin_deinit_transport(opcua_transport_protocol_type* transport);


rx_protocol_result_t opcua_bin_client_connected(struct rx_protocol_stack_endpoint* reference, rx_session* session);
rx_protocol_result_t opcua_bin_client_connect(struct rx_protocol_stack_endpoint* reference, rx_session* session);

rx_protocol_result_t opcua_bin_bytes_received(struct rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);



#ifdef __cplusplus
}
#endif


#endif
