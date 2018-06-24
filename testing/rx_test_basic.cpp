

/****************************************************************************
*
*  testing\rx_test_basic.cpp
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


// rx_test_basic
#include "testing/rx_test_basic.h"

#include "classes/rx_meta.h"
#include "sys_internal/rx_internal_ns.h"


namespace testing {

namespace basic_tests {

namespace lib_test {

// Class testing::basic_tests::lib_test::library_test_category 

library_test_category::library_test_category()
	: test_category("lib")
{
	register_test_case(rx_create_reference<platform_callback_test>());
	register_test_case(rx_create_reference<values_test>());
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


} // namespace lib_test

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



	 size_t ret = sizeof(rx::pointers::basic_smart_ptr<rx::job_ptr>);

	 auto one = rx_platform::rx_gate::instance().get_root_directory();

	 ret = sizeof(one);

	 out << "Creating test_class\r\n";
	 server_directory_ptr dir = ctx->get_current_directory();

	 rx_platform::meta::object_class_ptr test_class("test_class", 55, false);
	 test_class->complex_data().register_const_value("testBool", true);
	 test_class->complex_data().register_simple_value("testVal", 158);

	 rx_platform::meta::struct_class_ptr test_struct("test_struct_type",41,false);
	 test_struct->complex_data().register_simple_value("structVal", false);

	 test_class->complex_data().register_struct("structName", 41);

	 if (model::internal_classes_manager::instance().get_type_cache<rx_platform::meta::object_class>().register_class(test_class))
	 {
		 out << "test_class created\r\n";

		 auto rx_class_item = test_class->get_item_ptr();
		 dir->add_item(rx_class_item);
		 if (rx_class_item->generate_json(out, err))
		 {

			 if (model::internal_classes_manager::instance().get_type_cache<rx_platform::meta::struct_class>().register_class(test_struct))
			 {

				 out << "test_struct created\r\n";

				 auto rx_struct_item = test_struct->get_item_ptr();

				 dir->add_item(rx_struct_item);

				 if (rx_struct_item->generate_json(out, err))
				 {
					 out << "Creating test_object\r\n";

					 std::map<rx_node_id, std::function<rx_platform::objects::object_runtime_ptr()> > mapa;
					 mapa.emplace(55, [] {
						 return rx_create_reference<objects::user_object>();
					 });

					 auto test_object = model::internal_classes_manager::instance().get_type_cache<rx_platform::meta::object_class>().create_runtime("test_object", 59, 55);
					 if (test_object)
					 {
						 out << "test_class test_object\r\n";

						 dir->add_item(test_object->get_item_ptr());
						 if (test_object->generate_json(out, err))
							 ctx->set_passed();
						 return true;
					 }
				 }
			 }
		 }
	 }
	 ctx->set_failed();

	 return true;
 }


} // namespace meta_test

namespace lib_test {
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
	using namespace std::string_literals;

	ctx->set_failed();

	rx::values::rx_simple_value bool_before(true);
	rx::values::rx_simple_value int_before(8);
	rx::values::rx_simple_value double_before(6.7);
	rx::values::rx_simple_value str_before("string"s);

	rx::values::rx_timed_value boolt_before(false);
	rx::values::rx_timed_value intt_before(9);
	rx::values::rx_timed_value doublet_before(6.9);
	rx::values::rx_timed_value strt_before("string2"s);

	strt_before.set_time(
		doublet_before.set_time(
		intt_before.set_time(
		boolt_before.set_time(rx_time::now()))));

	out << "Created four basic values and four timed values\r\n";
	out << "\r\nbool_before=";
	bool_before.dump_to_stream(out);
	out << "\r\nint_before=";
	int_before.dump_to_stream(out);
	out << "\r\ndouble_before=";
	double_before.dump_to_stream(out);
	out << "\r\nstr_before=";
	str_before.dump_to_stream(out);
	out << "\r\nboolt_before=";
	boolt_before.dump_to_stream(out);
	out << "\r\nintt_before=";
	intt_before.dump_to_stream(out);
	out << "\r\ndoublet_before=";
	doublet_before.dump_to_stream(out);
	out << "\r\nstrt_before=";
	strt_before.dump_to_stream(out);
	out << "\r\nJSON Serialization:\r\n==============================\r\n";


	out << "JSON serialization array of values...\r\n";
	serialization::json_writter writter;

	writter.write_header(STREAMING_TYPE_VALUES);
	
	bool_before.serialize(writter);
	int_before.serialize(writter);
	double_before.serialize(writter);
	str_before.serialize(writter);
	boolt_before.serialize(writter);
	intt_before.serialize(writter);
	doublet_before.serialize(writter);
	strt_before.serialize(writter);


	if (writter.write_footer())
	{
		string_type result;
		bool succeeded = writter.get_string(result, true);

		if (succeeded)
		{
			out << result;

			rx::values::rx_simple_value bool_after;
			rx::values::rx_simple_value int_after;
			rx::values::rx_simple_value double_after;
			rx::values::rx_simple_value str_after;

			rx::values::rx_timed_value boolt_after;
			rx::values::rx_timed_value intt_after;
			rx::values::rx_timed_value doublet_after;
			rx::values::rx_timed_value strt_after;


			out << "JSON deserialization array of values...\r\n";

			serialization::json_reader reader;

			if (reader.parse_data(result))
			{
				int type;
				reader.read_header(type);

				if (type == STREAMING_TYPE_VALUES)
				{
					if (!reader.array_end())
					{
						bool_after.deserialize(reader);
						if (!reader.array_end())
						{
							int_after.deserialize(reader);
							if (!reader.array_end())
							{
								double_after.deserialize(reader);
								if (!reader.array_end())
								{
									str_after.deserialize(reader);
									if (!reader.array_end())
									{
										boolt_after.deserialize(reader);
										if (!reader.array_end())
										{
											intt_after.deserialize(reader);
											if (!reader.array_end())
											{
												doublet_after.deserialize(reader);
												if (!reader.array_end())
												{
													strt_after.deserialize(reader);
													if (reader.array_end())
													{

														out << "/r\n==============================\r\nValues deserialize:/r\n";

														out << "\r\nbool_after=";
														bool_after.dump_to_stream(out);
														out << "\r\nint_after=";
														int_after.dump_to_stream(out);
														out << "\r\ndouble_after=";
														double_after.dump_to_stream(out);
														out << "\r\nstr_after=";
														str_after.dump_to_stream(out);
														out << "\r\nboolt_after=";
														boolt_after.dump_to_stream(out);
														out << "\r\nintt_after=";
														intt_after.dump_to_stream(out);
														out << "\r\ndoublet_after=";
														doublet_after.dump_to_stream(out);
														out << "\r\nstrt_after=";
														strt_after.dump_to_stream(out);
														out << "\r\nJSON DEserialization:\r\n==============================\r\n";

														ctx->set_passed();
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return true;
}


} // namespace lib_test
} // namespace basic_tests
} // namespace testing

