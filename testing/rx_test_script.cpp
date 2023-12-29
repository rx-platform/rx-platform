

/****************************************************************************
*
*  testing\rx_test_script.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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

// rx_test_script
#include "testing/rx_test_script.h"

#include "terminal/rx_terminal_style.h"
#include "system/server/rx_server.h"
#include "system/server/rx_file_helpers.h"


namespace testing {

namespace script_test {

namespace py_test {
void dump_python_information(std::ostream& out)
{
#ifdef PYTHON_SUPPORT
	out << "Python information\r\n==============================\r\n";
	out << "Version:\r\n" << Py_GetVersion() << "\r\n";
	out << "\r\n" << Py_GetCopyright() << "\r\n\r\n";
#endif
}

void do_python_test(std::ostream& out, const string_type& command)
{
#ifdef PYTHON_SUPPORT
	PyRun_SimpleString(command.c_str());
#endif
}

// Class testing::script_test::py_test::python_test 

python_test::python_test()
	: test_category("python")
{
	register_test_case(rx_create_reference<dump_version_test>());
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



bool dump_version_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	dump_python_information(out);
	ctx->set_passed();
	return true;
}


} // namespace py_test

namespace rxs {

// Class testing::script_test::rxs::rx_script_category 

rx_script_category::rx_script_category()
	: test_category("rx-script")
{

	register_test_case(rx_create_reference<read_and_run_file>());
}


rx_script_category::~rx_script_category()
{
}



// Class testing::script_test::rxs::read_and_run_file 

read_and_run_file::read_and_run_file()
	: test_case("file-run")
{
}


read_and_run_file::~read_and_run_file()
{
}



bool read_and_run_file::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
{
	bool ret = false;
	string_type file_name;
	in >> file_name;
	if (file_name.empty())
	{
		out << "\r\n" << get_help() << "\r\n";
		return true;
	}
	else
	{
		file_name = "rx-script-1.rxs";
		auto storage = rx_gate::instance().get_host()->get_test_storage();
		if (storage)
		{
			sys_handle_t file = 0;// storage->get_host_test_file(file_name);
			if (file)
			{

				memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer(pointers::_create_new);
				//if (buffer->fill_with_file_content(file))
				{
					out << "file loaded in memory...\r\n";
					out << "Running file script:" << file_name;
					out << "\r\n=====================================\r\n";

					while (!buffer->eof())
					{
						string_type line;
						buffer->read_line(line);
						out << "\r\n" << ANSI_COLOR_GREEN ">>>" ANSI_COLOR_RESET << line << "\r\n";

						//TODOIO
						/*prog::server_console_program temp_prog(line);

						prog::program_context_base_ptr ctx_script = temp_prog.create_program_context(
							prog::server_program_holder_ptr::null_ptr,
							prog::program_context_base_ptr::null_ptr,
							ctx->get_current_directory(),
							ctx->get_out(),
							ctx->get_err(),
							prog::console_client::smart_ptr::null_ptr);

						bool ret = temp_prog.process_program(ctx_script, rx_time::now(), false);
						if (!ret)
						{
							ret = false;
							break;
						}*/
						ret = true;
					}

					out << RX_CONSOLE_HEADER_LINE "\r\nScript done.\r\n";
				}
				//else
				{
					err << "error reading file content\r\n";
				}
				rx_file_close(file);
				ctx->set_passed();
				return ret;
			}
		}
		return true;
	}
}


} // namespace rxs
} // namespace script_test
} // namespace testing

#endif //EXCLUDE_TEST_CODE
