

/****************************************************************************
*
*  system\server\rx_directory_cache.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#define RX_DIR_NEXT_PREFIX RX_DIR_DELIMETER_STR "{"

// rx_directory_cache
#include "system/server/rx_directory_cache.h"

#include "rx_configuration.h"


namespace rx_platform {

namespace ns {

// Class rx_platform::ns::rx_directory_cache 

rx_directory_cache::rx_directory_cache()
{
	root_ = rx_create_reference<rx_platform_directory>();
	root_->meta_.name = "";
	root_->meta_.path = "";
	root_->meta_.created_time = rx_time::now();
	root_->meta_.modified_time = root_->meta_.created_time;
	root_->meta_.version = RX_INITIAL_ITEM_VERSION;
	root_->meta_.attributes = namespace_item_internal_access;
	cache_.emplace(RX_DIR_DELIMETER_STR, root_);
	sub_items_cache_.emplace(root_, sub_items_cache_type::mapped_type());
}



rx_result_with<rx_directory_ptr> rx_directory_cache::add_directory (const string_type& dir_path, rx_storage_ptr storage)
{
	if (dir_path.size() > 2 && dir_path.substr(0, 2) == "//")
		RX_ASSERT(false);
	if (dir_path.empty())
		return RX_INVALID_PATH ", empty path is invalid";
	size_t idx = dir_path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
		return RX_INVALID_PATH;
	string_type name;
	string_type path;
	if (idx == 0)
	{
		name = dir_path.substr(1);
		path =RX_DIR_DELIMETER_STR;
	}
	else
	{
		name = dir_path.substr(idx + 1);
		path = dir_path.substr(0, idx);
	}
	if (name.empty())
		return RX_INVALID_PATH ", empty name is invalid";

	locks::const_auto_lock_t _(&cache_lock_);
	auto it = cache_.find(path);
	if (it != cache_.end())
	{
		if(it->second->sub_items_.find(name)!= it->second->sub_items_.end())
			return RX_INVALID_PATH ", name already exists";

		rx_directory_ptr dir = rx_create_reference<rx_platform_directory>();
		auto result = cache_.emplace(dir_path, dir);
		if (result.second)
		{
			sub_items_cache_[it->second].emplace(dir);
			if (storage)
				dir->storage_ = storage;
			else
				dir->storage_ = it->second->storage_;
			dir->meta_.name = name;
			dir->meta_.path = path;
			dir->meta_.created_time = rx_time::now();
			dir->meta_.modified_time = dir->meta_.created_time;

			valid_scope valid(it->second->valid_);
			it->second->meta_.modified_time = dir->meta_.created_time;
			if (storage)
				storage->set_base_path(dir_path);
			return dir;
		}
		else
		{
			return RX_INVALID_PATH ", directory already exists";
		}
	}
	else
	{
		return RX_INVALID_PATH;
	}
}

rx_result_with<rx_directory_ptr> rx_directory_cache::add_directory (rx_directory_ptr parent, const string_type& dir_name, rx_storage_ptr storage)
{
	string_type full_path = parent->meta_info().get_full_path();
	if(!full_path.empty() && *full_path.rbegin()==RX_DIR_DELIMETER)
		full_path += dir_name;
	else
		full_path += (RX_DIR_DELIMETER + dir_name);
	return add_directory(full_path, storage);
}

rx_result rx_directory_cache::remove_directory (const string_type& dir_path)
{
	if (dir_path.empty())
		return RX_INVALID_PATH ", empty path is invalid";
	size_t idx = dir_path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
		return RX_INVALID_PATH;
	string_type name;
	string_type path;
	if (idx == 0)
	{
		name = dir_path.substr(1);
		path = RX_DIR_DELIMETER_STR;
	}
	else
	{
		name = dir_path.substr(idx + 1);
		path = dir_path.substr(0, idx);
	}
	if (name.empty())
		return RX_INVALID_PATH ", empty name is invalid";

	rx_result result;
	rx_directory_ptr parent_dir;
	{
		locks::const_auto_lock_t _(&cache_lock_);
		auto it = cache_.find(dir_path);
		if (it != cache_.end())
		{
			if (!it->second->sub_items_.empty() || !sub_items_cache_[it->second].empty())
				return "Directory not empty";

			auto it_parent = cache_.find(path);
			RX_ASSERT(it_parent != cache_.end());
			if (it_parent == cache_.end())
				return "Sorry, but something critical happened, please restart platform!!!";

			auto it_parent_items = sub_items_cache_.find(it_parent->second);
			RX_ASSERT(it_parent_items != sub_items_cache_.end());
			if (it_parent_items == sub_items_cache_.end())
				return "Sorry, but something critical happened, please restart platform!!!";

			it_parent_items->second.erase(it->second);
			sub_items_cache_.erase(it->second);
			cache_.erase(it);
			parent_dir = it_parent->second;


			result = true;
		}
		else
		{
			result = RX_INVALID_PATH;
		}
	}
	if (result)
	{
		valid_scope valid(parent_dir->valid_);
		if(valid)
			parent_dir->meta_.modified_time = rx_time::now();
	}
	return result;
}

rx_result_with<rx_directory_ptr> rx_directory_cache::get_or_create_directory (const string_type& path)
{
	
	{
		locks::const_auto_lock_t _(&cache_lock_);
		auto it = cache_.find(path);
		if (it != cache_.end())
			return it->second;
	}
	return add_directory(path, rx_storage_ptr::null_ptr);
}

rx_directory_ptr rx_directory_cache::get_directory (const string_type& path) const
{
	locks::const_auto_lock_t _(&cache_lock_);
	auto it = cache_.find(path);
	if (it != cache_.end())
		return it->second;
	else
		return rx_directory_ptr::null_ptr;
}

rx_directory_ptr rx_directory_cache::get_sub_directory (rx_directory_ptr whose, const string_type& name) const
{
	string_type full_path = whose->meta_info().get_full_path();
	if (!name.empty())
	{
		if(!full_path.empty() && *full_path.rbegin() == RX_DIR_DELIMETER)
			full_path += name;
		else
			full_path += (RX_DIR_DELIMETER + name);
	}
	return get_directory(full_path);
}

bool rx_directory_cache::get_sub_directories (rx_directory_ptr whose, std::vector<rx_directory_ptr>& items, const char* c_pattern) const
{
	locks::const_auto_lock_t _(&cache_lock_);
	auto it = sub_items_cache_.find(whose);
	if (it != sub_items_cache_.end())
	{

		if (c_pattern)
		{
			for (auto one : it->second)
			{
				if (!rx_match_pattern(one->meta_info().name.c_str(), c_pattern, 0))
					continue;
				items.emplace_back(one);
			}
			return true;
		}
		else
		{
			std::copy(it->second.begin(), it->second.end(), std::back_inserter(items));
			return true;
		}
	}
	else
	{
		return false;
	}
}

rx_directory_cache& rx_directory_cache::instance ()
{
	static rx_directory_cache g_object;
	return g_object;
}

rx_directory_ptr rx_directory_cache::get_root () const
{
	return root_;
}

rx_result rx_directory_cache::insert_cached_item (const string_type& name, const rx_namespace_item& item)
{
	return names_cache_.insert_cached_item(name, item);
}

rx_result rx_directory_cache::remove_cached_item (const string_type& name, const rx_namespace_item& item)
{
	return RX_NOT_IMPLEMENTED;
}

rx_namespace_item rx_directory_cache::get_cached_item (const string_type& name) const
{
	return names_cache_.get_cached_item(name);
}

void rx_directory_cache::clear_cache ()
{
	cache_.clear();
	sub_items_cache_.clear();
	names_cache_.clear();
}


} // namespace ns
} // namespace rx_platform

