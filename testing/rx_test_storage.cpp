

/****************************************************************************
*
*  testing\rx_test_storage.cpp
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

#ifndef EXCLUDE_TEST_CODE

// rx_test_storage
#include "testing/rx_test_storage.h"

#include "system/hosting/rx_host.h"
#include "system/server/rx_server.h"
#include "host/rx_file_storage.h"


namespace testing {

namespace basic_tests {

namespace storage_test {

// Class testing::basic_tests::storage_test::storage_test_category 

storage_test_category::storage_test_category()
	: test_category("storage")
{
	register_test_case(rx_create_reference<storage_list_test>());
}


storage_test_category::~storage_test_category()
{
}



// Class testing::basic_tests::storage_test::storage_list_test 

storage_list_test::storage_list_test()
	: test_case("storage-list")
{
}


storage_list_test::~storage_list_test()
{
}



bool storage_list_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	std::vector<hosting::rx_storage_item_ptr> storage_items;
	auto storage = rx_gate::instance().get_host()->get_test_storage();
	if (!storage || storage->get_storage_reference().empty())
	{// no test storage
		out << "Test storage not defined in platform\r\n";
		return true;
	}
	auto result = storage->list_storage(storage_items);

	for (auto& one : storage_items)
	{
		rx_directory_ptr current_dir = rx_platform::rx_gate::instance().get_root_directory();
		rx_directory_ptr temp_dir;
		string_type path = one->get_path();
		string_type temp_path;
		size_t last = 0;
		size_t next = 0;
		while ((next = path.find(RX_DIR_DELIMETER, last)) != string_type::npos)
		{
			temp_path = path.substr(last, next - last);
			temp_dir = current_dir->get_sub_directory(temp_path);
			if (temp_dir)
			{
				current_dir = temp_dir;
			}
			else
			{
				current_dir = current_dir->add_sub_directory(temp_path);
			}
			last = next + 1;
		}
		temp_path = path.substr(last);
		temp_dir = current_dir->get_sub_directory(temp_path);
		if (temp_dir)
		{
			current_dir = temp_dir;
		}
		else
		{
			current_dir = current_dir->add_sub_directory(temp_path);
		}
		rx_result result = one->open_for_read();
		if (result)
		{
			auto& stream = one->read_stream();
			int type = 0;
			result = stream.read_header(type);
			if (result)
			{
				switch (type)
				{
				case STREAMING_TYPE_TYPE:
					result = read_type_from_storage(stream, current_dir);
					break;
				case STREAMING_TYPE_OBJECT:
					result = read_object_from_storage(stream, current_dir);
					break;
				default:
					out << "Invalid serialization type!";
				}
			}
			one->close();
		}
	}
	ctx->set_failed();
	return true;
}

rx_result storage_list_test::read_object_from_storage (base_meta_reader& stream, rx_directory_ptr dir)
{
	meta::checkable_data meta_data;
	string_type target_type;
	auto result = stream.start_object("Meta");
	if (!result)
		return result;
	result = meta_data.deserialize_checkable_definition(stream, STREAMING_TYPE_OBJECT, target_type);
	if (!result)
		return result;
	result = stream.end_object();
	return result;
}

rx_result storage_list_test::read_type_from_storage (base_meta_reader& stream, rx_directory_ptr dir)
{
	meta::checkable_data meta_data;
	string_type target_type;
	auto result = meta_data.deserialize_checkable_definition(stream, STREAMING_TYPE_TYPE, target_type);
	if (!result)
		return result;
	if (target_type == "object_type")
	{
		auto dummy = rx_create_reference<meta::object_types::object_type>();
		dummy->meta_data() = meta_data;
		result = dummy->deserialize_definition(stream, STREAMING_TYPE_TYPE);
	}
	return result;
}


} // namespace storage_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE
