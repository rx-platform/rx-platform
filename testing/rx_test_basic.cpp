

/****************************************************************************
*
*  testing\rx_test_basic.cpp
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

#ifndef EXCLUDE_TEST_CODE

// rx_test_basic
#include "testing/rx_test_basic.h"

#include "system/serialization/rx_ser_json.h"
#include "system/serialization/rx_ser_bin.h"
#include "terminal/rx_terminal_style.h"
#include "system/server/rx_server.h"


namespace testing {

namespace basic_tests {

namespace lib_test {

// Class testing::basic_tests::lib_test::library_test_category 

library_test_category::library_test_category()
	: test_category("lib")
{
	register_test_case(rx_create_reference<platform_callback_test>());
	register_test_case(rx_create_reference<values_test>());
	register_test_case(rx_create_reference<external_interfaces_test>());
}


library_test_category::~library_test_category()
{
}



class test_base_class
{
public:
	void stuff_heapend(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) <<"callback in " << _rx_func_ << "\r\n";
	}
	virtual void stuff_heapend_virtual(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) << "callback in " << _rx_func_ << "\r\n";
	}
	void stuff_heapend_diff(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) << "callback in " << _rx_func_ << "\r\n";
	}
};
class test_derived_class : public test_base_class
{
public:
	void stuff_heapend(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out)<< "callback in " << _rx_func_ << "\r\n";
	}
	void stuff_heapend_virtual(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) << "callback in " << _rx_func_ << "\r\n";
	}

	void stuff_heapend_diff2(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) << "callback in " << _rx_func_ << "\r\n";
	}
};
void static_callback(const std::ostream& out, callback::callback_state_t state)
{
	const_cast<std::ostream&>(out) << "callback in " << _rx_func_ << "r\n";
}
void test_callbacks(std::ostream& out)
{
	
}

// Class testing::basic_tests::lib_test::platform_callback_test 

platform_callback_test::platform_callback_test()
	: test_case("callback")
{
}


platform_callback_test::~platform_callback_test()
{
}



bool platform_callback_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	test_callbacks(out);
	ctx->set_passed();
	return true;
}


// Class testing::basic_tests::lib_test::values_test 

values_test::values_test()
	: test_case("values")
{
}


values_test::~values_test()
{
}



bool values_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{

	{
		ctx->set_failed();
		bool failed = true;

		std::vector<rx::values::rx_simple_value> simples(4);
		simples[0].assign_static(true);
		simples[1].assign_static(8);
		simples[2].assign_static(6.7);
		simples[3].assign_static("string");

		rx_time now = rx::rx_time::now();

		std::vector<rx::values::rx_timed_value> timed(4);
		timed[0].assign_static(false, now);
		timed[1].assign_static(9, now);
		timed[2].assign_static(6.8, now);
		timed[3].assign_static("string2", now);


		std::vector<rx::values::rx_value> fulls;

		out << ANSI_COLOR_GREEN "Created four simple values and four timed values in an std::vector\r\n" ANSI_COLOR_RESET;

		out << "\r\nsimple values\r\n" RX_CONSOLE_HEADER_LINE;
		for (size_t idx = 0; idx < simples.size(); idx++)
		{
			out << "\r\nsimple[" << idx << "]=";
			simples[idx].dump_to_stream(out);
		}

		out << "\r\ntimed values\r\n" RX_CONSOLE_HEADER_LINE;
		for (size_t idx = 0; idx < timed.size(); idx++)
		{
			out << "\r\ntimed[" << idx << "]=";
			timed[idx].dump_to_stream(out);
		}

		serialization::json_writer writter;


		if (test_serialization("JSON", simples, timed, fulls, writter, out))
		{
			string_type result(writter.get_string());
			if (!result.empty())
			{
				out << "\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
				out << result;
				out << "\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
				serialization::json_reader reader;
				if (reader.parse_data(result))
				{
					if (test_deserialization("JSON", simples, timed, fulls, reader, out))
					{
						failed = false;
					}
				}
			}
		}
		if (!failed)
		{
			memory::std_buffer buffer;
			serialization::std_buffer_writer bwriter(buffer);
			if (test_serialization("Binary", simples, timed, fulls, bwriter, out))
			{
				out << "\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
				bwriter.dump_to_stream(out);
				out << "\r\n" RX_CONSOLE_HEADER_LINE "\r\n";
				serialization::std_buffer_reader reader(buffer);
				if (test_deserialization("Binary", simples, timed, fulls, reader, out))
				{
					failed = false;
				}

			}
		}
		if (!failed)
			ctx->set_passed();
	}
	out << "\r\n\r\n";
	return true;
}

bool values_test::test_serialization (const string_type& name, std::vector<rx::values::rx_simple_value>& simples, std::vector<rx::values::rx_timed_value>& timed, std::vector<rx::values::rx_value>& fulls, base_meta_writer& writer, std::ostream& out)
{

	out << "\r\n\r\n" << name << " serialization std::vector 8 of values...\r\n";

	memory::std_buffer binary_buffer;
	serialization::binary_writer<memory::std_vector_allocator, false> bwritter(binary_buffer);

	writer.write_header(STREAMING_TYPE_VALUES, simples.size() + timed.size());

	for (const auto& one : simples)
		one.serialize("simple", writer);
	for (const auto& one : timed)
		one.serialize("timed", writer);

	if (writer.write_footer())
	{
		out << "\r\n==============================\r\n " ANSI_COLOR_GREEN << name << " serialization succeeded" ANSI_COLOR_RESET "\r\n";
		return true;
	}
	else
	{
		out << "\r\n==============================\r\n " ANSI_COLOR_RED << name << " serialization failed" ANSI_COLOR_RESET "\r\n";
		return false;
	}
}

bool values_test::test_deserialization (const string_type& name, std::vector<rx::values::rx_simple_value>& simples, std::vector<rx::values::rx_timed_value>& timed, std::vector<rx::values::rx_value>& fulls, base_meta_reader& reader, std::ostream& out)
{
	bool failed = false;
	int type;


	out << "\r\n\r\n" << name << " deserialization std::vector, 8 values...\r\n";
	if (reader.read_header(type))
	{
		std::vector<rx::values::rx_simple_value> simples_after(4);
		std::vector<rx::values::rx_timed_value> timed_after(4);

		if (type == STREAMING_TYPE_VALUES)
		{
			for (auto& one : simples_after)
			{
				if (reader.array_end())
				{
					failed = true;
					break;
				}
				if (!one.deserialize("simple", reader))
				{
					failed = true;
					break;
				}
			}
			for (auto& one : timed_after)
			{
				if (reader.array_end())
				{
					failed = true;
					break;
				}
				if (!one.deserialize("timed", reader))
				{
					failed = true;
					break;
				}
			}


			if (!failed)
			{
				out << "\r\n==============================\r\n " ANSI_COLOR_GREEN << name << " deserialization succeeded" ANSI_COLOR_RESET "\r\n";

				out << ANSI_COLOR_GREEN "Deserialize four simple values and four timed values in an two std::vector\r\n" ANSI_COLOR_RESET;

				out << "\r\nsimple values\r\n==================================";
				for (size_t idx = 0; idx < simples.size(); idx++)
				{
					out << "\r\nsimple[" << idx << "]=";
					simples[idx].dump_to_stream(out);
				}

				out << "\r\ntimed values\r\n==================================";
				for (size_t idx = 0; idx < timed.size(); idx++)
				{
					out << "\r\ntimed[" << idx << "]=";
					timed[idx].dump_to_stream(out);
				}

				out << "\r\ncomparing values";
				for (size_t idx = 0; idx < simples.size(); idx++)
				{
					bool same = simples[idx] == simples_after[idx];
					if (!same)
						failed = true;
					out << "\r\nsimples[" << idx << "] - " << (same ? "same" : ANSI_COLOR_RED "different" ANSI_COLOR_RESET);
				}

				for (size_t idx = 0; idx < timed.size(); idx++)
				{
					bool same = timed[idx] == timed_after[idx];
					if (!same)
						failed = true;
					out << "\r\ntimed[" << idx << "] - " << (same ? "same" : ANSI_COLOR_RED "different" ANSI_COLOR_RESET);
				}
			}
			else
			{
				out << "\r\n==============================\r\n " ANSI_COLOR_RED << name << " deserialization failed" ANSI_COLOR_RESET "\r\n";
			}
		}
	}
	else
	{
		out << "\r\n==============================\r\n " ANSI_COLOR_RED << name << " deserialization failed" ANSI_COLOR_RESET "\r\n";
		failed = true;
	}
	return !failed;
}


// Class testing::basic_tests::lib_test::external_interfaces_test 

external_interfaces_test::external_interfaces_test()
		: test_case("interfaces")
{
}


external_interfaces_test::~external_interfaces_test()
{
}



bool external_interfaces_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	ctx->set_failed();
	out << "\r\nNot Implemented yet!!!\r\n";
	return true;
}


} // namespace lib_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE
