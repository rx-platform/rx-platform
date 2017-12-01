

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


// rx_classes
#include "system/meta/rx_classes.h"
// rx_objbase
#include "system/meta/rx_objbase.h"



namespace rx_platform {

namespace meta {

// Class rx_platform::meta::command_class 

string_type command_class::type_name = "COMMAND";

command_class::command_class()
{
}


command_class::~command_class()
{
}



// Class rx_platform::meta::variable_class 

string_type variable_class::type_name = "VARIABLE CLASS";

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

string_type struct_class::type_name = "STRUCT CLASS";

struct_class::struct_class()
{
}

struct_class::struct_class (const string_type& name, const rx_node_id& id, bool system)
{
}


struct_class::~struct_class()
{
}



// Class rx_platform::meta::const_value 

const_value::const_value()
{
}

const_value::const_value(const const_value &right)
{
}

const_value::const_value (const string_type& name)
	: _name(name)
{
}


const_value::~const_value()
{
}


const_value & const_value::operator=(const const_value &right)
{
	RX_ASSERT(false);
	return *this;
}



bool const_value::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	stream.write_string("Name", _name.c_str());
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


// Class rx_platform::meta::internal_value 

internal_value::internal_value()
      : _read_only(true)
{
}

internal_value::internal_value(const internal_value &right)
      : _read_only(true)
{
}


internal_value::~internal_value()
{
}


internal_value & internal_value::operator=(const internal_value &right)
{
	RX_ASSERT(false);
	return *this;
}



bool internal_value::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	rx_value val;
	get_value(val);
	if (!val.serialize_value(stream))
		return false;
	return true;
}

bool internal_value::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}


// Class rx_platform::meta::object_class 

string_type object_class::type_name = "OBJECT CLASS";

object_class::object_class()
{
}

object_class::object_class (const string_type& name, const rx_node_id& id, bool system)
	: object_class_t(name,id,system)
{
}


object_class::~object_class()
{
}



void object_class::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

void object_class::get_value (values::rx_value& val) const
{
}

const string_type& object_class::get_item_name () const
{
	return get_name();
}

namespace_item_attributes object_class::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_command | namespace_item_attributes::namespace_item_execute | namespace_item_attributes::namespace_item_read_access | namespace_item_attributes::namespace_item_system);
}


// Parameterized Class rx_platform::meta::base_complex_type 

template <class metaT>
base_complex_type<metaT>::base_complex_type()
{
}

template <class metaT>
base_complex_type<metaT>::base_complex_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
	: checkable_type<metaT>(name,id,parent,system)
	, _sealed(sealed)
	, _abstract(abstract)
{
}


template <class metaT>
base_complex_type<metaT>::~base_complex_type()
{
	for (auto one : _internal_values)
		delete one;
}



template <class metaT>
bool base_complex_type<metaT>::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!checkable_type<metaT>::serialize_definition(stream, type))
		return false;

	if (!stream.write_bool("Sealed", _sealed))
		return false;

	if (!stream.write_bool("Abstract", _abstract))
		return false;

	if (!stream.start_array("Const", _const_values.size()))
		return false;
	for (const auto& one : _const_values)
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

	if (!stream.start_array("Vals", _internal_values.size()))
		return false;
	for (const auto& one : _internal_values)
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

	if (!stream.start_array("Structs", _structs.size()))
		return false;
	for (const auto& one : _structs)
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

	if (!stream.start_array("Vars", _variables.size()))
		return false;
	for (const auto& one : _variables)
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

template <class metaT>
bool base_complex_type<metaT>::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!checkable_type<metaT>::deserialize_definition(stream, type))
		return false;

	if (!stream.read_bool("Sealed", _sealed))
		return false;

	return true;
}

template <class metaT>
bool base_complex_type<metaT>::generate_json (std::ostream& def, std::ostream& err) const
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

template <class metaT>
bool base_complex_type<metaT>::register_internal_value (internal_value* item)
{
	if (check_name(item->get_name()))
	{
		_internal_values.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT>
bool base_complex_type<metaT>::register_struct (const struct_attribute& item)
{
	if (check_name(item.get_name()))
	{
		_structs.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT>
bool base_complex_type<metaT>::register_variable (const variable_attribute& item)
{
	if (check_name(item.get_name()))
	{
		_variables.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

template <class metaT>
bool base_complex_type<metaT>::check_name (const string_type& name)
{
	auto it = _names_cache.find(name);
	if (it == _names_cache.end())
	{
		_names_cache.emplace(name);
		return true;
	}
	else
	{
		return false;
	}
}


// Class rx_platform::meta::mapper_class 

string_type mapper_class::type_name = "MAPPER CLASS";

mapper_class::mapper_class()
{
}


mapper_class::~mapper_class()
{
}



// Class rx_platform::meta::domain_class 

string_type domain_class::type_name = "DOMAIN CLASS";

domain_class::domain_class()
{
}

domain_class::domain_class (const string_type& name, const rx_node_id& id, bool system)
{
}


domain_class::~domain_class()
{
}



// Class rx_platform::meta::application_class 

string_type application_class::type_name = "DOMAIN CLASS";

application_class::application_class()
{
}

application_class::application_class (const string_type& name, const rx_node_id& id, bool system)
{
}


application_class::~application_class()
{
}



// Class rx_platform::meta::port_class 

string_type port_class::type_name = "PORT CLASS";

port_class::port_class()
{
}

port_class::port_class (const string_type& name, const rx_node_id& id, bool system)
{
}


port_class::~port_class()
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
	if (!stream.write_string("Name", _name.c_str()))
		return false;
	if (!stream.write_id("Id", _target_id))
		return false;
	return true;
}

bool complex_class_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_string("Name", _name))
		return false;
	if (!stream.read_id("Id", _target_id))
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

string_type source_class::type_name = "SOURCE CLASS";

source_class::source_class()
{
}


source_class::~source_class()
{
}



// Class rx_platform::meta::filter_class 

string_type filter_class::type_name = "FILTER CLASS";

filter_class::filter_class()
{
}


filter_class::~filter_class()
{
}



// Class rx_platform::meta::event_class 

string_type event_class::type_name = "EVENT CLASS";

event_class::event_class()
{
}


event_class::~event_class()
{
}



} // namespace meta
} // namespace rx_platform

