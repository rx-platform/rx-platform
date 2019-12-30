

/****************************************************************************
*
*  system\server\rx_cmds.cpp
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

#include "terminal/rx_terminal_version.h"

// rx_cmds
#include "system/server/rx_cmds.h"

#include "system/server/rx_async_functions.h"
#include "terminal/rx_telnet.h"
#include "terminal/rx_commands.h"
#include "sys_internal/rx_internal_ns.h"
using namespace rx_platform;
using namespace terminal::commands;


#define SCRIPT_LINE_LENGTH 0x400



namespace rx_platform {

namespace prog {

// Class rx_platform::prog::server_script_host 

server_script_host::server_script_host (const script_def_t& definition)
{
}


server_script_host::~server_script_host()
{
}



} // namespace prog
} // namespace rx_platform

