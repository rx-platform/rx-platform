

/****************************************************************************
*
*  protocols\ansi_c\http_c\rx_http_c_impl.c
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


// rx_http_c_impl
#include "protocols/ansi_c/http_c/rx_http_c_impl.h"
#include "third-party/picohttpparser/picohttpparser.h"

const size_t http_buffer_size = 0x1000;

void http_closed(struct rx_protocol_stack_endpoint* transport, rx_protocol_result_t result)
{
}
rx_protocol_result_t http_close(struct rx_protocol_stack_endpoint* transport, rx_protocol_result_t result)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t http_allocate_buffer(struct rx_protocol_stack_endpoint* reference, rx_packet_buffer* buffer, size_t initial_size)
{
	rx_protocol_result_t result;
	void* temp_ptr;

	http_transport_protocol_type* transport = (http_transport_protocol_type*)reference->user_data;
	if (rx_stack_empty(&transport->free_buffers))
	{
		result = rx_init_packet_buffer(buffer, max(initial_size, transport->initial_packet_size), reference->downward);
		if (result == RX_PROTOCOL_OK)
		{
			temp_ptr = rx_alloc_from_packet(buffer, http_buffer_size, &result);
			if (!temp_ptr)
				result = rx_deinit_packet_buffer(buffer);
		}
		return result;
	}
	else
	{
		result = rx_pop(&transport->free_buffers, buffer);
		if (result == RX_PROTOCOL_OK)
		{
			temp_ptr = rx_alloc_from_packet(buffer, http_buffer_size, &result);
			if (!temp_ptr)
				result = rx_deinit_packet_buffer(buffer);
		}
		return result;
	}
}
rx_protocol_result_t http_free_buffer(struct rx_protocol_stack_endpoint* reference, rx_packet_buffer* buffer)
{
	http_transport_protocol_type* transport = (http_transport_protocol_type*)reference->user_data;
	rx_reinit_packet_buffer(buffer);
	return rx_push(&transport->free_buffers, buffer);
}
rx_protocol_result_t http_init_transport_state(http_transport_protocol_type* transport)
{
	memset(&transport->parser_data, 0, sizeof(http_parser_data));
	return RX_PROTOCOL_OK;
}
rx_protocol_result_t http_init_transport(http_transport_protocol_type* transport
	, size_t buffer_size
	, size_t queue_size)
{
	rx_protocol_result_t result;
	memset(transport, 0, sizeof(http_transport_protocol_type));
	rx_init_stack_entry(&transport->stack_entry, transport);

	// fill protocol stack header
	transport->stack_entry.received_function = http_bytes_received;
	//transport->stack_entry.send_function = http_bytes_send;
	//transport->stack_entry.ack_function = http_bytes_sent;
	//transport->stack_entry.closed_function = http_closed;
	//transport->stack_entry.close_function = http_close;
	transport->stack_entry.allocate_packet_function = http_allocate_buffer;
	transport->stack_entry.free_packet_function = http_free_buffer;
	// fill options
	transport->initial_packet_size = buffer_size;
	// fill state
	result = http_init_transport_state(transport);
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
rx_protocol_result_t http_deinit_transport(http_transport_protocol_type* transport)
{
	rx_deinit_stack(&transport->free_buffers);
	rx_deinit_queue(&transport->send_queue);
	rx_deinit_packet_buffer(&transport->receive_buffer);

	return RX_PROTOCOL_OK;
}

rx_protocol_result_t http_bytes_send(struct rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}
rx_protocol_result_t http_bytes_sent(struct rx_protocol_stack_endpoint* reference, rx_packet_id_type id, rx_protocol_result_t result)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}
rx_protocol_result_t http_bytes_received(struct rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
	char *method, *path;
	int pret, minor_version;
	struct phr_header headers[100];
	size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
	rx_protocol_result_t result;
	http_transport_protocol_type* transport = (http_transport_protocol_type*)reference->user_data;

	prevbuflen = transport->receive_buffer.size;
	result = rx_push_to_packet(&transport->receive_buffer, packet.buffer->buffer_ptr, packet.buffer->size);
	if (result == RX_PROTOCOL_OK)
	{
		buflen = transport->receive_buffer.size;
		num_headers = sizeof(headers) / sizeof(headers[0]);
		pret = phr_parse_request(transport->receive_buffer.buffer_ptr, buflen, &method, &method_len, &path, &path_len,
			&minor_version, headers, &num_headers, prevbuflen);
		if (pret > 0)
		{
			char path_buff[0x100];
			if (path_len + 1 < sizeof(path_buff))
			{
				memcpy(path_buff, path, path_len);
				path_buff[path_len] = '\0';
				strcat_s(path_buff, sizeof(path_buff), "\r\n");
				OutputDebugStringA(path_buff);
			}
			rx_reinit_packet_buffer(&transport->receive_buffer);
		}
		else if (pret == -1)
			result = RX_PROTOCOL_PARSING_ERROR;
		/* request is incomplete, continue the loop */
		//RX_ASSERT(pret == -2);
	//	if (buflen == sizeof(buf))
	//		return RX_PROTOCOL_BUFFER_SIZE_ERROR;
	}
	return result;
}


