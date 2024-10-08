

/****************************************************************************
*
*  interfaces\rx_port_stack_passive.cpp
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


// rx_port_stack_passive
#include "interfaces/rx_port_stack_passive.h"

#include "sys_internal/rx_async_functions.h"
#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"
#include "system/runtime/rx_runtime_holder.h"
#include "rx_port_stack_active.h"


namespace rx_internal {

namespace interfaces {

namespace port_stack {

namespace stack_passive {

// Class rx_internal::interfaces::port_stack::stack_passive::passive_builder 


rx_result passive_builder::send_listen (rx_port_ptr who, const io::any_address& local_addr, const io::any_address& remote_addr)
{
    auto& who_data = who->get_instance_data().stack_data;
    if (!who_data.build_map.stack_ready || !who_data.build_map.stack_top)
        return "Protocol stack not ready";

    rx_port_ptr stack_top = who_data.build_map.stack_top;
    auto& top_data = stack_top->get_instance_data().stack_data;
    auto& top_passive_kind = stack_top->get_instance_data().behavior.passive_behavior;
    if (!top_passive_kind)
    {
        RX_ASSERT(false);
        return RX_NOT_IMPLEMENTED;
    }

    io::any_address local_addr_copy(local_addr);
    io::any_address remote_addr_copy(remote_addr);
    auto result = top_data.passive_map.register_passive(who, local_addr_copy, remote_addr_copy, stack_top);
    if (!result)
    {
        return result;
    }
    who_data.passive_map.bind_port = stack_top;
    if (top_passive_kind->is_listen_subscriber())
    {
        if (!top_data.passive_map.stack_binded_up)
        {
            result = stack_top->get_implementation()->start_listen(&local_addr_copy, &remote_addr_copy);
            if (result)
            {
                who_data.passive_map.stack_binded_down = true;
                top_data.passive_map.stack_binded_up = true;
            }
            else
            {
                std::ostringstream ss;
                ss << "Port "
                    << who->meta_info().get_full_path()
                    << " returned error on listen request "
                    << "["
                    << local_addr.to_string()
                    << ","
                    << remote_addr.to_string()
                    << "];"
                    << result.errors_line();
                RUNTIME_LOG_ERROR("passive_builder", 200, ss.str());
                top_data.passive_map.unregister_passive(who, stack_top);
                who_data.passive_map.bind_port = rx_port_ptr::null_ptr;
            }
        }
    }
    else
    {
        result = send_listen(stack_top, local_addr, remote_addr);
        if (result)
        {
            top_data.passive_map.stack_binded_up = true;
            who_data.passive_map.stack_binded_down = true;
            who_data.passive_map.bind_port = stack_top;
        }
        else
        {
            top_data.passive_map.unregister_passive(who, stack_top);
            who_data.passive_map.bind_port = rx_port_ptr::null_ptr;
        }

    }
    return result;
}

rx_result passive_builder::send_connect (rx_port_ptr who, const io::any_address& local_addr, const io::any_address& remote_addr)
{
    auto& who_data = who->get_instance_data().stack_data;
    if (!who_data.build_map.stack_ready || !who_data.build_map.stack_top)
    {
        rx_result result("Protocol stack not ready");
        std::ostringstream ss;
        ss << "Port "
            << who->meta_info().get_full_path()
            << " not connected to "
            << "["
            << local_addr.to_string()
            << ","
            << remote_addr.to_string()
            << "];"
            << result.errors_line();
        RUNTIME_LOG_ERROR("passive_builder", 200, ss.str());

        return result;
    }

    auto ep = who->get_implementation()->construct_initiator_endpoint(&local_addr, &remote_addr);
    if (ep)
    {
        io::any_address local_addr_copy(local_addr);
        io::any_address remote_addr_copy(remote_addr);
        std::pair<rx_protocol_stack_endpoint*, rx_session*> connected_data{};
        auto result = send_connect_down_recursive(who, ep, local_addr_copy, remote_addr_copy, connected_data);
        if (result)
        {
            result = stack_active::active_builder::register_connection_endpoints(
                who, ep, who, nullptr, &local_addr_copy, &remote_addr_copy);
            if (result)
            {
                who_data.passive_map.stack_binded_down = true;
                if (connected_data.first)
                {
                    auto session = rx_create_session(&local_addr, &remote_addr, 0, 0, nullptr);
                    rx_notify_connected(connected_data.first, &session);
                }
                return true;
            }
            else
            {
                RX_ASSERT(false);
            }
        }
        if (!result)
        {
            who->get_implementation()->destroy_endpoint(ep);
            std::ostringstream ss;
            ss << "Port "
                << who->meta_info().get_full_path()
                << " not connected to "
                << "["
                << local_addr_copy.to_string()
                << ","
                << remote_addr_copy.to_string()
                << "];"
                << result.errors_line();
            
            RUNTIME_LOG_ERROR("passive_builder", 200, ss.str());
        }
        return result;
    }
    else
    {
        rx_result result("Error constructing endpoint.");

        who->get_implementation()->destroy_endpoint(ep);
        std::ostringstream ss;
        ss << "Port "
            << who->meta_info().get_full_path()
            << " not connected to "
            << "["
            << local_addr.to_string()
            << ","
            << remote_addr.to_string()
            << "];"
            << result.errors_line();
        
        return result;
    }
}

rx_result passive_builder::send_connect_down_recursive (rx_port_ptr who, rx_protocol_stack_endpoint* ep, io::any_address& local_addr, io::any_address remote_addr, std::pair<rx_protocol_stack_endpoint*, rx_session*>& connected)
{
    auto& who_data = who->get_instance_data().stack_data;
    if (!who_data.build_map.stack_ready || !who_data.build_map.stack_top)
        return "Protocol stack not ready";
    rx_port_ptr stack_top = who_data.build_map.stack_top;
    auto& top_data = stack_top->get_instance_data().stack_data;
    auto& top_passive_kind = stack_top->get_instance_data().behavior.passive_behavior;
    if (!top_passive_kind)
    {
        RX_ASSERT(false);
        return RX_NOT_IMPLEMENTED;
    }

    auto result = top_data.passive_map.register_passive(who, local_addr, remote_addr, stack_top);
    if (!result)
    {
        return result.errors();
    }
    who_data.passive_map.bind_port = stack_top;
    rx_protocol_stack_endpoint* top_ep = nullptr;
    if (top_passive_kind->is_connect_subscriber())
    {
        if (true)//!top_data.passive_map.stack_binded)
        {
            rx_result result;
            auto connect_result = stack_top->get_implementation()->start_connect(&local_addr, &remote_addr, ep);
            if (connect_result)
            {
                result = stack_active::active_builder::register_connection_endpoints(stack_top, connect_result.value().endpoint, who, ep, &local_addr, &remote_addr);
                if (result)
                {
                    who_data.passive_map.stack_binded_down = true;
                    top_data.passive_map.stack_binded_up = true;
                    if(connect_result.value().connected)
                        connected.first = connect_result.value().endpoint;
                    return true;
                }
                if (connect_result.value().endpoint)
                {
                    auto result = rx_pop_stack(connect_result.value().endpoint);
                    if (result != RX_PROTOCOL_OK)
                        RX_ASSERT(false);
                }
            }
            else
            {
                result.register_errors(connect_result.errors());
            }
            who->get_implementation()->destroy_endpoint(ep);

            top_data.passive_map.unregister_passive(who, stack_top);
            who_data.passive_map.bind_port = rx_port_ptr::null_ptr;
            return result;
        }
        else
        {
            return "Invalid state, stack binded";
        }
    }
    else
    {

        top_ep = stack_top->get_implementation()->construct_initiator_endpoint(&local_addr, &remote_addr);
        if (top_ep)
        {
            result = stack_active::active_builder::register_connection_endpoints(
                stack_top, top_ep, who, ep, &local_addr, &remote_addr);
            if (result)
            {

                io::any_address local_addr_copy(local_addr);
                io::any_address remote_addr_copy(remote_addr);
                result = send_connect_down_recursive(stack_top, top_ep, local_addr_copy, remote_addr_copy, connected);
                if (result)
                {
                    top_data.passive_map.stack_binded_up = true;
                    who_data.passive_map.stack_binded_down = true;
                    who_data.passive_map.bind_port = stack_top;
                    return true;
                }
            }
            if (ep)
            {
                auto result = rx_pop_stack(ep);
                if (result != RX_PROTOCOL_OK)
                    RX_ASSERT(false);
            }
            stack_top->get_implementation()->destroy_endpoint(top_ep);
        }
        else
        {
            result.register_error("Error constructing endpoint.");
        }
        top_data.passive_map.unregister_passive(who, stack_top);
        who_data.passive_map.bind_port = rx_port_ptr::null_ptr;
    }
    return result;
}

rx_result passive_builder::unbind_passive (rx_port_ptr who)
{
    auto& who_build_kind = who->get_instance_data().behavior.passive_behavior;
    bool bottom_port = (who_build_kind->is_connect_subscriber() || who_build_kind->is_listen_subscriber())
        && !(who_build_kind->is_connect_sender() || who_build_kind->is_listen_sender());
    auto next_port = who;
    while (!bottom_port)
    {// send up
        auto& who_data = next_port->get_instance_data().stack_data.passive_map;
        if (!who_data.bind_port)
        {
            // we are already unbind-ed
            break;
        }
        who_data.stack_binded_down = false;
        auto bottom_ptr = who_data.bind_port;
        auto& temp_build_kind = bottom_ptr->get_instance_data().behavior.passive_behavior;
        auto& bottom_data = bottom_ptr->get_instance_data().stack_data.passive_map;
        RX_ASSERT(bottom_data.stack_binded_up);
        auto result = bottom_data.unregister_passive(next_port, bottom_ptr);
        if (result)
        {
            if(bottom_data.empty())
                bottom_data.stack_binded_up = false;
            who_data.bind_port = rx_port_ptr::null_ptr;
        }
        bottom_port = temp_build_kind->is_connect_subscriber() || temp_build_kind->is_listen_subscriber();
        next_port = bottom_ptr;

    }
    bottom_port = who_build_kind->is_connect_subscriber() || who_build_kind->is_listen_subscriber();
    if (bottom_port)
    {
        auto result = who->get_implementation()->stop_passive();
        who->get_instance_data().stack_data.passive_map.stack_binded_up = false;
        if (!result)
            return result;
    }
    rx_internal::interfaces::port_stack::stack_active::active_builder::close_all_endpoints(who);
    auto result = send_unbind_up_recursive(who);
    if (!result)
        return result;
    return true;
}

rx_result passive_builder::send_unbind_up_recursive (rx_port_ptr who)
{
    auto& who_data = who->get_instance_data().stack_data.passive_map;
    auto binded = who_data.get_binded();
    for (auto& one : binded)
    {
        auto& one_data = one->get_instance_data().stack_data.passive_map;
        auto& one_build_kind = one->get_instance_data().behavior.passive_behavior;
        one_data.stack_binded_down = false;
        who_data.unregister_passive(one, who);
        one_data.bind_port = rx_port_ptr::null_ptr;
        if (!one_build_kind->is_connect_sender() && !one_build_kind->is_listen_sender())
        {
            send_unbind_up_recursive(one);
        }
    }
    who_data.stack_binded_up = false;
    return true;
}


// Class rx_internal::interfaces::port_stack::stack_passive::listen_sender 


bool listen_sender::is_listen_sender ()
{
    return true;
}

bool listen_sender::is_connect_sender ()
{
    return false;
}

bool listen_sender::is_listen_subscriber ()
{
    return false;
}

bool listen_sender::is_connect_subscriber ()
{
    return false;
}


// Class rx_internal::interfaces::port_stack::stack_passive::connect_sender 


bool connect_sender::is_listen_sender ()
{
    return false;
}

bool connect_sender::is_connect_sender ()
{
    return true;
}

bool connect_sender::is_listen_subscriber ()
{
    return false;
}

bool connect_sender::is_connect_subscriber ()
{
    return false;
}


// Class rx_internal::interfaces::port_stack::stack_passive::passive_ignorant 


bool passive_ignorant::is_listen_sender ()
{
    return false;
}

bool passive_ignorant::is_connect_sender ()
{
    return false;
}

bool passive_ignorant::is_listen_subscriber ()
{
    return false;
}

bool passive_ignorant::is_connect_subscriber ()
{
    return false;
}


// Class rx_internal::interfaces::port_stack::stack_passive::listen_subscriber 


bool listen_subscriber::is_listen_sender ()
{
    return false;
}

bool listen_subscriber::is_connect_sender ()
{
    return false;
}

bool listen_subscriber::is_listen_subscriber ()
{
    return true;
}

bool listen_subscriber::is_connect_subscriber ()
{
    return false;
}


// Class rx_internal::interfaces::port_stack::stack_passive::connect_subscriber 


bool connect_subscriber::is_listen_sender ()
{
    return false;
}

bool connect_subscriber::is_connect_sender ()
{
    return false;
}

bool connect_subscriber::is_listen_subscriber ()
{
    return false;
}

bool connect_subscriber::is_connect_subscriber ()
{
    return true;
}


// Class rx_internal::interfaces::port_stack::stack_passive::listen_connect_subscriber 


bool listen_connect_subscriber::is_listen_sender ()
{
    return false;
}

bool listen_connect_subscriber::is_connect_sender ()
{
    return false;
}

bool listen_connect_subscriber::is_listen_subscriber ()
{
    return true;
}

bool listen_connect_subscriber::is_connect_subscriber ()
{
    return true;
}


// Class rx_internal::interfaces::port_stack::stack_passive::server_master_router 


bool server_master_router::is_listen_sender ()
{
    return true;
}

bool server_master_router::is_connect_sender ()
{
    return false;
}

bool server_master_router::is_listen_subscriber ()
{
    return true;
}

bool server_master_router::is_connect_subscriber ()
{
    return false;
}


// Class rx_internal::interfaces::port_stack::stack_passive::client_slave_router 


bool client_slave_router::is_listen_sender ()
{
    return false;
}

bool client_slave_router::is_connect_sender ()
{
    return true;
}

bool client_slave_router::is_listen_subscriber ()
{
    return false;
}

bool client_slave_router::is_connect_subscriber ()
{
    return true;
}


// Class rx_internal::interfaces::port_stack::stack_passive::full_router 


bool full_router::is_listen_sender ()
{
    return true;
}

bool full_router::is_connect_sender ()
{
    return true;
}

bool full_router::is_listen_subscriber ()
{
    return true;
}

bool full_router::is_connect_subscriber ()
{
    return true;
}


// Class rx_internal::interfaces::port_stack::stack_passive::listen_connect_sender 


bool listen_connect_sender::is_listen_sender ()
{
    return true;
}

bool listen_connect_sender::is_connect_sender ()
{
    return true;
}

bool listen_connect_sender::is_listen_subscriber ()
{
    return false;
}

bool listen_connect_sender::is_connect_subscriber ()
{
    return false;
}


} // namespace stack_passive
} // namespace port_stack
} // namespace interfaces
} // namespace rx_internal

