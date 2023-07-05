

/****************************************************************************
*
*  system\server\rx_ns.cpp
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


// rx_ns
#include "system/server/rx_ns.h"

#include "rx_server.h"
#include "sys_internal/rx_internal_ns.h"
#include "rx_directory_cache.h"
#include "sys_internal/rx_namespace_algorithms.h"
//IMPLEMENT_CODE_BEHIND_CLASS(rx_platform::ns::rx_server_directory, IEC61850 Engine, IEC61850 Ed 2 Goose Control Block - Stack Based, 1.0.0.0);


namespace rx_platform {
bool rx_is_valid_name_character(char ch)
{
	return ((ch >= 'a' && ch <= 'z')
		|| (ch >= 'A' && ch <= 'Z')
		|| (ch >= '0' && ch <= '9')
		|| ch == '_');
}
bool rx_is_valid_namespace_name(const string_type& name)
{
	if (name.empty())
		return false;
	for (const auto& one : name)
	{
		if (!rx_is_valid_name_character(one))
			return false;
	}
	return true;
}
void rx_split_path(const string_type& full_path, string_type& directory_path, string_type& item_path)
{
	auto idx = full_path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
	{// no directory stuff
		item_path = full_path;
	}
	else
	{// we have directory stuff

		directory_path = full_path.substr(0, idx);
		item_path = full_path.substr(idx + 1);
	}
}


namespace ns {
/*
namespace_item_read_access = 1,
namespace_item_write_access = 2,
namespace_item_delete_access = 4,
namespace_item_execute = 8,
namespace_item_system = 0x10
*/

void fill_attributes_string(namespace_item_attributes attr, string_type& str)
{
	str.assign(7, '-');
	str[4] = ' ';
	if (attr&namespace_item_read_access)
		str[0] = 'r';
	if (attr&namespace_item_write_access)
		str[1] = 'w';
	if (attr&namespace_item_delete_access)
		str[2] = 'd';
	if (attr&namespace_item_pull_access)
		str[3] = 'p';
	if (attr&namespace_item_execute_access)
		str[4] = 'x';
	//////////////////////////
	if (attr&namespace_item_system)
		str[6] = 's';
	if (attr&namespace_item_internal)
		str[6] = 'i';
}



// Class rx_platform::ns::rx_platform_directory 

rx_platform_directory::~rx_platform_directory()
{
#ifdef _DEBUG
	if(!meta_.name.empty())
		printf("Deleted directory: %s\r\n", meta_.name.c_str());
#endif
}



void rx_platform_directory::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = get_class_name();
	has_own_code_info = true;
}

rx_item_type rx_platform_directory::get_type_id () const
{
  // generated from ROSE!!!
  static string_type type_name = "DIR";
  return rx_item_type::rx_directory;


}

void rx_platform_directory::fill_dir_code_info (std::ostream& info)
{
	string_type name = meta_.path;
	fill_code_info(info, name);
}

meta_data rx_platform_directory::meta_info () const
{
	valid_scope valid(valid_);
	if (valid)
	{
		return meta_;
	}
	else
	{
		return meta_data();
	}
}

void rx_platform_directory::list_content (platform_directories_type& sub_directories, platform_items_type& sub_items, const string_type& pattern) const
{
	const char* c_pattern = pattern.empty() ? nullptr : pattern.c_str();
	rx_directory_cache::instance().get_sub_directories(smart_this(), sub_directories, c_pattern);
	
	valid_scope valid(valid_);
	if (valid)
	{
		for (const auto& one : sub_items_)
		{
			if (c_pattern)
			{
				if (!rx_match_pattern(one.first.c_str(), c_pattern, 0))
					continue;
			}
			sub_items.emplace_back(one.second);
		}
	}
}

rx_result rx_platform_directory::add_item (rx_namespace_item item)
{
	auto name = item.get_meta().name;
	rx_result ret;
	{
		valid_scope valid(valid_);
		if (valid)
		{
			auto it = sub_items_.find(name);
			if (it == sub_items_.end())
			{
				// check if name is reserved
				auto it = reserved_.find(name);
				if (it != reserved_.end())
					reserved_.erase(it);
				sub_items_.emplace(name, item);
			}
			else
			{
				ret.register_error("Item " + name + " already exists");
			}
		}
		else
		{
			ret = RX_NOT_VALID_DIRECTORY;
		}
	}
	if (ret && rx_names_cache::should_cache(item))
	{
		auto cache_result = ns::rx_directory_cache::instance().insert_cached_item(name, item);
		if (!cache_result)
		{
			std::ostringstream stream;
			stream << "Unable to register item "
				<< item.get_meta().get_full_path()
				<< " to names cache! Details: ";
			bool first = true;
			for (auto one : cache_result.errors())
			{
				if (first)
					first = false;
				else
					stream << ", ";
				stream << one;
			}
			stream << " ";
			NAMESPACE_LOG_WARNING("root", 900, stream.str().c_str());
		}
		else
		{
			std::ostringstream stream;
			stream << "Item "
				<< name
				<< " registered in cache";
			NAMESPACE_LOG_TRACE("root", 500, stream.str().c_str());
		}
	}
	return ret;
}

