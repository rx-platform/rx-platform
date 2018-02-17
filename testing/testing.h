#pragma once

namespace testing
{
	namespace obsolite
	{
		int test_smart_ptr();

		void test_thread();

		void test_callbacks();


#ifdef PYTHON_SUPPORT
		namespace python
		{
			void dump_python_information(std::ostream& out);
			void do_python_test(std::ostream& out, const string_type& command);
		}
#endif

}//namespace obsolite
}// testing
