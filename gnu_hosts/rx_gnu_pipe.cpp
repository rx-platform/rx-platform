

/****************************************************************************
*
*  gnu_hosts\rx_gnu_pipe.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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

#include "rx_gnu_pipe_version.h"
#include "rx_gnu_common.h"

// rx_gnu_pipe
#include "gnu_hosts/rx_gnu_pipe.h"



namespace gnu {

// Class gnu::gnu_pipe_host 

gnu_pipe_host::gnu_pipe_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
  : rx_pipe_host(storages)
{
}


gnu_pipe_host::~gnu_pipe_host()
{
}



string_type gnu_pipe_host::get_default_name () const
{
	string_type ret;
	get_gnu_host_name(ret);
	return ret;
}

void gnu_pipe_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_gnu_pipe_info());
	host::pipe::rx_pipe_host::get_host_info(hosts);
}

string_type gnu_pipe_host::get_gnu_pipe_info ()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
	{
		ASSIGN_MODULE_VERSION(ret, RX_GNU_PIPE_HOST_NAME, RX_GNU_PIPE_HOST_MAJOR_VERSION, RX_GNU_PIPE_HOST_MINOR_VERSION, RX_GNU_PIPE_HOST_BUILD_NUMBER);
	}
	return ret;
}

rx_result gnu_pipe_host::fill_host_directories (hosting::rx_host_directories& data)
{
	return build_directories(data);
}

void gnu_pipe_host::get_stdio_handles (sys_handle_t& in, sys_handle_t& out, sys_handle_t& err)
{
  in = 0;
  out = 1;
  err = 2;
}

string_type gnu_pipe_host::get_full_path (const string_type& path)
{
    return get_full_path_from_relative(path);
}

bool gnu_pipe_host::supports_ansi () const
{
  return true;

}


} // namespace gnu

