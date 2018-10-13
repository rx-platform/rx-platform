

/****************************************************************************
*
*  win32_hosts\rx_win32_file_sys.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_win32_file_sys
#include "win32_hosts/rx_win32_file_sys.h"



namespace win32 {

// Class win32::win32_file_system_storage 

win32_file_system_storage::win32_file_system_storage()
{
}


win32_file_system_storage::~win32_file_system_storage()
{
}



string_type win32_file_system_storage::get_root_folder ()
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
		return buff;
	}
	else
		return "";
}


} // namespace win32

