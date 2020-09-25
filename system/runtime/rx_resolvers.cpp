

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
using namespace rx_internal::sys_runtime;


namespace rx_platform {

namespace runtime {

namespace resolvers {

// Parameterized Class rx_platform::runtime::resolvers::runtime_resolver 


template <class typeT>
rx_result runtime_resolver<typeT>::start_resolver (const rx_item_reference& ref, resolver_user<typeT>* user, ns::rx_directory_resolver* dirs)
{
    user_ = user;
    directories_ = dirs;
    runtime_reference_ = ref;
    my_state_ = resolver_state::waiting;
    rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().register_subscriber(runtime_reference_, this);
    return true;
}

template <class typeT>
void runtime_resolver<typeT>::stop_resolver ()
{
    my_state_ = resolver_state::stopped;
    rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().unregister_subscriber(runtime_reference_, this);
    if (user_)
    {
        user_->runtime_disconnected();
    }
}

template <class typeT>
void runtime_resolver<typeT>::runtime_appeared (platform_item_ptr&& item)
{
    if (my_state_ != resolver_state::waiting && my_state_!=resolver_state::querying)
        return;

    if (runtime_reference_.is_null())
        return;

    if (runtime_reference_.is_node_id())
    {
        auto rt_ptr = platform_runtime_manager::instance().get_cache().get_runtime<typeT>(runtime_reference_.get_node_id());
        if (rt_ptr)
        {
            my_state_ = resolver_state::resolved;
            {
                resolved_id_ = runtime_reference_.get_node_id();
                if (!user_->runtime_connected(std::move(item), rt_ptr->get_implementation()))
                {
                    my_state_ = resolver_state::waiting;
                    RUNTIME_LOG_ERROR("item_port_resolver", 100, "Resolver user returned error for item "s + runtime_reference_.to_string());
                }
            }
        }
    }
    else
    {
        if (my_state_ != resolver_state::querying)
            my_state_ = resolver_state::querying;

        auto callback = rx_result_with_callback<resolve_result>(user_->get_reference(), [this](rx_result_with<resolve_result>&& result)
            {
                if (my_state_ == resolver_state::querying)
                {
                    if (result)
                    {
                        my_state_ = resolver_state::resolved;
                        resolved_id_ = result.value().item->meta_info().id;
                        if (!user_->runtime_connected(std::move(result.value().item), std::move(result.value().implementation)))
                        {
                            my_state_ = resolver_state::waiting;
                            RUNTIME_LOG_ERROR("item_port_resolver", 100, "Resolver user returned error for item "s + runtime_reference_.to_string());
                        }
                    }
                    else
                    {
                        my_state_ = resolver_state::waiting;
                        RUNTIME_LOG_ERROR("item_port_resolver", 100, "Unable to resolve port reference to "s + runtime_reference_.to_string());
                    }
                }
            });

        // this one is path based so do all the things
        rx_do_with_callback(RX_DOMAIN_META, user_->get_reference(),
            [this](rx_item_reference ref) -> rx_result_with<resolve_result>
            {
                auto ref_result = rx_internal::model::algorithms::resolve_reference(ref, *directories_);
                if (!ref_result)
                {
                    return ref_result.errors();
                }
                rx_node_id id = ref_result.move_value();

                auto rt_result = platform_runtime_manager::instance().get_cache().get_runtime<typeT>(id);
                if (!rt_result)
                {
                    return "Item not registered as running runtime.";
                }
                else
                {
                    resolve_result result;
                    result.item = rt_result->get_item_ptr();
                    result.implementation = rt_result->get_implementation();
                    return result;
                }
            }, std::move(callback), runtime_reference_);
    }
}

template <class typeT>
void runtime_resolver<typeT>::runtime_destroyed (const rx_node_id& id)
{
    if (my_state_ != resolver_state::resolved)
        return; // we're in the wrong state
    if (id == resolved_id_)
    {
        my_state_ = resolver_state::waiting;
        resolved_id_ = rx_node_id::null_id;
        if (user_)
        {
            user_->runtime_disconnected();
        }
    }
}

template <class typeT>
rx_reference_ptr runtime_resolver<typeT>::get_reference ()
{
    if (user_)
        return user_->get_reference();
    else
        return rx_reference_ptr();
}

// explicit instantiations
template class resolver_user<meta::object_types::object_type>;
template class resolver_user<meta::object_types::port_type>;
template class resolver_user<meta::object_types::domain_type>;
template class resolver_user<meta::object_types::application_type>;

template class runtime_resolver<meta::object_types::object_type>;
template class runtime_resolver<meta::object_types::port_type>;
template class runtime_resolver<meta::object_types::domain_type>;
template class runtime_resolver<meta::object_types::application_type>;
// Parameterized Class rx_platform::runtime::resolvers::resolver_user 


// Class rx_platform::runtime::resolvers::runtime_subscriber 


// Class rx_platform::runtime::resolvers::runtime_item_resolver 


rx_result runtime_item_resolver::start_resolver (const rx_item_reference& ref, item_resolver_user* user, ns::rx_directory_resolver* dirs)
{
    user_ = user;
    directories_ = dirs;
    runtime_reference_ = ref;
    my_state_ = resolver_state::waiting;
    rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().register_subscriber(runtime_reference_, this);
    return true;
}

void runtime_item_resolver::stop_resolver ()
{
    my_state_ = resolver_state::stopped;
    rx_internal::sys_runtime::platform_runtime_manager::instance().get_cache().unregister_subscriber(runtime_reference_, this);
    if (user_)
    {
        user_->runtime_disconnected();
    }
}

void runtime_item_resolver::runtime_appeared (platform_item_ptr&& item)
{
    if (my_state_ != resolver_state::waiting && my_state_ != resolver_state::querying)
        return;

    if (runtime_reference_.is_null())
        return;

    if (runtime_reference_.is_node_id())
    {
        my_state_ = resolver_state::resolved;
        resolved_id_ = runtime_reference_.get_node_id();
        if (!user_->runtime_connected(std::move(item)))
        {
            my_state_ = resolver_state::waiting;
            RUNTIME_LOG_ERROR("item_port_resolver", 100, "Resolver user returned error for item "s + runtime_reference_.to_string());
        }
    }
    else
    {
        if (my_state_ != resolver_state::querying)
            my_state_ = resolver_state::querying;

        auto callback = rx_result_with_callback<platform_item_ptr>(user_->get_reference(), [this](rx_result_with<platform_item_ptr>&& result)
            {
                if (my_state_ == resolver_state::querying)
                {
                    if (result)
                    {
                        my_state_ = resolver_state::resolved;
                        resolved_id_ = result.value()->meta_info().id;
                        if (!user_->runtime_connected(std::move(result.value())))
                        {
                            my_state_ = resolver_state::waiting;
                            RUNTIME_LOG_ERROR("item_port_resolver", 100, "Resolver user returned error for item "s + runtime_reference_.to_string());
                        }
                    }
                    else
                    {
                        my_state_ = resolver_state::waiting;
                        RUNTIME_LOG_ERROR("item_port_resolver", 100, "Unable to resolve port reference to "s + runtime_reference_.to_string());
                    }
                }
            });
        // this one is path based so do all the things
        rx_do_with_callback(RX_DOMAIN_META, user_->get_reference(),
            [this](rx_item_reference ref)-> rx_result_with<platform_item_ptr>
            {
                auto ref_result = rx_internal::model::algorithms::resolve_reference(ref, *directories_);
                if (!ref_result)
                {
                    return ref_result.errors();
                }
                else
                {
                    auto item = platform_runtime_manager::instance().get_cache().get_item(ref_result.value());
                    if (!item)
                        return "Item not registered as running runtime.";

                    return item;
                }
            }, std::move(callback), runtime_reference_);
    }    
}

void runtime_item_resolver::runtime_destroyed (const rx_node_id& id)
{
    if (my_state_ != resolver_state::resolved)
        return; // we're in the wrong state
    if (id == resolved_id_)
    {
        my_state_ = resolver_state::waiting;
        resolved_id_ = rx_node_id::null_id;
        if (user_)
        {
            user_->runtime_disconnected();
        }
    }
}

rx_reference_ptr runtime_item_resolver::get_reference ()
{
    if (user_)
        return user_->get_reference();
    else
        return rx_reference_ptr();
}


// Class rx_platform::runtime::resolvers::item_resolver_user 


} // namespace resolvers
} // namespace runtime
} // namespace rx_platform

