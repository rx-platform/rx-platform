

/****************************************************************************
*
*  win32_hosts\rx_win32_common.cpp
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


// rx_win32_common
#include "win32_hosts/rx_win32_common.h"

#include "system/server/rx_file_helpers.h"

namespace win32
{

string_type get_full_path_from_relative(const string_type& path)
{
	char buff[MAX_PATH];
	if (GetFullPathNameA(path.c_str(), MAX_PATH, buff, NULL))
	{
		return buff;
	}
	else
	{
		return path;
	}
}

rx_result build_directories(rx_host_directories& data)
{
	char buff[MAX_PATH];
	DWORD size = sizeof(buff);
#ifdef _DEBUG
	data.local_folder = get_full_path("");
#else
	GetCurrentDirectoryA(MAX_PATH, buff);
	data.local_folder = buff;
#endif
	if (GetEnvironmentVariableA("ProgramData", buff, size))
	{
		if (buff[0] == '"')
		{
			data.system_config = rx::rx_combine_paths(&buff[1], "rx-platform/config");
			data.system_storage = rx::rx_combine_paths(&buff[1], "rx-platform/storage/rx-system-storage");//copyright_file
			data.copyright_file = rx::rx_combine_paths(&buff[1], "rx-platform/config/COPYRIGHT");
			data.license_file = rx::rx_combine_paths(&buff[1], "rx-platform/config/LICENSE");
			data.http = rx::rx_combine_paths(&buff[1], "rx-platform/http");

			data.manuals = rx::rx_combine_paths(&buff[1], "rx-platform/man");
		}
		else
		{
			data.system_config = rx::rx_combine_paths(buff, "rx-platform/config");
			data.system_storage = rx::rx_combine_paths(buff, "rx-platform/storage/rx-system-storage");
			data.license_file = rx::rx_combine_paths(buff, "rx-platform/config/LICENSE");
			data.copyright_file = rx::rx_combine_paths(buff, "rx-platform/config/COPYRIGHT");
			data.manuals = rx::rx_combine_paths(buff, "rx-platform/man");
			data.http = rx::rx_combine_paths(buff, "rx-platform/http");
		}
		if (GetEnvironmentVariableA("LocalAppData", buff, sizeof(buff)))
		{
			if (buff[0] == '"')
			{
				data.user_config = rx::rx_combine_paths(&buff[1], "rx-platform/config");
				data.user_storage = rx::rx_combine_paths(&buff[1], "rx-platform/storage/default");
			}
			else
			{
				data.user_config = rx::rx_combine_paths(buff, "rx-platform/config");
				data.user_storage = rx::rx_combine_paths(buff, "rx-platform/storage/default");
			}
		}
		else
		{
			return rx_result::create_from_last_os_error("Unable to read ENV %LocalAppData%");
		}
	}
	else
	{
		return rx_result::create_from_last_os_error("Unable to read ENV %ProgramData%");
	}
	if (data.license_file.empty() || rx_file_exsist(data.license_file.c_str()) == 0)
	{
		data.license_file = get_full_path("license.txt");
	}
	return true;
}
string_type get_storage_directory()
{
	char buff[MAX_PATH];
	if (GetEnvironmentVariableA("ProgramData", buff, sizeof(buff)))
	{
		if (buff[0] == '"')
			return rx::rx_combine_paths(&buff[1], "rx-platform/storage");
		else
			return rx::rx_combine_paths(&buff[1], "rx-platform/storage");
	}
	else
	{
		return "";
	}
}

string_type get_full_path(const string_type& base)
{
	char buff[1024];

	if (GetModuleFileNameA(NULL, buff, 1024))
	{
		int found = 0;
		size_t j = strlen(buff);
		for (size_t i = j - 1; i > 0; i--)
		{
			if (buff[i] == L'\\' || buff[i]==L'/')
			{
				found++;
#ifndef _DEBUG
				if (found == 2)
#endif
				{
					buff[i + 1] = L'\0';
					break;
				}
			}
		}

		strcat_s(buff, 1024, base.c_str());

		return buff;
	}
	return "";
}
void get_win_host_name(string_type& name)
{
	char buff[0x100];
	DWORD szname = sizeof(buff);
	GetComputerNameA(buff, &szname);
	name = buff;
}
}


