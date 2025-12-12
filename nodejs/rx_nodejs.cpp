

/****************************************************************************
*
*  nodejs\rx_nodejs.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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

#include "rx_nodejs_version.h"
#include "version/rx_code_macros.h"
#include "platform_api/rx_api.h"

// rx_nodejs
#include "nodejs/rx_nodejs.h"

#include "lib/rx_heap.cpp"
const char* deps[] = { nullptr };
RX_DECLARE_PLUGIN2(nodejs::nodejs_plugin, deps);


namespace nodejs {

// Class nodejs::nodejs_plugin 


rx_result nodejs_plugin::init_plugin ()
{
	return RX_NOT_IMPLEMENTED;
}

rx_result nodejs_plugin::deinit_plugin ()
{
	return RX_NOT_IMPLEMENTED;
}

rx_result nodejs_plugin::build_plugin ()
{
	return RX_NOT_IMPLEMENTED;
}


} // namespace nodejs

