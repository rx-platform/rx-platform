

/****************************************************************************
*
*  system\runtime\rx_port_instance.cpp
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


// rx_runtime_instance
#include "system/runtime/rx_runtime_instance.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_port_instance
#include "system/runtime/rx_port_instance.h"

#include "rx_runtime_holder.h"
#include "rx_port_stack_construction.h"


namespace rx_platform {

namespace runtime {

namespace items {

// Class rx_platform::runtime::items::port_instance_data 

port_instance_data::port_instance_data (const port_data& data, port_behaviors&& rt_behavior)
      : executer_(-1)
    , data_(data)
    , behavior(std::move(rt_behavior))
{
}


port_instance_data::~port_instance_data()
{
}



bool port_instance_data::connect_application (rx_application_ptr&& app, rx_port_ptr whose)
{
    RX_ASSERT(!my_application_);
    my_application_ = std::move(app);
    my_application_->get_instance_data().add_port(whose);
    return true;
}

bool port_instance_data::disconnect_application (rx_port_ptr whose)
{
    if (my_application_)
    {
        auto temp = my_application_;
        my_application_ = rx_application_ptr::null_ptr;
        temp->get_instance_data().remove_port(whose);
    }
    return true;
}

rx_result port_instance_data::before_init_runtime (rx_port_ptr what, runtime::runtime_init_context& ctx)
{
    RX_ASSERT(what->get_instance_data().my_application_);
    if (what->get_instance_data().my_application_)
    {
        what->get_instance_data().executer_ = what->get_instance_data().my_application_->get_executer();
        auto result = what->get_instance_data().identity_.create_context("", what->meta_info().get_full_path(), what->get_instance_data().data_.identity);
        // for port we have to have executer cached value
        what->get_instance_data().implementation_ = what->get_implementation();
        auto rt_ptr = what->get_instance_data().implementation_;
        RX_ASSERT(rt_ptr);
        if (rt_ptr)
        {
            rt_ptr->context_ = ctx.context;
            rt_ptr->runtime_ = what;
            rt_ptr->executer_ = what->get_instance_data().executer_;
            if (what->get_instance_data().identity_.get_context())
                rt_ptr->identity_ = what->get_instance_data().identity_.get_context()->get_handle();
        }
    }
    else
    {

    }
    return true;
}

rx_result port_instance_data::before_start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx, operational::binded_tags* binded)
{
    return true;
}

rx_result port_instance_data::after_deinit_runtime (rx_port_ptr what, runtime::runtime_deinit_context& ctx)
{
    what->get_implementation()->runtime_ = rx_port_ptr::null_ptr;
    what->get_instance_data().identity_.destory_context();
    return true;
}

rx_result port_instance_data::after_stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx)
{
    auto result = runtime::io_types::stack_builder::disconnect_stack(what);
    if (result)
        RUNTIME_LOG_DEBUG("port_stack_relation", 900, what->meta_info().get_full_path() + " disconnected from stack");
    else
        RUNTIME_LOG_ERROR("port_stack_relation", 900, what->meta_info().get_full_path() + " not disconnected from stack");

    return true;
}

const security::security_context_ptr& port_instance_data::get_security_context () const
{
    return identity_.get_context();
}


const rx_application_ptr port_instance_data::get_my_application () const
{
  return my_application_;
}


// Class rx_platform::runtime::items::port_behaviors 


} // namespace items

namespace io_types {

// Class rx_platform::runtime::io_types::port_build_behavior 


// Class rx_platform::runtime::io_types::port_passive_behavior 


// Class rx_platform::runtime::io_types::port_active_behavior 


} // namespace io_types
} // namespace runtime
} // namespace rx_platform

