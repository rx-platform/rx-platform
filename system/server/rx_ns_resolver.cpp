

/****************************************************************************
*
*  system\server\rx_ns_resolver.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


// rx_ns_resolver
#include "system/server/rx_ns_resolver.h"

#include "rx_platform_item.h"
#include "rx_directory_cache.h"
#include "sys_internal/rx_namespace_algorithms.h"
#include "rx_configuration.h"


namespace rx_platform {

namespace ns {

// Class rx_platform::ns::rx_names_cache 

rx_names_cache::rx_names_cache()
{
}



rx_namespace_item rx_names_cache::get_cached_item (const string_type& name) const
{
	locks::const_auto_lock_t _(&lock_);

	const auto it = name_items_hash_.find(name);
	if (it != name_items_hash_.end() && it->second.size() == 1)
	{
		return *it->second.begin();
	}
	else
	{
		return std::move(rx_namespace_item());
	}
}

rx_result rx_names_cache::insert_cached_item (const string_type& name, const rx_namespace_item& item)
{
	std::scoped_lock _(lock_);

	auto it = name_items_hash_.find(name);
	if (it != name_items_hash_.end())
	{
		it->second.push_back(item);
		return true;
	}
	else
	{
		name_items_hash_[name] = { item };
		return true;
	}
}

bool rx_names_cache::should_cache (const platform_item_ptr& item)
{
	// stupid algorithm here, should be checked!!!
	//if (item->meta_info().attributes & namespace_item_system_mask)
	{
		return true;
	}
	//else
	{
	//	return false;
	}
}

bool rx_names_cache::should_cache (const rx_namespace_item& item)
{
	//if (item.get_meta().attributes & namespace_item_system_mask)
	{
		return true;
	}
	//else
	{
		return false;
	}
}

rx_result rx_names_cache::removed_cached_item (const string_type& name, const rx_node_id& id)
{
	std::scoped_lock _(lock_);

	auto it = name_items_hash_.find(name);
	if (it != name_items_hash_.end())
	{
		for (auto it_items = it->second.begin(); it_items != it->second.end(); it_items++)
		{
			if (it_items->get_meta().id == id)
			{
				it->second.erase(it_items);
				return true;
			}
		}
	}
	return RX_INVALID_ARGUMENT;
}

void rx_names_cache::clear ()
{
	std::scoped_lock _(lock_);

	name_items_hash_.clear();
}


// Class rx_platform::ns::rx_directory_resolver 

rx_directory_resolver::rx_directory_resolver()
	: parent_(nullptr)
{
}

rx_directory_resolver::rx_directory_resolver (rx_directory_resolver* parent)
	: parent_(parent)
{
}



rx_namespace_item rx_directory_resolver::resolve_item (const string_type& path) const
{
	if (path.empty())
		return rx_namespace_item();
	// path length is checked in previous condition!!!
	if (path[0] == RX_DIR_DELIMETER)
	{// global path
		string_type dir_path;
		string_type item_name;
		rx_split_path(path, dir_path, item_name);
		auto dir = rx_directory_cache::instance().get_directory(dir_path);
		if (dir)
			return dir->get_item(item_name);
		else
			return rx_namespace_item();
	}
	string_type dir_path;
	string_type item_name;
	rx_split_path(path, dir_path, item_name);
	if (dir_path.empty())
	{
		rx_namespace_item ret = rx_directory_cache::instance().get_cached_item(item_name);
		if (ret)
			return ret;
	}
	for (auto& one : directories_)
	{
		string_type resolved;
		auto result = rx_internal::internal_ns::namespace_algorithms::translate_path(one.path, dir_path, resolved);
		if (result)
		{
			auto dir = rx_directory_cache::instance().get_directory(resolved);
			if (dir)
			{
				rx_namespace_item ret = dir->get_item(item_name);
				if (ret)
					return ret;
			}
		}
	}
	return rx_namespace_item();
}

rx_directory_ptr rx_directory_resolver::resolve_directory (const string_type& path) const
{
	if (path.empty())
	{
		if(directories_.empty())
			return rx_directory_ptr();
		else
			return rx_directory_cache::instance().get_directory(directories_.rbegin()->path);
	}
	// path length is checked in previous condition!!!
	if (path[0] == RX_DIR_DELIMETER)
	{// global path
		return rx_directory_cache::instance().get_directory(path);
	}
	for (auto& one : directories_)
	{
		string_type resolved;
		auto result = rx_internal::internal_ns::namespace_algorithms::translate_path(one.path, path, resolved);
		if (result)
		{
			rx_directory_ptr ret = rx_directory_cache::instance().get_directory(resolved);
			if (ret)
				return ret;
		}
	}
	return rx_directory_ptr();
}

void rx_directory_resolver::add_paths (std::initializer_list<string_type> paths)
{
	for (auto&& one : paths)
		directories_.emplace_back(resolver_data{ std::forward<decltype(one)>(one) });
}


} // namespace ns
} // namespace rx_platform

