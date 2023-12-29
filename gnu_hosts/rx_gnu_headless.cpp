

/****************************************************************************
*
*  gnu_hosts\rx_gnu_headless.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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

#include "rx_gnu_headless_version.h"
#include "rx_gnu_common.h"

// rx_gnu_headless
#include "gnu_hosts/rx_gnu_headless.h"



namespace gnu {

// Class gnu::gnu_headless_host 

gnu_headless_host::gnu_headless_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
  : host::headless::headless_platform_host(storages)
{
}


gnu_headless_host::~gnu_headless_host()
{
}



void gnu_headless_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_gnu_headless_info());
	host::headless::headless_platform_host::get_host_info(hosts);
}

string_type gnu_headless_host::get_gnu_headless_info ()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_GNU_HEADLESS_HOST_NAME, RX_GNU_HEADLESS_HOST_MAJOR_VERSION, RX_GNU_HEADLESS_HOST_MINOR_VERSION, RX_GNU_HEADLESS_HOST_BUILD_NUMBER);
	}
	return ret;
}

rx_result gnu_headless_host::fill_host_directories (hosting::rx_host_directories& data)
{
	return build_directories(data);
}

string_type gnu_headless_host::get_full_path (const string_type& path)
{
	return get_full_path_from_relative(path);
}

bool gnu_headless_host::supports_ansi () const
{
	return true;
}

void gnu_headless_host::fill_plugin_libs (string_array& paths)
{
	fill_plugin_paths(paths);
}


} // namespace gnu

