

/****************************************************************************
*
*  system\runtime\rx_active_endpoints.cpp
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

#include "lib/rx_io_addr.h"
#include "rx_runtime_helpers.h"
using namespace rx_platform;

// rx_active_endpoints
#include "system/runtime/rx_active_endpoints.h"
// simple_slave_endpoint
#include "system/runtime/simple_slave_endpoint.h"
// slave_endpoint
#include "system/runtime/slave_endpoint.h"

#include "rx_runtime_holder.h"
#include "interfaces/rx_endpoints.h"


namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_templates {

// Parameterized Class rx_platform::runtime::io_types::ports_templates::session_endpoint 

template <typename translatorT, typename addrT>
session_endpoint<translatorT,addrT>::session_endpoint (addrT local_addr, addrT remote_addr, rx_address_reference_type local_reference, rx_address_reference_type remote_reference, rx_protocol_stack_endpoint* bellow)
      : router(nullptr),
        local_addr(local_addr),
        remote_addr(remote_addr),
        local_reference_(local_reference),
        remote_reference_(remote_reference),
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
    addr_packet.from = local_reference_;
    addr_packet.to = remote_reference_;
    addr_packet.from_addr = &local_addr;
    addr_packet.to_addr = &remote_addr;
    return rx_move_packet_down(bellow_, addr_packet);
}

template <typename translatorT, typename addrT>
void session_endpoint<translatorT,addrT>::closed_function (rx_protocol_result_t reason)
{
    if (router)
    {
        router->remove_session(&stack);
    }
}


// Parameterized Class rx_platform::runtime::io_types::ports_templates::routing_endpoint 

template <typename translatorT, typename addrT>
routing_endpoint<translatorT,addrT>::routing_endpoint (runtime::items::port_runtime* port)
      : port_(port)
{
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
}



template <typename translatorT, typename addrT>
rx_protocol_result_t routing_endpoint<translatorT,addrT>::recv_packet (recv_protocol_packet* packet)
{
    auto map_reference = translator_.get_reference(packet);
    auto it = active_map_.find(map_reference);
    if (it == active_map_.end())
    {
        addrT local_addr;
        addrT remote_addr;
        auto translate_result = translator_.translate_packet(packet, map_reference, local_addr, remote_addr);
        if (translate_result!=RX_PROTOCOL_OK)
            return translate_result;

        auto session_ptr = std::make_unique<session_endpoint<translatorT, addrT> >(std::move(local_addr), std::move(remote_addr), packet->to, packet->from, &stack);
        session_ptr->router = this;
        auto stack_endpoint = &session_ptr->stack;
        auto reg_result = port_->bind_stack_endpoint(stack_endpoint, &local_addr, &remote_addr);
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
            auto emplace_result = active_map_.emplace(map_reference, std::move(session_ptr));
            it = emplace_result.first;
        }
    }
    return rx_move_packet_up(&it->second->stack, *packet);
}

template <typename translatorT, typename addrT>
rx_protocol_result_t routing_endpoint<translatorT,addrT>::connected (rx_session_def* session)
{
    /*for (auto& one : port_->)
    {
        addrT local_addr;
        addrT remote_addr;
        auto trans_result = translator_.translate_from_passive();
        if (!trans_result)
        {
            return trans_result;
        }
        if(one.second->local_addr.is_null())
            connected_function
    }*/
    return RX_PROTOCOL_OK;

    rx_address_reference_type map_reference = 0;
    
    auto it = active_map_.find(map_reference);
    if (it == active_map_.end())
    {
        addrT local_addr;
        addrT remote_addr;
        if (session->local_addr)
        {
            auto result = local_addr.parse(session->local_addr);
        }
        if (session->remote_addr)
        {
            auto result = remote_addr.parse(session->remote_addr);
        }
        auto session_ptr = std::make_unique<session_endpoint<translatorT, addrT> >(local_addr, remote_addr, session->local_ref, session->local_ref, &stack);
        auto stack_endpoint = &session_ptr->stack;

        auto reg_result = port_->bind_stack_endpoint(stack_endpoint, &local_addr, &remote_addr);
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
            auto emplace_result = active_map_.emplace(map_reference, std::move(session_ptr));
            it = emplace_result.first;
        }
        
        
    }
    return rx_notify_connected(&it->second->stack, session);
}

template <typename translatorT, typename addrT>
rx_protocol_result_t routing_endpoint<translatorT,addrT>::disconnected (rx_session_def* session, rx_protocol_result_t reason)
{
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
    close_sessions();
    port_->remove_endpoint(&stack);
}

template <typename translatorT, typename addrT>
void routing_endpoint<translatorT,addrT>::remove_session (rx_protocol_stack_endpoint* endpoint)
{
}

template <typename translatorT, typename addrT>
void routing_endpoint<translatorT,addrT>::close_sessions ()
{
    auto active_size = active_map_.size();
    if (active_size)
    {
        std::vector<rx_protocol_stack_endpoint*> endpoints;
        endpoints.reserve(active_size);
        for (auto& one : active_map_)
            endpoints.emplace_back(&one.second->stack);
        for (auto one : endpoints)
            rx_notify_closed(one, RX_PROTOCOL_OK);
        active_map_.clear();
    }
}

template class routing_endpoint<simple_slave_routing_translator<uint8_t>, io::numeric_address<uint8_t> >;
template class routing_endpoint<simple_slave_routing_translator<uint16_t>, io::numeric_address<uint16_t> >;
template class routing_endpoint<simple_slave_routing_translator<uint32_t>, io::numeric_address<uint32_t> >;
} // namespace ports_templates
} // namespace io_types
} // namespace runtime
} // namespace rx_platform

