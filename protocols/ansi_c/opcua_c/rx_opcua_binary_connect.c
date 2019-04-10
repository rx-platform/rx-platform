

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_binary_connect.c
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


#include "pch.h"

#include "rx_opcua_transport.h"

// rx_opcua_binary_connect
#include "protocols/ansi_c/opcua_c/rx_opcua_binary_connect.h"


const size_t opcua_hello_min_size = sizeof(opcua_transport_header) + sizeof(opcua_hello_message);
const size_t opcua_hello_max_size = sizeof(opcua_transport_header) + sizeof(opcua_hello_message) + 4096;


rx_protocol_result_t opcua_parse_hello_message(opcua_transport_protocol_type* transport, const opcua_transport_header* header, protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	rx_protocol_result_t result;
	opcua_hello_message* message;
	opcua_acknowledge_message* ack_message;
	opcua_transport_header* response_header;
	rx_packet_buffer response;

	// check message sizes
	if (header->message_size < opcua_hello_min_size || header->message_size > opcua_hello_max_size)
		return RX_PROTOCOL_PARSING_ERROR;
	
	// check hello message
	message = (opcua_hello_message*)rx_get_from_packet(buffer, sizeof(opcua_transport_header), &result);
	if (!message)
		return result;

	if (message->receive_buffer_size < RX_OPCUA_MIN_BUFFER || message->send_buffer_size < RX_OPCUA_MIN_BUFFER)
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;

	// now fill-out the rest of this
	transport->connection_data.protocol_version = min(transport->connection_data.protocol_version, message->protocol_version);
	transport->connection_data.receive_buffer_size = message->send_buffer_size;
	transport->connection_data.send_buffer_size = message->receive_buffer_size;
	transport->connection_data.max_message_size = message->max_message_size;
	transport->connection_data.max_chunk_count = message->max_chunk_count;

	// allocate the response
	result = rx_init_packet_buffer(&response, 0x1000, &transport->protocol_stack_entry);
	if (result != RX_PROTOCOL_OK)
		return result;
	// fill the response header
	response_header = (opcua_transport_header*)rx_alloc_from_packet(&response, sizeof(opcua_transport_header), &result);
	if (!header)
		return result;

	response_header->message_type[0] = 'A';
	response_header->message_type[1] = 'C';
	response_header->message_type[2] = 'K';
	response_header->is_final = 'F';
	// fill the message
	ack_message = (opcua_acknowledge_message*)rx_alloc_from_packet(&response, sizeof(opcua_transport_header), &result);
	if (!ack_message)
		return result;
	ack_message->protocol_version = transport->connection_data.protocol_version;
	ack_message->receive_buffer_size = transport->connection_data.receive_buffer_size;
	ack_message->send_buffer_size = transport->connection_data.send_buffer_size;
	ack_message->max_message_size = 0;
	ack_message->max_chunk_count = 0;


	response_header->message_size = (uint32_t)rx_get_packet_usable_data(&response);

	result = rx_move_packet_down(&transport->protocol_stack_entry, end_point, &response);
	if (result == RX_PROTOCOL_OK)
	{
		if (transport->supports_pipe)
			transport->current_state = opcua_transport_active;
		else
			transport->current_state = opcua_transport_opening;
	}
	return result;
}
rx_protocol_result_t opcua_parse_reverse_hello_message(opcua_transport_protocol_type* transport, const opcua_transport_header* header, protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}
rx_protocol_result_t opcua_parse_ack_message(opcua_transport_protocol_type* transport, const opcua_transport_header* header, protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}


