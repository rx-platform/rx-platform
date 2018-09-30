

/****************************************************************************
*
*  system\meta\rx_def_blocks.cpp
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


// rx_def_blocks
#include "system/meta/rx_def_blocks.h"

#include "system/meta/rx_obj_classes.h"
#include "sys_internal/rx_internal_ns.h"
#include "classes/rx_meta.h"
#include "system/constructors/rx_construct.h"
#include "rx_objbase.h"
using namespace rx;


namespace rx_platform {

namespace meta {

namespace def_blocks {

// Class rx_platform::meta::def_blocks::complex_class_attribute 

complex_class_attribute::complex_class_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}


complex_class_attribute::~complex_class_attribute()
{
}



bool complex_class_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
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


// Class rx_platform::meta::def_blocks::complex_data_type 

complex_data_type::complex_data_type()
{
}

complex_data_type::complex_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



bool complex_data_type::serialize_complex_definition (base_meta_writer& stream, uint8_t type) const
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
		structs_.emplace_back(std::make_unique<struct_attribute>(name, id));
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
		what->register_const_value(one.get_name(), one.get_value());
	}

	for (auto& one : simple_values_)
	{
		what->register_value(one.get_name(), rx_timed_value::from_simple(one.get_value(rx_time::now())));
	}
	//for (auto& one : structs_)
	//{
		//!!!what->register_sub_item(one->get_name(),one->construct().cast_to<objects::struct_runtime_ptr>());
	//}
}

bool complex_data_type::register_simple_value (const string_type& name, rx_simple_value&& val)
{
	if (check_name(name))
	{
		simple_values_.emplace_back(name, std::move(val));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::register_const_value (const string_type& name, rx_simple_value&& val)
{
	if (check_name(name))
	{
		const_values_.emplace_back(name, std::move(val));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::register_simple_value (const string_type& name, const rx_simple_value& val)
{
	if (check_name(name))
	{
		simple_values_.emplace_back(name, val);
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::register_const_value (const string_type& name, const rx_simple_value& val)
{
	if (check_name(name))
	{
		const_values_.emplace_back(name, val);
		return true;
	}
	else
	{
		return false;
	}
}


// Class rx_platform::meta::def_blocks::const_value_def 

const_value_def::const_value_def (const string_type& name, rx_simple_value&& value)
      : created_time_(rx_time::now()),
        modified_time_(rx_time::now())
	, name_(name)
	, storage_(std::move(value))
{
}

const_value_def::const_value_def (const string_type& name, const rx_simple_value& value)
      : created_time_(rx_time::now()),
        modified_time_(rx_time::now())
	, name_(name)
	, storage_(value)
{
}



bool const_value_def::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	stream.write_string("Name", name_.c_str());
	if (!get_value().serialize(stream))
		return false;
	return true;
}

bool const_value_def::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}

rx_simple_value const_value_def::get_value () const
{
	return storage_;
}


// Class rx_platform::meta::def_blocks::event_attribute 

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

bool event_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool event_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::def_blocks::filter_attribute 

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

bool filter_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool filter_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::def_blocks::mapped_data_type 

mapped_data_type::mapped_data_type()
{
}

mapped_data_type::mapped_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



bool mapped_data_type::serialize_mapped_definition (base_meta_writer& stream, uint8_t type) const
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
	return false;
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


// Class rx_platform::meta::def_blocks::mapper_attribute 

mapper_attribute::mapper_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name, id)
{
}


mapper_attribute::~mapper_attribute()
{
}



complex_runtime_ptr mapper_attribute::construct ()
{
	return nullptr;
}

bool mapper_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool mapper_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::def_blocks::simple_value_def 

simple_value_def::simple_value_def (const string_type& name, rx_simple_value&& value)
      : read_only_(true),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
	, name_(name)
	, storage_(std::move(value))
{
}

simple_value_def::simple_value_def (const string_type& name, const rx_simple_value& value)
      : read_only_(true),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
	, name_(name)
	, storage_(value)
{
}



bool simple_value_def::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	stream.write_string("Name", name_.c_str());
	if (!storage_.serialize(stream))
		return false;
	return true;
}

bool simple_value_def::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}

rx_simple_value simple_value_def::get_value (rx_time now) const
{
	return storage_;
}


// Class rx_platform::meta::def_blocks::source_attribute 

source_attribute::source_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name, id)
{
}


source_attribute::~source_attribute()
{
}



complex_runtime_ptr source_attribute::construct ()
{
	return nullptr;
}

bool source_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool source_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::def_blocks::struct_attribute 

struct_attribute::struct_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name, id)
{
}


struct_attribute::~struct_attribute()
{
}



struct_runtime_ptr struct_attribute::construct ()
{
	auto what = new objects::blocks::struct_runtime();
	auto cls = model::internal_classes_manager::instance().get_type_cache<struct_class>().get_class_definition(get_target_id());
	if (cls)
	{
		cls->construct(what);
	}
	return what;
}

bool struct_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object(objects::blocks::struct_runtime::type_name.c_str()))
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

	return false;
}


// Class rx_platform::meta::def_blocks::variable_attribute 

variable_attribute::variable_attribute (const string_type& name, const rx_node_id& id)
	: complex_class_attribute(name, id)
{
}


variable_attribute::~variable_attribute()
{
}



variable_runtime_ptr variable_attribute::construct ()
{
	auto what = new objects::blocks::variable_runtime();
	auto cls = model::internal_classes_manager::instance().get_type_cache<variable_class>().get_class_definition(get_target_id());
	if (cls)
	{
		cls->construct(what);
	}
	return what;
}

bool variable_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object(objects::blocks::variable_runtime::type_name.c_str()))
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


// Class rx_platform::meta::def_blocks::variable_data_type 

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

bool variable_data_type::serialize_variable_definition (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool variable_data_type::deserialize_variable_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


} // namespace def_blocks
} // namespace meta
} // namespace rx_platform

