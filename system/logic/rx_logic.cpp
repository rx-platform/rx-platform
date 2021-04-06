

/****************************************************************************
*
*  system\logic\rx_logic.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_logic
#include "system/logic/rx_logic.h"

#include "rx_configuration.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace logic {

// Class rx_platform::logic::program_runtime 

program_runtime::program_runtime()
{
}

program_runtime::program_runtime (const string_type& name, const rx_node_id& id)
{
	name_ = name;
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

bool program_runtime::save_program (base_meta_writer& stream, uint8_t type) const
{
	return serialize(stream, type);
}

bool program_runtime::load_program (base_meta_reader& stream, uint8_t type)
{
	return deserialize(stream, type);
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

void program_runtime::process_program (runtime::runtime_process_context& ctx)
{
}


sl_runtime::sl_program_holder& program_runtime::my_program ()
{
  return my_program_;
}


// Class rx_platform::logic::ladder_program 

ladder_program::ladder_program()
{
}

ladder_program::ladder_program (const string_type& name, const rx_node_id& id)
	: program_runtime(name,id)
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


// Class rx_platform::logic::method_runtime 

method_runtime::method_runtime()
{
}

method_runtime::method_runtime (const string_type& name, const rx_node_id& id)
{
}


method_runtime::~method_runtime()
{
}



rx_result method_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result method_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result method_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result method_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}


} // namespace logic
} // namespace rx_platform

