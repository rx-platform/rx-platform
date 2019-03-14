

/****************************************************************************
*
*  host\rx_file_storage.cpp
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/


#include "pch.h"


// rx_file_storage
#include "host/rx_file_storage.h"

#include "rx_configuration.h"
#include "rx_file_storage_version.h"
#include "testing/rx_test_storage.h"


namespace host {

namespace files {

// Class host::files::file_system_storage

file_system_storage::file_system_storage()
{
#ifndef EXCLUDE_TEST_CODE
	testing::testing_enviroment::instance().register_test_category(std::make_unique<testing::basic_tests::storage_test::storage_test_category>());
#endif //EXCLUDE_TEST_CODE
}


file_system_storage::~file_system_storage()
{
}



void file_system_storage::get_storage_info (string_type& info)
{
	static string_type ret;
	if (ret.empty())
	{
		ASSIGN_MODULE_VERSION(ret, RX_STORAGE_NAME, RX_STORAGE_MAJOR_VERSION, RX_STORAGE_MINOR_VERSION, RX_STORAGE_BUILD_NUMBER);
	}
	info = ret;
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

const string_type& file_system_storage::get_license ()
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

rx_result file_system_storage::init_storage (const string_type& storage_reference)
{
	root_ = storage_reference;
	return true;
}

rx_result file_system_storage::deinit_storage ()
{
	return true;
}

rx_result file_system_storage::list_storage (std::vector<rx_platform::hosting::rx_storage_item_ptr>& items)
{
	return recursive_list_storage("/", root_, items);
}

rx_result file_system_storage::recursive_list_storage (const string_type& path, const string_type& file_path, std::vector<rx_platform::hosting::rx_storage_item_ptr>& items)
{
	string_type result_path;
	string_array file_names, directory_names;
	rx_list_files(file_path, "*", file_names, directory_names);
	for (auto& one : directory_names)
	{
		if (one == ".git")
			continue;// skip git's folder
		result_path = rx_combine_paths(file_path, one);
		auto ret = recursive_list_storage(path + one + RX_DIR_DELIMETER, result_path, items);
	}
	for (auto& one : file_names)
	{
		string_type ext = rx_get_extension(one);
		if (ext == RX_JSON_FILE_EXTESION)
		{
			result_path = rx_combine_paths(file_path, one);
			items.emplace_back(new rx_json_file(path, result_path));
		}
		else if (ext == RX_BINARY_FILE_EXTESION)
		{
			result_path = rx_combine_paths(file_path, one);
			items.emplace_back(new rx_binary_file(path + one, result_path));
		}
	}
	return true;
}

void file_system_storage::get_storage_reference (string_type& ref)
{
	ref = root_;
}

// Class host::files::rx_file_item

rx_file_item::rx_file_item (const string_type& path, const string_type& file_path)
      : valid_(false),
        file_path_(file_path)
	, rx_storage_item(path)
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


// Class host::files::rx_json_file

rx_json_file::rx_json_file (const string_type& path, const string_type& file_path)
	: rx_file_item(path, file_path)
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
				return "Error parsing Json file "s + file_path_ + "!";
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
		writer_.release();
		return ret;
	}
	if (reader_)
		reader_.release();
	return true;
}


// Class host::files::rx_binary_file

rx_binary_file::rx_binary_file (const string_type& path, const string_type& file_path)
	: rx_file_item(path, file_path)
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
} // namespace host

