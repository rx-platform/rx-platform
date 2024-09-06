

/****************************************************************************
*
*  system\runtime\rx_blocks.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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

filter_runtime::filter_runtime (lock_reference_struct* extern_data)
      : container_(nullptr)
    , reference_object(extern_data)
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

rx_result filter_runtime::filter_changed ()
{
    if (container_)
    {
        return container_->filter_changed();
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

mapper_runtime::mapper_runtime (lock_reference_struct* extern_data)
      : container_(nullptr),
        value_type_(RX_NULL_TYPE)
    , reference_object(extern_data)
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
    {
        if (rx_is_debug_instance())
        {
            static string_type message;
            static char* message_buffer = nullptr;
            if (message.empty())
            {
                std::ostringstream ss;
                ss << "Mapper "
                    << container_->full_path
                    << " started write with id:%08X";
                message = ss.str();
                message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/];
            }
            sprintf(message_buffer, message.c_str(), data.transaction_id);
            RUNTIME_LOG_DEBUG("mapper_runtime", 500, message_buffer);
        }
        container_->mapper_write_pending(std::move(data));
    }
}

void mapper_runtime::mapper_execute_pending (execute_data&& data)
{
    if (container_)
    {
        if (rx_is_debug_instance())
        {
            static string_type message;
            static char* message_buffer = nullptr;
            if (message.empty())
            {
                std::ostringstream ss;
                ss << "Mapper "
                    << container_->full_path
                    << " started execute with id:%08X";
                message = ss.str();
                message_buffer = new char[message.size() + 0x10/*This will hold digits and just a bit reserve*/];
            }
            sprintf(message_buffer, message.c_str(), data.transaction_id);
            RUNTIME_LOG_DEBUG("mapper_runtime", 500, message_buffer);
        }
        container_->mapper_execute_pending(std::move(data));
    }
}

void mapper_runtime::map_current_value () const
{
    if (container_)
    {
        container_->value_changed(rx_value());
    }
}

void mapper_runtime::mapped_value_changed (rx_value&& val, runtime_process_context* ctx)
{
}

void mapper_runtime::mapper_result_received (rx_result&& result, runtime_transaction_id_t id, runtime_process_context* ctx)
{
}

void mapper_runtime::mapped_event_fired (rx_timed_value val, string_view_type queue, bool state, bool remove, runtime_process_context* ctx)
{
}

void mapper_runtime::mapper_execute_result_received (rx_result&& result, values::rx_simple_value out_data, runtime_transaction_id_t id, runtime_process_context* ctx)
{
}

std::vector<rx_simple_value> mapper_runtime::get_mapping_values (runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path) const
{
    return ctx.mappers.get_mapping_values(id, path);
}

data::runtime_data_model mapper_runtime::get_method_inputs ()
{
    if (container_)
    {
        return container_->get_method_inputs();
    }
    else
    {
        RX_ASSERT(false);
        return data::runtime_data_model();
    }
}

data::runtime_data_model mapper_runtime::get_method_outputs ()
{
    if (container_)
    {
        return container_->get_method_outputs();
    }
    else
    {
        RX_ASSERT(false);
        return data::runtime_data_model();
    }
}

data::runtime_data_model mapper_runtime::get_event_arguments ()
{
    if (container_)
    {
        return container_->get_event_arguments();
    }
    else
    {
        RX_ASSERT(false);
        return data::runtime_data_model();
    }
}

data::runtime_data_model mapper_runtime::get_data_type ()
{
    if (container_)
    {
        return container_->get_data_type();
    }
    else
    {
        RX_ASSERT(false);
        return data::runtime_data_model();
    }
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

source_runtime::source_runtime (lock_reference_struct* extern_data)
      : container_(nullptr),
        value_type_(RX_NULL_TYPE)
    , reference_object(extern_data)
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
    {
        container_->source_result_pending(std::move(result), id);
    }
}

void source_runtime::source_execute_result_received (rx_simple_value out_val, rx_result&& result, runtime_transaction_id_t id)
{
}

rx_result source_runtime::source_write (write_data&& data, runtime_process_context* ctx)
{
    return RX_NOT_SUPPORTED;
}

rx_result source_runtime::source_execute (execute_data&& data, runtime_process_context* ctx)
{
    return RX_NOT_SUPPORTED;
}

std::vector<rx_simple_value> source_runtime::get_source_values (runtime::runtime_init_context& ctx, const rx_node_id& id, const string_type& path) const
{
    return ctx.sources.get_source_values(id, path);
}

data::runtime_data_model source_runtime::get_data_type ()
{
    if (container_)
    {
        return container_->get_data_type();
    }
    else
    {
        RX_ASSERT(false);
        return data::runtime_data_model();
    }
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

struct_runtime::struct_runtime (lock_reference_struct* extern_data)
    : reference_object(extern_data)
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


// Class rx_platform::runtime::blocks::event_runtime 

string_type event_runtime::type_name = RX_CPP_EVENT_TYPE_NAME;

event_runtime::event_runtime()
      : container_(nullptr)
{
}

event_runtime::event_runtime (lock_reference_struct* extern_data)
      : container_(nullptr)
    , reference_object(extern_data)
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

void event_runtime::event_fired (event_fired_data&& data)
{
    if(container_)
    {
        container_->event_fired(std::move(data));
    }
    else
    {
        RX_ASSERT(false);
    }
}

data::runtime_data_model event_runtime::get_arguemnts ()
{
    if (container_)
    {
        return container_->get_arguments();
    }
    else
    {
        RX_ASSERT(false);
        return data::runtime_data_model();
    }
}


// Class rx_platform::runtime::blocks::variable_runtime 

string_type variable_runtime::type_name = RX_CPP_VARIABLE_TYPE_NAME;

variable_runtime::variable_runtime()
      : container_(nullptr)
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

void variable_runtime::process_variable (runtime_process_context* ctx)
{
    ctx->variable_pending(container_);
}

void variable_runtime::send_write_result (structure::write_task* task, rx_result result)
{
    container_->send_write_result(task, std::move(result));
}

void variable_runtime::process_result (runtime_transaction_id_t id, rx_result&& result)
{
}

void variable_runtime::post_process_value (const rx_value& val)
{
}


// Class rx_platform::runtime::blocks::data_type_runtime 

string_type data_type_runtime::type_name = RX_CPP_DATA_TYPE_NAME;

data_type_runtime::data_type_runtime()
{
}

data_type_runtime::data_type_runtime (lock_reference_struct* extern_data)
    : reference_object(extern_data)
{
}


data_type_runtime::~data_type_runtime()
{
}



string_type data_type_runtime::get_type_name () const
{
  return type_name;

}

rx_result data_type_runtime::initialize_data_type (runtime::runtime_init_context& ctx, const data::runtime_data_model& data)
{
    return true;
}

rx_result data_type_runtime::start_data_type (runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result data_type_runtime::stop_data_type (runtime::runtime_stop_context& ctx)
{
    return true;
}

rx_result data_type_runtime::deinitialize_data_type (runtime::runtime_deinit_context& ctx)
{
    return true;
}


} // namespace blocks
} // namespace runtime
} // namespace rx_platform

