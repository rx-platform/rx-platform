

/****************************************************************************
*
*  system\runtime\rx_process_context.cpp
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


// rx_operational
#include "system/runtime/rx_operational.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"

#include "rx_blocks.h"
#include "system/logic/rx_logic.h"



namespace rx_platform {

namespace runtime {
namespace
{
async_values_type g_empty_async_values;
variables_type g_empty_variables;
block_variables_type g_empty_block_variables;
structs_type g_empty_structs;
source_writes_type g_empty_source_writes;
source_updates_type g_empty_source_updates;
source_results_type g_empty_source_results;
variable_results_type g_empty_variable_results;
block_variable_results_type g_empty_block_variable_results;
programs_type g_empty_programs;
owner_jobs_type g_empty_jobs;
mapper_writes_type g_empty_mapper_writes;
mapper_executes_type g_empty_mapper_executes;
mapper_updates_type g_empty_mapper_updates;
filters_type g_empty_filters;
events_type g_empty_events;
method_results_type g_empty_method_results;
}


template<runtime_process_step step>
void runtime_process_context::turn_on_pending()
{
    if (!pending_steps_.test((uint_fast8_t)step))
    {
        pending_steps_[(uint_fast8_t)step] = true;
        if (current_step_ == runtime_process_step::idle)
            fire_callback_();
    }
}
template<runtime_process_step step>
bool runtime_process_context::should_do_step()
{
    RX_ASSERT(current_step_ == step);
    if (pending_steps_.test((uint_fast8_t)step))
    {
        pending_steps_[(uint_fast8_t)step] = false;
        return true;
    }
    else
    {
        current_step_ = (runtime_process_step)((uint_fast8_t)(runtime_process_step)current_step_ + 1);
        return false;
    }
}

// Class rx_platform::runtime::runtime_process_context 

runtime_process_context::runtime_process_context (tag_blocks::binded_tags& binded, tag_blocks::connected_tags& tags, const meta_data& info, ns::rx_directory_resolver* dirs, rx_reference_ptr anchor, security::security_guard_ptr guard)
      : tags_(tags),
        binded_(binded),
        current_step_(runtime_process_step::idle),
        now_(rx_time::now().c_data().t_value),
        meta_info(info),
        directory_resolver_(dirs),
        serialize_value_(false),
        stopping_(false),
        job_queue_(nullptr),
        anchor_(anchor),
        security_guard_(guard)
{
    mode_.turn_off();
    mode_time_ = rx_time_struct_t{ now_ };
}



rx_result runtime_process_context::init_context ()
{
    now_ = rx_time::now().c_data().t_value;
    current_step_ = runtime_process_step::async_values;
    return true;
}

void runtime_process_context::init_state (fire_callback_func_t fire_callback)
{
    fire_callback_ = fire_callback;
    mode_.turn_on();
}

void runtime_process_context::runtime_stopped ()
{
    locks::auto_lock_t _(&context_lock_);
    stopping_ = true;
}

void runtime_process_context::runtime_deinitialized ()
{
    anchor_ = rx_reference_ptr::null_ptr;
}

rx_result runtime_process_context::get_value (runtime_handle_t handle, values::rx_simple_value& val) const
{
    return binded_.get_value(handle, val);
}

rx_result runtime_process_context::set_value (runtime_handle_t handle, values::rx_simple_value&& val, tag_blocks::binded_write_result_callback_t callback)
{
    return binded_.set_value(handle, std::move(val), tags_, this, std::move(callback));
}

rx_result runtime_process_context::set_item (const string_type& path, values::rx_simple_value&& what, runtime_init_context& ctx)
{
    return binded_.set_item(path, std::move(what), ctx);
}

bool runtime_process_context::should_repeat ()
{
    locks::auto_lock_t _(&context_lock_);
    RX_ASSERT(current_step_ == runtime_process_step::beyond_last);
    bool ret =  pending_steps_.to_ulong() != 0;
    if (ret)
        return init_context();
    else
        current_step_ = runtime_process_step::idle;
    return ret;
}

void runtime_process_context::async_value_pending (async_data data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::async_values>();
    async_values_.emplace_back(std::move(data));
}

void runtime_process_context::status_change_pending ()
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::status_change>();
}

void runtime_process_context::source_result_pending (write_result_struct<structure::source_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::source_inputs>();
    source_results_.emplace_back(std::move(data));
}

void runtime_process_context::source_update_pending (update_data_struct<structure::source_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::source_inputs>();
    source_inputs_.emplace_back(std::move(data));
}

void runtime_process_context::mapper_write_pending (write_data_struct<structure::mapper_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::mapper_inputs>();
    mapper_inputs_.emplace_back(std::move(data));
}

void runtime_process_context::mapper_execute_pending (execute_data_struct<structure::mapper_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::mapper_inputs>();
    mapper_executes_.emplace_back(std::move(data));
}

void runtime_process_context::tag_writes_pending ()
{
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::tag_inputs>();
}

void runtime_process_context::variable_pending (structure::variable_data* whose)
{
    RX_ASSERT(whose->value.get_type() != RX_STRUCT_TYPE);
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::variables>();
    variables_.emplace_back(std::move(whose));
}

void runtime_process_context::variable_result_pending (write_result_struct<structure::variable_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::variables>();
    variable_results_.emplace_back(std::move(data));
}

void runtime_process_context::variable_pending (structure::variable_block_data* whose)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::variables>();
    block_variables_.emplace_back(std::move(whose));
}

void runtime_process_context::variable_result_pending (write_result_struct<structure::variable_block_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::variables>();
    block_variable_results_.emplace_back(std::move(data));
}

void runtime_process_context::method_result_pending (method_execute_result_data data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::programs>();
    method_results_.emplace_back(std::move(data));
}

void runtime_process_context::program_pending (logic_blocks::program_data* whose)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::programs>();
    programs_.emplace_back(whose);
}

void runtime_process_context::filter_pending (structure::filter_data* whose)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::filters>();
    filters_.emplace_back(whose);
}

void runtime_process_context::own_pending (job_ptr what)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::own>();
    owns_.emplace_back(std::move(what));
}

void runtime_process_context::tag_updates_pending ()
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::tag_outputs>();
}

void runtime_process_context::mapper_update_pending (update_data_struct<structure::mapper_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::mapper_outputs>();
    mapper_outputs_.emplace_back(std::move(data));
}

void runtime_process_context::source_write_pending (write_data_struct<structure::source_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::source_outputs>();
    source_outputs_.emplace_back(std::move(data));
}

bool runtime_process_context::should_process_tag_updates ()
{
    locks::auto_lock_t _(&context_lock_);
    return should_do_step<runtime_process_step::tag_outputs>();
}

bool runtime_process_context::should_process_tag_writes ()
{
    locks::auto_lock_t _(&context_lock_);
    return should_do_step<runtime_process_step::tag_inputs>();
}

std::pair<mapper_writes_type*, mapper_executes_type*> runtime_process_context::get_mapper_inputs ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::mapper_inputs>())
        return { &mapper_inputs_.get_and_swap() , &mapper_executes_.get_and_swap() };
    else
        return { &g_empty_mapper_writes, &g_empty_mapper_executes };
}

mapper_updates_type& runtime_process_context::get_mapper_updates ()
{
    if (should_do_step<runtime_process_step::mapper_outputs>())
        return mapper_outputs_.get_and_swap();
    else
        return g_empty_mapper_updates;
}

source_writes_type& runtime_process_context::get_source_writes ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::source_outputs>())
        return source_outputs_.get_and_swap();
    else
        return g_empty_source_writes;
}

variable_data_for_process_t runtime_process_context::get_variables_for_process ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::variables>())
        return variable_data_for_process_t{
        &variable_results_.get_and_swap() ,
        &variables_.get_and_swap(),
        &block_variable_results_.get_and_swap() ,
        &block_variables_.get_and_swap() };
    else
        return variable_data_for_process_t{ &g_empty_variable_results, &g_empty_variables, &g_empty_block_variable_results, &g_empty_block_variables };
}

bool runtime_process_context::should_process_status_change ()
{
    locks::auto_lock_t _(&context_lock_);
    return should_do_step<runtime_process_step::status_change>();
}

std::pair<method_results_type*, programs_type*> runtime_process_context::get_logic_for_process ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::programs>())
        return { &method_results_.get_and_swap() , &programs_.get_and_swap() };
    else
        return { &g_empty_method_results, &g_empty_programs };
}

filters_type& runtime_process_context::get_filters_for_process ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::filters>())
        return filters_.get_and_swap();
    else
        return g_empty_filters;
}

void runtime_process_context::variable_value_changed (structure::variable_data* whose, const values::rx_value& val)
{
    rx_value adapted_val = adapt_value(val);
    /*if(is_mine_value(val))
        binded_.variable_change(whose, adapted_val);*/
    tags_.variable_change(whose, std::move(adapted_val));
}

