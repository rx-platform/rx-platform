

/****************************************************************************
*
*  system\runtime\rx_runtime_instance.cpp
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

#include "system/serialization/rx_ser.h"

// rx_runtime_instance
#include "system/runtime/rx_runtime_instance.h"

#include "rx_runtime_holder.h"
#include "api/rx_platform_api.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "system/server/rx_server.h"
#include "runtime_internal/rx_runtime_relations.h"
#include "model/rx_meta_internals.h"
using namespace meta::object_types;


namespace rx_platform {

namespace runtime {

namespace items {

// Class rx_platform::runtime::items::object_instance_data 

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
            str_val.assign_static<string_type>(string_type(relation_ptr->target_path), rx_time::now());
            relation_ptr->value.value = str_val;
            relation_ptr->value.value_opt[runtime::structure::value_data::opt_readonly] = true;

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


// Class rx_platform::runtime::items::domain_instance_data 

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
            str_val.assign_static<string_type>(string_type(relation_ptr->target_path), rx_time::now());
            relation_ptr->value.value = str_val;
            relation_ptr->value.value_opt[runtime::structure::value_data::opt_readonly] = true;

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
    return true;
}

rx_result domain_instance_data::before_start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx, tag_blocks::binded_tags* binded)
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


// Class rx_platform::runtime::items::application_instance_data 

application_instance_data::application_instance_data (const application_data& data, int rt_behavior)
      : executer_(-1)
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
    auto sec_ctx = what->get_instance_data().identity_.create_context(what->meta_info().get_full_path(), "", what->get_instance_data().data_.identity);

  //what->get_instance_data().security_ctx_
    if (what->get_instance_data().security_ctx_)
        what->get_instance_data().security_ctx_->logout();
    what->get_instance_data().executer_ = rx_internal::sys_runtime::platform_runtime_manager::instance().resolve_app_processor(what->get_instance_data());
    what->get_implementation()->context_ = ctx.context;
    what->get_implementation()->executer_ = what->get_instance_data().executer_;
    return true;
}

rx_result application_instance_data::before_start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx, tag_blocks::binded_tags* binded)
{
    auto result = binded->set_item_static<int>("CPU", (int)rx_internal::infrastructure::server_runtime::instance().get_CPU(what->get_instance_data().executer_), ctx);
    return true;
}

rx_result application_instance_data::after_deinit_runtime (rx_application_ptr what, runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result application_instance_data::after_stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx)
{
    if (what->get_instance_data().security_ctx_)
        what->get_instance_data().security_ctx_->logout();
    return true;
}

security::security_context_ptr application_instance_data::get_security_context () const
{
    if (security_ctx_)
        return security_ctx_;
    else
        return security::unauthorized_context();
}

//application_instance_data::application_instance_data(application_instance_data&& right)
//{
//    domains_ = std::move(right.domains_);
//    ports_ = std::move(right.ports_);
//    executer_ = std::move(right.executer_);
//    identity_ = std::move(right.identity_);
//}
//application_instance_data::application_instance_data(const application_instance_data& right)
//{
//    domains_ = right.domains_;
//    ports_ = right.ports_;
//    executer_ = right.executer_;
//    identity_ = right.identity_;
//}
} // namespace items
} // namespace runtime
} // namespace rx_platform

