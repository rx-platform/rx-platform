

/****************************************************************************
*
*  system\server\rx_file_helpers.cpp
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


#include "pch.h"


// rx_file_helpers
#include "system/server/rx_file_helpers.h"

namespace rx
{


rx_result create_directory(const std::string& dir, bool fail_on_exsists)
{
	return rx_create_directory(dir.c_str(), fail_on_exsists ? 1 : 0) != 0;
}
rx_result rx_delete_all_files(const std::string& dir, const std::string& pattern)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	auto result = rx_list_files(dir, pattern, files, dirs);
	if (result)
	{
		for (auto& one : files)
		{
			string_type temp_path = rx_combine_paths(dir, one);
			if (!rx_file_delete(temp_path.c_str()))
				return rx_result::create_from_last_os_error("Error deleting file.");
		}
	}
	return result;
}

rx_result rx_list_files(const std::string& dir, const std::string& pattern, std::vector<std::string>& files, std::vector<std::string>& directories)
{
	std::string search = rx_combine_paths(dir, pattern);
	rx_file_directory_entry_t one;

	find_file_handle_t hndl = rx_open_find_file_list(search.c_str(), &one);
	if (hndl)
	{
		do
		{
			if (strcmp(one.file_name, ".") != 0 && strcmp(one.file_name, "..") != 0)
			{
				if (one.is_directory)
					directories.emplace_back(one.file_name);
				else
					files.emplace_back(one.file_name);
			}

		} while (rx_get_next_file(hndl, &one));
		rx_find_file_close(hndl);
		return true;
	}
	else
	{
		return rx_result::create_from_last_os_error("Error listing directory "s + search);
	}
}
std::string rx_get_extension(const std::string& path)
{
	auto idx = path.find_last_of(".\\/");
	if (idx == string_type::npos || path[idx] != '.')
		return string_type();
	else
		return path.substr(idx + 1);
}
std::string rx_remove_extension(const std::string& path)
{
	auto idx = path.find_last_of(".\\/");
	if (idx == string_type::npos || path[idx] != '.')
		return string_type(path);
	else
		return path.substr(0, idx);
}
std::string rx_combine_paths(const std::string& path1, const std::string& path2)
{
	std::string path;
	path = path1;
	if (!path1.empty())
	{
		if (path1.at(path1.size() - 1) != '\\' && path1.at(path1.size() - 1) != '/')
		{
			path += "/";
		}
	}
	if (!path2.empty())
	{
		if (path2.at(0) == '\\' || path2.at(0) == '/')
			path += path2.substr(1);
		else
			path += path2;
	}
	return path;
}
rx_result file_exist(const std::string& file)
{
	return rx_file_exsist(file.c_str()) != 0;
}

rx_source_file::rx_source_file()
	: m_handle(0)
{
}
rx_result rx_source_file::open(const char* file_name)
{
	m_handle = rx_file(file_name, RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	if (m_handle != 0)
		return true;
	else
		return rx_result::create_from_last_os_error("Error opening file "s + file_name + " for read");
}
rx_result rx_source_file::open_write(const char* file_name)
{
	m_handle = rx_file(file_name, RX_FILE_OPEN_WRITE, RX_FILE_CREATE_ALWAYS);
	if (m_handle != 0)
		return true;
	else
		return rx_result::create_from_last_os_error("Error opening file "s + file_name + " for write");
}
rx_result rx_source_file::read_string(std::string& buff)
{
	if (m_handle == 0)
	{
		RX_ASSERT(false);
		return "File not opened!";
	}
	uint64_t size64;
	if (rx_file_get_size(m_handle, &size64) != RX_OK)
		return rx_result::create_from_last_os_error("Unable to get file size!");

	size_t size = (size_t)size64;
	char* temp = new char[size];

	uint32_t readed = 0;
	if (rx_file_read(m_handle, temp, (uint32_t)size, &readed) == RX_OK)
	{
		buff.assign(temp, size);
		delete[] temp;
		return true;
	}
	else
	{
		delete[] temp;
		return rx_result::create_from_last_os_error("Error reading file!");
	}
}
rx_result rx_source_file::write_string(const std::string& buff)
{
	if (m_handle == 0)
	{
		RX_ASSERT(false);
		return "File not opened!";
	}

	uint32_t size = (uint32_t)buff.size();
	uint32_t written = 0;
	if (rx_file_write(m_handle, buff.c_str(), size, &written) == RX_OK)
	{
		return true;
	}
	else
	{
		return rx_result::create_from_last_os_error("Error writing to file!");
	}
}
rx_result rx_source_file::read_data(byte_string& buff)
{
	if (m_handle == 0)
	{
		RX_ASSERT(false);
		return "File not opened!";
	}
	uint64_t size64;
	if (rx_file_get_size(m_handle, &size64) != RX_OK)
		return rx_result::create_from_last_os_error("Unable to get file size!");

	size_t size = (size_t)size64;
	uint8_t* temp = new uint8_t[size];

	uint32_t readed = 0;
	if (rx_file_read(m_handle, temp, (uint32_t)size, &readed) == RX_OK)
	{
		buff.assign(temp, temp + size);
		delete[] temp;
		return true;
	}
	else
	{
		delete[] temp;
		return rx_result::create_from_last_os_error("Error reading file!");
	}
}
rx_result rx_source_file::write_data(const byte_string& buff)
{
	if (m_handle == 0)
	{
		RX_ASSERT(false);
		return "File not opened!";
	}

	uint32_t size = (uint32_t)buff.size();
	uint32_t written = 0;
	if (rx_file_write(m_handle, &buff[0], size, &written) == RX_OK)
	{
		return true;
	}
	else
	{
		return rx_result::create_from_last_os_error("Error writing to file!");
	}
}
rx_source_file::~rx_source_file()
{
	if (m_handle != 0)
		rx_file_close(m_handle);
}

} // namespace rx


