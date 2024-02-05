

/****************************************************************************
*
*  rx_errors.c
*
*  Copyright (c) 2018 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include <stdint.h>


// rx_errors
#include "rx_errors.h"

const char* rx_get_error_text(uint32_t code)
{
	switch (code)
	{
		case RX_INTERNAL_ERROR_NO_REGISTERED_NAME:
			return "Internal platform_error. No Constant Value registered name in parent (1001)";
		default:
			return "jebiga";
	}
}


