

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
#include "rx_runtime_helpers.h"
#include "rx_rt_struct.h"
#include "rx_objbase.h"
#include "model/rx_model_algorithms.h"


namespace rx_platform {

namespace runtime {

namespace blocks {

// Class rx_platform::runtime::blocks::filter_runtime 

string_type filter_runtime::type_name = RX_CPP_FILTER_TYPE_NAME;

filter_runtime::filter_runtime()
      : container_(nullptr)
{
}


filter_runtime::~filter_runtime()
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

bool filter_runtime::supports_input () const
{
  return true;

}

bool filter_runtime::supports_output () const
{
  return true;

}

bool filter_runtime::is_input () const
{
    return supports_input() || io_.get_input();
}

bool filter_runtime::is_output () const
{
    return supports_output() || io_.get_output();
}

rx_result filter_runtime::filter_write_value (rx_simple_value& val)
{
    return true;
}

rx_result filter_runtime::filter_read_value (rx_value& val)
{
    return true;
}


// Class rx_platform::runtime::blocks::struct_runtime 

string_type struct_runtime::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_runtime::struct_runtime()
{
}


struct_runtime::~struct_runtime()
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


// Class rx_platform::runtime::blocks::variable_runtime 

string_type variable_runtime::type_name = RX_CPP_VARIABLE_TYPE_NAME;

variable_runtime::variable_runtime()
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

rx_result variable_runtime::write_value (rx_simple_value&& val, const structure::write_context& ctx, runtime_sources_type& sources)
{
    RX_ASSERT(!sources.empty());
    rx_result ret = RX_NOT_SUPPORTED;
    if (sources.size() == 1)
    {
        if (sources[0].is_output())
            ret = sources[0].write_value(rx_simple_value(val), ctx);
    }
    else
    {
        for (auto& one : sources)
        {
            if (one.is_output())
            {
                ret = one.write_value(rx_simple_value(val), ctx);
                if (ret)
                    break;
            }
        }
    }
    return ret;
}

rx_value variable_runtime::select_input (algorithms::runtime_process_context* ctx, runtime_sources_type& sources)
{
    rx_value ret;
    for (auto& one : sources)
    { 
        if (one.is_input())
        {
            ret = one.source_ptr->get_current_value();
            if (ret.is_good())
                return ret;
        }
    }
    return ret;
}


// Class rx_platform::runtime::blocks::event_runtime 

string_type event_runtime::type_name = RX_CPP_EVENT_TYPE_NAME;

event_runtime::event_runtime()
{
}


event_runtime::~event_runtime()
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


// Class rx_platform::runtime::blocks::source_runtime 

string_type source_runtime::type_name = RX_CPP_SOURCE_TYPE_NAME;

source_runtime::source_runtime()
      : container_(nullptr)
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
	return true;
}

rx_result source_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
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

rx_result source_runtime::write_value (rx_simple_value&& val, const structure::write_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result source_runtime::filter_output_value (rx_simple_value& val, runtime_filters_type& filters)
{
    RX_ASSERT(!filters.empty());
    return RX_NOT_IMPLEMENTED;
}

rx_result source_runtime::filter_input_value (rx_simple_value& val, runtime_filters_type& filters)
{
    RX_ASSERT(!filters.empty());
    return RX_NOT_IMPLEMENTED;
}

bool source_runtime::supports_input () const
{
  return true;

}

bool source_runtime::supports_output () const
{
  return true;

}

bool source_runtime::is_input () const
{
    return supports_input() || io_.get_input();
}

bool source_runtime::is_output () const
{
    return supports_output() || io_.get_output();
}

rx_result source_runtime::value_changed (rx_value&& val)
{
    if (container_)
        container_->source_update_pending(std::move(val));
    return true;
}

const rx_value& source_runtime::get_current_value () const
{
    return current_value_;
}


// Class rx_platform::runtime::blocks::mapper_runtime 

string_type mapper_runtime::type_name = RX_CPP_MAPPER_TYPE_NAME;

mapper_runtime::mapper_runtime()
      : container_(nullptr)
{
}


mapper_runtime::~mapper_runtime()
{
}



string_type mapper_runtime::get_type_name () const
{
  return type_name;

}

rx_result mapper_runtime::initialize_mapper (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result mapper_runtime::deinitialize_mapper (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result mapper_runtime::start_mapper (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result mapper_runtime::stop_mapper (runtime::runtime_stop_context& ctx)
{
	return true;
}

void mapper_runtime::mapper_write_pending (values::rx_simple_value&& value, runtime_transaction_id_t id)
{
    if (container_)
        container_->mapper_write_pending(std::move(value), id);
}

bool mapper_runtime::supports_read () const
{
  return true;

}

bool mapper_runtime::supports_write () const
{
  return true;

}

bool mapper_runtime::can_read () const
{
    return supports_read() || io_.get_input();
}

bool mapper_runtime::can_write () const
{
    return supports_write() || io_.get_output();
}

void mapper_runtime::mapped_value_changed (rx_value&& val)
{
}

void mapper_runtime::mapper_write_result (rx_result&& result, runtime_transaction_id_t id)
{
}

void mapper_runtime::map_current_value () const
{
    if (container_)
    {
        container_->value_changed(rx_value());
    }
}


} // namespace blocks
} // namespace runtime
} // namespace rx_platform

