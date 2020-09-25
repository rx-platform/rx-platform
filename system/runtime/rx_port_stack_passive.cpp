

/****************************************************************************
*
*  system\runtime\rx_port_stack_passive.cpp
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


// rx_port_stack_passive
#include "system/runtime/rx_port_stack_passive.h"

#include "sys_internal/rx_async_functions.h"
#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"
#include "rx_runtime_holder.h"


namespace rx_platform {

namespace runtime {

namespace io_types {

// Class rx_platform::runtime::io_types::passive_builder 


rx_result passive_builder::send_listen (rx_port_ptr who, const io::any_address& local_addr, const io::any_address& remote_addr)
{
    auto& who_data = who->get_instance_data().stack_data;
    if (!who_data.stack_ready || !who_data.stack_top)
        return "Protocol stack not ready";

    rx_port_ptr top = who_data.stack_top;
    auto& top_data = top->get_instance_data().stack_data;
    auto& top_passive_kind = top->get_instance_data().behavior.passive_behavior;
    if (!top_passive_kind)
    {
        RX_ASSERT(false);
        return RX_NOT_IMPLEMENTED;
    }

    auto result = top_data.passive_map.register_passive(who, &local_addr, &remote_addr, top);
    if (!result)
    {
        return result;
    }
    if (top_passive_kind->is_listen_subscriber())
    {
        result = top->get_implementation()->start_listen(&local_addr, &remote_addr);
        if(!result)
            top_data.passive_map.unregister_passive(who, top);
    }
    else
    {
        result = send_listen(top, local_addr, remote_addr);
        if(!result)
            top_data.passive_map.unregister_passive(who, top);

    }
    return result;
}

rx_result passive_builder::send_connect (rx_port_ptr who, const io::any_address& local_addr, const io::any_address& remote_addr)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result passive_builder::send_unbind (rx_port_ptr who)
{
    auto& who_data = who->get_instance_data().stack_data;
    auto binded = who_data.passive_map.get_binded();
    for (auto& one : binded)
    {
        auto& one_data = one->get_instance_data().stack_data;
        auto& one_build_kind = one->get_instance_data().behavior.passive_behavior;
        if (!one_build_kind->is_connect_subscriber() && !one_build_kind->is_listen_subscriber())
        {
            who_data.passive_map.unregister_passive(one, who);
            send_unbind(one);
        }
    }
    return true;
}


// Class rx_platform::runtime::io_types::listen_sender 


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


// Class rx_platform::runtime::io_types::connect_sender 


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


// Class rx_platform::runtime::io_types::passive_ignorant 


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


// Class rx_platform::runtime::io_types::listen_subscriber 


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


// Class rx_platform::runtime::io_types::connect_subscriber 


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


// Class rx_platform::runtime::io_types::listen_connect_subscriber 


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


// Class rx_platform::runtime::io_types::server_master_router 


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


// Class rx_platform::runtime::io_types::client_slave_router 


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


} // namespace io_types
} // namespace runtime
} // namespace rx_platform

