

/****************************************************************************
*
*  system\runtime\rx_io_relations.cpp
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
#include "system/runtime/rx_io_relations.h"

#include "model/rx_meta_internals.h"
#include "system/server/rx_platform_item.h"


namespace rx_platform {

namespace runtime {

namespace relations {

// Class rx_platform::runtime::relations::port_up_relation 

port_up_relation::port_up_relation (rx_port_ptr my_port)
    : from_(my_port)
{
}



void port_up_relation::process_stack ()
{
}

rx_result port_up_relation::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto ret = relation_runtime::initialize_runtime(ctx);
    return ret;
}

rx_result port_up_relation::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    auto ret = relation_runtime::deinitialize_runtime(ctx);
    return ret;
}

rx_result port_up_relation::start_runtime (runtime::runtime_start_context& ctx)
{
    auto ret = relation_runtime::start_runtime(ctx);
    return ret;
}

rx_result port_up_relation::stop_runtime (runtime::runtime_stop_context& ctx)
{
    auto ret = relation_runtime::stop_runtime(ctx);
    return ret;
}

rx_result_with<platform_item_ptr> port_up_relation::resolve_runtime_sync (const rx_node_id& id)
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

void port_up_relation::relation_connected ()
{
    if (from_ && to_)
    {
        from_->get_implementation()->connect_up_stack(to_->get_implementation());
    }
}

void port_up_relation::relation_disconnected ()
{
}


} // namespace relations
} // namespace runtime
} // namespace rx_platform

