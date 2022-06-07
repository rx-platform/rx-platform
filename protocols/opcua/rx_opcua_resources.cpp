

/****************************************************************************
*
*  protocols\opcua\rx_opcua_resources.cpp
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


// rx_opcua_resources
#include "protocols/opcua/rx_opcua_resources.h"

#include "rx_opcua_requests.h"


namespace protocols {

namespace opcua {

// Class protocols::opcua::opcua_resources_repository 


opcua_resources_repository& opcua_resources_repository::instance ()
{
	static opcua_resources_repository g_obj;
	return g_obj;
}

rx_result opcua_resources_repository::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
	requests::opcua_requests_repository::instance().init_requests();
	requests::opcua_requests_repository::instance().init_responses();
	return true;
}

void opcua_resources_repository::deinitialize ()
{
}


// Class protocols::opcua::opcua_security_channel 


// Class protocols::opcua::opcua_session 


} // namespace opcua
} // namespace protocols

