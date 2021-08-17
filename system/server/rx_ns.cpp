

/****************************************************************************
*
*  system\server\rx_ns.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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

rx_platform_directory::rx_platform_directory (const string_type& name, namespace_item_attributes attrs, rx_storage_ptr storage)
      : storage_(storage)
{
	meta_.name = name;
	meta_.created_time = rx_time::now();
	meta_.modified_time = meta_.created_time;
	meta_.attributes = attrs;
}


rx_platform_directory::~rx_platform_directory()
{
#ifdef _DEBUG
	printf("Deleted directory: %s\r\n", meta_.name.c_str());
#endif
}



void rx_platform_directory::get_content (platform_directories_type& sub_directories, platform_items_type& sub_items, const string_type& pattern) const
{
	locks::const_auto_slim_lock dummy(&structure_lock_);
	bool is_pattern_empty = pattern.empty();
	const char* c_pattern = is_pattern_empty ? nullptr : pattern.c_str();
	for (const auto& one : sub_directories_)
	{
		if (!is_pattern_empty)
		{
			if (!match_pattern(one.first.c_str(), c_pattern, 0))
				continue;
		}
		sub_directories.emplace_back(one.second);
	}
	for (const auto& one : sub_items_)
	{
		if (!is_pattern_empty)
		{
			if (!match_pattern(one.first.c_str(), c_pattern, 0))
				continue;
		}
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
		ret = meta_.name;
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
	name = meta_.name;
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
	string_type path;
	parent->fill_path(path);
	locks::auto_lock_t<decltype(structure_lock_)> _(&structure_lock_);
	parent_ = parent;
	meta_.path = path;
	for (auto& one : sub_directories_)
		one.second->set_parent(smart_this());
}

namespace_item_attributes rx_platform_directory::get_attributes () const
{
	locks::const_auto_lock_t<decltype(structure_lock_)> _(&structure_lock_);
	return meta_.attributes;
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
	value.assign_static(get_name().c_str(), meta_.created_time);
}

void rx_platform_directory::fill_dir_code_info (std::ostream& info)
{
	string_type name = meta_.path;
	fill_code_info(info, name);
}

void rx_platform_directory::get_value (const string_type& name, rx_value& value)
{
}

rx_result rx_platform_directory::add_sub_directory (rx_directory_ptr who)
{
	rx_result ret;
	structure_lock();
	auto it = sub_directories_.find(who->meta_.name);
	if (it == sub_directories_.end())
	{
		sub_directories_.emplace(who->meta_.name, who);
		who->set_parent(smart_this());
	}
	else
	{
		ret.register_error("Directory " + who->meta_.name + " already exists");
	}
	structure_unlock();
	return ret;
}

rx_result rx_platform_directory::add_item (platform_item_ptr&& who)
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
	}
	else
	{
		ret.register_error("Item " + name + " already exists");
	}
	structure_unlock();
	if (ret && rx_names_cache::should_cache(who))
	{
		auto cache_result = rx_internal::internal_ns::platform_root::insert_cached_item(name, who);
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
	rx_directory_ptr new_dir = rx_create_reference<rx_internal::internal_ns::user_directory>(path);
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

rx_result rx_platform_directory::reserve_name (const string_type& name)
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

rx_namespace_item rx_platform_directory::get_sub_item (const string_type& path) const
{
	size_t idx = path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
	{// plain item		
		locks::const_auto_slim_lock dummy(&structure_lock_);
		auto it = sub_items_.find(path);
		if (it != sub_items_.end())
			return it->second;
		else
			return rx_namespace_item();
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
			return rx_namespace_item();
		}
	}
}

rx_result rx_platform_directory::add_item (const rx_namespace_item& what)
{
	auto name = what.get_meta().name;
	rx_result ret;
	structure_lock();
	auto it = sub_items_.find(name);
	if (it == sub_items_.end())
	{
		// check if name is reserved
		auto it = reserved_.find(name);
		if (it != reserved_.end())
			reserved_.erase(it);
		sub_items_.emplace(name, what);
	}
	else
	{
		ret.register_error("Item " + name + " already exists");
	}
	structure_unlock();
	if (ret && rx_names_cache::should_cache(what))
	{
		auto cache_result = rx_internal::internal_ns::platform_root::insert_cached_item(name, what);
		if (!cache_result)
		{
			std::ostringstream stream;
			stream << "Unable to register item "
				<< what.get_meta().get_full_path()
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

rx_result rx_platform_directory::move_directory (const string_type& source, const string_type& dest)
{
	if (source.empty() || dest.empty() || source == dest)
		return "Invalid directory name!";
	rx_directory_ptr source_dir_ptr;
	rx_directory_ptr source_parent_dir_ptr;
	rx_directory_ptr target_dir_ptr;
	string_type source_name;
	string_type target_name;
	auto idx = source.rfind(RX_DIR_DELIMETER);
	if (idx != string_type::npos)
	{// full path
		source_name = source.substr(idx + 1);
		auto where_from = get_sub_directory(source.substr(0, idx));
		if (!where_from)
			return "Directory not found!";
		else
		{
			source_parent_dir_ptr = where_from;
			source_dir_ptr = source_parent_dir_ptr->get_sub_directory(source_name);
			if(!source_dir_ptr)
				return "Directory not found!";
		}
	}
	else
	{// local name
		source_name = source;
		rx_result ret;
		structure_lock();
		auto it = sub_directories_.find(source);
		if (it != sub_directories_.end())
		{
			source_parent_dir_ptr = smart_this();
			source_dir_ptr = it->second;

			if ((it->second->get_attributes() & namespace_item_delete_access) == namespace_item_null)
				ret.register_error(RX_ACCESS_DENIED);
		}
		else
		{
			ret.register_error("Directory does not exists!");
		}
		structure_unlock();
		if(!ret)
			return ret;
	}
	idx = dest.rfind(RX_DIR_DELIMETER);
	if (idx != string_type::npos)
	{// plain name just
		target_name = dest.substr(idx + 1);
		auto where_to = get_sub_directory(dest.substr(0, idx));
		if (where_to)
			target_dir_ptr = where_to;
		else
			return "Directory already exists!";
	}
	else
	{// our path
		target_name = dest;
		rx_result ret;
		structure_lock();
		auto it = sub_directories_.find(dest);
		if (it != sub_directories_.end())
		{
			return "Directory already exists!";
		}
		else
		{
			target_dir_ptr = smart_this();

			if ((it->second->get_attributes() & namespace_item_write_access) == namespace_item_null)
				ret.register_error(RX_ACCESS_DENIED);
		}
		structure_unlock();
		if(!ret)
			return ret;
	}
	if (target_dir_ptr == smart_this())
	{
		rx_transaction_type transaction;
		rx_result ret;
		{
			// handle source
			{
				locks::auto_lock_t _(&source_parent_dir_ptr->structure_lock_);
				auto it = source_parent_dir_ptr->sub_directories_.find(source_name);
				if (it != sub_directories_.end())
				{
					source_dir_ptr = it->second;
					source_dir_ptr->parent_ = rx_directory_ptr::null_ptr;
					sub_directories_.erase(it);
					transaction.push([source_dir_ptr, source_parent_dir_ptr, source_name]() mutable
						{
							locks::auto_lock_t _(&source_parent_dir_ptr->structure_lock_);
							source_parent_dir_ptr->sub_directories_.emplace(source_name, source_dir_ptr);
							source_dir_ptr->parent_ = source_parent_dir_ptr;
						});
				}
				else
				{
					ret.register_error("Directory does not exists!");
				}
			}
			// now handle the target
			{
				locks::auto_lock_t _(&target_dir_ptr->structure_lock_);
				auto it = target_dir_ptr->sub_directories_.find(target_name);
				if (it == sub_directories_.end())
				{
					source_dir_ptr->parent_ = target_dir_ptr;
					target_dir_ptr->sub_directories_.emplace(target_name, source_dir_ptr);
					transaction.push([target_dir_ptr, target_name, source_dir_ptr]() mutable
						{
							locks::auto_lock_t _(&target_dir_ptr->structure_lock_);
							target_dir_ptr->sub_directories_.erase(target_name);
						});
				}
				else
				{
					ret.register_error("Directory already exists!");
				}
			}
		}
		if (ret)
		{
			auto storage_result = target_dir_ptr->resolve_storage();
			if (storage_result)
			{
				auto storage_ptr = storage_result.move_value();
				//storage_ptr->
			}
			else
			{
				ret.register_errors(storage_result.errors());
			}
		}
		if (ret)
			transaction.commit();
		return ret;
	}
	else
	{
		string_type source_path;
		source_dir_ptr->fill_path(source_path);
		return target_dir_ptr->move_directory(source_path, target_name);
	}
}

rx_result rx_platform_directory::copy_directory (const string_type& source, const string_type& dest)
{
	return RX_NOT_IMPLEMENTED;
}

void rx_platform_directory::register_suggestions (const string_type& line, suggestions_type& suggestions)
{
}

template<class TImpl>
rx_result rx_platform_directory::add_item(TImpl who)
{
	return add_item(rx_internal::internal_ns::rx_item_implementation<TImpl>());
}
// Class rx_platform::ns::rx_names_cache 

rx_names_cache::rx_names_cache()
{
}



rx_namespace_item rx_names_cache::get_cached_item (const string_type& name) const
{
	const auto it = name_items_hash_.find(name);
	if (it != name_items_hash_.end())
	{
		return it->second;
	}
	else
	{
		return std::move(rx_namespace_item());
	}
}

rx_result rx_names_cache::insert_cached_item (const string_type& name, const rx_namespace_item& item)
{
	auto it = name_items_hash_.find(name);
	if (it != name_items_hash_.end())
	{
		return name + "is already registered name.";
	}
	else
	{
		auto result = name_items_hash_.emplace(name, item);
		return true;
	}
}

bool rx_names_cache::should_cache (const platform_item_ptr& item)
{
	// stupid algorithm here, should be checked!!!
	if (item->meta_info().attributes& namespace_item_system_mask)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool rx_names_cache::should_cache (const rx_namespace_item& item)
{
	if (item.get_meta().attributes & namespace_item_system_mask)
	{
		return true;
	}
	else
	{
		return false;
	}
}

rx_result rx_names_cache::removed_cached_item (const string_type& name)
{
	return RX_NOT_IMPLEMENTED;
}

void rx_names_cache::clear ()
{
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



rx_namespace_item rx_directory_resolver::resolve_path (const string_type& path) const
{
	if (path.empty())
		return rx_namespace_item();
	// path length is checked in previous condition!!!
	if (path[0] == RX_DIR_DELIMETER)
	{// global path
		return rx_gate::instance().get_root_directory()->get_sub_item(path);
	}
	for (auto& one : directories_)
	{
		if (!one.resolved)
		{
			const_cast<resolver_data&>(one).dir = rx_gate::instance().get_root_directory()->get_sub_directory(one.path);
			if (one.dir)
			{
				const_cast<resolver_data&>(one).resolved = true;
			}
		}
		if (!one.dir)
		{// get directory
			return rx_namespace_item();
		}
		auto item = one.dir->get_sub_item(path);
		if (item)
		{
			return item;
		}
	}
	return rx_namespace_item();
}

void rx_directory_resolver::add_paths (std::initializer_list<string_type> paths)
{
	for (auto&& one : paths)
		directories_.emplace_back(resolver_data { std::forward<decltype(one)>(one), rx_directory_ptr::null_ptr, false });
}


// Class rx_platform::ns::rx_namespace_item 

rx_namespace_item::rx_namespace_item()
      : type_(rx_item_type::rx_invalid_type)
{
}

rx_namespace_item::rx_namespace_item (const platform_item_ptr& who)
      : type_(rx_item_type::rx_invalid_type)
{
	type_ = who->get_type_id();
	meta_ = who->meta_info();
	//executer_ = who->get_executer();
	value_.assign_static(meta_.version, meta_.modified_time);
}



rx_namespace_item::operator bool () const
{
	return type_ != rx_item_type::rx_invalid_type;
}

string_type rx_namespace_item::callculate_path (rx_directory_ptr dir) const
{
	string_type ret;
	if (dir)
	{
		dir->fill_path(ret);
	}
	return ret + meta_.name;
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


} // namespace ns
} // namespace rx_platform

