

/****************************************************************************
*
*  system\runtime\rx_runtime_holder.cpp
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

#include "system/meta/rx_obj_types.h"

// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_runtime_holder
#include "system/runtime/rx_runtime_holder.h"

#include "sys_internal/rx_async_functions.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/meta/rx_obj_types.h"
#include "rx_write_transaction.h"
#include "rx_scan_algorithms.h"
#include "rx_holder_algorithms.h"


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

// Parameterized Class rx_platform::runtime::algorithms::runtime_holder 

template <class typeT>
runtime_holder<typeT>::runtime_holder (const meta_data& meta, const typename typeT::instance_data_t& instance, typename typeT::runtime_behavior_t&& rt_behavior)
      : job_pending_(false),
        points_count(0)
    , meta_info_(meta)
    , instance_data_(instance.instance_data, std::move(rt_behavior))
    , security_guard_(rx_create_reference<security::security_guard>(meta, security::rx_security_null))
{
    using rimpl_t = typename typeT::RImplType;
    context_ = runtime_holder_algorithms<typeT>::create_context(*this, nullptr);
    
    RUNTIME_LOG_DEBUG("runtime_holder", 900, (rx_item_type_name(rimpl_t::type_id) + " constructor, for " + meta.get_full_path()));
}


template <class typeT>
runtime_holder<typeT>::~runtime_holder()
{
    using rimpl_t = typename typeT::RImplType;
    if (meta_info_.name.empty())
    {
        RUNTIME_LOG_DEBUG("runtime_holder", 900, (rx_item_type_name(rimpl_t::type_id) + " destructor, for unknown"));
    }
    else
    {
        RUNTIME_LOG_DEBUG("runtime_holder", 900, (rx_item_type_name(rimpl_t::type_id) + " destructor, for " + meta_info_.get_full_path()));
    }
    RUNTIME_LOG_DEBUG("runtime_holder", 900, "Deleted "s + rx_item_type_name(typeT::runtime_type_id) + ": " + meta_info_.name);
}



template <class typeT>
runtime_process_context& runtime_holder<typeT>::get_context ()
{
    return *context_;
}

template <class typeT>
rx_result runtime_holder<typeT>::serialize (base_meta_writer& stream, uint8_t type) const
{
    rx_result ret;
    if (type != STREAMING_TYPE_MESSAGE)
    {
        ret = meta_info_.serialize_meta_data(stream, type, typeT::RImplType::type_id);
        if (!ret)
            return ret;
    }

    if (!stream.start_object("def"))
        return stream.get_error();

    if (!instance_data_.get_data().serialize(stream, type))
        return stream.get_error();

    if (!stream.write_init_values("overrides", overrides_))
        return stream.get_error();

    if (!logic_.serialize(stream, type))
        return stream.get_error();
    
    if (!stream.end_object())
        return stream.get_error();

    return true;
}

template <class typeT>
rx_result runtime_holder<typeT>::initialize_runtime (runtime_init_context& ctx)
{
    my_job_ptr_ = rx_create_reference<process_runtime_job<typeT> >(smart_this());
    ctx.anchor = smart_this();
    job_pending_ = true;// this is a false stuff to pause execution untill the end of initialization
    context_->init_state([this]
        {
            runtime_holder_algorithms<typeT>::fire_job(*this);
        });
    ctx.context = context_.get();
    string_type rt_path = meta_info_.get_full_path();
    auto result = tags_.initialize_runtime(ctx, &relations_, &logic_, &displays_);
    if (result)
    {
        result = relations_.initialize_relations(ctx);
        if (result)
        {
            result = logic_.initialize_logic(ctx);
            if (result)
            {
                result = displays_.initialize_displays(ctx, rt_path);
            }
        }
    }
    if (result)
    {
        result = implementation_->initialize_runtime(ctx);

        if (result)
        {
            // point_count!!!!
            points_count = ctx.points_count;// +1;
        }
    }
    ctx.structure.pop_item();
    job_pending_ = false;
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::deinitialize_runtime (runtime_deinit_context& ctx)
{
    string_type rt_path = meta_info_.get_full_path();
    rx_result result = implementation_->deinitialize_runtime(ctx);
    if (result)
    {
        
        result = displays_.deinitialize_displays(ctx, rt_path);
        if (result)
        {
            result = logic_.deinitialize_logic(ctx);
            if (result)
            {
                result = relations_.deinitialize_relations(ctx);
                if (result)
                {
                    result = tags_.deinitialize_runtime(ctx);
                }
            }
        }
    }
    context_->runtime_deinitialized();
    my_job_ptr_ = process_runtime_job<typeT>::smart_ptr::null_ptr;
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::start_runtime (runtime_start_context& ctx)
{
    context_->start_state(std::move(ctx.status_data));
    ctx.get_platform_item = [this]() {return get_item_ptr(); };
    string_type rt_path = meta_info_.get_full_path();
    auto result = tags_.start_runtime(ctx);
    if (result)
    {
        result = relations_.start_relations(ctx);
        if (result)
        {
            result = logic_.start_logic(ctx);
            if (result)
            {
                RX_ASSERT(rx_thread_context() == instance_data_.get_executer());
                result = displays_.start_displays(ctx, rt_path);
            }
        }
    }
    if (result)
    {
        tags_.collect_data(runtime_data_cache_, runtime_value_type::simple_runtime_value);
        result = implementation_->start_runtime(ctx);
        if (result)
        {
            context_->started(rx_time::now());
            tags_.runtime_started(ctx);
        }
    }
    ctx.structure.pop_item();
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::stop_runtime (runtime_stop_context& ctx)
{
    string_type rt_path = meta_info_.get_full_path();
    context_->runtime_stopping();
    rx_result result = implementation_->stop_runtime(ctx);
    if (result)
    {
        RX_ASSERT(typeT::runtime_type_id == rx_item_type::rx_port || rx_thread_context() == instance_data_.get_executer());
        result = displays_.stop_displays(ctx, rt_path);
        if (result)
        {
            result = logic_.stop_logic(ctx);
            if (result)
            {
                result = relations_.stop_relations(ctx);
                if (result)
                {
                    result = tags_.stop_runtime(ctx);
                }
            }
        }
    }
    context_->runtime_stopped();
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::do_command (rx_object_command_t command_type)
{
    return context_->do_command(command_type);
}

template <class typeT>
meta_data& runtime_holder<typeT>::meta_info ()
{
  return meta_info_;

}

template <class typeT>
platform_item_ptr runtime_holder<typeT>::get_item_ptr () const
{
    return std::make_unique<rx_internal::internal_ns::rx_item_implementation<smart_ptr> >(smart_this());
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
const typename typeT::runtime_data_t& runtime_holder<typeT>::get_instance_data () const
{
    return instance_data_;
}

template <class typeT>
rx_result runtime_holder<typeT>::add_target_relation (relations::relation_data::smart_ptr data)
{
    runtime_start_context ctx = runtime_holder_algorithms<typeT>::create_start_context(*this);
    return relations_.add_target_relation(std::move(data), ctx);
}

template <class typeT>
rx_result runtime_holder<typeT>::remove_target_relation (const string_type& name)
{
    runtime_stop_context ctx(meta_info_, context_.get());
    auto result = relations_.remove_target_relation(name, ctx);
    if (result)
    {
        tags_.target_relation_removed(result.move_value());
        return true;
    }
    else
    {
        return result.errors();
    }
}

template <class typeT>
typename typeT::instance_data_t runtime_holder<typeT>::get_definition_data ()
{
    typename typeT::instance_data_t def_data{};
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

template <class typeT>
security::security_guard_ptr runtime_holder<typeT>::get_security_guard ()
{
    return security_guard_;
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
process_runtime_job<typeT>::~process_runtime_job()
{
}



template <class typeT>
void process_runtime_job<typeT>::process ()
{
    runtime_scan_algorithms<typeT>::process_runtime(*whose_);
}


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform

