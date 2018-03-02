

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

// Parameterized Class rx_platform::meta::mapped_data_type 

template <class complexT>
mapped_data_type<complexT>::mapped_data_type()
{
}

template <class complexT>
mapped_data_type<complexT>::mapped_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



template <class complexT>
bool mapped_data_type<complexT>::serialize_mapped_definition (base_meta_writter& stream, uint8_t type) const
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

template <class complexT>
bool mapped_data_type<complexT>::deserialize_mapped_definition (base_meta_reader& stream, uint8_t type)
{

	return true;
}

template <class complexT>
bool mapped_data_type<complexT>::register_mapper (const mapper_attribute& item, complexT& complex_data)
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

template <class complexT>
void mapped_data_type<complexT>::construct (complex_runtime_ptr what)
{
	typedef typename metaT::CType rtype_t;
	typedef typename rtype_t::smart_ptr rptr_t;
	rptr_t runtime = what.cast_to<rptr_t>();
}

template <class complexT>
void mapped_data_type<complexT>::construct (objects::object_runtime_ptr what)
{
}


// Class rx_platform::meta::variable_class 

string_type variable_class::type_name = RX_CPP_VARIABLE_CLASS_TYPE_NAME;

variable_class::variable_class()
{
}

variable_class::variable_class (const string_type& name, const rx_node_id& id, bool system)
	: variable_class_t(name,id,system)
{
}


variable_class::~variable_class()
{
}



void variable_class::construct (complex_runtime_ptr what)
{
}

bool variable_class::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return variable_data().serialize_variable_definition(stream, type);
}

bool variable_class::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return variable_data().deserialize_variable_definition(stream, type);
}

platform_item_ptr variable_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::struct_class 

string_type struct_class::type_name = RX_CPP_STRUCT_CLASS_TYPE_NAME;

struct_class::struct_class()
{
}

struct_class::struct_class (const string_type& name, const rx_node_id& id, bool system)
	: struct_class_t(name,id, system)
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
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_read_access | (get_system() ? namespace_item_attributes::namespace_item_system : namespace_item_attributes::namespace_item_null));
}

void struct_class::construct (complex_runtime_ptr what)
{
	struct_class_t::construct(what);
	objects::struct_runtime::smart_ptr runtime = what.cast_to<objects::struct_runtime::smart_ptr>();
}

platform_item_ptr struct_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

}


// Parameterized Class rx_platform::meta::checkable_type 

template <class metaT, bool _browsable>
checkable_type<metaT,_browsable>::checkable_type()
      : version_(RX_INITIAL_ITEM_VERSION),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
{
}

template <class metaT, bool _browsable>
checkable_type<metaT,_browsable>::checkable_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system)
      : version_(RX_INITIAL_ITEM_VERSION),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
	, id_(id)
	, name_(name)
	, system_(system)
{
}



template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::serialize_node (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const
{
	if (!stream.write_header(type))
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

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value)
{
	return false;
}

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::check_in (base_meta_reader& stream)
{
	return false;
}

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::check_out (base_meta_writter& stream) const
{
	if (!stream.write_header(STREAMING_TYPE_CHECKOUT))
		return false;

	/*std::function<void(base_meta_writter& stream, uint8_t)> func(std::bind(&metaT::serialize_definition, this, _1, _2));
	func(stream, STREAMING_TYPE_CHECKOUT);
*/

	if (!stream.write_footer())
		return false;

	return true;
}

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::serialize_checkable_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!stream.write_id("NodeId", id_))
		return false;
	if (!stream.write_bool("System", system_))
		return false;
	return true;
	if (!stream.write_string("Name", name_.c_str()))
		return false;
	return true;
	if (!stream.write_id("SuperId", parent_))
		return false;
	if (!stream.write_version("Ver", version_))
		return false;
	return true;
}

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::deserialize_checkable_definition (base_meta_reader& stream, uint8_t type)
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

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::is_browsable () const
{
	return false;
}

template <class metaT, bool _browsable>
void checkable_type<metaT,_browsable>::construct (complex_runtime_ptr what)
{
}

template <class metaT, bool _browsable>
values::rx_value checkable_type<metaT,_browsable>::get_value () const
{
	return values::rx_value(this->get_name(), modified_time_);
}

template <class metaT, bool _browsable>
void checkable_type<metaT,_browsable>::construct (objects::object_runtime_ptr what)
{
}

template <class metaT, bool _browsable>
string_type checkable_type<metaT,_browsable>::get_type_name () const
{
	return metaT::type_name;
}


// Class rx_platform::meta::const_value 

const_value::const_value()
      : created_time_(rx_time::now()),
        modified_time_(rx_time::now())
{
}

const_value::const_value (const string_type& name)
      : created_time_(rx_time::now()),
        modified_time_(rx_time::now())
	, name_(name)
{
}


const_value::~const_value()
{
}



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
	val.set_good_locally();
	val.set_time(modified_time_);
}


// Class rx_platform::meta::simple_value_def 

simple_value_def::simple_value_def()
      : read_only_(true)
{
}

simple_value_def::simple_value_def (const string_type& name)
      : read_only_(true)
	, name_(name)
{
}


simple_value_def::~simple_value_def()
{
}



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


// Parameterized Class rx_platform::meta::variable_data_type 

template <class complexT>
variable_data_type<complexT>::variable_data_type()
{
}

template <class complexT>
variable_data_type<complexT>::variable_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}


template <class complexT>
variable_data_type<complexT>::~variable_data_type()
{
}



template <class complexT>
bool variable_data_type<complexT>::register_source (const source_attribute& item, complexT& complex_data)
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

template <class complexT>
bool variable_data_type<complexT>::register_filter (const filter_attribute& item, complexT& complex_data)
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

template <class complexT>
bool variable_data_type<complexT>::register_event (const event_attribute& item, complexT& complex_data)
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

template <class complexT>
void variable_data_type<complexT>::construct (complex_runtime_ptr what)
{
}

template <class complexT>
bool variable_data_type<complexT>::serialize_variable_definition (base_meta_writter& stream, uint8_t type) const
{
	
	return true;
}

template <class complexT>
bool variable_data_type<complexT>::deserialize_variable_definition (base_meta_reader& stream, uint8_t type)
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
		if (!one->serialize_definition(stream, type))
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
		if (!one->serialize_definition(stream, type))
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
		one->get_value(temp);
		what->register_const_value(one->get_name(), temp);
	}

	for (auto& one : simple_values_)
	{
		rx_value temp;
		one->get_value(temp);
		what->register_value(one->get_name(), temp);
	}
	for (auto& one : structs_)
	{
		what->register_struct(one->get_name(),one->construct().cast_to<objects::struct_runtime_ptr>());
	}
}

void complex_data_type::construct (objects::object_runtime_ptr what)
{
	auto runtime = what->get_complex_item();
	// constant values
	for (auto& one : const_values_)
	{
		rx_value temp;
		one->get_value(temp);
		runtime->register_const_value(one->get_name(), temp);
	}

	for (auto& one : simple_values_)
	{
		rx_value temp;
		one->get_value(temp);
		runtime->register_value(one->get_name(), temp);
	}
	for (auto& one : structs_)
	{
		runtime->register_struct(one->get_name(),one->construct());
	}
}


// Class rx_platform::meta::mapper_class 

string_type mapper_class::type_name = RX_CPP_MAPPER_CLASS_TYPE_NAME;

mapper_class::mapper_class()
{
}


mapper_class::~mapper_class()
{
}



platform_item_ptr mapper_class::get_item_ptr ()
{
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

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
	auto what = rx_create_reference<objects::struct_runtime>();
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
	objects::variable_runtime_ptr what(pointers::_create_new);
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
	return complex_runtime_ptr::null_ptr;
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
	return complex_runtime_ptr::null_ptr;
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
	return complex_runtime_ptr::null_ptr;
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
	return complex_runtime_ptr::null_ptr;
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
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

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
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

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
  return rx_create_reference<sys_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::not_implemented 


bool not_implemented::serialize_mapped_definition (base_meta_writter& stream, uint8_t type) const
{
	return true;
}

bool not_implemented::deserialize_mapped_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}

bool not_implemented::serialize_variable_definition (base_meta_writter& stream, uint8_t type) const
{
	return true;
}

bool not_implemented::deserialize_variable_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}

bool not_implemented::serialize_object_definition (base_meta_writter& stream, uint8_t type) const
{
	return true;
}

bool not_implemented::deserialize_object_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}


} // namespace meta
} // namespace rx_platform

#define RX_TEMPLATE_INST(typeArg, brwArg) \
template class rx_platform::meta::checkable_type<typeArg, brwArg>;\

RX_TEMPLATE_INST(rx_platform::meta::object_class, false);
RX_TEMPLATE_INST(rx_platform::meta::domain_class, false);
RX_TEMPLATE_INST(rx_platform::meta::application_class, false);
RX_TEMPLATE_INST(rx_platform::meta::port_class, false);
RX_TEMPLATE_INST(rx_platform::meta::struct_class, false);


template class rx_platform::meta::variable_data_type<rx_platform::meta::complex_data_type>;
RX_TEMPLATE_INST(rx_platform::meta::variable_class, false);

#define RX_TEMPLATE_INST_SIMPLE(typeArg, brwArg) \
template class rx_platform::meta::checkable_type<typeArg, brwArg>;\

RX_TEMPLATE_INST_SIMPLE(rx_platform::objects::object_runtime, true);
RX_TEMPLATE_INST_SIMPLE(rx_platform::logic::program_runtime, false);

