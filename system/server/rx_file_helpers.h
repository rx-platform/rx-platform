

/****************************************************************************
*
*  system\server\rx_file_helpers.h
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


#ifndef rx_file_helpers_h
#define rx_file_helpers_h 1




namespace rx
{

rx_result rx_list_files(const std::string& dir, const std::string& pattern, std::vector<std::string>& files, std::vector<std::string>& directories);
std::string rx_combine_paths(const std::string& path1, const std::string& path2);
std::string rx_get_extension(const std::string& path);
std::string rx_remove_extension(const std::string& path);

rx_result create_directory(const std::string& dir, bool fail_on_exsists);
rx_result rx_delete_all_files(const std::string& dir, const std::string& pattern);

class rx_source_file
{
	sys_handle_t m_handle;
public:
	rx_source_file();
	rx_result open(const char* file_name);
	rx_result open_write(const char* file_name);
	rx_result read_string(std::string& buff);
	rx_result write_string(const std::string& buff);
	~rx_source_file();

};


} // namespace rx




#endif
