#pragma once

namespace testing
{

int test_smart_ptr();

void test_thread();

void test_callbacks();


#ifndef NO_PYTHON_SUPPORT
namespace python
{
void dump_python_information(std::ostream& out);
}
#endif

}// testing