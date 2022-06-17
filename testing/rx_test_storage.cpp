

/****************************************************************************
*
*  testing\rx_test_storage.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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

#include "system/server/rx_server.h"
#include "system/server/rx_log.h"
#ifndef EXCLUDE_TEST_CODE

// rx_test_storage
#include "testing/rx_test_storage.h"

#include "system/hosting/rx_host.h"
#include "system/server/rx_server.h"
#include "model/rx_meta_internals.h"
#include "terminal/rx_term_table.h"


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
	if (!storage || storage.value()->get_storage_reference().empty())
	{// no test storage
		out << "Test storage not defined in platform\r\n";
		return true;
	}
	out << "Listing test storage " ANSI_COLOR_GREEN ANSI_COLOR_BOLD
		<< storage.value()->get_storage_info()
		<< ANSI_COLOR_RESET "\r\nStorage reference: " ANSI_COLOR_GREEN ANSI_COLOR_BOLD
		<< storage.value()->get_storage_reference()
		<< ANSI_COLOR_RESET "\r\n\r\n";
	auto result = storage.value()->list_storage(storage_items);
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
		for (size_t i = 0; i<storage_items.size(); i++)
		{
			rx_row_type row
			{
				rx_table_cell_struct("["  "aaaaaa"  "]", ANSI_RX_OBJECT_COLOR, ANSI_COLOR_RESET),
				rx_table_cell_struct("jjj", ANSI_RX_DIR_COLOR, ANSI_COLOR_RESET)
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


} // namespace storage_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE
