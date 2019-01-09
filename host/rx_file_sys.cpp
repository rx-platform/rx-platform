

/****************************************************************************
*
*  host\rx_file_sys.cpp
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_file_sys
#include "host/rx_file_sys.h"

#include "rx_configuration.h"


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
		string_type lic_path;
		rx_combine_paths(root_, "LICENSE", lic_path);
		sys_handle_t file = rx_file(lic_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
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
	rx_combine_paths(get_root_folder(), RX_FILE_STORAGE_FOLDER, root_);
}

void file_system_storage::deinit_storage ()
{
}

void file_system_storage::list_storage (const string_type& path, server_directories_type& sub_directories, server_items_type& sub_items, const string_type& pattern)
{
	string_type result_path;
	rx_combine_paths(root_, path, result_path);
	string_array file_names, directory_names;
	rx_list_files(result_path, "*", file_names, directory_names);
	for(auto& one : file_names)
	{
		string_type one_path;
		rx_combine_paths(result_path, one, one_path);
		sub_items.push_back(file_system_file::smart_ptr(one_path, one)->get_item_ptr());
	}
}


// Class host::files::file_system_file 

file_system_file::file_system_file (const string_type& path, const string_type& name)
      : handle_(0),
        path_(path),
        name_(name),
        valid_(false)
{
}


file_system_file::~file_system_file()
{
}



values::rx_value file_system_file::get_value () const
{
	values::rx_value temp;
	temp.set_time(get_created_time());
	temp.set_quality(valid_ ? RX_GOOD_QUALITY : RX_BAD_QUALITY);
	return temp;
}

rx_time file_system_file::get_created_time () const
{
	return created_time_;
}

string_type file_system_file::get_name () const
{
	return name_;
}

size_t file_system_file::get_size () const
{
	return 0;
}


} // namespace files
} // namespace host

