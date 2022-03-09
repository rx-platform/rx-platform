

/****************************************************************************
*
*  protocols\ansi_c\http_c\rx_http_c_impl.h
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


#ifndef rx_http_c_impl_h
#define rx_http_c_impl_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"
#include "third-party/picohttpparser/picohttpparser.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct http_parser_data_def
{
	int minor_version;
	char* method;
	char* path;
	struct phr_header headers[0x20];
	size_t prevbufflen;
	size_t method_len;
	size_t path_len;
	size_t num_headers;

} http_parser_data;

// definition of transport struct
typedef struct http_transport_protocol_def
{
	struct rx_protocol_stack_endpoint stack_entry;

	struct http_parser_data_def parser_data;
	// receive collector
	rx_packet_buffer receive_buffer;

} http_transport_protocol_type;
// initialize and deinitialize of transport
rx_protocol_result_t http_init_transport(http_transport_protocol_type* transport
	, size_t buffer_size
	, size_t queue_size);
rx_protocol_result_t http_deinit_transport(http_transport_protocol_type* transport);

rx_protocol_result_t http_bytes_send(struct rx_protocol_stack_endpoint* reference, send_protocol_packet packet);
rx_protocol_result_t http_bytes_sent(struct rx_protocol_stack_endpoint* reference, rx_packet_id_type id, rx_protocol_result_t result);
rx_protocol_result_t http_bytes_received(struct rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);



#ifdef __cplusplus
}
#endif


#endif
