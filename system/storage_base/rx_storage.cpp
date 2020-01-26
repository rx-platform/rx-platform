

/****************************************************************************
*
*  system\storage_base\rx_storage.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_storage
#include "system/storage_base/rx_storage.h"

#include "system/server/rx_server.h"


namespace rx_platform {

namespace storage_base {

// Class rx_platform::storage_base::rx_platform_storage 

rx_platform_storage::rx_platform_storage()
{
}


rx_platform_storage::~rx_platform_storage()
{
}



rx_result rx_platform_storage::init_storage (const string_type& storage_reference)
{
	return RX_NOT_IMPLEMENTED;
}

void rx_platform_storage::deinit_storage ()
{
}


// Class rx_platform::storage_base::rx_storage_item 

rx_storage_item::rx_storage_item (const string_type& serialization_type)
      : serialization_type_(serialization_type)
{
}


rx_storage_item::~rx_storage_item()
{
}



// Class rx_platform::storage_base::rx_platform_storage_holder 

rx_platform_storage_holder::rx_platform_storage_holder()
{
}


rx_platform_storage_holder::~rx_platform_storage_holder()
{
}



void rx_platform_storage_holder::deinit_storage ()
{
	for (auto one : initialized_storages_)
	{
		one.second->deinit_storage();
	}
	initialized_storages_.clear();
}

rx_result_with<rx_storage_ptr> rx_platform_storage_holder::get_storage (const string_type& name)
{
	auto it = initialized_storages_.find(name);
	if (it != initialized_storages_.end())
	{
		return it->second;
	}
	else if(rx_gate::instance().get_platform_status() == rx_platform_status::initializing)
	{
		auto result = get_and_init_storage(name);
		if (result)
			initialized_storages_.emplace(name, result.value());
		return result;
	}
	else
	{
		return "Storage "s + name + "not found!";
	}
}


} // namespace storage_base
} // namespace rx_platform

