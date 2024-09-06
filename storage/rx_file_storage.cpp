

/****************************************************************************
*
*  storage\rx_file_storage.cpp
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

#define RX_FILE_PREFIX "file"
#define RX_TYPE_JSON_FILE_EXTENSION "type.json"
#define RX_TYPE_BINARY_FILE_EXTENSION "type.bin"
#define RX_INSTANCE_JSON_FILE_EXTENSION "rt.json"
#define RX_INSTANCE_BINARY_FILE_EXTENSION "rt.bin"
#define RX_RUNTIME_JSON_FILE_EXTENSION "json"
#define RX_RUNTIME_BINARY_FILE_EXTENSION "bin"

#define RX_ROLES_FILE_NAME "roles.json"

#define RX_FILE_STORAGE_RUNTIME_DIR ".runtime"

#include "lib/rx_ser_json.h"
#include "lib/rx_ser_bin.h"

// rx_file_internals
#include "storage/rx_file_internals.h"
// rx_file_storage
#include "storage/rx_file_storage.h"

#include "system/server/rx_file_helpers.h"
#include "rx_configuration.h"
#include "rx_file_storage_version.h"
#include "system/hosting/rx_host.h"
#include "sys_internal/rx_async_functions.h"


namespace storage {

namespace files {
string_type get_file_storage_info()
{
	static char ret[0x60] = { 0 };
	if (!ret[0])
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

rx_result file_system_storage::list_storage_roles (std::vector<rx_roles_storage_item_ptr>& items)
{
	string_type path = rx_combine_paths(root_, RX_ROLES_FILE_NAME);
	if (rx_file_exsist(path.c_str()))
	{
		items.push_back(std::make_unique<rx_roles_file>(path));
	}
	return true;
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
			meta_data storage_meta;
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
			rx_time_struct created, modified;
			if (RX_OK == rx_file_get_time_from_path(result_path.c_str(), &created, &modified))
			{
				storage_meta.created_time = created;
				storage_meta.modified_time = modified;
			}
			rx_storage_item_ptr storage_item = get_storage_item_from_file_path(result_path, storage_meta);
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

rx_storage_item_ptr file_system_storage::get_storage_item_from_file_path (const string_type& path, const meta_data& storage_meta)
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

rx_result_with<rx_storage_item_ptr> file_system_storage::get_item_storage (const meta_data& data, rx_item_type type)
{
	string_type path = get_file_path(data, root_, get_base_path(), type);
	if (path.empty())
		return "Unable to get file path for the file storage item!";
	auto result = ensure_path_exsistence(path);
	if (result)
	{
		meta_data storage_meta;
		rx_time_struct created, modified;
		if (RX_OK == rx_file_get_time_from_path(path.c_str(), &created, &modified))
		{
			storage_meta.created_time = created;
			storage_meta.modified_time = modified;
		}
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

string_type file_system_storage::get_file_path (const meta_data& data, const string_type& root, const string_type& base, rx_item_type type)
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
		string_type ext;
		if (data.attributes & namespace_item_attributes::namespace_item_system)
		{
			if (type != rx_directory)
			{
				ext = rx_is_runtime(type) ?
					"." RX_INSTANCE_JSON_FILE_EXTENSION
					: "." RX_TYPE_JSON_FILE_EXTENSION;
			}
		}
		else
		{
			if (type != rx_directory)
			{
				ext = rx_is_runtime(type) ?
					"." RX_INSTANCE_JSON_FILE_EXTENSION
					: "." RX_TYPE_JSON_FILE_EXTENSION;
			}
		}
		file_path = rx_combine_paths(file_path, data.name + ext);
		items_cache_.emplace(data.get_full_path(), file_path);
		return file_path;
	}
	else
		return it->second;
}

void file_system_storage::add_file_path (const meta_data& data, const string_type& path)
{
	locks::auto_lock_t<decltype(cache_lock_)> _(&cache_lock_);
	items_cache_[data.get_full_path()] = path;
}

rx_result_with<rx_storage_item_ptr> file_system_storage::get_runtime_storage (const meta_data& data, rx_item_type type)
{
	string_type path = get_runtime_file_path(data, root_, get_base_path(), type);
	if (path.empty())
		return "Unable to get runtime file path for the file storage item!";
	auto result = ensure_path_exsistence(path);
	if (result)
	{
		meta_data storage_meta;
		rx_time_struct created, modified;
		if (RX_OK == rx_file_get_time_from_path(path.c_str(), &created, &modified))
		{
			storage_meta.created_time = created;
			storage_meta.modified_time = modified;
		}
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

string_type file_system_storage::get_runtime_file_path (const meta_data& data, const string_type& root, const string_type& base, rx_item_type type)
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

void file_system_storage::preprocess_meta_data (meta_data& data)
{
	string_type base = get_base_path();
	size_t idx;
	string_type file_path;
	idx = data.path.find(base);
	if (idx == 0)
	{
		idx = base.size();
		idx = data.path.find(RX_DIR_DELIMETER, idx);
		if (idx != string_type::npos)
			file_path = rx_combine_paths(root_, data.path.substr(idx + 1));
		else
			file_path = root_;
	}
	if (!file_path.empty())
	{
		rx_time_struct created, modified;
		if (RX_OK == rx_file_get_time_from_path(file_path.c_str(), &created, &modified))
		{
			data.created_time = created;
			data.modified_time = modified;
		}
	}
}

runtime_transaction_id_t file_system_storage::get_new_unique_ids (size_t count)
{
	string_type path =	rx_combine_paths(root_, RX_FILE_STORAGE_RUNTIME_DIR "/_db_status.");
	runtime_transaction_id_t id = 0;
	runtime_transaction_id_t ret_id = 0;
	{
		rx_source_file file;
		auto result = file.open(path.c_str());
		if (result)
		{
			string_type data;
			if (!file.read_string(data))
			{
				result = rx_result::create_from_last_os_error("Error reading file "s + path + "!");
			}
			if (!data.empty())
			{
				auto res = std::from_chars(data.c_str(), data.c_str() + data.size(), id);
				if (res.ec != std::errc())
				{
					id = 0;
				}
			}
		}
	}

	if (id == 0)
		id = 1;

	ret_id = id;
	id += (runtime_transaction_id_t)count;

	char buffer[0x20];
	auto [ptr, ep] = std::to_chars(buffer, buffer + sizeof(buffer) / sizeof(buffer[0]), id);
	*ptr = '\0';


	rx_source_file file;
	auto result = file.open_write(path.c_str());
	if (result)
	{
		if (!file.write_string(buffer))
		{
			result = rx_result::create_from_last_os_error("Error writing file "s + path + "!");
		}
	}

	return ret_id;
}

void file_system_storage::set_next_unique_id (runtime_transaction_id_t id)
{
	string_type path = rx_combine_paths(root_, RX_FILE_STORAGE_RUNTIME_DIR "/_db_status.");
	char buffer[0x20];
	auto [ptr, ep] = std::to_chars(buffer, buffer + sizeof(buffer) / sizeof(buffer[0]), id);
	*ptr = '\0';


	rx_source_file file;
	auto result = file.open_write(path.c_str());
	if (result)
	{
		if (!file.write_string(buffer))
		{
			result = rx_result::create_from_last_os_error("Error writing file "s + path + "!");
		}
	}
}


// Parameterized Class storage::files::rx_file_item 

template <class fileT, class streamT, bool isStringBased>
rx_file_item<fileT,streamT,isStringBased>::rx_file_item (const string_type& file_path, const meta_data& storage_meta, rx_storage_item_type storage_type)
      : valid_(false),
        storage_meta_(storage_meta),
        file_path_(file_path)
	, rx_storage_item(storage_type)
{
	if (storage_meta_.path.size() > 2 && *storage_meta_.path.rbegin() == RX_DIR_DELIMETER)
		storage_meta_.path.pop_back();
}


template <class fileT, class streamT, bool isStringBased>
rx_file_item<fileT,streamT,isStringBased>::~rx_file_item()
{
}



template <class fileT, class streamT, bool isStringBased>
values::rx_value rx_file_item<fileT,streamT,isStringBased>::get_value () const
{
	values::rx_value temp;
	temp.set_time(storage_meta_.created_time);
	temp.set_quality(valid_ ? RX_GOOD_QUALITY : RX_BAD_QUALITY);
	return temp;
}

template <class fileT, class streamT, bool isStringBased>
rx_time rx_file_item<fileT,streamT,isStringBased>::get_created_time () const
{
	return storage_meta_.created_time;
}

template <class fileT, class streamT, bool isStringBased>
size_t rx_file_item<fileT,streamT,isStringBased>::get_size () const
{
	return 0;
}

template <class fileT, class streamT, bool isStringBased>
rx_result rx_file_item<fileT,streamT,isStringBased>::delete_item ()
{
	int result = rx_file_delete(file_path_.c_str());
	return result == RX_OK ? true : false;
}

template <class fileT, class streamT, bool isStringBased>
string_type rx_file_item<fileT,streamT,isStringBased>::get_file_path () const
{
	return file_path_;
}

template <class fileT, class streamT, bool isStringBased>
const string_type& rx_file_item<fileT,streamT,isStringBased>::get_item_reference () const
{
	return file_path_;
}

template <class fileT, class streamT, bool isStringBased>
bool rx_file_item<fileT,streamT,isStringBased>::preprocess_meta_data (meta_data& data)
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
	if (storage_meta_.modified_time != rx_time::null_time() 
		&& data.modified_time == rx_time::null_time())
	{
		data.modified_time = storage_meta_.modified_time;
		ret = true;
	}
	if (storage_meta_.created_time != rx_time::null_time()
		&& data.modified_time == rx_time::null_time())
	{
		data.created_time = storage_meta_.created_time;
		ret = true;
	}
	return ret;
}

template <class fileT, class streamT, bool isStringBased>
base_meta_reader& rx_file_item<fileT,streamT,isStringBased>::read_stream ()
{
	return item_data_.read_stream();
}

template <class fileT, class streamT, bool isStringBased>
base_meta_writer& rx_file_item<fileT,streamT,isStringBased>::write_stream ()
{
	return item_data_.write_stream();
}

template <class fileT, class streamT, bool isStringBased>
rx_result rx_file_item<fileT,streamT,isStringBased>::open_for_read ()
{
	fileT file;
	auto result = file.open(file_path_.c_str());
	if (result)
	{
		if constexpr (isStringBased)
		{
			string_type data;
			result = file.read_string(data);
			if (result)
			{
				result = item_data_.open_for_read(data, file_path_);
			}
			else
			{
				result.register_error("Error reading file "s + file_path_ + "!");
			}
		}
		else
		{
			byte_string data;
			result = file.read_data(data);
			if (result)
			{
				result = item_data_.open_for_read(data, file_path_);
			}
			else
			{
				result.register_error("Error reading file "s + file_path_ + "!");
			}
		}
	}
	else
	{
		result.register_error("Unable to open file "s + file_path_ + "!");
	}
	return result;
}

template <class fileT, class streamT, bool isStringBased>
rx_result rx_file_item<fileT,streamT,isStringBased>::open_for_write ()
{
	auto result = item_data_.open_for_write(file_path_);
	if (result)
	{/*
		fileT file;
		auto result = file.open(file_path_.c_str());
		if (result)
		{
			if constexpr (isStringBased)
			{
				string_type data;
				result = file.read_string(data);
				if (result)
				{
					item_cache_.value = std::move(data);
				}
				else
				{
					result.register_error("Error reading file "s + file_path_ + "!");
				}
			}
			else
			{
				byte_string data;
				result = file.read_data(data);
				if (result)
				{
					item_cache_.value = std::move(data);
				}
				else
				{
					result.register_error("Error reading file "s + file_path_ + "!");
				}
			}
		}
		else
		{
			result.register_error("Unable to open file "s + file_path_ + "!");
		}*/
		return result;
	}
	return result;
}

template <class fileT, class streamT, bool isStringBased>
rx_result rx_file_item<fileT,streamT,isStringBased>::close_read ()
{
	return item_data_.close_read(file_path_);
}

template <class fileT, class streamT, bool isStringBased>
rx_result rx_file_item<fileT,streamT,isStringBased>::commit_write_sync ()
{

	if constexpr (streamT::string_based)
	{
		string_type data;
		auto result = item_data_.get_data(data);
		if (result)
		{
			byte_string buffer(data.size());
			memcpy(&buffer[0], &data[0], data.size());
			result = internal_save(std::move(buffer), file_path_);
		}
		return result;
	}
	else
	{
		byte_string data;
		auto result = item_data_.get_data(data);
		if (result)
		{
			result = internal_save(std::move(data), file_path_);
		}
		return result;
	}

}

template <class fileT, class streamT, bool isStringBased>
void rx_file_item<fileT,streamT,isStringBased>::commit_write (storage_callback_t callback, runtime_transaction_id_t trans_id)
{
}

template <class fileT, class streamT, bool isStringBased>
string_type rx_file_item<fileT,streamT,isStringBased>::get_item_path () const
{
	return storage_meta_.get_full_path();
}

template <class fileT, class streamT, bool isStringBased>
rx_result rx_file_item<fileT,streamT,isStringBased>::save (runtime_transaction_id_t trans_id, byte_string data, storage_callback_t callback)
{
	auto exec = rx_thread_context();
	rx_post_function_to(RX_DOMAIN_META, rx_reference_ptr(), [exec, trans_id](byte_string data, storage_callback_t callback, string_type path)
		{
			rx_result result = internal_save(std::move(data), path);

			rx_post_function_to(exec, rx_reference_ptr(), [](runtime_transaction_id_t trans_id, storage_callback_t callback, rx_result result)
				{
					callback(trans_id, std::move(result));

				}, trans_id, std::move(callback), std::move(result));

		}, std::move(data), std::move(callback), file_path_);
	return true;
}

