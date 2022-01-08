

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_transport.h
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


#ifndef rx_opcua_transport_h
#define rx_opcua_transport_h 1


#define RX_OPCUA_MAX_VERSION 0
#define RX_OPCUA_MIN_BUFFER 8192
#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"
#include "protocols/ansi_c/common_c/rx_packet_decoder.h"


#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(push)
#pragma pack(1)

typedef struct opcua_transport_header_def
{
	uint8_t message_type[3];
	uint8_t is_final;
	uint32_t message_size;

} opcua_transport_header;

enum opcua_message_types
{
	// transport level messages
	opcua_hello_msg_type = ('H' << 16) | ('E' << 8) | 'L',
	opcua_reverse_hello_msg_type = ('R' << 16) | ('H' << 8) | 'E',
	opcua_ack_msg_type = ('A' << 16) | ('C' << 8) | 'K',
	opcua_error_msg_type = ('E' << 16) | ('R' << 8) | 'R',
	// secure channel messages
	opcua_open_msg_type = ('O' << 16) | ('P' << 8) | 'N',
	opcua_close_msg_type = ('C' << 16) | ('C' << 8) | 'S',
	// regular messages
	opcua_regular_msg_type = ('M' << 16) | ('S' << 8) | 'G'
};


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

typedef struct opcua_acknowledge_message_def
{
	uint32_t protocol_version;
	uint32_t receive_buffer_size;
	uint32_t send_buffer_size;
	uint32_t max_message_size;
	uint32_t max_chunk_count;

} opcua_acknowledge_message;


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

typedef struct opcua_transport_error_def
{
	uint32_t error_code;
	int32_t reason_size;
	//char reason[reason_size];

} opcua_transport_error;


#pragma pack(pop)



enum opcua_transport_state
{
	opcua_transport_idle = 0,
	opcua_transport_hello_ack = 1,
	opcua_transport_opening = 2,
	opcua_transport_active = 3,
	opcua_transport_closing = 4,

};


// definition of transport struct
typedef struct opcua_transport_protocol_def
{
	struct rx_protocol_stack_endpoint stack_entry;
	// settings
	int supports_pipe;
	int server_side;
	// state
	enum opcua_transport_state current_state;
	opcua_acknowledge_message connection_data;
	uint32_t received_request_id;
	uint32_t received_sequence_id;
	int last_chunk_received;
	uint32_t current_request_id;
	uint32_t current_sequence_id;
	// receive collector
	rx_packet_buffer receive_buffer;

	struct packed_decoder_type packet_decoder;
	uint8_t header_buffer[sizeof(opcua_transport_header)];
	rx_packet_buffer transport_receive_buffer;

} opcua_transport_protocol_type;
// initialize and deinitialize of transport
rx_protocol_result_t opcua_bin_init_client_transport(opcua_transport_protocol_type* transport
	, rx_packet_buffer receive_buffer);
rx_protocol_result_t opcua_bin_init_server_transport(opcua_transport_protocol_type* transport
	, rx_packet_buffer receive_buffer);
rx_protocol_result_t opcua_bin_init_pipe_transport(opcua_transport_protocol_type* transport
	, rx_packet_buffer receive_buffer);
rx_protocol_result_t opcua_bin_deinit_transport(opcua_transport_protocol_type* transport);

rx_protocol_result_t opcua_bin_bytes_send(struct rx_protocol_stack_endpoint* reference, send_protocol_packet packet);
rx_protocol_result_t opcua_bin_bytes_received(struct rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);



#ifdef __cplusplus
}
#endif


#endif
