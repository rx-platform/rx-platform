

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
		current_dir = rx_platform::rx_gate::instance().get_root_directory();
	}
	while ((next = path.find(RX_DIR_DELIMETER, last)) != string_type::npos)
	{
		temp_path = path.substr(last, next - last);
		temp_dir = current_dir->get_sub_directory(temp_path);
		if (temp_dir)
		{
			current_dir = temp_dir;
		}
		else
		{
			result = current_dir->add_sub_directory(temp_path);
			if (!result)
				return result;
			current_dir = result.value();

		}
		last = next + 1;
	}
	temp_path = path.substr(last);
	temp_dir = current_dir->get_sub_directory(temp_path);
	if (temp_dir)
	{
		current_dir = temp_dir;
	}
	else
	{
		result = current_dir->add_sub_directory(temp_path);
		if (!result)
			return result;
		current_dir = result.value();
	}
	return current_dir;
}


} // namespace internal_ns
} // namespace rx_internal

