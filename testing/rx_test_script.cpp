

/****************************************************************************
*
*  testing\rx_test_script.cpp
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


#include "stdafx.h"


// rx_test_script
#include "testing/rx_test_script.h"



namespace testing {

namespace script_test {

namespace py_test {
void dump_python_information(std::ostream& out)
{

	out << "Python information\r\n==============================\r\n";
	out << "Version:\r\n" << Py_GetVersion() << "\r\n";
	out << "\r\n" << Py_GetCopyright() << "\r\n\r\n";
}

void do_python_test(std::ostream& out, const string_type& command)
{
	PyRun_SimpleString(command.c_str());
}

// Class testing::script_test::py_test::python_test 

python_test::python_test()
	: test_category("python")
{
	register_test_case(std::make_unique<dump_version_test>());
}


python_test::~python_test()
{
}



// Class testing::script_test::py_test::dump_version_test 

dump_version_test::dump_version_test()
	: test_case("version")
{
}


dump_version_test::~dump_version_test()
{
}



bool dump_version_test::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	dump_python_information(out);
	return true;
}


} // namespace py_test
} // namespace script_test
} // namespace testing

