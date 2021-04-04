

/****************************************************************************
*
*  testing\rx_test_basic.h
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


#ifndef rx_test_basic_h
#define rx_test_basic_h 1


#ifndef EXCLUDE_TEST_CODE

// rx_test
#include "testing/rx_test.h"



namespace testing {

namespace basic_tests {

namespace lib_test {





class library_test_category : public test_category  
{

  public:
      library_test_category();

      ~library_test_category();


  protected:

  private:


};






class platform_callback_test : public test_case  
{
	DECLARE_REFERENCE_PTR(platform_callback_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing of callback capabilities.");

  public:
      platform_callback_test();

      ~platform_callback_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};






class values_test : public test_case  
{
	DECLARE_REFERENCE_PTR(values_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing for platform values.");

  public:
      values_test();

      ~values_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:

      bool test_serialization (const string_type& name, std::vector<rx::values::rx_simple_value>& simples, std::vector<rx::values::rx_timed_value>& timed, std::vector<rx::values::rx_value>& fulls, base_meta_writer& writer, std::ostream& out);

      bool test_deserialization (const string_type& name, std::vector<rx::values::rx_simple_value>& simples, std::vector<rx::values::rx_timed_value>& timed, std::vector<rx::values::rx_value>& fulls, base_meta_reader& reader, std::ostream& out);



};






class external_interfaces_test : public test_case  
{
	DECLARE_REFERENCE_PTR(external_interfaces_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing for Ethernet and RS232/485 port resources on this staffroom.");

  public:
      external_interfaces_test();

      ~external_interfaces_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};


} // namespace lib_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE


#endif
