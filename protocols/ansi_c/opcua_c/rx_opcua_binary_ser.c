

/****************************************************************************
*
*  protocols\ansi_c\opcua_c\rx_opcua_binary_ser.c
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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

#include "protocols/ansi_c/common_c/rx_packet_buffer.h"

// rx_opcua_binary_ser
#include "protocols/ansi_c/opcua_c/rx_opcua_binary_ser.h"

rx_protocol_result_t opcua_write_string(rx_packet_buffer* buffer, const char* str)
{
	rx_protocol_result_t result;

	uint32_t len;
	if (str)
	{
		len = (uint32_t)strlen(str);
		result = rx_push_to_packet(buffer, &len, sizeof(len));
		if (result == RX_PROTOCOL_OK && len > 0)
		{
			result= rx_push_to_packet(buffer, str, len);
		}
		return result;
	}
	else
	{// empty string just write -1
		len = (uint32_t)(-1);
		result = rx_push_to_packet(buffer, &len, sizeof(len));
		return result;
	}
}


