

/****************************************************************************
*
*  testing\rx_test.cpp
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


// rx_test
#include "testing/rx_test.h"

#include "rx_test_io.h"
#include "rx_test_script.h"


namespace testing {

// Class testing::test_command 

test_command::test_command()
	: server_command("test")
{
	register_code_test(std::make_unique<io_test::ip_test::tcp_client_test>());
	register_code_test(std::make_unique<script_test::py_test::python_basic_test>());
}


test_command::~test_command()
{
}



bool test_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	if (!dword_check_premissions(0, 0))
	{
		err << "Access Denied!";
		return false;
	}
	string_type test_cat;
	in >> test_cat;
	const auto& it = _registred_tests.find(test_cat);
	if (it != _registred_tests.end())
	{
		return it->second->do_console_test(in, out, err, ctx);
	}
	else
	{
		err << "Unkonwon test category!!!";
		return false;
	}
}

void test_command::register_code_test (code_test::smart_ptr test)
{
	_registred_tests.emplace(test->get_category(),std::forward<code_test::smart_ptr>(test));
}


// Class testing::code_test 

code_test::code_test(const code_test &right)
{
}

code_test::code_test (const string_type& category)
	: _category(category)
{
}


code_test::~code_test()
{
}


code_test & code_test::operator=(const code_test &right)
{
	RX_ASSERT(false);
	return *this;
}



} // namespace testing

