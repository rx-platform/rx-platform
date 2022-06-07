

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_transport.c
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


#include "pch.h"


// rx_opcua_transport
#include "protocols/ansi_c/opcua_c/rx_opcua_transport.h"


rx_protocol_result_t opcua_init_transport_state(opcua_transport_protocol_type* transport)
{
	transport->current_state = opcua_transport_idle;
	memset(&transport->connection_data, 0, sizeof(opcua_acknowledge_message));
	transport->connection_data.protocol_version = RX_OPCUA_MAX_VERSION;

	return RX_PROTOCOL_OK;
}




rx_protocol_result_t opcua_check_header(uint8_t* header, size_t* size)
{
	uint32_t len = (*((uint32_t*)&header[4]));
	*size = len;
	return RX_PROTOCOL_OK;
}

rx_protocol_result_t opcua_bin_init_transport(opcua_transport_protocol_type* transport
	, int server_side)
{
	rx_protocol_result_t result;
	memset(transport, 0, sizeof(opcua_transport_protocol_type));
	rx_init_stack_entry(&transport->stack_entry, transport);

	// fill protocol stack header
	transport->stack_entry.received_function = opcua_bin_bytes_received;
	//transport->stack_entry.send_function = opcua_bin_bytes_send;
	//transport->stack_entry.closed_function = opcua_bin_closed;
	//transport->stack_entry.close_function = opcua_bin_close;
	// fill options
	transport->server_side = server_side;

	// fill state
	result = opcua_init_transport_state(transport);
	if (result != RX_PROTOCOL_OK)
		return result;

	return RX_PROTOCOL_OK;
}
rx_protocol_result_t opcua_bin_init_client_transport(opcua_transport_protocol_type* transport)
{
	return opcua_bin_init_transport(transport, 0);
}
rx_protocol_result_t opcua_bin_init_server_transport(opcua_transport_protocol_type* transport)
{
	return opcua_bin_init_transport(transport, 1);
}


rx_protocol_result_t opcua_bin_deinit_transport(opcua_transport_protocol_type* transport)
{

	return RX_PROTOCOL_OK;
}



const size_t opcua_hello_min_size = sizeof(opcua_transport_header) + sizeof(opcua_hello_message);
const size_t opcua_hello_max_size = sizeof(opcua_transport_header) + sizeof(opcua_hello_message) + 4096;


rx_protocol_result_t opcua_parse_hello_message(struct rx_protocol_stack_endpoint* stack, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
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
	result = stack->allocate_packet(stack, &response);
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
	ack_message = (opcua_acknowledge_message*)rx_alloc_from_packet(&response, sizeof(opcua_acknowledge_message), &result);
	if (!ack_message)
		return result;
	ack_message->protocol_version = transport->connection_data.protocol_version;
	ack_message->receive_buffer_size = transport->connection_data.receive_buffer_size;
	ack_message->send_buffer_size = transport->connection_data.send_buffer_size;
	ack_message->max_message_size = 0;
	ack_message->max_chunk_count = 0;


	response_header->message_size = (uint32_t)rx_get_packet_usable_data(&response);


	send_protocol_packet pack = rx_create_send_packet(id, &response, 0, 0);

	rx_const_packet_buffer to_channel;
	rx_init_const_from_packet_buffer(&to_channel, &response);

	recv_protocol_packet up_packet = rx_create_recv_packet(id, &to_channel, 0, 0);

	result = rx_move_packet_down(stack, pack);
	if (result == RX_PROTOCOL_OK)
	{
		// move packet up
		result = rx_move_packet_up(stack, up_packet);
		if (result == RX_PROTOCOL_OK)
		{
			transport->current_state = opcua_transport_opening;
		}
	}
	stack->release_packet(stack, &response);
	return result;
}
rx_protocol_result_t opcua_parse_reverse_hello_message(struct rx_protocol_stack_endpoint* stack, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}
rx_protocol_result_t opcua_parse_ack_message(struct rx_protocol_stack_endpoint* stack, opcua_transport_protocol_type* transport, const opcua_transport_header* header, rx_const_packet_buffer* buffer, rx_packet_id_type id)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
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

	result = RX_PROTOCOL_OK;

	available_data = rx_get_packet_available_data(packet.buffer);
	// check message sizes
	if (available_data <= sizeof(opcua_transport_header))
	{
		return RX_PROTOCOL_BUFFER_SIZE_ERROR;
	}

	size_t packet_start = packet.buffer->next_read;
	// check message type
	header = (opcua_transport_header*)rx_get_from_packet(packet.buffer, sizeof(opcua_transport_header), &result);
	if (result != RX_PROTOCOL_OK)
		return result;

	message_type = (header->message_type[0] << 16) | (header->message_type[1] << 8) | (header->message_type[2]);

	// check correct message size in header
	if (header->message_size > available_data)
	{
		return RX_PROTOCOL_PARSING_ERROR;
	}

	if (transport->server_side)
	{
		switch (message_type)
		{
		case opcua_error_msg_type:
			result = opcua_parse_error_message(reference, transport, header, packet.buffer, packet.id);
			break;
		case opcua_hello_msg_type:
			if (transport->current_state == opcua_transport_idle)
			{
				result = opcua_parse_hello_message(reference, transport, header, packet.buffer, packet.id);
				if (result == RX_PROTOCOL_OK)
				{
					transport->current_state = opcua_transport_opening;
				}
			}
			else
			{
				result =  RX_PROTOCOL_WRONG_STATE;
			}
			break;
		case opcua_reverse_hello_msg_type:
			result = RX_PROTOCOL_NOT_IMPLEMENTED;
			break;
		case opcua_open_msg_type:
		case opcua_close_msg_type:
		case opcua_regular_msg_type:
			// forward to the security channel 
			packet.buffer->next_read = packet_start;
			result = rx_move_packet_up(reference, packet);
			break;
		default:
			result = RX_PROTOCOL_PARSING_ERROR;
		}
	}
	else
	{
		switch (message_type)
		{
		case opcua_ack_msg_type:
			result = opcua_parse_ack_message(reference, transport, header, packet.buffer, packet.id);
			break;
		case opcua_error_msg_type:
			result = opcua_parse_error_message(reference, transport, header, packet.buffer, packet.id);
			break;
		case opcua_regular_msg_type:
		case opcua_open_msg_type:
		case opcua_close_msg_type:
			// forward to the security channel 
			result = rx_move_packet_up(reference, packet);
			break;
		default:
			result = RX_PROTOCOL_PARSING_ERROR;
		}
	}

	if (result != RX_PROTOCOL_OK)
	{
		opcua_init_transport_state(transport);
	}

	return result;
}

