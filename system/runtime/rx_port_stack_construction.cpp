

/****************************************************************************
*
*  system\runtime\rx_port_stack_construction.cpp
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


// rx_port_stack_data
#include "system/runtime/rx_port_stack_data.h"
// rx_port_stack_construction
#include "system/runtime/rx_port_stack_construction.h"

#include "sys_internal/rx_async_functions.h"
#include "api/rx_namespace_api.h"
#include "model/rx_model_algorithms.h"
#include "rx_runtime_holder.h"
#include "rx_port_stack_passive.h"


namespace rx_platform {

namespace runtime {

namespace io_types {

// Class rx_platform::runtime::io_types::assemble_sender 


bool assemble_sender::is_assemble_sender ()
{
    return true;
}

bool assemble_sender::is_assemble_subscriber ()
{
    return false;
}

bool assemble_sender::is_external ()
{
    return true;
}

bool assemble_sender::is_application ()
{
    return false;
}


// Class rx_platform::runtime::io_types::assemble_ignorant 


bool assemble_ignorant::is_assemble_sender ()
{
    return false;
}

bool assemble_ignorant::is_assemble_subscriber ()
{
    return false;
}

bool assemble_ignorant::is_external ()
{
    return false;
}

bool assemble_ignorant::is_application ()
{
    return false;
}


// Class rx_platform::runtime::io_types::assemble_subscriber 


bool assemble_subscriber::is_assemble_sender ()
{
    return false;
}

bool assemble_subscriber::is_assemble_subscriber ()
{
    return true;
}

bool assemble_subscriber::is_external ()
{
    return false;
}

bool assemble_subscriber::is_application ()
{
    return true;
}


// Class rx_platform::runtime::io_types::assemble_sender_subscriber 


bool assemble_sender_subscriber::is_assemble_sender ()
{
    return true;
}

bool assemble_sender_subscriber::is_assemble_subscriber ()
{
    return true;
}

bool assemble_sender_subscriber::is_external ()
{
    return false;
}

bool assemble_sender_subscriber::is_application ()
{
    return false;
}


// Class rx_platform::runtime::io_types::stack_builder 


rx_result stack_builder::connect_stack_top (rx_port_ptr top, rx_port_ptr who)
{
    auto& top_build_kind = top->get_instance_data().behavior.build_behavior;
    auto& who_build_kind = who->get_instance_data().behavior.build_behavior;
    // check to see if we support it
    if (!top_build_kind || top_build_kind->is_application())
    {
        // can't put anything on top of the application port
        return RX_NOT_SUPPORTED;
    }
    if (!who_build_kind || who_build_kind->is_external())
    {
        // can't put external on top
        return RX_NOT_SUPPORTED;
    }
    // collect instance data and do the connection
    auto& top_data = top->get_instance_data().stack_data;
    auto& who_data = who->get_instance_data().stack_data;
    if (who_data.stack_top)
    {
        RX_ASSERT(false);// this shouldn't happen
        return RX_ALREADY_CONNECTED;
    }
    who_data.stack_top = top;
    auto result = top_data.passive_map.register_port(who, top);
    if (result)
    {
        if (top_build_kind->is_assemble_sender() || top_data.stack_ready)
            stack_builder::recursive_send_stack_assembled(who);
    }
    else
    {
        who_data.stack_top = rx_port_ptr::null_ptr;
    }
    return result;
}

rx_result stack_builder::disconnect_stack (rx_port_ptr who)
{
    auto& who_data = who->get_instance_data().stack_data;
    auto& who_build_kind = who->get_instance_data().behavior.build_behavior;
    bool is_sender = who_build_kind && who_build_kind->is_assemble_sender();
    if (is_sender)
    {
        recursive_send_stack_disassembled_up(who);
    }
    else if (who_data.stack_top)
    {
        who_data.stack_top->get_instance_data().stack_data.passive_map.unregister_port(who, who_data.stack_top);

        auto top_ptr = who_data.stack_top;
        who_data.stack_top = rx_port_ptr::null_ptr;
        while (top_ptr)
        {
            auto& top_build_kind = top_ptr->get_instance_data().behavior.build_behavior;
            if (top_build_kind->is_assemble_sender())
            {
                recursive_send_stack_disassembled_up(top_ptr);
                break;
            }
            else
                top_ptr = top_ptr->get_instance_data().stack_data.stack_top;
        }
    }
    return true;
}

void stack_builder::recursive_send_stack_assembled (rx_port_ptr who)
{
    auto& who_data = who->get_instance_data().stack_data;
    auto& who_build_kind = who->get_instance_data().behavior.build_behavior;
    who_data.stack_ready = true;
    if (who_build_kind->is_assemble_subscriber())
    {
        std::ostringstream ss;
        ss << "Stack assembled:";

        std::vector<rx_port_ptr> stack;
        stack.emplace_back(who);
        auto stack_top = who->get_instance_data().stack_data.stack_top;
        if (stack_top)
            recursive_get_stack(stack_top, stack);
        bool first = true;
        for (auto& one : stack)
        {
            if (first)
                first = false;
            else
                ss << ">>";
            ss << one->meta_info().name;
        }
        RUNTIME_LOG_TRACE("stack_builder", 900, ss.str());
        who->get_implementation()->stack_assembled();
        return;
    }
    auto up_stack = who_data.passive_map.get_registered();
    for (auto& one : up_stack)
    {
        recursive_send_stack_assembled(one);
    }
}

void stack_builder::recursive_send_stack_disassembled_up (rx_port_ptr who)
{
    auto unbind_result = passive_builder::send_unbind(who);
    auto& who_data = who->get_instance_data().stack_data;
    auto& who_build_kind = who->get_instance_data().behavior.build_behavior;
    who_data.stack_ready = false;
    if (who_build_kind->is_assemble_sender())
    {
        who->get_implementation()->stack_disassembled();
        return;
    }
    auto up_stack = who_data.passive_map.get_registered();
    for (auto& one : up_stack)
    {
        recursive_send_stack_disassembled_up(one);
    }
}

void stack_builder::recursive_get_stack (rx_port_ptr top, std::vector<rx_port_ptr>& stack)
{
    stack.push_back(top);
    if (top->get_instance_data().behavior.build_behavior->is_assemble_sender())
        return;
    auto stack_top = top->get_instance_data().stack_data.stack_top;
    if (stack_top)
        recursive_get_stack(stack_top, stack);
}


} // namespace io_types
} // namespace runtime
} // namespace rx_platform

