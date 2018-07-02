

/****************************************************************************
*
*  system\meta\rx_classes.cpp
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

#include "rx_configuration.h"

// rx_classes
#include "system/meta/rx_classes.h"
// rx_objbase
#include "system/meta/rx_objbase.h"

#include "system/meta/rx_obj_classes.h"
#include "sys_internal/rx_internal_ns.h"
#include "classes/rx_meta.h"
#include "system/constructors/rx_construct.h"
#include "rx_objbase.h"
using namespace rx;


namespace rx_platform {

namespace meta {


class meta_helpers
{
public:
	template<class complexT>
	static bool serialize_complex_class(const complexT& whose, base_meta_writter& stream, uint8_t type)
	{
		if (!whose.meta_data_.serialize_checkable_definition(stream, type))
			return false;
		if (!whose.complex_data_.serialize_complex_definition(stream, type))
			return false;
		return true;
	}

	template<class complexT>
	static bool deserialize_complex_class(complexT& whose, base_meta_reader& stream, uint8_t type)
	{
		return false;
	}


	template<class complexT>
	static bool serialize_struct_class(const complexT& whose, base_meta_writter& stream, uint8_t type)
	{
		if (!whose.meta_data_.serialize_checkable_definition(stream, type))
			return false;
		if (!whose.complex_data_.serialize_complex_definition(stream, type))
			return false;
		return true;
	}

	template<class complexT>
	static bool deserialize_struct_class(complexT& whose, base_meta_reader& stream, uint8_t type)
	{
		return false;
	}
};

// Class rx_platform::meta::mapped_data_type 

mapped_data_type::mapped_data_type()
{
}

mapped_data_type::mapped_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



bool mapped_data_type::serialize_mapped_definition (base_meta_writter& stream, uint8_t type) const
{
	
	if (!stream.start_array("Mappers", mappers_.size()))
		return false;
	for (const auto& one : mappers_)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	return true;
}

bool mapped_data_type::deserialize_mapped_definition (base_meta_reader& stream, uint8_t type)
{

	return true;
}

bool mapped_data_type::register_mapper (const mapper_attribute& item, complex_data_type& complex_data)
{
	if (complex_data.check_name(item.get_name()))
	{
		mappers_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

void mapped_data_type::construct (complex_runtime_ptr what)
{
}

void mapped_data_type::construct (objects::object_runtime_ptr what)
{
}


// Class rx_platform::meta::variable_class 

string_type variable_class::type_name = RX_CPP_VARIABLE_CLASS_TYPE_NAME;

variable_class::variable_class()
{
}

variable_class::variable_class (const string_type& name, const rx_node_id& id, bool system)
	: meta_data_(name,id,system)
{
}


variable_class::~variable_class()
{
}



void variable_class::construct (variable_runtime_ptr what)
{
}

bool variable_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return variable_data_.serialize_variable_definition(stream, type);
}

bool variable_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return variable_data_.deserialize_variable_definition(stream, type);
}

platform_item_ptr variable_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

rx_value variable_class::get_value () const
{
	return rx_value(meta_data_.get_version(), meta_data_.get_created_time());
}

const checkable_data& variable_class::meta_data () const
{
  return meta_data_;

}


// Class rx_platform::meta::struct_class 

string_type struct_class::type_name = RX_CPP_STRUCT_CLASS_TYPE_NAME;

struct_class::struct_class()
{
}

struct_class::struct_class (const string_type& name, const rx_node_id& id, bool system)
	: meta_data_(name,id, system)
{
}


struct_class::~struct_class()
{
}



void struct_class::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

namespace_item_attributes struct_class::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_read_access | (meta_data_.get_system() ? namespace_item_attributes::namespace_item_system : namespace_item_attributes::namespace_item_null));
}

void struct_class::construct (struct_runtime_ptr what)
{
	//!!!!!!!!!!!TODO
}

platform_item_ptr struct_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

const checkable_data& struct_class::meta_data () const
{
  return meta_data_;

}

bool struct_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return meta_helpers::serialize_struct_class<struct_class>(*this, stream, type);
}

bool struct_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}

struct_runtime_ptr struct_class::create_runtime_ptr ()
{
	return new objects::struct_runtime;
}


// Class rx_platform::meta::checkable_data 

checkable_data::checkable_data()
      : version_(RX_INITIAL_ITEM_VERSION),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
{
}

checkable_data::checkable_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system)
      : version_(RX_INITIAL_ITEM_VERSION),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
	, name_(name)
	, id_(id)
	, system_(system)
{
}



bool checkable_data::serialize_node (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const
{
	if (!stream.write_header(type, 0))
		return false;

	/*std::function<void(base_meta_writter& stream, uint8_t)> func(std::bind(&metaT::serialize_definition, this, _1, _2));
	func(stream, type);*/
	/*if(!ret)
		return false;
*/
	if (!stream.write_footer())
		return false;

	return true;
}

bool checkable_data::deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value)
{
	return false;
}

bool checkable_data::check_in (base_meta_reader& stream)
{
	return false;
}

bool checkable_data::check_out (base_meta_writter& stream) const
{
	if (!stream.write_header(STREAMING_TYPE_CHECKOUT, 0))
		return false;

	/*std::function<void(base_meta_writter& stream, uint8_t)> func(std::bind(&metaT::serialize_definition, this, _1, _2));
	func(stream, STREAMING_TYPE_CHECKOUT);
*/

	if (!stream.write_footer())
		return false;

	return true;
}

bool checkable_data::serialize_checkable_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!stream.write_id("NodeId", id_))
		return false;
	if (!stream.write_bool("System", system_))
		return false;
	if (!stream.write_string("Name", name_.c_str()))
		return false;
	if (!stream.write_id("SuperId", parent_))
		return false;
	if (!stream.write_version("Ver", version_))
		return false;
	return true;
}

bool checkable_data::deserialize_checkable_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_id("NodeId", id_))
		return false;
	if (!stream.read_bool("System", system_))
		return false;
	return true;
	if (!stream.read_string("Name", name_))
		return false;
	return true;
	if (!stream.read_id("Parent", parent_))
		return false;
	return true;
}

void checkable_data::construct (complex_runtime_ptr what)
{
}

values::rx_value checkable_data::get_value () const
{
	return values::rx_value(version_, modified_time_);
}

void checkable_data::construct (objects::object_runtime_ptr what)
{
}


// Class rx_platform::meta::const_value 


bool const_value::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	stream.write_string("Name", name_.c_str());
	rx_value val;
	get_value(val);
	if (!val.serialize_value(stream))
		return false;
	return true;
}

bool const_value::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}

void const_value::get_value (values::rx_value& val) const
{
	rx_mode_type t;
	storage_.get_value(val, modified_time_, t);
}


// Class rx_platform::meta::simple_value_def 


bool simple_value_def::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	stream.write_string("Name", name_.c_str());
	rx_value val;
	get_value(val);
	if (!val.serialize_value(stream))
		return false;
	return true;
}

bool simple_value_def::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}

void simple_value_def::get_value (values::rx_value& val) const
{
}


// Class rx_platform::meta::variable_data_type 

variable_data_type::variable_data_type()
{
}

variable_data_type::variable_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}


variable_data_type::~variable_data_type()
{
}



