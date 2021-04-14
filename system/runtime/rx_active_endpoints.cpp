

/****************************************************************************
*
*  system\runtime\rx_active_endpoints.cpp
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

#include "lib/rx_io_addr.h"
#include "rx_runtime_helpers.h"
using namespace rx_platform;

// simple_slave_endpoint
#include "system/runtime/simple_slave_endpoint.h"
// address_endpoint
#include "system/runtime/address_endpoint.h"
// simple_master_endpoint
#include "system/runtime/simple_master_endpoint.h"
// rx_active_endpoints
#include "system/runtime/rx_active_endpoints.h"

#include "rx_runtime_holder.h"
#include "interfaces/rx_endpoints.h"


namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {

// Parameterized Class rx_platform::runtime::io_types::ports_templates::routing_endpoint 

template <typename translatorT, typename addrT>
routing_endpoint<translatorT,addrT>::routing_endpoint (runtime::items::port_runtime* port)
      : port_(port),
        connected_(false)
{
    std::ostringstream ss;
    ss << "Routing endpoint at "
        << port_->runtime_->meta_info().get_full_path()
        << " created.";
    ITF_LOG_DEBUG("routing_endpoint", 200, ss.str());
    rx_init_stack_entry(&stack, this);
    this->stack.received_function = [](rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
    {
        routing_endpoint* me = (routing_endpoint*)reference->user_data;
        return me->recv_packet(&packet);
    };
    this->stack.connected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session)
    {
        routing_endpoint* me = (routing_endpoint*)reference->user_data;
        return me->connected(session);
    };
    this->stack.disconnected_function = [](rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
    {
        routing_endpoint* me = (routing_endpoint*)reference->user_data;
        return me->disconnected(session, reason);
    };
    this->stack.closed_function = [](rx_protocol_stack_endpoint* reference, rx_protocol_result_t reason)
    {
        routing_endpoint* me = (routing_endpoint*)reference->user_data;
        return me->closed(reason);
    };
}


template <typename translatorT, typename addrT>
routing_endpoint<translatorT,addrT>::~routing_endpoint()
{
    std::ostringstream ss;
    ss << "Routing endpoint at "
        << port_->runtime_->meta_info().get_full_path()
        << " destroyed.";
    ITF_LOG_DEBUG("routing_endpoint", 200, ss.str());
}



template <typename translatorT, typename addrT>
rx_protocol_result_t routing_endpoint<translatorT,addrT>::recv_packet (recv_protocol_packet* packet)
{
    using translates_packet_t = typename translatorT::translates_packet;
    using client_master_t = typename translatorT::client_master_translator;
    auto map_key = translator_.get_reference(packet);
    locks::auto_slim_lock _(&sessions_lock_);
    auto it = active_map_.find(map_key);
    if (it == active_map_.end())
    {
        if constexpr (translates_packet_t::value && !client_master_t::value)
        {
            addrT local_addr;
            addrT remote_addr;
            auto translate_result = translator_.translate_packet(packet, map_key, local_addr, remote_addr);
            if (translate_result != RX_PROTOCOL_OK)
                return translate_result;

            auto session_ptr = std::make_unique<session_endpoint<translatorT, addrT> >(local_addr, remote_addr, packet->to, packet->from, &stack);
            session_ptr->router = smart_this();
            auto stack_endpoint = &session_ptr->stack;
            auto reg_result = port_->add_stack_endpoint(stack_endpoint, &local_addr, &remote_addr);
            if (!reg_result)
            {
                return RX_PROTOCOL_INVALID_ADDR;
            }
            else
            {
                std::ostringstream ss;
                ss << "Client from address "
                    << remote_addr.to_string()
                    << " connected to address "
                    << local_addr.to_string()
                    << ".";
                ITF_LOG_TRACE("routing_endpoint", 500, ss.str());
                auto emplace_result = active_map_.emplace(map_key, std::move(session_ptr));
                it = emplace_result.first;
            }
        }
        else
        {
            return RX_PROTOCOL_INVALID_ADDR;
        }
    }
    return rx_move_packet_up(&it->second->stack, *packet);
}

template <typename translatorT, typename addrT>
rx_protocol_result_t routing_endpoint<translatorT,addrT>::connected (rx_session_def* session)
{
    using translates_sessiont_t = typename translatorT::translates_session;
    using client_master_t = typename translatorT::client_master_translator;
    if constexpr (client_master_t::value)
    {
        if constexpr (translates_sessiont_t::value)
        {
        }
        else
        {
            locks::auto_slim_lock _(&sessions_lock_);
            connected_ = true;
            for (auto& one : active_map_)
            {
                auto result = rx_notify_connected(&one.second->stack, session);
                if (result != RX_PROTOCOL_OK)
                    return result;
            }
            return RX_PROTOCOL_OK;
        }
        
    }
    else
    {
        if constexpr (translates_sessiont_t::value)
        {
            addrT local_addr;
            addrT remote_addr;
            session_key_t map_key;
            auto translate_result = translator_.translate_session(session, map_key, local_addr, remote_addr);
            if (translate_result != RX_PROTOCOL_OK)
                return translate_result;

            rx_protocol_stack_endpoint* stack_endpoint = nullptr;
            {

                locks::auto_slim_lock _(&sessions_lock_);

                auto it = active_map_.find(map_key);
                if (it != active_map_.end())
                    return RX_PROTOCOL_INVALID_ADDR;

                auto session_ptr = std::make_unique<session_endpoint<translatorT, addrT> >(local_addr, remote_addr, 0, 0, &stack);
                session_ptr->router = smart_this();
                stack_endpoint = &session_ptr->stack;
                auto reg_result = port_->add_stack_endpoint(stack_endpoint, &local_addr, &remote_addr);
                if (!reg_result)
                {
                    return RX_PROTOCOL_INVALID_ADDR;
                }
                else
                {
                    std::ostringstream ss;
                    ss << "Client from address "
                        << remote_addr.to_string()
                        << " connected to address "
                        << local_addr.to_string()
                        << ".";
                    ITF_LOG_TRACE("routing_endpoint", 500, ss.str());
                    auto emplace_result = active_map_.emplace(map_key, std::move(session_ptr));
                    it = emplace_result.first;
                }
            }
            if (stack_endpoint)
            {
                auto next_session = rx_create_session(&local_addr, &remote_addr, 0, 0, session);
                return rx_notify_connected(stack_endpoint, &next_session);
            }
            else
            {
                // this shouldn't happen!!!
                RX_ASSERT(false);
                return RX_PROTOCOL_EMPTY;
            }
        }
        else
        {
            return RX_PROTOCOL_INVALID_ADDR;
        }
    }
}

template <typename translatorT, typename addrT>
rx_protocol_result_t routing_endpoint<translatorT,addrT>::disconnected (rx_session_def* session, rx_protocol_result_t reason)
{
    locks::auto_slim_lock _(&sessions_lock_);
    connected_ = false;
    auto active_size = active_map_.size();
    if (active_size)
    {
        std::vector<rx_protocol_stack_endpoint*> endpoints;
        endpoints.reserve(active_size);
        for (auto& one : active_map_)
            endpoints.emplace_back(&one.second->stack);
        for (auto one : endpoints)
            rx_notify_disconnected(one, session, reason);
    }
    return RX_PROTOCOL_OK;
}

template <typename translatorT, typename addrT>
void routing_endpoint<translatorT,addrT>::closed (rx_protocol_result_t reason)
{
    using client_master_t = typename translatorT::client_master_translator;
    if constexpr (client_master_t::value)
    {
        port_->unbind_stack_endpoint(&stack);
    }
    else
    {
        port_->destroy_endpoint(&stack);
        /*std::vector<rx_protocol_stack_endpoint*> endpoints;
        {
            locks::auto_slim_lock _(&sessions_lock_);
            auto active_size = active_map_.size();
            endpoints.reserve(active_size);
            for (auto& one : active_map_)
                endpoints.emplace_back(&one.second->stack);
        }
        for (auto& one : endpoints)
        {
            rx_notify_closed(one, reason);
        }*/
        // this will delete the whole endpoint so nothing can be done after it!!!
       // port_->unbind_stack_endpoint(&stack);

    }
}

