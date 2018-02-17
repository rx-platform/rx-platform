

/****************************************************************************
*
*  testing\rx_test.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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

#include "lib/security/rx_security.h"
#include "system/server/rx_server.h"
#include "sys_internal/rx_internal_ns.h"

#include "rx_test_io.h"
#include "rx_test_script.h"
#include "rx_test_basic.h"


namespace testing {

// Class testing::test_command 

test_command::test_command()
	: server_command("test")
{
}


test_command::~test_command()
{
}



bool test_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, rx_platform::prog::console_program_context::smart_ptr ctx)
{
	string_type temp_str;
	in >> temp_str;

	if (temp_str == "status" || temp_str == "stat")
	{
		return do_status_command(in, out, err, ctx);
	}
	else if (temp_str == "run")
	{
		return do_run_command(in, out, err, ctx);
	}
	else if (temp_str == "code")
	{
		return do_info_command(in, out, err, ctx);
	}
	else if (temp_str == "list")
	{
		return do_list_command(in, out, err, ctx);
	}
	err << temp_str << " is unknown Test Command.\r\n";
	return false;
}

bool test_command::do_info_command (std::istream& in, std::ostream& out, std::ostream& err, rx_platform::prog::console_program_context::smart_ptr ctx)
{
	string_type temp_str;

	in >> temp_str;

	test_case::smart_ptr test = testing_enviroment::instance().get_test_case(temp_str);

	if (test)
	{
		test->fill_code_info(out, temp_str);
		return true;
	}

	err << temp_str << " is unknown Test Case.\r\n";

	return false;
}

bool test_command::do_run_command (std::istream& in, std::ostream& out, std::ostream& err, rx_platform::prog::console_program_context::smart_ptr ctx)
{

	string_type temp_str;
	in >> temp_str;
	if (temp_str=="-a" || temp_str=="--all")
	{
		bool ret = true;
		std::vector<test_case::smart_ptr> cases;
		testing_enviroment::instance().collect_test_cases("", cases);
		for (auto one : cases)
		{
			out << "Test Case " << one->get_name() << "\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
			ret = one->do_console_test(in, out, err, ctx);
			out << RX_CONSOLE_HEADER_LINE "\r\n";
			if (!ret)
				break;

		}
		return ret;
	}
	test_case::smart_ptr test = testing_enviroment::instance().get_test_case(temp_str);

	if(test)
	{
		return test->do_console_test(in, out, err, ctx);
	}
	err << temp_str << " is unknown Test Case.\r\n";

	return false;
}

bool test_command::do_status_command (std::istream& in, std::ostream& out, std::ostream& err, rx_platform::prog::console_program_context::smart_ptr ctx)
{
	string_type filter;
	in >> filter;
	std::vector<test_case::smart_ptr> cases;
	testing_enviroment::instance().collect_test_cases(filter,cases);

	rx_table_type table;

	rx_row_type header;
	header.emplace_back("Name");
	header.emplace_back("Status");
	header.emplace_back("Time");
	header.emplace_back("User");
	
	table.emplace_back(header);

	for (auto& one : cases)
	{
		rx_row_type row;
		row.emplace_back(one->get_name(), ANSI_COLOR_BOLD ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);

		test_context_data data = one->get_data();
		test_status_t result = one->get_status();
		switch (result)
		{
		case RX_TEST_STATUS_OK:
			row.emplace_back(RX_TEST_STATUS_OK_NAME, ANSI_COLOR_BOLD ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
			break;
		case RX_TEST_STATUS_FAILED:
			row.emplace_back(RX_TEST_STATUS_FAILED_NAME, ANSI_COLOR_BOLD ANSI_COLOR_RED, ANSI_COLOR_RESET);
			break;
		case RX_TEST_STATUS_UNKNOWN:
			row.emplace_back(RX_TEST_STATUS_UNKNOWN_NAME, ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
			break;
		default:
			row.emplace_back("Internal Error!!!", ANSI_COLOR_BOLD ANSI_COLOR_RED, ANSI_COLOR_RESET);
			RX_ASSERT(false);
		}
		if (data.time_stamp.is_valid_time())
			row.emplace_back(data.time_stamp.get_string());
		else
			row.emplace_back("-");
		row.emplace_back(data.user);
		table.emplace_back(row);
	}
	rx_dump_table(table, out, true);
	return true;
}

bool test_command::do_list_command (std::istream& in, std::ostream& out, std::ostream& err, rx_platform::prog::console_program_context::smart_ptr ctx)
{
	string_type temp_str;
	in >> temp_str;
	if (temp_str.empty())
	{
		string_array categories;
		testing_enviroment::instance().get_categories(categories);
		out << "Registered  Test Categories\r\n" RX_CONSOLE_HEADER_LINE "\r\n" ANSI_COLOR_BOLD ANSI_COLOR_YELLOW;
		for (const auto& one : categories)
		{
			out << one;
			out << "\r\n";
		}
		out << ANSI_COLOR_RESET;
		return true;
	}
	else
	{
		string_array cases;
		testing_enviroment::instance().get_cases(temp_str,cases);

		out << "Registered Test Cases for ";
		out << temp_str << " :\r\n=====================================\r\n" ANSI_COLOR_BOLD ANSI_COLOR_YELLOW;
		for (const auto& one : cases)
		{
			out << one;
			out << "\r\n";
		}
		out << ANSI_COLOR_RESET;
		return true;
	}
}


// Class testing::test_category 

test_category::test_category(const test_category &right)
{
}

test_category::test_category (const string_type& category)
	: category_(category)
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



void test_category::register_test_case (test_case::smart_ptr test)
{
	cases_.emplace(test->get_name(), std::forward<test_case::smart_ptr>(test));
}

void test_category::collect_test_cases (std::vector<test_case::smart_ptr>& cases)
{
	for (auto& one : cases_)
		cases.emplace_back(one.second);
}

void test_category::get_cases (string_array& cases)
{
	cases.reserve(cases_.size());
	for (auto one : cases_)
		cases.emplace_back(one.first);
}

test_case::smart_ptr test_category::get_test_case (const string_type& test_name)
{
	const auto& it = cases_.find(test_name);
	if (it != cases_.end())
	{
		return it->second;
	}
	return test_case::smart_ptr::null_ptr;
}


// Class testing::test_case 

test_case::test_case(const test_case &right)
      : start_tick_(0),
        status_(RX_TEST_STATUS_UNKNOWN),
        modified_time_(rx_time::now())
{
	RX_ASSERT(false);
}

test_case::test_case (const string_type& name)
      : start_tick_(0),
        status_(RX_TEST_STATUS_UNKNOWN),
        modified_time_(rx_time::now())
	, name_(name)
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



bool test_case::test_start (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	bool ret = false;
	string_type start_message("Test Case started by :");
	out << start_message;
	security::security_context_ptr active = security::active_security();
	out << ANSI_COLOR_CYAN;
	start_message += active->get_full_name();
	out << active->get_user_name();
	out << ANSI_COLOR_RESET "\r\n";
	if (active->is_interactive())
	{
		ret = true;
		start_tick_ = rx_get_us_ticks();
		TEST_LOG_INFO(start_message.c_str(), 500, "Test Case Started");
	}
	else
	{
		err << "Access Denied!!!\r\n";
	}
	return ret;
}

void test_case::test_end (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	out << RX_CONSOLE_HEADER_LINE "\r\n";
	TEST_LOG_INFO(get_name(), 500, "Test Case Ended");
	out << "Result:";

	status_lock_.lock();
	data_ = ctx->get_data();
	data_.time_stamp = rx_time::now();
	data_.user = security::active_security()->get_full_name();
	test_status_t result = status_ = ctx->get_status();
	status_lock_.unlock();
	switch (result)
	{
	case RX_TEST_STATUS_OK:
		out << ANSI_COLOR_BOLD ANSI_COLOR_GREEN  RX_TEST_STATUS_OK_NAME;
		break;
	case RX_TEST_STATUS_FAILED:
		out << ANSI_COLOR_BOLD ANSI_COLOR_RED  RX_TEST_STATUS_FAILED_NAME;
		break;
	case RX_TEST_STATUS_UNKNOWN:
		out << ANSI_COLOR_CYAN  RX_TEST_STATUS_UNKNOWN_NAME;
		break;
	default:
		RX_ASSERT(false);
		out << ANSI_COLOR_BOLD ANSI_COLOR_RED << "Internal Error!!!";
	}
	out << ANSI_COLOR_RESET "\r\n";
	uint64_t ellapsed = rx_get_us_ticks() - start_tick_;
	out << "Test lasted " << (double)(ellapsed / 1000.0) << "ms.\r\n";
	modified_time_ = rx_time::now();
}

void test_case::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "RXTestCase";
	has_own_code_info = true;
}

string_type test_case::get_type_name () const
{
  // generated from ROSE!!!
  static string_type type_name = "TEST CASE";
  return type_name;


}

values::rx_value test_case::get_value () const
{
	return rx_value(status_,modified_time_);
}

namespace_item_attributes test_case::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_test_case | namespace_item_execute_access | namespace_item_read_access | namespace_item_system);
}

bool test_case::generate_json (std::ostream& def, std::ostream& err) const
{
	return true;
}

test_status_t test_case::get_status (test_context_data* data)
{
	if (data)
		*data = data_;
	return status_;
}

test_context_data test_case::get_data (test_context_data* data) const
{
	return data_;
}

bool test_case::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, rx_platform::prog::console_program_context::smart_ptr ctx)
{
	rx_reference<test_program_context> test_ctx = testing_enviroment::instance().create_test_context(ctx);
	if (test_start(in, out, err, test_ctx))
	{
		bool ret = run_test(in, out, err, test_ctx);
		test_end(in, out, err, test_ctx);
		return ret;
	}
	else
	{
		err << "Error starting test case:" << get_name() << "\r\n";
		return false;
	}
}

bool test_case::is_browsable () const
{
	return false;
}

rx_time test_case::get_created_time () const
{
	return rx_gate::instance().get_started();
}

platform_item_ptr test_case::get_item_ptr ()
{
	return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

bool test_case::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return true;
}

bool test_case::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}

size_t test_case::get_size () const
{
	return 0;
}


// Class testing::testing_enviroment 

testing_enviroment::testing_enviroment()
{
	register_code_test(std::make_unique<io_test::ip_test::tcp_test>());
	register_code_test(std::make_unique<script_test::py_test::python_test>());
	register_code_test(std::make_unique<test_test>());
	register_code_test(std::make_unique<script_test::rxs::rx_script_category>());
	register_code_test(std::make_unique<basic_tests::function_test::function_test_category>());
	register_code_test(std::make_unique<basic_tests::meta_test::meta_model_test_category>());
}


testing_enviroment::~testing_enviroment()
{
}



void testing_enviroment::register_code_test (test_category::smart_ptr test)
{
	categories_.emplace(test->get_category(), std::forward<test_category::smart_ptr>(test));
}

testing_enviroment& testing_enviroment::instance ()
{
	static testing_enviroment g_obj;
	return g_obj;
}

void testing_enviroment::collect_test_cases (const string_type& category, std::vector<test_case::smart_ptr>& cases)
{
	if (category.empty())
	{
		for (auto& one : categories_)
			one.second->collect_test_cases(cases);
	}
	else
	{
		const auto& it = categories_.find(category);
		if (it != categories_.end())
			it->second->collect_test_cases(cases);
	}
}

void testing_enviroment::get_categories (string_array& categories)
{
	for(auto& one : categories_)
	{
		categories.push_back(one.first);
	}
}

void testing_enviroment::get_cases (const string_type& category, string_array& cases)
{
	const auto& it = categories_.find(category);
	if (it != categories_.end())
		return it->second->get_cases(cases);
}

test_case::smart_ptr testing_enviroment::get_test_case (const string_type& test_name)
{
	size_t idx = test_name.find_first_of("./");
	if (idx != string_type::npos)
	{
		string_type category(test_name.substr(0, idx));
		string_type name = test_name.substr(idx + 1);
		const auto& it = categories_.find(category);
		if (it != categories_.end())
		{
			return it->second->get_test_case(name);
		}
	}
	return test_case::smart_ptr::null_ptr;
}

test_program_context::smart_ptr testing_enviroment::create_test_context (rx_platform::prog::console_program_context::smart_ptr console_ctx)
{
	return rx_create_reference<test_program_context>(
		prog::server_program_holder_ptr::null_ptr,
		prog::program_context_base_ptr::null_ptr,
		console_ctx->get_current_directory(),
		console_ctx->get_out(),
		console_ctx->get_err(),
		console_ctx->get_program()
		);
}


// Class testing::test_program_context 

test_program_context::test_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<server_program_base> program)
      : status_(RX_TEST_STATUS_UNKNOWN)
	, rx_platform::prog::program_context_base(holder, root_context, current_directory, out, err,program)
{
}


test_program_context::~test_program_context()
{
}



void test_program_context::set_failed ()
{
	status_ = RX_TEST_STATUS_FAILED;
	fill_data();
}

void test_program_context::set_passed ()
{
	status_ = RX_TEST_STATUS_OK;
	fill_data();
}

void test_program_context::fill_data ()
{
	data_.time_stamp = rx_time::now();
	data_.user = security::active_security()->get_full_name();
}

size_t test_program_context::get_possition () const
{
	return 0;
}


// Class testing::basic_test_case_test 

basic_test_case_test::basic_test_case_test()
	: test_case("test")
{
}


basic_test_case_test::~basic_test_case_test()
{
}



bool basic_test_case_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{

	bool pass = true;
	if (!in.eof())
	{
		in >> pass;
	}

	out << "Testing Test Category apstract....\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
	out << "This is a dummy test case that is testing the test_case mechanisms\r\n";

	if (pass)
		ctx->set_passed();
	else
		ctx->set_failed();

	return true;
}


// Class testing::test_test 

test_test::test_test()
	: test_category("test")
{
	register_test_case(rx_create_reference<basic_test_case_test>());
}


test_test::~test_test()
{
}



} // namespace testing

