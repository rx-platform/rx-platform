

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_protocol_errors.c
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#include "pch.h"


// rx_protocol_errors
#include "protocols/ansi_c/common_c/rx_protocol_errors.h"


const char* error_messages[] =
{
	"OK",
	"Container is empty",
	"Wrong protocol state",
	"Protocol parsing error",
	"Buffer size error",
	"Not implemented",
	"Out of memory",
	"Invalid parsing sequence",
	"Transport collecting state error",
	"Protocol stack not defined completely",
	"Insufficient data"
};


const char* rx_protocol_error_message(rx_protocol_result_t code)
{
	if (code < sizeof(error_messages) / sizeof(error_messages[0]))
		return error_messages[code];
	else
		return "Unknown error code!";
}