template <typename translatorT, typename addrT>
void routing_endpoint<translatorT,addrT>::remove_session (const session_type* who)
{
    locks::auto_slim_lock _(&sessions_lock_);
    auto ref = get_reference(who);
    auto it = active_map_.find(ref);
    if (it != active_map_.end())
    {
        port_->unbind_stack_endpoint(&it->second->stack);
        active_map_.erase(it);
    }
    else
    {
        RX_ASSERT(false);
    }
}

template <typename translatorT, typename addrT>
void routing_endpoint<translatorT,addrT>::close_sessions ()
{
    std::vector<rx_protocol_stack_endpoint*> endpoints;
    {
        locks::auto_slim_lock _(&sessions_lock_);
        auto active_size = active_map_.size();
        if (active_size)
        {
            endpoints.reserve(active_size);
            for (auto& one : active_map_)
                endpoints.emplace_back(&one.second->stack);
        }
    }
    for (auto one : endpoints)
        rx_notify_closed(one, RX_PROTOCOL_OK);
}

template <typename translatorT, typename addrT>
typename routing_endpoint<translatorT, addrT>::session_key_t routing_endpoint<translatorT,addrT>::get_reference (const session_type* who)
{
    using translates_session_t = typename translatorT::translates_session;
    typename routing_endpoint<translatorT, addrT>::session_key_t ref;
    addrT local_addr;
    addrT remote_addr;
    if constexpr (translates_session_t::value)
    {
        auto session = rx_create_session(&who->local_addr, &who->remote_addr, who->local_reference, who->remote_reference, nullptr);
        auto result = translator_.translate_session(&session, ref, local_addr, remote_addr);
        RX_ASSERT(result == RX_PROTOCOL_OK);
        return ref;
    }
    else
    {
        auto packet = rx_create_recv_packet(0, nullptr, who->remote_reference, who->local_reference);
        packet.from_addr = &who->remote_addr; 
        packet.to_addr = &who->local_addr;
        auto result = translator_.translate_packet(&packet, ref, local_addr, remote_addr);
        RX_ASSERT(result == RX_PROTOCOL_OK);
        return ref;
    }
}

template <typename translatorT, typename addrT>
rx_result_with<rx_protocol_stack_endpoint*> routing_endpoint<translatorT,addrT>::create_session (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
    using translates_packet_t = typename translatorT::translates_packet;
    using client_master_t = typename translatorT::client_master_translator;

    addrT local_addr;
    addrT remote_addr;
    auto result = local_addr.parse(local_address);
    if (!result)
        return result.errors();
    result = remote_addr.parse(remote_address);
    if (!result)
        return result.errors();
    session_key_t map_key;
    rx_address_reference_type  local_ref;
    rx_address_reference_type remote_ref;
    auto translate_result = translator_.create_references(local_addr, remote_addr, map_key, local_ref, remote_ref);
    if (translate_result != RX_PROTOCOL_OK)
        return "Jebi ga!!!";
    auto session_ptr = std::make_unique<session_endpoint<translatorT, addrT> >(local_addr, remote_addr, local_ref, remote_ref, &stack);
    session_ptr->router = smart_this();

    auto it = active_map_.find(map_key);
    if (it == active_map_.end())
    {
        auto ret = &session_ptr->stack;
        auto emplace_result = active_map_.emplace(map_key, std::move(session_ptr));
        std::ostringstream ss;
        ss << "Client binded at address "
            << local_addr.to_string()
            << " connecting to address "
            << remote_addr.to_string()
            << ".";
        ITF_LOG_TRACE("routing_endpoint", 500, ss.str());
        if constexpr (client_master_t::value && translates_packet_t::value)
        {
            if (connected_)
            {
                // we are already connected so send connected up
                rx_session session = rx_create_session(&local_addr, &remote_addr, local_ref, remote_ref, nullptr);
                rx_notify_connected(&emplace_result.first->second->stack, &session);
            }
        }

        return ret;
    }
    else
    {
        return "Address already occupied";
    }
}

template class routing_endpoint<simple_slave_routing_translator<uint8_t>, io::numeric_address<uint8_t> >;
template class routing_endpoint<simple_slave_routing_translator<uint16_t>, io::numeric_address<uint16_t> >;
template class routing_endpoint<simple_slave_routing_translator<uint32_t>, io::numeric_address<uint32_t> >;

template class routing_endpoint<simple_master_routing_translator<uint8_t>, io::numeric_address<uint8_t> >;
template class routing_endpoint<simple_master_routing_translator<uint16_t>, io::numeric_address<uint16_t> >;
template class routing_endpoint<simple_master_routing_translator<uint32_t>, io::numeric_address<uint32_t> >;

template class routing_endpoint<address_routing_translator<io::ip4_address>, io::ip4_address>;
// Parameterized Class rx_platform::runtime::io_types::ports_templates::session_endpoint 

template <typename translatorT, typename addrT>
session_endpoint<translatorT,addrT>::session_endpoint (addrT local_addr, addrT remote_addr, rx_address_reference_type local_reference, rx_address_reference_type remote_reference, rx_protocol_stack_endpoint* bellow)
      : local_addr(local_addr),
        remote_addr(remote_addr),
        local_reference(local_reference),
        remote_reference(remote_reference),
        bellow_(bellow)
{
    rx_init_stack_entry(&stack, this);
    this->stack.send_function = [](rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
    {
        session_endpoint* me = (session_endpoint*)reference->user_data;
        return me->send_function(packet);
    };
    this->stack.closed_function = [](rx_protocol_stack_endpoint* reference, rx_protocol_result_t reason)
    {
        session_endpoint* me = (session_endpoint*)reference->user_data;
        return me->closed_function(reason);
    };
}


template <typename translatorT, typename addrT>
session_endpoint<translatorT,addrT>::~session_endpoint()
{
}



template <typename translatorT, typename addrT>
rx_protocol_result_t session_endpoint<translatorT,addrT>::send_function (send_protocol_packet packet)
{
    send_protocol_packet addr_packet = packet;
    addr_packet.from = local_reference;
    addr_packet.to = remote_reference;
    addr_packet.from_addr = &local_addr;
    addr_packet.to_addr = &remote_addr;
    return rx_move_packet_down(bellow_, addr_packet);
}

template <typename translatorT, typename addrT>
void session_endpoint<translatorT,addrT>::closed_function (rx_protocol_result_t reason)
{
    // this will delete the whole endpoint so nothing can be done after it!!!
    router->remove_session(this);
}


} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform

