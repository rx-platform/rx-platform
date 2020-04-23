

/****************************************************************************
*
*  system\runtime\rx_runtime_holder.cpp
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

#include "system/meta/rx_obj_types.h"

// rx_relations
#include "system/runtime/rx_relations.h"
// rx_runtime_holder
#include "system/runtime/rx_runtime_holder.h"

#include "api/rx_platform_api.h"
#include "sys_internal/rx_async_functions.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/meta/rx_obj_types.h"


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
        auto one_result = whose.connected_tags_.connect_tag(path, *whose.item_, monitor, whose.state_);
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
    whose.process_runtime(whose.context_);
}

template <class typeT>
rx_result object_runtime_algorithms<typeT>::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    std::vector<rx_result> results;
    auto ret = whose.read_items(items, monitor, results);
    if (ret)
    {
        whose.context_.tag_updates_pending();
    }
    return ret;
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
    auto ret = whose.write_items(transaction_id, items, monitor, results);
    if (ret)
    {
        whose.context_.tag_writes_pending();
    }
    return ret;
}

template <class typeT>
rx_result object_runtime_algorithms<typeT>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, std::vector<rx_result>& results, bool& has_errors, typename typeT::RType& whose)
{
    if (items.empty())
        return true;
    results.clear();// just in case
    results.reserve(items.size());
    has_errors = false;
    for (const auto& handle : items)
    {
        auto one_result = whose.connected_tags_.disconnect_tag(handle, monitor);
        if (!has_errors && !one_result)
            has_errors = true;
        results.emplace_back(std::move(one_result));
    }
    return true;
}


template <>
void object_runtime_algorithms<object_types::port_type>::process_runtime(typename object_types::port_type::RType& whose)
{
    whose.job_lock_.lock();
    whose.job_pending_ = false;
    whose.job_lock_.unlock();
    whose.context_.init_context();
    whose.process_runtime(whose.context_);
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
runtime_holder<typeT>::runtime_holder()
      : scan_time_item_(0),
        last_scan_time_(-1),
        max_scan_time_(0),
        max_scan_time_item_(0),
        job_pending_(false)
    , meta_info_(namespace_item_pull_access)
    , context_(binded_tags_, connected_tags_)
    , state_(relations_, &context_)
{
    my_job_ptr_ = rx_create_reference<process_runtime_job<typeT> >(smart_this());
}

template <class typeT>
runtime_holder<typeT>::runtime_holder (const meta::meta_data& meta, const typename typeT::instance_data_t& instance)
      : scan_time_item_(0),
        last_scan_time_(-1),
        max_scan_time_(0),
        max_scan_time_item_(0),
        job_pending_(false)
    , meta_info_(meta)
    , instance_data_(instance)
    , context_(binded_tags_, connected_tags_)
    , state_(relations_, &context_)
{
    my_job_ptr_ = rx_create_reference<process_runtime_job<typeT> >(smart_this());
}



template <class typeT>
rx_result runtime_holder<typeT>::read_value (const string_type& path, rx_value& value) const
{
    rx_result result;
    if (path.empty())
    {// our value
        if (!json_cache_.empty())
        {
            value.assign_static<string_type>(string_type(json_cache_), meta_info_.get_modified_time());
        }
        else
        {
            serialization::json_writer writer;
            writer.write_header(STREAMING_TYPE_MESSAGE, 0);
            result = serialize_value(writer, runtime_value_type::simple_runtime_value);
            if (result)
            {
#ifdef _DEBUG
                writer.get_string(const_cast<string_type&>(json_cache_), true);
#else
                writer.get_string(const_cast<runtime_holder<typeT>* >(this)->json_cache_, false);
#endif
                value.assign_static<string_type>(string_type(json_cache_), meta_info_.get_modified_time());
            }
        }
    }
    else
    {
        result = item_->get_value(state_, path, value);
    }
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx, rx_thread_handle_t whose)
{
    if (path.empty())
    {// our value
        return RX_ACCESS_DENIED;
    }
    std::function<rx_result(const string_type&, rx_simple_value)> func = [this, ctx](const string_type& path, rx_simple_value val)
    {
        structure::write_context my_ctx = structure::write_context::create_write_context(state_, false);
        return item_->write_value(path, std::move(val), my_ctx);
    };
    auto current_thread = rx_thread_context();
    if (current_thread == whose)
    {
        auto result = func(path, std::move(val));
        callback(std::move(result));
        return true;
    }
    else
    {
        rx_do_with_callback<rx_result, decltype(ctx.object), const string_type&, rx_simple_value>(func, whose, callback, ctx.object, path, std::move(val));
        return true;
    }
}

template <class typeT>
bool runtime_holder<typeT>::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!meta_info_.serialize_meta_data(stream, type, typeT::RImplType::type_id))
        return false;

    if (!stream.start_object("def"))
        return false;

    if (!instance_data_.serialize(stream, type))
        return false;

    if (!stream.write_init_values("overrides", overrides_))
        return false;
    if (!stream.start_array("programs", programs_.size()))
        return false;
    for (const auto& one : programs_)
    {
        if (!one->save_program(stream, type))
            return false;
    }
    if (!stream.end_array())
        return false;

    if (!stream.end_object())
        return false;

    return true;
}

template <class typeT>
bool runtime_holder<typeT>::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("def"))
        return false;

    if (!instance_data_.deserialize(stream, type))
        return false;

    if (!stream.read_init_values("overrides", overrides_))
        return false;
    if (!stream.start_array("programs"))
        return false;
    while (!stream.array_end())
    {
        logic::ladder_program::smart_ptr one(pointers::_create_new);
        if (!one->load_program(stream, type))
            return false;
        programs_.push_back(one);
    }

    if (!stream.end_object())
        return false;

    return true;
}

template <class typeT>
rx_result runtime_holder<typeT>::initialize_runtime (runtime_init_context& ctx)
{
    context_.init_state(&state_, [this]
        {
            object_runtime_algorithms<typeT>::fire_job(*this);
        });
    ctx.structure.push_item(*item_);
    ctx.context = &context_;
   
    auto result = item_->initialize_runtime(ctx);
    if (result)
    {
        auto bind_result = ctx.tags->bind_item("Object.LastScanTime", ctx);
        if (bind_result)
            scan_time_item_ = bind_result.value();
        bind_result = ctx.tags->bind_item("Object.MaxScanTime", ctx);
        if (bind_result)
            max_scan_time_item_ = bind_result.value();

        for (auto& one : relations_)
        {
            result = one->initialize_runtime(ctx);
            if (!result)
                break;
        }
        if (result)
        {
            for (auto& one : programs_)
            {
                result = one->initialize_runtime(ctx);
                if (!result)
                    break;
            }
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
    for (auto& one : programs_)
    {
        result = one->deinitialize_runtime(ctx);
        if (!result)
            break;
    }
    if (result)
    {
        for (auto& one : relations_)
        {
            result = one->deinitialize_runtime(ctx);
            if (!result)
                break;
        }
        if (result)
        {
            result = item_->deinitialize_runtime(ctx);
        }
    }
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::start_runtime (runtime_start_context& ctx)
{
    state_.time = rx_time::now();
    ctx.structure.push_item(*item_);
    auto result = item_->start_runtime(ctx);
    if (result)
    {
        for (auto& one : relations_)
        {
            result = one->start_runtime(ctx);
            if (!result)
                break;
        }
        if (result)
        {
            for (auto& one : programs_)
            {
                result = one->start_runtime(ctx);
                if (!result)
                    break;
            }
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
    for (auto& one : programs_)
    {
        result = one->stop_runtime(ctx);
        if (!result)
            break;
    }
    if (result)
    {
        for (auto& one : relations_)
        {
            result = one->stop_runtime(ctx);
            if (!result)
                break;
        }
        if (result)
        {
            result = item_->stop_runtime(ctx);
        }
    }
    connected_tags_.runtime_stopped(rx_time::now());
    return result;
}

template <class typeT>
rx_result runtime_holder<typeT>::do_command (rx_object_command_t command_type)
{
    switch (command_type)
    {
    case rx_object_command_t::rx_turn_off:
        {
            if (state_.mode.turn_off())
            {
                state_.time = rx_time::now();
                item_->object_state_changed(state_);
            }
        }
        break;
    case rx_object_command_t::rx_turn_on:
        {
            if (state_.mode.turn_on())
            {
                state_.time = rx_time::now();
                item_->object_state_changed(state_);
            }
        }
        break;
    case rx_object_command_t::rx_set_blocked:
        {
            if (state_.mode.set_blocked())
            {
                state_.time = rx_time::now();
                item_->object_state_changed(state_);
            }
        }
        break;
    case rx_object_command_t::rx_reset_blocked:
        {
            if (state_.mode.reset_blocked())
            {
                state_.time = rx_time::now();
                item_->object_state_changed(state_);
            }

        }
        break;
    case rx_object_command_t::rx_set_test:
        {
            if (state_.mode.set_test())
            {
                state_.time = rx_time::now();
                item_->object_state_changed(state_);
            }
        }
        break;
    case rx_object_command_t::rx_reset_test:
        {
            if (state_.mode.reset_test())
            {
                state_.time = rx_time::now();
                item_->object_state_changed(state_);
            }
        }
        break;
    default:
        return "Unsupported command type!";
    }
    return true;
}

template <class typeT>
void runtime_holder<typeT>::set_runtime_data (meta::runtime_data_prototype& prototype)
{
    for (auto one : prototype.additional_relations)
    {
        relations_.emplace_back(one);
    }
    item_ = std::move(create_runtime_data(prototype));
}

template <class typeT>
void runtime_holder<typeT>::fill_data (const data::runtime_values_data& data)
{
    structure::init_context ctx(relations_, &context_);
    ctx.context = &context_;
    item_->fill_data(data, ctx);
    // now do the relations
    // they create their own context!
    for (auto& one : relations_)
    {
        one->fill_data(data);
    }
}

template <class typeT>
void runtime_holder<typeT>::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
    item_->collect_data(data, type);
    for (auto& one : relations_)
    {
        one->collect_data(data, type);
    }
}

template <class typeT>
rx_result runtime_holder<typeT>::get_value_ref (const string_type& path, rt_value_ref& ref)
{
    return item_->get_value_ref(path, ref);
}

template <class typeT>
bool runtime_holder<typeT>::process_runtime (runtime_process_context& ctx)
{
    security::secured_scope _(instance_data_.get_security_context());
    bool ret = false;
    if (scan_time_item_)
        set_binded_as(scan_time_item_, last_scan_time_);
    auto old_tick = rx_get_us_ticks();
    do
    {
        if (ctx.should_process_writes())
            ret = ret | connected_tags_.process_runtime(ctx);
        if (ctx.should_process_tags())
            ret = ret | connected_tags_.process_runtime(ctx);

    } while (ctx.should_repeat());
    auto diff = rx_get_us_ticks() - old_tick;
    last_scan_time_ = (double)diff / 1000.0;
    if (max_scan_time_ < last_scan_time_)
    {
        max_scan_time_ = last_scan_time_;
        if (max_scan_time_item_)
            set_binded_as(max_scan_time_item_, max_scan_time_);
    }
    return ret;
}

template <class typeT>
rx_result runtime_holder<typeT>::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items)
{
    if (path.empty())
    {
        auto ret = item_->browse_items(filter, prefix + path, items);
        if (ret)
        {
            for (const auto one : relations_)
            {
                runtime_item_attribute attr;
                attr.full_path = prefix + one->name;
                attr.name = one->name;
                attr.type = rx_attribute_type::relation_attribute_type;
                items.push_back(attr);
            }
        }
        return ret;
    }
    else
    {
        string_type current_path(path + RX_OBJECT_DELIMETER);
        const auto& sub_item = item_->get_child_item(path);
        if (!sub_item)
        {
            auto idx = path.find(RX_OBJECT_DELIMETER);
            string_type sub_path;
            string_type rest_path;
            if (idx != string_type::npos)
            {
                sub_path = path.substr(0, idx);
                rest_path = path.substr(idx + 1);
            }
            else
            {
                sub_path = path;
            }
            for (auto one : relations_)
            {
                if (one->name == sub_path)
                {
                    return one->browse(prefix + sub_path + RX_OBJECT_DELIMETER, rest_path, filter, items);
                }
            }
            return path + " not found";
        }
        return sub_item->browse_items(filter, prefix + current_path, items);
    }
}

template <class typeT>
rx_result runtime_holder<typeT>::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, std::vector<rx_result>& results)
{
    for (const auto& item : items)
        connected_tags_.read_tag(item, monitor, state_);
    return true;
}

template <class typeT>
rx_result runtime_holder<typeT>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor, std::vector<rx_result>& results)
{
    for (const auto& item : items)
    {
        auto result = connected_tags_.write_tag(item.first, rx_simple_value(item.second), monitor, state_);
        results.emplace_back(std::move(result));
    }
    return true;
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
    return runtime::runtime_init_context(*item_, meta_info_, &context_, &binded_tags_);
}

template <class typeT>
runtime_start_context runtime_holder<typeT>::create_start_context ()
{
    return runtime_start_context(*item_, &context_);
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
const typename typeT::instance_data_t& runtime_holder<typeT>::get_instance_data () const
{
    return instance_data_;
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

