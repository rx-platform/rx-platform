

/****************************************************************************
*
*  runtime_internal\rx_runtime_instance.cpp
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

#include "lib/rx_ser_json.h"

// rx_runtime_instance
#include "runtime_internal/rx_runtime_instance.h"

#include "system/runtime/rx_runtime_holder.h"
#include "api/rx_platform_api.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "system/server/rx_server.h"
#include "runtime_internal/rx_runtime_relations.h"
#include "model/rx_meta_internals.h"
#include "system/runtime/rx_internal_objects.h"
#include "sys_internal/rx_security/rx_platform_security.h"
using namespace meta::object_types;


namespace rx_internal {

namespace sys_runtime {

namespace runtime_core {

namespace runtime_data {

// Class rx_internal::sys_runtime::runtime_core::runtime_data::application_instance_data 

application_instance_data::application_instance_data (const application_data& data, int rt_behavior)
      : executer_(-1),
        mine_security_(false)
    , data_(data)
{
}



void application_instance_data::get_ports (api::query_result& result, bool extern_only)
{
    result.items.reserve(ports_.size());
    for (const auto& one : ports_)
    {
        if (extern_only 
            && one.second->get_instance_data().behavior.active_behavior 
            && one.second->get_instance_data().behavior.active_behavior->is_extern())
            result.items.emplace_back(api::query_result_detail{ rx_port, one.second->meta_info() });
    }
}

void application_instance_data::add_port (rx_port_ptr what)
{
    ports_.emplace(what->meta_info().id, what);
}

void application_instance_data::add_domain (rx_domain_ptr what)
{
    // domains can be from other thread so do the locking
    locks::auto_lock_t<decltype(domains_lock_)> _(&domains_lock_);
    domains_.emplace(what->meta_info().id, what);
}

void application_instance_data::remove_port (rx_port_ptr what)
{
    auto it = ports_.find(what->meta_info().id);
    if (it != ports_.end())
        ports_.erase(it);
}

void application_instance_data::remove_domain (rx_domain_ptr what)
{
    // domains can be from other thread so do the locking
    locks::auto_lock_t<decltype(domains_lock_)> _(&domains_lock_);
    auto it = domains_.find(what->meta_info().id);
    if (it != domains_.end())
        domains_.erase(it);
}

void application_instance_data::get_domains (api::query_result& result)
{
    // domains can be from other thread so do the locking
    locks::auto_lock_t<decltype(domains_lock_)> _(&domains_lock_);
    result.items.reserve(domains_.size());
    for (const auto& one : domains_)
    {
        result.items.emplace_back(api::query_result_detail{ rx_domain, one.second->meta_info() });
    }
}

rx_result application_instance_data::before_init_runtime (rx_application_ptr what, runtime::runtime_init_context& ctx)
{
    security::security_context_ptr sec_ctx;
    auto sec_result = what->get_instance_data().identity_.create_context(what->meta_info().get_full_path(), "", what->get_instance_data().data_.identity, sec_ctx);

    if (!sec_result)
    {
        RUNTIME_LOG_WARNING("application_instance_data", 900, "Unable to create security context:"s + sec_result.errors_line());
    }
    if(sec_ctx)
    {
        what->get_instance_data().security_ctx_ = sec_ctx;

        what->get_instance_data().security_ctx_->login();
        what->get_instance_data().mine_security_ = true;
    }
    else if(what->meta_info().id != rx_node_id(RX_HOST_APP_ID)
        && what->meta_info().id != rx_node_id(RX_NS_SYSTEM_APP_ID)
        && what->meta_info().id != rx_node_id(RX_NS_WORLD_APP_ID))
    {
        if (what->meta_info().path.size() > 5)
        {
            if (memcmp(what->meta_info().path.c_str(), "/world", 6) == 0)
            {
                what->get_instance_data().security_ctx_ = rx_platform::sys_objects::world_application::instance()->world_identity;
            }
            else if (memcmp(what->meta_info().path.c_str(), "/sys/", 5) == 0)
            {
                if (what->meta_info().path.size() > 10 && memcmp(what->meta_info().path.c_str(), "/sys/host/", 7) == 0)
                {
                    what->get_instance_data().security_ctx_ = rx_platform::sys_objects::host_application::instance()->host_identity;
                }
                else
                {
                    what->get_instance_data().security_ctx_ = rx_platform::sys_objects::system_application::instance()->system_identity;
                }
            }
            else
            {
                what->get_instance_data().security_ctx_ = rx_platform::sys_objects::unassigned_application::instance()->unassigned_identity;
            }
        }
        else
        {
            RUNTIME_LOG_WARNING("application_instance_data", 900, "Unable to create security context: Unrecognized path");
        }
 
    }
    else
    {
        switch (what->meta_info().id.get_numeric())
        {
        case RX_HOST_APP_ID:
            what->get_instance_data().security_ctx_ = rx_platform::sys_objects::host_application::instance()->host_identity;
            break;
        case RX_NS_SYSTEM_APP_ID:
            what->get_instance_data().security_ctx_ = rx_platform::sys_objects::system_application::instance()->system_identity;
            break;
        case RX_NS_WORLD_APP_ID:
            what->get_instance_data().security_ctx_ = rx_platform::sys_objects::world_application::instance()->world_identity;
            break;
        default:
            RUNTIME_LOG_WARNING("application_instance_data", 900, "Unable to create security context:"s + sec_result.errors_line());
        }
    }

    what->get_instance_data().executer_ = rx_internal::sys_runtime::platform_runtime_manager::instance().resolve_app_processor(what->get_instance_data());
    what->get_implementation()->context_ = ctx.context;
    ctx.context->set_events_manager(&what->get_instance_data().events_);
    what->get_implementation()->executer_ = what->get_instance_data().executer_;
    ctx.set_item_static("CPU", (int)rx_internal::infrastructure::server_runtime::instance().get_CPU(what->get_instance_data().executer_));
    return true;
}

rx_result application_instance_data::before_start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result application_instance_data::after_deinit_runtime (rx_application_ptr what, runtime::runtime_deinit_context& ctx)
{
    if (what->get_instance_data().mine_security_ && what->get_instance_data().security_ctx_)
        what->get_instance_data().security_ctx_->logout();
    what->get_instance_data().security_ctx_ = security::security_context_ptr::null_ptr;
    return true;
}

rx_result application_instance_data::after_stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx)
{
    
    return true;
}

security::security_context_ptr application_instance_data::get_security_context () const
{
    if (security_ctx_)
    {
        return security_ctx_;
    }
    else
    {
        return security::unauthorized_context();
    }
}

std::vector<rx_domain_ptr> application_instance_data::get_domains ()
{
    std::vector<rx_domain_ptr> result;
    result.reserve(domains_.size());
    locks::auto_lock_t<decltype(domains_lock_)> _(&domains_lock_);
    for (const auto& one : domains_)
    {
        result.emplace_back(one.second);
    }
    return result;
}

std::vector<rx_port_ptr> application_instance_data::get_ports ()
{
    std::vector<rx_port_ptr> result;
    result.reserve(ports_.size());
    for (const auto& one : ports_)
    {
        result.emplace_back(one.second);
    }
    return result;
}

rx_thread_handle_t application_instance_data::resolve_executer ()
{
    executer_ = executer_;
    return executer_;
}


// Class rx_internal::sys_runtime::runtime_core::runtime_data::domain_instance_data 

domain_instance_data::domain_instance_data (const domain_data& data, int rt_behavior)
      : executer_(-1)
    , data_(data)
{
}



void domain_instance_data::get_objects (api::query_result& result)
{
    result.items.reserve(objects_.size());
    for (const auto& one : objects_)
    {
        if (one.second)
            result.items.emplace_back(api::query_result_detail{ rx_object, one.second->meta_info() });
    }
}

void domain_instance_data::add_object (rx_object_ptr what)
{
    objects_.emplace(what->meta_info().id, what);
}

void domain_instance_data::remove_object (rx_object_ptr what)
{
    auto it = objects_.find(what->meta_info().id);
    if (it != objects_.end())
        objects_.erase(it);
}

bool domain_instance_data::connect_application (rx_application_ptr&& app, rx_domain_ptr whose)
{
    RX_ASSERT(!my_application_);
    my_application_ = std::move(app);
    my_application_->get_instance_data().add_domain(whose);
    return true;
}

bool domain_instance_data::disconnect_application (rx_domain_ptr whose)
{
    if (my_application_)
    {
        auto temp = my_application_;
        my_application_ = rx_application_ptr::null_ptr;
        temp->get_instance_data().remove_domain(whose);
    }
    return true;
}

rx_result domain_instance_data::before_init_runtime (rx_domain_ptr what, runtime::runtime_init_context& ctx)
{
    RX_ASSERT(what->get_instance_data().my_application_);
    if (what->get_instance_data().my_application_)
    {
        if (what->get_instance_data().get_data().processor >= 0)
            what->get_instance_data().executer_ = rx_internal::sys_runtime::platform_runtime_manager::instance().resolve_domain_processor(what->get_instance_data());
        else
            what->get_instance_data().executer_ = what->get_instance_data().my_application_->get_executer();
        what->get_implementation()->executer_ = what->get_instance_data().executer_;

        const auto& app_meta = what->get_instance_data().my_application_->meta_info();
        auto relation_ptr = rx_create_reference<relations::relation_data>();
        auto& repository = rx_internal::model::platform_types_manager::instance().get_relations_repository();
        auto create_result = repository.create_runtime(RX_NS_APPLICATION_RELATION_ID, "App", *relation_ptr, ctx.directories);
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
                ss << "Error adding Domain=>App relation for "
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
    }
    else
    {

    }
    what->get_implementation()->context_ = ctx.context;
    ctx.context->set_events_manager(&what->get_instance_data().events_);

    ctx.set_item_static("CPU", (int)rx_internal::infrastructure::server_runtime::instance().get_CPU(what->get_instance_data().executer_));
    return true;
}

rx_result domain_instance_data::before_start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result domain_instance_data::after_deinit_runtime (rx_domain_ptr what, runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result domain_instance_data::after_stop_runtime (rx_domain_ptr what, runtime::runtime_stop_context& ctx)
{
    return true;
}

security::security_context_ptr domain_instance_data::get_security_context () const
{
    if (my_application_)
    {
        return my_application_->get_instance_data().get_security_context();
    }
    else
    {
        static security::security_context_ptr g_dummy;
        return g_dummy;
    }
}

std::vector<rx_object_ptr> domain_instance_data::get_objects ()
{
    std::vector<rx_object_ptr> result;
    result.reserve(objects_.size());
    for (const auto& one : objects_)
    {
        result.emplace_back(one.second);
    }
    return result;
}


// Class rx_internal::sys_runtime::runtime_core::runtime_data::object_instance_data 

object_instance_data::object_instance_data (const object_data& data, int rt_behavior)
      : executer_(-1)
    , data_(data)
{
}



bool object_instance_data::connect_domain (rx_domain_ptr&& domain, rx_object_ptr whose)
{
    RX_ASSERT(!my_domain_);
    my_domain_ = std::move(domain);
    my_domain_->get_instance_data().add_object(whose);
    return true;
}

bool object_instance_data::disconnect_domain (rx_object_ptr whose)
{
    if (my_domain_)
    {
        auto temp = my_domain_;
        my_domain_ = rx_domain_ptr::null_ptr;
        temp->get_instance_data().remove_object(whose);
    }
    return true;
}

rx_result object_instance_data::before_init_runtime (rx_object_ptr what, runtime::runtime_init_context& ctx)
{
    RX_ASSERT(what->get_instance_data().my_domain_);
    if (what->get_instance_data().my_domain_)
    {

        ctx.context->set_events_manager(
            &what->get_instance_data().my_domain_->get_instance_data().get_events());

        what->get_instance_data().executer_ = what->get_instance_data().my_domain_->get_executer();
        what->get_implementation()->executer_ = what->get_instance_data().executer_;

        const auto& app_meta = what->get_instance_data().my_domain_->meta_info();
        auto relation_ptr = rx_create_reference<relations::relation_data>();
        auto& repository = rx_internal::model::platform_types_manager::instance().get_relations_repository();
        auto create_result = repository.create_runtime(RX_NS_DOMAIN_RELATION_ID, "Domain", *relation_ptr, ctx.directories);
        if (create_result)
        {
            relation_ptr->target_path = app_meta.get_full_path() + RX_OBJECT_DELIMETER + what->meta_info().name;
            relation_ptr->target_id = app_meta.id;
            relation_ptr->name = "Domain";
            relation_ptr->target_relation_name = what->meta_info().name;
            rx_timed_value str_val;
            str_val.assign_static(relation_ptr->target_path.c_str(), rx_time::now());
            relation_ptr->value.value = str_val;
            relation_ptr->value.value_opt[runtime::structure::value_opt_readonly] = true;

            auto result = what->add_implicit_relation(relation_ptr);
            if (!result)
            {
                std::ostringstream ss;
                ss << "Error adding Object=>Domain relation for "
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
    }
    else
    {

    }
    what->get_implementation()->context_ = ctx.context;
    return true;
}

rx_result object_instance_data::before_start_runtime (rx_object_ptr what, runtime::runtime_start_context& ctx, tag_blocks::binded_tags* binded)
{
    return true;
}

rx_result object_instance_data::after_deinit_runtime (rx_object_ptr what, runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result object_instance_data::after_stop_runtime (rx_object_ptr what, runtime::runtime_stop_context& ctx)
{
    return true;
}

security::security_context_ptr object_instance_data::get_security_context () const
{
    if (my_domain_)
    {
        return my_domain_->get_instance_data().get_security_context();
    }
    else
    {
        static security::security_context_ptr g_dummy;
        return g_dummy;
    }
}


} // namespace runtime_data
} // namespace runtime_core
} // namespace sys_runtime
} // namespace rx_internal

