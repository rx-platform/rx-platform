

/****************************************************************************
*
*  testing\rx_test_basic.cpp
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


// rx_test_basic
#include "testing/rx_test_basic.h"



namespace testing {

namespace basic_tests {

namespace function_test {

// Class testing::basic_tests::function_test::function_test_category 

function_test_category::function_test_category()
	: test_category("function-objects")
{
	register_test_case(rx_create_reference<platform_callback_test>());
}


function_test_category::~function_test_category()
{
}



class dummy_class1
{
public:
	void stuff_heapend(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) <<"same 1\r\n";
	}
	virtual void stuff_heapend_virtual(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) <<"virt 1\r\n";
	}
	void stuff_heapend_diff(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) <<"diff 1\r\n";
	}
};
class dummy_class2 : public dummy_class1
{
public:
	void stuff_heapend(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out)<<"same 2\r\n";
	}
	void stuff_heapend_virtual(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) <<"virt 2\r\n";
	}

	void stuff_heapend_diff2(const std::ostream& out, callback::callback_state_t state)
	{
		const_cast<std::ostream&>(out) <<"diff 2\r\n";
	}
};

class dummy
{
public:
	void dummy_method(std::ostream& out)
	{

	}
};


void static_callback(const std::ostream& out, callback::callback_state_t state)
{
	const_cast<std::ostream&>(out) <<"static callback";
}


typedef decltype(std::mem_fn(&dummy::dummy_method)) element_type;

void callback_func(const std::ostream& out, uint32_t state)
{
	const_cast<std::ostream&>(out) <<"Callback occured!!!";
}

typedef void(dummy::*callback_t)(const int&);
void test_callbacks(std::ostream& out)
{
	dummy_class1 obj1;
	dummy_class2 obj2;
	


	auto var = std::mem_fn(&dummy_class2::stuff_heapend_diff2);

	size_t sz = sizeof(var);

	uint8_t* test = new uint8_t[sz];

	memcpy(test, test, sz);

	callback::callback_functor_container<locks::slim_lock, std::ostream> container;
	callback::callback_handle_t handle1 = container.register_callback(std::bind(&dummy_class2::stuff_heapend_diff2, &obj2, _1, _2));
	callback::callback_handle_t handle2 = container.register_callback(&obj1, &dummy_class1::stuff_heapend_diff);


	callback::callback_handle_t handle3 = container.register_callback(&obj2, &dummy_class2::stuff_heapend);
	callback::callback_handle_t handle4 = container.register_callback(&obj1, &dummy_class1::stuff_heapend);


	callback::callback_handle_t handle5 = container.register_callback(&obj2, &dummy_class2::stuff_heapend_virtual);
	callback::callback_handle_t handle6 = container.register_callback(&obj1, &dummy_class1::stuff_heapend_virtual);

	callback::callback_handle_t handle7 = container.register_callback(&static_callback);

	container(out, 0);

	container.unregister_callback(handle3);
	container.unregister_callback(handle4);
	container.unregister_callback(handle7);

	container(out, 0);


	container.unregister_callback(handle1);
	container.unregister_callback(handle2);
	container.unregister_callback(handle5);
	container.unregister_callback(handle6);

}


// Class testing::basic_tests::function_test::platform_callback_test 

platform_callback_test::platform_callback_test()
	: test_case("rx-callback")
{
}


platform_callback_test::~platform_callback_test()
{
}



bool platform_callback_test::do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx)
{
	test_callbacks(out);
	return true;
}


} // namespace function_test
} // namespace basic_tests
} // namespace testing

