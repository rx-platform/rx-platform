

/****************************************************************************
*
*  testing\rx_test_runtime.cpp
*
*  Copyright (c) 2018-2019 Dusan Ciric
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/


#include "pch.h"

#ifndef EXCLUDE_TEST_CODE

// rx_test_runtime
#include "testing/rx_test_runtime.h"

#include "api/rx_meta_api.h"
#include "sys_internal/rx_internal_ns.h"
#include "lib/rx_rt_data.h"
#include <valarray>
#include "lib/rx_const_size_vector.h"
#include "system/server/rx_async_functions.h"
#define ANSI_RX_OBJECT_SIZE ANSI_COLOR_GREEN ANSI_COLOR_BOLD
#define ANSI_RX_OBJECT_NAME ANSI_COLOR_YELLOW ANSI_COLOR_BOLD


namespace testing {

namespace basic_tests {

namespace runtime_test {

// Class testing::basic_tests::runtime_test::runtime_structure_test

runtime_structure_test::runtime_structure_test()
	 : test_case("structure")
{
}


runtime_structure_test::~runtime_structure_test()
{
}



bool runtime_structure_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	 out << "Calculating runtime sizes:\r\n" RX_CONSOLE_HEADER_LINE "\r\n";

	 out << "object_runtime:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::objects::object_runtime) << ANSI_COLOR_RESET "\r\n";
	 out << "variable_runtime:" ANSI_RX_OBJECT_SIZE  << sizeof(runtime::blocks::variable_runtime) << ANSI_COLOR_RESET << "\r\n";

	 out << "\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
	 out << "empty_item_data:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::empty_item_data) << ANSI_COLOR_RESET "\r\n";
	 out << "basic_item_data:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::basic_item_data) << ANSI_COLOR_RESET "\r\n";
	 out << "full_item_data:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::full_item_data) << ANSI_COLOR_RESET "\r\n";

	 std::vector< runtime::structure::variable_data> stuff;
	 std::valarray<runtime::structure::variable_data> testing;

	 rx::const_size_vector<size_t> perica({ 56, 38, 38 });

	 for (auto rr : perica)
		 out << "Element:" << rr << "\r\n";

	 out << "const_size_vector:" ANSI_RX_OBJECT_SIZE << sizeof(rx::const_size_vector<runtime::structure::variable_data>) << ANSI_COLOR_RESET "\r\n";
	 out << "array:" ANSI_RX_OBJECT_SIZE << sizeof(std::array<runtime::structure::variable_data, 0>) << ANSI_COLOR_RESET "\r\n";
	 out << "empty:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::empty<runtime::structure::variable_data>) << ANSI_COLOR_RESET "\r\n";
	 out << "mixed:" ANSI_RX_OBJECT_SIZE << sizeof(runtime::structure::mixed_item_data) << ANSI_COLOR_RESET "\r\n";

	 out << "bitset<8>:" ANSI_RX_OBJECT_SIZE << sizeof(std::bitset<8>) << ANSI_COLOR_RESET "\r\n";
	 out << "bitset<32>:" ANSI_RX_OBJECT_SIZE << sizeof(std::bitset<32>) << ANSI_COLOR_RESET "\r\n";
	 out << "bitset<64>:" ANSI_RX_OBJECT_SIZE << sizeof(std::bitset<64>) << ANSI_COLOR_RESET "\r\n";

	 ctx->set_failed();
	 return true;
}


// Class testing::basic_tests::runtime_test::runtime_test_category

runtime_test_category::runtime_test_category()
	: test_category("runtime")
{
	register_test_case(rx_create_reference<runtime_structure_test>());
	register_test_case(rx_create_reference<runtime_transaction_test>());
	register_test_case(rx_create_reference<runtime_connect_test>());
}


runtime_test_category::~runtime_test_category()
{
}



// Class testing::basic_tests::runtime_test::runtime_transaction_test

runtime_transaction_test::runtime_transaction_test()
	: test_case("transaction")
{
}


runtime_transaction_test::~runtime_transaction_test()
{
}



bool runtime_transaction_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{

	auto transaction_state = new rx_transaction_state<rx_result, int>();

	ctx->set_current_test_case(smart_this());
	ctx->set_waiting();

	out << "Starting in thread " << rx_current_thread() << "\r\n";

	rx_do_transaction_with_callback<rx_result, smart_ptr, int>(
		{
			{ [ctx](int val)->rx_result {
					auto& out = ctx->get_stdout();
					out << "Entered to first function\r\n";
					out << "Running in thread " << rx_current_thread() << "\r\n";
					out << "Argument=" << val << "\r\n";
					out << "Returning true!\r\n";
					return true;
				},
				RX_DOMAIN_IO
			}
			, { [ctx](int val)->rx_result {
					auto& out = ctx->get_stdout();
					out << "Entered to second function\r\n";
					out << "Running in thread " << rx_current_thread() << "\r\n";
					out << "Argument=" << val << "\r\n";
					out << "Returning error!\r\n";
					return "Jebi ga!!!";
				},
				2
			}
		}
		, [ctx] (rx_result result)
		{
			auto& out = ctx->get_stdout();
			out << "Result in thread " << rx_current_thread() << "\r\n";
			if (result)
			{
				out << "Returned with result of success!\r\n";
			}
			else
			{
				out << "Returned with result of failure!\r\n";
				for (const auto& one : result.errors())
				{
					out << one << "\r\n";
				}
			}
			ctx->set_passed();

			ctx->async_test_end();
		}, smart_this(), transaction_state, 5);


	return true;
}


// Class testing::basic_tests::runtime_test::runtime_connect_test

runtime_connect_test::runtime_connect_test()
	: test_case("connect")
{
	callback_.parent = this;
}


runtime_connect_test::~runtime_connect_test()
{
}



bool runtime_connect_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	bool no_subscription = true;
	string_type path("/_sys/objects/system/SystemApp.CPU");
	string_type expression("{rx://local#"s + path + "} + 1000");
	out << "Connecting to expression: " + expression + "\r\n";
	my_value_.connect(expression, 200,
		[ctx](const rx_value& val)
		{
			auto& out = ctx->get_stdout();
			out << ANSI_COLOR_CYAN "Callback: " ANSI_COLOR_RESET " ";
			val.dump_to_stream(out);
			out << "\r\n";
		});

	auto subs = rx_create_reference<sys_runtime::subscriptions::rx_subscription>(&callback_);
	subs->activate();
	string_array paths{ path };
	std::vector<rx_result_with<runtime_handle_t> > results;
	auto result = no_subscription ? rx_result(true) : subs->connect_items(paths, results);
	if (result)
	{
		if (no_subscription || results[0])
		{
			if(!no_subscription)
				out << "Connected to tag " << path << ", handle value = " << results[0].value() << "\r\n";

			std::function<void(string_type, runtime_handle_t)> func=[this, ctx, subs, expression, no_subscription](string_type path, runtime_handle_t hndl) mutable
				{
					auto& out = ctx->get_stdout();

					out << "Disconnecting from expression: " + expression + "\r\n";
					my_value_.disconnect();
					out << "Disconnected!!!\r\n";

					if (!no_subscription)
					{
						std::vector<runtime_handle_t> items{ hndl };
						results_array disconnect_results;
						auto result = no_subscription ? rx_result(true) : subs->disconnect_items(items, disconnect_results);
						if (result)
						{
							if (no_subscription || disconnect_results[0])
							{
								out << "Disconnected from tag " << path << ", handle value = " << hndl << "\r\n";
								ctx->set_passed();

							}
							else
							{
								out << "Error disconnecting from tag " << path << "\r\n";
								rx_dump_error_result(out, disconnect_results[0]);
							}
						}
						else
							rx_dump_error_result(out, result);
						subs->deactivate();
					}
					else
					{
						ctx->set_passed();
					}


					ctx->async_test_end();

				};

			rx_post_delayed_function<smart_ptr, string_type, runtime_handle_t>(5000, func
				, smart_this(), path, no_subscription ? 0 : results[0].value());

			ctx->set_current_test_case(smart_this());
			ctx->set_waiting();
			return true;
		}
		else
		{
			out << "Error connecting tag " << path << "\r\n";
			dump_error_result(out, results[0]);
		}

	}
	else
		rx_dump_error_result(out, result);

	subs->deactivate();
	ctx->set_failed();
	return true;
}


} // namespace runtime_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE
