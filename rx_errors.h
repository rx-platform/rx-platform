

/****************************************************************************
*
*  rx_errors.h
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


#ifndef rx_errors_h
#define rx_errors_h 1


// general codes
#define RX_FAILED 0x1

// item codes
#define RX_ITEM_NOT_FOUND 0x801

// Internal error codes
#define RX_INTERNAL_ERROR_NO_REGISTERED_NAME 0x1001
#define RX_DUPLICATED_NAME 0x1002




typedef uint32_t rx_error_code_t;
typedef uint_fast8_t rx_error_severity_t;

const char* rx_get_error_text(rx_error_code_t code);




#endif
