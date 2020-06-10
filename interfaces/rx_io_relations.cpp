

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

#include "model/rx_meta_internals.h"
#include "system/server/rx_platform_item.h"


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

rx_result port_stack_relation::initialize_relation (runtime::runtime_init_context& ctx, rx_item_reference& ref)
{
    auto result = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().get_runtime(ctx.meta.get_id());
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
    auto port_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().get_runtime(id);
    if (!port_ptr)
    {
        return port_ptr.errors();
    }
    else
    {
        if (port_ptr.value()->get_instance_data().get_my_application() != from_->get_instance_data().get_my_application())
            return "Connected I/O ports must be in the same application.";
        auto ret = port_ptr.value()->get_item_ptr();
        to_ = port_ptr.move_value();
        return ret;
    }
}

void port_stack_relation::relation_connected ()
{
    if (from_ && to_)
    {
        RUNTIME_LOG_DEBUG("port_stack_relation", 900, from_->meta_info().get_full_path() + " connected to port " + to_->meta_info().get_full_path());
        meta::meta_data meta;
        to_->get_implementation()->push(from_->get_implementation(), meta);
    }
}

void port_stack_relation::relation_disconnected ()
{
}


} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal

