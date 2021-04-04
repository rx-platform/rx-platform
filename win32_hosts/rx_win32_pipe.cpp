

/****************************************************************************
*
*  win32_hosts\rx_win32_pipe.cpp
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

#include "rx_win32_pipe_version.h"
#include "rx_win32_common.h"

// rx_win32_pipe
#include "win32_hosts/rx_win32_pipe.h"



namespace win32 {

// Class win32::win32_pipe_host 

win32_pipe_host::win32_pipe_host (const std::vector<storage_base::rx_platform_storage_type*>& storages)
      : supports_ansi_(false)
	, rx_pipe_host(storages)
{
}


win32_pipe_host::~win32_pipe_host()
{
}



string_type win32_pipe_host::get_default_name () const
{
	string_type ret;
	get_win_host_name(ret);
	return ret;
}

void win32_pipe_host::get_host_info (string_array& hosts)
{
	hosts.emplace_back(get_win32_pipe_info());
	host::pipe::rx_pipe_host::get_host_info(hosts);
}

string_type win32_pipe_host::get_win32_pipe_info ()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_WIN32_PIPE_HOST_NAME, RX_WIN32_PIPE_HOST_MAJOR_VERSION, RX_WIN32_PIPE_HOST_MINOR_VERSION, RX_WIN32_PIPE_HOST_BUILD_NUMBER);
	}
	return ret;
}

rx_result win32_pipe_host::fill_host_directories (hosting::rx_host_directories& data)
{
	return build_directories(data);
}

void win32_pipe_host::get_stdio_handles (sys_handle_t& in, sys_handle_t& out, sys_handle_t& err)
{
    in = GetStdHandle(STD_INPUT_HANDLE);
    out = GetStdHandle(STD_OUTPUT_HANDLE);
    err = GetStdHandle(STD_ERROR_HANDLE);
}

string_type win32_pipe_host::get_full_path (const string_type& path)
{
    return get_full_path_from_relative(path);
}

bool win32_pipe_host::supports_ansi () const
{
	return supports_ansi_;
}

rx_result win32_pipe_host::setup_console (int argc, char* argv[])
{
	auto out_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD in_mode = 0;
	DWORD out_mode = 0;
	GetConsoleMode(out_handle, &out_mode);

	std::bitset<32> out_bits(out_mode);

	out_bits.set(2);
	out_bits.set(0);
	//out_bits.reset(3);

	SetConsoleMode(out_handle, out_bits.to_ulong());

	if (GetConsoleMode(out_handle, &out_mode) && (ENABLE_VIRTUAL_TERMINAL_PROCESSING & out_mode))
		supports_ansi_ = true;

	return true;
}


} // namespace win32