bool variable_data_type::register_source (const source_attribute& item, complex_data_type& complex_data)
{
	if (complex_data.check_name(item.get_name()))
	{
		sources_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

bool variable_data_type::register_filter (const filter_attribute& item, complex_data_type& complex_data)
{
	if (complex_data.check_name(item.get_name()))
	{
		filters_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

bool variable_data_type::register_event (const event_attribute& item, complex_data_type& complex_data)
{
	if (complex_data.check_name(item.get_name()))
	{
		events_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

void variable_data_type::construct (complex_runtime_ptr what)
{
}

bool variable_data_type::serialize_variable_definition (base_meta_writter& stream, uint8_t type) const
{
	
	return true;
}

bool variable_data_type::deserialize_variable_definition (base_meta_reader& stream, uint8_t type)
{
	
	return true;
}


// Class rx_platform::meta::complex_data_type 

complex_data_type::complex_data_type()
{
}

complex_data_type::complex_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



bool complex_data_type::serialize_complex_definition (base_meta_writter& stream, uint8_t type) const
{
	
	if (!stream.write_bool("Sealed", sealed_))
		return false;

	if (!stream.write_bool("Abstract", abstract_))
		return false;

	if (!stream.start_array("Const", const_values_.size()))
		return false;
	for (const auto& one : const_values_)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	if (!stream.start_array("Vals", simple_values_.size()))
		return false;
	for (const auto& one : simple_values_)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	if (!stream.start_array("Structs", structs_.size()))
		return false;
	for (const auto& one : structs_)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one->serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	if (!stream.start_array("Vars", variables_.size()))
		return false;
	for (const auto& one : variables_)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one->serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	return true;
}

bool complex_data_type::deserialize_complex_definition (base_meta_reader& stream, uint8_t type)
{
	
	if (!stream.read_bool("Sealed", sealed_))
		return false;

	if (!stream.start_array("Items"))
		return false;

	//for (const auto& one : sub_items_)
	//{
	//	if (!one->deserialize_definition(stream, type))
	//		return false;
	//}

	if (!stream.array_end())
		return false;

	return false;//!!!! NOT DONE
}

bool complex_data_type::register_struct (const string_type& name, const rx_node_id& id)
{
	if (check_name(name))
	{
		structs_.emplace_back(std::make_unique<struct_attribute>(name,id));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::register_variable (const string_type& name, const rx_node_id& id)
{
	if (check_name(name))
	{
		variables_.emplace_back(std::make_unique<variable_attribute>(name, id));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::check_name (const string_type& name)
{
	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
	{
		names_cache_.emplace(name);
		return true;
	}
	else
	{
		return false;
	}
}

void complex_data_type::construct (complex_runtime_ptr what)
{
	// constant values
	for (auto& one : const_values_)
	{
		rx_value temp;
		one.get_value(temp);
		what->register_const_value(one.get_name(), temp);
	}

	for (auto& one : simple_values_)
	{
		rx_value temp;
		one.get_value(temp);
		what->register_value(one.get_name(), temp);
	}
	for (auto& one : structs_)
	{
		//!!!what->register_sub_item(one->get_name(),one->construct().cast_to<objects::struct_runtime_ptr>());
	}
}


// Class rx_platform::meta::mapper_class 

string_type mapper_class::type_name = RX_CPP_MAPPER_CLASS_TYPE_NAME;

mapper_class::mapper_class()
{
}



platform_item_ptr mapper_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

const checkable_data& mapper_class::meta_data () const
{
  return meta_data_;

}

bool mapper_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return meta_helpers::serialize_complex_class(*this, stream, type);
}

bool mapper_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return meta_helpers::deserialize_complex_class(*this, stream, type);
}


// Class rx_platform::meta::struct_attribute 

struct_attribute::struct_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name,id)
{
}


struct_attribute::~struct_attribute()
{
}



struct_runtime_ptr struct_attribute::construct ()
{
	auto what = new objects::struct_runtime();
	auto cls = model::internal_classes_manager::instance().get_type_cache<struct_class>().get_class_definition(get_target_id());
	if (cls)
	{
		cls->construct(what);
	}
	return what;
}

bool struct_attribute::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!stream.start_object(objects::struct_runtime::type_name.c_str()))
		return false;

	if (!complex_class_attribute::serialize_definition(stream, type))
		return false;


	if (!stream.end_object())
		return false;

	return true;
}

bool struct_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	/*if (!stream.start_object(objects::struct_runtime::type_name.c_str()))
		return false;

	if (!complex_class_attribute::deserialize_definition(stream, type))
		return false;

	if (!stream.end_object())
		return false;*/

	return true;
}


// Class rx_platform::meta::variable_attribute 

variable_attribute::variable_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name, id)
{
}


variable_attribute::~variable_attribute()
{
}



variable_runtime_ptr variable_attribute::construct ()
{
	auto what = new objects::variable_runtime();
	auto cls = model::internal_classes_manager::instance().get_type_cache<variable_class>().get_class_definition(get_target_id());
	if (cls)
	{
		cls->construct(what);
	}
	return what;
}

bool variable_attribute::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!stream.start_object(objects::variable_runtime::type_name.c_str()))
		return false;

	if (!complex_class_attribute::serialize_definition(stream, type))
		return false;


	if (!stream.end_object())
		return false;

	return true;
}

bool variable_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::mapper_attribute 

mapper_attribute::mapper_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name,id)
{
}


