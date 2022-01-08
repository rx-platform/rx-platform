

/****************************************************************************
*
*  system\runtime\rx_holder_algorithms.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_holder_algorithms
#include "system/runtime/rx_holder_algorithms.h"

#include "sys_internal/rx_inf.h"
#include "api/rx_platform_api.h"
#include "rx_write_transaction.h"
#include "rx_process_context.h"
#include "system/runtime/rx_blocks.h"
#include "system/serialization/rx_ser_json.h"


namespace rx_platform {

namespace runtime {

namespace algorithms {

// Parameterized Class rx_platform::runtime::algorithms::runtime_holder_algorithms 


template <class typeT>
std::vector<rx_result_with<runtime_handle_t> > runtime_holder_algorithms<typeT>::connect_items (const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose)
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
        auto one_result = whose.tags_.connected_tags_.connect_tag(path, *whose.tags_.item_, monitor);
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
rx_result runtime_holder_algorithms<typeT>::read_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    std::vector<rx_result> results;
    for (const auto& item : items)
        whose.tags_.connected_tags_.read_tag(item, monitor);
    return true;
}

template <class typeT>
void runtime_holder_algorithms<typeT>::fire_job (typename typeT::RType& whose)
{
    locks::auto_lock_t<decltype(whose.job_lock_)> _(&whose.job_lock_);
    if (!whose.job_pending_)
    {
        whose.job_pending_ = true;
        RX_ASSERT(whose.my_job_ptr_);
        if (whose.my_job_ptr_)
        {
            auto executer = rx_internal::infrastructure::server_runtime::instance().get_executer(whose.get_executer());
            executer->append(whose.my_job_ptr_);
        }
    }
}

template <class typeT>
rx_result runtime_holder_algorithms<typeT>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    std::vector<rx_result> results;
    for (const auto& item : items)
    {
        auto result = whose.tags_.connected_tags_.write_tag(transaction_id, item.first, rx_simple_value(item.second), monitor);
        results.emplace_back(std::move(result));
    }
    return true;
}

template <class typeT>
std::vector<rx_result> runtime_holder_algorithms<typeT>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    std::vector<rx_result> results;
    if (!items.empty())
    {
        results.reserve(items.size());
        for (const auto& handle : items)
        {
            auto one_result = whose.tags_.connected_tags_.disconnect_tag(handle, monitor);
            results.emplace_back(std::move(one_result));
        }
    }
    return results;
}

template <class typeT>
void runtime_holder_algorithms<typeT>::save_runtime (typename typeT::RType& whose)
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
                result = serialize_runtime_value(item_result.value()->write_stream(), runtime_value_type::persistent_runtime_value, whose);
                if (result)
                {
                    result = item_result.value()->write_stream().write_footer();
                    if (result)
                        result = item_result.value()->commit_write();
                    if (result)
                        RUNTIME_LOG_DEBUG("runtime_model_algorithm", 100, "Saved "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + whose.meta_info_.get_full_path());
                    else
                        RUNTIME_LOG_ERROR("runtime_model_algorithm", 100, "Error saving "s + rx_item_type_name(typeT::RImplType::type_id) + " "s + whose.meta_info_.get_full_path());
                }
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

template <class typeT>
runtime_process_context runtime_holder_algorithms<typeT>::create_context (typename typeT::RType& whose)
{
    return runtime_process_context(whose.tags_.binded_tags_, whose.tags_.connected_tags_, whose.meta_info_, &whose.directories_);
}

template <class typeT>
runtime_init_context runtime_holder_algorithms<typeT>::create_init_context (typename typeT::RType& whose)
{
    return runtime::runtime_init_context(*whose.tags_.item_, whose.meta_info_, &whose.context_, &whose.tags_.binded_tags_, &whose.directories_, typeT::runtime_type_id);
}

template <class typeT>
runtime_start_context runtime_holder_algorithms<typeT>::create_start_context (typename typeT::RType& whose)
{
    return runtime_start_context(*whose.tags_.item_, &whose.context_, &whose.tags_.binded_tags_, &whose.directories_, &whose.relations_);
}

template <class typeT>
rx_result runtime_holder_algorithms<typeT>::get_value_ref (const string_type& path, rt_value_ref& ref, typename typeT::RType& whose)
{
    if (whose.tags_.is_this_yours(path))
        return whose.tags_.get_value_ref(path, ref);
    else if (whose.relations_.is_this_yours(path))
        whose.relations_.get_value_ref(path, ref);
    else if (whose.logic_.is_this_yours(path))
        whose.logic_.get_value_ref(path, ref);
    else if (whose.displays_.is_this_yours(path))
        whose.displays_.get_value_ref(path, ref);
    return path + " not found!";
}

template <class typeT>
void runtime_holder_algorithms<typeT>::browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback, typename typeT::RType& whose)
{
    rx_result result;
    static std::vector<runtime_item_attribute> g_empty_result;
    if (path.empty())
    {
        std::vector<runtime_item_attribute> items;
        auto ret = whose.tags_.browse(prefix, path, filter, items, &whose.context_);
        if (ret)
        {
            ret = whose.relations_.browse(prefix, filter, items);
            if (ret)
            {
                ret = whose.logic_.browse(prefix, path, filter, items, &whose.context_);
                if (ret)
                {
                    ret = whose.displays_.browse(prefix, path, filter, items, &whose.context_);
                }
            }
        }
        callback(std::move(ret), std::move(items));
    }
    else
    {
        if (whose.tags_.is_this_yours(path))
        {
            std::vector<runtime_item_attribute> items;
            auto result = whose.tags_.browse(prefix, path, filter, items, &whose.context_);
            callback(std::move(result), std::move(items));
        }
        else if (whose.relations_.is_this_yours(path))
        {
            // this one sends it's own callback, it can be remote!!!
            whose.relations_.browse(prefix, path, filter, std::move(callback));
        }
        else if (whose.logic_.is_this_yours(path))
        {
            std::vector<runtime_item_attribute> items;
            auto result = whose.logic_.browse(prefix, path, filter, items, &whose.context_);
            callback(std::move(result), std::move(items));
        }
        else if (whose.displays_.is_this_yours(path))
        {
            std::vector<runtime_item_attribute> items;
            auto result = whose.displays_.browse(prefix, path, filter, items, &whose.context_);
            callback(std::move(result), std::move(items));
        }
        else
        {
            callback(path + " not found!", g_empty_result);
        }
    }
}

template <class typeT>
void runtime_holder_algorithms<typeT>::fill_data (const data::runtime_values_data& data, typename typeT::RType& whose)
{
    whose.tags_.fill_data(data, &whose.context_);
    whose.relations_.fill_data(data, &whose.context_);
    whose.logic_.fill_data(data, &whose.context_);
    whose.displays_.fill_data(data, &whose.context_);
}

template <class typeT>
void runtime_holder_algorithms<typeT>::collect_data (data::runtime_values_data& data, runtime_value_type type, const typename typeT::RType& whose)
{
    whose.tags_.collect_data(data, type);
    whose.relations_.collect_data(data, type);
    whose.logic_.collect_data(data, type);
    whose.displays_.collect_data(data, type);
}

template <class typeT>
void runtime_holder_algorithms<typeT>::read_value (const string_type& path, read_result_callback_t callback, const typename typeT::RType& whose)
{

    rx_result result;
    if (path.empty())
    {// our value
        values::rx_value value;
#ifndef RX_MIN_MEMORY
        if (!whose.json_cache_.empty())
        {
            value.assign_static(whose.json_cache_.c_str(), whose.meta_info_.modified_time);
        }
        else
        {
            serialization::json_writer writer;
            writer.write_header(STREAMING_TYPE_MESSAGE, 0);
            result = serialize_runtime_value(writer, runtime_value_type::simple_runtime_value, whose);
            if (result)
            {
                result = writer.write_footer();
                if (result)
                {
#ifdef _DEBUG
                    const_cast<string_type&>(whose.json_cache_) = writer.get_string();
#else
                    const_cast<string_type&>(whose.json_cache_) = writer.get_string();
#endif
                    if (!whose.json_cache_.empty())
                        value.assign_static(whose.json_cache_.c_str(), whose.meta_info_.modified_time);
                                        
                }
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
                value.assign_static(temp_str.c_str(), meta_info_.modified_time);
            }
        }
#endif
        callback(std::move(result), std::move(value));
    }
    else if (path == "Storage")
    {// our runtime value
        values::rx_value value;
        serialization::json_writer writer;
        writer.write_header(STREAMING_TYPE_MESSAGE, 0);
        result = serialize_runtime_value(writer, runtime_value_type::persistent_runtime_value, whose);
        if (result)
        {
            result = writer.write_footer();
            if (result)
            {
                string_type temp_str(writer.get_string());
                if (!temp_str.empty())
                {
                    value.assign_static(temp_str.c_str(), whose.meta_info_.modified_time);
                }
            }
        }
        callback(std::move(result), std::move(value));
    }
    else
    {
        values::rx_value value;

        if (whose.tags_.is_this_yours(path))
            result = whose.tags_.get_value(path, value, const_cast<runtime_process_context*>(&whose.context_));
        else if (whose.relations_.is_this_yours(path))
        {
            whose.relations_.read_value(path, std::move(callback), const_cast<runtime_process_context*>(&whose.context_));
            return;// relations will do callback!!!
        }
        else if (whose.logic_.is_this_yours(path))
            result = whose.logic_.get_value(path, value, const_cast<runtime_process_context*>(&whose.context_));
        else if (whose.displays_.is_this_yours(path))
            result = whose.displays_.get_value(path, value, const_cast<runtime_process_context*>(&whose.context_));
        else
            result = path + " not found!";

        callback(std::move(result), std::move(value));
    }
}

template <class typeT>
void runtime_holder_algorithms<typeT>::write_value (const string_type& path, rx_simple_value&& val, write_result_callback_t callback, api::rx_context ctx, typename typeT::RType& whose)
{
    auto connect_result = whose.tags_.connected_tags_.connect_tag(path, *whose.tags_.item_);
    if (!connect_result)
    {
        connect_result.register_error("Error writing to item "s + path);
        callback(connect_result.errors());
        return;
    }
    auto transaction_ptr = rx_create_reference<write_item_transaction>(std::move(callback));
    auto result = whose.tags_.connected_tags_.write_tag(1, connect_result.value(), std::move(val), transaction_ptr);
    if (!result)
    {
        whose.tags_.connected_tags_.disconnect_tag(connect_result.value());
        (*transaction_ptr)(std::move(result));
        return;
    }
}

template <class typeT>
void runtime_holder_algorithms<typeT>::read_struct (string_view_type path, read_struct_data data, const typename typeT::RType& whose)
{
    rx_result result;
    data::runtime_values_data collected_data;
    if (path.empty())
    {// our value
        whose.tags_.collect_data(collected_data, data.type);
        whose.relations_.collect_data(collected_data, data.type);
        whose.logic_.collect_data(collected_data, data.type);
        whose.displays_.collect_data(collected_data, data.type);
        result = true;
    }
    else
    {
        // someone's else
        string_view_type item;
        auto idx = path.find(RX_OBJECT_DELIMETER);
        if (idx == string_view_type::npos)
            item = path;
        else
            item = path.substr(0, idx);

        if (whose.tags_.is_this_yours(item))
        {
            result = whose.tags_.get_struct_value(path, collected_data, data.type, const_cast<runtime_process_context*>(&whose.context_));
        }
        else if (whose.relations_.is_this_yours(item))
        {
            whose.relations_.read_struct(path, std::move(data));
            return;// relations will do callback!!!
        }
        else if (whose.logic_.is_this_yours(item))
        {
            result = whose.logic_.get_struct_value(item, path.substr(idx+1), collected_data, data.type, const_cast<runtime_process_context*>(&whose.context_));
        }
        else if (whose.displays_.is_this_yours(item))
        {
            result = whose.displays_.get_struct_value(item, path.substr(idx + 1), collected_data, data.type, const_cast<runtime_process_context*>(&whose.context_));
        }
        else
            result = string_type(path) + " not found!";
    }
    data.callback(std::move(result), std::move(collected_data));
}

template <class typeT>
void runtime_holder_algorithms<typeT>::write_struct (string_view_type path, write_struct_data data, typename typeT::RType& whose)
{
}

template <class typeT>
rx_result runtime_holder_algorithms<typeT>::serialize_runtime_value (base_meta_writer& stream, runtime_value_type type, const typename typeT::RType& whose)
{
    data::runtime_values_data data;
    collect_data(data, type, whose);
    if (!stream.write_init_values(nullptr, data))
        return stream.get_error();
    else
        return true;
}

template <>
std::vector<rx_result_with<runtime_handle_t> > runtime_holder_algorithms<meta::object_types::relation_type>::connect_items(const string_array& paths, runtime::tag_blocks::tags_callback_ptr monitor, meta::object_types::relation_type::RType& whose)
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

template class runtime_holder_algorithms<meta::object_types::port_type>;
template class runtime_holder_algorithms<meta::object_types::object_type>;
template class runtime_holder_algorithms<meta::object_types::domain_type>;
template class runtime_holder_algorithms<meta::object_types::application_type>;

// Class rx_platform::runtime::algorithms::runtime_relation_algorithms 


void runtime_relation_algorithms::notify_relation_connected (const string_type& name, const platform_item_ptr& item, runtime_process_context* ctx)
{
    auto& subscribers = ctx->tags_.parent_relations_->relation_subscribers_;
    auto it = subscribers.find(name);
    if (it != subscribers.end())
    {
        for (auto& one : it->second)
            one->relation_connected(name, item);
    }
}

void runtime_relation_algorithms::notify_relation_disconnected (const string_type& name, runtime_process_context* ctx)
{
    auto& subscribers = ctx->tags_.parent_relations_->relation_subscribers_;
    auto it = subscribers.find(name);
    if (it != subscribers.end())
    {
        for (auto& one : it->second)
            one->relation_disconnected(name);
    }
}

void runtime_relation_algorithms::relation_value_change (relations::relation_value_data* whose, const rx_value& val, runtime_process_context* ctx)
{
    if (val.is_dead())
    {// avoid sending dead to others
        rx_value local_val(val);
        local_val.set_quality(RX_NOT_CONNECTED_QUALITY);
        ctx->tags_.relation_value_change(whose, val);
    }
    else
    {
        ctx->tags_.relation_value_change(whose, val);
    }
}


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform

