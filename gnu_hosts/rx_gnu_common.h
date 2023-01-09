

/****************************************************************************
*
*  gnu_hosts\rx_gnu_common.h
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


#ifndef rx_gnu_common_h
#define rx_gnu_common_h 1




#include "system/hosting/rx_host.h"

namespace gnu
{
void get_full_path(const std::string& base, std::string& path);
std::string get_full_path_from_relative(const std::string& path);
rx_result build_directories(hosting::rx_host_directories& data);
void fill_plugin_paths(string_array& paths);
}




#endif
