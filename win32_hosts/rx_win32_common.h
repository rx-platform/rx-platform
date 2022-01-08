

/****************************************************************************
*
*  win32_hosts\rx_win32_common.h
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


#ifndef rx_win32_common_h
#define rx_win32_common_h 1



#include "system/hosting/rx_host.h"
using rx_platform::hosting::rx_host_directories;



namespace win32
{
void get_win_host_name(string_type& name);
rx_result build_directories(rx_host_directories& data);
string_type get_full_path_from_relative(const string_type& path);
string_type get_full_path(const string_type& base);
string_type get_storage_directory();


}




#endif
