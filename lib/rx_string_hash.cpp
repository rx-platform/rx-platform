

/****************************************************************************
*
*  lib\rx_string_hash.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_string_hash
#include "lib/rx_string_hash.h"



namespace rx {

namespace rx_hashes {

// Class rx::rx_hashes::rx_string_hash 

rx_string_hash::rx_string_hash()
{
}


rx_string_hash::~rx_string_hash()
{
}



rx_string_hash& rx_string_hash::instance ()
{
	static rx_string_hash g_object;
	return g_object;
}


// Class rx::rx_hashes::hashed_string_type 

std::atomic<hash_handle_t> hashed_string_type::g_new_handle_;

hashed_string_type::hashed_string_type (const string_type& right)
{
	rx_string_hash::strings_hash_type& local_ref = rx_string_hash::instance().hash_;
	auto it = local_ref.find(rx_string_hash::hash_item_t { right.c_str() });
	if (it != local_ref.end())
	{
		handle_ = it->second;
	}
	else
	{
		rx_string_hash::instance().values_.emplace_back(right);
		handle_ = ++g_new_handle_;
		local_ref.emplace(rx_string_hash::hash_item_t{ right.c_str() }, handle_);
	}
}

hashed_string_type::hashed_string_type (const char* right)
{
	rx_string_hash::strings_hash_type& local_ref = rx_string_hash::instance().hash_;
	string_type tmp_str(right);
	auto it = local_ref.find(rx_string_hash::hash_item_t{ right });
	if (it != local_ref.end())
	{
		handle_ = it->second;
	}
	else
	{
		//const string_type& val = rx_string_hash::instance().values_.emplace_back(right);
		//handle_ = ++g_new_handle_;
		//local_ref.emplace(rx_string_hash::hash_item_t{ val.c_str() }, handle_);
	}
}


hashed_string_type::hashed_string_type(string_type&& right) noexcept
{
	rx_string_hash::strings_hash_type& local_ref = rx_string_hash::instance().hash_;
	auto it = local_ref.find(rx_string_hash::hash_item_t{ right.c_str() });
	if (it != local_ref.end())
	{
		handle_ = it->second;
	}
}
} // namespace rx_hashes
} // namespace rx

