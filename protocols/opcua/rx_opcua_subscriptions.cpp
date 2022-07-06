

/****************************************************************************
*
*  protocols\opcua\rx_opcua_subscriptions.cpp
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


// rx_opcua_subscriptions
#include "protocols/opcua/rx_opcua_subscriptions.h"

#include "system/threads/rx_job.h"
#include "sys_internal/rx_inf.h"
#include "sys_internal/rx_async_functions.h"
#include "rx_opcua_server.h"
using namespace protocols::opcua::requests::opcua_subscription;


namespace protocols {

namespace opcua {

namespace opcua_subscriptions {
constexpr const uint32_t const_opcua_min_subs_interval = 20;

// Class protocols::opcua::opcua_subscriptions::opcua_subscription 

opcua_subscription::opcua_subscription (uint32_t id, opcua_subscription_data& data, opcua_subscriptions_collection* container)
      : container_(container),
        subscription_id_(id),
        enabled_(false),
        state_(subscription_state::closed),
        next_seq_number_(1),
        current_keep_alive_(0)
    , interval_(data.interval)
    , lifetime_count_(data.lifetime_count)
    , max_keep_alive_(data.max_keep_alive)
    , max_notifications_(data.max_notifications)
    , priority_(data.priority)
{
    if (interval_ < const_opcua_min_subs_interval)
    {
        interval_ = const_opcua_min_subs_interval;
        data.interval = const_opcua_min_subs_interval;
    }
}



rx_result opcua_subscription::start ()
{
    locks::auto_lock_t _(&lock_);
    if (timer_)
        return RX_INVALID_STATE;

    current_keep_alive_ = 0;
    timer_ = rx_create_io_periodic_function(smart_this(), [this]()
        {
            timer_tick();
        });
    rx_internal::infrastructure::server_runtime::instance().append_timer_io_job(timer_);
    timer_->start(interval_);
    return true;
}

rx_result opcua_subscription::stop ()
{
    locks::auto_lock_t _(&lock_);
    if (!timer_)
        return RX_INVALID_STATE;
    timer_->cancel();
    timer_ = rx_timer_ptr::null_ptr;
    return true;
}

void opcua_subscription::set_enabled ()
{
    locks::auto_lock_t _(&lock_);
    enabled_ = true;
}

void opcua_subscription::set_disabled ()
{
    locks::auto_lock_t _(&lock_);
    enabled_ = false;
}

rx_result opcua_subscription::create_monitored_item (uint32_t server_handle, timestamps_return_type timestamps, const create_monitored_item_data& data, create_monitored_item_result& out, opcua_addr_space::opcua_address_space_base* addr_space)
{
    auto item_ptr = opcua_monitored_item::create_monitoring_item(server_handle, timestamps, data, out);
    if (item_ptr)
    {
        auto result = item_ptr->register_monitor(addr_space);
        if (result == opcid_OK)
        {
            {
                locks::auto_lock_t _(&lock_);
                items_.emplace(server_handle, std::move(item_ptr));
            }
            if (data.parameters.interval >= 0)
                out.interval = interval_;
            else
                out.interval = data.parameters.interval;
            out.queue_size = data.parameters.queue_size;
            out.status = opcid_OK;
        }
        else
        {
            out.status = result;
        }
        return true;
    }
    else
    {
        out.status = opcid_Bad_AttributeIdInvalid;
    }

    return true;
}

opcua_result_t opcua_subscription::delete_monitored_item (uint32_t server_handle, opcua_addr_space::opcua_address_space_base* addr_space)
{
    locks::auto_lock_t _(&lock_);
    auto it = items_.find(server_handle);
    if (it != items_.end())
    {
        auto result = it->second->unregister_monitor(addr_space);
        if (result == opcid_OK)
        {
            items_.erase(it);
        }
        return result;
    }
    else
    {
        return opcid_Bad_MonitoredItemIdInvalid;
    }
}

void opcua_subscription::clear_all_items (opcua_addr_space::opcua_address_space_base* addr_space)
{
    locks::auto_lock_t _(&lock_);
    for (auto& one : items_)
    {
        one.second->unregister_monitor(addr_space);
    }
    items_.clear();
}

void opcua_subscription::timer_tick ()
{
    std::vector<monitored_item_notification> notifications;
    for (auto& item : items_)
        item.second->timer_tick(notifications);
    if (!notifications.empty())
    {
        current_keep_alive_ = 0;
        auto req_ptr = container_->dequeue_publish_request();
        if (req_ptr)
        {
            auto resp_ptr = std::make_unique<opcua_publish_response>(*req_ptr);
            resp_ptr->subscription_id = subscription_id_;
            resp_ptr->more_notifications = false;
            if (!req_ptr->subscription_acks.empty())
            {
                resp_ptr->results.assign(req_ptr->subscription_acks.size(), opcid_Good);
            }
            resp_ptr->sequence_number = next_seq_number_++;
            if (resp_ptr->sequence_number == 0)// rollover
                resp_ptr->sequence_number = next_seq_number_++;
            resp_ptr->publish_time = rx_time::now();
            auto notif_ptr = std::make_unique<opcua_data_change_notification>();
            notif_ptr->monitored_items = std::move(notifications);
            resp_ptr->notification = std::move(notif_ptr);
            req_ptr->endpoint->send_response(std::move(resp_ptr));
        }
    }
    else
    {
        current_keep_alive_++;
        if (current_keep_alive_ >= max_keep_alive_)
        {
            auto req_ptr = container_->dequeue_publish_request();
            if (req_ptr)
            {

                auto resp_ptr = std::make_unique<opcua_publish_response>(*req_ptr);
                resp_ptr->subscription_id = subscription_id_;
                resp_ptr->more_notifications = false;
                if (!req_ptr->subscription_acks.empty())
                {
                    resp_ptr->results.assign(req_ptr->subscription_acks.size(), opcid_Good);
                }
                resp_ptr->sequence_number = next_seq_number_++;
                if (resp_ptr->sequence_number == 0)// rollover
                    resp_ptr->sequence_number = next_seq_number_++;
                resp_ptr->publish_time = rx_time::now();
                auto notif_ptr = std::make_unique<opcua_data_change_notification>();
                //notif_ptr->monitored_items = std::move(notifications);
                resp_ptr->notification = std::move(notif_ptr);
                req_ptr->endpoint->send_response(std::move(resp_ptr));
            }
        }
    }
}


// Class protocols::opcua::opcua_subscriptions::opcua_monitored_item 

opcua_monitored_item::opcua_monitored_item (uint32_t handle, timestamps_return_type timestamps, const create_monitored_item_data& data)
    : server_handle(handle)
    , node_id_(data.to_monitor.node_id)
    , client_handle(data.parameters.client_handle)
    , mode(data.mode)
    , interval(data.parameters.interval < 0 ? 0 : (uint32_t)data.parameters.interval)
    , queue_size(data.parameters.queue_size)
    , discard_oldest(data.parameters.discard_oldest)
    , filter_ptr(data.parameters.filter_ptr ? data.parameters.filter_ptr->make_filter_copy() : monitoring_filter_ptr())
{
}



const rx_node_id& opcua_monitored_item::get_node_id () const
{
    return node_id_;
}

std::unique_ptr<opcua_monitored_item> opcua_monitored_item::create_monitoring_item (uint32_t server_handle, timestamps_return_type timestamps, const create_monitored_item_data& data, create_monitored_item_result& out)
{
    switch (data.to_monitor.attr_id)
    {
    case attribute_id::value:
        {// variable monitor item
            auto result = std::make_unique<opcua_monitored_value>(server_handle, timestamps, data);
            return result;
        }
        break;
    case attribute_id::event_notifier:
        {// event monitor item

        }
        break;
    default:
        {// attribute monitor item

        }
        break;
    }
    return std::unique_ptr<opcua_monitored_item>();
}


// Class protocols::opcua::opcua_subscriptions::opcua_subscriptions_collection 

uint32_t opcua_subscriptions_collection::g_next_id_ = 1;

opcua_subscriptions_collection::opcua_subscriptions_collection (opcua_addr_space::opcua_address_space_base* as)
    : address_space_(as)
{
}



uint32_t opcua_subscriptions_collection::create_subscription (opcua_subscription_data& data, bool active)
{
    opcua_subscription::smart_ptr new_subs;
    uint32_t new_id = 0;
    {
        locks::auto_lock_t _(&lock_);
        new_id = g_next_id_++;
        if (new_id == 0)
            new_id = g_next_id_++;
        new_subs = rx_create_reference<opcua_subscription>(new_id, data, this);
        auto result = new_subs->start();
        if (result)
        {
            subscriptions_.emplace(new_id, new_subs);
        }
        else
        {
            new_id = 0;
        }
    }
    if (active)
    {
        new_subs->set_enabled();
    }
    return new_id;
}

opcua_result_t opcua_subscriptions_collection::delete_subscription (uint32_t id)
{
    locks::auto_lock_t _(&lock_);
    auto it = subscriptions_.find(id);
    if (it == subscriptions_.end())
        return opcid_Bad_SubscriptionIdInvalid;
    it->second->stop();
    it->second->clear_all_items(address_space_);
    subscriptions_.erase(it);
    return opcid_OK;
}

create_monitored_item_result opcua_subscriptions_collection::create_monitored_item (uint32_t id, timestamps_return_type timestamps, create_monitored_item_data& data)
{
    create_monitored_item_result ret;
    locks::auto_lock_t _(&lock_);
    auto it = subscriptions_.find(id);
    if (it == subscriptions_.end())
    {
        ret.status = opcid_Bad_SubscriptionIdInvalid;
    }
    else
    {   
        auto new_id = g_next_id_++;
        if (new_id == 0)
            new_id = g_next_id_++;
        ret.server_handle = new_id;
        ret.status = opcid_OK;
        it->second->create_monitored_item(new_id, timestamps, data, ret, address_space_);
    }
    return ret;
}

opcua_result_t opcua_subscriptions_collection::queue_publish_request (publish_request_ptr req)
{
    locks::auto_lock_t _(&lock_);
    publish_requests_.push(std::move(req));
    return opcid_OK;
}

publish_request_ptr opcua_subscriptions_collection::dequeue_publish_request ()
{
    publish_request_ptr ret_ptr;
    locks::auto_lock_t _(&lock_);
    if (!publish_requests_.empty())
    {
        ret_ptr = std::move(publish_requests_.front());
        publish_requests_.pop();
    }
    return ret_ptr;
}

opcua_response_ptr opcua_subscriptions_collection::republish_request (republish_request_ptr req)
{
    locks::auto_lock_t _(&lock_);
    auto it = subscriptions_.find(req->subscription_id);
    if (it != subscriptions_.end())
    {// return not supported for now
        return std::make_unique<requests::opcua_service_fault>(*req, opcid_Bad_MessageNotAvailable);
    }
    else
    {// invalid subscription id
        return std::make_unique<requests::opcua_service_fault>(*req, opcid_Bad_SubscriptionIdInvalid);
    }
}

opcua_result_t opcua_subscriptions_collection::delete_monitored_item (uint32_t subscription_id, uint32_t server_handle)
{
    locks::auto_lock_t _(&lock_);
    auto it = subscriptions_.find(subscription_id);
    if (it != subscriptions_.end())
    {// return not supported for now
        return it->second->delete_monitored_item(server_handle, address_space_);
    }
    else
    {// invalid subscription id
        return opcid_Bad_SubscriptionIdInvalid;
    }
}


// Class protocols::opcua::opcua_subscriptions::opcua_monitored_value 

opcua_monitored_value::opcua_monitored_value (uint32_t handle, timestamps_return_type timestamps, const create_monitored_item_data& data)
    : opcua_monitored_item(handle, timestamps, data)
{
}



void opcua_monitored_value::timer_tick (std::vector<requests::opcua_subscription::monitored_item_notification>& notifications)
{
    if (!queue_.empty())
    {
        for(auto& one : queue_)
        {
            notifications.emplace_back(monitored_item_notification{ client_handle, std::move(one) });
        }
        queue_.clear();
    }
}

opcua_result_t opcua_monitored_value::register_monitor (opcua_addr_space::opcua_address_space_base* addr_space)
{
    data_value val;
    opcua_result_t result = addr_space->register_value_monitor(this, val);
    if (result == opcid_OK)
    {
        queue_.push_back(std::move(val));
    }
    return result;
}

opcua_result_t opcua_monitored_value::unregister_monitor (opcua_addr_space::opcua_address_space_base* addr_space)
{
    opcua_result_t result = addr_space->unregister_value_monitor(this);
    RX_ASSERT(result == opcid_OK);// has to be or something is really wrong
    return result;
}

void opcua_monitored_value::monitored_value_changed (data_value val)
{
    if (val.status_code != last_value_.status_code || val.value != last_value_.value)
    {
        queue_.push_back(std::move(val));
    }
}


} // namespace opcua_subscriptions
} // namespace opcua
} // namespace protocols