template <class fileT, class streamT, bool isStringBased>
rx_result rx_file_item<fileT,streamT,isStringBased>::internal_save (byte_string data, const string_type& path)
{
	rx_result result;
	{
		fileT file;
		result = file.open_write((path + "~").c_str());
		if (result)
		{
			if (!file.write_data(data))
			{
				result = rx_result::create_from_last_os_error("Error writing file "s + path + "!");
			}
		}
	}

	if (result)
	{// now do the move!!!!
		if (!rx_file_rename((path + "~").c_str(), path.c_str()))
		{
			result = rx_result::create_from_last_os_error("Error renaming file "s + path + "!");
			if (!rx_file_delete((path + "~").c_str()))
			{
				result = rx_result::create_from_last_os_error("Error deleting file "s + path + "~" + "!");
			}
		}
	}
	else
	{
		result = rx_result::create_from_last_os_error("Unable to open file "s + path + " for write!");
	}
	return result;
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
	file_system_storage::smart_ptr result_ptr = rx_create_reference<file_system_storage>();
	auto result = result_ptr->init_storage(sub_path, host);
	if (result)
		return rx_storage_ptr(result_ptr);
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


// Class storage::files::rx_roles_file 

rx_roles_file::rx_roles_file (const string_type& file_path)
      : file_path_(file_path)
{
}


rx_roles_file::~rx_roles_file()
{
}



rx_result rx_roles_file::delete_item ()
{
	int result = rx_file_delete(file_path_.c_str());
	return result == RX_OK ? true : false;
}

base_meta_reader& rx_roles_file::read_stream ()
{
	return item_data_.read_stream();
}

base_meta_writer& rx_roles_file::write_stream ()
{
	return item_data_.write_stream();
}

rx_result rx_roles_file::open_for_read ()
{
	rx_source_file file;
	auto result = file.open(file_path_.c_str());
	if (result)
	{
		string_type data;
		result = file.read_string(data);
		if (result)
		{
			result = item_data_.open_for_read(data, file_path_);
		}
		else
		{
			result.register_error("Error reading file "s + file_path_ + "!");
		}
	}
	else
	{
		result.register_error("Unable to open file "s + file_path_ + "!");
	}
	return result;
}

rx_result rx_roles_file::open_for_write ()
{
	auto result = item_data_.open_for_write(file_path_);
	return result;
}

rx_result rx_roles_file::close_read ()
{
	return item_data_.close_read(file_path_);
}

rx_result rx_roles_file::commit_write ()
{
	rx_source_file file;
	auto result = file.open_write(file_path_.c_str());
	if (result)
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
		result = "Unable to open file "s + file_path_ + " for write!";
	}
	return result;
}


} // namespace files
} // namespace storage

