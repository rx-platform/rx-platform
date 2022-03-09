

/****************************************************************************
*
*  testing\rx_test_meta.cpp
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

// rx_test_meta
#include "testing/rx_test_meta.h"

#include "model/rx_meta_internals.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/server/rx_server.h"
#include "model/rx_model_algorithms.h"
#include "system/runtime/rx_holder_algorithms.h"
#include "api/rx_namespace_api.h"
#include "system/server/rx_file_helpers.h"
#include "system/server/rx_directory_cache.h"


namespace testing {

namespace basic_tests {

namespace meta_test {
#define RX_TEST_DIRECTORY "/test"
namespace
{
template<typename T>
rx_result_with<typename T::smart_ptr> create_prototype(const string_type& type_name, const string_type& base_reference)
{
	string_type path(RX_TEST_DIRECTORY);
	string_type name = type_name;
	rx_platform::ns::rx_directory_resolver resolver;
	resolver.add_paths({ path });
	string_type full_path = rx_combine_paths(path, name);
	rx_split_path(full_path, path, name);
	object_type_creation_data data;
	data.name = name;
	data.attributes = namespace_item_attributes::namespace_item_full_access;
	data.path = path;

	auto base_resolved = api::ns::rx_resolve_type_reference<T>(rx_item_reference(base_reference), resolver, tl::type2type<T>());
	if (!base_resolved)
	{
		base_resolved.register_error("Error resolving base reference:\r\n");
		return base_resolved.errors();
	}
	data.base_id = base_resolved.move_value();
	auto ret = rx_create_reference<T>();
	ret->meta_info = meta_data(data);
	return ret;
}
template<typename T>
rx_result_with<typename T::smart_ptr> create_simple_prototype(const string_type& type_name, const string_type& base_reference)
{
	string_type path(RX_TEST_DIRECTORY);
	string_type name = type_name;
	rx_platform::ns::rx_directory_resolver resolver;
	resolver.add_paths({ path });
	string_type full_path = rx_combine_paths(path, name);
	rx_split_path(full_path, path, name);
	type_creation_data data;
	data.name = name;
	data.attributes = namespace_item_attributes::namespace_item_full_access;
	data.path = path;

	auto base_resolved = api::ns::rx_resolve_simple_type_reference<T>(base_reference, resolver, tl::type2type<T>());
	if (!base_resolved)
	{
		base_resolved.register_error("Error resolving base reference:\r\n");
		return base_resolved.errors();
	}
	data.base_id = base_resolved.move_value();
	auto ret = rx_create_reference<T>();
	ret->meta_info = meta_data(data);
	return ret;
}
template<typename T>
typename T::instance_data_t create_runtime_prototype(const string_type& rt_name, const string_type& base_reference)
{
	using definition_data_type = typename T::instance_data_t;

	string_type path(RX_TEST_DIRECTORY);
	string_type name = rt_name;
	rx_platform::ns::rx_directory_resolver resolver;
	resolver.add_paths({ path });
	string_type full_path = rx_combine_paths(path, name);
	rx_split_path(full_path, path, name);
	definition_data_type data;
	data.meta_info.name = name;
	data.meta_info.attributes = namespace_item_attributes::namespace_item_full_access;
	data.meta_info.path = path;

	auto base_resolved = api::ns::rx_resolve_type_reference<T>(base_reference, resolver, tl::type2type<T>());
	if (base_resolved)
		data.meta_info.parent = base_resolved.move_value();
	return data;
}
}

 // Class testing::basic_tests::meta_test::meta_model_test_category 

 meta_model_test_category::meta_model_test_category()
	 : test_category("meta")
 {
	 register_test_case(rx_create_reference<object_creation_test>());
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
	 auto one = rx_platform::rx_gate::instance().get_directory("/");
	 rx_node_id object_type_id;

	 out << ANSI_COLOR_YELLOW "Creating simple variable sub-types!\r\n" ANSI_COLOR_RESET;
	 auto source_id = create_source_type(in, out, err, ctx);
	 if (source_id)
	 {
		 auto filter_id = create_filter_type(in, out, err, ctx);
		 if (filter_id)
		 {
			 auto event_id = create_event_type(in, out, err, ctx);
			 if (event_id)
			 {
				 out << ANSI_COLOR_YELLOW "Creating other simple sub-types!\r\n" ANSI_COLOR_RESET;
				 auto mapper_id = create_mapper_type(in, out, err, ctx);
				 if (mapper_id)
				 {
					 auto variable_id = create_variable_type(in, out, err, ctx, source_id, filter_id, event_id);
					 if (variable_id)
					 {
						 auto struct_id = create_struct_type(in, out, err, ctx, variable_id);
						 if (struct_id)
						 {
							 out << ANSI_COLOR_YELLOW "Creating object type!\r\n" ANSI_COLOR_RESET;
							 object_type_id = create_object_type(in, out, err, ctx, struct_id, mapper_id);
						 }
					 }
				 }
			 }
		 }
	 }
	 if (object_type_id)
	 {
		 out << ANSI_COLOR_YELLOW "\r\nCreating test object!\r\n" ANSI_COLOR_RESET;
		 auto def_data = create_runtime_prototype<object_type>("test_object", "test_object_type");
		 auto test_result = rx_internal::model::algorithms::runtime_model_algorithm<object_type>::create_runtime_sync(std::move(def_data), data::runtime_values_data());
		 if (test_result)
		 {
			 rx_object_ptr test_object=test_result.value();
			 ns::rx_directory_cache::instance().get_directory(ctx->get_current_directory())->add_item(test_object->get_item_ptr());
			 out << ANSI_COLOR_YELLOW "Test object created!!!\r\n" ANSI_COLOR_RESET;
			 auto json_str = test_object->get_item_ptr()->get_definition_as_json();
			 if (!json_str.empty())
			 {
				 out << json_str;
				 out << ANSI_COLOR_YELLOW "changing initialization data for object\r\n" ANSI_COLOR_RESET;
				 data::runtime_values_data init_data;
				 runtime::algorithms::runtime_holder_algorithms<object_type>::collect_data(init_data, runtime_value_type::simple_runtime_value, *test_object);
				 init_data.children["structName"].values["structVal"].value.assign_static(113);
				 runtime::algorithms::runtime_holder_algorithms<object_type>::fill_data(init_data, *test_object);

				 out << ANSI_COLOR_YELLOW "Dumping test_object\r\n" ANSI_COLOR_RESET;
				 json_str = test_object->get_item_ptr()->get_definition_as_json();
				 if (!json_str.empty())
				 {
					 out << json_str;
					 ctx->set_passed();
					 return true;
				 }
			 }
		 }
		 else
		 {
			 out << "Error creating object\r\n";
			 dump_error_result(out, test_result);
		 }
	 }
	 
	 ctx->set_failed();
	 return true;
 }

 rx_node_id object_creation_test::create_variable_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id source_id, rx_node_id filter_id, rx_node_id event_id)
 {
	 rx_node_id id;
	 out << "Creating variable type\r\n";
	 auto proto_result = create_simple_prototype<variable_type>("test_variable", "VariableBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("variableVal", 66.9, false, false);
	 test_type->complex_data.register_event(event_attribute("eventName", event_id));
	 test_type->mapping_data.register_source("sourceName", source_id, test_type->complex_data);
	 test_type->variable_data.register_filter("filterName", filter_id, test_type->complex_data);
	 auto result = rx_internal::model::algorithms::simple_types_model_algorithm<variable_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 ns::rx_directory_cache::instance().get_directory(ctx->get_current_directory())->add_item(std::move(rx_type_item));
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Variable type created\r\n";
		 }
	 }
	 else
	 {
		 err << "Error creating derived object type\r\n";
		 dump_error_result(out, result);
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_struct_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id variable_id)
 {
	 rx_node_id id;
	 out << "Creating struct type\r\n";
	 auto proto_result = create_simple_prototype<rx_platform::meta::basic_types::struct_type>("test_struct", "StructBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("structVal", false, false, false);
	 test_type->complex_data.register_variable_static("variableName", variable_id, 456u, true, false);
	 auto result = rx_internal::model::algorithms::simple_types_model_algorithm<rx_platform::meta::basic_types::struct_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 ns::rx_directory_cache::instance().get_directory(ctx->get_current_directory())->add_item(std::move(rx_type_item));
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Struct type created\r\n";
		 }
	 }
	 else
	 {
		 out << "Error creating derived object type\r\n";
		 dump_error_result(out, result);
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_object_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id struct_id, rx_node_id mapper_id)
 {
	 rx_node_id id;
	 out << "Creating object type\r\n";
	 auto proto_result = create_prototype<object_type>("test_object_type", "ObjectBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("simpleVal", true, false, false);
	 test_type->complex_data.register_const_value_static("constVal", 113.5);
	 test_type->complex_data.register_struct("structName", struct_id);
	 test_type->mapping_data.register_mapper("mapperName", mapper_id, test_type->complex_data);
	 auto result = rx_internal::model::algorithms::types_model_algorithm<object_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = result.value()->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Object type created\r\n";
		 }
	 }
	 else
	 {
		 out << "Error creating derived object type\r\n";
		 dump_error_result(out, result);
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_filter_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 rx_node_id id;
	 out << "Creating filter type\r\n";
	 auto proto_result = create_simple_prototype<rx_platform::meta::basic_types::filter_type>("test_filter", "FilterBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("filterVal", false, false, false);
	 auto result = rx_internal::model::algorithms::simple_types_model_algorithm<rx_platform::meta::basic_types::filter_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Filter type created\r\n";
		 }
	 }
	 else
	 {
		 out << "Error creating filter type\r\n";
		 dump_error_result(out, result);
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_event_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 rx_node_id id;
	 out << "Creating event type\r\n";
	 auto proto_result = create_simple_prototype<rx_platform::meta::basic_types::event_type>("test_event", "EventBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("eventVal", false, false, false);
	 auto result = rx_internal::model::algorithms::simple_types_model_algorithm<rx_platform::meta::basic_types::event_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Event type created\r\n";
		 }
	 }
	 else
	 {
		 out << "Error creating event type\r\n";
		 dump_error_result(out, result);
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_source_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 rx_node_id id;
	 out << "Creating source type\r\n";
	 auto proto_result = create_simple_prototype<source_type>("test_source", "SourceBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("sourceVal", false, false, false);
	 auto result = rx_internal::model::algorithms::simple_types_model_algorithm<source_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Source type created\r\n";
		 }
	 }
	 else
	 {
		 out << "Error creating derived object type\r\n";
		 dump_error_result(out, result);
	 }
	 return id;
 }

 rx_node_id object_creation_test::create_mapper_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 rx_node_id id;
	 out << "Creating mapper type\r\n";
	 auto proto_result = create_simple_prototype<mapper_type>("test_mapper", "MapperBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("mapperVal", "Test", false, false);
	 auto result = rx_internal::model::algorithms::simple_types_model_algorithm<mapper_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Mapper type created\r\n";
		 }
	 }
	 else
	 {
		 out << "Error creating derived object type\r\n";
		 dump_error_result(out, result);
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
	 auto one = rx_platform::rx_gate::instance().get_directory("/");
	 rx_node_id object_type_id;
	 
	out << ANSI_COLOR_YELLOW "Creating simple sub-types!\r\n" ANSI_COLOR_RESET;
	
	auto variable_id = create_variable_type(in, out, err, ctx);
	if (variable_id)
	{
		auto struct_id = create_struct_type(in, out, err, ctx, variable_id);
		if (struct_id)
		{
			out << ANSI_COLOR_YELLOW "Creating base object type!\r\n" ANSI_COLOR_RESET;
			auto base_object_type_id = create_base_object_type(in, out, err, ctx, struct_id);
			if (base_object_type_id)
			{
				out << ANSI_COLOR_YELLOW "Creating derived object type!\r\n" ANSI_COLOR_RESET;
				object_type_id = create_derived_object_type(in, out, err, ctx, base_object_type_id, variable_id);
			}
		}
	}

	 if (object_type_id)
	 {
		 out << ANSI_COLOR_YELLOW "\r\nCreating test object!\r\n" ANSI_COLOR_RESET;
		 auto def_data = create_runtime_prototype<object_type>("inh_test_object", "derived_test_object_type");
		 auto test_result = rx_internal::model::algorithms::runtime_model_algorithm<object_type>::create_runtime_sync(std::move(def_data), data::runtime_values_data());
		 if (test_result)
		 {
			 auto test_object = test_result.value()->get_item_ptr();
			 auto json_str = test_object->get_definition_as_json();
			 ns::rx_directory_cache::instance().get_directory(ctx->get_current_directory())->add_item(std::move(test_object));
			 out << ANSI_COLOR_YELLOW "Test object created!!!\r\n" ANSI_COLOR_RESET;
			 if (!json_str.empty())
			 {				
				 out << json_str;
				ctx->set_passed();
				return true;
			 }
		 }
	 }

	 ctx->set_failed();
	 return true;
 }

 rx_node_id inheritance_creation_test::create_base_object_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id struct_id)
 {
	 rx_node_id id;
	 out << "Creating object type\r\n";
	 auto proto_result = create_prototype<object_type>("base_test_object_type", "ObjectBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_const_value_static("constVal", true);
	 test_type->complex_data.register_simple_value_static("simpleVal", 113.5, false, false);
	 test_type->complex_data.register_struct("structName", struct_id);
	 auto result = rx_internal::model::algorithms::types_model_algorithm<object_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Object type created\r\n";
		 }
	 }
	 else
	 {
		 out << "Error creating derived object type\r\n";
		 dump_error_result(out, result);
	 }
	 return id;
 }

 rx_node_id inheritance_creation_test::create_struct_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id variable_id)
 {
	 rx_node_id id;
	 out << "Creating struct type\r\n";
	 auto proto_result = create_simple_prototype<rx_platform::meta::basic_types::struct_type>("inh_test_struct", "StructBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("structVal", false, false, false);
	 test_type->complex_data.register_variable_static("variableName", variable_id, 456u, true, false);
	 auto result = rx_internal::model::algorithms::simple_types_model_algorithm<struct_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Struct type created\r\n";
		 }
	 }
	 return id;
 }

 rx_node_id inheritance_creation_test::create_variable_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx)
 {
	 rx_node_id id;
	 out << "Creating variable type\r\n";
	 auto proto_result = create_simple_prototype<variable_type>("inh_test_variable", "VariableBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("variableVal", 66.9, false, false);

	 auto result = rx_internal::model::algorithms::simple_types_model_algorithm<variable_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Variable type created\r\n";
		 }
	 }
	 return id;
 }

 rx_node_id inheritance_creation_test::create_derived_object_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id base_id, rx_node_id variable_id)
 {
	 rx_node_id id;
	 out << "Creating object type\r\n";
	 auto proto_result = create_prototype<object_type>("derived_test_object_type", "base_test_object_type");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_variable_static("simpleVal", variable_id, 114.8, false, false);
	 auto result = rx_internal::model::algorithms::types_model_algorithm<object_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Object type created\r\n";
		 }
	 }
	 else
	 {
		 out << "Error creating derived object type\r\n";
		 dump_error_result(out, result);
	 }
	 return id;
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
	 // static one, rejected by the hashes
	 static auto fake_mapper_id = rx_node_id::generate_new();
	 // local one rejected by namespace
	 auto fake_variable_id = rx_node_id::generate_new();
	 // name based rejected by namespace
	 static string_type fake_name = "default_fake";

	 out << ANSI_COLOR_YELLOW "Generated non-existing node ids:\r\n" ANSI_COLOR_RESET;
	 out << "Mapper Id:" << fake_mapper_id << "\r\n";
	 out << "Variable Id:" << fake_variable_id << "\r\n";
	 out << ANSI_COLOR_YELLOW "Registering Struct with fake_variable_id...\r\n" ANSI_COLOR_RESET;
	 rx_node_id struct_id;
	 auto struct_result = create_struct_type(in, out, err, ctx, fake_variable_id, struct_id);
	 if (!struct_result)
	 {
		 out << ANSI_COLOR_YELLOW "registering struct failed:\r\n" ANSI_COLOR_RESET;
		 for (const auto& one : struct_result.errors())
		 {
			 out << one << "\r\n";
		 }
		 ctx->set_passed();
		 return true;
	 }
	 auto object_id = create_object_type(in, out, err, ctx, struct_id, fake_mapper_id);


	 out << ANSI_COLOR_YELLOW "Checking invalid Object Type\r\n" ANSI_COLOR_RESET;
	 meta::type_check_context check_ctx;
	 rx_result result = rx_internal::model::platform_types_manager::instance().get_type_repository<rx_platform::meta::object_types::object_type>().check_type(object_id, check_ctx);
	 if (!result)
	 {
		 out << "check returned following errors:\r\n";
		 for (const auto& one : check_ctx.get_records())
		 {
			 out << one.text << "\r\n";
		 }
		 out << ANSI_COLOR_YELLOW "Checking invalid Struct Type\r\n" ANSI_COLOR_RESET;
		 check_ctx.reinit();
		 if (!rx_internal::model::platform_types_manager::instance().get_simple_type_repository<rx_platform::meta::basic_types::struct_type>().check_type(struct_id, check_ctx))
		 {
			 out << "check returned following errors:\r\n";
			 for (const auto& one : check_ctx.get_records())
			 {
				 out << one.text << "\r\n";
			 }

			 auto def_data = create_runtime_prototype<object_type>("shouldnt_create", "check_test_object_type");
			 auto obj = rx_internal::model::algorithms::runtime_model_algorithm<object_type>::create_runtime_sync(std::move(def_data), data::runtime_values_data());
			 if (!obj)
			 {
				 out << ANSI_COLOR_YELLOW "create returned following errors:\r\n" ANSI_COLOR_RESET;
				 for (const auto& one : check_ctx.get_records())
				 {
					 out << one.text << "\r\n";
				 }
				 ctx->set_passed();
				 return true;
			 }
			 else
			 {
				 out << "Object created - ERROR!!!\r\n";
			 }
		 }
		 else
		 {
			 out << "Struct checked O.K. - ERROR!!!\r\n";
		 }
	 }
	 else
	 {
		 out << "Object checked O.K. - ERROR!!!\r\n";
	 }
	 ctx->set_failed();
	 return true;
 }

 rx_node_id type_check_test::create_object_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id struct_id, rx_node_id mapper_id)
 {
	 rx_node_id id;
	 out << "Creating object type\r\n";
	 auto proto_result = create_prototype<object_type>("check_test_object_type", "/_sys/types/base/ObjectBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("simpleVal", true, false, false);
	 test_type->complex_data.register_const_value_static("simpleVal", 113.5);
	 test_type->complex_data.register_struct("structName", struct_id);
	 test_type->mapping_data.register_mapper("mapperName", mapper_id, test_type->complex_data);
	 auto result = rx_internal::model::algorithms::types_model_algorithm<object_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Object type created\r\n";
		 }
	 }
	 else
	 {
		 out << "Error creating derived object type\r\n";
		 dump_error_result(out, result);
	 }
	 return id;
 }

 rx_result type_check_test::create_struct_type (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx, rx_node_id variable_id, rx_node_id& struct_id)
 {
	 rx_node_id id;
	 out << "Creating struct type\r\n";
	 auto proto_result = create_simple_prototype<rx_platform::meta::basic_types::struct_type>("check_test_struct", "StructBase");
	 if (!proto_result)
	 {
		 err << "Error creating prototype!\r\n";
		 dump_error_result(out, proto_result);
	 }
	 auto test_type = proto_result.value();
	 test_type->complex_data.register_simple_value_static("structVal", false, false, false);
	 test_type->complex_data.register_variable_static("variableName", variable_id, 'a', true, false);
	 auto result = rx_internal::model::algorithms::simple_types_model_algorithm<struct_type>::create_type_sync(test_type);
	 if (result)
	 {
		 auto rx_type_item = test_type->get_item_ptr();
		 auto json_str = rx_type_item->get_definition_as_json();
		 if (!json_str.empty())
		 {
			 out << json_str;
			 id = test_type->meta_info.id;
			 out << "Struct type created\r\n";
		 }
	 }
	 else
	 {
		 return "failed to register struct_type with failed node_id!";
	 }
	 struct_id=id;
	 return true;
 }


} // namespace meta_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE
