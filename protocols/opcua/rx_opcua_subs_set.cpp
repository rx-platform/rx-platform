

/****************************************************************************
*
*  protocols\opcua\rx_opcua_subs_set.cpp
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


// rx_opcua_subs_set
#include "protocols/opcua/rx_opcua_subs_set.h"

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;

#include "rx_opcua_server.h"
#include "rx_opcua_client.h"


namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_subscription {

// Class protocols::opcua::requests::opcua_subscription::opcua_create_subs_request 

opcua_create_subs_request::opcua_create_subs_request (uint32_t req_id, uint32_t req_handle)
      : publish_interval(0),
        lifetime_count(0),
        keep_alive_count(0),
        max_notifications(0),
        enabled(false),
        priority(0)
    , opcua_request_base(req_id, req_handle)
{
}



rx_node_id opcua_create_subs_request::get_binary_request_id ()
{
    return rx_node_id::opcua_standard_id(opcid_CreateSubscriptionRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_create_subs_request::create_empty () const
{
    return std::make_unique<opcua_create_subs_request>();
}

rx_result opcua_create_subs_request::deserialize_binary (binary::ua_binary_istream& stream)
{
    stream >> publish_interval;
    stream >> lifetime_count;
    stream >> keep_alive_count;
    stream >> max_notifications;
    stream >> enabled;
    stream >> priority;
    return true;
}

opcua_response_ptr opcua_create_subs_request::do_job (opcua_server_endpoint_ptr ep)
{
    opcua_subscriptions::opcua_subscription_data data;
    data.interval = (uint32_t)publish_interval;
    data.lifetime_count = lifetime_count;
    data.max_keep_alive = keep_alive_count;
    data.max_notifications = max_notifications;
    data.priority = priority;

    auto subs_obj = ep->get_subscriptions();
    uint32_t ret_val = subs_obj->create_subscription(data, enabled);
    if (ret_val)
    {
        auto ret_ptr = std::make_unique<opcua_create_subs_response>(*this);
        ret_ptr->subscription_id = ret_val;
        ret_ptr->publish_interval = (double)data.interval;
        ret_ptr->lifetime_count = data.lifetime_count;
        ret_ptr->keep_alive_count = data.max_keep_alive;

        return ret_ptr;
    }
    else
    {
        auto ret_ptr = std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_InvalidArgument);
        return ret_ptr;
    }
}

rx_result opcua_create_subs_request::serialize_binary (binary::ua_binary_ostream& stream)
{
    stream << publish_interval;
    stream << lifetime_count;
    stream << keep_alive_count;
    stream << max_notifications;
    stream << enabled;
    stream << priority;
    return true;
}


// Class protocols::opcua::requests::opcua_subscription::opcua_create_subs_response 

opcua_create_subs_response::opcua_create_subs_response (const opcua_request_base& req)
      : subscription_id(0),
        publish_interval(0),
        lifetime_count(0),
        keep_alive_count(0)
    , opcua_response_base(req)
{
}



rx_node_id opcua_create_subs_response::get_binary_response_id ()
{
    return rx_node_id::opcua_standard_id(opcid_CreateSubscriptionResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_create_subs_response::create_empty () const
{
    return std::make_unique<opcua_create_subs_response>();
}

rx_result opcua_create_subs_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
    stream << subscription_id;
    stream << publish_interval;
    stream << lifetime_count;
    stream << keep_alive_count;
    return true;
}

rx_result opcua_create_subs_response::deserialize_binary (binary::ua_binary_istream& stream)
{
    stream >> subscription_id;
    stream >> publish_interval;
    stream >> lifetime_count;
    stream >> keep_alive_count;
    return true;
}

rx_result opcua_create_subs_response::process_response (opcua_client_endpoint_ptr ep)
{
    ep->transactions_lock.lock();
    auto it = ep->subscriptions.pending.find(request_handle);
    if (it != ep->subscriptions.pending.end())
    {
        ep->subscriptions.pending.erase(it);

        active_client_subscription_data data;
        data.interval = (uint32_t)publish_interval;
        ep->subscriptions.active.emplace(subscription_id, std::move(data));

        ep->transactions_lock.unlock();

        uint32_t req_id = ep->current_request_id++;
        auto req = std::make_unique<opcua_publish_request>(req_id, req_id);


        auto result = ep->send_request(std::move(req));
        if (!result)
        {
            std::ostringstream ss;
            ss << "Error sending Publish Request";
            ss << result.errors_line();
            return ss.str();
        }
        else
        {
            return ep->subscription_created(subscription_id);
        }
    }
    else
    {
        ep->transactions_lock.unlock();
        return "Unknown response to Create Subscription Request!";
    }
}


// Class protocols::opcua::requests::opcua_subscription::opcua_delete_subs_request 


rx_node_id opcua_delete_subs_request::get_binary_request_id ()
{
    return rx_node_id::opcua_standard_id(opcid_DeleteSubscriptionsRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_delete_subs_request::create_empty () const
{
    return std::make_unique<opcua_delete_subs_request>();
}

rx_result opcua_delete_subs_request::deserialize_binary (binary::ua_binary_istream& stream)
{
    stream >> subscription_ids;
    return true;
}

opcua_response_ptr opcua_delete_subs_request::do_job (opcua_server_endpoint_ptr ep)
{
    if(subscription_ids.empty())
        return std::make_unique<requests::opcua_service_fault>(*this, opcid_Bad_NothingToDo);
    auto subs_obj = ep->get_subscriptions();

    auto ret_ptr = std::make_unique<opcua_delete_subs_response>(*this);
    ret_ptr->results.reserve(subscription_ids.size());
    for (auto id : subscription_ids)
    {
        ret_ptr->results.push_back(subs_obj->delete_subscription(id));
    }
    return ret_ptr;
}


// Class protocols::opcua::requests::opcua_subscription::opcua_delete_subs_response 

opcua_delete_subs_response::opcua_delete_subs_response (const opcua_request_base& req)
    : opcua_response_base(req)
{
}



rx_node_id opcua_delete_subs_response::get_binary_response_id ()
{
    return rx_node_id::opcua_standard_id(opcid_DeleteSubscriptionsResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_delete_subs_response::create_empty () const
{
    return std::make_unique<opcua_delete_subs_response>();
}

rx_result opcua_delete_subs_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
    stream << results;
    stream << diagnostics_info;
    return true;
}


// Class protocols::opcua::requests::opcua_subscription::opcua_publish_request 

opcua_publish_request::opcua_publish_request (uint32_t req_id, uint32_t req_handle)
    : opcua_request_base(req_id, req_handle)
{
}



rx_node_id opcua_publish_request::get_binary_request_id ()
{
    return rx_node_id::opcua_standard_id(opcid_PublishRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_publish_request::create_empty () const
{
    return std::make_unique<opcua_publish_request>();
}

rx_result opcua_publish_request::deserialize_binary (binary::ua_binary_istream& stream)
{
    stream.deserialize_array(subscription_acks);
    return true;
}

opcua_response_ptr opcua_publish_request::do_job (opcua_server_endpoint_ptr ep)
{
    auto subs_obj = ep->get_subscriptions();

    auto req = std::make_unique<opcua_publish_request>();
    move_header_to(req.get());
    req->subscription_acks = std::move(subscription_acks);
    req->endpoint = ep;
    subs_obj->queue_publish_request(std::move(req));

    return opcua_response_ptr();
}

rx_result opcua_publish_request::serialize_binary (binary::ua_binary_ostream& stream)
{
    stream.serialize_array(subscription_acks);
    return true;
}


void subscription_ack::serialize(binary::ua_binary_ostream& stream) const
{
    stream << subscription_id;
    stream << sequence_number;
}
void subscription_ack::deserialize(binary::ua_binary_istream& stream)
{
    stream >> subscription_id;
    stream >> sequence_number;
}
// Class protocols::opcua::requests::opcua_subscription::opcua_publish_response 

opcua_publish_response::opcua_publish_response (const opcua_request_base& req)
    : opcua_response_base(req)
{
}



rx_node_id opcua_publish_response::get_binary_response_id ()
{
    return rx_node_id::opcua_standard_id(opcid_PublishResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_publish_response::create_empty () const
{
    return std::make_unique<opcua_publish_response>();
}

rx_result opcua_publish_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
    stream << subscription_id;
    stream << sequence_numbers;
    stream << more_notifications;
    stream << sequence_number;
    stream << publish_time;
    if (notification)
    {
        stream << 1u;
        stream.serialize_extension(notification.get());
    }
    else
    {
        stream << -1;
    }
    stream << results;
    stream << diagnostics_info;
    return true;
}

rx_result opcua_publish_response::deserialize_binary (binary::ua_binary_istream& stream)
{
    stream >> subscription_id;
    stream >> sequence_numbers;
    stream >> more_notifications;
    stream >> sequence_number;
    stream >> publish_time;
    int32_t count = 0;
    stream >> count;
    if (count>0)
    {
        RX_ASSERT(count == 1);

        notification = stream.deserialize_extension<opcua_notification_data>([](const rx_node_id& id) -> notification_data_ptr {
            static rx_node_id data_id = rx_node_id::opcua_standard_id(opcid_DataChangeNotification_Encoding_DefaultBinary);
            if (id == data_id)
                return std::make_unique<opcua_data_change_notification>();
            else
                return notification_data_ptr();
            });
    }
    stream >> results;
    stream >> diagnostics_info;
    return true;
}

rx_result opcua_publish_response::process_response (opcua_client_endpoint_ptr ep)
{
    std::vector<subscription_ack> acks;
    {
        locks::auto_lock_t _(&ep->transactions_lock);
        for (auto& one : ep->subscriptions.active)
        {
            if (one.first == subscription_id)
            {
                one.second.sequence_number = sequence_number;
                opcua_data_change_notification* data_changes = nullptr;
                if (notification)
                {

                    if (one.second.sequence_number)
                    {
                        subscription_ack ack;
                        ack.subscription_id = one.first;
                        ack.sequence_number = one.second.sequence_number;
                        acks.push_back(std::move(ack));
                    }

                    if (notification->binary_id == rx_node_id::opcua_standard_id(opcid_DataChangeNotification_Encoding_DefaultBinary))
                    {
                        // o.k. this reinterpret cast is, well have to do it this way, id is already checked
                        data_changes = reinterpret_cast<opcua_data_change_notification*>(notification.get());
                    }
                }
                ep->subscription_notification(data_changes);
            }
            
        }
    }
    uint32_t req_id = ep->current_request_id++;
    auto req = std::make_unique<opcua_publish_request>(req_id, req_id);
    req->subscription_acks = std::move(acks);

    auto result = ep->send_request(std::move(req));
    if (!result)
    {
        std::ostringstream ss;
        ss << "Error sending Publish Request";
        ss << result.errors_line();
        return ss.str();
    }
    else
    {
        return true;
    }
}


opcua_notification_data::opcua_notification_data(rx_node_id class_id, rx_node_id binary_id, rx_node_id xml_id)
    : common::ua_extension(class_id, binary_id, xml_id)
{
}

void monitored_item_notification::serialize(binary::ua_binary_ostream& stream) const
{
    stream << client_handle;
    stream << value;
}
void monitored_item_notification::deserialize(binary::ua_binary_istream& stream)
{
    stream >> client_handle;
    stream >> value;
}

opcua_data_change_notification::opcua_data_change_notification()
    : opcua_notification_data(rx_node_id::opcua_standard_id(opcid_DataChangeNotification)
            , rx_node_id::opcua_standard_id(opcid_DataChangeNotification_Encoding_DefaultBinary)
            , rx_node_id::opcua_standard_id(opcid_DataChangeNotification_Encoding_DefaultXml))
{
}

void opcua_data_change_notification::internal_serialize_extension(binary::ua_binary_ostream& stream) const
{
    stream.serialize_array(monitored_items);
    stream << diagnostics_info;
}
void opcua_data_change_notification::internal_deserialize_extension(binary::ua_binary_istream& stream)
{
    stream.deserialize_array(monitored_items);
    stream >> diagnostics_info;
}
// Class protocols::opcua::requests::opcua_subscription::opcua_republish_request 


rx_node_id opcua_republish_request::get_binary_request_id ()
{
    return rx_node_id::opcua_standard_id(opcid_RepublishRequest_Encoding_DefaultBinary);
}

opcua_request_ptr opcua_republish_request::create_empty () const
{
    return std::make_unique<opcua_republish_request>();
}

rx_result opcua_republish_request::deserialize_binary (binary::ua_binary_istream& stream)
{
    stream >> subscription_id;
    stream >> sequence_number;
    return true;
}

opcua_response_ptr opcua_republish_request::do_job (opcua_server_endpoint_ptr ep)
{
    auto subs_obj = ep->get_subscriptions();

    auto req = std::make_unique<opcua_republish_request>();
    move_header_to(req.get());
    req->subscription_id = subscription_id;
    req->sequence_number = sequence_number;
    opcua_response_ptr ret = subs_obj->republish_request(std::move(req));

    return ret;
}


// Class protocols::opcua::requests::opcua_subscription::opcua_republish_response 

opcua_republish_response::opcua_republish_response (const opcua_request_base& req)
    : opcua_response_base(req)
{
}



rx_node_id opcua_republish_response::get_binary_response_id ()
{
    return rx_node_id::opcua_standard_id(opcid_RepublishResponse_Encoding_DefaultBinary);
}

opcua_response_ptr opcua_republish_response::create_empty () const
{
    return std::make_unique<opcua_republish_response>();
}

rx_result opcua_republish_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
    stream << sequence_number;
    stream << publish_time;
    if (notification)
    {
        stream << 1u;
        stream.serialize_extension(notification.get());
    }
    else
    {
        stream << -1;
    }
    return true;
}


} // namespace opcua_subscription
} // namespace requests
} // namespace opcua
} // namespace protocols

