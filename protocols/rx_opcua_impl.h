

/****************************************************************************
*
*  protocols\rx_opcua_impl.h
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


#ifndef rx_opcua_impl_h
#define rx_opcua_impl_h 1


#define RX_OPCUA_MAX_VERSION 0
#define RX_OPCUA_MIN_BUFFER 8192
#include "rx_transport_base.h"


#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(push)
#pragma pack(1)

typedef struct opcua_transport_header_def
{
	uint8_t message_type[3];
	uint8_t reserved;
	uint32_t message_size;

} opcua_transport_header;

const uint32_t opcua_hello_msg_type = ('H' << 16) | ('E' << 8) | 'L';
const uint32_t opcua_reverse_hello_msg_type = ('R' << 16) | ('H' << 8) | 'E';
const uint32_t opcua_ack_msg_type = ('A' << 16) | ('C' << 8) | 'K';
const uint32_t opcua_open_msg_type = ('O' << 16) | ('P' << 8) | 'N';
const uint32_t opcua_close_msg_type = ('C' << 16) | ('C' << 8) | 'S';
const uint32_t opcua_error_msg_type = ('E' << 16) | ('R' << 8) | 'R';
const uint32_t opcua_regular_msg_type = ('M' << 16) | ('S' << 8) | 'G';

// !!!!!! IMPLEMENTATION SPECIFIC, NO SECURITY, LOCAL PIPE
const uint32_t opcua_pipe_msg_type = ('L' << 16) | ('O' << 8) | 'C';


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


typedef struct security_simetric_header_def
{
	uint32_t channel_id;
	uint32_t token_id;

} security_simetric_header;

typedef struct sequence_header_def
{
	uint32_t sequence_number;
	uint32_t request_id;

} sequence_header;

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
	// settings
	int supports_pipe;
	int server_side;
	// state
	enum opcua_transport_state current_state;
	opcua_acknowledge_message connection_data;
	// protocol callbacks
	rx_protocol_client protocol_client;
	// send queue
	rx_transport_queue send_queue;
	// receive collector
	rx_transport_packet_collector collector;

} opcua_transport_protocol_type;
// initialize and deinitialize of transport
rx_transport_result_t opcua_bin_init_client_transport(opcua_transport_protocol_type* transport
	, rx_protocol_client* protocol_client
	, size_t buffer_size
	, size_t queue_size
	, int supports_pipe
	, rx_memory_functions* memory);
rx_transport_result_t opcua_bin_init_server_transport(opcua_transport_protocol_type* transport
	, rx_protocol_client* protocol_client
	, size_t buffer_size
	, size_t queue_size
	, int supports_pipe
	, rx_memory_functions* memory);
rx_transport_result_t opcua_bin_deinit_transport(opcua_transport_protocol_type* transport
	, rx_memory_functions* memory);

// i/o for data
rx_transport_result_t opcua_bin_bytes_sent(opcua_transport_protocol_type* transport, rx_transport_result_t result, rx_memory_functions* memory);
rx_transport_result_t opcua_bin_bytes_received(opcua_transport_protocol_type* transport, rx_transport_result_t result, const uint8_t* buffer, size_t buffer_size, rx_memory_functions* memory);
rx_transport_result_t opcua_bin_cloced(opcua_transport_protocol_type* transport, rx_transport_result_t result, rx_memory_functions* memory);
// split and send
rx_transport_result_t opcua_bin_send_packets(opcua_transport_protocol_type* transport, const uint8_t* buffer, size_t buffer_size, rx_memory_functions* memory);



#ifdef __cplusplus
}
#endif


#endif
