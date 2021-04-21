

/****************************************************************************
*
*  system\runtime\rx_scan_algorithms.cpp
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
    whose.job_lock_.lock();
    whose.job_pending_ = false;
    whose.job_lock_.unlock();
    whose.context_.init_context();

    string_type full_path = whose.meta_info().get_full_path();

    security::secured_scope _(whose.instance_data_.get_security_context());
    whose.loop_count_.commit();
    whose.last_scan_time_.commit();

    auto old_tick = rx_get_us_ticks();

    // persistence handling receive side

    size_t lap_count = 0;
    do
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////
        // REMOTE UPDATES
        process_from_remotes(whose, whose.context_);
        /////////////////////////////////////////////////////////////////////////////////////////////////
        // STATUS
        process_status_change(whose, whose.context_);
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // INPUTS
        process_source_inputs(whose, whose.context_);
        process_mapper_inputs(whose, whose.context_);
        process_subscription_inputs(whose, whose.context_);
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // PROCESS
        process_variables(whose, whose.context_);
        process_programs(whose, whose.context_);
        process_events(whose, whose.context_);
        process_filters(whose, whose.context_);
        process_structs(whose, whose.context_);
        process_own(whose, whose.context_);
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // OUTPUTS
        process_subscription_outputs(whose, whose.context_);
        process_mapper_outputs(whose, whose.context_);
        process_source_outputs(whose, whose.context_);
        /////////////////////////////////////////////////////////////////////////////////////////////////

        lap_count++;

    } while (whose.context_.should_repeat());

    auto diff = rx_get_us_ticks() - old_tick;
    whose.last_scan_time_ = (double)diff / 1000.0;
    whose.loop_count_ += lap_count;

    // persistence handling send side
    if (whose.context_.should_save())
        runtime_holder_algorithms<typeT>::save_runtime(whose);

    if (whose.max_scan_time_ < whose.last_scan_time_)
    {
        whose.max_scan_time_ = whose.last_scan_time_;
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_status_change (typename typeT::RType& whose, runtime_process_context& ctx)
{
    while (ctx.should_process_status_change())
        whose.tags_.item_->object_state_changed(&ctx);
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_source_inputs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    source_results_type* source_results = &ctx.get_source_results();
    source_updates_type* source_updates = &ctx.get_source_updates();
    while (!source_updates->empty() || !source_results->empty())
    {
        for (auto& one : *source_results)
            one.whose->process_result(one.transaction_id, std::move(one.result));
        source_results = &ctx.get_source_results();

        for (auto& one : *source_updates)
            one.whose->process_update(std::move(one.value));
        source_updates = &ctx.get_source_updates();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_mapper_inputs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto mapper_writes = &ctx.get_mapper_writes();
    while (!mapper_writes->empty())
    {
        for (auto& one : *mapper_writes)
            one.whose->process_write(std::move(one.data));
        mapper_writes = &ctx.get_mapper_writes();
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
    auto variables = &ctx.get_variables_for_process();
    while (!variables->empty())
    {
        for (auto& one : *variables)
            one->process_runtime(&ctx);
        variables = &ctx.get_variables_for_process();
    }
}

template <class typeT>
void runtime_scan_algorithms<typeT>::process_programs (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto programs = &ctx.get_programs_for_process();
    while (!programs->empty())
    {
        for (auto& one : *programs)
            one->process_program(ctx);
        programs = &ctx.get_programs_for_process();
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

template <class typeT>
void runtime_scan_algorithms<typeT>::process_events (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto events = &ctx.get_events_for_process();
    while (!events->empty())
    {
        for (auto& one : *events)
            one->process_runtime(&ctx);
        events = &ctx.get_events_for_process();
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
void runtime_scan_algorithms<typeT>::process_from_remotes (typename typeT::RType& whose, runtime_process_context& ctx)
{
    auto remote_updates = &ctx.get_from_remote();
    if (!remote_updates->empty())
    {
        for (auto& one : *remote_updates)
        {
            ctx.set_value(one.handle, std::move(one.value));
        }
    }
}

template class runtime_scan_algorithms<meta::object_types::object_type>;
template class runtime_scan_algorithms<meta::object_types::port_type>;
template class runtime_scan_algorithms<meta::object_types::application_type>;
template class runtime_scan_algorithms<meta::object_types::domain_type>;
} // namespace algorithms
} // namespace runtime
} // namespace rx_platform

