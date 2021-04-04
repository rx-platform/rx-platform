

/****************************************************************************
*
*  storage\rx_file_storage.cpp
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

#define RX_FILE_PREFIX "file"
#define RX_TYPE_JSON_FILE_EXTENSION "type.json"
#define RX_TYPE_BINARY_FILE_EXTENSION "type.bin"
#define RX_INSTANCE_JSON_FILE_EXTENSION "rt.json"
#define RX_INSTANCE_BINARY_FILE_EXTENSION "rt.bin"
#define RX_RUNTIME_JSON_FILE_EXTENSION "json"
#define RX_RUNTIME_BINARY_FILE_EXTENSION "bin"

#define RX_FILE_STORAGE_RUNTIME_DIR ".runtime"

// rx_file_internals
#include "storage/rx_file_internals.h"
// rx_file_storage
#include "storage/rx_file_storage.h"

#include "rx_configuration.h"
#include "rx_file_storage_version.h"
#include "system/hosting/rx_host.h"


namespace storage {

namespace files {
string_type get_file_storage_info()
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_STORAGE_NAME, RX_STORAGE_MAJOR_VERSION, RX_STORAGE_MINOR_VERSION, RX_STORAGE_BUILD_NUMBER);
	}
	return ret;
}

// Class storage::files::file_system_storage 

file_system_storage::file_system_storage()
{
}


file_system_storage::~file_system_storage()
{
}



string_type file_system_storage::get_storage_info ()
{
	// this function is moved there because of the template nature of file storage!!!
	return get_file_storage_info();
}

sys_handle_t file_system_storage::get_host_test_file (const string_type& path)
{
	string_type full_path = rx_combine_paths(root_ + "_test/", path);
	sys_handle_t file = rx_file(full_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	return file;
}

sys_handle_t file_system_storage::get_host_console_script_file (const string_type& path)
{
	string_type full_path = rx_combine_paths(root_ + "_script/", path);
	sys_handle_t file = rx_file(full_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	return file;
}

rx_result file_system_storage::init_storage (const string_type& storage_reference, hosting::rx_platform_host* host)
{
	auto result = recursive_create_directory(storage_reference);
	if(result)
		root_ = storage_reference;
	else
		result.register_error("Error opening storage directory at: "s + storage_reference);
	return result;
}

void file_system_storage::deinit_storage ()
{
}

rx_result file_system_storage::list_storage (std::vector<rx_storage_item_ptr>& items)
{
	auto result = recursive_list_storage("/", root_, items);
	if (result)
	{
		auto rt_path = rx_combine_paths(root_, RX_FILE_STORAGE_RUNTIME_DIR);
		if(rx_file_exsist(rt_path.c_str()))
			result = recursive_list_storage("/", rt_path, items);
	}
	return result;
}

rx_result file_system_storage::recursive_list_storage (const string_type& path, const string_type& file_path, std::vector<rx_storage_item_ptr>& items)
{
	string_type result_path;
	string_array file_names, directory_names;
	auto result = rx_list_files(file_path, "*", file_names, directory_names);
	if (result)
	{
		for (auto& one : directory_names)
		{
			if (one.empty() || one[0] == '.')
				continue;// skip specific folders
			result_path = rx_combine_paths(file_path, one);
			auto ret = recursive_list_storage(path + one + RX_DIR_DELIMETER, result_path, items);
			if (!ret)
				return ret;
		}
		for (auto& one : file_names)
		{
			meta::meta_data storage_meta;
			auto idx1 = one.rfind('.');
			auto idx = idx1;
			while (idx1 != string_type::npos && idx1 != 0)
			{
				idx = idx1;
				idx1 = one.rfind('.', idx1 - 1);
			}
			storage_meta.name = one.substr(0, idx);
			storage_meta.path = get_base_path() + path;
			result_path = rx_combine_paths(file_path, one);
			auto storage_item = get_storage_item_from_file_path(result_path, storage_meta);
			if(storage_item)
				items.emplace_back(std::move(storage_item));
		}
	}
	return result;
}

string_type file_system_storage::get_storage_reference ()
{
    return root_;
}

bool file_system_storage::is_valid_storage () const
{
	return !root_.empty();
}

rx_storage_item_ptr file_system_storage::get_storage_item_from_file_path (const string_type& path, const meta::meta_data& storage_meta)
{
	string_type ext = rx_get_extension(path);

	auto idx1 = path.find_last_of("\\/.");
	auto idx = idx1;
	while (idx1 != string_type::npos && idx1 != 0 && path[idx1] == '.')
	{
		idx = idx1;
		idx1 = path.find_last_of("\\/.", idx1 - 1);
	}
	ext = path.substr(idx+1);

	if (ext == RX_RUNTIME_BINARY_FILE_EXTENSION)
	{
		return std::make_unique<rx_file_item<rx_source_file, rx_binary_file> >(path, storage_meta, rx_storage_item_type::runtime);
	}
	else if (ext == RX_RUNTIME_JSON_FILE_EXTENSION)
	{
		return std::make_unique<rx_file_item<rx_source_file, rx_json_file> >(path, storage_meta, rx_storage_item_type::runtime);
	}
	else if (ext == RX_TYPE_BINARY_FILE_EXTENSION)
	{
		return std::make_unique<rx_file_item<rx_source_file, rx_binary_file> >(path, storage_meta, rx_storage_item_type::type);
	}
	else if (ext == RX_TYPE_JSON_FILE_EXTENSION)
	{
		return std::make_unique<rx_file_item<rx_source_file, rx_json_file> >(path, storage_meta, rx_storage_item_type::type);
	}
	else if (ext == RX_INSTANCE_BINARY_FILE_EXTENSION)
	{
		return std::make_unique<rx_file_item<rx_source_file, rx_binary_file> >(path, storage_meta, rx_storage_item_type::instance);
	}
	else if (ext == RX_INSTANCE_JSON_FILE_EXTENSION)
	{
		return std::make_unique<rx_file_item<rx_source_file, rx_json_file> >(path, storage_meta, rx_storage_item_type::instance);
	}
	else
	{
		return std::unique_ptr<storage_base::rx_storage_item>();
	}
}

rx_result file_system_storage::ensure_path_exsistence (const string_type& path)
{
	if (!rx_file_exsist(path.c_str()))
	{
		string_type sub_path;
		size_t idx = path.find_last_of("\\/");
		if (idx == string_type::npos)
			return "Something wrong with the path "s + path;
		sub_path = path.substr(0, idx);
		auto result = recursive_create_directory(sub_path);

		return result;
	}
	else
		return true;
}

rx_result file_system_storage::recursive_create_directory (const string_type& path)
{
	if (!rx_file_exsist(path.c_str()))
	{
		string_type sub_path;
		size_t idx = path.find_last_of("\\/");
		if (idx == string_type::npos)
			return "Something wrong with the path "s + path;
		sub_path = path.substr(0, idx);
		auto result = recursive_create_directory(sub_path);
		if (result)
		{// now create our directory
			auto result = rx_create_directory(path.c_str(), false);
			if (result == RX_OK)
			{
				return true;
			}
			else
			{
				return "Error creating directory "s + sub_path;
			}
		}
		else
			return result;
	}
	else
		return true;
}

rx_result_with<rx_storage_item_ptr> file_system_storage::get_item_storage (const meta::meta_data& data, rx_item_type type)
{
	string_type path = get_file_path(data, root_, get_base_path(), type);
	if (path.empty())
		return "Unable to get file path for the file storage item!";
	auto result = ensure_path_exsistence(path);
	if (result)
	{
		meta::meta_data storage_meta;
		rx_storage_item_ptr storage_item = get_storage_item_from_file_path(path, storage_meta);
		if (storage_item)
			return storage_item;
		else
			return "Unable to get storage item for file " + path;
	}
	else
	{
		result.register_error("Unable to create needed directories!");
		return result.errors();
	}
}

string_type file_system_storage::get_file_path (const meta::meta_data& data, const string_type& root, const string_type& base, rx_item_type type)
{
	if (data.path.empty())
		return "";

	locks::const_auto_lock_t<decltype(cache_lock_)> _(&cache_lock_);
	auto it = items_cache_.find(data.get_full_path());
	if (it == items_cache_.end())
	{// we don't have this one yet
		size_t idx;
		idx = data.path.find(base);
		if (idx != 0)
			return "";
		idx = base.size();
		idx = data.path.find(RX_DIR_DELIMETER, idx);
		string_type file_path;
		if (idx != string_type::npos)
			file_path = rx_combine_paths(root, data.path.substr(idx + 1));
		else
			file_path = root;
		string_type ext = rx_is_runtime(type) ?
			"." RX_INSTANCE_JSON_FILE_EXTENSION
			: "." RX_TYPE_JSON_FILE_EXTENSION;
		file_path = rx_combine_paths(file_path, data.name + ext);
		items_cache_.emplace(data.get_full_path(), file_path);
		return file_path;
	}
	else
		return it->second;
}

void file_system_storage::add_file_path (const meta::meta_data& data, const string_type& path)
{
	locks::auto_lock_t<decltype(cache_lock_)> _(&cache_lock_);
	items_cache_[data.get_full_path()] = path;
}

rx_result_with<rx_storage_item_ptr> file_system_storage::get_runtime_storage (const meta::meta_data& data, rx_item_type type)
{
	string_type path = get_runtime_file_path(data, root_, get_base_path(), type);
	if (path.empty())
		return "Unable to get runtime file path for the file storage item!";
	auto result = ensure_path_exsistence(path);
	if (result)
	{
		meta::meta_data storage_meta;
		rx_storage_item_ptr storage_item = get_storage_item_from_file_path(path, storage_meta);
		if (storage_item)
			return storage_item;
		else
			return "Unable to get storage item for file " + path;
	}
	else
	{
		result.register_error("Unable to create needed directories!");
		return result.errors();
	}
}

string_type file_system_storage::get_runtime_file_path (const meta::meta_data& data, const string_type& root, const string_type& base, rx_item_type type)
{
	if (data.path.empty())
		return "";

	locks::const_auto_lock_t<decltype(cache_lock_)> _(&cache_lock_);
	auto it = runtime_cache_.find(data.get_full_path());
	if (it == runtime_cache_.end())
	{// we don't have this one yet
		size_t idx;
		idx = data.path.find(base);
		if (idx != 0)
			return "";
		idx = base.size();
		idx = data.path.find(RX_DIR_DELIMETER, idx);
		string_type file_path;
		if (idx != string_type::npos)
			file_path = rx_combine_paths(root + RX_FILE_STORAGE_RUNTIME_DIR, data.path.substr(idx + 1));
		else
			file_path = root + RX_FILE_STORAGE_RUNTIME_DIR;
		file_path = rx_combine_paths(file_path, data.name + "." + RX_RUNTIME_JSON_FILE_EXTENSION);
		runtime_cache_.emplace(data.get_full_path(), file_path);
		return file_path;
	}
	else
		return it->second;
}


// Parameterized Class storage::files::rx_file_item 

template <class fileT, class streamT>
rx_file_item<fileT,streamT>::rx_file_item (const string_type& file_path, const meta::meta_data& storage_meta, rx_storage_item_type storage_type)
      : valid_(false),
        storage_meta_(storage_meta),
        file_path_(file_path)
	, rx_storage_item(storage_type)
{
	if (storage_meta_.path.size() > 2 && *storage_meta_.path.rbegin() == RX_DIR_DELIMETER)
		storage_meta_.path.pop_back();
}


template <class fileT, class streamT>
rx_file_item<fileT,streamT>::~rx_file_item()
{
}



template <class fileT, class streamT>
values::rx_value rx_file_item<fileT,streamT>::get_value () const
{
	values::rx_value temp;
	temp.set_time(get_created_time());
	temp.set_quality(valid_ ? RX_GOOD_QUALITY : RX_BAD_QUALITY);
	return temp;
}

template <class fileT, class streamT>
rx_time rx_file_item<fileT,streamT>::get_created_time () const
{
	return created_time_;
}

template <class fileT, class streamT>
size_t rx_file_item<fileT,streamT>::get_size () const
{
	return 0;
}

template <class fileT, class streamT>
rx_result rx_file_item<fileT,streamT>::delete_item ()
{
	int result = rx_file_delete(file_path_.c_str());
	return result == RX_OK ? true : false;
}

template <class fileT, class streamT>
string_type rx_file_item<fileT,streamT>::get_file_path () const
{
	return file_path_;
}

template <class fileT, class streamT>
const string_type& rx_file_item<fileT,streamT>::get_item_reference () const
{
	return file_path_;
}

template <class fileT, class streamT>
bool rx_file_item<fileT,streamT>::preprocess_meta_data (meta::meta_data& data)
{
	bool ret = false;
	if (!storage_meta_.name.empty() && storage_meta_.name!=data.name)
	{
		data.name = storage_meta_.name;
		ret = true;
	}
	if (!storage_meta_.path.empty() && storage_meta_.path != data.path)
	{
		data.path = storage_meta_.path;
		ret = true;
	}
	if (storage_meta_.modified_time > data.modified_time)
	{
		data.modified_time = storage_meta_.modified_time;
		// ret = true;// this is not worth the trouble
	}
	if (storage_meta_.created_time > data.created_time)
	{
		data.created_time = storage_meta_.created_time;
		// ret = true;// this is not worth the trouble
	}
	return ret;
}

template <class fileT, class streamT>
base_meta_reader& rx_file_item<fileT,streamT>::read_stream ()
{
	return item_data_.read_stream();
}

template <class fileT, class streamT>
base_meta_writer& rx_file_item<fileT,streamT>::write_stream ()
{
	return item_data_.write_stream();
}

template <class fileT, class streamT>
rx_result rx_file_item<fileT,streamT>::open_for_read ()
{
	fileT file;
	auto result = file.open(file_path_.c_str());
	if (result)
	{
		if constexpr (streamT::string_based)
		{
			string_type data;
			if (file.read_string(data))
			{
				result = item_data_.open_for_read(data, file_path_);
			}
			else
				result = "Error reading file "s + file_path_ + "!";
		}
		else
		{
			result = RX_NOT_IMPLEMENTED;
		}
	}
	else
	{
		result = "Unable to open file "s + file_path_ + "!";
	}
	return result;
}

template <class fileT, class streamT>
rx_result rx_file_item<fileT,streamT>::open_for_write ()
{
	auto result = item_data_.open_for_write(file_path_);
	return result;
}

template <class fileT, class streamT>
rx_result rx_file_item<fileT,streamT>::close_read ()
{
	return item_data_.close_read(file_path_);
}

template <class fileT, class streamT>
rx_result rx_file_item<fileT,streamT>::commit_write ()
{
	fileT file;
	auto result = file.open_write(file_path_.c_str());
	if (result)
	{
		if constexpr (streamT::string_based)
		{
			string_type data;
			result = item_data_.get_data(data);
			if (result)
			{
				if (!file.write_string(data))
				{
					result = "Error writing file "s + file_path_ + "!";
				}
			}
		}
		else
		{
			result = RX_NOT_IMPLEMENTED;
		}
	}
	else
	{
		result = "Unable to open file "s + file_path_ + " for write!";
	}
	return result;
}

template <class fileT, class streamT>
string_type rx_file_item<fileT,streamT>::get_item_path () const
{
	return storage_meta_.get_full_path();
}


// Class storage::files::file_system_storage_connection 

file_system_storage_connection::~file_system_storage_connection()
{
}



string_type file_system_storage_connection::get_storage_info () const
{
	return get_file_storage_info();
}

rx_result file_system_storage_connection::init_connection (const string_type& storage_reference, hosting::rx_platform_host* host)
{
	root_path_ = host->get_full_path(storage_reference);
	string_array files, directories;
	auto result = rx_list_files(root_path_, "*", files, directories);
	if (!result)
	{
		result.register_error("error reading storage directory at: "s + storage_reference);
	}
	return result;
}

rx_result_with<rx_storage_ptr> file_system_storage_connection::get_and_init_storage (const string_type& name, hosting::rx_platform_host* host)
{
	string_type sub_path = rx_combine_paths(root_path_, name);
	rx_storage_ptr result_ptr = rx_create_reference<file_system_storage>();
	auto result = result_ptr->init_storage(sub_path, host);
	if (result)
		return result_ptr;
	else
		return result.errors();
}

string_type file_system_storage_connection::get_storage_reference () const
{
	return root_path_;
}


// Class storage::files::file_system_storage_type 


string_type file_system_storage_type::get_storage_info ()
{
	return get_file_storage_info();
}

rx_storage_connection::smart_ptr file_system_storage_type::construct_storage_connection ()
{
	return rx_create_reference<file_system_storage_connection>();
}

string_type file_system_storage_type::get_reference_prefix () const
{
	return RX_FILE_PREFIX;
}


} // namespace files
} // namespace storage

