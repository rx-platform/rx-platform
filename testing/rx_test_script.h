

/****************************************************************************
*
*  testing\rx_test_script.h
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


#ifndef rx_test_script_h
#define rx_test_script_h 1


#ifndef EXCLUDE_TEST_CODE
#include "lib/rx_lib.h"

// rx_test
#include "testing/rx_test.h"



namespace testing {

namespace script_test {

namespace py_test {





class dump_version_test : public test_case  
{
	DECLARE_REFERENCE_PTR(dump_version_test)
	DECLARE_TEST_CODE_INFO( 1,0,0, "\
Dumping a python version emended and exiting.\
Done and checked can, be done everywhere.\
");

  public:
      dump_version_test();

      ~dump_version_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};







class python_test : public test_category  
{

  public:
      python_test();

      ~python_test();


  protected:

  private:


};


} // namespace py_test

namespace rxs {





class read_and_run_file : public test_case  
{
	DECLARE_REFERENCE_PTR(read_and_run_file)
	DECLARE_TEST_CODE_INFO(0,2,0, "\
loading a file->reading it line by line->executing.");

  public:
      read_and_run_file();

      ~read_and_run_file();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};







class rx_script_category : public test_category  
{

  public:
      rx_script_category();

      ~rx_script_category();


  protected:

  private:


};


} // namespace rxs
} // namespace script_test
} // namespace testing

#endif //EXCLUDE_TEST_CODE


#endif
