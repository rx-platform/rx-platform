

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_session_map.h
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


#ifndef rx_session_map_h
#define rx_session_map_h 1


#include "rx_protocol_errors.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct protocol_endpoint_def
{
	// length of the whole data
	size_t data_length;
	// data itself
	uint8_t data[1];

} protocol_endpoint;

void rx_init_endpoint(protocol_endpoint* value, size_t value_size);

// functions for manipulating protocol endpoint



#ifdef __cplusplus
}
#endif


#endif
