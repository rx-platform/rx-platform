

/****************************************************************************
*
*  system\runtime\rx_runtime_holder.cpp
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

#include "system/meta/rx_obj_types.h"

// rx_value_point
#include "runtime_internal/rx_value_point.h"
// rx_runtime_holder
#include "system/runtime/rx_runtime_holder.h"

#include "api/rx_platform_api.h"
#include "sys_internal/rx_async_functions.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/meta/rx_obj_types.h"
#include "rx_write_transaction.h"
using rx_internal::sys_runtime::data_source::value_point;


namespace rx_platform {

namespace runtime {

namespace algorithms {
template<typename CT>
namespace_item_attributes create_attributes_from_creation_data(const CT& data)
{
    if (data.system)
    {
        return namespace_item_system_access;
    }
    else
    {
        return namespace_item_full_access;
    }
}

// Parameterized Class rx_platform::runtime::algorithms::object_runtime_algorithms 


template <class typeT>
std::vector<rx_result_with<runtime_handle_t> > object_runtime_algorithms<typeT>::connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    using connect_result_t = std::vector<rx_result_with<runtime_handle_t> >;

    connect_result_t results;
    bool has_errors = false;
    bool had_good = false;
    if (paths.empty())
        return std::vector<rx_result_with<runtime_handle_t> >();
    results.clear();// just in case
    results.reserve(paths.size());

    has_errors = false;
    for (const auto& path : paths)
    {
        auto one_result = whose.connected_tags_.connect_tag(path, *whose.item_, monitor);
        if (!has_errors && !one_result)
            has_errors = true;
        if (!had_good && one_result)
            had_good = true;
        results.emplace_back(std::move(one_result));
    }
    if (had_good)
    {
        whose.context_.tag_updates_pending();
    }
    return results;
}

template <class typeT>
void object_runtime_algorithms<typeT>::process_runtime (typename typeT::RType& whose)
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
        // STATUS
        whose.process_status_change(whose.context_);
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // INPUTS
        whose.process_source_inputs(whose.context_);
        whose.process_mapper_inputs(whose.context_);
        whose.process_subscription_inputs(whose.context_);
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // PROCESS
        whose.process_variables(whose.context_);
        whose.process_programs(whose.context_);
        whose.process_events(whose.context_);
        whose.process_filters(whose.context_);
        whose.process_structs(whose.context_);
        whose.process_own(whose.context_);
        /////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////
        // OUTPUTS
        whose.process_subscription_outputs(whose.context_);
        whose.process_mapper_outputs(whose.context_);
        whose.process_source_outputs(whose.context_);
        /////////////////////////////////////////////////////////////////////////////////////////////////

        lap_count++;

    } while (whose.context_.should_repeat());

    auto diff = rx_get_us_ticks() - old_tick;
    whose.last_scan_time_ = (double)diff / 1000.0;
    whose.loop_count_ += lap_count;

    // persistence handling send side
    if (whose.context_.should_save())
        save_runtime(whose);
    
    if (whose.max_scan_time_ < whose.last_scan_time_)
    {
        whose.max_scan_time_ = whose.last_scan_time_;
    }
}

template <class typeT>
rx_result object_runtime_algorithms<typeT>::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    std::vector<rx_result> results;
    for (const auto& item : items)
        whose.connected_tags_.read_tag(item, monitor);
    return true;
}

template <class typeT>
void object_runtime_algorithms<typeT>::fire_job (typename typeT::RType& whose)
{
    locks::auto_lock_t<decltype(whose.job_lock_)> _(&whose.job_lock_);
    if (!whose.job_pending_)
    {
        whose.job_pending_ = true;
        RX_ASSERT(whose.my_job_ptr_);
        auto executer = rx_internal::infrastructure::server_runtime::instance().get_executer(whose.get_executer());
        executer->append(whose.my_job_ptr_);
    }
}

template <class typeT>
rx_result object_runtime_algorithms<typeT>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    std::vector<rx_result> results;
    for (const auto& item : items)
    {
        auto result = whose.connected_tags_.write_tag(transaction_id, item.first, rx_simple_value(item.second), monitor);
        results.emplace_back(std::move(result));
    }
    return true;
}

template <class typeT>
std::vector<rx_result> object_runtime_algorithms<typeT>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    std::vector<rx_result> results;
    if (!items.empty())
    {
        results.reserve(items.size());
        for (const auto& handle : items)
        {
            auto one_result = whose.connected_tags_.disconnect_tag(handle, monitor);
            results.emplace_back(std::move(one_result));
        }
    }
    return results;
}

template <class typeT>
rx_result object_runtime_algorithms<typeT>::write_value (const string_type& path, rx_simple_value&& val, rx_result_callback callback, api::rx_context ctx, typename typeT::RType& whose)
{
    auto transaction_ptr = rx_create_reference<write_item_transaction>(std::move(callback), rx_thread_context());
    auto connect_result = whose.connected_tags_.connect_tag(path, *whose.item_, transaction_ptr);
    if (!connect_result)
    {
        connect_result.register_error("Error writing to item "s + path);
        return connect_result.errors();
    }
    auto result = whose.connected_tags_.write_tag(1, connect_result.value(), std::move(val), transaction_ptr);
    if (!result)
    {
        whose.connected_tags_.disconnect_tag(connect_result.value(), transaction_ptr);
    }
    return result;
}

template <class typeT>
void object_runtime_algorithms<typeT>::save_runtime (typename typeT::RType& whose)
{
    auto storage_result = whose.meta_info_.resolve_storage();
    if (storage_result)
    {
        auto item_result = storage_result.value()->get_runtime_storage(whose.meta_info_, whose.get_type_id());
        if (item_result)
        {
            auto result = item_result.value()->open_for_write();
            if (result)
            {
                item_result.value()->write_stream().write_header(STREAMING_TYPE_MESSAGE, 0);
                result = whose.serialize_value(item_result.value()->write_stream(), runtime_value_type::persistent_runtime_value);
                if(result)
                    result = item_result.value()->commit_write();
                RUNTIME_LOG_DEBUG("runtime_model_algorithm", 100, "Saved "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + whose.meta_info_.get_full_path());
            }
            else
            {
                RUNTIME_LOG_ERROR("runtime_model_algorithm", 100, "Error saving "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + whose.meta_info_.get_full_path());
            }
        }
        else
        {
            RUNTIME_LOG_ERROR("runtime_model_algorithm", 100, "Error saving "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + whose.meta_info_.get_full_path());
        }
    }
}



template <>
std::vector<rx_result_with<runtime_handle_t> > object_runtime_algorithms<object_types::relation_type>::connect_items(const string_array& paths, runtime::operational::tags_callback_ptr monitor, object_types::relation_type::RType& whose)
{
    using connect_result_t = std::vector<rx_result_with<runtime_handle_t> >;

    connect_result_t result;
    result.reserve(paths.size());

    for (size_t i = 0; i < paths.size(); i++)
    {
        result.emplace_back(RX_NOT_IMPLEMENTED);
    }

    return result;

    // OutputDebugStringA("****************Something to connect object runtime\r\n");

    /*std::function<connect_result_t(string_array, operational::tags_callback_ptr, smart_ptr)> func = [](string_array paths, operational::tags_callback_ptr monitor, smart_ptr whose)
    {

        // OutputDebugStringA("****************Something to connect object runtime\r\n");
        connect_result_t results;
        bool has_errors = false;
        auto ret = whose->runtime_.connect_items(paths, monitor, results, has_errors);
        if (ret)
        {
            whose->runtime_context_.process_tag_connections = true;
        }
        else
        {
            auto size = paths.size();
            results.reserve(size);
            for (size_t i = 0; i < size; i++)
                results.emplace_back(ret.errors());
        }
        if (whose->runtime_context_.process_tag_connections)
        {
            whose->fire_job();
        }
        return results;
    };
    auto ret_thread = whose->get_executer();
    rx_do_with_callback<connect_result_t, decltype(ctx.object), string_array, operational::tags_callback_ptr, smart_ptr>(func, ret_thread, callback, ctx.object, paths, monitor, whose->smart_this());
    return true;*/
}

