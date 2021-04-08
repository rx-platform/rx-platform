

/****************************************************************************
*
*  system\runtime\rx_holder_algorithms.cpp
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


// rx_holder_algorithms
#include "system/runtime/rx_holder_algorithms.h"

#include "sys_internal/rx_inf.h"
#include "api/rx_platform_api.h"
#include "rx_write_transaction.h"
#include "rx_process_context.h"


namespace rx_platform {

namespace runtime {

namespace algorithms {

// Parameterized Class rx_platform::runtime::algorithms::runtime_holder_algorithms


template <class typeT>
std::vector<rx_result_with<runtime_handle_t> > runtime_holder_algorithms<typeT>::connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose)
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
rx_result runtime_holder_algorithms<typeT>::read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose)
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
        auto executer = rx_internal::infrastructure::server_runtime::instance().get_executer(whose.get_executer());
        executer->append(whose.my_job_ptr_);
    }
}

template <class typeT>
rx_result runtime_holder_algorithms<typeT>::write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose)
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
std::vector<rx_result> runtime_holder_algorithms<typeT>::disconnect_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, typename typeT::RType& whose)
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
rx_result runtime_holder_algorithms<typeT>::write_value (const string_type& path, rx_simple_value&& val, rx_result_callback callback, api::rx_context ctx, typename typeT::RType& whose)
{
    auto transaction_ptr = rx_create_reference<write_item_transaction>(std::move(callback), rx_thread_context());
    auto connect_result = whose.tags_.connected_tags_.connect_tag(path, *whose.tags_.item_, transaction_ptr);
    if (!connect_result)
    {
        connect_result.register_error("Error writing to item "s + path);
        return connect_result.errors();
    }
    auto result = whose.tags_.connected_tags_.write_tag(1, connect_result.value(), std::move(val), transaction_ptr);
    if (!result)
    {
        whose.tags_.connected_tags_.disconnect_tag(connect_result.value(), transaction_ptr);
    }
    return result;
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

template <class typeT>
runtime_process_context runtime_holder_algorithms<typeT>::create_context (typename typeT::RType& whose)
{
    return runtime_process_context(whose.tags_.binded_tags_, whose.tags_.connected_tags_, whose.meta_info_, &whose.directories_, whose.tags_.points_.get());
}

template <class typeT>
runtime_init_context runtime_holder_algorithms<typeT>::create_init_context (typename typeT::RType& whose)
{
    return runtime::runtime_init_context(*whose.tags_.item_, whose.meta_info_, &whose.context_, &whose.tags_.binded_tags_, &whose.directories_);
}

template <class typeT>
runtime_start_context runtime_holder_algorithms<typeT>::create_start_context (typename typeT::RType& whose)
{
    return runtime_start_context(*whose.tags_.item_, &whose.context_, &whose.directories_);
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
rx_result runtime_holder_algorithms<typeT>::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, typename typeT::RType& whose)
{
    rx_result result;
    if (path.empty())
    {
        auto ret = whose.tags_.browse(prefix, path, filter, items, &whose.context_);
        if (ret)
        {
            ret = whose.relations_.browse(prefix, path, filter, items);
            if (ret)
            {
                ret = whose.logic_.browse(prefix, path, filter, items, &whose.context_);
                if (ret)
                {
                    ret = whose.displays_.browse(prefix, path, filter, items, &whose.context_);
                }
            }
        }
        return ret;
    }
    else
    {
        if (whose.tags_.is_this_yours(path))
            return whose.tags_.browse(prefix, path, filter, items, &whose.context_);
        else if (whose.relations_.is_this_yours(path))
            return whose.relations_.browse(prefix, path, filter, items);
        else if (whose.logic_.is_this_yours(path))
            return whose.logic_.browse(prefix, path, filter, items, &whose.context_);
        else if (whose.displays_.is_this_yours(path))
            return whose.displays_.browse(prefix, path, filter, items, &whose.context_);
        else
            return path + " not found!";
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
rx_result runtime_holder_algorithms<typeT>::read_value (const string_type& path, rx_value& value, const typename typeT::RType& whose)
{

    rx_result result;
    if (path.empty())
    {// our value
#ifndef RX_MIN_MEMORY
        if (!whose.json_cache_.empty())
        {
            value.assign_static<string_type>(string_type(whose.json_cache_), whose.meta_info_.modified_time);
        }
        else
        {
            serialization::json_writer writer;
            writer.write_header(STREAMING_TYPE_MESSAGE, 0);
            result = serialize_runtime_value(writer, runtime_value_type::simple_runtime_value, whose);
            if (result)
            {
#ifdef _DEBUG
                if (writer.get_string(const_cast<string_type&>(whose.json_cache_), true))
#else
                if (writer.get_string(const_cast<string_type&>(whose.json_cache_), false))
#endif
                    value.assign_static<string_type>(string_type(whose.json_cache_), whose.meta_info_.modified_time);
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
        result = serialize_runtime_value(writer, runtime_value_type::persistent_runtime_value, whose);
        if (result)
        {
            string_type temp_str;
            if (writer.get_string(const_cast<string_type&>(temp_str), true))
            {
                value.assign_static<string_type>(string_type(temp_str), whose.meta_info_.modified_time);
            }
        }
    }
    else
    {
        if (whose.tags_.is_this_yours(path))
            return whose.tags_.get_value(path, value, const_cast<runtime_process_context*>(&whose.context_));
        else if (whose.relations_.is_this_yours(path))
            whose.relations_.get_value(path, value, const_cast<runtime_process_context*>(&whose.context_));
        else if (whose.logic_.is_this_yours(path))
            whose.logic_.get_value(path, value, const_cast<runtime_process_context*>(&whose.context_));
        else if (whose.displays_.is_this_yours(path))
            whose.displays_.get_value(path, value, const_cast<runtime_process_context*>(&whose.context_));
        return path + " not found!";
    }

    return result;
}

template <class typeT>
rx_result runtime_holder_algorithms<typeT>::serialize_runtime_value (base_meta_writer& stream, runtime_value_type type, const typename typeT::RType& whose)
{
    data::runtime_values_data data;
    collect_data(data, type, whose);
    if (!stream.write_init_values(nullptr, data))
        return "Error writing values to the stream";
    else
        return true;
}

template <>
std::vector<rx_result_with<runtime_handle_t> > runtime_holder_algorithms<meta::object_types::relation_type>::connect_items(const string_array& paths, runtime::operational::tags_callback_ptr monitor, meta::object_types::relation_type::RType& whose)
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

} // namespace algorithms
} // namespace runtime
} // namespace rx_platform

