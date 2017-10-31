

/****************************************************************************
*
*  testing\rx_test_script.h
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


#ifndef rx_test_script_h
#define rx_test_script_h 1



// rx_test
#include "testing/rx_test.h"



namespace testing {

namespace script_test {

namespace py_test {





class dump_version_test : public test_case  
{

  public:
      dump_version_test();

      virtual ~dump_version_test();


      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};







class python_test : public test_category  
{

  public:
      python_test();

      virtual ~python_test();


  protected:

  private:


};


} // namespace py_test
} // namespace script_test
} // namespace testing



#endif
