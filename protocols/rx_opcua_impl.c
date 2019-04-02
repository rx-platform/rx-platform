

/****************************************************************************
*
*  protocols\rx_opcua_impl.c
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


// rx_opcua_impl
#include "protocols/rx_opcua_impl.h"


rx_transport_result_t init_transport_state(opcua_transport_protocol_type* transport, rx_memory_functions* memory)
{
	transport->current_state = opcua_transport_idle;
	memset(&transport->connection_data, 0, sizeof(opcua_acknowledge_message));
	transport->connection_data.protocol_version = RX_OPCUA_MAX_VERSION;

	return RX_PROTOCOL_OK;
}

rx_transport_result_t opcua_bin_init_transport(opcua_transport_protocol_type* transport
	, rx_protocol_client* protocol_client
	, size_t buffer_size
	, size_t queue_size
	, int supports_pipe
	, int server_side
	, rx_memory_functions* memory)
{
	rx_transport_result_t result;

	transport->supports_pipe = supports_pipe;
	transport->server_side = server_side;
	transport->protocol_client = *protocol_client;
	result = rx_init_queue(&transport->send_queue, queue_size, memory);
	if (result != RX_PROTOCOL_OK)
		return result;
	result = rx_init_packet_collector(&transport->collector, buffer_size, memory);
	if (result != RX_PROTOCOL_OK)
	{
		rx_deinit_queue(&transport->send_queue, memory);
		return result;
	}
	result = init_transport_state(transport, memory);
	if (result != RX_PROTOCOL_OK)
		return result;

	return RX_PROTOCOL_OK;
}
rx_transport_result_t opcua_bin_init_client_transport(opcua_transport_protocol_type* transport
	, rx_protocol_client* protocol_client
	, size_t buffer_size
	, size_t queue_size
	, int supports_pipe
	, rx_memory_functions* memory)
{
	return opcua_bin_init_transport(transport, protocol_client, buffer_size, queue_size, supports_pipe, 0, memory);
}
rx_transport_result_t opcua_bin_init_server_transport(opcua_transport_protocol_type* transport
	, rx_protocol_client* protocol_client
	, size_t buffer_size
	, size_t queue_size
	, int supports_pipe
	, rx_memory_functions* memory)
{
	return opcua_bin_init_transport(transport, protocol_client, buffer_size, queue_size, supports_pipe, 1, memory);
}

rx_transport_result_t opcua_bin_deinit_transport(opcua_transport_protocol_type* transport
	, rx_memory_functions* memory)
{
	rx_deinit_queue(&transport->send_queue, memory);
	rx_deinit_packet_collector(&transport->collector, memory);

	return RX_PROTOCOL_OK;
}
rx_transport_result_t opcua_bin_bytes_sent(opcua_transport_protocol_type* transport, rx_transport_result_t result, rx_memory_functions* memory)
{
	rx_transport_buffer buffer;

	if (result == RX_PROTOCOL_OK)
	{
		if (!rx_queue_empty(&transport->send_queue))
		{// send another next packet from queue
			result = rx_pop_from_queue(&transport->send_queue, &buffer.buffer_ptr, &buffer.size);
			if (!result)
				return result;
			result = transport->protocol_client.send_function(transport->protocol_client.reference, buffer.buffer_ptr, buffer.size);
			if (!result)
			{
				memory->free_buffer_function(buffer.buffer_ptr, buffer.size);
				return result;
			}
		}
	}
	else
	{
		init_transport_state(transport, memory);
	}

	return result;
}
rx_transport_result_t opcua_check_header(opcua_transport_protocol_type* transport, const opcua_transport_header* header)
{
	return RX_PROTOCOL_OK;
}
const size_t opcua_hello_min_size = sizeof(opcua_transport_header) + sizeof(opcua_hello_message);
const size_t opcua_hello_max_size = sizeof(opcua_transport_header) + sizeof(opcua_hello_message) + 4096;

rx_transport_result_t opcua_parse_hello_message(opcua_transport_protocol_type* transport, const uint8_t* buffer, size_t buffer_size, rx_memory_functions* memory)
{
	rx_transport_result_t result;
	opcua_transport_header* header;
	opcua_hello_message* message;
	opcua_acknowledge_message* ack_message;
	uint32_t message_type;
	uint32_t response_size;

	// check message sizes
	if (buffer_size < opcua_hello_min_size || buffer_size > opcua_hello_max_size)
		return RX_PROTOCOL_PARSING_ERROR;

	// check message type
	header = (opcua_transport_header*)buffer;
	message_type = (header->message_type[0] << 16) | (header->message_type[1] << 8) | (header->message_type[2]);
	if (message_type != opcua_hello_msg_type)
		return RX_PROTOCOL_PARSING_ERROR;

	// check hello message
	message = (opcua_hello_message*)&buffer[sizeof(opcua_transport_header)];
	if (message->receive_buffer_size < RX_OPCUA_MIN_BUFFER || message->send_buffer_size < RX_OPCUA_MIN_BUFFER)
		return RX_PROTOCOL_BUFFER_NEGOTIATE_ERROR;

	// now fill-out the rest of this
	transport->connection_data.protocol_version = min(transport->connection_data.protocol_version, message->protocol_version);
	transport->connection_data.receive_buffer_size = message->send_buffer_size;
	transport->connection_data.send_buffer_size = message->receive_buffer_size;
	transport->connection_data.max_message_size = message->max_message_size;
	transport->connection_data.max_chunk_count = message->max_chunk_count;

	// allocate the response
	response_size = sizeof(opcua_transport_header) + sizeof(opcua_acknowledge_message);
	uint8_t* response_buffer;
	result = memory->alloc_buffer_function((void**)&response_buffer, response_size);
	if (result != RX_PROTOCOL_OK)
		return result;
	// fill the header
	header = (opcua_transport_header*)response_buffer;
	header->message_type[0] = 'A';
	header->message_type[1] = 'C';
	header->message_type[2] = 'K';
	header->reserved = 'F';
	header->message_size = response_size;
	// fill the message
	ack_message = (opcua_acknowledge_message*)&response_buffer[sizeof(opcua_transport_header)];
	ack_message->protocol_version = transport->connection_data.protocol_version;
	ack_message->receive_buffer_size = transport->connection_data.receive_buffer_size;
	ack_message->send_buffer_size = transport->connection_data.send_buffer_size;
	ack_message->max_message_size = 0;
	ack_message->max_chunk_count = 0;

	result = transport->protocol_client.send_function(transport->protocol_client.reference, response_buffer, response_size);
	if (result == RX_PROTOCOL_OK)
	{
		transport->current_state = opcua_transport_opening;
	}

	return result;
}
rx_transport_result_t opcua_parse_rhello_message(opcua_transport_protocol_type* transport, const uint8_t* buffer, size_t buffer_size, rx_memory_functions* memory)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}
rx_transport_result_t opcua_parse_ack_message(opcua_transport_protocol_type* transport, const uint8_t* buffer, size_t buffer_size, rx_memory_functions* memory)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}
rx_transport_result_t opcua_parse_regular_message(opcua_transport_protocol_type* transport, const uint8_t* buffer, size_t buffer_size, rx_memory_functions* memory)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}
rx_transport_result_t opcua_parse_open_message(opcua_transport_protocol_type* transport, const uint8_t* buffer, size_t buffer_size, rx_memory_functions* memory)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_transport_result_t opcua_bin_bytes_received(opcua_transport_protocol_type* transport, rx_transport_result_t result, const uint8_t* buffer, size_t buffer_size, rx_memory_functions* memory)
{
	if (result == RX_PROTOCOL_OK)
	{
		if (transport->server_side)
		{
			switch (transport->current_state)
			{
			case opcua_transport_idle:
				result = opcua_parse_hello_message(transport, buffer, buffer_size, memory);
				break;
			case opcua_transport_hello_ack:
				assert(0);
				break;
			case opcua_transport_opening:
				result = opcua_parse_open_message(transport, buffer, buffer_size, memory);
				break;
			case opcua_transport_active:
				result = opcua_parse_regular_message(transport, buffer, buffer_size, memory);
				break;
			case opcua_transport_closing:
				break;
			default:
				assert(0);
			}
		}
		else
		{
			switch (transport->current_state)
			{
			case opcua_transport_idle:
				result = opcua_parse_rhello_message(transport, buffer, buffer_size, memory);
				break;
			case opcua_transport_hello_ack:
				result = opcua_parse_ack_message(transport, buffer, buffer_size, memory);
				break;
			case opcua_transport_opening:
				result = opcua_parse_open_message(transport, buffer, buffer_size, memory);
				break;
			case opcua_transport_active:
				result = opcua_parse_regular_message(transport, buffer, buffer_size, memory);
				break;
			case opcua_transport_closing:
				break;
			default:
				assert(0);
			}
		}
	}
	else
	{
		init_transport_state(transport, memory);
	}
	return result;
}
rx_transport_result_t opcua_bin_cloced(opcua_transport_protocol_type* transport, rx_transport_result_t result, rx_memory_functions* memory)
{
	return result;
}


rx_transport_result_t opcua_bin_send_packets(opcua_transport_protocol_type* transport, const uint8_t* buffer, size_t buffer_size, rx_memory_functions* memory)
{
	if (transport->current_state != opcua_transport_active)
		return RX_PROTOCOL_WRONG_STATE;

	return RX_PROTOCOL_OK;
}



/*
opcua_result_t opcua_create_hello_message(uint8_t* buffer, size_t* buffer_size);
opcua_result_t opcua_parse_hello_message(const uint8_t* buffer, size_t buffer_size, struct opcua_hello_message* msg);
opcua_result_t opcua_create_acknowledge_message(uint8_t* buffer, size_t* buffer_size, const struct opcua_hello_message* msg);
opcua_result_t opcua_parse_acknowledge_message(const uint8_t* buffer, size_t buffer_size, struct opcua_hello_message* msg);

*/



