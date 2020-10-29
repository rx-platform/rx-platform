

/****************************************************************************
*
*  interfaces\rx_io_relations.cpp
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


// rx_io_relations
#include "interfaces/rx_io_relations.h"

#include "system/runtime/rx_runtime_holder.h"
#include "model/rx_meta_internals.h"
#include "system/server/rx_platform_item.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "system/runtime/rx_port_stack_construction.h"
#include "system/runtime/rx_port_stack_active.h"
#include "system/runtime/rx_port_stack_passive.h"


namespace rx_internal {

namespace interfaces {

namespace io_endpoints {

// Class rx_internal::interfaces::io_endpoints::port_stack_relation 

port_stack_relation::port_stack_relation()
{
}



void port_stack_relation::process_stack ()
{
}

rx_result port_stack_relation::initialize_relation (runtime::runtime_init_context& ctx)
{
    auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().get_runtime(ctx.meta.id);
    if (!result)
    {
        result.register_error(ctx.meta.get_full_path() + "is not a port!");
        return result.errors();
    }
    from_ = result.value();
    return true;
}

rx_result port_stack_relation::deinitialize_relation (runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result port_stack_relation::start_relation (runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result port_stack_relation::stop_relation (runtime::runtime_stop_context& ctx)
{
    return true;
}

rx_result_with<platform_item_ptr> port_stack_relation::resolve_runtime_sync (const rx_node_id& id)
{
    auto port_ptr = rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().get_port(id);
    if (!port_ptr)
    {
        return "Item not registered!";
    }
    else
    {
        if (port_ptr->get_instance_data().get_my_application() != from_->get_instance_data().get_my_application())
            return "Connected I/O ports must be in the same application.";
        auto ret = port_ptr->get_item_ptr();
        to_ = port_ptr;
        return ret;
    }
}

void port_stack_relation::relation_connected ()
{
    if (from_ && to_)
    {
        auto result = runtime::io_types::stack_build::stack_builder::connect_stack_top(to_, from_);
        if (result)
        {
            RUNTIME_LOG_DEBUG("port_stack_relation", 900, from_->meta_info().get_full_path() + "=>" + to_->meta_info().get_full_path() + " OK");
        }
        else
        {
            result.register_error(from_->meta_info().get_full_path() + "=>" + to_->meta_info().get_full_path() + " ERROR");
            RUNTIME_LOG_ERROR("port_stack_relation", 900, result.errors_line());
        }
    }
}

void port_stack_relation::relation_disconnected ()
{
    if (from_ && to_)
    {
        RUNTIME_LOG_DEBUG("port_stack_relation", 900, from_->meta_info().get_full_path() + "=>" + to_->meta_info().get_full_path() + " DISCONNECTED");
    }
}


} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal

