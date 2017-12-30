

/****************************************************************************
*
*  system\server\rx_ns.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "stdafx.h"

using namespace std::string_literals;

// rx_ns
#include "system/server/rx_ns.h"

#include "rx_server.h"


//IMPLEMENT_CODE_BEHIND_CLASS(rx_platform::ns::rx_server_directory, IEC61850 Engine, IEC61850 Ed 2 Goose Control Block - Stack Based, 1.0.0.0);


namespace rx_platform {

namespace ns {
/*
namespace_item_read_access = 1,
namespace_item_write_access = 2,
namespace_item_delete_access = 4,
namespace_item_execute = 8,
namespace_item_system = 0x10,
namespace_item_command = 0x20,
namespace_item_script = 0x40,
namespace_item_class = 0x80,
namespace_item_object = 0x100,
namespace_item_point = 0x200
*/
void fill_namepsace_string(namespace_item_attributes attr, string_type& str)
{
	str.assign(15, '-');
	str[4] = ' ';
	if (attr&namespace_item_read_access)
		str[0] = 'r';
	if (attr&namespace_item_write_access)
		str[1] = 'w';
	if (attr&namespace_item_delete_access)
		str[2] = 'd';
	if (attr&namespace_item_execute_access)
		str[3] = 'x';
	//////////////////////////
	if (attr&namespace_item_system)
		str[5] = 's';
	if (attr&namespace_item_command)
		str[6] = 'c';
	if (attr&namespace_item_script)
		str[7] = 's';
	if (attr&namespace_item_class)
		str[8] = 'c';
	if (attr&namespace_item_object)
		str[9] = 'o';
	if (attr&namespace_item_variable)
		str[10] = 'v';
	if (attr&namespace_item_application)
		str[11] = 'a';
	if (attr&namespace_item_domain)
		str[12] = 'd';
	if (attr&namespace_item_program)
		str[13] = 'p';
	if (attr&namespace_item_test_case)
		str[14] = 't';
}

void fill_quality_string(values::rx_value val, string_type& str)
{
	str = "-";
	str += " - - ";
	str += "--------------- --";
	//hi,lo stuff
	str += "--";
	if (val.is_good())
		str[0] = 'g';
	else if (val.is_uncertain())
		str[0] = 'u';
	else if (val.is_bad())
		str[0] = 'b';
	if (val.is_test())
		str[2] = 't';
	if (val.is_substituted())
		str[4] = 's';
}

// Class rx_platform::ns::rx_server_directory 

rx_server_directory::rx_server_directory()
      : created_(rx_time::now())
, name_("<unnamed>")
{
}

rx_server_directory::rx_server_directory (const string_type& name)
      : created_(rx_time::now())
	, name_(name)
{
}

rx_server_directory::rx_server_directory (const string_type& name, const server_directories_type& sub_directories, const server_items_type& items)
      : created_(rx_time::now())
	, name_(name)
{
	for (auto one : sub_directories)
	{
		auto it = sub_directories_.find(one->get_name());
		if (it == sub_directories_.end())
		{
			one->set_parent(smart_this());
			sub_directories_.emplace(one->get_name(), one);
		}
	}
	for (auto one : items)
	{
		auto it = sub_items_.find(one->get_name());
		if (it == sub_items_.end())
		{
			one->set_parent(smart_this());
			string_type name(one->get_name());
			sub_items_.emplace(name, one);
		}
	}
}


rx_server_directory::~rx_server_directory()
{
}



void rx_server_directory::get_content (server_directories_type& sub_directories, server_items_type& sub_items, const string_type& pattern) const
{
	locks::const_auto_slim_lock dummy(&structure_lock_);
	for (const auto& one : sub_directories_)
	{
		sub_directories.emplace_back(one.second);
	}
	for (const auto& one : sub_items_)
	{
		sub_items.emplace_back(one.second);
	}
}

void rx_server_directory::structure_lock ()
{
	structure_lock_.lock();
}

void rx_server_directory::structure_unlock ()
{
	structure_lock_.unlock();
}

server_directory_ptr rx_server_directory::get_parent () const
{
	server_directory_ptr ret;
	structure_lock();
	if (parent_)
	{
		parent_->structure_lock();
		ret = parent_;
		parent_->structure_unlock();
	}
	structure_unlock();
	return ret;
}

server_directory_ptr rx_server_directory::get_sub_directory (const string_type& path) const
{
	
	if (path.empty() || path == ".")
	{
		return smart_ptr::create_from_pointer(const_cast<rx_server_directory*>(this));
	}
	else if (path == "..")
	{// go one up
		server_directory_ptr ret = get_parent();
		if (!ret)
			return smart_ptr::create_from_pointer(const_cast<rx_server_directory*>(this));
		else
			return ret;
	}
	else if (path == "/")
	{// go one up
		return rx_gate::instance().get_root_directory();
	}
	else if (path[0] == '/')
	{// checked for empty before
		return rx_gate::instance().get_root_directory()->get_sub_directory(path.substr(1));
	}
	else
	{
		string_type mine;
		string_type rest;
		extract_next(path, mine, rest, '/');

		server_directory_ptr ret;

		locks::const_auto_slim_lock dummy(&structure_lock_);
		auto it = sub_directories_.find(mine);
		if (it != sub_directories_.end())
		{
			if (rest.empty())
				return it->second;
			else
				return it->second->get_sub_directory(rest);
		}
		else
			return server_directory_ptr::null_ptr;
	}
}

string_type rx_server_directory::get_path () const
{
	string_type path;
	fill_path(path);
	return path;
}

