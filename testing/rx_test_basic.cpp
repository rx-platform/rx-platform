

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


#include "pch.h"


// rx_test_basic
#include "testing/rx_test_basic.h"

#include "model/rx_meta.h"
#include "sys_internal/rx_internal_ns.h"
using namespace std::string_literals;


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
		bool failed = false;

		std::vector<rx::values::rx_simple_value> simples(4);
		simples[0].assign_static(true);
		simples[1].assign_static(8);
		simples[2].assign_static(6.7);
		simples[3].assign_static("string"s);

		rx_time now = rx::rx_time::now();

		std::vector<rx::values::rx_timed_value> timed(4);
		timed[0].assign_static(false, now);
		timed[1].assign_static(9, now);
		timed[2].assign_static(6.8, now);
		timed[3].assign_static("string2"s, now);


		std::vector<rx::values::rx_value> fulls;

		out << ANSI_COLOR_GREEN "Created four simple values and four timed values in an std::vector\r\n" ANSI_COLOR_RESET;

		out << "\r\nsimple values" RX_TESTING_CON_LINE;
		for (size_t idx = 0; idx < simples.size(); idx++)
		{
			out << "\r\nsimple[" << idx << "]=";
			simples[idx].dump_to_stream(out);
		}

		out << "\r\ntimed values" RX_TESTING_CON_LINE;
		for (size_t idx = 0; idx < timed.size(); idx++)
		{
			out << "\r\ntimed[" << idx << "]=";
			timed[idx].dump_to_stream(out);
		}

		serialization::json_writer writter;


		if (test_serialization("JSON", simples, timed, fulls, writter, out))
		{
			string_type result;
			if (writter.get_string(result, true))
			{
				out << RX_TESTING_CON_LINE;
				out << result;
				out << RX_TESTING_CON_LINE;
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
		memory::std_buffer buffer;
		serialization::std_buffer_writer bwriter(buffer);
		if (test_serialization("Binary", simples, timed, fulls, bwriter, out))
		{
			out << RX_TESTING_CON_LINE;
			bwriter.dump_to_stream(out);
			out << RX_TESTING_CON_LINE;
			serialization::std_buffer_reader reader(buffer);
			if (test_deserialization("Binary", simples, timed, fulls, reader, out))
			{
				failed = false;
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
		one.serialize(writer);
	for (const auto& one : timed)
		one.serialize(writer);

	if (writer.write_footer())
	{
		out << "\r\n==============================\r\n " ANSI_COLOR_GREEN << name << ANSI_COLOR_RESET " serialization succeeded";
		return true;
	}
	else return false;
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
				if (!one.deserialize(reader))
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
				if (!one.deserialize(reader))
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
		}
	}
	else
		failed = true;

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

	 auto one = rx_platform::rx_gate::instance().get_root_directory();


	 out << "Creating test_class\r\n";
	 server_directory_ptr dir = ctx->get_current_directory();

	 rx_platform::meta::object_class_ptr test_class(object_defs::object_type_creation_data{ "test_class", 55, RX_CLASS_OBJECT_BASE_ID, false });
	 test_class->complex_data().register_const_value_static("testBool", true);
	 test_class->complex_data().register_simple_value_static("testVal", false, 158);

	 rx_platform::meta::struct_class_ptr test_struct(std::move(rx_platform::meta::basic_defs::type_creation_data{ "test_struct_type",41, RX_CLASS_STRUCT_BASE_ID,  false }));
	 test_struct->complex_data().register_simple_value_static("structVal", false, false);

	 test_class->complex_data().register_struct("structName", 41);

	 if (model::internal_types_manager::instance().get_type_cache<rx_platform::meta::object_defs::object_class>().register_class(test_class))
	 {
		 out << "test_class created\r\n";

		 auto rx_class_item = test_class->get_item_ptr();
		 dir->add_item(rx_class_item);
		 if (rx_class_item->generate_json(out, err))
		 {

			 if (model::internal_types_manager::instance().get_type_cache<rx_platform::meta::basic_defs::struct_class>().register_class(test_struct))
			 {

				 out << "test_struct created\r\n";

				 auto rx_struct_item = test_struct->get_item_ptr();

				 dir->add_item(rx_struct_item);

				 if (rx_struct_item->generate_json(out, err))
				 {
					 out << "Creating test_object\r\n";

					 std::map<rx_node_id, std::function<rx_platform::runtime::object_runtime_ptr()> > map;
					 map.emplace(55, [] {
						 return rx_create_reference<runtime::object_types::user_object>();
					 });

					 auto test_object = model::internal_types_manager::instance().get_type_cache<rx_platform::meta::object_defs::object_class>().create_runtime("test_object", 55);
					 if (test_object)
					 {
						 out << "test_class test_object\r\n";

						 dir->add_item(test_object->get_item_ptr());
						 if (test_object->get_item_ptr()->generate_json(out, err))
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
} // namespace basic_tests
} // namespace testing

