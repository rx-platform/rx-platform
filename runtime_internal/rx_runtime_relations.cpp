

/****************************************************************************
*
*  runtime_internal\rx_runtime_relations.cpp
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


// rx_runtime_relations
#include "runtime_internal/rx_runtime_relations.h"

#include "system/runtime/rx_runtime_holder.h"
#include "system/server/rx_platform_item.h"
#include "model/rx_meta_internals.h"
using namespace rx_platform::runtime;


namespace rx_internal {

namespace sys_runtime {

namespace relations_runtime {
rx_result register_internal_relations_constructors()
{
    auto result = model::platform_types_manager::instance().get_relations_repository().register_constructor(
        RX_NS_DOMAIN_RELATION_ID, [] {
            return rx_create_reference<domain_relation>();
        });
    if(!result)
        return result;
    result = model::platform_types_manager::instance().get_relations_repository().register_constructor(
        RX_NS_APPLICATION_RELATION_ID, [] {
            return rx_create_reference<application_relation>();
        });
    if (!result)
        return result;
    result = model::platform_types_manager::instance().get_relations_repository().register_constructor(
        RX_NS_PORT_APPLICATION_RELATION_ID, [] {
            return rx_create_reference<port_app_relation>();
        });
    if (!result)
        return result;

    return result;
}

// Class rx_internal::sys_runtime::relations_runtime::domain_relation 


rx_result domain_relation::initialize_relation (runtime::runtime_init_context& ctx)
{
    return true;
}

rx_result domain_relation::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result domain_relation::start_relation (runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result domain_relation::stop_relation (runtime::runtime_stop_context& ctx)
{
    return true;
}

rx_result_with<platform_item_ptr> domain_relation::resolve_runtime_sync (const rx_node_id& id)
{
    auto domain_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(id);
    if (!domain_ptr)
    {
        return domain_ptr.errors();
    }
    else
    {
        auto ret = domain_ptr.value()->get_item_ptr();
        to_ = domain_ptr.move_value();
        return ret;
    }
}

void domain_relation::relation_connected ()
{
    if (from_ && to_)
    {
        RUNTIME_LOG_DEBUG("domain_relation", 900, from_->meta_info().get_full_path() + " connected to domain " + to_->meta_info().get_full_path());
        meta::meta_data meta;
        //to_->get_implementation()->push(from_->get_implementation(), meta);
    }
}

void domain_relation::relation_disconnected ()
{
}

rx_result domain_relation::get_implicit_reference (runtime::runtime_init_context& ctx, rx_item_reference& ref)
{
    auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<object_type>().get_runtime(ctx.meta.id);
    if (!result)
    {
        result.register_error(ctx.meta.get_full_path() + " is not a object!");
        return result.errors();
    }
    from_ = result.value();
    ref = from_->get_instance_data().get_data().domain_id;
    return true;
}


// Class rx_internal::sys_runtime::relations_runtime::application_relation 


rx_result application_relation::initialize_relation (runtime::runtime_init_context& ctx)
{
    return true;
}

rx_result application_relation::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result application_relation::start_relation (runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result application_relation::stop_relation (runtime::runtime_stop_context& ctx)
{
    return true;
}

rx_result_with<platform_item_ptr> application_relation::resolve_runtime_sync (const rx_node_id& id)
{
    auto app_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(id);
    if (!app_ptr)
    {
        return app_ptr.errors();
    }
    else
    {
        auto ret = app_ptr.value()->get_item_ptr();
        to_ = app_ptr.move_value();
        return ret;
    }
}

void application_relation::relation_connected ()
{
    if (from_ && to_)
    {
        RUNTIME_LOG_DEBUG("application_relation", 900, from_->meta_info().get_full_path() + " connected to application " + to_->meta_info().get_full_path());
        meta::meta_data meta;
        //to_->get_implementation()->push(from_->get_implementation(), meta);
    }
}

void application_relation::relation_disconnected ()
{
}

rx_result application_relation::get_implicit_reference (runtime::runtime_init_context& ctx, rx_item_reference& ref)
{
    auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(ctx.meta.id);
    if (!result)
    {
        result.register_error(ctx.meta.get_full_path() + " is not an application!");
        return result.errors();
    }
    from_ = result.value();
    ref = from_->get_instance_data().get_data().app_id;
    return true;
}


// Class rx_internal::sys_runtime::relations_runtime::port_app_relation 


rx_result port_app_relation::initialize_relation (runtime::runtime_init_context& ctx)
{
    return true;
}

rx_result port_app_relation::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result port_app_relation::start_relation (runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result port_app_relation::stop_relation (runtime::runtime_stop_context& ctx)
{
    return true;
}

rx_result_with<platform_item_ptr> port_app_relation::resolve_runtime_sync (const rx_node_id& id)
{
    auto app_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(id);
    if (!app_ptr)
    {
        return app_ptr.errors();
    }
    else
    {
        auto ret = app_ptr.value()->get_item_ptr();
        to_ = app_ptr.move_value();
        return ret;
    }
}

void port_app_relation::relation_connected ()
{
    if (from_ && to_)
    {
        RUNTIME_LOG_DEBUG("port_app_relation", 900, from_->meta_info().get_full_path() + " connected to application " + to_->meta_info().get_full_path());
        meta::meta_data meta;
        //to_->get_implementation()->push(from_->get_implementation(), meta);
    }
}

void port_app_relation::relation_disconnected ()
{
}

rx_result port_app_relation::get_implicit_reference (runtime::runtime_init_context& ctx, rx_item_reference& ref)
{
    auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().get_runtime(ctx.meta.id);
    if (!result)
    {
        result.register_error(ctx.meta.get_full_path() + " is not a port!");
        return result.errors();
    }
    from_ = result.value();
    ref = from_->get_instance_data().get_data().app_id;
    return true;
}


} // namespace relations_runtime
} // namespace sys_runtime
} // namespace rx_internal

