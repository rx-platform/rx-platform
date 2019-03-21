

/****************************************************************************
*
*  sys_internal\rx_storage_build.cpp
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


// rx_storage_build
#include "sys_internal/rx_storage_build.h"



namespace sys_internal {

namespace builders {

namespace storage {

// Class sys_internal::builders::storage::configuration_storage_builder 

configuration_storage_builder::configuration_storage_builder()
{
}


configuration_storage_builder::~configuration_storage_builder()
{
}



rx_result configuration_storage_builder::do_build (platform_root::smart_ptr root)
{
	using storage_items = std::vector<rx_storage_item_ptr>;
	auto storage = rx_gate::instance().get_host()->get_system_storage();
	auto result = build_from_storage(root, *storage);
	if (result)
	{
		BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from system storage done.");
		storage = rx_gate::instance().get_host()->get_user_storage();
		result = build_from_storage(root, *storage);
		if (result)
		{
			BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from user storage done.");
			storage = rx_gate::instance().get_host()->get_test_storage();
			if (!storage->is_valid_storage())
			{
				// just return true, we can run without test storage
				BUILD_LOG_WARNING("configuration_storage_builder", 900, "Test storage not initialized!");
			}
			else
			{
				result = build_from_storage(root, *storage);
				if (result)
				{

					BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from test storage done.");
				}
				else
				{
					for (auto& err : result.errors())
						BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
					BUILD_LOG_ERROR("configuration_storage_builder", 900, "Error building from test storage");
					// we still can start without test storage so just return success!!!
					result = true;
				}
			}
		}
		else
		{
			for (auto& err : result.errors())
				BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
			BUILD_LOG_ERROR("configuration_storage_builder", 900, "Error building from user storage");
		}
	}
	else
	{
		for (auto& err : result.errors())
			BUILD_LOG_ERROR("configuration_storage_builder", 800, err.c_str());
		BUILD_LOG_ERROR("configuration_storage_builder", 900, "Error building from system storage");
	}
	return result;
}

rx_result configuration_storage_builder::build_from_storage (platform_root::smart_ptr root, rx_platform::storage_base::rx_platform_storage& storage)
{
	if (!storage.is_valid_storage())
		return "Storage not initialized!";

	directory_creator creator;

	using storage_items = std::vector<rx_storage_item_ptr>;
	storage_items items;
	auto result = storage.list_storage(items);
	if (result)
	{
		for (auto& item : items)
		{
			auto dir = creator.get_or_create_direcotry(root, item->get_path());
			if (dir)
			{

			}
			else
			{
				result = dir.errors();
				result.register_error("Error retrieving directory for the new item!");
			}
		}
	}
	else
	{
		result.register_error("Error listing storage");		
	}
	return result;
}


// Class sys_internal::builders::storage::directory_creator 


rx_result_with<rx_directory_ptr> directory_creator::get_or_create_direcotry (rx_directory_ptr from, const string_type& path)
{
	rx_result_with<rx_directory_ptr> result;
	rx_directory_ptr current_dir = from;
	rx_directory_ptr temp_dir;
	string_type temp_path;
	size_t last = 0;
	size_t next = 0;
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


} // namespace storage
} // namespace builders
} // namespace sys_internal