string_type rx_server_directory::get_name () const
{
	locks::const_auto_slim_lock dummy(&structure_lock_);
	if (name_.empty())
		return "/";
	else
		return name_;
}

void rx_server_directory::fill_path (string_type& path) const
{

	structure_lock();
	if (parent_)
	{
		parent_->fill_path(path);
	}
	if (name_.empty())
		path = '/';
	else
	{
		if (path != "/")
			path += '/';
		path += name_;
	}
	structure_unlock();
}

void rx_server_directory::set_parent (server_directory_ptr parent)
{
	/*if (library::cpp_classes_manager::instance().check_class(parent.get_code_behind()))
	{
		locks::auto_slim_lock dummy(&structure_lock_);
		parent_ = parent;
	}
	else
		RX_ASSERT(false);*/
	locks::auto_slim_lock dummy(&structure_lock_);
	parent_ = parent;
}

void rx_server_directory::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = get_class_name();
	has_own_code_info = true;
}

const string_type& rx_server_directory::get_type_name () const
{
  // generated from ROSE!!!
  static string_type type_name = "DIR";
  return type_name;


}

platform_item_ptr rx_server_directory::get_sub_item (const string_type& path) const
{
	size_t idx = path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
	{// plain item
		locks::const_auto_slim_lock dummy(&structure_lock_);
		auto it = sub_items_.find(path);
		if (it != sub_items_.end())
			return it->second;
		else
			return platform_item_ptr::null_ptr;
	}
	else
	{// dir + item
		string_type dir_path = path.substr(0, idx);
		string_type item_name = path.substr(idx + 1);
		server_directory_ptr dir = get_sub_directory(dir_path);
		if (dir)
		{
			return dir->get_sub_item(item_name);
		}
		else
		{
			return platform_item_ptr::null_ptr;
		}
	}
}

void rx_server_directory::structure_lock () const
{
	const_cast<rx_server_directory*>(this)->structure_lock_.lock();
}

void rx_server_directory::structure_unlock () const
{
	const_cast<rx_server_directory*>(this)->structure_lock_.unlock();
}

void rx_server_directory::get_value (rx_value& value)
{
	values::rx_value temp;
	temp.set_time(get_created());
	temp.set_quality(RX_GOOD_QUALITY);
	value = temp;
}

void rx_server_directory::fill_code_info (std::ostream& info)
{
	string_type name = get_path();

	fill_code_info(info, name);
}

void rx_server_directory::get_value (const string_type& name, rx_value& value)
{
}

bool rx_server_directory::add_sub_directory (server_directory_ptr who)
{
	////!!!!
	return false;
}

bool rx_server_directory::add_item (platform_item_ptr who)
{
	structure_lock();
	auto ret = sub_items_.emplace(who->get_name(), who);
	structure_unlock();
	return ret.second;
}


// Class rx_platform::ns::rx_platform_item 

rx_platform_item::rx_platform_item()
{
}

rx_platform_item::rx_platform_item (const string_type& name)
	: name_(name)
{
}


rx_platform_item::~rx_platform_item()
{
}



void rx_platform_item::code_info_to_string (string_type& info)
{
}

void rx_platform_item::fill_code_info (std::ostream& info, const string_type& name)
{
}

void rx_platform_item::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	has_own_code_info = false;
}

void rx_platform_item::lock ()
{
	item_lock_.lock();
}

void rx_platform_item::unlock ()
{
	item_lock_.unlock();
}

server_directory_ptr rx_platform_item::get_parent () const
{
	locks::auto_lock_t<rx_platform_item> dummy (const_cast<rx_platform_item*>(this));
	return server_directory_ptr::null_ptr;
}

void rx_platform_item::set_parent (server_directory_ptr parent)
{
	locks::auto_lock_t<rx_platform_item> dummy(this);
	parent_ = parent;
}

string_type rx_platform_item::get_path () const
{
	string_type ret;
	locks::auto_lock_t<rx_platform_item> dummy(const_cast<rx_platform_item*>(this));
	if (runtime_parent_)
	{
		return runtime_parent_->get_path() + "."s + name_;
	}
	else
	{
		return name_;
	}
}

bool rx_platform_item::serialize (base_meta_writter& stream) const
{
	if (!stream.write_string("Name", name_.c_str()))
		return false;

	return true;
}

bool rx_platform_item::deserialize (base_meta_reader& stream)
{
	
	if (!stream.read_string("Name", name_))
		return false;

	return true;
}

platform_item_ptr rx_platform_item::get_sub_item (const string_type& path) const
{
	size_t idx = path.rfind(RX_OBJECT_DELIMETER);
	if (idx == string_type::npos)
	{// plain item
		//locks::const_auto_slim_lock dummy(&(const_cast<rx_platform_item*>(this)->item_lock_));
		auto it = sub_items_.find(path);
		if (it != sub_items_.end())
			return it->second;
		else
			return platform_item_ptr::null_ptr;
	}
	else
	{// dir + item
		string_type dir_path = path.substr(0, idx);
		string_type item_name = path.substr(idx + 1);
		platform_item_ptr next = get_sub_item(dir_path);
		if (next)
		{
			return next->get_sub_item(item_name);
		}
		else
		{
			return platform_item_ptr::null_ptr;
		}
	}
}

void rx_platform_item::get_content (server_items_type& sub_items, const string_type& pattern) const
{
	bool simple = pattern.empty();
	for (const auto& one : sub_items_)
	{
		if (simple || match_pattern(one.first.c_str(), pattern.c_str(), true))
		{
			sub_items.emplace_back(one.second);
		}
	}
}


} // namespace ns
} // namespace rx_platform

