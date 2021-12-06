

/****************************************************************************
*
*  sys_internal\rx_namespace_algorithms.cpp
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


// rx_namespace_algorithms
#include "sys_internal/rx_namespace_algorithms.h"

#include "rx_configuration.h"
#include "system/server/rx_server.h"
#include "system/server/rx_directory_cache.h"


namespace rx_internal {

namespace internal_ns {

// Class rx_internal::internal_ns::namespace_algorithms 


rx_result_with<rx_directory_ptr> namespace_algorithms::get_or_create_direcotry (rx_directory_ptr from, const string_type& path)
{
	if (path.empty())
		return RX_INVALID_PATH;
	rx_result_with<rx_directory_ptr> result;
	rx_directory_ptr current_dir = from;
	rx_directory_ptr temp_dir;
	string_type temp_path;
	size_t last = 0;
	size_t next = 0;
	if (path[0] == RX_DIR_DELIMETER)// checked at begin for empty
	{
		// root folder path
		current_dir = rx_platform::rx_gate::instance().get_directory("/");
	}
	while ((next = path.find(RX_DIR_DELIMETER, last)) != string_type::npos)
	{
		temp_path = path.substr(last, next - last);
		temp_dir = ns::rx_directory_cache::instance().get_sub_directory(current_dir, temp_path);
		if (temp_dir)
		{
			current_dir = temp_dir;
		}
		else
		{
			result = ns::rx_directory_cache::instance().add_directory(current_dir, temp_path);
			if (!result)
				return result;
			current_dir = result.value();

		}
		last = next + 1;
	}
	temp_path = path.substr(last);
	temp_dir = ns::rx_directory_cache::instance().get_sub_directory(current_dir, temp_path);
	if (temp_dir)
	{
		current_dir = temp_dir;
	}
	else
	{
		result = ns::rx_directory_cache::instance().add_directory(current_dir, temp_path);
		if (!result)
			return result;
		current_dir = result.value();
	}
	return current_dir;
}

rx_result namespace_algorithms::translate_path (const string_type& ref_path, const string_type& rel_path, string_type& result)
{
	if (rel_path.empty())
	{
		result = ref_path;
		return true;
	}
	if (rel_path[0] == RX_DIR_DELIMETER)
	{//  this is full path
		result = rel_path;
		return true;
	}
	if (ref_path.empty())
		return RX_INVALID_PATH;
	size_t ref_idx = ref_path.size() - 1;
	size_t rel_idx = 0;
	size_t rel_size = rel_path.size();
	int state = 0;
	while (rel_idx < rel_size)
	{
		char ch = rel_path[rel_idx];
		if (ch == RX_DIR_DELIMETER)
		{
			rel_idx++;
		}
		else if (ch == '.')
		{
			rel_idx++;
			if (rel_idx < rel_size)
			{
				switch (rel_path[rel_idx])
				{
				case RX_DIR_DELIMETER:
					rel_idx++;
					break;
				case '.':
					{
						if (ref_idx == 0)
							return RX_INVALID_PATH;
						size_t help_idx = ref_path.rfind(RX_DIR_DELIMETER, ref_idx);
						if (help_idx == string_type::npos)
							return RX_INVALID_PATH;
						ref_idx = help_idx;
						if (ref_idx > 0)
							ref_idx--;
						rel_idx++;
					}
					break;
				default:
					return RX_INVALID_PATH;
				}
			}
		}
		else
		{
			break;
		}
	}
	if (rel_idx < rel_size)
	{
		if (ref_path[ref_idx] != RX_DIR_DELIMETER)
			result = ref_path.substr(0, ref_idx + 1) + RX_DIR_DELIMETER + rel_path.substr(rel_idx);
		else
			result = ref_path.substr(0, ref_idx + 1) + rel_path.substr(rel_idx);
	}
	else
	{
		if (ref_path[ref_idx] == RX_DIR_DELIMETER && ref_idx > 0)
			result = ref_path.substr(0, ref_idx);
		else
			result = ref_path.substr(0, ref_idx + 1);
	}
	return true;
}


} // namespace internal_ns
} // namespace rx_internal