mapper_attribute::~mapper_attribute()
{
}



complex_runtime_ptr mapper_attribute::construct ()
{
	return nullptr;
}

bool mapper_attribute::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return false;
}

bool mapper_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::source_attribute 

source_attribute::source_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name,id)
{
}


source_attribute::~source_attribute()
{
}



complex_runtime_ptr source_attribute::construct ()
{
	return nullptr;
}

bool source_attribute::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return false;
}

bool source_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::complex_class_attribute 

complex_class_attribute::complex_class_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}


complex_class_attribute::~complex_class_attribute()
{
}



bool complex_class_attribute::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!stream.write_string("Name", name_.c_str()))
		return false;
	if (!stream.write_id("Id", target_id_))
		return false;
	return true;
}

bool complex_class_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_string("Name", name_))
		return false;
	if (!stream.read_id("Id", target_id_))
		return false;

	return true;
}


// Class rx_platform::meta::filter_attribute 

filter_attribute::filter_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name, id)
{
}


filter_attribute::~filter_attribute()
{
}



complex_runtime_ptr filter_attribute::construct ()
{
	return nullptr;
}

bool filter_attribute::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return false;
}

bool filter_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::event_attribute 

event_attribute::event_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name, id)
{
}


event_attribute::~event_attribute()
{
}



complex_runtime_ptr event_attribute::construct ()
{
	return nullptr;
}

bool event_attribute::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return false;
}

bool event_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::source_class 

string_type source_class::type_name = RX_CPP_SOURCE_CLASS_TYPE_NAME;

source_class::source_class()
{
}


source_class::~source_class()
{
}



platform_item_ptr source_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

const checkable_data& source_class::meta_data () const
{
  return meta_data_;

}

bool source_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return meta_helpers::serialize_complex_class<source_class>(*this, stream, type);
}

bool source_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return meta_helpers::deserialize_complex_class<source_class>(*this, stream, type);
}


// Class rx_platform::meta::filter_class 

string_type filter_class::type_name = RX_CPP_FILTER_CLASS_TYPE_NAME;

filter_class::filter_class()
{
}


filter_class::~filter_class()
{
}



platform_item_ptr filter_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

const checkable_data& filter_class::meta_data () const
{
  return meta_data_;

}

bool filter_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return meta_helpers::serialize_complex_class<filter_class>(*this, stream, type);
}

bool filter_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return meta_helpers::deserialize_complex_class<filter_class>(*this, stream, type);
}


// Class rx_platform::meta::event_class 

string_type event_class::type_name = RX_CPP_EVENT_CLASS_TYPE_NAME;

event_class::event_class()
{
}


event_class::~event_class()
{
}



platform_item_ptr event_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

const checkable_data& event_class::meta_data () const
{
  return meta_data_;

}

bool event_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return meta_helpers::serialize_complex_class<event_class>(*this, stream, type);
}

bool event_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return meta_helpers::deserialize_complex_class<event_class>(*this, stream, type);
}


} // namespace meta
} // namespace rx_platform

//#define RX_TEMPLATE_INST(typeArg, brwArg) \
//template class rx_platform::meta::checkable_type<typeArg, brwArg>;\
//
//RX_TEMPLATE_INST(rx_platform::meta::object_class, false);
//RX_TEMPLATE_INST(rx_platform::meta::domain_class, false);
//RX_TEMPLATE_INST(rx_platform::meta::application_class, false);
//RX_TEMPLATE_INST(rx_platform::meta::port_class, false);
//RX_TEMPLATE_INST(rx_platform::meta::struct_class, false);
//
//
//template class rx_platform::meta::variable_data_type<rx_platform::meta::complex_data_type>;
//RX_TEMPLATE_INST(rx_platform::meta::variable_class, false);
//
//#define RX_TEMPLATE_INST_SIMPLE(typeArg, brwArg) \
//template class rx_platform::meta::checkable_type<typeArg, brwArg>;\
//
//RX_TEMPLATE_INST_SIMPLE(rx_platform::objects::object_runtime, true);
//RX_TEMPLATE_INST_SIMPLE(rx_platform::logic::program_runtime, false);

