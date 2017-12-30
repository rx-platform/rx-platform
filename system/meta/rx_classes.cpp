

/****************************************************************************
*
*  system\meta\rx_classes.cpp
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

#include "rx_configuration.h"

// rx_classes
#include "system/meta/rx_classes.h"
// rx_objbase
#include "system/meta/rx_objbase.h"

#include "system/meta/rx_obj_classes.h"


namespace rx_platform {

namespace meta {

// Parameterized Class rx_platform::meta::base_meta_type 

template <class metaT, bool _browsable>
base_meta_type<metaT,_browsable>::base_meta_type()
{
}

template <class metaT, bool _browsable>
base_meta_type<metaT,_browsable>::base_meta_type (const string_type& name, const rx_node_id& id, bool system)
	: id_(id),
	system_(system)
	, rx_platform_item(name)
{
}


template <class metaT, bool _browsable>
base_meta_type<metaT,_browsable>::~base_meta_type()
{
}



template <class metaT, bool _browsable>
bool base_meta_type<metaT,_browsable>::serialize (base_meta_writter& stream) const
{
	if (!rx_platform_item::serialize(stream))
		return false;
	if (!stream.write_id("NodeId", id_))
		return false;
	if (!stream.write_bool("System", system_))
		return false;
	return true;
}

template <class metaT, bool _browsable>
bool base_meta_type<metaT,_browsable>::deserialize (base_meta_reader& stream)
{
	if (!rx_platform_item::deserialize(stream))
		return false;
	if (!stream.read_id("NodeId", id_))
		return false;
	if (!stream.read_bool("System", system_))
		return false;
	return true;
}

template <class metaT, bool _browsable>
string_type base_meta_type<metaT,_browsable>::get_type_name () const
{
	return metaT::type_name;
}

template <class metaT, bool _browsable>
void base_meta_type<metaT,_browsable>::construct (runtime_ptr_t what)
{
	typedef typename metaT::RType rtype_t;
	typedef typename rtype_t::smart_ptr rptr_t;
	rptr_t runtime = what.cast_to<rptr_t>();
}


// Parameterized Class rx_platform::meta::base_mapped_class 

template <class metaT, bool _browsable>
base_mapped_class<metaT,_browsable>::base_mapped_class()
{
}

template <class metaT, bool _browsable>
base_mapped_class<metaT,_browsable>::base_mapped_class (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
	: base_complex_type<metaT, _browsable>(name, id, parent, system, sealed, abstract)
{
}


template <class metaT, bool _browsable>
base_mapped_class<metaT,_browsable>::~base_mapped_class()
{
}



template <class metaT, bool _browsable>
bool base_mapped_class<metaT,_browsable>::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!base_complex_type<metaT, _browsable>::serialize_definition(stream, type))
		return false;

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

template <class metaT, bool _browsable>
bool base_mapped_class<metaT,_browsable>::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!base_complex_type<metaT, _browsable>::deserialize_definition(stream, type))
		return false;

	return true;
}

template <class metaT, bool _browsable>
bool base_mapped_class<metaT,_browsable>::register_mapper (const mapper_attribute& item)
{
	if (this->check_name(item.get_name()))
	{
		mappers_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT, bool _browsable>
void base_mapped_class<metaT,_browsable>::construct (runtime_ptr_t what)
{
	base_complex_type<metaT, _browsable>::construct(what);
	typedef typename metaT::RType rtype_t;
	typedef typename rtype_t::smart_ptr rptr_t;
	rptr_t runtime = what.cast_to<rptr_t>();
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



// Class rx_platform::meta::struct_class 

string_type struct_class::type_name = RX_CPP_STRUCT_CLASS_TYPE_NAME;

struct_class::struct_class()
{
}

struct_class::struct_class (const string_type& name, const rx_node_id& id, bool system)
{
}


struct_class::~struct_class()
{
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
	, base_meta_type<metaT, _browsable>(name, id, system)
{
}


template <class metaT, bool _browsable>
checkable_type<metaT,_browsable>::~checkable_type()
{
}



template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::serialize_node (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const
{
	if (!stream.write_header(type))
		return false;

	if (!this->serialize_definition(stream, type))
		return false;

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

	if (!this->serialize_definition(stream, STREAMING_TYPE_CHECKOUT))
		return false;

	if (!stream.write_footer())
		return false;

	return true;
}

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!base_meta_type<metaT, _browsable>::serialize(stream))
		return false;

	if (!stream.write_id("SuperId", parent_))
		return false;

	if (!stream.write_version("Ver", version_))
		return false;
	return true;
}

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!base_meta_type<metaT, _browsable>::deserialize(stream))
		return false;

	if (!stream.read_id("Parent", parent_))
		return false;

	return true;
}

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::generate_json (std::ostream& def, std::ostream& err) const
{
	err << "Function not implemented for this type.";
	return false;
}

template <class metaT, bool _browsable>
bool checkable_type<metaT,_browsable>::is_browsable () const
{
	return false;
}

template <class metaT, bool _browsable>
void checkable_type<metaT,_browsable>::construct (runtime_ptr_t what)
{
	base_meta_type<metaT, _browsable>::construct(what);
	typedef typename metaT::RType rtype_t;
	typedef typename rtype_t::smart_ptr rptr_t;
	rptr_t runtime = what.cast_to<rptr_t>();
	runtime->parent_ = this->get_id();
}

template <class metaT, bool _browsable>
values::rx_value checkable_type<metaT,_browsable>::get_value () const
{
	return values::rx_value(this->get_name(), modified_time_);
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


// Parameterized Class rx_platform::meta::base_variable_class 

template <class metaT, bool _browsable>
base_variable_class<metaT,_browsable>::base_variable_class()
{
}

template <class metaT, bool _browsable>
base_variable_class<metaT,_browsable>::base_variable_class (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
	: base_mapped_class<metaT, _browsable>(name, id, parent, system, sealed, abstract)
{
}


template <class metaT, bool _browsable>
base_variable_class<metaT,_browsable>::~base_variable_class()
{
}



template <class metaT, bool _browsable>
bool base_variable_class<metaT,_browsable>::register_source (const source_attribute& item)
{
	if (this->check_name(item.get_name()))
	{
		sources_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT, bool _browsable>
bool base_variable_class<metaT,_browsable>::register_filter (const filter_attribute& item)
{
	if (this->check_name(item.get_name()))
	{
		filters_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT, bool _browsable>
bool base_variable_class<metaT,_browsable>::register_event (const event_attribute& item)
{
	if (this->check_name(item.get_name()))
	{
		events_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}


// Parameterized Class rx_platform::meta::base_complex_type 

template <class metaT, bool _browsable>
base_complex_type<metaT,_browsable>::base_complex_type()
{
}

template <class metaT, bool _browsable>
base_complex_type<metaT,_browsable>::base_complex_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
	: checkable_type<metaT, _browsable>(name, id, parent, system)
	, sealed_(sealed)
	, abstract_(abstract)
{
}


template <class metaT, bool _browsable>
base_complex_type<metaT,_browsable>::~base_complex_type()
{
}



template <class metaT, bool _browsable>
bool base_complex_type<metaT,_browsable>::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!checkable_type<metaT, _browsable>::serialize_definition(stream, type))
		return false;

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
		if (!one.serialize_definition(stream, type))
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
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	return true;
}

template <class metaT, bool _browsable>
bool base_complex_type<metaT,_browsable>::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!checkable_type<metaT, _browsable>::deserialize_definition(stream, type))
		return false;

	if (!stream.read_bool("Sealed", sealed_))
		return false;

	return false;//!!!! NOT DONE
}

template <class metaT, bool _browsable>
bool base_complex_type<metaT,_browsable>::generate_json (std::ostream& def, std::ostream& err) const
{
	rx_platform::serialization::json_writter writter;

	writter.write_header(STREAMING_TYPE_CLASS);

	this->serialize_definition(writter, STREAMING_TYPE_CLASS);

	writter.write_footer();

	string_type result;
	bool out = writter.get_string(result, true);

	if (out)
		def << result;
	else
		def << "Error in JSON deserialization.";

	return true;
}

template <class metaT, bool _browsable>
bool base_complex_type<metaT,_browsable>::register_struct (const struct_attribute& item)
{
	if (check_name(item.get_name()))
	{
		structs_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT, bool _browsable>
bool base_complex_type<metaT,_browsable>::register_variable (const variable_attribute& item)
{
	if (check_name(item.get_name()))
	{
		variables_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT, bool _browsable>
bool base_complex_type<metaT,_browsable>::check_name (const string_type& name)
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

template <class metaT, bool _browsable>
void base_complex_type<metaT,_browsable>::construct (runtime_ptr_t what)
{
	checkable_type<metaT, _browsable>::construct(what);
	typedef typename metaT::RType rtype_t;
	typedef typename rtype_t::smart_ptr rptr_t;
	rptr_t runtime = what.cast_to<rptr_t>();
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
}


// Class rx_platform::meta::mapper_class 

string_type mapper_class::type_name = RX_CPP_MAPPER_CLASS_TYPE_NAME;

mapper_class::mapper_class()
{
}


mapper_class::~mapper_class()
{
}



// Class rx_platform::meta::struct_attribute 

struct_attribute::struct_attribute()
{
}


struct_attribute::~struct_attribute()
{
}



// Class rx_platform::meta::variable_attribute 

variable_attribute::variable_attribute()
{
}


variable_attribute::~variable_attribute()
{
}



// Class rx_platform::meta::mapper_attribute 

mapper_attribute::mapper_attribute()
{
}


mapper_attribute::~mapper_attribute()
{
}



// Class rx_platform::meta::source_attribute 

source_attribute::source_attribute()
{
}


source_attribute::~source_attribute()
{
}



// Class rx_platform::meta::complex_class_attribute 

complex_class_attribute::complex_class_attribute()
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

filter_attribute::filter_attribute()
{
}


filter_attribute::~filter_attribute()
{
}



// Class rx_platform::meta::event_attribute 

event_attribute::event_attribute()
{
}


event_attribute::~event_attribute()
{
}



// Class rx_platform::meta::source_class 

string_type source_class::type_name = RX_CPP_SOURCE_CLASS_TYPE_NAME;

source_class::source_class()
{
}


source_class::~source_class()
{
}



// Class rx_platform::meta::filter_class 

string_type filter_class::type_name = RX_CPP_FILTER_CLASS_TYPE_NAME;

filter_class::filter_class()
{
}


filter_class::~filter_class()
{
}



// Class rx_platform::meta::event_class 

string_type event_class::type_name = RX_CPP_EVENT_CLASS_TYPE_NAME;

event_class::event_class()
{
}


event_class::~event_class()
{
}


#define RX_TEMPLATE_INST(typeArg, brwArg) \
template class rx_platform::meta::base_mapped_class<typeArg, brwArg>;\
template class rx_platform::meta::base_complex_type<typeArg, brwArg>;\
template class rx_platform::meta::checkable_type<typeArg, brwArg>;\
template class rx_platform::meta::base_meta_type<typeArg, brwArg>;\

#define RX_TEMPLATE_INST_SIMPLE(typeArg, brwArg) \
template class rx_platform::meta::checkable_type<typeArg, brwArg>;\
template class rx_platform::meta::base_meta_type<typeArg, brwArg>;\



RX_TEMPLATE_INST(rx_platform::meta::object_class, false);
RX_TEMPLATE_INST(rx_platform::meta::domain_class, false);
RX_TEMPLATE_INST(rx_platform::meta::application_class, false);
RX_TEMPLATE_INST(rx_platform::meta::port_class, false);

RX_TEMPLATE_INST_SIMPLE(rx_platform::objects::object_runtime, true);
RX_TEMPLATE_INST_SIMPLE(rx_platform::logic::program_runtime, false);

template class rx_platform::meta::base_variable_class<rx_platform::meta::variable_class, false>;
RX_TEMPLATE_INST(rx_platform::meta::variable_class, false);
} // namespace meta
} // namespace rx_platform

