

/****************************************************************************
*
*  protocols\ansi_c\internal_c\rx_internal_impl.c
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


// rx_internal_impl
#include "protocols/ansi_c/internal_c/rx_internal_impl.h"

struct rx_hosting_functions g_memory;


rx_protocol_result_t rx_c_alloc_buffer(void** buffer, size_t buffer_size)
{
	*buffer = malloc(buffer_size);
	if (buffer)
	{
		return RX_PROTOCOL_OK;
	}
	else
	{
		return RX_PROTOCOL_OUT_OF_MEMORY;
	}
}
rx_protocol_result_t rx_c_free_buffer(void* buffer, size_t buffer_size)
{
	free(buffer);
	return RX_PROTOCOL_OK;
}




