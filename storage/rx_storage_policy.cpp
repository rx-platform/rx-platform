

/****************************************************************************
*
*  storage\rx_storage_policy.cpp
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


// rx_storage_policy
#include "storage/rx_storage_policy.h"



namespace storage {

namespace storage_policy {

// Class storage::storage_policy::file_path_addresing_policy 


string_type file_path_addresing_policy::get_file_path (const meta::meta_data& meta) const
{
	locks::const_auto_lock_t<decltype(cache_lock_)> _(&cache_lock_);
	auto it = items_cache_.find(meta.get_id());
	if (it == items_cache_.end())
		return "";
	else
		return it->second;
}

string_type file_path_addresing_policy::get_new_file_path (const meta::meta_data& meta, const string_type& root)
{
	locks::auto_lock_t<decltype(cache_lock_)> _(&cache_lock_);
	string_type file_path = rx_combine_paths(root, meta.get_path());
	return "Fuck it TODO!!!";
}

void file_path_addresing_policy::add_file_path (const meta::meta_data& meta, const string_type& path)
{
	locks::auto_lock_t<decltype(cache_lock_)> _(&cache_lock_);
	items_cache_[meta.get_id()] = path;
}


} // namespace storage_policy
} // namespace storage