// we do not need send function, just pass through the stack what security channel sent
/*
rx_protocol_result_t opcua_bin_bytes_send(struct rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	uint8_t* buffer_front_ptr = NULL;
	rx_protocol_result_t result;
	opcua_transport_header* header;
	opcua_security_simetric_header* sec_header;
	opcua_sequence_header* sequence_header;
	opcua_transport_protocol_type* transport = (opcua_transport_protocol_type*)reference->user_data;
	rx_packet_buffer my_buffer;
	size_t headers_size = sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header) + sizeof(opcua_sequence_header);


	size_t size = rx_get_packet_usable_data(packet.buffer);
	if (size + headers_size <= transport->connection_data.send_buffer_size)
	{// one packet, just handle header stuff
		buffer_front_ptr = (uint8_t*)rx_alloc_from_packet_front(packet.buffer
			, headers_size, &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		header = (opcua_transport_header*)buffer_front_ptr;
		sec_header = (opcua_security_simetric_header*)&buffer_front_ptr[sizeof(opcua_transport_header)];
		sequence_header = (opcua_sequence_header*)&buffer_front_ptr[sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header)];
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

		packet.id = sequence_header->request_id;

		result = rx_move_packet_down(reference, packet);
		if (result != RX_PROTOCOL_OK)
			return result;
	}
	else
	{
		int last = 0;
		size_t idx = 0;
		size_t packet_size = 0;
		size_t max_data = transport->connection_data.send_buffer_size - headers_size;
		send_protocol_packet splited_packet = packet;
		while (idx < size)
		{
			result = reference->allocate_packet(reference, &my_buffer);
			if (result != RX_PROTOCOL_OK)
				return result;
			if (size - idx > max_data)
			{
				packet_size = max_data;
			}
			else
			{
				packet_size = size - idx;
				last = 1;
			}
			result = rx_push_to_packet(&my_buffer, packet.buffer->buffer_ptr + idx, packet_size);
			if (result != RX_PROTOCOL_OK)
				return result;
			idx += packet_size;

			buffer_front_ptr = (uint8_t*)rx_alloc_from_packet_front(&my_buffer
				, headers_size, &result);
			if (result != RX_PROTOCOL_OK)
				return result;
			header = (opcua_transport_header*)buffer_front_ptr;
			sec_header = (opcua_security_simetric_header*)&buffer_front_ptr[sizeof(opcua_transport_header)];
			sequence_header = (opcua_sequence_header*)&buffer_front_ptr[sizeof(opcua_transport_header) + sizeof(opcua_security_simetric_header)];
			// header
			header->is_final = last!=0 ? 'F' : 'C';
			header->message_type[0] = 'M';
			header->message_type[1] = 'S';
			header->message_type[2] = 'G';
			header->message_size = (uint32_t)rx_get_packet_usable_data(&my_buffer);
			// security header
			sec_header->channel_id = 0;
			sec_header->token_id = 0;
			// sequence header
			sequence_header->request_id = transport->current_request_id;
			sequence_header->sequence_number = transport->current_sequence_id;
			// TODO - handle rollover
			transport->current_sequence_id++;

			packet.id = sequence_header->request_id;

			splited_packet.buffer = &my_buffer;
			result = rx_move_packet_down(reference, splited_packet);

			reference->release_packet(reference, &my_buffer);

			if (result != RX_PROTOCOL_OK)
				return result;
		}
		transport->current_request_id++;
	}

	return RX_PROTOCOL_OK;
}*/