rx_result rx_platform_directory::reserve_name (const string_type& name)
{
	rx_result ret;
	valid_scope valid(valid_);
	if (valid)
	{
		auto it = sub_items_.find(name);
		if (it == sub_items_.end())
		{
			auto it2 = reserved_.find(name);
			if (it2 == reserved_.end())
			{
				reserved_.insert(name);
				ret = true;;
			}
			else
			{
				ret.register_error("Item " + name + " already reserved");
			}
		}
		else
			ret.register_error("Item " + name + " already exists");
	}
	else
	{
		ret = RX_NOT_VALID_DIRECTORY;
	}
	return ret;
}

rx_result rx_platform_directory::cancel_reserve (const string_type& name)
{
	valid_scope valid(valid_);
	if (valid)
	{
		reserved_.erase(name);
		return true;
	}
	else
	{
		return RX_NOT_VALID_DIRECTORY;
	}
}

rx_namespace_item rx_platform_directory::get_item (const string_type& name) const
{
	valid_scope valid(valid_);
	if (valid)
	{
		auto it = sub_items_.find(name);
		if (it != sub_items_.end())
		{
			return it->second;
		}
	}
	return rx_namespace_item();
}

rx_result rx_platform_directory::delete_item (const string_type& name)
{
	valid_scope valid(valid_);
	if (valid)
	{
		auto it = sub_items_.find(name);
		if (it != sub_items_.end())
		{
			sub_items_.erase(it);
			return true;
		}
		else
		{
			return "Item " + name + " not exists";
		}
	}
	else
	{
		return RX_NOT_VALID_DIRECTORY;
	}
}


// Class rx_platform::ns::rx_namespace_item 

rx_namespace_item::rx_namespace_item()
{
	type_ = rx_item_type::rx_invalid_type;
	version_ = 0;
}

rx_namespace_item::rx_namespace_item (const platform_item_ptr& who)
{
	type_ = who->get_type_id();
	meta_ = who->meta_info();
	version_ = meta_.version;
}



rx_namespace_item::operator bool () const
{
	return type_ != rx_item_type::rx_invalid_type;
}

bool rx_namespace_item::is_object () const
{
	return type_ == rx_application || type_ == rx_object || type_ == rx_port || type_ == rx_domain;
}

bool rx_namespace_item::is_type () const
{
	return type_ == rx_application_type || type_ == rx_object_type || type_ == rx_domain_type
		|| (type_ >= rx_port_type && type_ <= rx_mapper_type) || (type_ >= rx_relation_type && type_ <= rx_method_type);
}

rx_value rx_namespace_item::get_value () const
{
	rx_value ret;
	ret.assign_static(version_, meta_.modified_time);
	ret.set_good_locally();
	return ret;
}


const rx::meta_data& rx_namespace_item::get_meta () const
{
  return meta_;
}


rx_item_type rx_namespace_item::get_type () const
{
  return type_;
}


// Class rx_platform::ns::validity_lock 

validity_lock::validity_lock()
      : valid_(true),
        locked_(false)
{
}


validity_lock::~validity_lock()
{
	if (locked_)
		lock_.unlock();
}



// Class rx_platform::ns::valid_scope 

valid_scope::valid_scope (const validity_lock& lock)
      : lock_(lock)
{
}


valid_scope::~valid_scope()
{
	if (lock_.locked_)
	{
		locks::auto_lock_t _(&const_cast<validity_lock&>(lock_).lock_);
		const_cast<validity_lock&>(lock_).locked_ = false;
	}
}



valid_scope::operator bool () const
{
	if (lock_.valid_)
	{
		locks::auto_lock_t _(&const_cast<validity_lock&>(lock_).lock_);
		const_cast<validity_lock&>(lock_).locked_ = true;
		if (lock_.valid_)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


} // namespace ns
} // namespace rx_platform

