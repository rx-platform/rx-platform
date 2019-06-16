

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

namespace runtime_test {





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







class runtime_test_category : public test_category  
{

  public:
      runtime_test_category();

      ~runtime_test_category();


  protected:

  private:


};






class runtime_transaction_test : public test_case  
{
	DECLARE_REFERENCE_PTR(runtime_transaction_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing transaction model.");

  public:
      runtime_transaction_test();

      ~runtime_transaction_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};






class runtime_connect_test : public test_case  
{
	DECLARE_REFERENCE_PTR(runtime_connect_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing subscription connect/disconnect.");

  public:
      runtime_connect_test();

      ~runtime_connect_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};


} // namespace runtime_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE


#endif
