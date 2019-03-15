

/****************************************************************************
*
*  terminal\rx_terminal_style.h
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


#ifndef rx_terminal_style_h
#define rx_terminal_style_h 1






#define ANSI_COLOR_BOLD	   "\x1b[1m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_CLS "\x1b[2J"
#define ANSI_CUR_HOME "\033[0;0H"

#define ANSI_CLEAR_LINE "\x1b[2K"
#define ANSI_CUR_BEGIN "\033[100D"

#define ANSI_REDRAW_LINE ANSI_CLEAR_LINE ANSI_CUR_BEGIN

#define ANSI_STATUS_OK ANSI_COLOR_GREEN ANSI_COLOR_BOLD "OK" ANSI_COLOR_RESET
#define ANSI_STATUS_ERROR ANSI_COLOR_RED ANSI_COLOR_BOLD "ERROR" ANSI_COLOR_RESET

#define RX_CONSOLE_CHECKED "[X]"
#define RX_CONSOLE_UNCHECKED "[ ]"

#define ANSI_RX_PULL_COLOR ANSI_COLOR_YELLOW ANSI_COLOR_BOLD
#define ANSI_RX_EXECUTE_COLOR ANSI_COLOR_GREEN ANSI_COLOR_BOLD

#define ANSI_RX_OBJECT_COLOR ANSI_RX_PULL_COLOR


#define ANSI_RX_ERROR_LIST ANSI_COLOR_YELLOW ANSI_COLOR_BOLD





#endif
