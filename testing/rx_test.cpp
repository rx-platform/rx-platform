

/****************************************************************************
*
*  testing\rx_test.cpp
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


#include "pch.h"

#include "system/server/rx_server.h"
#define ANSI_RX_TEST_NAME ANSI_COLOR_YELLOW ANSI_COLOR_BOLD

// rx_test
#include "testing/rx_test.h"

#include "security/rx_security.h"
#include "system/server/rx_server.h"
#include "sys_internal/rx_internal_ns.h"
#include "terminal/rx_term_table.h"
#include "rx_test_io.h"
#include "rx_test_script.h"
#include "rx_test_basic.h"
#include "rx_test_meta.h"
#include "rx_test_runtime.h"


namespace testing {
namespace {
testing_enviroment* g_obj = nullptr;
}

// Class testing::test_command 

test_command::test_command()
	: server_command("test")
{
}


test_command::~test_command()
{
}



bool test_command::do_console_command (std::istream& in, std::ostream& out, std::ostream& err, rx_internal::terminal::console_context_ptr ctx)
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

bool test_command::do_info_command (std::istream& in, std::ostream& out, std::ostream& err, rx_internal::terminal::console_context_ptr ctx)
{
	string_type temp_str;

	in >> temp_str;

	test_case::smart_ptr test = testing_enviroment::instance().get_test_case(temp_str);

	if (test)
	{
		test->fill_code_info(out, temp_str);
		return true;
	}

	err << temp_str << " is unknown Test case.\r\n";

	return false;
}

bool test_command::do_run_command (std::istream& in, std::ostream& out, std::ostream& err, rx_internal::terminal::console_context_ptr ctx)
{

	string_type temp_str;
	in >> temp_str;
	if (temp_str=="-a" || temp_str=="--all")
	{
		bool ret = true;
		std::vector<std::pair<string_type, test_case::smart_ptr> > cases;
		testing_enviroment::instance().collect_test_cases("", cases);
		for (auto one : cases)
		{
			out << "\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
			ret = one.second->do_console_test(in, out, err, ctx);
			out << RX_CONSOLE_HEADER_LINE "\r\n";
			if (!ret)
				break;
		}
		return ret;
	}
	test_case::smart_ptr test = testing_enviroment::instance().get_test_case(temp_str);

	if(test)
	{
		bool ret = test->do_console_test(in, out, err, ctx);
		return ret;
	}
	err << temp_str << " is unknown Test case.\r\n";

	return false;
}

bool test_command::do_status_command (std::istream& in, std::ostream& out, std::ostream& err, rx_internal::terminal::console_context_ptr ctx)
{
	string_type filter;
	in >> filter;
	std::vector<std::pair<string_type, test_case::smart_ptr> > cases;
	testing_enviroment::instance().collect_test_cases(filter,cases);

	rx_table_type table;

	rx_row_type header;
	header.emplace_back("Category");
	header.emplace_back("Name");
	header.emplace_back("Status");
	header.emplace_back("Time");
	header.emplace_back("User");

	table.emplace_back(header);

	string_type old_category;

	for (auto& one : cases)
	{
		rx_row_type row;
		if (old_category == one.first)
		{
			row.emplace_back("");
		}
		else
		{
			row.emplace_back(one.first, ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
			old_category = one.first;
		}
		row.emplace_back(one.second->get_name(), ANSI_COLOR_BOLD ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);

		test_context_data data = one.second->get_data();
		test_status_t result = one.second->get_status();
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
	rx_dump_table(table, out, true, false);
	return true;
}

bool test_command::do_list_command (std::istream& in, std::ostream& out, std::ostream& err, rx_internal::terminal::console_context_ptr ctx)
{
	string_array categories;
	testing_enviroment::instance().get_categories(categories);
	size_t size = categories.size();
	size_t table_size = 1;
	std::vector<string_array> cases(size);
	for (size_t i=0; i<size; i++)
	{
		testing_enviroment::instance().get_cases(categories[i], cases[i]);
		table_size += cases[i].size();
	}
	rx_table_type table(table_size);

	table[0].emplace_back("Category");
	table[0].emplace_back("Test Cases");

	int row = 1;
	for (size_t i = 0; i<size; i++)
	{
		table[row].emplace_back(categories[i], ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
		if (cases[i].empty())
		{
			table[row].emplace_back("");
		}
		else
		{
			table[row].emplace_back(cases[i][0],ANSI_COLOR_BOLD ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
			for (size_t j = 1; j < cases[i].size(); j++)
			{
				row++;
				table[row].emplace_back("");
				table[row].emplace_back(cases[i][j], ANSI_COLOR_BOLD ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
			}
		}
		row++;
	}
	rx_dump_table(table, out, true, true);
	return true;
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

void test_category::collect_test_cases (std::vector<std::pair<string_type, test_case::smart_ptr> >& cases)
{
	for (auto& one : cases_)
		cases.emplace_back(category_, one.second);
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



bool test_case::test_start (std::istream& in, std::ostream& out, std::ostream& err, test_program_context* ctx)
{
	bool ret = false;
	out << "Test case " ANSI_RX_TEST_NAME << get_name() << ANSI_COLOR_RESET " started.\r\n" RX_CONSOLE_HEADER_LINE "\r\n";

	security::security_context_ptr active = security::active_security();
	if (active->is_interactive())
	{
		ret = true;
		std::stringstream stream;
		stream << "Test case " << get_name() << " started by " << active->get_full_name();
		start_tick_ = rx_get_us_ticks();
		TEST_LOG_INFO(get_name(), 500, stream.str().c_str());
	}
	else
	{
		err << "Access Denied!!!\r\n";
	}
	return ret;
}

void test_case::test_end (std::ostream& out, std::ostream& err, test_program_context* ctx)
{
	out << RX_CONSOLE_HEADER_LINE "\r\n";
	out << "Result:";


	std::stringstream stream;
	stream << "Test case " << get_name() << " ended with result ";

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
		stream << RX_TEST_STATUS_OK_NAME;
		break;
	case RX_TEST_STATUS_FAILED:
		out << ANSI_COLOR_BOLD ANSI_COLOR_RED  RX_TEST_STATUS_FAILED_NAME;
		stream << RX_TEST_STATUS_FAILED_NAME;
		break;
	case RX_TEST_STATUS_UNKNOWN:
		out << ANSI_COLOR_CYAN  RX_TEST_STATUS_UNKNOWN_NAME;
		stream << RX_TEST_STATUS_UNKNOWN_NAME;
		break;
	default:
		RX_ASSERT(false);
		out << ANSI_COLOR_BOLD ANSI_COLOR_RED << "Internal Error!!!";
	}
	out << ANSI_COLOR_RESET "\r\n";
	uint64_t ellapsed_ticks = rx_get_us_ticks() - start_tick_;
	double ellapsed = (double)ellapsed_ticks / 1000.0;
	out << "Test lasted " << ellapsed << "ms.\r\n";

	stream << ", lasted " << ellapsed << "ms.";
	TEST_LOG_INFO(get_name(), 500, stream.str().c_str());

	modified_time_ = rx_time::now();
}

void test_case::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "RXTestCase";
	has_own_code_info = true;
}

rx_item_type test_case::get_type_id () const
{
  // generated from ROSE!!!
  static string_type type_name = "TEST CASE";
  return rx_item_type::rx_test_case_type;


}

values::rx_value test_case::get_value () const
{
	rx_value temp;
	temp.assign_static(status_, modified_time_);
	return temp;
}

namespace_item_attributes test_case::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_execute_access | namespace_item_read_access | namespace_item_system);
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

bool test_case::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, rx_internal::terminal::console_context_ptr ctx)
{
	auto test_ctx = testing_enviroment::instance().create_test_context(ctx);
	if (test_start(in, out, err, test_ctx))
	{
		bool ret = run_test(in, out, err, test_ctx);
		if (!ret)
		{
			test_end(out, err, test_ctx);
			return ret;
		}
		return true;
	}
	else
	{
		err << "Error starting test case:" << ANSI_RX_TEST_NAME << get_name() << ANSI_COLOR_RESET "\r\n";
		return false;
	}
}

rx_time test_case::get_created_time () const
{
	return rx_gate::instance().get_started();
}

platform_item_ptr test_case::get_item_ptr () const
{
	return std::make_unique<rx_internal::internal_ns::rx_other_implementation<smart_ptr> >(smart_this());
}

bool test_case::serialize (base_meta_writer& stream, uint8_t type) const
{
	return true;
}

bool test_case::deserialize (base_meta_reader& stream, uint8_t type)
{
	return true;
}

size_t test_case::get_size () const
{
	return 0;
}

void test_case::async_test_end (test_program_context* ctx)
{
	test_end(ctx->get_stdout(), ctx->get_stderr(), ctx);
	ctx->continue_scan();
}


const rx_platform::meta::meta_data& test_case::meta_info () const
{
  return meta_info_;
}


// Class testing::testing_enviroment 

testing_enviroment::testing_enviroment()
{
	register_test_category(std::make_unique<test_test>());
#ifndef EXCLUDE_TEST_CODE
	register_test_category(std::make_unique<script_test::rxs::rx_script_category>());
	register_test_category(std::make_unique<basic_tests::lib_test::library_test_category>());
	register_test_category(std::make_unique<basic_tests::meta_test::meta_model_test_category>());
	register_test_category(std::make_unique<basic_tests::runtime_test::runtime_test_category>());
	register_test_category(std::make_unique<io_test::ip_test::tcp_test>());
	register_test_category(std::make_unique<script_test::py_test::python_test>());
#endif // EXCLUDE_TEST_CODE
}


testing_enviroment::~testing_enviroment()
{
}



void testing_enviroment::register_test_category (test_category::smart_ptr test)
{
	categories_.emplace(test->get_category(), std::move(test));
}

testing_enviroment& testing_enviroment::instance ()
{
	if (g_obj == nullptr)
		g_obj = new testing_enviroment();
	return *g_obj;
}

void testing_enviroment::collect_test_cases (const string_type& category, std::vector<std::pair<string_type, test_case::smart_ptr> >& cases)
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

test_program_context* testing_enviroment::create_test_context (rx_internal::terminal::console_context_ptr console_ctx)
{
	auto api_ctx = console_ctx->create_api_context();
	return new test_program_context(
			console_ctx,
			console_ctx->get_program_holder(),
			console_ctx->get_current_directory(),
			rx_create_reference<memory::std_strbuff<memory::std_vector_allocator> >(),
			rx_create_reference<memory::std_strbuff<memory::std_vector_allocator> >(),
			api_ctx.object
		);
}


// Class testing::test_program_context 

test_program_context::test_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, const string_type& current_directory, buffer_ptr out, buffer_ptr err, rx_reference_ptr anchor)
      : status_(RX_TEST_STATUS_UNKNOWN),
        out_std_(out.unsafe_ptr()),
        err_std_(err.unsafe_ptr()),
        out_(out),
        err_(err)
	, console_program_context(parent, holder,  current_directory)
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

void test_program_context::async_test_end ()
{
	this->current_test_case_->async_test_end(this);
}

std::ostream& test_program_context::get_stdout ()
{
	return out_std_;
}

std::ostream& test_program_context::get_stderr ()
{
	return err_std_;
}

api::rx_context test_program_context::create_api_context ()
{
	api::rx_context ret;
	return ret;
}

void test_program_context::send_results (bool result, bool done)
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



bool basic_test_case_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	bool pass = true;
	if (!in.eof())
	{
		in >> pass;
	}

	out << "Testing Test Category abstract....\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
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

