

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_transport.c
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif


// rx_opcua_transport
#include "protocols/ansi_c/opcua_c/rx_opcua_transport.h"

#include "rx_opcua_binary_connect.h"
#include "rx_opcua_binary_sec.h"

const uint32_t invalid_sequence_id = (uint32_t)(-1);

rx_protocol_result_t opcua_bin_bytes_send(struct rx_protocol_stack_entry* reference,const protocol_endpoint* end_point,rx_packet_buffer* buffer);
rx_protocol_result_t opcua_bin_bytes_sent(struct rx_protocol_stack_entry* reference,const protocol_endpoint* end_point, rx_protocol_result_t result);



rx_protocol_result_t opcua_init_transport_state(opcua_transport_protocol_type* transport)
{
	transport->current_state = opcua_transport_idle;
	memset(&transport->connection_data, 0, sizeof(opcua_acknowledge_message));
	transport->connection_data.protocol_version = RX_OPCUA_MAX_VERSION;
	transport->received_sequence_id = invalid_sequence_id;
	transport->last_chunk_received = 0;
	transport->current_request_id = 1;
	transport->current_sequence_id = 1;

	return RX_PROTOCOL_OK;
}

rx_protocol_result_t opcua_check_header(opcua_transport_protocol_type* transport, const opcua_transport_header* header)
{
	return RX_PROTOCOL_OK;
}


const size_t opcua_regular_headers_size = sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header) + sizeof(opcua_sequence_header);

rx_protocol_result_t opcua_parse_regular_message(opcua_transport_protocol_type* transport, const opcua_transport_header* header,const protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	rx_protocol_result_t result;
	opcua_security_simetric_header* sec_header;
	opcua_sequence_header* sequence_header;

	// check message sizes
	if (header->message_size <= opcua_regular_headers_size/*at leas one byte in payload, so <= !!!*/ || header->message_size > transport->connection_data.receive_buffer_size)
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;

	// retrieve headers
	sec_header = (opcua_security_simetric_header*)rx_get_from_packet(buffer, sizeof(opcua_transport_header), &result);
	if (!sec_header)
		return result;
	sequence_header = (opcua_sequence_header*)rx_get_from_packet(buffer, sizeof(opcua_security_simetric_header), &result);
	if (!sequence_header)
		return result;


	result = RX_PROTOCOL_NOT_IMPLEMENTED;

	return result;
}

