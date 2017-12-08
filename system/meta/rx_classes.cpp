

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

#include "system/meta/rx_obj_classes.h"


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


// Class rx_platform::meta::mapper_class 

string_type mapper_class::type_name = "MAPPER CLASS";

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

