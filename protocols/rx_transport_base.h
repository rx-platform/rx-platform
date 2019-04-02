

/****************************************************************************
*
*  protocols\rx_transport_base.h
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


#ifndef rx_transport_base_h
#define rx_transport_base_h 1


#include "rx_protocol_errors.h"
#include "rx_containers.h"


#ifdef __cplusplus
extern "C" {
#endif

	

typedef rx_transport_result_t(*rx_send_function_type)(void* reference, uint8_t* buffer, size_t buffer_size);
typedef rx_transport_result_t(*rx_collected_function_type)(void* reference, uint8_t* buffer, size_t buffer_size);

// callbacks
typedef struct rx_protocol_client_type
{
	void* reference;
	rx_send_function_type send_function;
	rx_collected_function_type collected_callback;

} rx_protocol_client;




#ifdef __cplusplus
}
#endif


#endif
