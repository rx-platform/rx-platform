

/****************************************************************************
*
*  system\runtime\rx_holder_algorithms.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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
#include "lib/rx_ser_json.h"
#include "system/runtime/rx_event_blocks.h"


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
        whose.context_->tag_updates_pending();
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
rx_result runtime_holder_algorithms<typeT>::write_items (runtime_transaction_id_t transaction_id, bool test, const std::vector<std::pair<runtime_handle_t, data::runtime_values_data> >& items, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    std::vector<rx_result> results;
    for (const auto& item : items)
    {
        auto result = whose.tags_.connected_tags_.write_tag(transaction_id, test, item.first, data::runtime_values_data(item.second), monitor);
        results.emplace_back(std::move(result));
    }
    return true;
}

template <class typeT>
rx_result runtime_holder_algorithms<typeT>::write_items (runtime_transaction_id_t transaction_id, bool test, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    std::vector<rx_result> results;
    for (const auto& item : items)
    {
        auto result = whose.tags_.connected_tags_.write_tag(transaction_id, test, item.first, rx_simple_value(item.second), monitor);
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
    auto storage_result = resolve_storage(whose.meta_info_);
    if (storage_result)
    {
        auto item_result = storage_result.value()->get_runtime_storage(whose.meta_info_, whose.get_type_id());
        if (item_result)
        {
            runtime_transaction_id_t this_transaction_id = whose.context_->serialize_trans_id_.exchange(0);
            serialization::pretty_json_writer writer;
            string_type data;

            writer.write_header(STREAMING_TYPE_MESSAGE, 0);
            auto result = serialize_runtime_value(writer, runtime_value_type::persistent_runtime_value, whose);
            if (result)
            {
                result = writer.write_footer();
                if (result)
                {
                    data = writer.get_string();
                }
            }

            //
            if (result)
            {
                byte_string buffer(data.size());
                if (!data.empty())
                {
                    memcpy(&buffer[0], &data[0], data.size());
                }

                result = item_result.value()->save(this_transaction_id, std::move(buffer), [ whose = whose.smart_this()](runtime_transaction_id_t trans_id, rx_result result)
                    {
                        if (!result)
                        {
                            RUNTIME_LOG_ERROR("runtime_model_algorithm", 100, "Error saving "s + rx_item_type_name(typeT::RImplType::type_id)
                                + " "s + whose->meta_info_.get_full_path() + " :" + result.errors_line());

                        }
                        else
                        {
                            RUNTIME_LOG_INFO("runtime_model_algorithm", 100, "Saved "s
                                + rx_item_type_name(typeT::RImplType::type_id) + " "s + whose->meta_info_.get_full_path());
                        }
                        auto it = whose->context_->serialize_callbacks_.find(trans_id);
                        if (it != whose->context_->serialize_callbacks_.end())
                        {
                            for (auto callback : it->second)
                            {
                                if (result)
                                    callback(true);
                                else
                                    callback(result.errors());
                            }
                            whose->context_->serialize_callbacks_.erase(it);
                        }
                    });
            }

            if (!result)
            {
                RUNTIME_LOG_ERROR("runtime_model_algorithm", 100, "Error saving "s
                    + rx_item_type_name(typeT::RImplType::type_id) + " "s + whose.meta_info_.get_full_path() + " " + result.errors_line());
                auto it = whose.context_->serialize_callbacks_.find(this_transaction_id);
                if (it != whose.context_->serialize_callbacks_.end())
                {
                    for (auto callback : it->second)
                    {
                        callback(result.errors());
                    }
                    whose.context_->serialize_callbacks_.erase(it);
                }
            }
        }
        else
        {

            runtime_transaction_id_t this_transaction_id = whose.context_->serialize_trans_id_.exchange(0);
            RUNTIME_LOG_ERROR("runtime_model_algorithm", 100, "Error opening runtime storage for "s
                + rx_item_type_name(typeT::RImplType::type_id) + " "s + whose.meta_info_.get_full_path() + " " + item_result.errors_line());


            auto it = whose.context_->serialize_callbacks_.find(this_transaction_id);
            if (it != whose.context_->serialize_callbacks_.end())
            {
                for (auto callback : it->second)
                {
                    callback(item_result.errors());
                }
                whose.context_->serialize_callbacks_.erase(it);
            }
        }
    }
    else
    {
        runtime_transaction_id_t this_transaction_id = whose.context_->serialize_trans_id_.exchange(0);
        RUNTIME_LOG_ERROR("runtime_model_algorithm", 100, "Error resolving storage for "s
            + rx_item_type_name(typeT::RImplType::type_id) + " "s + whose.meta_info_.get_full_path() + " " + storage_result.errors_line());

        auto it = whose.context_->serialize_callbacks_.find(this_transaction_id);
        if (it != whose.context_->serialize_callbacks_.end())
        {
            for (auto callback : it->second)
            {
                callback(storage_result.errors());
            }
            whose.context_->serialize_callbacks_.erase(it);
        }
    }
}

template <class typeT>
std::unique_ptr<runtime_process_context> runtime_holder_algorithms<typeT>::create_context (typename typeT::RType& whose, runtime::events::runtime_events_manager* events)
{
    return std::make_unique<runtime_process_context>(whose.tags_.binded_tags_, whose.tags_.connected_tags_, whose.meta_info_, &whose.directories_, whose.smart_this(), &whose.security_guards_, events);
}

template <class typeT>
runtime_init_context runtime_holder_algorithms<typeT>::create_init_context (typename typeT::RType& whose)
{
    return runtime::runtime_init_context(*whose.tags_.item_, whose.meta_info_, whose.context_.get(), &whose.tags_.binded_tags_, &whose.directories_, typeT::runtime_type_id);
}

template <class typeT>
runtime_start_context runtime_holder_algorithms<typeT>::create_start_context (typename typeT::RType& whose)
{
    whose.context_->job_queue_ = rx_internal::infrastructure::server_runtime::instance().get_executer(whose.instance_data_.get_executer());
    return runtime_start_context(*whose.tags_.item_, whose.context_.get(), &whose.tags_.binded_tags_
        , &whose.directories_, &whose.relations_, whose.context_->job_queue_);
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
        auto ret = whose.tags_.browse(prefix, path, filter, items, whose.context_.get());
        if (ret)
        {
            ret = whose.relations_.browse(prefix, filter, items);
            if (ret)
            {
                ret = whose.logic_.browse(prefix, path, filter, items, whose.context_.get());
                if (ret)
                {
                    ret = whose.displays_.browse(prefix, path, filter, items, whose.context_.get());
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
            auto result = whose.tags_.browse(prefix, path, filter, items, whose.context_.get());
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
            auto result = whose.logic_.browse(prefix, path, filter, items, whose.context_.get());
            callback(std::move(result), std::move(items));
        }
        else if (whose.displays_.is_this_yours(path))
        {
            std::vector<runtime_item_attribute> items;
            auto result = whose.displays_.browse(prefix, path, filter, items, whose.context_.get());
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
    whose.tags_.fill_data(data, whose.context_.get());
    whose.relations_.fill_data(data, whose.context_.get());
    whose.logic_.fill_data(data, whose.context_.get());
    whose.displays_.fill_data(data, whose.context_.get());
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
            result = whose.tags_.get_value(path, value, const_cast<runtime_process_context*>(whose.context_.get()));
        else if (whose.relations_.is_this_yours(path))
        {
            whose.relations_.read_value(path, std::move(callback), const_cast<runtime_process_context*>(whose.context_.get()));
            return;// relations will do callback!!!
        }
        else if (whose.logic_.is_this_yours(path))
            result = whose.logic_.get_value(path, value, const_cast<runtime_process_context*>(whose.context_.get()));
        else if (whose.displays_.is_this_yours(path))
            result = whose.displays_.get_value(path, value, const_cast<runtime_process_context*>(whose.context_.get()));
        else
            result = path + " not found!";

        callback(std::move(result), std::move(value));
    }
}

template <class typeT>
void runtime_holder_algorithms<typeT>::write_value (const string_type& path, bool test, data::runtime_values_data val, write_result_callback_t callback, typename typeT::RType& whose)
{
    auto connect_result = whose.tags_.connected_tags_.connect_tag(path, *whose.tags_.item_);
    if (!connect_result)
    {
        connect_result.register_error("Error connecting to item "s + path);
        callback(0, connect_result.errors());
        return;
    }
    auto transaction_ptr = rx_create_reference<write_item_transaction>(std::move(callback));
    auto result = whose.tags_.connected_tags_.write_tag(1, test, connect_result.value(), std::move(val), transaction_ptr);
    if (!result)
    {
        whose.tags_.connected_tags_.disconnect_tag(connect_result.value());
        (*transaction_ptr)(0, std::move(result));
        return;
    }
}

template <class typeT>
void runtime_holder_algorithms<typeT>::write_value (const string_type& path, bool test, rx_simple_value&& val, write_result_callback_t callback, typename typeT::RType& whose)
{
    auto connect_result = whose.tags_.connected_tags_.connect_tag(path, *whose.tags_.item_);
    if (!connect_result)
    {
        connect_result.register_error("Error connecting to item "s + path);
        callback(0, connect_result.errors());
        return;
    }
    auto transaction_ptr = rx_create_reference<write_item_transaction>(std::move(callback));
    auto result = whose.tags_.connected_tags_.write_tag(1, test, connect_result.value(), std::move(val), transaction_ptr);
    if (!result)
    {
        whose.tags_.connected_tags_.disconnect_tag(connect_result.value());
        (*transaction_ptr)(0, std::move(result));
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
        collected_data = whose.runtime_data_cache_;
        result = true;
    }
    else if (path == "Storage")
    {// our runtime value
        data.type = runtime_value_type::persistent_runtime_value;
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
            result = whose.tags_.get_struct_value(path, collected_data, data.type, const_cast<runtime_process_context*>(whose.context_.get()));
        }
        else if (whose.relations_.is_this_yours(item))
        {
            whose.relations_.read_struct(path, std::move(data));
            return;// relations will do callback!!!
        }
        else if (whose.logic_.is_this_yours(item))
        {
            result = whose.logic_.get_struct_value(item, path.substr(idx+1), collected_data, data.type, const_cast<runtime_process_context*>(whose.context_.get()));
        }
        else if (whose.displays_.is_this_yours(item))
        {
            result = whose.displays_.get_struct_value(item, path.substr(idx + 1), collected_data, data.type, const_cast<runtime_process_context*>(whose.context_.get()));
        }
        else
            result = string_type(path) + " not found!";
    }
    data.callback(std::move(result), std::move(collected_data));
}

template <class typeT>
void runtime_holder_algorithms<typeT>::write_struct (string_view_type path, bool test, write_struct_data data, typename typeT::RType& whose)
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

template <class typeT>
void runtime_holder_algorithms<typeT>::execute_method (const string_type& path, bool test, data::runtime_values_data data, named_execute_method_callback_t callback, typename typeT::RType& whose)
{
    auto connect_result = whose.tags_.connected_tags_.connect_tag(path, *whose.tags_.item_);
    if (!connect_result)
    {
        connect_result.register_error("Error executing method "s + path);
        callback(0, connect_result.errors(), data::runtime_values_data());
        return;
    }
    auto transaction_ptr = rx_create_reference<named_execute_method_transaction>(std::move(callback));
    auto result = whose.tags_.connected_tags_.execute_tag(1, test, connect_result.value(), std::move(data), transaction_ptr);
    if (!result)
    {
        whose.tags_.connected_tags_.disconnect_tag(connect_result.value());
        (*transaction_ptr)(0, std::move(result), data::runtime_values_data());
        return;
    }
}

template <class typeT>
void runtime_holder_algorithms<typeT>::execute_method (const string_type& path, bool test, values::rx_simple_value data, execute_method_callback_t callback, typename typeT::RType& whose)
{
    auto connect_result = whose.tags_.connected_tags_.connect_tag(path, *whose.tags_.item_);
    if (!connect_result)
    {
        connect_result.register_error("Error executing method "s + path);
        callback(0, connect_result.errors(), values::rx_simple_value());
        return;
    }
    auto transaction_ptr = rx_create_reference<execute_method_transaction>(std::move(callback));
    auto result = whose.tags_.connected_tags_.execute_tag(1, test, connect_result.value(), std::move(data), transaction_ptr);
    if (!result)
    {
        whose.tags_.connected_tags_.disconnect_tag(connect_result.value());
        (*transaction_ptr)(0, std::move(result), values::rx_simple_value());
        return;
    }
}

template <class typeT>
rx_result runtime_holder_algorithms<typeT>::execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, data::runtime_values_data data, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    auto result = whose.tags_.connected_tags_.execute_tag(transaction_id, test, handle, data, monitor);
    return result;
}

template <class typeT>
rx_result runtime_holder_algorithms<typeT>::execute_item (runtime_transaction_id_t transaction_id, bool test, runtime_handle_t handle, values::rx_simple_value data, runtime::tag_blocks::tags_callback_ptr monitor, typename typeT::RType& whose)
{
    auto result = whose.tags_.connected_tags_.execute_tag(transaction_id, test, handle, data, monitor);
    return result;
}

template <class typeT>
rx_result_with<runtime_handle_t> runtime_holder_algorithms<typeT>::connect_events (const event_filter& filter, events_callback_ptr monitor, bool bin_value, typename typeT::RType& whose)
{
    return RX_NOT_IMPLEMENTED;
}

template <class typeT>
rx_result runtime_holder_algorithms<typeT>::disconnect_events (runtime_handle_t hndl, events_callback_ptr monitor, typename typeT::RType& whose)
{
    return RX_NOT_IMPLEMENTED;
}

template <class typeT>
void runtime_holder_algorithms<typeT>::fill_access_guards (meta::construct_context& data, typename typeT::RType& whose, security::security_guard root)
{
	auto& access_guards = data.get_access_guards();
    if (!root.is_null())
        access_guards[""] = std::move(root);

	std::map<string_type, uint32_t> access_map;
	std::vector<security::security_guard> access_guards_vector;
    for(const auto& guard : access_guards)
    {
		access_map[guard.first] = (uint32_t)access_guards_vector.size();
        access_guards_vector.push_back(guard.second);
	}
	whose.tags_.connected_tags_.security_guards_map_ = std::move(access_map);
    whose.security_guards_ = const_size_vector< security::security_guard>(access_guards_vector);
    
    whose.context_->security_guards_ = &whose.security_guards_;
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
        ctx->tags_.relation_value_change(whose, local_val);
    }
    else
    {
        ctx->tags_.relation_value_change(whose, val);
    }
}

void runtime_relation_algorithms::read_value (const string_type& path, read_result_callback_t callback, const relations::relation_data& whose)
{
    rx_result result;
    if (path.empty())
    {// our value
        rx_value value = whose.get_value();
        callback(std::move(result), std::move(value));
    }
    else
    {
        values::rx_value value;
        result = path + " not found!";
        callback(std::move(result), std::move(value));
    }
}

void runtime_relation_algorithms::write_value (const string_type& path, rx_simple_value&& val, write_result_callback_t callback, relations::relation_data& whose)
{
    if (path.empty())
    {// our value
    }
    else
    {
        rx_result result(path + " not found!");
        callback(0, std::move(result));
    }
}

void runtime_relation_algorithms::read_struct (string_view_type path, read_struct_data data, const relations::relation_data& whose)
{
}

void runtime_relation_algorithms::write_struct (string_view_type path, write_struct_data data, relations::relation_data& whose)
{
}


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform

