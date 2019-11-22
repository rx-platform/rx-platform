

/****************************************************************************
*
*  system\server\rx_ns.cpp
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


// rx_ns
#include "system/server/rx_ns.h"

#include "rx_server.h"
#include "sys_internal/rx_internal_ns.h"
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

// Class rx_platform::ns::rx_platform_item 

rx_platform_item::rx_platform_item()
{
}


rx_platform_item::~rx_platform_item()
{
}



void rx_platform_item::code_info_to_string (string_type& info)
{
	std::ostringstream ss;
	fill_code_info(ss, this->get_name());
	info = ss.str();
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

rx_directory_ptr rx_platform_item::get_parent () const
{
	locks::auto_lock_t<rx_platform_item> dummy (const_cast<rx_platform_item*>(this));
	return parent_;
}

void rx_platform_item::set_parent (rx_directory_ptr parent)
{
	locks::auto_lock_t<rx_platform_item> dummy(this);
	parent_ = parent;
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

string_type rx_platform_item::callculate_path () const
{
	string_type ret;
	locks::auto_lock_t<rx_platform_item> dummy(const_cast<rx_platform_item*>(this));
	if (parent_)
	{
		parent_->fill_path(ret);
	}
	return ret + get_name();
}

bool rx_platform_item::is_object () const
{
	auto type = get_type_id();
	return type == rx_application || type == rx_object || type == rx_port || type == rx_domain;
}

bool rx_platform_item::is_type () const
{
	auto type = get_type_id();
	return type == rx_application_type || type == rx_object_type || type == rx_domain_type
		|| (type >= rx_port_type && type <= rx_mapper_type) || type == rx_relation_type;
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


// Class rx_platform::ns::rx_platform_directory 

rx_platform_directory::rx_platform_directory (const string_type& name, namespace_item_attributes attrs, rx_storage_ptr storage)
      : storage_(storage)
	, meta_(name, rx_node_id::null_id, rx_node_id::null_id, attrs, "")
{
}


rx_platform_directory::~rx_platform_directory()
{
}



void rx_platform_directory::get_content (platform_directories_type& sub_directories, platform_items_type& sub_items, const string_type& pattern) const
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

void rx_platform_directory::structure_lock ()
{
	structure_lock_.lock();
}

void rx_platform_directory::structure_unlock ()
{
	structure_lock_.unlock();
}

rx_directory_ptr rx_platform_directory::get_parent () const
{
	rx_directory_ptr ret;
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

rx_directory_ptr rx_platform_directory::get_sub_directory (const string_type& path) const
{
	size_t idx = path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
	{// plain item
		if (path.empty() || path == ".")
		{
			return smart_ptr::create_from_pointer(const_cast<rx_platform_directory*>(this));
		}
		else if (path == "..")
		{// go one up
			rx_directory_ptr ret = get_parent();
			if (!ret)
				return smart_ptr::create_from_pointer(const_cast<rx_platform_directory*>(this));
			else
				return ret;
		}
		else
		{
			locks::const_auto_slim_lock dummy(&structure_lock_);
			auto it = sub_directories_.find(path);
			if (it != sub_directories_.end())
				return it->second;
			else
				return rx_directory_ptr::null_ptr;
		}
	}
	else
	{
		if (path[0] == '/')
		{// checked for empty before
			return rx_gate::instance().get_root_directory()->get_sub_directory(path.substr(1));
		}
		else
		{
			string_type mine;
			string_type rest;
			extract_next(path, mine, rest, '/');

			rx_directory_ptr temp = get_sub_directory(mine);
			if (temp)
				return temp->get_sub_directory(rest);
			else
				return rx_directory_ptr::null_ptr;
		}
	}
}

string_type rx_platform_directory::get_name () const
{
	string_type ret;
	{
		locks::const_auto_slim_lock dummy(&structure_lock_);
		ret = meta_.get_name();
	}
	if (ret.empty())
		return string_type({ RX_DIR_DELIMETER });
	else
		return ret;
}

void rx_platform_directory::fill_path (string_type& path) const
{
	string_type name;
	structure_lock();
	if (parent_)
	{
		parent_->fill_path(path);
	}
	name = meta_.get_name();
	structure_unlock();
	if (name.empty())
		path = RX_DIR_DELIMETER;
	else
	{
		if (path != "/")
			path += RX_DIR_DELIMETER;
		path += name;
	}
}

void rx_platform_directory::set_parent (rx_directory_ptr parent)
{
	/*if (library::cpp_classes_manager::instance().check_class(parent.get_code_behind()))
	{
		locks::auto_slim_lock dummy(&structure_lock_);
		parent_ = parent;
	}
	else
		RX_ASSERT(false);*/

	string_type path;
	parent->fill_path(path);
	locks::auto_lock_t<decltype(structure_lock_)> _(&structure_lock_);
	parent_ = parent;
	meta_.set_path(path);
	for (auto& one : sub_directories_)
		one.second->set_parent(smart_this());
}

