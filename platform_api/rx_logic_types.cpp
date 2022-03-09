

/****************************************************************************
*
*  platform_api\rx_logic_types.cpp
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


#include "pch.h"


// rx_logic_types
#include "platform_api/rx_logic_types.h"



namespace rx_platform_api {

// Class rx_platform_api::rx_method 

rx_method::rx_method()
{
}


rx_method::~rx_method()
{
}



rx_result rx_method::initialize_port (rx_init_context& ctx)
{
}

rx_result rx_method::start_port (rx_start_context& ctx)
{
}

rx_result rx_method::stop_port ()
{
}

rx_result rx_method::deinitialize_port ()
{
}


// Class rx_platform_api::rx_program 

rx_program::rx_program()
{
}


rx_program::~rx_program()
{
}



rx_result rx_program::initialize_port (rx_init_context& ctx)
{
}

rx_result rx_program::start_port (rx_start_context& ctx)
{
}

rx_result rx_program::stop_port ()
{
}

rx_result rx_program::deinitialize_port ()
{
}


} // namespace rx_platform_api

