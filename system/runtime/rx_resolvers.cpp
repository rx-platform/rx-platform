

/****************************************************************************
*
*  system\runtime\rx_resolvers.cpp
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


// rx_resolvers
#include "system/runtime/rx_resolvers.h"

#include "model/rx_model_algorithms.h"
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_platform {

namespace runtime {

namespace resolvers {

// Class rx_platform::runtime::resolvers::item_port_resolver 


rx_result item_port_resolver::init (const rx_item_reference& ref, port_resolver_user* user, ns::rx_directory_resolver* dirs)
{
    user_ = user;
    directories_ = dirs;
    port_reference_ = ref;
    my_state_ = resolver_state::waiting;
    rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().register_subscriber(port_reference_, this);
    return true;
}

void item_port_resolver::deinit ()
{
    my_state_ = resolver_state::stopped;
    rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().unregister_subscriber(port_reference_, this);
    if (user_)
    {
        user_->port_disconnected();
    }
}

void item_port_resolver::runtime_appeared (platform_item_ptr&& item)
{
    if (my_state_ != resolver_state::waiting && my_state_!=resolver_state::querying)
        return;

    if (port_reference_.is_null() || (item && item->get_type_id() != rx_item_type::rx_port))
        return;

    if (my_state_ != resolver_state::querying)
        my_state_ = resolver_state::querying;

    std::function<rx_result_with<resolve_result>(rx_item_reference)> func = [this](rx_item_reference ref)
        -> rx_result_with<resolve_result>
    {
        auto ref_result = rx_internal::model::algorithms::resolve_reference(ref, *directories_);
        if (!ref_result)
        {
            return ref_result.errors();
        }
        rx_node_id id = ref_result.move_value();

        auto port_ptr = rx_internal::model::platform_types_manager::instance().get_type_repository<port_type>().get_runtime(id);
        if (!port_ptr)
        {
            return port_ptr.errors();
        }
        else
        {
            resolve_result result;
            result.id = id;
            result.port = port_ptr.value()->get_implementation();
            return result;
        }
    };
    rx_do_with_callback<rx_result_with<resolve_result>, rx_reference_ptr, rx_item_reference>(
        func, RX_DOMAIN_META, [this](rx_result_with<resolve_result>&& result)
        {
            if (my_state_ == resolver_state::querying)
            {
                if (result)
                {
                    resolved_id_ = result.value().id;
                    my_state_ = resolver_state::same_thread;
                    if(!user_->port_connected(std::move(result.value().port), std::move(resolved_id_)))
                        my_state_ = resolver_state::waiting;
                }
                else
                {
                    my_state_ = resolver_state::waiting;
                    RUNTIME_LOG_ERROR("item_port_resolver", 100, "Unable to resolve port reference to "s + port_reference_.to_string());
                }
            }
        }, user_->get_reference(), port_reference_);
}

void item_port_resolver::runtime_destroyed (const rx_node_id& id)
{
    if (my_state_ != resolver_state::same_thread && my_state_ != resolver_state::other_thread)
        return; // we're in the wrong state
    if (id == resolved_id_)
    {
        if (user_)
        {
            user_->port_disconnected();
        }
        my_state_ = resolver_state::waiting;
    }
}

rx_reference_ptr item_port_resolver::get_reference ()
{
    if (user_)
        return user_->get_reference();
    else
        return rx_reference_ptr();
}


// Class rx_platform::runtime::resolvers::port_resolver_user 


// Class rx_platform::runtime::resolvers::runtime_subscriber 


} // namespace resolvers
} // namespace runtime
} // namespace rx_platform

