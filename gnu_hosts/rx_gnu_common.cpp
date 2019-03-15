

/****************************************************************************
*
*  gnu_hosts\rx_gnu_common.cpp
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


// rx_gnu_common
#include "gnu_hosts/rx_gnu_common.h"

namespace gnu
{
void get_full_path(const std::string& base, std::string& path)
{

	char lpath[PATH_MAX + 1];
	char buff[PATH_MAX + 1];
	//memset(buff,0,sizeof(buff)); // readlink does not null terminate!
	// does not need this, we'll read the place where to put zero
	// struct stat info;
	int ret;
	pid_t pid = getpid();
	sprintf(lpath, "/proc/%d/exe", pid);
	ret = readlink(lpath, buff, PATH_MAX);
	if (ret == -1)
		perror("readlink");
	else
	{
		// now plase zero at the end!
		buff[ret] = '\0';
	}
	if (ret != -1)
	{
		size_t j = strlen(buff);
		for (size_t i = j - 1; i > 0; i--)
		{
			if (buff[i] == L'/')
			{
				buff[i + 1] = L'\0';
				break;
			}
		}

		strcat(buff, base.c_str());

		path = buff;
	}
}
void get_host_name(std::string& name)
{
	char buff[0x100];
	gethostname(buff, sizeof(buff));
	name = buff;
}
}


