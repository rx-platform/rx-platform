

/****************************************************************************
*
*  testing\rx_test_basic.h
*
*  Copyright (c) 2017 Dusan Ciric
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


#ifndef rx_test_basic_h
#define rx_test_basic_h 1



// rx_test
#include "testing/rx_test.h"



namespace testing {

namespace basic_tests {

namespace function_test {





class platform_callback_test : public test_case  
{
	DECLARE_REFERENCE_PTR(platform_callback_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing of callback capabilities.");

  public:
      platform_callback_test();

      virtual ~platform_callback_test();


      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};







class function_test_category : public test_category  
{

  public:
      function_test_category();

      virtual ~function_test_category();


  protected:

  private:


};


} // namespace function_test
} // namespace basic_tests
} // namespace testing



#endif
