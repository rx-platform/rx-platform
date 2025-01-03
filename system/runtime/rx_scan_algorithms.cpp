

/****************************************************************************
*
*  system\runtime\rx_scan_algorithms.cpp
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

#include "system/server/rx_server.h"

// rx_scan_algorithms
#include "system/runtime/rx_scan_algorithms.h"

#include "rx_holder_algorithms.h"


namespace rx_platform {

namespace runtime {

namespace algorithms {

// Parameterized Class rx_platform::runtime::algorithms::runtime_scan_algorithms 


template <class typeT>
void runtime_scan_algorithms<typeT>::process_runtime (typename typeT::RType& whose)
{

    //!!!!
   // bool auto_obj = ("TankFarm" == whose.meta_info_.name);


    whose.job_lock_.lock();
    whose.job_pending_ = false;
    whose.job_lock_.unlock();

    if (!whose.context_->is_started())
    {
        whose.context_->fire_callback_();
        return;
    }

    whose.context_->init_context(false);

    string_type full_path = whose.meta_info().get_full_path();

    security::secured_scope _(whose.instance_data_.get_security_context());
    whose.tags_.common_tags_.loop_count_.commit();
    whose.tags_.common_tags_.last_scan_time_.commit();

    if (whose.tags_.common_tags_.loop_count_ == 0)
    {
        whose.context_->simple_value_changed_ = true;
    }

    check_context(whose, *whose.context_);

    auto old_tick = rx_get_us_ticks();

    // persistence handling receive side

    size_t lap_count = 0;
    do
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////
        // DEBUG SCAN SUPPPORT
        // ========================
        // this is a debug scan stuff that handles all arrived data
        // it is used to skip all scan steps that are needed
        process_debug_scan(whose);
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // REMOTE UPDATES
        if constexpr (C_has_remote_updates)
        {
            process_from_remotes(whose, *whose.context_);
        }
        else
        {
            auto remote_updates = whose.context_->get_async_values();
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////
        // STATUS
        if constexpr (C_has_status_response)
        {
            process_status_change(whose, *whose.context_);
        }
        else
        {
            auto status_response = whose.context_->should_process_status_change();
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // INPUTS
        if constexpr (C_has_source_inputs)
        {
            process_source_inputs(whose, *whose.context_);
        }
        else
        {
            auto source_results = &whose.context_->get_source_inputs();
        }
        if constexpr (C_has_mapper_writes)
        {
            process_mapper_inputs(whose, *whose.context_);
        }
        else
        {
            auto mapper_writes = whose.context_->get_mapper_inputs();
        }
        if constexpr (C_has_tag_writes)
        {
             process_subscription_inputs(whose, *whose.context_);
        }
        else
        {
            auto writes_response = whose.context_->should_process_tag_writes();
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // PROCESS
        if constexpr (C_has_variables)
        {
            process_variables(whose, *whose.context_);
        }
        else
        {
            auto variables = whose.context_->get_variables_for_process();
        }
        if constexpr (C_has_programs)
        {
            process_programs(whose, *whose.context_);
        }
        else
        {
            auto programs = whose.context_->get_logic_for_process();
        }
        if constexpr (C_has_events)
        {
            process_events(whose, *whose.context_);
        }
        else
        {
            auto events = &whose.context_->get_events_for_process();
            while (!events->empty())
            {
                events = &whose.context_->get_events_for_process();
            }
        }
        if constexpr (C_has_filters)
        {
            process_filters(whose, *whose.context_);
        }
        else
        {
            auto filters = whose.context_->get_filters_for_process();
        }
        if constexpr (C_has_structs)
        {
            process_structs(whose, *whose.context_);
        }
        else
        {
            auto structs = whose.context_->get_structs_for_process();
        }
        if constexpr (C_has_own)
        {
            process_own(whose, *whose.context_);
        }
        else
        {
            auto own = whose.context_->get_for_own_process();
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // OUTPUTS
        if constexpr (C_has_tag_updates)
        {
            process_subscription_outputs(whose, *whose.context_);
        }
        else
        {
            auto tag_updates_result = whose.context_->should_process_tag_updates();
        }
        if constexpr (C_has_mapper_updates)
        {
            process_mapper_outputs(whose, *whose.context_);
        }
        else
        {
            auto mapper_updates = whose.context_->get_mapper_updates();
        }
        if constexpr (C_has_source_writes)
        {
            process_source_outputs(whose, *whose.context_);
        }
        else
        {
            auto source_writes = &whose.context_->get_source_writes();
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////

        lap_count++;

    } while (whose.context_->should_repeat());
    if (whose.context_->simple_value_changed_)
    {
        whose.context_->simple_value_changed_ = false;
        if (whose.context_->object_changed)
        {
            data::runtime_values_data dt;
            if (whose.tags_.get_struct_value("", dt, runtime_value_type::simple_runtime_value, whose.context_.get()))
            {
                whose.runtime_data_cache_ = dt;
                whose.json_cache_.clear();
                whose.context_->object_changed->event_fired(std::move(dt));
            }
        }
    }
    RX_ASSERT(lap_count < 100);
    auto diff = rx_get_us_ticks() - old_tick;
    whose.tags_.common_tags_.last_scan_time_ = (double)diff / 1000.0;
    whose.tags_.common_tags_.loop_count_ += lap_count;

    // persistence handling send side
    if (whose.context_->should_save())
        runtime_holder_algorithms<typeT>::save_runtime(whose);

    if (whose.tags_.common_tags_.max_scan_time_ < whose.tags_.common_tags_.last_scan_time_)
    {
        whose.tags_.common_tags_.max_scan_time_ = whose.tags_.common_tags_.last_scan_time_;
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::check_context (typename typeT::RType& whose, runtime_process_context& ctx)
{
    size_t ret;
    {
        locks::auto_lock_t _(&ctx.context_lock_);
        ret = ctx.mapper_inputs_.full_size();
        ret += ctx.mapper_outputs_.full_size();
        ret += ctx.source_inputs_.full_size();
        ret += ctx.source_outputs_.full_size();
        ret += ctx.source_results_.full_size();
        ret += ctx.variables_.full_size();
        ret += ctx.filters_.full_size();
        ret += ctx.programs_.full_size();
        ret += ctx.events_.full_size();
        ret += ctx.structs_.full_size();
        ret += ctx.owns_.full_size();
        ret += ctx.async_values_.full_size();
        ret += ctx.method_results_.full_size();
    }
    RX_ASSERT(ret < 0x10000);
    whose.tags_.common_tags_.queues_size_.commit();
    whose.tags_.common_tags_.queues_size_ = (uint32_t)ret;
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_debug_scan (typename typeT::RType& whose)
{
    // this is important as it clears everything that is needed
    // it is so convenient for debugging, you can select what to exclude from scan

    // internal stuff
    if constexpr (C_has_remote_updates)
    {
    }
    if constexpr (C_has_status_response)
    {
    }
    // input stuff
    if constexpr (C_has_source_inputs)
    {
    }
    if constexpr (C_has_mapper_writes)
    {
    }
    if constexpr (C_has_tag_writes)
    {
    }
    // processing
    if constexpr (C_has_variables)
    {
    }
    if constexpr (C_has_programs)
    {
    }
    if constexpr (C_has_events)
    {
    }
    if constexpr (C_has_filters)
    {
    }
    if constexpr (C_has_structs)
    {
    }
    if constexpr (C_has_own)
    {
    }


    if constexpr (C_has_tag_updates)
    {
    }
    if constexpr (C_has_mapper_updates)
    {
    }
    if constexpr (!C_has_source_writes)
    {
    }

}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_from_remotes (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto async_values = &ctx.get_async_values();
    while (!async_values->empty())
    {
        for (auto& one : *async_values)
            ctx.set_value(one.handle, std::move(one.value), std::move(one.callback));
        async_values = &ctx.get_async_values();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_status_change (typename typeT::RType& whose, runtime_process_context& ctx)
{
    while (ctx.should_process_status_change())
    {

        bool on = whose.tags_.common_tags_.on_;
        bool test = whose.tags_.common_tags_.test_;
        bool my_on = whose.context_->mode_.is_on();
        bool my_test = whose.context_->mode_.is_test();
        if (on != my_on)
        {
            if (on)
            {
                if (whose.context_->mode_.turn_on())
                    whose.context_->status_change_pending();
            }
            else
            {
                if (whose.context_->mode_.turn_off())
                    whose.context_->status_change_pending();
            }

        }
        if (test != my_test)
        {
            if (test)
            {
                if (whose.context_->mode_.set_test())
                    whose.context_->status_change_pending();
            }
            else
            {
                if (whose.context_->mode_.reset_test())
                    whose.context_->status_change_pending();
            }
        }
        ctx.set_binded_as(whose.tags_.common_tags_.on_handle_, ctx.mode_.is_on());
        ctx.set_binded_as(whose.tags_.common_tags_.test_handle_, ctx.mode_.is_test());
        ctx.set_binded_as(whose.tags_.common_tags_.blocked_handle_, ctx.mode_.is_blocked());
        ctx.set_binded_as(whose.tags_.common_tags_.simulate_handle_, ctx.mode_.is_simulate());
        whose.tags_.connected_tags_.object_state_changed(&ctx);
        whose.tags_.item_->object_state_changed(&ctx);
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_source_inputs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto to_process = ctx.get_source_inputs();
    while (!to_process.first->empty() || !to_process.second->empty())
    {
        for (auto& one : *to_process.first)
            one.whose->process_result(one.transaction_id, std::move(one.result));

        for (auto& one : *to_process.second)
            one.whose->process_update(std::move(one.value));

        to_process = ctx.get_source_inputs();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_mapper_inputs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto to_process = ctx.get_mapper_inputs();
    while (!to_process.first->empty() || !to_process.second->empty())
    {
        for (auto& one : *to_process.first)
            one.whose->process_write(std::move(one.data));

        for (auto& one : *to_process.second)
            one.whose->process_execute(std::move(one.data));

        to_process = ctx.get_mapper_inputs();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_subscription_inputs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    while (ctx.should_process_tag_writes())
        whose.tags_.connected_tags_.process_transactions();
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_variables (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto to_process = ctx.get_variables_for_process();
    while (!to_process.var_results->empty() 
        || !to_process.vars->empty()
        || !to_process.block_results->empty()
        || !to_process.block_vars->empty())
    {
        for (auto& one : *to_process.var_results)
            one.whose->process_result(one.transaction_id, std::move(one.result));

        for (auto& one : *to_process.block_results)
            one.whose->process_result(one.transaction_id, std::move(one.result));

        for (auto& one : *to_process.vars)
            one->process_runtime(&ctx);

        for (auto& one : *to_process.block_vars)
            one->process_runtime(&ctx);

        to_process = ctx.get_variables_for_process();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_programs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto to_process = ctx.get_logic_for_process();
    while (!to_process.first->empty() || !to_process.second->empty())
    {
        for (auto& one : *to_process.first)
            one.whose->process_execute_result(one.transaction_id, std::move(one.result), std::move(one.data));

        for (auto& one : *to_process.second)
            one->process_program(ctx);

        to_process = ctx.get_logic_for_process();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_events (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto events = &ctx.get_events_for_process();
    while (!events->empty())
    {
        // event processing!!!!
        for (auto& one : *events)
        {
            one.whose->process_runtime(&ctx, std::move(one.data));
        }
        events = &ctx.get_events_for_process();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_filters (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto filters = &ctx.get_filters_for_process();
    while (!filters->empty())
    {
        for (auto& one : *filters)
            one->process_runtime(&ctx);
        filters = &ctx.get_filters_for_process();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_structs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto structs = &ctx.get_structs_for_process();
    while (!structs->empty())
    {
        for (auto& one : *structs)
            one->process_runtime(&ctx);
        structs = &ctx.get_structs_for_process();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_own (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto own_jobs = &ctx.get_for_own_process();
    while (!own_jobs->empty())
    {
        for (auto& one : *own_jobs)
            one->process();
        own_jobs = &ctx.get_for_own_process();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_subscription_outputs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    while (ctx.should_process_tag_updates())
        whose.tags_.connected_tags_.process_runtime();
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_mapper_outputs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto mapper_updates = &ctx.get_mapper_updates();
    while (!mapper_updates->empty())
    {
        for (auto& one : *mapper_updates)
            one.whose->process_update(std::move(one.value));
        mapper_updates = &ctx.get_mapper_updates();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_source_outputs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto source_writes = &ctx.get_source_writes();
    while (!source_writes->empty())
    {
        for (auto& one : *source_writes)
            one.whose->process_write(std::move(one.data));
        source_writes = &ctx.get_source_writes();
    }
}

template class runtime_scan_algorithms<meta::object_types::object_type>;
template class runtime_scan_algorithms<meta::object_types::port_type>;
template class runtime_scan_algorithms<meta::object_types::application_type>;
template class runtime_scan_algorithms<meta::object_types::domain_type>;
} // namespace algorithms
} // namespace runtime
} // namespace rx_platform

