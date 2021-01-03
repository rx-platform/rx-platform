

/****************************************************************************
*
*  system\logic\rx_logic.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_logic
#include "system/logic/rx_logic.h"

#include "rx_configuration.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace logic {

// Class rx_platform::logic::program_runtime 

rx_item_type program_runtime::type_id = rx_program;

program_runtime::program_runtime()
{
}

program_runtime::program_runtime (const string_type& name, const rx_node_id& id, bool system)
{
	meta_info_.name = name;
	meta_info_.id = id;
	meta_info_.attributes = create_attributes_from_flags(system);
}


program_runtime::~program_runtime()
{
}



bool program_runtime::serialize (base_meta_writer& stream, uint8_t type) const
{
	return true;
}

bool program_runtime::deserialize (base_meta_reader& stream, uint8_t type)
{
	return true;
}

namespace_item_attributes program_runtime::get_attributes () const
{
	return (namespace_item_attributes)(
		namespace_item_write_access
		| namespace_item_system
		| namespace_item_execute_access
		| namespace_item_read_access);
}

bool program_runtime::save_program (base_meta_writer& stream, uint8_t type) const
{
	return serialize(stream, type);
}

bool program_runtime::load_program (base_meta_reader& stream, uint8_t type)
{
	return deserialize(stream, type);
}

void program_runtime::get_value (values::rx_value& val) const
{
	val.assign_static(0u, meta_info_.modified_time);
}

platform_item_ptr program_runtime::get_item_ptr () const
{
	return std::make_unique<rx_internal::internal_ns::rx_other_implementation<smart_ptr> >(smart_this());
}

values::rx_value program_runtime::get_value () const
{
	rx_value temp;
	temp.assign_static(0u, meta_info_.modified_time);
	return temp;
}

rx_time program_runtime::get_created_time () const
{
	return meta_info_.created_time;
}

string_type program_runtime::get_name () const
{
	return meta_info_.name;
}

size_t program_runtime::get_size () const
{
	return sizeof(*this);
}

namespace_item_attributes program_runtime::create_attributes_from_flags (bool system)
{
	if (system)
	{
		return namespace_item_system_access | namespace_item_execute_access;
	}
	else
	{
		return namespace_item_full_access | namespace_item_execute_access;
	}
}

rx_result program_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result program_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result program_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result program_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}


sl_runtime::sl_program_holder& program_runtime::my_program ()
{
  return my_program_;
}

const meta::meta_data& program_runtime::meta_info () const
{
  return meta_info_;
}


// Class rx_platform::logic::ladder_program 

ladder_program::ladder_program()
{
}

ladder_program::ladder_program (const string_type& name, const rx_node_id& id, bool system)
	: program_runtime(name,id,system)
{
}


ladder_program::~ladder_program()
{
}



bool ladder_program::serialize (base_meta_writer& stream, uint8_t type) const
{
	if (!program_runtime::serialize(stream, type))
		return false;

	return true;
}

bool ladder_program::deserialize (base_meta_reader& stream, uint8_t type)
{
	if (!program_runtime::deserialize(stream, type))
		return false;

	return true;
}


} // namespace logic
} // namespace rx_platform

