

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

rx_result filter_runtime::initialize_filter (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result filter_runtime::start_filter (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result filter_runtime::stop_filter (runtime::runtime_stop_context& ctx)
{
	return true;
}

rx_result filter_runtime::deinitialize_filter (runtime::runtime_deinit_context& ctx)
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

rx_result filter_runtime::filter_input (rx_value& val)
{
    return true;
}

rx_result filter_runtime::filter_output (rx_simple_value& val)
{
    return true;
}

rx_result filter_runtime::get_value (runtime_handle_t handle, values::rx_simple_value& val) const
{
    if (container_)
    {
        return container_->get_value(handle, val);
    }
    else
    {
        RX_ASSERT(false);
        return "Context not binded!";
    }
}

rx_result filter_runtime::set_value (runtime_handle_t handle, values::rx_simple_value&& val)
{
    if (container_)
    {
        return container_->set_value(handle, std::move(val));
    }
    else
    {
        RX_ASSERT(false);
        return "Context not binded!";
    }
}


// Class rx_platform::runtime::blocks::mapper_runtime 

string_type mapper_runtime::type_name = RX_CPP_MAPPER_TYPE_NAME;

mapper_runtime::mapper_runtime()
      : container_(nullptr),
        value_type_(RX_NULL_TYPE)
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

rx_result mapper_runtime::start_mapper (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result mapper_runtime::stop_mapper (runtime::runtime_stop_context& ctx)
{
	return true;
}

rx_result mapper_runtime::deinitialize_mapper (runtime::runtime_deinit_context& ctx)
{
	return true;
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
    return container_ ? container_->can_read() : false;
}

bool mapper_runtime::can_write () const
{
    return container_ ? container_->can_write() : false;
}

void mapper_runtime::mapper_write_pending (write_data&& data)
{
    if (container_)
        container_->mapper_write_pending(std::move(data));
}

void mapper_runtime::map_current_value () const
{
    if (container_)
    {
        container_->value_changed(rx_value());
    }
}

void mapper_runtime::mapped_value_changed (rx_value&& val)
{
}

void mapper_runtime::mapper_result_received (rx_result&& result, runtime_transaction_id_t id)
{
}

threads::job_thread* mapper_runtime::get_jobs_queue ()
{
    return nullptr;
    /*if (container_)
    {
        return rx_internal::infrastructure::server_runtime::instance().get_executer(container_->);
    }*/
}

std::vector<rx_value> mapper_runtime::get_mapped_values (runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path)
{
    return ctx.mappers.get_mapped_values(id, path);
}


rx_value_t mapper_runtime::get_value_type () const
{
  return value_type_;
}


// Class rx_platform::runtime::blocks::source_runtime 

string_type source_runtime::type_name = RX_CPP_SOURCE_TYPE_NAME;

source_runtime::source_runtime()
      : container_(nullptr),
        value_type_(RX_NULL_TYPE)
{
}


source_runtime::~source_runtime()
{
}



string_type source_runtime::get_type_name () const
{
  return type_name;

}

rx_result source_runtime::initialize_source (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result source_runtime::start_source (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result source_runtime::stop_source (runtime::runtime_stop_context& ctx)
{
	return true;
}

rx_result source_runtime::deinitialize_source (runtime::runtime_deinit_context& ctx)
{
	return true;
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
    return container_ ? container_->is_input() : false;
}

bool source_runtime::is_output () const
{
    return container_ ? container_->is_output() : false;
}

rx_result source_runtime::source_value_changed (rx_value&& val)
{
    if (container_)
        container_->source_update_pending(std::move(val));
    return true;
}

void source_runtime::source_result_received (rx_result&& result, runtime_transaction_id_t id)
{
    if (container_)
        container_->source_result_pending(std::move(result), id);
}

rx_result source_runtime::source_write (write_data&& data, runtime_process_context* ctx)
{
    return RX_NOT_IMPLEMENTED;
}

threads::job_thread* source_runtime::get_jobs_queue ()
{
    if (container_)
        return container_->get_jobs_queue();
    else
        return nullptr;
}

void source_runtime::add_periodic_job (jobs::periodic_job::smart_ptr job)
{
    if (container_)
        container_->add_periodic_job(job);
}


rx_value_t source_runtime::get_value_type () const
{
  return value_type_;
}


// Class rx_platform::runtime::blocks::struct_runtime 

string_type struct_runtime::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_runtime::struct_runtime()
{
}


struct_runtime::~struct_runtime()
{
}



string_type struct_runtime::get_type_name () const
{
  return type_name;

}

rx_result struct_runtime::initialize_struct (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result struct_runtime::start_struct (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result struct_runtime::stop_struct (runtime::runtime_stop_context& ctx)
{
	return true;
}

rx_result struct_runtime::deinitialize_struct (runtime::runtime_deinit_context& ctx)
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

rx_result variable_runtime::initialize_variable (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result variable_runtime::deinitialize_variable (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result variable_runtime::start_variable (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result variable_runtime::stop_variable (runtime::runtime_stop_context& ctx)
{
	return true;
}

rx_value variable_runtime::select_variable_input (runtime_process_context* ctx, runtime_sources_type& sources)
{
    rx_value ret;
    for (auto& one : sources)
    {
        if (one.is_input())
        {
            ret = one.get_current_value();
                return ret;
        }
    }
    return ret;
}

rx_result variable_runtime::variable_write (write_data&& data, runtime_process_context* ctx, runtime_sources_type& sources)
{
    RX_ASSERT(!sources.empty());
    rx_result ret = RX_NOT_SUPPORTED;
    if (sources.size() == 1)
    {
        if (sources[0].is_output())
            ret = sources[0].write_value(std::move(data));
    }
    else
    {
        write_data data_copy(data);
        for (auto& one : sources)
        {
            if (one.is_output())
            {
                ret = one.write_value(write_data(data_copy));
                if (ret)
                    break;
            }
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

rx_result event_runtime::initialize_event (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result event_runtime::start_event (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result event_runtime::stop_event (runtime::runtime_stop_context& ctx)
{
	return true;
}

rx_result event_runtime::deinitialize_event (runtime::runtime_deinit_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::mapper_start_context 


rx_value& mapper_start_context::init_value ()
{
    return *initial_value_;
}


// Class rx_platform::runtime::blocks::mapper_stop_context 


} // namespace blocks
} // namespace runtime
} // namespace rx_platform