template class object_runtime_algorithms<object_types::port_type>;
template class object_runtime_algorithms<object_types::object_type>;
template class object_runtime_algorithms<object_types::domain_type>;
template class object_runtime_algorithms<object_types::application_type>;

// Parameterized Class rx_platform::runtime::algorithms::runtime_holder 

template <class typeT>
runtime_holder<typeT>::runtime_holder (const meta::meta_data& meta, const typename typeT::instance_data_t& instance, typename typeT::runtime_behavior_t&& rt_behavior)
      : job_pending_(false),
        last_scan_time_(-1),
        max_scan_time_(0),
        loop_count_(0)
    , meta_info_(meta)
    , instance_data_(instance.instance_data, std::move(rt_behavior))
    , points_(std::make_unique<std::vector<value_point> >())
    , context_(binded_tags_, connected_tags_, meta_info_, &directories_, points_.get())
{
    using rimpl_t = typename typeT::RImplType;
    RUNTIME_LOG_DEBUG("runtime_holder", 900, (rx_item_type_name(rimpl_t::type_id) + " constructor, for " + meta.get_full_path()));
    my_job_ptr_ = rx_create_reference<process_runtime_job<typeT> >(smart_this());
    connected_tags_.init_tags(&context_, &relations_);
}


template <class typeT>
runtime_holder<typeT>::~runtime_holder()
{
    using rimpl_t = typename typeT::RImplType;
    if (meta_info_.name.empty())
        RUNTIME_LOG_DEBUG("runtime_holder", 900, (rx_item_type_name(rimpl_t::type_id) + " destructor, for unknown"));
    else
        RUNTIME_LOG_DEBUG("runtime_holder", 900, (rx_item_type_name(rimpl_t::type_id) + " constructor, for " + meta_info_.get_full_path()));
}



