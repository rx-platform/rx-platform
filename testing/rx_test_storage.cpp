

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
#include "storage/rx_file_storage.h"
#include "model/rx_meta_internals.h"


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
	: test_case("list-storage")
{
}


storage_list_test::~storage_list_test()
{
}



bool storage_list_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	std::vector<rx_storage_item_ptr> storage_items;
	auto storage = rx_gate::instance().get_host()->get_test_storage();
	if (!storage || storage->get_storage_reference().empty())
	{// no test storage
		out << "Test storage not defined in platform\r\n";
		return true;
	}
	out << "Listing test storage " ANSI_COLOR_GREEN ANSI_COLOR_BOLD
		<< storage->get_storage_info()
		<< ANSI_COLOR_RESET "\r\nStorage reference: " ANSI_COLOR_GREEN ANSI_COLOR_BOLD
		<< storage->get_storage_reference()
		<< ANSI_COLOR_RESET "\r\n\r\n";
	auto result = storage->list_storage(storage_items);
	if (result)
	{
		out << "Storage list succeeded:\r\n\r\n";
		rx_table_type table;
		rx_row_type header
		{
			rx_table_cell_struct("Name [Type]"),
			rx_table_cell_struct("Path")
		};
		table.emplace_back(std::move(header));
		for (auto& one : storage_items)
		{
			rx_row_type row
			{
				rx_table_cell_struct(one->get_name() + "[" + one->get_serialization_type() + "]", ANSI_RX_OBJECT_COLOR, ANSI_COLOR_RESET),
				rx_table_cell_struct(one->get_path(), ANSI_RX_DIR_COLOR, ANSI_COLOR_RESET)
			};
			table.emplace_back(std::move(row));
		}
		rx_dump_table(table, out, true, false);
		out << "\r\n";
		ctx->set_passed();
		return true;
	}
	out << "Error listing storage:";
	rx_dump_error_result(out, std::move(result));
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
		auto created = rx_create_reference<meta::object_types::object_type>();
		created->meta_data() = meta_data;
		result = created->deserialize_definition(stream, STREAMING_TYPE_TYPE);
		/*if (result)
		{
			auto create_result = result = model::platform_types_manager::instance().create_type_helper<rx_platform::meta::object_types::object_type>(
				"test_object_type", "ObjectBase", created, dir
				, ns::namespace_item_attributes::namespace_item_full_access
				, tl::type2type< rx_platform::meta::object_types::object_type>());
			if (create_result)
			{
				auto rx_type_item = create_result.value()->get_item_ptr();
				if (rx_type_item->generate_json(out, err))
				{
					id = test_type->meta_data().get_id();
					out << "Object type created\r\n";
				}
			}
			else
			{
				out << "Error creating derived object type\r\n";
				dump_error_result(out, result);
			}
		}*/
	}
	return result;
}


} // namespace storage_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE
