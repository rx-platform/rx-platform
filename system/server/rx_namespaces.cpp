

/****************************************************************************
*
*  system\server\rx_namespaces.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


// rx_namespaces
#include "system/server/rx_namespaces.h"



namespace rx_platform {

namespace ns {

// Class rx_platform::ns::rx_namespace_array 

rx_namespace_array::rx_namespace_array()
{
	string_array default_namespaces = {
		"http://opcfoundation.org/UA/",
		"ENSACO.RxPlatform.System"
	};
	for (const auto& ns : default_namespaces) 
	{
		namespaces_.emplace(static_cast<uint16_t>(namespaces_.size() + 1), ns);
	}
	for (const auto& pair : namespaces_) 
	{
		rev_namespaces_.emplace(pair.second, pair.first);
	}
}


rx_namespace_array::~rx_namespace_array()
{
}



rx_result_with<uint16_t> rx_namespace_array::add_namespace (const string_type& name, const string_type& alias)
{
	std::scoped_lock lock(namespaces_lock_);
	
	auto it = rev_namespaces_.find(name);
	if (it != rev_namespaces_.end()) 
	{
		if (!alias.empty())
		{
			auto alias_it = aliases_.find(alias);
			if (alias_it != aliases_.end())
			{
				if (alias_it->second != name)
					return "Alias '" + alias + "' is already in use for namespace '" + alias_it->second + "'.";
			}
			aliases_.emplace(alias, name);
		}
		return it->second;
	}
	uint16_t new_id = static_cast<uint16_t>(namespaces_.size() + 1);
	namespaces_.emplace(new_id, name);
	rev_namespaces_.emplace(name, new_id);
	if (!alias.empty())
	{
		auto alias_it = aliases_.find(alias);
		if (alias_it != aliases_.end())
		{
			if (alias_it->second != name)	
			return "Alias '" + alias + "' is already in use for namespace '" + alias_it->second + "'.";
		}
		aliases_.emplace(alias, name);
	}
	return new_id;
}

string_type rx_namespace_array::namespace_lookup (uint16_t id) const
{
	locks::const_auto_lock_t lock(&namespaces_lock_);
	auto it = namespaces_.find(id);
	if (it != namespaces_.end()) {
		return it->second;
	}
	return string_type();
}

string_array rx_namespace_array::get_namespaces ()
{
	string_array result;
	result.reserve(namespaces_.size());
	for (const auto& pair : namespaces_)
	{
		result.emplace_back(pair.second);
	}
	return result;
}

rx_result_with<uint16_t> rx_namespace_array::get_namespace_id (const string_type& name, bool check_alias)
{
	locks::const_auto_lock_t lock(&namespaces_lock_);
	auto it = rev_namespaces_.find(name);
	if (it != rev_namespaces_.end()) {
		return it->second;
	}
	if (check_alias)
	{
		auto alias_it = aliases_.find(name);
		if (alias_it != aliases_.end())
			return get_namespace_id(alias_it->second, false);
	}

	return "Namespace not found";
}


// Class rx_platform::ns::rx_namespace_translator 

rx_namespace_translator::rx_namespace_translator()
	: mine_(nullptr)
	, pair_(nullptr)
{
}

rx_namespace_translator::rx_namespace_translator (rx_namespace_array* mine)
	: mine_(mine)
	, pair_(nullptr)
{
}

rx_namespace_translator::rx_namespace_translator (const rx_namespace_array* pair, rx_namespace_array* mine)
	: mine_(mine)
	, pair_(pair)
{
}


rx_namespace_translator::~rx_namespace_translator()
{
}



rx_result_with<uint16_t> rx_namespace_translator::translate_namespace (uint16_t in, const rx_namespace_array* pair)
{
	if(mine_ == nullptr)
	{
		return "Namespace translator not properly initialized.";
	}
	if(pair == nullptr)
	{
		pair = pair_;
	}
	if(pair == nullptr)
	{
		return "No valid pair namespace array provided.";
	}
	string_type ns_name = pair->namespace_lookup(in);
	if(ns_name.empty())
	{
		return "Input namespace ID not found in pair namespace array.";
	}
	std::scoped_lock lock(mine_->namespaces_lock_);
	auto it = mine_->rev_namespaces_.find(ns_name);
	if (it != mine_->rev_namespaces_.end())
	{
		return it->second;
	}
	uint16_t new_id = static_cast<uint16_t>(mine_->namespaces_.size() + 1);
	mine_->namespaces_.emplace(new_id, ns_name);
	mine_->rev_namespaces_.emplace(ns_name, new_id);
	return new_id;
}


} // namespace ns
} // namespace rx_platform

