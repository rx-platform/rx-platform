

/****************************************************************************
*
*  runtime_internal\rx_relations_runtime.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


// rx_relations_runtime
#include "runtime_internal/rx_relations_runtime.h"

#include "system/server/rx_platform_item.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "model/rx_model_algorithms.h"
using namespace rx_platform::runtime;


namespace rx_internal {

namespace sys_runtime {

namespace relations_runtime {

// Class rx_internal::sys_runtime::relations_runtime::local_relation_connector 

local_relation_connector::local_relation_connector (platform_item_ptr&& item, relations::relation_connections* whose)
      : item_ptr_(std::move(item)),
        parent_(whose)
{
    monitor_ = rx_create_reference<local_relation_callback>(this);
}


local_relation_connector::~local_relation_connector()
{
    monitor_->parent_destroyed();
}



std::vector<rx_result_with<runtime_handle_t> > local_relation_connector::connect_items (const string_array& paths)
{
    auto result = item_ptr_->connect_items(paths, monitor_);
    return result;
}

rx_result local_relation_connector::disconnect_items (const std::vector<runtime_handle_t>& items)
{
    item_ptr_->disconnect_items(items, monitor_);
    return true;
}

rx_result local_relation_connector::write_tag (runtime_transaction_id_t trans, runtime_handle_t item, rx_simple_value&& value)
{
    std::vector<std::pair<runtime_handle_t, rx_simple_value> > args;
    args.emplace_back(item, std::move(value));
    return item_ptr_->write_items(trans, std::move(args), monitor_);
}

rx_result local_relation_connector::execute_tag (runtime_transaction_id_t trans, runtime_handle_t item, data::runtime_values_data&& value)
{
    return item_ptr_->execute_item(trans, item, value, monitor_);
}

void local_relation_connector::browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback)
{
   item_ptr_->browse(prefix, path, filter, std::move(callback));
}

void local_relation_connector::read_value (const string_type& path, read_result_callback_t callback) const
{
    item_ptr_->read_value(path, std::move(callback));
}

void local_relation_connector::read_struct (string_view_type path, read_struct_data data) const
{
    item_ptr_->read_struct(path, std::move(data));
}

void local_relation_connector::items_changed (const std::vector<update_item>& items)
{
    parent_->items_changed(items);
}

void local_relation_connector::execute_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result result, data::runtime_values_data data)
{
    parent_->execute_complete(transaction_id, item, std::move(result), std::move(data));
}

void local_relation_connector::write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result)
{
    parent_->write_complete(transaction_id, item, std::move(result));
}


// Class rx_internal::sys_runtime::relations_runtime::remote_relation_connector 

remote_relation_connector::remote_relation_connector (platform_item_ptr&& item, relations::relation_connections* whose)
      : item_ptr_(std::move(item)),
        parent_(whose)
{
    item_path_ = item_ptr_->meta_info().get_full_path();
}


remote_relation_connector::~remote_relation_connector()
{
    for (auto& one : values_)
        one.second->disconnect();
}



std::vector<rx_result_with<runtime_handle_t> > remote_relation_connector::connect_items (const string_array& paths)
{
    std::vector<rx_result_with<runtime_handle_t> > ret_values;
    if (!paths.empty())
    {
        string_type path_buffer;
        path_buffer.reserve(item_path_.size() * 2);
        string_array to_connect;
        size_t count = paths.size();
        to_connect.reserve(count);
        ret_values.reserve(count);
        for (size_t i = 0; i < count; i++)
        {
            auto my_handle = rx_internal::sys_runtime::platform_runtime_manager::get_new_handle();
            auto vp_smart_ptr = std::make_unique<relation_value_point>(this, my_handle);
            auto vp_ptr = vp_smart_ptr.get();
            path_buffer = "{";
            path_buffer += item_path_;
            path_buffer += ".";
            path_buffer += paths[i];
            path_buffer += "}";
            values_.emplace(my_handle, std::move(vp_smart_ptr));
            vp_ptr->connect(path_buffer, 200);
            ret_values.emplace_back(my_handle);
        }
    }
    return ret_values;
}

rx_result remote_relation_connector::disconnect_items (const std::vector<runtime_handle_t>& items)
{
    for (const auto& one : items)
    {
        auto it = values_.find(one);
        if (it!=values_.end())
        {
            it->second->disconnect();
            values_.erase(it);
        }
    }
    return true;
}

rx_result remote_relation_connector::write_tag (runtime_transaction_id_t trans, runtime_handle_t item, rx_simple_value&& value)
{
    auto it = values_.find(item);
    if (it != values_.end())
    {
        it->second->write(std::move(value), trans);
        return true;
    }
    else
    {
        return RX_INVALID_ARGUMENT;
    }
}

rx_result remote_relation_connector::execute_tag (runtime_transaction_id_t trans, runtime_handle_t item, data::runtime_values_data&& value)
{
    auto it = values_.find(item);
    if (it != values_.end())
    {
        it->second->execute(std::move(value), trans);
        return true;
    }
    else
    {
        return RX_INVALID_ARGUMENT;
    }
}

void remote_relation_connector::browse (const string_type& prefix, const string_type& path, const string_type& filter, browse_result_callback_t callback)
{
    auto target = item_ptr_->get_executer();
    rx_node_id id = item_ptr_->meta_info().id;

    rx_post_function_to(target, callback.get_anchor()
        , [](rx_thread_handle_t should_be_target, rx_node_id id, string_type prefix, string_type path, string_type filter, browse_result_callback_t callback)
        {
            static std::vector<runtime_item_attribute> g_empty_result;

            auto item = sys_runtime::platform_runtime_manager::instance().get_cache().get_item(id);
            if (!item)
            {
                callback(id.to_string() + " is not the registered id!", g_empty_result);
            }
            else
            {
                if(item->get_executer()!= should_be_target)
                    callback(id.to_string() + " is recreated, wrong working thread!", g_empty_result);
                else
                    item->browse(prefix, path, filter, std::move(callback));
            }

        }, target, std::move(id), string_type(prefix), string_type(path), string_type(filter), std::move(callback));

}

void remote_relation_connector::read_value (const string_type& path, read_result_callback_t callback) const
{
    auto target = item_ptr_->get_executer();
    rx_node_id id = item_ptr_->meta_info().id;

    rx_post_function_to(target, callback.get_anchor()
        , [](rx_thread_handle_t should_be_target, rx_node_id id, string_type path, read_result_callback_t callback)
        {
            auto item = sys_runtime::platform_runtime_manager::instance().get_cache().get_item(id);
            if (!item)
            {
                callback(id.to_string() + " is not the registered id!", rx_value());
            }
            else
            {
                if (item->get_executer() != should_be_target)
                    callback(id.to_string() + " is recreated, wrong working thread!", rx_value());
                else
                    item->read_value(path, std::move(callback));
            }

        }, target, std::move(id), string_type(path), std::move(callback));

}

void remote_relation_connector::read_struct (string_view_type path, read_struct_data data) const
{
    auto target = item_ptr_->get_executer();
    rx_node_id id = item_ptr_->meta_info().id;

    auto anchor = data.callback.get_anchor();

    rx_post_function_to(target, anchor
        , [](rx_thread_handle_t should_be_target, rx_node_id id, string_type path, read_struct_data data)
        {
            auto item = sys_runtime::platform_runtime_manager::instance().get_cache().get_item(id);
            if (!item)
            {
                data.callback(id.to_string() + " is not the registered id!", data::runtime_values_data());
            }
            else
            {
                if (item->get_executer() != should_be_target)
                    data.callback(id.to_string() + " is recreated, wrong working thread!", data::runtime_values_data());
                else
                    item->read_struct(path, std::move(data));
            }

        }, target, std::move(id), string_type(path), std::move(data));

}

void remote_relation_connector::value_changed (runtime_handle_t handle, const rx_value& val)
{
    std::vector<update_item> items;
    items.emplace_back(update_item{ handle, val });
    parent_->items_changed(items);
}

void remote_relation_connector::result_received (runtime_handle_t handle, rx_result&& result, runtime_transaction_id_t id)
{
    parent_->write_complete(id, handle, std::move(result));
}

void remote_relation_connector::execute_result_received (runtime_handle_t handle, rx_result&& result, const data::runtime_values_data& data, runtime_transaction_id_t id)
{
    parent_->execute_complete(id, handle, std::move(result), data);
}


// Class rx_internal::sys_runtime::relations_runtime::relation_value_point 

relation_value_point::relation_value_point (remote_relation_connector* connector, runtime_handle_t handle)
      : connector_(connector),
        handle_(handle)
{
}



void relation_value_point::value_changed (const rx_value& val)
{
    connector_->value_changed(handle_, val);
}

void relation_value_point::result_received (rx_result&& result, runtime_transaction_id_t id)
{
    connector_->result_received(handle_, std::move(result), id);
}

void relation_value_point::execute_result_received (rx_result&& result, const data::runtime_values_data& data, runtime_transaction_id_t id)
{
    connector_->execute_result_received(handle_, std::move(result), data, id);
}


} // namespace relations_runtime
} // namespace sys_runtime
} // namespace rx_internal

