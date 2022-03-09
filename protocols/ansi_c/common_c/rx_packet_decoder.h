

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_packet_decoder.h
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


#ifndef rx_packet_decoder_h
#define rx_packet_decoder_h 1


#include "rx_protocol_errors.h"
#include "rx_packet_buffer.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef rx_protocol_result_t(*check_header_function_type)(uint8_t* header, size_t* size);

typedef struct packed_decoder_type
{
	size_t header_size;
	size_t expected;
	size_t collected;
	uint8_t* header;
	size_t collected_header;
	uint8_t* header_buffer;

	struct rx_packet_buffer_type* receive_buffer;

	check_header_function_type check_header_func;

} packed_decoder;

RX_COMMON_API rx_protocol_result_t rx_init_packet_decoder(struct packed_decoder_type* decoder, struct rx_packet_buffer_type* recv_buffer, check_header_function_type chk_function, size_t hdr_size, void* hdr_buffer);

RX_COMMON_API rx_protocol_result_t rx_decode_arrived(struct packed_decoder_type* decoder, struct rx_const_packet_buffer_type* data, struct rx_const_packet_buffer_type* recv_buffer);




#ifdef __cplusplus
}
#endif


#endif