template <class typeT>
bool runtime_holder<typeT>::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!meta_info_.serialize_meta_data(stream, type, typeT::RImplType::type_id))
        return false;

    if (!stream.start_object("def"))
        return false;

    if (!instance_data_.get_data().serialize(stream, type))
        return false;

    if (!stream.write_init_values("overrides", overrides_))
        return false;

    if (!logic_.serialize(stream, type))
        return false;
    
    if (!stream.end_object())
        return false;

    return true;
}

template <class typeT>
rx_result runtime_holder<typeT>::initialize_runtime (runtime_init_context& ctx)
{
    ctx.anchor = smart_this();
    context_.init_state([this]
        {
            object_runtime_algorithms<typeT>::fire_job(*this);
        });
    ctx.structure.push_item(*item_);
    ctx.context = &context_;
    auto result = item_->initialize_runtime(ctx);
    if (result)
    {
        auto bind_result = last_scan_time_.bind("Object.LastScanTime", ctx);
        if (!bind_result)
            RX_ASSERT(false);
        bind_result = max_scan_time_.bind("Object.MaxScanTime", ctx);
        if (!bind_result)
            RX_ASSERT(false);
        bind_result = loop_count_.bind("Object.LoopCount", ctx);
        if (!bind_result)
            RX_ASSERT(false);

        result = relations_.initialize_relations(ctx);
        if (result)
        {
            logic_.initialize_logic(ctx);
        }
    }
    if (result)
    {
        result = implementation_->initialize_runtime(ctx);
    }
    ctx.structure.pop_item();
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::deinitialize_runtime (runtime_deinit_context& ctx)
{
    rx_result result = implementation_->deinitialize_runtime(ctx);
    if (result)
    {
        result = logic_.deinitialize_logic(ctx);
        if (result)
        {
            result = relations_.deinitialize_relations(ctx);
            if (result)
            {
                result = item_->deinitialize_runtime(ctx);
            }
        }
    }
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::start_runtime (runtime_start_context& ctx)
{
    ctx.structure.push_item(*item_);
    auto result = item_->start_runtime(ctx);
    if (result)
    {
        result = relations_.start_relations(ctx);
        if (result)
        {
            result = logic_.start_logic(ctx);
        }
    }
    if (result)
    {
        result = implementation_->start_runtime(ctx);
    }
    ctx.structure.pop_item();
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::stop_runtime (runtime_stop_context& ctx)
{
    rx_result result = implementation_->stop_runtime(ctx);
    if (result)
    {
        result = logic_.stop_logic(ctx);
        if (result)
        {
            result = relations_.stop_relations(ctx);
            if (result)
            {
                result = item_->stop_runtime(ctx);
            }
        }
    }
    connected_tags_.runtime_stopped(rx_time::now());
    return result;
}

template <class typeT>
void runtime_holder<typeT>::fill_data (const data::runtime_values_data& data)
{
    structure::fill_context ctx(&context_);
    item_->fill_data(data);
    // now do the relations
    relations_.fill_data(data, &context_);
    logic_.fill_data(data, &context_);
}

template <class typeT>
void runtime_holder<typeT>::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    item_->collect_data(data, type);
    logic_.collect_data(data, type);
    relations_.collect_data(data, type);
}

template <class typeT>
rx_result runtime_holder<typeT>::read_value (const string_type& path, rx_value& value) const
{
    rx_result result;
    if (path.empty())
    {// our value
#ifndef RX_MIN_MEMORY
        if (!json_cache_.empty())
        {
            value.assign_static<string_type>(string_type(json_cache_), meta_info_.modified_time);
        }
        else
        {
            serialization::json_writer writer;
            writer.write_header(STREAMING_TYPE_MESSAGE, 0);
            result = serialize_value(writer, runtime_value_type::simple_runtime_value);
            if (result)
            {
#ifdef _DEBUG
                if (writer.get_string(const_cast<string_type&>(json_cache_), true))
#else
                if (writer.get_string(const_cast<runtime_holder<typeT>*>(this)->json_cache_, false))
#endif
                    value.assign_static<string_type>(string_type(json_cache_), meta_info_.modified_time);
            }
        }
#else
        serialization::json_writer writer;
        writer.write_header(STREAMING_TYPE_MESSAGE, 0);
        result = serialize_value(writer, runtime_value_type::simple_runtime_value);
        if (result)
        {
            string_type temp_str;
            if (writer.get_string(const_cast<string_type&>(temp_str), true))
            {
                value.assign_static<string_type>(string_type(temp_str), meta_info_.modified_time);
            }
        }
#endif
    }
    if (path == "Storage")
    {// our runtime value
        serialization::json_writer writer;
        writer.write_header(STREAMING_TYPE_MESSAGE, 0);
        result = serialize_value(writer, runtime_value_type::persistent_runtime_value);
        if (result)
        {
            string_type temp_str;
            if (writer.get_string(const_cast<string_type&>(temp_str), true))
            {
                value.assign_static<string_type>(string_type(temp_str), meta_info_.modified_time);
            }
        }
    }
    else
    {
        auto result = item_->get_value(path, value, const_cast<runtime_process_context*>(&context_));
        if (!result)
        {// check relations
            bool not_found = true;
            result = relations_.get_value(path, value, const_cast<runtime_process_context*>(&context_), not_found);
            if (not_found)
            {
                result = logic_.get_value(path, value, const_cast<runtime_process_context*>(&context_), not_found);
            }
        }
    }

    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::do_command (rx_object_command_t command_type)
{
    return context_.do_command(command_type);
}

template <class typeT>
void runtime_holder<typeT>::set_runtime_data (meta::runtime_data_prototype& prototype)
{
    item_ = std::move(create_runtime_data(prototype));
}

template <class typeT>
rx_result runtime_holder<typeT>::get_value_ref (const string_type& path, rt_value_ref& ref)
{
    return item_->get_value_ref(path, ref);
}

template <class typeT>
rx_result runtime_holder<typeT>::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
    if (path.empty())
    {
        auto ret = item_->browse_items(filter, prefix, items, &context_);
        if (ret)
        {
            bool not_found = true;
            ret = relations_.browse(prefix, "", filter, items, not_found);
            if (ret)
            {
                logic_.browse(prefix, "", filter, items, not_found);
            }
        }
        return ret;
    }
    else
    {
        string_type current_path(path + RX_OBJECT_DELIMETER);
        const auto& sub_item = item_->get_child_item(path);
        if (sub_item)
        {
            auto ret = sub_item->browse_items(filter, prefix + current_path, items, &context_);
            return ret;
        }
        else
        {
            bool not_found = true;
            auto ret = relations_.browse(prefix, path, filter, items, not_found);
            if (not_found)
            {
                ret = logic_.browse(prefix, path, filter, items, not_found);
            }
            return ret;
        }
    }
}

template <class typeT>
meta::meta_data& runtime_holder<typeT>::meta_info ()
{
  return meta_info_;

}

template <class typeT>
platform_item_ptr runtime_holder<typeT>::get_item_ptr () const
{
    return std::make_unique<rx_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
}

template <class typeT>
runtime_init_context runtime_holder<typeT>::create_init_context ()
{
    return runtime::runtime_init_context(*item_, meta_info_, &context_, &binded_tags_, &directories_);
}

template <class typeT>
runtime_start_context runtime_holder<typeT>::create_start_context ()
{
    return runtime_start_context(*item_, &context_, &directories_);
}

template <class typeT>
typename runtime_holder<typeT>::ImplPtr runtime_holder<typeT>::get_implementation ()
{
    return implementation_;
}

template <class typeT>
rx_thread_handle_t runtime_holder<typeT>::get_executer () const
{
    return instance_data_.get_executer();
}

template <class typeT>
rx_result runtime_holder<typeT>::serialize_value (base_meta_writer& stream, runtime_value_type type) const
{
    data::runtime_values_data data;
    collect_data(data, type);
    if (!stream.write_init_values(nullptr, data))
        return "Error writing values to the stream";
    else
        return true;
}

template <class typeT>
rx_result runtime_holder<typeT>::deserialize_value (base_meta_reader& stream, runtime_value_type type)
{
    return RX_NOT_IMPLEMENTED;
}

template <class typeT>
const typename typeT::runtime_data_t& runtime_holder<typeT>::get_instance_data () const
{
    return instance_data_;
}

template <class typeT>
void runtime_holder<typeT>::process_status_change (runtime_process_context& ctx)
{
    while (ctx.should_process_status_change())
        item_->object_state_changed(&context_);
}

template <class typeT>
void runtime_holder<typeT>::process_source_inputs (runtime_process_context& ctx)
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
void runtime_holder<typeT>::process_mapper_inputs (runtime_process_context& ctx)
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
void runtime_holder<typeT>::process_subscription_inputs (runtime_process_context& ctx)
{
    while (ctx.should_process_tag_writes())
        connected_tags_.process_transactions();
}

template <class typeT>
void runtime_holder<typeT>::process_variables (runtime_process_context& ctx)
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
void runtime_holder<typeT>::process_programs (runtime_process_context& ctx)
{
    auto programs = &ctx.get_programs_for_process();
    while (!programs->empty())
    {
        // TODO!!!
        /*for (auto& one : programs)
            one->process_runtime(&ctx);*/
        programs = &ctx.get_programs_for_process();
    }
}

template <class typeT>
void runtime_holder<typeT>::process_filters (runtime_process_context& ctx)
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
void runtime_holder<typeT>::process_subscription_outputs (runtime_process_context& ctx)
{
    while (ctx.should_process_tag_updates())
        connected_tags_.process_runtime();
}

template <class typeT>
void runtime_holder<typeT>::process_mapper_outputs (runtime_process_context& ctx)
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
void runtime_holder<typeT>::process_source_outputs (runtime_process_context& ctx)
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
void runtime_holder<typeT>::process_events (runtime_process_context& ctx)
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
void runtime_holder<typeT>::process_structs (runtime_process_context& ctx)
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
rx_result runtime_holder<typeT>::add_target_relation (relations::relation_data::smart_ptr data)
{
    auto ctx = create_start_context();
    return relations_.add_target_relation(std::move(data), ctx);
}

template <class typeT>
rx_result runtime_holder<typeT>::remove_target_relation (const string_type& name)
{
    runtime_stop_context ctx;
    auto result = relations_.remove_target_relation(name, ctx);
    if (result)
    {
        connected_tags_.target_relation_removed(result.move_value());
        return true;
    }
    else
    {
        return result.errors();
    }
}

template <class typeT>
void runtime_holder<typeT>::process_own (runtime_process_context& ctx)
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
typename typeT::instance_data_t runtime_holder<typeT>::get_definition_data ()
{
    typename typeT::instance_data_t def_data;
    def_data.meta_info = meta_info_;
    def_data.instance_data = instance_data_.get_data();
    def_data.overrides = overrides_;
    return def_data;
}

template <class typeT>
rx_result runtime_holder<typeT>::add_implicit_relation (relations::relation_data::smart_ptr data)
{
    return relations_.add_implicit_relation(std::move(data));
}

template class runtime_holder<object_types::object_type>;
template class runtime_holder<object_types::domain_type>;
template class runtime_holder<object_types::port_type>;
template class runtime_holder<object_types::application_type>;
// Parameterized Class rx_platform::runtime::algorithms::process_runtime_job 

template <class typeT>
process_runtime_job<typeT>::process_runtime_job (typename typeT::RTypePtr whose)
      : whose_(whose)
{
}



template <class typeT>
void process_runtime_job<typeT>::process ()
{
    object_runtime_algorithms<typeT>::process_runtime(*whose_);
}


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform

