

/****************************************************************************
*
*  system\runtime\rx_blocks_templates.cpp
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


// rx_blocks_templates
#include "system/runtime/rx_blocks_templates.h"

#include "model/rx_model_algorithms.h"


namespace rx_platform {

namespace runtime {

namespace blocks {

namespace blocks_templates {

// Class rx_platform::runtime::blocks::blocks_templates::item_port_resolver 


void item_port_resolver::resolve_port ()
{
    if (my_state_ != resolver_state::idle)
        return;// not in right state

    if (port_path_.empty())
        return;

    auto resolve_result = rx_internal::model::algorithms::resolve_reference(port_path_, directories_);
    if (!resolve_result)
    {
        RUNTIME_LOG_ERROR("item_port_resolver", 100, "Unable to resolve port reference to "s + port_path_);
        return;
    }
    rx_node_id id = resolve_result.move_value();

    if (id.is_null())
        return;

    my_state_ = resolver_state::querying;

    std::function<rx_result_with<rx_port_impl_ptr>()> func = [id, this]
    {
        return resolve_port_sync(id);
    };
    rx_do_with_callback<rx_result_with<rx_port_impl_ptr>, rx_reference_ptr>(
        func, RX_DOMAIN_META, [this](rx_result_with<rx_port_impl_ptr>&& result)
        {
            if (my_state_ == resolver_state::querying)
            {
                if (result)
                {
                    my_state_ = resolver_state::same_thread;
                    my_port_ = result.move_value();
                    user_->port_connected(my_port_);
                }
                else
                {
                    my_state_ = resolver_state::idle;
                }
            }
        }, user_->get_reference());
}

rx_port_impl_ptr item_port_resolver::try_get_port ()
{
    return rx_port_impl_ptr::null_ptr;
}

rx_result item_port_resolver::init (runtime::runtime_start_context& ctx, resolver_user* user)
{
    port_path_ = ctx.structure.get_current_item().get_local_as<string_type>("Port", "");
    user_ = user;
    directories_.add_paths({ "/world" });
    resolve_port();
    return true;
}

rx_result_with<rx_port_impl_ptr> item_port_resolver::resolve_port_sync (const rx_node_id& id)
{
    if (my_state_ == resolver_state::querying)
    {
        auto port_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().get_runtime(id);
        if (!port_ptr)
        {
            return port_ptr.errors();
        }
        else
        {
            return port_ptr.move_value()->get_implementation();
        }
    }
    else
    {
        return "Wrong resolver state!";
    }
}

void item_port_resolver::deinit ()
{
    if (my_port_ && user_)
    {
        auto temp = my_port_;
        my_port_ = rx_port_impl_ptr::null_ptr;
        user_->port_disconnected(temp);
    }
}


// Class rx_platform::runtime::blocks::blocks_templates::resolver_user 


} // namespace blocks_templates
} // namespace blocks
} // namespace runtime
} // namespace rx_platform

