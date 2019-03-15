

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
	using storage_items = std::vector<hosting::rx_storage_item_ptr>;
	BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from system storage...");
	auto storage = rx_gate::instance().get_host()->get_system_storage();
	storage_items items;
	auto result = storage->list_storage(items);
	if (result)
	{

	}
	BUILD_LOG_INFO("configuration_storage_builder", 900, "Building from system storage done.");
	return true;
}


} // namespace storage
} // namespace builders
} // namespace sys_internal

