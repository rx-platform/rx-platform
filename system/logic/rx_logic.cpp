

/****************************************************************************
*
*  system\logic\rx_logic.cpp
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


// rx_logic
#include "system/logic/rx_logic.h"



namespace rx_platform {

namespace logic {

// Class rx_platform::logic::ladder_program 

ladder_program::ladder_program()
{
}

ladder_program::ladder_program (const string_type& name, const rx_node_id& id, bool system)
{
}


ladder_program::~ladder_program()
{
}



bool ladder_program::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	if (!program_runtime::serialize_definition(stream, type))
		return false;

	return true;
}

bool ladder_program::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!program_runtime::deserialize_definition(stream, type))
		return false;

	return true;
}

string_type ladder_program::get_type_name () const
{
  return type_name;

}


// Class rx_platform::logic::program_runtime 

string_type program_runtime::type_name = "PROGRAM";

program_runtime::program_runtime()
      : _created_time(rx_time::now()),
        _modified_time(rx_time::now())
{
}

program_runtime::program_runtime (const string_type& name, const rx_node_id& id, bool system)
      : _created_time(rx_time::now()),
        _modified_time(rx_time::now())
	, program_runtime_t(name,id,system)
{
}


program_runtime::~program_runtime()
{
}



bool program_runtime::serialize_definition (base_meta_writter& stream, uint8_t type) const
{
	return true;
}

bool program_runtime::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return true;
}

string_type program_runtime::get_type_name () const
{
  return type_name;

}

namespace_item_attributes program_runtime::get_attributes () const
{
	return (namespace_item_attributes)(
		namespace_item_write_access 
		| namespace_item_system 
		| namespace_item_execute_access 
		| namespace_item_program 
		| namespace_item_read_access);
}

bool program_runtime::save_program (base_meta_writter& stream, uint8_t type) const
{
	return serialize_definition(stream, type);
}

bool program_runtime::load_program (base_meta_reader& stream, uint8_t type)
{
	return deserialize_definition(stream, type);
}

void program_runtime::get_value (values::rx_value& val) const
{
	val = rx_value(0u, get_modified_time());
}

const string_type& program_runtime::get_item_name () const
{
	return get_name();
}


} // namespace logic
} // namespace rx_platform
