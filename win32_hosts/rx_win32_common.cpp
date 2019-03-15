

/****************************************************************************
*
*  win32_hosts\rx_win32_common.cpp
*
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_win32_common
#include "win32_hosts/rx_win32_common.h"

namespace win32
{
void get_full_path(const std::string& base, std::string& path)
{
	char buff[1024];

	if (GetModuleFileNameA(NULL, buff, 1024))
	{
		size_t j = strlen(buff);
		for (size_t i = j - 1; i > 0; i--)
		{
			if (buff[i] == L'\\')
			{
				buff[i + 1] = L'\0';
				break;
			}
		}

		strcat_s(buff, 1024, base.c_str());

		path = buff;
	}
}
void get_host_name(std::string& name)
{
	char buff[0x100];
	DWORD szname = sizeof(buff);
	GetComputerNameA(buff, &szname);
	name = buff;
}
}


