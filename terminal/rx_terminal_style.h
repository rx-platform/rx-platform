

/****************************************************************************
*
*  terminal\rx_terminal_style.h
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


#ifndef rx_terminal_style_h
#define rx_terminal_style_h 1




#include "ansi_codes.h"

#define ANSI_STATUS_OK ANSI_COLOR_GREEN ANSI_COLOR_BOLD "OK" ANSI_COLOR_RESET
#define ANSI_STATUS_ERROR ANSI_COLOR_RED ANSI_COLOR_BOLD "ERROR" ANSI_COLOR_RESET

#define RX_CONSOLE_CHECKED "[X]"
#define RX_CONSOLE_UNCHECKED "[ ]"

#define ANSI_RX_TYPE_COLOR ANSI_COLOR_WHITE ANSI_COLOR_BOLD
#define ANSI_RX_OBJECT_COLOR ANSI_COLOR_GREEN ANSI_COLOR_BOLD
#define ANSI_RX_APP_COLOR ANSI_COLOR_CYAN ANSI_COLOR_BOLD
#define ANSI_RX_DOMAIN_COLOR ANSI_COLOR_CYAN ANSI_COLOR_BOLD
#define ANSI_RX_PORT_COLOR ANSI_COLOR_YELLOW ANSI_COLOR_BOLD
#define ANSI_RX_DATA_COLOR ANSI_COLOR_GRAY ANSI_COLOR_BOLD


#define ANSI_RX_GOOD_COLOR ANSI_COLOR_GREEN ANSI_COLOR_BOLD
#define ANSI_RX_UNCERTAIN_COLOR ANSI_COLOR_YELLOW ANSI_COLOR_BOLD
#define ANSI_RX_BAD_COLOR ANSI_COLOR_RED ANSI_COLOR_BOLD
#define ANSI_RX_TEST_COLOR ANSI_COLOR_CYAN ANSI_COLOR_BOLD

#define ANSI_RX_DIR_COLOR ANSI_COLOR_CYAN ANSI_COLOR_BOLD


#define ANSI_RX_CONST_COLOR ANSI_COLOR_GRAY ANSI_COLOR_BOLD
#define ANSI_RX_VALUE_COLOR ANSI_COLOR_GREEN ANSI_COLOR_BOLD

#define ANSI_RX_VARIABLE_COLOR ANSI_COLOR_YELLOW ANSI_COLOR_BOLD

#define ANSI_RX_STRUCT_COLOR ANSI_COLOR_CYAN ANSI_COLOR_BOLD
#define ANSI_RX_MAPPER_COLOR ANSI_COLOR_RED
#define ANSI_RX_SOURCE_COLOR ANSI_COLOR_YELLOW
#define ANSI_RX_FILTER_COLOR ANSI_COLOR_WHITE
#define ANSI_RX_EVENT_COLOR ANSI_COLOR_WHITE 

#define ANSI_RX_RELATION_COLOR ANSI_COLOR_CYAN ANSI_COLOR_BOLD
#define ANSI_RX_RELATION_TARGET_COLOR ANSI_COLOR_CYAN ANSI_COLOR_BOLD


#define ANSI_RX_FUNCTION_COLOR ANSI_COLOR_WHITE
#define ANSI_RX_PROGRAM_COLOR ANSI_COLOR_WHITE

#define RX_TERMINAL_STRUCT_SYMBOL "{...}"
#define RX_TERMINAL_STRUCT_SYMBOL_SIZE 5
#define RX_TERMINAL_ARRAY_SYMBOL "[...]"

#define RX_TERMINAL_RELATION_SYMBOL "=>"
#define RX_TERMINAL_RELATION_TARGET_SYMBOL "<="


#define ANSI_RX_ERROR_LIST ANSI_COLOR_YELLOW ANSI_COLOR_BOLD

// log colors
#define ANSI_RX_LOG_INFO		ANSI_COLOR_CYAN ANSI_COLOR_BOLD
#define ANSI_RX_LOG_WARNING		ANSI_COLOR_YELLOW ANSI_COLOR_BOLD
#define ANSI_RX_LOG_ERROR		ANSI_COLOR_RED ANSI_COLOR_BOLD
#define ANSI_RX_LOG_CRITICAL	ANSI_COLOR_RED ANSI_COLOR_BOLD
#define ANSI_RX_LOG_DEBUG		ANSI_COLOR_BLUE ANSI_COLOR_BOLD
#define ANSI_RX_LOG_TRACE		ANSI_COLOR_GRAY
#define ANSI_RX_LOG_UNKNOWN		ANSI_COLOR_RED ANSI_COLOR_BOLD

#define RX_CONSOLE_WIDTH 60





#endif