void runtime_process_context::variable_block_value_changed (structure::variable_block_data* whose, const values::rx_value& val)
{
    rx_value adapted_val = adapt_value(val);
    /*if (is_mine_value(val))
        binded_.block_variable_change(whose, adapted_val);*/
    tags_.variable_block_change(whose, std::move(adapted_val));
}

void runtime_process_context::event_pending (event_fired_struct<structure::event_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    if (stopping_)
        return;
    turn_on_pending<runtime_process_step::events>();
    events_.emplace_back(std::move(data));
}

events_type& runtime_process_context::get_events_for_process ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::events>())
        return events_.get_and_swap();
    else
        return g_empty_events;
}

void runtime_process_context::struct_pending (structure::event_data* whose)
{
}

structs_type& runtime_process_context::get_structs_for_process ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::structs>())
        return structs_.get_and_swap();
    else
        return g_empty_structs;
}

rx_value runtime_process_context::adapt_value (const rx_value& from) const
{
    rx_value ret(from);
    from.get_value(ret, mode_time_, mode_);
    return ret;
}

rx_value runtime_process_context::adapt_value (const rx_timed_value& from) const
{
    rx_value ret;
    from.get_value(ret, mode_time_, mode_);
    return ret;
}

rx_value runtime_process_context::adapt_value (const rx_simple_value& from) const
{
    rx_value ret;
    from.get_value(ret, mode_time_, mode_);
    return ret;
}

