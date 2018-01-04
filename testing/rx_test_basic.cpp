

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

#include "classes/rx_meta.h"


namespace testing {

namespace basic_tests {

namespace function_test {

// Class testing::basic_tests::function_test::function_test_category

function_test_category::function_test_category()
	: test_category("functions")
{
	register_test_case(rx_create_reference<platform_callback_test>());
}


function_test_category::~function_test_category()
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
	test_base_class base_object;
	test_derived_class derived_object;

	callback::callback_functor_container<locks::slim_lock, std::ostream> container;

	out << "Adding derived_object.stuff_heapend_diff2\r\n";
	callback::callback_handle_t handle1 = container.register_callback(std::bind(&test_derived_class::stuff_heapend_diff2, &derived_object, _1, _2));
	out << "Adding base_object.stuff_heapend_diff\r\n";
	callback::callback_handle_t handle2 = container.register_callback(&base_object, &test_base_class::stuff_heapend_diff);
	out << "Adding derived_object.stuff_heapend\r\n";
	callback::callback_handle_t handle3 = container.register_callback(&derived_object, &test_derived_class::stuff_heapend);
	out << "Adding base_object.stuff_heapend\r\n";
	callback::callback_handle_t handle4 = container.register_callback(&base_object, &test_base_class::stuff_heapend);
	out << "Adding derived_object.stuff_heapend_virtual\r\n";
	callback::callback_handle_t handle5 = container.register_callback(&derived_object, &test_derived_class::stuff_heapend_virtual);
	out << "Adding base_object.stuff_heapend_virtual\r\n";
	callback::callback_handle_t handle6 = container.register_callback(&base_object, &test_base_class::stuff_heapend_virtual);
	out << "Adding static_callback\r\n";
	callback::callback_handle_t handle7 = container.register_callback(&static_callback);

	out << "Firing Callbacks\r\n";
	container(out, 0);

	out << "Removing derived_object.stuff_heapend\r\n";
	container.unregister_callback(handle3);
	out << "Removing base_object.stuff_heapend\r\n";
	container.unregister_callback(handle4);
	out << "Removing static_callback\r\n";
	container.unregister_callback(handle7);

	out << "Firing Callbacks\r\n";
	container(out, 0);

	out << "Removing derived_object.stuff_heapend_diff2\r\n";
	container.unregister_callback(handle1);
	out << "Removing base_object.stuff_heapend_diff\r\n";
	container.unregister_callback(handle2);
	out << "Removing derived_object.stuff_heapend_virtual\r\n";
	container.unregister_callback(handle5);
	out << "Removing base_object.stuff_heapend_virtual\r\n";
	container.unregister_callback(handle6);

	out << "Firing Callbacks\r\n";
	container(out, 0);
	out << "\r\n";
}

// Class testing::basic_tests::function_test::platform_callback_test

platform_callback_test::platform_callback_test()
	: test_case("rx-callback")
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


} // namespace function_test

namespace meta_test {

 // Class testing::basic_tests::meta_test::meta_model_test_category

 meta_model_test_category::meta_model_test_category()
	 : test_category("meta")
 {
	 register_test_case(rx_create_reference<object_creation_test>());
 }


 meta_model_test_category::~meta_model_test_category()
 {
 }



 // Class testing::basic_tests::meta_test::object_creation_test

 object_creation_test::object_creation_test()
	 : test_case("construct")
 {
 }


 object_creation_test::~object_creation_test()
 {
 }



 bool object_creation_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {

	 out << "Creating test_class\r\n";
	 server_directory_ptr dir = ctx->get_current_directory();

	 rx_platform::meta::object_class_ptr test_class("test_class", 55, false);
	 test_class->register_const_value("testBool", true);
	 test_class->register_simple_value("testVal", 158);

	 rx_platform::meta::struct_class_ptr test_struct("test_struct_type",41,false);
	 test_struct->register_simple_value("structVal", false);

	 test_class->register_struct("structName", 41);

	 if (model::internal_classes_manager::instance().get_type_cache<rx_platform::meta::object_class>().register_class(test_class))
	 {
		 out << "test_class created\r\n";

		 dir->add_item(test_class);

		 if (model::internal_classes_manager::instance().get_type_cache<rx_platform::meta::struct_class>().register_class(test_struct))
		 {

			 out << "test_struct created\r\n";

			 dir->add_item(test_struct);


			 out << "Creating test_object\r\n";

			 constructors::user_object_constructor constructor;
			 rx_platform::objects::object_runtime_ptr test_object = constructor.create_object("test_object", 59, 55);
			 if (test_object)
			 {
				 out << "test_class test_object\r\n";

				 dir->add_item(test_object);
				 ctx->set_passed();
				 return true;
			 }
		 }
	 }
	 ctx->set_failed();

	 return true;
 }


} // namespace meta_test
} // namespace basic_tests
} // namespace testing

