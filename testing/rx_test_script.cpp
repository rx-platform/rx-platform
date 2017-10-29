

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
	Py_Initialize();
	if (!Py_IsInitialized()) {
		out << "Unable to initialize Python interpreter!!!\r\n";
		return;
	}

	out << "Python information\r\n==============================\r\n";
	out << "Version:\r\n" << Py_GetVersion() << "\r\n";
	out << "\r\n" << Py_GetCopyright() << "\r\n\r\n";

	Py_Finalize();
}

void do_python_test(std::ostream& out, const string_type& command)
{
	PyRun_SimpleString(command.c_str());
}

// Class testing::script_test::py_test::python_basic_test 

python_basic_test::python_basic_test()
	: code_test("py-basic")
{
}


python_basic_test::~python_basic_test()
{
}



bool python_basic_test::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	out << "Testing Python Scripting\r\n==========================\r\n";
	string_type what;
	in >> what;
	if (what.empty())
		dump_python_information(out);
	else
		do_python_test(out, what);
	return true;
}


} // namespace py_test
} // namespace script_test
} // namespace testing

