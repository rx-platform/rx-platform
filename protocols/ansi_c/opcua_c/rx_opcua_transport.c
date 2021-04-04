

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_transport.c
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#include "pch.h"


// rx_opcua_transport
#include "protocols/ansi_c/opcua_c/rx_opcua_transport.h"

#include "rx_opcua_binary_connect.h"
#include "rx_opcua_binary_sec.h"

const uint32_t invalid_sequence_id = (uint32_t)(-1);




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

rx_protocol_result_t opcua_parse_regular_message(struct rx_protocol_stack_endpoint* stack, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
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
	sec_header = (opcua_security_simetric_header*)rx_get_from_packet(buffer, sizeof(opcua_security_simetric_header), &result);
	if (!sec_header)
		return result;
	sequence_header = (opcua_sequence_header*)rx_get_from_packet(buffer, sizeof(opcua_sequence_header), &result);
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
		recv_protocol_packet pack = rx_create_recv_packet(id, buffer, 0, 0);
		// one packet for all, send it directly
		result = rx_move_packet_up(stack, pack);
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

			recv_protocol_packet pack = rx_create_recv_packet(id, &upward_buffer, 0, 0);

			result = rx_move_packet_up(stack, pack);
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

rx_protocol_result_t opcua_parse_error_message(struct rx_protocol_stack_endpoint* stack, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t opcua_bin_bytes_received(struct rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	size_t available_data;
	rx_protocol_result_t result;
	opcua_transport_header* header;
	uint32_t message_type;
	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference->user_data;

	while (1)
	{

		available_data = rx_get_packet_available_data(packet.buffer);
		// check message sizes
		if (available_data <= sizeof(opcua_transport_header))
		{
			result = RX_PROTOCOL_BUFFER_SIZE_ERROR;
			break;
		}

		// check message type
		header = (opcua_transport_header*)rx_get_from_packet(packet.buffer, sizeof(opcua_transport_header), &result);
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
				result = opcua_parse_regular_message(reference, transport, header, packet.buffer, packet.id);
				break;
			case opcua_error_msg_type:
				result = opcua_parse_error_message(reference, transport, header, packet.buffer, packet.id);
				break;
			case opcua_hello_msg_type:
				result = opcua_parse_hello_message(reference, transport, header, packet.buffer, packet.id);
				break;
			case opcua_reverse_hello_msg_type:
				result = opcua_parse_reverse_hello_message(reference, transport, header, packet.buffer, packet.id);
				break;
			case opcua_open_msg_type:
				result = opcua_parse_open_message(reference, transport, header, packet.buffer, packet.id);
				break;
			case opcua_close_msg_type:
				result = opcua_parse_close_message(reference, transport, header, packet.buffer, packet.id);
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
				result = opcua_parse_regular_message(reference, transport, header, packet.buffer, packet.id);
				break;
			case opcua_ack_msg_type:
				result = opcua_parse_ack_message(reference, transport, header, packet.buffer, packet.id);
				break;
			case opcua_error_msg_type:
				result = opcua_parse_error_message(reference, transport, header, packet.buffer, packet.id);
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

void opcua_bin_closed(struct rx_protocol_stack_endpoint* transport, rx_protocol_result_t result)
{
}
rx_protocol_result_t opcua_bin_close(struct rx_protocol_stack_endpoint* transport, rx_protocol_result_t result)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t opcua_bin_allocate_buffer(struct rx_protocol_stack_endpoint* reference, rx_packet_buffer* buffer, size_t initial_size)
{
	rx_protocol_result_t result;
	void* temp_ptr;

	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference->user_data;
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
rx_protocol_result_t opcua_bin_free_buffer(struct rx_protocol_stack_endpoint* reference, rx_packet_buffer* buffer)
{
	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference->user_data;
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
	rx_init_stack_entry(&transport->stack_entry, transport);

	// fill protocol stack header
	transport->stack_entry.received_function = opcua_bin_bytes_received;
	transport->stack_entry.send_function = opcua_bin_bytes_send;
	transport->stack_entry.ack_function = opcua_bin_bytes_sent;
	transport->stack_entry.closed_function = opcua_bin_closed;
	transport->stack_entry.close_function = opcua_bin_close;
	transport->stack_entry.allocate_packet_function = opcua_bin_allocate_buffer;
	transport->stack_entry.free_packet_function = opcua_bin_free_buffer;
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
rx_protocol_result_t opcua_bin_bytes_send(struct rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	rx_protocol_result_t result;
	opcua_transport_header* header;
	opcua_security_simetric_header* sec_header;
	opcua_sequence_header* sequence_header;
	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference->user_data;


	size_t size = rx_get_packet_usable_data(packet.buffer);
	if (size <= transport->connection_data.receive_buffer_size)
	{// one packet, just handle header stuff
		header = (opcua_transport_header*)rx_get_packet_data_at(packet.buffer, 0, &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		sec_header = (opcua_security_simetric_header*)rx_get_packet_data_at(packet.buffer, sizeof(opcua_transport_header), &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		sequence_header = (opcua_sequence_header*)rx_get_packet_data_at(packet.buffer, sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header), &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		// header
		header->is_final = 'F';
		header->message_type[0] = 'M';
		header->message_type[1] = 'S';
		header->message_type[2] = 'G';
		header->message_size = (uint32_t)rx_get_packet_usable_data(packet.buffer);
		// security header
		sec_header->channel_id = 0;
		sec_header->token_id = 0;
		// sequence header
		sequence_header->request_id = transport->current_request_id;
		transport->current_request_id++;
		sequence_header->sequence_number = transport->current_sequence_id;
		// TODO - handle rollover
		transport->current_sequence_id++;

		result = rx_enqueue(&transport->send_queue, packet.buffer);
		if (result != RX_PROTOCOL_OK)
			return result;

	}
	else
		return RX_PROTOCOL_NOT_IMPLEMENTED;

	return opcua_bin_bytes_sent(reference, packet.id, RX_PROTOCOL_OK);
}
rx_protocol_result_t opcua_bin_bytes_sent(
	struct rx_protocol_stack_endpoint* reference
	, rx_packet_id_type id
	, rx_protocol_result_t result)
{
	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference->user_data;
	rx_packet_buffer buffer;

	if (!rx_queue_empty(&transport->send_queue))
	{// send another next packet from queue
		result = rx_dequeue(&transport->send_queue, &buffer);
		if (result != RX_PROTOCOL_OK)
			return result;


		send_protocol_packet pack = rx_create_send_packet(0, &buffer, 0, 0);

		result = rx_move_packet_down(reference, pack);//!!!!!!
		if (result != RX_PROTOCOL_OK)
			return result;
		rx_deinit_packet_buffer(&buffer);
	}

	return result;
}


