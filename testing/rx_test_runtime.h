

/****************************************************************************
*
*  testing\rx_test_runtime.h
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


#ifndef rx_test_runtime_h
#define rx_test_runtime_h 1


#ifndef EXCLUDE_TEST_CODE

// rx_test
#include "testing/rx_test.h"



namespace testing {

namespace basic_tests {

namespace meta_test {





 class runtime_structure_test : public test_case  
 {
	 DECLARE_REFERENCE_PTR(runtime_structure_test)
	 DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing runtime capabilities.");

   public:
       runtime_structure_test();

       ~runtime_structure_test();


       bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


   protected:

   private:


 };


} // namespace meta_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE


#endif
