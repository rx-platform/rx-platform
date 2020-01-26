

/****************************************************************************
*
*  system\runtime\rx_blocks.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_blocks
#include "system/runtime/rx_blocks.h"

#include "rx_configuration.h"


namespace rx_platform {

namespace runtime {

namespace blocks {

// Class rx_platform::runtime::blocks::filter_runtime 

string_type filter_runtime::type_name = RX_CPP_FILTER_TYPE_NAME;

filter_runtime::filter_runtime()
{
}



string_type filter_runtime::get_type_name () const
{
  return type_name;

}

rx_result filter_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result filter_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result filter_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result filter_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::mapper_runtime 

string_type mapper_runtime::type_name = RX_CPP_MAPPER_TYPE_NAME;

mapper_runtime::mapper_runtime()
{
}


mapper_runtime::~mapper_runtime()
{
}



string_type mapper_runtime::get_type_name () const
{
  return type_name;

}

rx_result mapper_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result mapper_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result mapper_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result mapper_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::source_runtime 

string_type source_runtime::type_name = RX_CPP_SOURCE_TYPE_NAME;

source_runtime::source_runtime()
      : input_(false),
        output_(false)
{
}


source_runtime::~source_runtime()
{
}



string_type source_runtime::get_type_name () const
{
  return type_name;

}

rx_result source_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	my_variable_ = ctx.variables.get_current_variable();
	return true;
}

rx_result source_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	my_variable_ = rx_variable_ptr::null_ptr;
	return true;
}

rx_result source_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result source_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::struct_runtime 

string_type struct_runtime::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_runtime::struct_runtime()
{
}

struct_runtime::struct_runtime (const string_type& name, const rx_node_id& id, bool system)
{
}



bool struct_runtime::serialize_definition (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!stream.start_object(struct_runtime::type_name.c_str()))
		return false;/*
	if (!complex_runtime_item::serialize_definition(stream, type, ts, mode))
		return false;*/
	if (!stream.end_object())
		return false;
	return true;
}

bool struct_runtime::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	RX_ASSERT(false);//
	return true;
}

string_type struct_runtime::get_type_name () const
{
  return type_name;

}

rx_result struct_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result struct_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result struct_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result struct_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::event_runtime 

string_type event_runtime::type_name = RX_CPP_EVENT_TYPE_NAME;

event_runtime::event_runtime()
{
}



string_type event_runtime::get_type_name () const
{
  return type_name;

}

rx_result event_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result event_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result event_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result event_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::variable_runtime 

string_type variable_runtime::type_name = RX_CPP_VARIABLE_TYPE_NAME;

variable_runtime::variable_runtime()
{
}

variable_runtime::variable_runtime (const string_type& name, const rx_node_id& id, bool system)
{
}


variable_runtime::~variable_runtime()
{
}



string_type variable_runtime::get_type_name () const
{
  return type_name;

}

rx_result variable_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result variable_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result variable_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result variable_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


} // namespace blocks
} // namespace runtime
} // namespace rx_platform

