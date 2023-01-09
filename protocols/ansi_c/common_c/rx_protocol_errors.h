

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_protocol_errors.h
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


#ifndef rx_protocol_errors_h
#define rx_protocol_errors_h 1



// Hide ugly details about RX_COMMON_API
#ifdef _MSC_VER // MSVC compiler
#include "common/win32/win32_common.h"
#endif
#ifdef __GNUC__ // GCC compiler
#include "common/gnu/gnu_common.h"
#endif

#define RX_PROTOCOL_OK 0
#define RX_PROTOCOL_EMPTY 1
#define RX_PROTOCOL_WRONG_STATE 2
#define RX_PROTOCOL_PARSING_ERROR 3
#define RX_PROTOCOL_BUFFER_SIZE_ERROR 4
#define RX_PROTOCOL_NOT_IMPLEMENTED 5
#define RX_PROTOCOL_OUT_OF_MEMORY 6
#define RX_PROTOCOL_INVALID_SEQUENCE 7
#define RX_PROTOCOL_COLLECT_ERROR 8
#define RX_PROTOCOL_STACK_STRUCTURE_ERROR 9
#define RX_PROTOCOL_INSUFFICIENT_DATA 10
#define RX_PROTOCOL_INVALID_ADDR 11
#define RX_PROTOCOL_CLOSED 12
#define RX_PROTOCOL_DISCONNECTED 13
#define RX_PROTOCOL_INVALID_ARG 14


#ifdef __cplusplus
extern "C" {
#endif


typedef uint_fast8_t rx_protocol_result_t;
RX_COMMON_API const char* rx_protocol_error_message(rx_protocol_result_t code);



#ifdef __cplusplus
}
#endif


#endif
