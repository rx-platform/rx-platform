

/****************************************************************************
*
*  system\runtime\rx_process_context.cpp
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
remotes_data_type g_empty_remotes;
variables_type g_empty_variables;
structs_type g_empty_structs;
source_writes_type g_empty_source_writes;
source_updates_type g_empty_source_updates;
source_results_type g_empty_source_results;
variable_results_type g_empty_variable_results;
programs_type g_empty_programs;
owner_jobs_type g_empty_jobs;
mapper_writes_type g_empty_mapper_writes;
mapper_updates_type g_empty_mapper_updates;
filters_type g_empty_filters;
events_type g_empty_events;
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
        current_step_ = (runtime_process_step)((uint_fast8_t)current_step_ + 1);
        return false;
    }
}

// Class rx_platform::runtime::runtime_process_context 

runtime_process_context::runtime_process_context (tag_blocks::binded_tags& binded, tag_blocks::connected_tags& tags, const meta::meta_data& info, ns::rx_directory_resolver* dirs)
      : tags_(tags),
        binded_(binded),
        current_step_(runtime_process_step::idle),
        meta_info(info),
        directory_resolver_(dirs),
        serialize_value_(false)
{
    mode_.turn_off();
    now = rx_time::now();
    mode_time_ = now;
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

void runtime_process_context::tag_updates_pending ()
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::tag_outputs>();
}

rx_result runtime_process_context::init_context ()
{
    now = rx_time::now();
    current_step_ = runtime_process_step::remote_updates;
    return true;
}

void runtime_process_context::tag_writes_pending ()
{
    turn_on_pending<runtime_process_step::tag_inputs>();
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

rx_result runtime_process_context::get_value (runtime_handle_t handle, values::rx_simple_value& val) const
{
    return binded_.get_value(handle, val);
}

rx_result runtime_process_context::set_value (runtime_handle_t handle, values::rx_simple_value&& val)
{
    return binded_.set_value(handle, std::move(val), tags_, this);
}

rx_result runtime_process_context::set_item (const string_type& path, values::rx_simple_value&& what, runtime_init_context& ctx)
{
    return binded_.set_item(path, std::move(what), ctx);
}

void runtime_process_context::init_state (fire_callback_func_t fire_callback)
{
    fire_callback_ = fire_callback;
    mode_.turn_on();
}

void runtime_process_context::mapper_write_pending (write_data_struct<structure::mapper_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::mapper_inputs>();
    mapper_inputs_.emplace_back(std::move(data));
}

mapper_writes_type& runtime_process_context::get_mapper_writes ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::mapper_inputs>())
        return mapper_inputs_.get_and_swap();
    else
        return g_empty_mapper_writes;
}

void runtime_process_context::status_change_pending ()
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::status_change>();
}

void runtime_process_context::mapper_update_pending (update_data_struct<structure::mapper_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::mapper_outputs>();
    mapper_outputs_.emplace_back(std::move(data));
}

mapper_updates_type& runtime_process_context::get_mapper_updates ()
{
    if (should_do_step<runtime_process_step::mapper_outputs>())
        return mapper_outputs_.get_and_swap();
    else
        return g_empty_mapper_updates;
}

void runtime_process_context::source_write_pending (write_data_struct<structure::source_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::source_outputs>();
    source_outputs_.emplace_back(std::move(data));
}

source_writes_type& runtime_process_context::get_source_writes ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::source_outputs>())
        return source_outputs_.get_and_swap();
    else
        return g_empty_source_writes;
}

void runtime_process_context::source_update_pending (update_data_struct<structure::source_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::source_inputs>();
    source_inputs_.emplace_back(std::move(data));
}

source_updates_type& runtime_process_context::get_source_updates ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::source_inputs>())
        return source_inputs_.get_and_swap();
    else
        return g_empty_source_updates;
}

void runtime_process_context::variable_pending (structure::variable_data* whose)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::variables>();
    variables_.emplace_back(std::move(whose));
}

void runtime_process_context::variable_result_pending (write_result_struct<structure::variable_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::variables>();
    variable_results_.emplace_back(std::move(data));
}

std::pair<variable_results_type*, variables_type*> runtime_process_context::get_variables_for_process ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::variables>())
        return { &variable_results_.get_and_swap() , &variables_.get_and_swap() };
    else
        return { &g_empty_variable_results, &g_empty_variables };
}

bool runtime_process_context::should_process_status_change ()
{
    locks::auto_lock_t _(&context_lock_);
    return should_do_step<runtime_process_step::status_change>();
}

void runtime_process_context::program_pending (program_runtime_ptr whose)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::programs>();
    programs_.emplace_back(std::move(whose));
}

programs_type& runtime_process_context::get_programs_for_process ()
{
    locks::auto_lock_t _(&context_lock_);
    if (should_do_step<runtime_process_step::programs>())
        return programs_.get_and_swap();
    else
        return g_empty_programs;
}

void runtime_process_context::filter_pending (structure::filter_data* whose)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::events>();
    filters_.emplace_back(std::move(whose));
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
    tags_.variable_change(whose, std::move(adapted_val));
}

void runtime_process_context::event_pending (structure::event_data* whose)
{
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

void runtime_process_context::own_pending (jobs::job_ptr what)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::own>();
    owns_.emplace_back(std::move(what));
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

void runtime_process_context::source_result_pending (write_result_struct<structure::source_data> data)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::source_inputs>();
    source_results_.emplace_back(std::move(data));
}

source_results_type& runtime_process_context::get_source_results ()
{
    locks::auto_lock_t _(&context_lock_);
    RX_ASSERT(current_step_ == runtime_process_step::source_inputs);
    if (current_step_ == runtime_process_step::source_inputs)
        return source_results_.get_and_swap();
    else
        return g_empty_source_results;
}

void runtime_process_context::runtime_dirty ()
{
    serialize_value_ = true;
}

bool runtime_process_context::should_save ()
{
    return serialize_value_.exchange(false);
}

void runtime_process_context::from_remote_pending (remotes_data data)
{
    locks::auto_lock_t _(&context_lock_);
    turn_on_pending<runtime_process_step::remote_updates>();
    from_remote_.emplace_back(std::move(data));
}

remotes_data_type& runtime_process_context::get_from_remote ()
{
    locks::auto_lock_t _(&context_lock_);
    RX_ASSERT(current_step_ == runtime_process_step::remote_updates);
    if (should_do_step<runtime_process_step::remote_updates>())
        return from_remote_.get_and_swap();
    else
        return g_empty_remotes;
}

rx_result rx_set_value_to_context(runtime_process_context* ctx, runtime_handle_t handle, values::rx_simple_value&& val)
{
    return ctx->set_value(handle, std::move(val));
}
// Parameterized Class rx_platform::runtime::process_context_job 


// Class rx_platform::runtime::context_job 


// Class rx_platform::runtime::write_data 


// Class rx_platform::runtime::relation_subscriber 


// Class rx_platform::runtime::context_value_point 


void context_value_point::value_changed (const rx_value& val)
{
}

void context_value_point::result_received (rx_result&& result, runtime_transaction_id_t id)
{
}


} // namespace runtime
} // namespace rx_platform

