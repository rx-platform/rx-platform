

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
#include "system/security/rx_security.h"
#include "rx_test_script.h"
#include "system/server/rx_server.h"


namespace testing {

// Class testing::test_command 

test_command::test_command()
	: server_command("test")
{
	register_code_test(std::make_unique<io_test::ip_test::tcp_test>());
	register_code_test(std::make_unique<script_test::py_test::python_test>());
	register_code_test(std::make_unique<test_test>());
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
	const auto& it = _registered_tests.find(test_cat);
	if (it != _registered_tests.end())
	{
		return it->second->do_console_test(in, out, err, ctx);
	}
	else if(test_cat.empty())
	{
		out << "Registered  test categories\r\n=====================================\r\n" ANSI_COLOR_YELLOW;
		for (const auto& one : _registered_tests)
		{
			out << one.first;
			out << "\r\n";
		}
		out << ANSI_COLOR_RESET;
		return true;
	}
	else
	{
		err << test_cat << "is unknown test category.\r\n";
		return false;
	}
}

void test_command::register_code_test (test_category::smart_ptr test)
{
	_registered_tests.emplace(test->get_category(),std::forward<test_category::smart_ptr>(test));
}


// Class testing::test_category 

test_category::test_category(const test_category &right)
{
}

test_category::test_category (const string_type& category)
	: _category(category)
{
}


test_category::~test_category()
{
}


test_category & test_category::operator=(const test_category &right)
{
	RX_ASSERT(false);
	return *this;
}



bool test_category::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	string_type case_name;
	in >> case_name;

	auto it = _cases.find(case_name);
	if (it != _cases.end())
	{
		if (it->second->test_start(in, out, err, ctx))
		{
			bool ret = it->second->do_console_test(in, out, err, ctx);
			it->second->test_end(in, out, err, ctx);
			return true;
		}
		else
		{
			err << case_name << "error starting test case.\r\n";
			return false;
		}
	}
	else if(case_name.empty())
	{
		out << "Registred test tases for ";
		out << _category<< " :\r\n=====================================\r\n" ANSI_COLOR_YELLOW;
		for (const auto& one : _cases)
		{
			out << one.first;
			out << "\r\n";
		}
		out << ANSI_COLOR_RESET;
		return true;
	}
	else
	{
		err << case_name << "is unknown test case.\r\n";
		return false;
	}
}

void test_category::register_test_case (test_case::smart_ptr test)
{
	_cases.emplace(test->get_name(), std::forward<test_case::smart_ptr>(test));
}


// Class testing::test_case 

test_case::test_case(const test_case &right)
      : _start_tick(0)
{
	RX_ASSERT(false);
}

test_case::test_case (const string_type& name)
      : _start_tick(0)
	, _name(name)
{
}


test_case::~test_case()
{
}


test_case & test_case::operator=(const test_case &right)
{
	RX_ASSERT(false);
	return *this;
}



bool test_case::test_start (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	bool ret = false;
	string_type start_message("Test started by :");
	out << start_message;
	security::security_context_ptr active = security::active_security();
	out << ANSI_COLOR_CYAN;
	start_message += active->get_full_name();
	out << active->get_user_name();
	out << ANSI_COLOR_RESET "\r\n";
	TEST_LOG_INFO(start_message.c_str(), 500,"Test Started");
	if (active->is_interactive())
	{
		ret = true;
		_start_tick = rx_get_us_ticks();
	}
	return ret;
}

void test_case::test_end (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	out << "===================================\r\n";
	TEST_LOG_INFO(_name, 500, "Test Ended");
	qword ellapsed = rx_get_us_ticks() - _start_tick;
	out << "Test lasted " << (double)(ellapsed / 1000.0) << "ms.\r\n";
}


// Class testing::test_test 

test_test::test_test()
	: test_category("test")
{
	register_test_case(std::make_unique<basic_test_case_test>());
}


test_test::~test_test()
{
}



// Class testing::basic_test_case_test 

basic_test_case_test::basic_test_case_test()
	: test_case("test")
{
}


basic_test_case_test::~basic_test_case_test()
{
}



bool basic_test_case_test::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{

	out << "Testing Test Category apstract....\r\n==========================\r\n";
	out << "This is a dummy test case that is testing the test_case mechanisms\r\n";
	return true;
}


} // namespace testing

