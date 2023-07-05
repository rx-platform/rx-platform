

/****************************************************************************
*
*  interfaces\rx_port_instance.cpp
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


// rx_runtime_instance
#include "runtime_internal/rx_runtime_instance.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_port_instance
#include "interfaces/rx_port_instance.h"

#include "system/runtime/rx_runtime_holder.h"
#include "rx_port_stack_construction.h"
#include "rx_port_stack_active.h"
#include "rx_port_stack_passive.h"
#include "runtime_internal/rx_runtime_relations.h"
#include "model/rx_meta_internals.h"


namespace rx_internal {

namespace sys_runtime {

namespace runtime_core {

namespace runtime_data {

// Class rx_internal::sys_runtime::runtime_core::runtime_data::port_instance_data 

port_instance_data::port_instance_data (const port_data& data, port_behaviors&& rt_behavior)
      : executer_(-1),
        mine_security_(false)
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

        security::security_context_ptr sec_ctx;
        auto sec_result = what->get_instance_data().identity_.create_context(what->meta_info().get_full_path(), "", what->get_instance_data().data_.identity, sec_ctx);

        if (!sec_result)
        {
            RUNTIME_LOG_WARNING("application_instance_data", 900, "Unable to create security context:"s + sec_result.errors_line());
        }
        if (sec_ctx)
        {
            what->get_instance_data().security_ctx_ = sec_ctx;

            what->get_instance_data().security_ctx_->login();
            what->get_instance_data().mine_security_ = true;
        }
        else 
        {
            what->get_instance_data().security_ctx_ = what->get_instance_data().my_application_->get_instance_data().get_security_context();
        }

        const auto& app_meta = what->get_instance_data().my_application_->meta_info();
        auto relation_ptr = rx_create_reference<relations::relation_data>();
        auto& repository = rx_internal::model::platform_types_manager::instance().get_relations_repository();
        auto create_result = repository.create_runtime(RX_NS_PORT_APPLICATION_RELATION_ID, "App", *relation_ptr, ctx.directories);
        if (create_result)
        {
            relation_ptr->target_path = app_meta.get_full_path() + RX_OBJECT_DELIMETER + what->meta_info().name;
            relation_ptr->target_id = app_meta.id;
            relation_ptr->name = "App";
            relation_ptr->target_relation_name = what->meta_info().name;
            rx_timed_value str_val;
            str_val.assign_static(relation_ptr->target_path.c_str(), rx_time::now());
            relation_ptr->value.value = str_val;
            relation_ptr->value.value_opt[runtime::structure::value_opt_readonly] = true;

            auto result = what->add_implicit_relation(relation_ptr);
            if (!result)
            {
                std::ostringstream ss;
                ss << "Error adding Port=>App relation for "
                    << what->meta_info().get_full_path()
                    << ". "
                    << result.errors_line();
                RUNTIME_LOG_ERROR("port_stack_relation", 900, ss.str());
                return result;
            }
        }
        else
        {
            return create_result.errors();
        }
        // for port we have to have executer cached value
        what->get_instance_data().implementation_ = what->get_implementation();
        auto rt_ptr = what->get_instance_data().implementation_;
        RX_ASSERT(rt_ptr);
        if (rt_ptr)
        {
            rt_ptr->context_ = ctx.context;
            rt_ptr->runtime_ = what;
            rt_ptr->executer_ = what->get_instance_data().executer_;
        }
    }
    else
    {

    }
    return true;
}

rx_result port_instance_data::before_start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx, tag_blocks::binded_tags* binded)
{
    return true;
}

rx_result port_instance_data::after_deinit_runtime (rx_port_ptr what, runtime::runtime_deinit_context& ctx)
{
    if (what->get_instance_data().mine_security_ && what->get_instance_data().security_ctx_)
        what->get_instance_data().security_ctx_->logout();
    what->get_implementation()->runtime_ = rx_port_ptr::null_ptr;
    return true;
}

rx_result port_instance_data::after_stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx)
{
    auto result = rx_internal::interfaces::port_stack::stack_build::stack_builder::disconnect_stack(what);
    if (result)
        RUNTIME_LOG_TRACE("port_stack_relation", 900, what->meta_info().get_full_path() + " disconnected from stack");
    else
        RUNTIME_LOG_ERROR("port_stack_relation", 900, what->meta_info().get_full_path() + " error disconnecting stack:" + result.errors_line());

    return true;
}

security::security_context_ptr port_instance_data::get_security_context () const
{
    if (security_ctx_)
    {
        return security_ctx_;
    }
    else if (my_application_)
    {
        return my_application_->get_instance_data().get_security_context();
    }
    else
    {
        static security::security_context_ptr g_dummy;
        return g_dummy;
    }
}


rx_application_ptr port_instance_data::get_my_application ()
{
  return my_application_;
}


// Class rx_internal::sys_runtime::runtime_core::runtime_data::port_behaviors 


// Class rx_internal::sys_runtime::runtime_core::runtime_data::ports_auto_lock 

ports_auto_lock::ports_auto_lock (runtime_core::runtime_data::port_instance_data* data)
      : data_(data)
{
    locks::slim_lock* tmp = const_cast<locks::slim_lock*>(&data_->get_my_application()->get_instance_data().ports_lock);
    tmp->lock();
}


ports_auto_lock::~ports_auto_lock()
{
    locks::slim_lock* tmp = const_cast<locks::slim_lock*>(&data_->get_my_application()->get_instance_data().ports_lock);
    tmp->unlock();
}



} // namespace runtime_data
} // namespace runtime_core
} // namespace sys_runtime

namespace interfaces {

namespace port_stack {

// Class rx_internal::interfaces::port_stack::port_active_behavior 


// Class rx_internal::interfaces::port_stack::port_build_behavior 


// Class rx_internal::interfaces::port_stack::port_passive_behavior 


} // namespace port_stack
} // namespace interfaces
} // namespace rx_internal



// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
    auto sec_result = identity_.create_context(meta.get_full_path(), rx_gate::instance().get_instance_name(), data_.identity);
    if (sec_result)
        return sec_result.value();
    else
        return security::unauthorized_context();

#endif
