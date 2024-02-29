

/****************************************************************************
*
*  model\rx_model_transactions.cpp
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


// rx_model_transactions
#include "model/rx_model_transactions.h"

#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "sys_internal/rx_inf.h"
#include "system/meta/rx_obj_types.h"
#include "system/server/rx_directory_cache.h"


namespace rx_internal {

namespace model {

namespace transactions {

// Class rx_internal::model::transactions::model_transaction_base 

model_transaction_base::model_transaction_base ()
{
}



// Parameterized Class rx_internal::model::transactions::delete_type_transaction 

template <class typeT>
delete_type_transaction<typeT>::delete_type_transaction (const meta_data& info)
      : meta_info_(info)
{
}



template <class typeT>
rx_result delete_type_transaction<typeT>::do_step (executer_phase state, rx_platform::rx_result_callback callback)
{
    return true;
}

template <class typeT>
rx_result delete_type_transaction<typeT>::do_step (executer_phase state)
{
    if (state == executer_phase::deleting_types)
    {
        auto result = platform_types_manager::instance().get_type_repository<typeT>().get_type_definition(meta_info_.id);
        if (!result)
            return result.errors();

        auto del_result =algorithms::types_model_algorithm<typeT>::delete_type_sync(meta_info_.id);
        if (!del_result)
            return del_result;

        type_ptr_ = result.move_value();
        meta_info_ = type_ptr_->meta_info;

    }
    return true;
}

template <class typeT>
void delete_type_transaction<typeT>::do_rollback (executer_phase state)
{
    if (state == executer_phase::deleting_types)
    {
        if (type_ptr_)
        {
            auto result = platform_types_manager::instance().get_type_repository<typeT>().register_type(type_ptr_);
            if (!result)
            {
                std::ostringstream ss;
                ss << "Error rolling-back type "
                    << type_ptr_->meta_info.get_full_path()
                    << " :"
                    << result.errors_line();
                META_LOG_ERROR("", 500, ss.str());
            }
        }
    }
}

template <class typeT>
meta_data& delete_type_transaction<typeT>::meta_info ()
{
    return meta_info_;
}

template <class typeT>
bool delete_type_transaction<typeT>::is_remove (executer_phase state) const
{
    if (state == executer_phase::idle || state == executer_phase::deleting_types)
        return true;
    else
        return false;
}

template <class typeT>
bool delete_type_transaction<typeT>::is_create (executer_phase state, rx_item_type type) const
{
    return false;
}

template class delete_type_transaction<object_type>;
template class delete_type_transaction<port_type>;
template class delete_type_transaction<domain_type>;
template class delete_type_transaction<application_type>;
// Class rx_internal::model::transactions::model_transactions_executer 

model_transactions_executer::model_transactions_executer (rx_platform::rx_result_callback callback)
      : callback_(std::move(callback)),
        state_(executer_phase::idle)
{
}



void model_transactions_executer::add_transaction (meta_transaction_ptr_t what)
{
    transactions_.emplace_back(what);
}

void model_transactions_executer::execute ()
{
    if (state_ != executer_phase::idle)
    {
        callback_(RX_INVALID_STATE);
        return;
    }
    // consolidate directories and meta data
    for (auto& one : transactions_)
    {
        if (one->is_remove(executer_phase::idle))
            dirs_to_delete_.emplace(one->meta_info().path);
        if (one->is_create(executer_phase::idle, rx_item_type::rx_invalid_type))
        {
            dirs_to_create_.emplace(one->meta_info().path);
            consolidate_meta_data(one->meta_info());
        }
    }
    state_ = executer_phase::deleting_objects;
    iterator_ = transactions_.begin();
    process(true);
}

void model_transactions_executer::process (rx_result result)
{
    if (!result)
    {
        state_ = executer_phase::done;
        callback_(std::move(result));
        return;
    }
    rx_result res = true;
    switch (state_)
    {
    case executer_phase::idle:
        res = RX_INTERNAL_ERROR;
        break;
    case executer_phase::deleting_objects:
    case executer_phase::deleting_ports:
    case executer_phase::deleting_domains:
    case executer_phase::deleting_apps:
        res = do_delete_runtimes(state_);
        break;
    case executer_phase::deleting_types:
        res = do_delete_types(state_);
        if (!res)
            break;
        state_ = (executer_phase)((int)(executer_phase)state_ + 1);
        [[fallthrough]];
    case executer_phase::deleting_directories:
        res = delete_directories();
        if (!res)
            break;
        state_ = (executer_phase)((int)(executer_phase)state_ + 1);
        [[fallthrough]];
    case executer_phase::building_directories:
        res = build_directories();
        if (!res)
            break;
        state_ = (executer_phase)((int)(executer_phase)state_ + 1);
        [[fallthrough]];
    case executer_phase::building_types:
        res = do_build_types(state_);
        if (!res)
            break;
        state_ = (executer_phase)((int)(executer_phase)state_ + 1);
        [[fallthrough]];
    case executer_phase::building_runtimes:
        res = do_build_runtimes(state_);
        if (!res)
            break;
        [[fallthrough]];
    case executer_phase::done:
        callback_(std::move(res));
        return;
    };
    if (!res)
    {
        state_ = executer_phase::done;
        callback_(std::move(res));
    }
}

rx_result model_transactions_executer::consolidate_meta_data (meta_data& new_data, const meta_data& old_data)
{
    new_data.id = old_data.id;
    if (old_data.created_time.is_valid_time())
        new_data.created_time = old_data.created_time;
    if (new_data.version < old_data.version)
    {
        new_data.version = old_data.version;
        new_data.increment_version(false);
    }
    new_data.attributes = new_data.attributes | old_data.attributes;
    return true;
}

rx_result model_transactions_executer::consolidate_meta_data (meta_data& data)
{
    if (data.id.is_null())
        data.id = rx_node_id::generate_new();
    if (data.attributes == 0)
        data.attributes = namespace_item_full_access;
    if (!data.modified_time.is_valid_time())
        data.modified_time = rx_time::now();
    if (data.version == 0)
        data.version = RX_INITIAL_ITEM_VERSION;
    return true;
}

rx_result model_transactions_executer::do_delete_runtimes (executer_phase state)
{
    
    bool waiting = false;
    while (iterator_ != transactions_.end())
    {
        if ((*iterator_)->is_remove(state))
        {
            auto trans = *iterator_;
            iterator_++;
            
            rx_result_callback callback(smart_this(), [this](rx_result&& res)
                {
                    process(std::move(res));
                });
            auto result = trans->do_step(state, std::move(callback));
            if (!result)
            {
                return result.errors();
            }
            else
            {
                waiting = true;
                break;
            }
        }
        else
        {
            iterator_++;
        }
    }
    if (!waiting)
    {
        state_ = (executer_phase)((int)(executer_phase)state_ + 1);
        process(true);
    }

    return true;
}

rx_result model_transactions_executer::do_delete_types (executer_phase state)
{
    rx_result ret = true;
    for (auto it = transactions_.rbegin(); it != transactions_.rend(); it++)
    {
        if ((*it)->is_remove(state))
        {
            auto result = (*it)->do_step(state);
            if (!result)
                return result.errors();
        }
    }
    return ret;
}

rx_result model_transactions_executer::delete_directories ()
{
    RX_ASSERT(state_ == executer_phase::deleting_directories);
    for (auto it = dirs_to_delete_.rbegin(); it != dirs_to_delete_.rend(); it++)
    {
        auto ret = ns::rx_directory_cache::instance().remove_directory(*it);
        if (!ret)
            return ret;
    }
    state_ = executer_phase::building_directories;
    return true;
}

rx_result model_transactions_executer::build_directories ()
{
    RX_ASSERT(state_ == executer_phase::building_directories);
    for (auto it = dirs_to_create_.begin(); it != dirs_to_create_.end(); it++)
    {
        auto ret = ns::rx_directory_cache::instance().get_or_create_directory(*it);
        if (!ret)
            return ret.errors();
    }
    state_ = executer_phase::building_types;
    return true;
}

rx_result model_transactions_executer::do_build_types (executer_phase state)
{

    auto result = do_build_types(state, rx_item_type::rx_data_type);
    if (!result)
        return result;

    result = do_build_types(state, rx_item_type::rx_filter_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_mapper_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_source_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_event_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_variable_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_struct_type);
    if (!result)
        return result;

    result = do_build_types(state, rx_item_type::rx_method_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_program_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_display_type);
    if (!result)
        return result;

    result = do_build_types(state, rx_item_type::rx_relation_type);
    if (!result)
        return result;


    result = do_build_types(state, rx_item_type::rx_object_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_port_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_domain_type);
    if (!result)
        return result;
    result = do_build_types(state, rx_item_type::rx_application_type);
    if (!result)
        return result;

    state_ = (executer_phase)((int)(executer_phase)state_ + 1);

    return true;
}

rx_result model_transactions_executer::do_build_types (executer_phase state, rx_item_type type)
{
    rx_result ret = true;
    std::vector<string_type> add_order;
    std::vector<std::pair<string_type, string_type> > local_to_add;
    std::map<string_type, meta_transaction_ptr_t> data;
    for (auto one : transactions_)
    {
        if (one->is_create(state, type))
        {
            data.emplace(one->meta_info().get_full_path(), one);
        }
    }
    for (const auto& one : data)
    {
        ns::rx_directory_resolver dirs;
        dirs.add_paths({ one.second->meta_info().path });
        auto parent_id = algorithms::resolve_reference(one.second->meta_info().parent, dirs);
        if (parent_id)
        {
            local_to_add.emplace_back(one.second->meta_info().get_full_path(), string_type());
        }
        else
        {
            auto it_local = data.find(one.second->meta_info().parent.to_string());
            if (it_local != data.end())
            {
                local_to_add.emplace_back(one.second->meta_info().get_full_path(), one.second->meta_info().parent.to_string());
            }
            else
            {//!!!
                local_to_add.emplace_back(one.second->meta_info().get_full_path(), string_type());
            }
        }
    }
    std::set<string_type> to_add;
    // first add all items to set for faster search
    for (const auto& one : local_to_add)
        to_add.insert(one.first);

    while (!to_add.empty())
    {
        // check for items not dependent on any items and add them next
        for (auto& one : local_to_add)
        {
            if (!one.first.empty())
            {
                auto it_help = to_add.find(one.second);
                if (it_help == to_add.end())
                {
                    add_order.push_back(one.first);
                    to_add.erase(one.first);
                    one.first.clear();
                }
            }
        }
    }
    for (auto& one : add_order)
    {
        auto it = data.find(one);
        RX_ASSERT(it != data.end());
        if (it != data.end())
        {
            auto result = it->second->do_step(state);
            if (!result)
                return result.errors();
        }
    }
    return ret;
}

rx_result model_transactions_executer::do_build_runtimes (executer_phase state)
{
    auto result = do_build_runtimes(state, rx_item_type::rx_object_type);
    if (!result)
        return result;
    result = do_build_runtimes(state, rx_item_type::rx_port_type);
    if (!result)
        return result;
    result = do_build_runtimes(state, rx_item_type::rx_domain_type);
    if (!result)
        return result;
    result = do_build_runtimes(state, rx_item_type::rx_application_type);
    if (!result)
        return result;

    state_ = (executer_phase)((int)(executer_phase)state_ + 1);

    return true;
}

rx_result model_transactions_executer::do_build_runtimes (executer_phase state, rx_item_type type)
{
    for (auto one : transactions_)
    {
        if (one->is_create(state, type))
        {
            auto result = one->do_step(state);
            if (!result)
                return result.errors();
        }
    }
    return true;
}


template<typename T>
void model_transactions_executer::do_consolidate_for_types(T& container)
{
    for (auto& one : container)
    {
        if (one.second.remove)
            dirs_to_delete_.emplace(one.second.item->meta_info.path);
        if (one.second.create)
        {
            dirs_to_create_.emplace(one.second.item->meta_info.path);
            consolidate_meta_data(one.second.item->meta_info);
        }
    }
}
} // namespace transactions
} // namespace model
} // namespace rx_internal

