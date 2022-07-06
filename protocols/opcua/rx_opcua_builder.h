

/****************************************************************************
*
*  protocols\opcua\rx_opcua_builder.h
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


#ifndef rx_opcua_builder_h
#define rx_opcua_builder_h 1


#include "rx_opcua_std.h"


namespace protocols {
namespace opcua {
namespace opcua_addr_space {

class server_address_space;

rx_result build_standard_address_space(opcua_std_address_space& server, const string_type& server_uri, const string_type& app_uri, const string_type& server_type);



}// opcua_addr_space
}// opcua
}// protocols


#endif
