

/****************************************************************************
*
*  help\rx_help_security.h
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


#ifndef rx_help_security_h
#define rx_help_security_h 1


#include "terminal/rx_terminal_style.h"


#define HELP_RX_SEC_COMMAND "\
This command provides basic diagnostic and managment tools for security in the rx-platform\r\n\
\r\n\
command usage:\
" ANSI_COLOR_GREEN ":>sec " ANSI_COLOR_YELLOW ANSI_COLOR_BOLD "<sub-command>" ANSI_COLOR_RESET "\r\n\r\n\
List of available subcommands:\r\n\r\n\
" ANSI_COLOR_YELLOW ANSI_COLOR_BOLD "\
active" ANSI_COLOR_RESET" - displays list of currently active users\r\n\
" ANSI_COLOR_YELLOW ANSI_COLOR_BOLD "\
help" ANSI_COLOR_RESET" - displays more information about specific sub-command\r\n\
\r\n\
"
#define HELP_RX_SEC_ACTIVE_COMMAND "\
" ANSI_COLOR_YELLOW ANSI_COLOR_BOLD "\
sec active " ANSI_COLOR_RESET "command\r\n" RX_CONSOLE_HEADER_LINE "\r\n\
Displays currently active users in a table containing following columns:\r\n\
Id - current system wide id of the user\r\n\
User Name - name of the user\r\n\
Console - is there a console attached to this user instance?\r\n\
System - is this a system level user type?\r\n\
Port - associated port for this user instance\r\n\
\r\n\
The list of users is static!\r\n\
"




#endif