namespace_item_attributes rx_platform_directory::get_attributes () const
{
	locks::const_auto_lock_t<decltype(structure_lock_)> _(&structure_lock_);
	return meta_.get_attributes();
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

platform_item_ptr rx_platform_directory::get_sub_item (const string_type& path) const
{
	size_t idx = path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
	{// plain item
		auto ret_ptr = sys_internal::internal_ns::platform_root::get_cached_item(path);
		if (ret_ptr)
		{// found it in cache, return!
			return ret_ptr;
		}
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
		rx_directory_ptr dir = get_sub_directory(dir_path);
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

void rx_platform_directory::structure_lock () const
{
	const_cast<rx_platform_directory*>(this)->structure_lock_.lock();
}

void rx_platform_directory::structure_unlock () const
{
	const_cast<rx_platform_directory*>(this)->structure_lock_.unlock();
}

void rx_platform_directory::get_value (rx_value& value)
{
	value.assign_static(get_name(), meta_.get_created_time());
}

void rx_platform_directory::fill_dir_code_info (std::ostream& info)
{
	string_type name = meta_.get_path();
	fill_code_info(info, name);
}

void rx_platform_directory::get_value (const string_type& name, rx_value& value)
{
}

rx_result rx_platform_directory::add_sub_directory (rx_directory_ptr who)
{
	rx_result ret;
	structure_lock();
	auto it = sub_directories_.find(who->meta_.get_name());
	if (it == sub_directories_.end())
	{
		sub_directories_.emplace(who->meta_.get_name(), who);
		who->set_parent(smart_this());
	}
	else
	{
		ret.register_error("Directory " + who->meta_.get_name() + " already exists");
	}
	structure_unlock();
	return ret;
}

rx_result rx_platform_directory::add_item (platform_item_ptr who)
{
	auto name = who->get_name();
	rx_result ret;
	structure_lock();
	auto it = sub_items_.find(name);
	if (it == sub_items_.end())
	{
		// check if name is reserved
		auto it = reserved_.find(name);
		if (it != reserved_.end())
			reserved_.erase(it);
		sub_items_.emplace(name, who);
		who->set_parent(smart_this());
	}
	else
	{
		ret.register_error("Item " + name + " already exists");
	}
	structure_unlock();
	if (ret && rx_names_cache::should_cache(who))
	{
		auto cache_result = sys_internal::internal_ns::platform_root::insert_cached_item(name, who);
		if (!cache_result)
		{
			std::ostringstream stream;
			stream << "Unable to register item "
				<< who->meta_info().get_full_path()
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

rx_result rx_platform_directory::delete_item (platform_item_ptr who)
{
	rx_result ret;
	structure_lock();
	auto it = sub_items_.find(who->get_name());
	if (it != sub_items_.end())
	{
		sub_items_.erase(it);
	}
	else
	{
		ret.register_error("Item does not exists!");
	}
	structure_unlock();
	return ret;
}

rx_result rx_platform_directory::delete_item (const string_type& path)
{
	size_t idx = path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
	{// plain item
		locks::const_auto_slim_lock dummy(&structure_lock_);
		auto it = sub_items_.find(path);
		if (it != sub_items_.end())
		{
			sub_items_.erase(it);
			return true;
		}
	}
	else
	{// dir + item
		string_type dir_path = path.substr(0, idx);
		string_type item_name = path.substr(idx + 1);
		rx_directory_ptr dir = get_sub_directory(dir_path);
		if (dir)
		{
			return dir->delete_item(item_name);
		}
	}
	return false;
}

rx_result_with<rx_directory_ptr> rx_platform_directory::add_sub_directory (const string_type& path)
{
	if (path.empty())
		return "Invalid directory name:"s + path;
	auto idx = path.rfind(RX_DIR_DELIMETER);
	if (idx != string_type::npos)
	{// plain name just
		string_type name(path.substr(idx + 1));
		auto where = get_sub_directory(path.substr(0, idx));
		if (!where)
			return "Directory "s + path.substr(0, idx) + " not found!";
		else
			return where->add_sub_directory(name);
	}
	if (path.empty() || !rx_is_valid_namespace_name(path))
	{
		return "Invalid directory name:"s + path;
	}
	rx_directory_ptr new_dir = rx_create_reference<sys_internal::internal_ns::user_directory>(path);
	auto result = add_sub_directory(new_dir);
	if (result)
		return new_dir;
	else
		return result.errors();
}

rx_result rx_platform_directory::delete_sub_directory (const string_type& path)
{
	if (path.empty())
		return "Invalid directory name!";
	auto idx = path.rfind(RX_DIR_DELIMETER);
	if (idx != string_type::npos)
	{// plain name just
		string_type name(path.substr(idx + 1));
		auto where = get_sub_directory(path.substr(0, idx));
		if (!where)
			return "Directory not found!";
		else
			return where->delete_sub_directory(name);
	}
	rx_result ret;
	structure_lock();
	auto it = sub_directories_.find(path);
	if (it != sub_directories_.end())
	{

		if ((it->second->get_attributes()&namespace_item_delete_access) == namespace_item_delete_access)
		{
			if (!it->second->empty())
				ret.register_error("Directory not empty!");
			else
				sub_directories_.erase(it);
		}
		else
			ret.register_error(RX_ACCESS_DENIED);
	}
	else
	{
		ret.register_error("Directory does not exists!");
	}
	structure_unlock();
	return ret;
}

bool rx_platform_directory::empty () const
{
	bool ret;
	structure_lock();
	ret = sub_directories_.empty() && sub_items_.empty();
	structure_unlock();
	return ret;
}

rx_result rx_platform_directory::reserve_name (const string_type& name, string_type& path)
{
	rx_result ret;
	structure_lock();
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
	structure_unlock();
	if (ret)
		fill_path(path);
	return ret;
}

rx_result rx_platform_directory::cancel_reserve (const string_type& name)
{
	structure_lock();
	reserved_.erase(name);
	structure_unlock();
	return true;
}

meta_data_t rx_platform_directory::meta_info () const
{
	return meta_;
}

rx_result_with<rx_storage_ptr> rx_platform_directory::resolve_storage () const
{
	if (storage_)
		return storage_;
	else if (parent_)
		return parent_->resolve_storage();
	else
		return "Storage not defined";
}

template<class TImpl>
rx_result rx_platform_directory::add_item(TImpl who)
{
	return add_item(sys_internal::internal_ns::rx_item_implementation<TImpl>());
}
// Class rx_platform::ns::rx_names_cache 

rx_names_cache::rx_names_cache()
{
}



platform_item_ptr rx_names_cache::get_cached_item (const string_type& name) const
{
	const auto it = name_items_hash_.find(name);
	if (it != name_items_hash_.end())
	{
		return it->second;
	}
	else
	{
		return platform_item_ptr::null_ptr;
	}
}

rx_result rx_names_cache::insert_cached_item (const string_type& name, platform_item_ptr item)
{
	auto it = name_items_hash_.find(name);
	if (it != name_items_hash_.end())
	{
		return name + "is already registered name.";
	}
	else
	{
		name_items_hash_.emplace(name, item);
		return true;
	}
}

bool rx_names_cache::should_cache (const platform_item_ptr& item)
{
	// stupid algorithm here, should be checked!!!
	if (item->meta_info().get_attributes()& namespace_item_system_mask)
	{
		return true;
	}
	else
	{
		return false;
	}
}


// Class rx_platform::ns::rx_directory_resolver 


platform_item_ptr rx_directory_resolver::resolve_path (const string_type& path)
{
	if (path.empty() && directories_.empty())
		return platform_item_ptr::null_ptr;
	// path length is checked in preious condition!!!
	if (path[0] == RX_DIR_DELIMETER)
	{// global path
		return rx_gate::instance().get_root_directory()->get_sub_item(path);
	}
	for (auto& one : directories_)
	{
		if (!one.resolved)
		{
			one.dir = rx_gate::instance().get_root_directory()->get_sub_directory(one.path);
			one.resolved = true;
		}
		if (!one.dir)
		{// get directory
			return platform_item_ptr::null_ptr;
		}
		auto item = one.dir->get_sub_item(path);
		if (item)
		{
			return item;
		}
	}
	return platform_item_ptr::null_ptr;
}

void rx_directory_resolver::add_paths (std::initializer_list<string_type> paths)
{
	for (auto&& one : paths)
		directories_.emplace_back(resolver_data { std::forward<decltype(one)>(one), rx_directory_ptr::null_ptr, false });
}


} // namespace ns
} // namespace rx_platform