rx_result runtime_process_context::do_command (rx_object_command_t command_type)
{
    locks::auto_lock_t _(&const_cast<runtime_process_context*>(this)->context_lock_);
    switch (command_type)
    {
    case rx_object_command_t::rx_turn_off:
        {
            if (mode_.turn_off())
            {
                mode_time_ = rx_time::now();
                status_change_pending();
                return true;
            }
        }
        break;
    case rx_object_command_t::rx_turn_on:
        {
            if (mode_.turn_on())
            {
                mode_time_ = rx_time::now();
                status_change_pending();
                return true;
            }
        }
        break;
    case rx_object_command_t::rx_set_blocked:
        {
            if (mode_.set_blocked())
            {
                mode_time_ = rx_time::now();
                status_change_pending();
                return true;
            }
        }
        break;
    case rx_object_command_t::rx_reset_blocked:
        {
            if (mode_.reset_blocked())
            {
                mode_time_ = rx_time::now();
                status_change_pending();
                return true;
            }
        }
        break;
    case rx_object_command_t::rx_set_test:
        {
            if (mode_.set_test())
            {
                mode_time_ = rx_time::now();
                status_change_pending();
                return true;
            }
        }
        break;
    case rx_object_command_t::rx_reset_test:
        {
            if (mode_.reset_test())
            {
                mode_time_ = rx_time::now();
                status_change_pending();
                return true;
            }
        }
        break;
    default:
        return "Unsupported command type!";
    }
    return "Error executing command!";
}

owner_jobs_type& runtime_process_context::get_for_own_process ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::own>())
        return owns_.get_and_swap();
    else
        return g_empty_jobs;
}

runtime_handle_t runtime_process_context::connect (const string_type& path, uint32_t rate, std::function<void(const rx_value&)> callback, runtime_start_context& ctx)
{
    //if (points_)
    //{
    //    runtime_handle_t ret = (runtime_handle_t)points_->size() + 1;// avoid zero
    //    auto point = points_->emplace_back(value_point());
    //    point.connect(path, rate, callback);
    //    return ret;
    //}
    //else
    {
        return 0;
    }
}

std::pair<source_results_type*, source_updates_type*> runtime_process_context::get_source_inputs ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::source_inputs>())
        return { &source_results_.get_and_swap() , &source_inputs_.get_and_swap() };
    else
        return { &g_empty_source_results, &g_empty_source_updates };
}

void runtime_process_context::runtime_dirty ()
{
    serialize_value_ = true;
}

bool runtime_process_context::should_save ()
{
    return serialize_value_.exchange(false);
}

async_values_type& runtime_process_context::get_async_values ()
{
    locks::auto_lock_t _(&context_lock_);
    RX_ASSERT(current_step_ == runtime_process_step::async_values);
    if (should_do_step<runtime_process_step::async_values>())
        return async_values_.get_and_swap();
    else
        return g_empty_async_values;
}

void runtime_process_context::full_value_changed (structure::full_value_data* whose)
{
    binded_.full_value_changed(whose, whose->get_value(this), tags_);
}

security::security_guard_ptr runtime_process_context::get_security_guard ()
{
    return security_guard_;
}

bool runtime_process_context::is_mine_value (const rx_value& from) const
{
    locks::auto_lock_t _(&const_cast<runtime_process_context*>(this)->context_lock_);
    if (mode_.is_off())
    {
        return false;
    }
    else if (mode_.is_test())
    {
        if (!from.is_test())
            return false;
    }
    else
    {
        if (from.is_test())
            return false;
    }
    return true;
}

void runtime_process_context::value_changed (structure::value_data* whose)
{
    binded_.value_change(whose, whose->get_value(this));
    tags_.binded_value_change(whose, whose->get_value(this));
}

void runtime_process_context::method_changed (logic_blocks::method_data* whose)
{
    tags_.method_changed(whose, whose->get_value(this));
}

rx_result runtime_process_context::write_connected (runtime_handle_t handle, rx_simple_value&& val, runtime_transaction_id_t trans_id)
{
    return binded_.write_connected(handle, std::move(val), trans_id);
}

rx_result runtime_process_context::execute_connected (runtime_handle_t handle, rx_simple_value&& val, runtime_transaction_id_t trans_id)
{
    return binded_.execute_connected(handle, std::move(val), trans_id);
}

rx_time runtime_process_context::now ()
{
    if (current_step_ == runtime_process_step::idle)
        return rx_time::now();
    else
        return rx_time_struct_t{ now_ };
}

// Parameterized Class rx_platform::runtime::process_context_job 


// Class rx_platform::runtime::context_job 


// Class rx_platform::runtime::context_write_data 


// Class rx_platform::runtime::relation_subscriber 


// Class rx_platform::runtime::context_execute_data 


// Class rx_platform::runtime::write_data 


// Class rx_platform::runtime::execute_data 


// Class rx_platform::runtime::event_fired_data 


} // namespace runtime
} // namespace rx_platform

