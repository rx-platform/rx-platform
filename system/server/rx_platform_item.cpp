

/****************************************************************************
*
*  system\server\rx_platform_item.cpp
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


// rx_platform_item
#include "system/server/rx_platform_item.h"



namespace rx_platform {

namespace ns {

// Class rx_platform::ns::rx_platform_item 

rx_platform_item::rx_platform_item()
{
}


rx_platform_item::~rx_platform_item()
{
}



rx_result rx_platform_item::save () const
{
	const auto& meta = meta_info();
	auto storage_result = meta.resolve_storage();
	if (storage_result)
	{
		auto item_result = storage_result.value()->get_item_storage(meta);
		if (!item_result)
		{
			item_result.register_error("Error saving item "s + meta.get_path());
			return item_result.errors();
		}
		auto result = item_result.value()->open_for_write();
		if (result)
		{
			result = serialize(item_result.value()->write_stream());
			item_result.value()->close();
		}
		return result;
	}
	else // !storage_result
	{
		rx_result result(storage_result.errors());
		storage_result.register_error("Error saving item "s + meta.get_path());
		return result;
	}
}

rx_result rx_platform_item::delete_item () const
{
	const auto& meta = meta_info();
	auto storage_result = meta.resolve_storage();
	if (storage_result)
	{
		auto item_result = storage_result.value()->get_item_storage(meta);
		if (!item_result)
		{
			item_result.register_error("Error saving item "s + meta.get_path());
			return item_result.errors();
		}
		auto result = item_result.value()->delete_item();

		return result;
	}
	else // !storage_result
	{
		rx_result result(storage_result.errors());
		storage_result.register_error("Error saving item "s + meta.get_path());
		return result;
	}
}


} // namespace ns
} // namespace rx_platform

