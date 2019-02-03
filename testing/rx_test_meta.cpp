

/****************************************************************************
*
*  testing\rx_test_meta.cpp
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"

#ifndef EXCLUDE_TEST_CODE

// rx_test_meta
#include "testing/rx_test_meta.h"

#include "model/rx_meta.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/server/rx_server.h"


namespace testing {

namespace basic_tests {

namespace meta_test {

 // Class testing::basic_tests::meta_test::meta_model_test_category 

 meta_model_test_category::meta_model_test_category()
	 : test_category("meta")
 {
	 register_test_case(rx_create_reference<object_creation_test>());
	 register_test_case(rx_create_reference<runtime_structure_test>());
	 register_test_case(rx_create_reference<inheritance_creation_test>());
	 register_test_case(rx_create_reference<type_check_test>());
 }


 meta_model_test_category::~meta_model_test_category()
 {
 }



 // Class testing::basic_tests::meta_test::object_creation_test 

 object_creation_test::object_creation_test()
	 : test_case("construct-wide")
 {
 }
 

 object_creation_test::~object_creation_test()
 {
 }



 bool object_creation_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 auto one = rx_platform::rx_gate::instance().get_root_directory();
	 rx_node_id object_type_id;

	 out << "Creating simple variable sub-types!\r\n";
	 auto source_id = create_source_type(in, out, err, ctx);
	 if (source_id)
	 {
		 auto filter_id = create_filter_type(in, out, err, ctx);
		 if (filter_id)
		 {
			 auto event_id = create_event_type(in, out, err, ctx);
			 if (event_id)
			 {
				 out << "Creating other simple sub-types!\r\n";
				 auto mapper_id = create_mapper_type(in, out, err, ctx);
				 if (mapper_id)
				 {
					 auto variable_id = create_variable_type(in, out, err, ctx, source_id, filter_id, event_id);
					 if (variable_id)
					 {
						 auto struct_id = create_struct_type(in, out, err, ctx, variable_id);
						 if (struct_id)
						 {
							 out << "Creating object type!\r\n";
							 object_type_id = create_object_type(in, out, err, ctx, struct_id, mapper_id);
						 }
					 }
				 }
			 }
		 }
	 }
	 if (object_type_id)
	 {
		 out << "\r\nCreating test object!\r\n";
		 auto test_object = model::platform_types_manager::instance().get_type_cache<rx_platform::meta::object_types::object_type>().create_runtime("test_object", object_type_id);
		 if (test_object)
		 {
			 ctx->get_current_directory()->add_item(test_object->get_item_ptr());
			 out << "Test object created!!!\r\n";
			 if (test_object->get_item_ptr()->generate_json(out, err))
			 {
				 out << "changing initialization data for object\r\n";
				 runtime::data::runtime_values_data init_data;
				 test_object->collect_data(init_data);
				 init_data.children["variableName"].values["variableVal"].value.assign_static(113);
				 init_data.children["variableName"].values["Description"].value.assign_static(114);
				 test_object->fill_data(init_data);

				 out << "test_class test_object\r\n";
				 if (test_object->get_item_ptr()->generate_json(out, err))
				 {
					 ctx->set_passed();
					 return true;
				 }
			 }
		 }
	 }
	 
	 ctx->set_failed();
	 return true;
 }

 rx_node_id object_creation_test::create_variable_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id source_id, rx_node_id filter_id, rx_node_id event_id)
 {
	 rx_node_id id;
	 out << "Creating variable type\r\n";
	 rx_platform::meta::variable_type_ptr test_type(std::move(rx_platform::meta::type_creation_data{ "test_variable", rx_node_id::null_id, RX_CLASS_VARIABLE_BASE_ID,  false }));
	 
	 test_type->complex_data().register_simple_value_static("variableVal", false, 66.9);
	 test_type->variable_data().register_source("sourceName", source_id, test_type->complex_data());
	 test_type->variable_data().register_filter("filterName", filter_id, test_type->complex_data());
	 test_type->variable_data().register_event("eventName", event_id, test_type->complex_data());

	 if (model::platform_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_types::variable_type>().register_type(test_type))
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 ctx->get_current_directory()->add_item(rx_type_item);
		 if (rx_type_item->generate_json(out, err))
		 {
			 id = test_type->meta_data().get_id();
			 out << "Variable type created\r\n";
		 }
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_struct_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id variable_id)
 {
	 rx_node_id id;
	 out << "Creating struct type\r\n";
	 rx_platform::meta::struct_type_ptr test_type(std::move(rx_platform::meta::type_creation_data{ "test_struct", rx_node_id::null_id, RX_CLASS_STRUCT_BASE_ID,  false }));
	 test_type->complex_data().register_simple_value_static("structVal", false, false);
	 test_type->complex_data().register_variable_static("variableName", variable_id, 456u, true);
	 if (model::platform_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_types::struct_type>().register_type(test_type))
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 ctx->get_current_directory()->add_item(rx_type_item);
		 if (rx_type_item->generate_json(out, err))
		 {
			 id = test_type->meta_data().get_id();
			 out << "Struct type created\r\n";
		 }
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_object_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id struct_id, rx_node_id mapper_id)
 {
	 rx_node_id id;
	 out << "Creating object type\r\n";
	 rx_platform::meta::object_type_ptr test_type(std::move(rx_platform::meta::object_type_creation_data{ "test_object_type", rx_node_id::null_id, RX_CLASS_OBJECT_BASE_ID,  false }));
	 test_type->complex_data().register_simple_value_static("simpleVal", false, true);
	 test_type->complex_data().register_const_value_static("simpleVal", 113.5);
	 test_type->complex_data().register_struct("structName", struct_id);
	 test_type->mapping_data().register_mapper("mapperName", mapper_id, test_type->complex_data());
	 if (model::platform_types_manager::instance().get_type_cache<rx_platform::meta::object_types::object_type>().register_type(test_type))
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 ctx->get_current_directory()->add_item(rx_type_item);
		 if (rx_type_item->generate_json(out, err))
		 {
			 id = test_type->meta_data().get_id();
			 out << "Object type created\r\n";
		 }
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_filter_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 rx_node_id id;
	 out << "Creating filter type\r\n";
	 rx_platform::meta::filter_type_ptr test_type(std::move(rx_platform::meta::type_creation_data{ "test_filter", rx_node_id::null_id, RX_CLASS_FILTER_BASE_ID,  false }));
	 test_type->complex_data().register_simple_value_static("filterVal", false, false);
	 if (model::platform_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_types::filter_type>().register_type(test_type))
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 ctx->get_current_directory()->add_item(rx_type_item);
		 if (rx_type_item->generate_json(out, err))
		 {
			 id = test_type->meta_data().get_id();
			 out << "Filter type created\r\n";
		 }
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_event_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 rx_node_id id;
	 out << "Creating event type\r\n";
	 rx_platform::meta::event_type_ptr test_type(std::move(rx_platform::meta::type_creation_data{ "test_event", rx_node_id::null_id, RX_CLASS_EVENT_BASE_ID,  false }));
	 test_type->complex_data().register_simple_value_static("eventVal", false, false);
	 if (model::platform_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_types::event_type>().register_type(test_type))
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 ctx->get_current_directory()->add_item(rx_type_item);
		 if (rx_type_item->generate_json(out, err))
		 {
			 id = test_type->meta_data().get_id();
			 out << "Event type created\r\n";
		 }
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_source_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 rx_node_id id;
	 out << "Creating source type\r\n";
	 rx_platform::meta::source_type_ptr test_type(std::move(rx_platform::meta::type_creation_data{ "test_source", rx_node_id::null_id, RX_CLASS_SOURCE_BASE_ID,  false }));
	 test_type->complex_data().register_simple_value_static("sourceVal", false, false);
	 if (model::platform_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_types::source_type>().register_type(test_type))
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 ctx->get_current_directory()->add_item(rx_type_item);
		 if (rx_type_item->generate_json(out, err))
		 {
			 id = test_type->meta_data().get_id();
			 out << "Source type created\r\n";
		 }
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_mapper_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 rx_node_id id;
	 out << "Creating mapper type\r\n";
	 rx_platform::meta::mapper_type_ptr test_type(std::move(rx_platform::meta::type_creation_data{ "test_mapper", rx_node_id::null_id, RX_CLASS_MAPPER_BASE_ID,  false }));
	 test_type->complex_data().register_simple_value_static("mapperVal", false, "Test"s);
	 if (model::platform_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_types::mapper_type>().register_type(test_type))
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 ctx->get_current_directory()->add_item(rx_type_item);
		 if (rx_type_item->generate_json(out, err))
		 {
			 id = test_type->meta_data().get_id();
			 out << "Mapper type created\r\n";
		 }
	 }
	 return id;
 }


 // Class testing::basic_tests::meta_test::inheritance_creation_test 

 inheritance_creation_test::inheritance_creation_test()
	 : test_case("construct-inheritance")
 {
 }


 inheritance_creation_test::~inheritance_creation_test()
 {
 }



 bool inheritance_creation_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 ctx->set_failed();
	 return true;
 }


 // Class testing::basic_tests::meta_test::type_check_test 

 type_check_test::type_check_test()
	 : test_case("type-check")
 {
 }


 type_check_test::~type_check_test()
 {
 }



 bool type_check_test::run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 ctx->set_failed();
	 return true;
 }


} // namespace meta_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE
