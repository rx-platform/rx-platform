

/****************************************************************************
*
*  storage\rx_file_storage.cpp
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


// rx_file_storage
#include "storage/rx_file_storage.h"

#include "rx_configuration.h"
#include "rx_file_storage_version.h"


namespace storage {

namespace files {

// Parameterized Class storage::files::file_system_storage 

template <class policyT>
file_system_storage<policyT>::file_system_storage()
{
}


template <class policyT>
file_system_storage<policyT>::~file_system_storage()
{
}



template <class policyT>
string_type file_system_storage<policyT>::get_storage_info ()
{
	// this function is moved there because of the template nature of file storage!!!
	return rx_file_item::get_file_storage_info();
}

template <class policyT>
sys_handle_t file_system_storage<policyT>::get_host_test_file (const string_type& path)
{
	string_type full_path = rx_combine_paths(root_ + "_test/", path);
	sys_handle_t file = rx_file(full_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	return file;
}

template <class policyT>
sys_handle_t file_system_storage<policyT>::get_host_console_script_file (const string_type& path)
{
	string_type full_path = rx_combine_paths(root_ + "_script/", path);
	sys_handle_t file = rx_file(full_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
	return file;
}

template <class policyT>
const string_type& file_system_storage<policyT>::get_license ()
{
	static string_type lic_cached;
	static bool tried_get = false;
	if (!tried_get)
	{
		tried_get = true;
		string_type lic_path = rx_combine_paths(root_, "rx-system-storage/LICENSE");
		sys_handle_t file = rx_file(lic_path.c_str(), RX_FILE_OPEN_READ, RX_FILE_OPEN_EXISTING);
		if (file)
		{
			uint64_t size = 0;
			if (RX_OK == rx_file_get_size(file, &size) && size > 0)
			{
				lic_cached.assign(size, ' ');
				if (RX_OK != rx_file_read(file, &lic_cached[0], (uint32_t)size, nullptr))
				{
					lic_cached.clear();
				}
			}
			rx_file_close(file);
		}
	}
	return lic_cached;
}

template <class policyT>
rx_result file_system_storage<policyT>::init_storage (const string_type& storage_reference)
{
	root_ = storage_reference;
	string_array files, directories;
	auto result = rx_list_files(root_, "*", files, directories);
	if(!result)
	{
		result.register_error("error reading storage directory at: "s + storage_reference);
	}
	return result;
}

template <class policyT>
rx_result file_system_storage<policyT>::deinit_storage ()
{
	return true;
}

template <class policyT>
rx_result file_system_storage<policyT>::list_storage (std::vector<rx_storage_item_ptr>& items)
{
	return recursive_list_storage("/", root_, items);
}

template <class policyT>
rx_result file_system_storage<policyT>::recursive_list_storage (const string_type& path, const string_type& file_path, std::vector<rx_storage_item_ptr>& items)
{
	string_type result_path;
	string_array file_names, directory_names;
	auto result = rx_list_files(file_path, "*", file_names, directory_names);
	if (result)
	{
		for (auto& one : directory_names)
		{
			if (one == ".git")
				continue;// skip git's folder
			result_path = rx_combine_paths(file_path, one);
			auto ret = recursive_list_storage(path + one + RX_DIR_DELIMETER, result_path, items);
			if (!ret)
				return ret;
		}
		for (auto& one : file_names)
		{
			result_path = rx_combine_paths(file_path, one);
			auto storage_item = get_storage_item_from_file_path(result_path);
			if(storage_item)
				items.emplace_back(std::move(storage_item));
		}
	}
	return result;
}

template <class policyT>
string_type file_system_storage<policyT>::get_storage_reference ()
{
    return root_;
}

template <class policyT>
bool file_system_storage<policyT>::is_valid_storage () const
{
	return !root_.empty();
}

template <class policyT>
std::unique_ptr<rx_file_item> file_system_storage<policyT>::get_storage_item_from_file_path (const string_type& path)
{
	string_type ext = rx_get_extension(path);

	if (ext == RX_JSON_FILE_EXTESION)
	{
		return std::make_unique<rx_json_file>(path);

	}
	else if (ext == RX_BINARY_FILE_EXTESION)
	{
		return std::make_unique<rx_binary_file>(path);
	}
	else
	{
		return std::unique_ptr<rx_file_item>();
	}
}

template <class policyT>
rx_result file_system_storage<policyT>::ensure_path_exsistence (const string_type& path)
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

template <class policyT>
rx_result file_system_storage<policyT>::recursive_create_directory (const string_type& path)
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

template <class policyT>
rx_result_with<rx_storage_item_ptr> file_system_storage<policyT>::get_item_storage (const meta::meta_data& data)
{
	string_type path = cache_.get_file_path(data, root_, get_base_path());
	if (path.empty())
		return "Unable to get file path for the file storage item!";
	auto result = ensure_path_exsistence(path);
	if (result)
	{
		rx_storage_item_ptr storage_item = get_storage_item_from_file_path(path);
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

template file_system_storage<storage::storage_policy::file_path_addresing_policy>::file_system_storage();
// Class storage::files::rx_file_item 

rx_file_item::rx_file_item (const string_type& serialization_type, const string_type& file_path)
      : valid_(false),
        file_path_(file_path)
	, rx_storage_item(serialization_type)
{
}


rx_file_item::~rx_file_item()
{
}



values::rx_value rx_file_item::get_value () const
{
	values::rx_value temp;
	temp.set_time(get_created_time());
	temp.set_quality(valid_ ? RX_GOOD_QUALITY : RX_BAD_QUALITY);
	return temp;
}

rx_time rx_file_item::get_created_time () const
{
	return created_time_;
}

size_t rx_file_item::get_size () const
{
	return 0;
}

rx_result rx_file_item::delete_item ()
{
	int result = rx_file_delete(file_path_.c_str());
	return result == RX_OK ? true : false;
}

string_type rx_file_item::get_file_path () const
{
	return file_path_;
}

string_type rx_file_item::get_file_storage_info ()
{
	// this function is here because of the template nature of file storage!!!
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_STORAGE_NAME, RX_STORAGE_MAJOR_VERSION, RX_STORAGE_MINOR_VERSION, RX_STORAGE_BUILD_NUMBER);
	}
	return ret;
}

const string_type& rx_file_item::get_item_reference () const
{
	return file_path_;
}


// Class storage::files::rx_json_file 

rx_json_file::rx_json_file (const string_type& file_path)
	: rx_file_item(RX_JSON_SERIALIZATION_TYPE, file_path)
{
}


rx_json_file::~rx_json_file()
{
}



base_meta_reader& rx_json_file::read_stream ()
{
	return *reader_;
}

base_meta_writer& rx_json_file::write_stream ()
{
	return *writer_;
}

rx_result rx_json_file::open_for_read ()
{
	if (reader_)
		return "File storage "s + file_path_ + " already opened for reading";
	if (writer_)
		return "File storage "s + file_path_ + " already opened for writing";

	rx_source_file file;

	if (file.open(file_path_.c_str()))
	{
		std::string data;
		if (file.read_string(data))
		{
			reader_ = std::make_unique<rx_platform::serialization::json_reader>();
			if (reader_->parse_data(data))
				return true;
			else
			{
				rx_result ret = reader_->get_errors();
				ret.register_error("Error parsing Json file "s + file_path_ + "!");
				return ret;
			}
		}
		else
			return "Error reading file "s + file_path_ + "!";
	}
	else
		return "Unable to open file "s + file_path_ + "!";
}

rx_result rx_json_file::open_for_write ()
{
	if (reader_)
		return "File storage "s + file_path_ + " already opened for reading";
	if (writer_)
		return "File storage "s + file_path_ + " already opened for writing";

	writer_ = std::make_unique<rx_platform::serialization::json_writer>();
	return true;
}

rx_result rx_json_file::close ()
{
	if (writer_)
	{
		rx_result ret = false;
		rx_source_file file;
		if (file.open_write(file_path_.c_str()))
		{
			std::string buff;
			if (writer_->get_string(buff, true))
			{
				if (file.write_string(buff))
					ret = true;
				else
					ret = "Error writing Json to file "s + file_path_ + "!";
			}
		}
		else
			ret = "Error opening Json file "s + file_path_ + "!";
		writer_.release();
		return ret;
	}
	if (reader_)
		reader_.release();
	return true;
}


// Class storage::files::rx_binary_file 

rx_binary_file::rx_binary_file (const string_type& file_path)
	: rx_file_item(RX_BINARY_SERIALIZATION_TYPE, file_path)
{
}


rx_binary_file::~rx_binary_file()
{
}



base_meta_reader& rx_binary_file::read_stream ()
{
	return *reader_;
}

base_meta_writer& rx_binary_file::write_stream ()
{
	return *writer_;
}

rx_result rx_binary_file::open_for_read ()
{
	return "Not implemented!";
}

rx_result rx_binary_file::open_for_write ()
{
	return "Not implemented!";
}

rx_result rx_binary_file::close ()
{
	return "Not implemented!";
	if (writer_)
		writer_.release();
	if (reader_)
		reader_.release();
}


} // namespace files
} // namespace storage

