

/****************************************************************************
*
*  host\rx_file_sys.cpp
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


#include "stdafx.h"


// rx_file_sys
#include "host/rx_file_sys.h"



namespace host {

namespace files {

// Class host::files::file_system_storage

file_system_storage::file_system_storage()
      : root_("")
{
}


file_system_storage::~file_system_storage()
{
}



void file_system_storage::get_storage_info (string_type& info)
{
}

sys_handle_t file_system_storage::get_host_test_file (const string_type& path)
{
	string_type full_path;
	rx_combine_paths(root_ + "_test/", path, full_path);
	sys_handle_t file = rx_file(full_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	return file;
}

sys_handle_t file_system_storage::get_host_console_script_file (const string_type& path)
{
	string_type full_path;
	rx_combine_paths(root_+"_script/", path, full_path);
	sys_handle_t file = rx_file(full_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	return file;
}

const string_type& file_system_storage::get_license ()
{
	static string_type lic_cached;
	static bool tried_get = false;
	if (!tried_get)
	{
		tried_get = true;
		sys_handle_t file = rx_file((root_+"LICENSE").c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
		if (file)
		{
			uint64_t size = 0;
			if (RX_OK == rx_file_get_size(file, &size) && size > 0)
			{
				lic_cached.assign(size, ' ');
				if (RX_OK != rx_file_read(file, &lic_cached[0], (uint32_t)size, nullptr))
				{
					lic_cached.clear();
				}
			}
			rx_file_close(file);
		}
	}
	return lic_cached;
}

void file_system_storage::init_storage ()
{
	root_ = get_root_folder();
}

void file_system_storage::deinit_storage ()
{
}


} // namespace files
} // namespace host