rx_protocol_result_t opcua_parse_error_message(opcua_transport_protocol_type* transport, const opcua_transport_header* header,const protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t opcua_parse_pipe_message(opcua_transport_protocol_type* transport, const opcua_transport_header* header,const protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	rx_protocol_result_t result;
	opcua_security_simetric_header* sec_header;
	opcua_sequence_header* sequence_header;
	size_t usable_size;
	const void* buffer_ptr;
	rx_const_packet_buffer upward_buffer;

	// check message sizes
	if (header->message_size <= opcua_regular_headers_size/*at leas one byte in payload, so <= !!!*/ || header->message_size > transport->connection_data.receive_buffer_size)
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;

	// retrieve headers
	sec_header = (opcua_security_simetric_header*)rx_get_from_packet(buffer, sizeof(opcua_transport_header), &result);
	if (!sec_header)
		return result;
	sequence_header = (opcua_sequence_header*)rx_get_from_packet(buffer, sizeof(opcua_security_simetric_header), &result);
	if (!sequence_header)
		return result;

	// check security header
	if (sec_header->channel_id != 0 || sec_header->token_id != 0)
		return RX_PROTOCOL_PARSING_ERROR;

	// sequence header
	if (transport->received_sequence_id != invalid_sequence_id)
	{
		// TODO - handle rollover
		transport->received_sequence_id++;
		if (transport->received_sequence_id != sequence_header->sequence_number)
			return RX_PROTOCOL_INVALID_SEQUENCE;
		if (transport->received_request_id != sequence_header->request_id)
		{// new request
			if (!transport->last_chunk_received)
				return RX_PROTOCOL_COLLECT_ERROR;
			result = rx_reinit_packet_buffer(&transport->receive_buffer);
			if (result != RX_PROTOCOL_OK)
				return result;
		}
	}
	else
	{
		// this is a first regular transport packet
		transport->received_sequence_id = sequence_header->sequence_number;
		transport->received_request_id = sequence_header->request_id;
		result = rx_reinit_packet_buffer(&transport->receive_buffer);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	if (header->is_final == 'A')
	{// abort the message
		transport->last_chunk_received = 1;
		result = rx_reinit_packet_buffer(&transport->receive_buffer);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	else if (header->is_final == 'F' && transport->last_chunk_received)
	{
		// one packet for all, send it directly
		result = rx_move_packet_up(&transport->protocol_stack_entry, end_point, buffer);
	}
	else
	{
		usable_size = header->message_size - opcua_regular_headers_size;
		buffer_ptr = rx_get_from_packet(buffer, usable_size, &result);
		if (!buffer_ptr)
			return result;
		result = rx_push_to_packet(&transport->receive_buffer, buffer_ptr, usable_size);
		if (result != RX_PROTOCOL_OK)
			return result;

		if (header->is_final == 'F')
		{// final chunk
			transport->last_chunk_received = 1;
			result = rx_init_const_from_packet_buffer(&upward_buffer, &transport->receive_buffer);
			if (result != RX_PROTOCOL_OK)
				return result;//TODO //not sure if this is enough
			
			result = rx_move_packet_up(&transport->protocol_stack_entry, end_point, &upward_buffer);
			if (result == RX_PROTOCOL_OK)
				rx_reinit_packet_buffer(&transport->receive_buffer);
		}
		else if (header->is_final == 'C')
		{// intermediate
			transport->last_chunk_received = 0;
		}
		else
		{
			return RX_PROTOCOL_PARSING_ERROR;
		}
	}

	return result;
}


rx_protocol_result_t opcua_bin_bytes_received(struct rx_protocol_stack_entry* reference,const protocol_endpoint* end_point, rx_const_packet_buffer* buffer)
{
	size_t available_data;
	rx_protocol_result_t result;
	opcua_transport_header* header;
	uint32_t message_type;
	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference;

	while (1)
	{

		available_data = rx_get_packet_available_data(buffer);
		// check message sizes
		if (available_data <= sizeof(opcua_transport_header))
		{
			result = RX_PROTOCOL_BUFFER_SIZE_ERROR;
			break;
		}

		// check message type
		header = (opcua_transport_header*)rx_get_from_packet(buffer, sizeof(opcua_transport_header), &result);
		if (result != RX_PROTOCOL_OK)
			return result;

		message_type = (header->message_type[0] << 16) | (header->message_type[1] << 8) | (header->message_type[2]);

		// check correct message size in header
		if (header->message_size > available_data)
		{
			result = RX_PROTOCOL_PARSING_ERROR;
			break;
		}

		if (transport->server_side)
		{
			switch (message_type)
			{
			case opcua_regular_msg_type:
				result = opcua_parse_regular_message(transport, header, end_point, buffer);
				break;
			case rx_pipe_msg_type:
				result = opcua_parse_pipe_message(transport, header, end_point, buffer);
				break;
			case opcua_error_msg_type:
				result = opcua_parse_error_message(transport, header, end_point, buffer);
				break;
			case opcua_hello_msg_type:
				result = opcua_parse_hello_message(transport, header, end_point, buffer);
				break;
			case opcua_open_msg_type:
				result = opcua_parse_open_message(transport, header, end_point, buffer);
				break;
			case opcua_close_msg_type:
				result = opcua_parse_close_message(transport, header, end_point, buffer);
				break;
			default:
				result = RX_PROTOCOL_PARSING_ERROR;
			}
		}
		else
		{
			switch (message_type)
			{
			case opcua_regular_msg_type:
				result = opcua_parse_regular_message(transport, header, end_point, buffer);
				break;
			case rx_pipe_msg_type:
				result = opcua_parse_pipe_message(transport, header, end_point, buffer);
				break;
			case opcua_reverse_hello_msg_type:
				result = opcua_parse_reverse_hello_message(transport, header, end_point, buffer);
				break;
			case opcua_ack_msg_type:
				result = opcua_parse_ack_message(transport, header, end_point, buffer);
				break;
			case opcua_error_msg_type:
				result = opcua_parse_error_message(transport, header, end_point, buffer);
				break;
			default:
				result = RX_PROTOCOL_PARSING_ERROR;
			}
		}
		// check correct message size in header
		if (header->message_size == available_data)
		{
			break;
		}
	}
	if (result != RX_PROTOCOL_OK)
	{
		opcua_init_transport_state(transport);
	}
	return result;
}

rx_protocol_result_t opcua_bin_closed(struct rx_protocol_stack_entry* transport, rx_protocol_result_t result)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}
rx_protocol_result_t opcua_bin_close(struct rx_protocol_stack_entry* transport, rx_protocol_result_t result)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t opcua_bin_allocate_buffer(struct rx_protocol_stack_entry* reference, rx_packet_buffer* buffer, size_t initial_size)
{
	rx_protocol_result_t result;
	void* temp_ptr;

	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference;
	if (rx_stack_empty(&transport->free_buffers))
	{
		result = rx_init_packet_buffer(buffer, max(initial_size , transport->initial_packet_size), reference->downward);
		if (result == RX_PROTOCOL_OK)
		{
			temp_ptr = rx_alloc_from_packet(buffer, opcua_regular_headers_size, &result);
			if (!temp_ptr)
				result = rx_deinit_packet_buffer(buffer);
		}
		return result;
	}
	else
	{
		result = rx_pop(&transport->free_buffers, buffer);
		if(result == RX_PROTOCOL_OK)
		{
			temp_ptr = rx_alloc_from_packet(buffer, opcua_regular_headers_size, &result);
			if (!temp_ptr)
				result = rx_deinit_packet_buffer(buffer);
		}
		return result;
	}
}
rx_protocol_result_t opcua_bin_free_buffer(struct rx_protocol_stack_entry* reference, rx_packet_buffer* buffer)
{
	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference;
	rx_reinit_packet_buffer(buffer);
	return rx_push(&transport->free_buffers, buffer);
}


rx_protocol_result_t opcua_bin_init_transport(opcua_transport_protocol_type* transport
	, size_t buffer_size
	, size_t queue_size
	, int supports_pipe
	, int server_side
	)
{
	rx_protocol_result_t result;

	memset(transport, 0, sizeof(opcua_transport_protocol_type));
	// fill protocol stack header
	transport->protocol_stack_entry.received_function = opcua_bin_bytes_received;
	transport->protocol_stack_entry.send_function = opcua_bin_bytes_send;
	transport->protocol_stack_entry.sent_function = opcua_bin_bytes_sent;
	transport->protocol_stack_entry.closed_function = opcua_bin_closed;
	transport->protocol_stack_entry.close_function = opcua_bin_close;
	transport->protocol_stack_entry.allocate_packet_function = opcua_bin_allocate_buffer;
	transport->protocol_stack_entry.free_packet_function = opcua_bin_free_buffer;
	// fill options
	transport->supports_pipe = supports_pipe;
	transport->server_side = server_side;
	transport->initial_packet_size = buffer_size;
	// fill state
	result = opcua_init_transport_state(transport);
	if (result != RX_PROTOCOL_OK)
		return result;
	// initialize containers
	result = rx_init_queue(&transport->send_queue, queue_size);
	if (result != RX_PROTOCOL_OK)
		return result;
	result = rx_init_packet_buffer(&transport->receive_buffer, buffer_size, NULL);
	if (result != RX_PROTOCOL_OK)
	{
		rx_deinit_queue(&transport->send_queue);
		return result;
	}
	result = rx_init_stack(&transport->free_buffers, queue_size);
	if (result != RX_PROTOCOL_OK)
	{
		rx_deinit_packet_buffer(&transport->receive_buffer);
		rx_deinit_queue(&transport->send_queue);
		return result;
	}

	return RX_PROTOCOL_OK;
}
rx_protocol_result_t opcua_bin_init_client_transport(opcua_transport_protocol_type* transport
	, size_t buffer_size
	, size_t queue_size
	)
{
	return opcua_bin_init_transport(transport, buffer_size, queue_size, 0, 0);
}
rx_protocol_result_t opcua_bin_init_server_transport(opcua_transport_protocol_type* transport
	, size_t buffer_size
	, size_t queue_size
	)
{
	return opcua_bin_init_transport(transport, buffer_size, queue_size, 0, 1);
}


rx_protocol_result_t opcua_bin_init_pipe_transport(opcua_transport_protocol_type* transport
	, size_t buffer_size
	, size_t queue_size
	)
{
	return opcua_bin_init_transport(transport, buffer_size, queue_size, 1, 1);
}

rx_protocol_result_t opcua_bin_deinit_transport(opcua_transport_protocol_type* transport
	)
{
	rx_deinit_stack(&transport->free_buffers);
	rx_deinit_queue(&transport->send_queue);
	rx_deinit_packet_buffer(&transport->receive_buffer);

	return RX_PROTOCOL_OK;
}
rx_protocol_result_t opcua_bin_bytes_send(
	struct rx_protocol_stack_entry* reference
	,const protocol_endpoint* end_point
	, rx_packet_buffer* buffer)
{
	rx_protocol_result_t result;
	opcua_transport_header* header;
	opcua_security_simetric_header* sec_header;
	opcua_sequence_header* sequence_header;
	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference;


	size_t size = rx_get_packet_usable_data(buffer);
	if (size <= transport->connection_data.receive_buffer_size)
	{// one packet, just handle header stuff
		header = (opcua_transport_header*)rx_get_packet_data_at(buffer, 0, &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		sec_header = (opcua_security_simetric_header*)rx_get_packet_data_at(buffer, sizeof(opcua_transport_header), &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		sequence_header = (opcua_sequence_header*)rx_get_packet_data_at(buffer, sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header), &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		// header
		header->is_final = 'F';
		header->message_type[0] = 'M';
		header->message_type[1] = 'S';
		header->message_type[2] = 'G';
		header->message_size = (uint32_t)rx_get_packet_usable_data(buffer);
		// security header
		sec_header->channel_id = 0;
		sec_header->token_id = 0;
		// sequence header
		sequence_header->request_id = transport->current_request_id;
		transport->current_request_id++;
		sequence_header->sequence_number = transport->current_sequence_id;
		// TODO - handle rollover
		transport->current_sequence_id++;

		result = rx_enqueue(&transport->send_queue, buffer);
		if (result != RX_PROTOCOL_OK)
			return result;

	}
	else
		return RX_PROTOCOL_NOT_IMPLEMENTED;

	return opcua_bin_bytes_sent(reference, end_point, RX_PROTOCOL_OK);
}
rx_protocol_result_t opcua_bin_bytes_sent(
	struct rx_protocol_stack_entry* reference
	,const protocol_endpoint* end_point
	, rx_protocol_result_t result)
{
	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference;
	rx_packet_buffer buffer;

	if (!rx_queue_empty(&transport->send_queue))
	{// send another next packet from queue
		result = rx_dequeue(&transport->send_queue, &buffer);
		if (result != RX_PROTOCOL_OK)
			return result;
		result = rx_move_packet_down(&transport->protocol_stack_entry, end_point, &buffer);
		if (result != RX_PROTOCOL_OK)
		{
			result = rx_deinit_packet_buffer(&buffer);
			return result;
		}
	}

	return result;
}


