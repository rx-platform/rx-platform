

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
#include "lib/security/rx_security.h"
#include "rx_test_script.h"
#include "system/server/rx_server.h"


namespace testing {

// Class testing::test_command 

test_command::test_command()
	: server_command("test")
{
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

	return testing_enviroment::instance().do_console_command(in, out, err, ctx);
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



bool test_category::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx, bool code)
{
	string_type case_name;
	in >> case_name;

	auto it = _cases.find(case_name);
	if (it != _cases.end())
	{
		if (code)
		{
			it->second->fill_code_info(out,case_name);
			return true;
		}
		else
		{
			if (it->second->test_start(in, out, err, ctx))
			{
				bool ret = it->second->do_console_test(in, out, err, ctx);
				it->second->test_end(in, out, err, ctx);
				return ret;
			}
			else
			{
				err << case_name << "error starting test case.\r\n";
				return false;
			}
		}
	}
	else if(case_name.empty())
	{
		out << "Registered Test Cases for ";
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

void test_category::collect_test_cases (std::vector<rx_server_item::smart_ptr>& cases)
{
	for (auto& one : _cases)
		cases.emplace_back(one.second);
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
	string_type start_message("Test Case started by :");
	out << start_message;
	security::security_context_ptr active = security::active_security();
	out << ANSI_COLOR_CYAN;
	start_message += active->get_full_name();
	out << active->get_user_name();
	out << ANSI_COLOR_RESET "\r\n";
	TEST_LOG_INFO(start_message.c_str(), 500,"Test Case Started");
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
	TEST_LOG_INFO(_name, 500, "Test Case Ended");
	uint64_t ellapsed = rx_get_us_ticks() - _start_tick;
	out << "Test lasted " << (double)(ellapsed / 1000.0) << "ms.\r\n";
}

void test_case::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "_TestCase";
	has_own_code_info = true;
}

string_type test_case::get_type_name () const
{
  // generated from ROSE!!!
  static string_type type_name = "TEST CASE";
  return type_name;


}

void test_case::get_value (values::rx_value& val) const
{
	val = rx_value(_name);
}

namespace_item_attributes test_case::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_test_case | namespace_item_execute | namespace_item_read_access | namespace_item_system);
}

const string_type& test_case::get_item_name () const
{
	return _name;
}

bool test_case::generate_json (std::ostream& def, std::ostream& err) const
{
	return true;
}


// Class testing::testing_enviroment 

testing_enviroment::testing_enviroment()
{
	register_code_test(std::make_unique<io_test::ip_test::tcp_test>());
	register_code_test(std::make_unique<script_test::py_test::python_test>());
	register_code_test(std::make_unique<test_test>());
	register_code_test(std::make_unique<script_test::rxs::rx_script_category>());
}


testing_enviroment::~testing_enviroment()
{
}



void testing_enviroment::register_code_test (test_category::smart_ptr test)
{
	_registered_tests.emplace(test->get_category(), std::forward<test_category::smart_ptr>(test));
}

testing_enviroment& testing_enviroment::instance ()
{
	static testing_enviroment g_obj;
	return g_obj;
}

bool testing_enviroment::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	bool code = false;

	string_type temp_str;
	in >> temp_str;
	if (temp_str == "code")
	{
		code = true;
		if (!in.eof())
			in >> temp_str;
		else
			temp_str.clear();
	}

	const auto& it = _registered_tests.find(temp_str);
	if (it != _registered_tests.end())
	{
		return it->second->do_console_test(in, out, err, ctx, code);
	}
	else if (temp_str.empty())
	{
		out << "Registered  Test Categories\r\n=====================================\r\n" ANSI_COLOR_YELLOW;
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
		err << temp_str << "is unknown Test Category.\r\n";
		return false;
	}
}

void testing_enviroment::collect_test_cases (std::vector<rx_server_item::smart_ptr>& cases)
{
	for (auto& one : _registered_tests)
		one.second->collect_test_cases(cases);
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


// Class testing::test_test 

test_test::test_test()
	: test_category("test")
{
	register_test_case(basic_test_case_test::smart_ptr(pointers::_create_new));
}


test_test::~test_test()
{
}



} // namespace testing

