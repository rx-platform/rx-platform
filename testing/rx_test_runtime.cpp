

/****************************************************************************
*
*  testing\rx_test_runtime.cpp
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

#ifndef EXCLUDE_TEST_CODE

// rx_test_runtime
#include "testing/rx_test_runtime.h"

#include "model/rx_meta_api.h"
#include "sys_internal/rx_internal_ns.h"
#include "lib/rx_rt_data.h"
#include <valarray>
#include "lib/rx_const_size_vector.h"
#include "system/server/rx_server.h"
#define ANSI_RX_OBJECT_SIZE ANSI_COLOR_GREEN ANSI_COLOR_BOLD
#define ANSI_RX_OBJECT_NAME ANSI_COLOR_YELLOW ANSI_COLOR_BOLD


namespace testing {

namespace basic_tests {

namespace meta_test {

 // Class testing::basic_tests::meta_test::runtime_structure_test 

 runtime_structure_test::runtime_structure_test()
	 : test_case("runtime")
 {
 }


 runtime_structure_test::~runtime_structure_test()
 {
 }



 bool runtime_structure_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 out << "Calculating runtime sizes:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";

	 out << "object_runtime:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::objects::object_runtime) << ANSI_COLOR_RESET "\r\n";
	 out << "variable_runtime:" ANSI_RX_OBJECT_SIZE  << sizeof(runtime::blocks::variable_runtime) << ANSI_COLOR_RESET << "\r\n";
	
	 out << "\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
	 out << "empty_item_data:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::empty_item_data) << ANSI_COLOR_RESET "\r\n";
	 out << "basic_item_data:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::basic_item_data) << ANSI_COLOR_RESET "\r\n";
	 out << "full_item_data:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::full_item_data) << ANSI_COLOR_RESET "\r\n";

	 std::vector< runtime::structure::variable_data> stuff;
	 std::valarray<runtime::structure::variable_data> testing;

	 rx::const_size_vector<size_t> perica({ 56, 38, 38 });

	 for (auto rr : perica)
		 out << "Element:" << rr << "\r\n";

	 out << "const_size_vector:" ANSI_RX_OBJECT_SIZE << sizeof(rx::const_size_vector<runtime::structure::variable_data>) << ANSI_COLOR_RESET "\r\n";
	 out << "array:" ANSI_RX_OBJECT_SIZE << sizeof(std::array<runtime::structure::variable_data, 0>) << ANSI_COLOR_RESET "\r\n";
	 out << "empty:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::empty<runtime::structure::variable_data>) << ANSI_COLOR_RESET "\r\n";
	 out << "mixed:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::mixed_item_data) << ANSI_COLOR_RESET "\r\n";

	 out << "bitset<8>:" ANSI_RX_OBJECT_SIZE << sizeof(std::bitset<8>) << ANSI_COLOR_RESET "\r\n";
	 out << "bitset<32>:" ANSI_RX_OBJECT_SIZE << sizeof(std::bitset<32>) << ANSI_COLOR_RESET "\r\n";
	 out << "bitset<64>:" ANSI_RX_OBJECT_SIZE << sizeof(std::bitset<64>) << ANSI_COLOR_RESET "\r\n";

	 ctx->set_failed();
	 return true;
 }


} // namespace meta_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE
