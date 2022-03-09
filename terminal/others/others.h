

/****************************************************************************
*
*  terminal\others\others.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef others_h
#define others_h 1




#include "asteroid.h"
#include "xena.h"
#include "urke.h"
#include "hcf.h"

void register_other(server_command_base_ptr cmd, std::map<string_type, server_command_base_ptr>& where_to)
{
	string_type con_name = cmd->get_console_name();
	where_to.emplace(con_name, cmd);
}
void register_others(std::map<string_type, server_command_base_ptr>& where_to)
{
	register_other(rx_create_reference<terminal::commands::asteroid_command>(), where_to);
	register_other(rx_create_reference<terminal::commands::xena_command>(), where_to);
	register_other(rx_create_reference<terminal::commands::urke_command>(), where_to);
	register_other(rx_create_reference<terminal::commands::hcf_command>(), where_to);
}




#endif
