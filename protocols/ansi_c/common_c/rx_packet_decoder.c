

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_packet_decoder.c
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


// rx_packet_decoder
#include "protocols/ansi_c/common_c/rx_packet_decoder.h"

RX_COMMON_API rx_protocol_result_t rx_init_packet_decoder(struct packed_decoder_type* decoder, struct rx_packet_buffer_type* recv_buffer, check_header_function_type chk_function, size_t hdr_size, void* hdr_buffer)
{
	memset(decoder, 0, sizeof(*decoder));
	decoder->header_size = hdr_size;
	decoder->check_header_func = chk_function;
	decoder->receive_buffer = recv_buffer;
	decoder->header_buffer = (uint8_t*)hdr_buffer;
	return RX_PROTOCOL_OK;
}

RX_COMMON_API rx_protocol_result_t rx_decode_arrived(struct packed_decoder_type* decoder, struct rx_const_packet_buffer_type* data, struct rx_const_packet_buffer_type* recv_buffer)
{
	size_t temp_size;
	rx_protocol_result_t result = RX_PROTOCOL_OK;
	size_t count = data->size - data->next_read;
	const void* buffer = NULL;
	if (decoder->header == NULL)
	{
		if (decoder->collected_header == 0)
		{
			// reinitialize collect buffer
			result = rx_reinit_packet_buffer(decoder->receive_buffer);
			if (result != RX_PROTOCOL_OK)
				return result;
		}
		size_t head_size = decoder->collected_header + count < decoder->header_size ? count : decoder->header_size - decoder->collected_header;
		buffer = rx_get_from_packet(data, head_size, &result);
		if (result != RX_PROTOCOL_OK)
			return result;

		memcpy(&decoder->header_buffer[decoder->collected_header], buffer, head_size);
		decoder->collected_header += head_size;
		if (decoder->collected_header < decoder->header_size)
			return result;

		decoder->collected_header = 0;
		decoder->header = decoder->header_buffer;

		result = decoder->check_header_func(decoder->header, &temp_size);
		if (result!= RX_PROTOCOL_OK)
			return result;

		decoder->expected = temp_size - decoder->header_size;
		result = rx_reinit_packet_buffer(decoder->receive_buffer);
		if (result != RX_PROTOCOL_OK)
			return result;
		result = rx_push_to_packet(decoder->receive_buffer, decoder->header_buffer, decoder->header_size);
		if (result != RX_PROTOCOL_OK)
			return result;
		count = count - head_size;
		decoder->collected_header = 0;
	}

	if (count + decoder->collected >= decoder->expected)
	{

		size_t usefull = decoder->expected - decoder->collected;
		buffer = rx_get_from_packet(data, usefull, &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		result = rx_push_to_packet(decoder->receive_buffer, buffer, usefull);
		if (result != RX_PROTOCOL_OK)
			return result;
		// fill receive buffer
		rx_init_const_from_packet_buffer(recv_buffer, decoder->receive_buffer);
		// set everything to 0
		decoder->collected = 0;
		decoder->expected = 0;
		decoder->header = NULL;
	}
	else if(count > 0)
	{
		// just move from input buffer
		size_t usefull = count;
		buffer = rx_get_from_packet(data, usefull, &result);
		if (result != RX_PROTOCOL_OK)
			return result;
		result = rx_push_to_packet(decoder->receive_buffer, buffer, usefull);
		if (result != RX_PROTOCOL_OK)
			return result;
		decoder->collected += count;
	}
	return RX_PROTOCOL_OK;
}


