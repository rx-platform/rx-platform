

/****************************************************************************
*
*  system\runtime\rx_runtime_instance.cpp
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

#include "system/serialization/rx_ser.h"

// rx_runtime_instance
#include "system/runtime/rx_runtime_instance.h"

#include "rx_runtime_holder.h"
#include "api/rx_platform_api.h"
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_platform {

namespace runtime {

namespace items {

// Class rx_platform::runtime::items::object_instance_data 

object_instance_data::object_instance_data()
      : executer_(-1)
{
}



bool object_instance_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.write_id("domain", domain_id))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}

bool object_instance_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.read_id("domain", domain_id))
        return false;
    if (!stream.end_object())
        return false;
    return true;
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

rx_result object_instance_data::init_runtime (rx_object_ptr what, runtime::runtime_init_context& ctx)
{
    RX_ASSERT(what->get_instance_data().my_domain_);
    if (what->get_instance_data().my_domain_)
    {
        what->get_instance_data().executer_ = what->get_instance_data().my_domain_->get_executer();
    }
    else
    {

    }
    return true;
}

rx_result object_instance_data::start_runtime (rx_object_ptr what, runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result object_instance_data::deinit_runtime (rx_object_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result object_instance_data::stop_runtime (rx_object_ptr what, runtime::runtime_stop_context& ctx)
{
    return true;
}


// Class rx_platform::runtime::items::domain_instance_data 

domain_instance_data::domain_instance_data()
      : executer_(-1)
{
}



bool domain_instance_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.write_int("processor", processor))
        return false;
    if (!stream.write_id("app", app_id))
        return false;

    if (!stream.start_array("objects", objects.size()))
        return false;
    for (const auto& one : objects)
    {
        if (!stream.write_id("id", one))
            return false;
    }
    if (!stream.end_array())
        return false;

    if (!stream.end_object())
        return false;
    return true;
}

bool domain_instance_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.read_int("processor", processor))
        return false;
    if (!stream.read_id("app", app_id))
        return false;

    if (!stream.start_array("objects"))
        return false;
    while (!stream.array_end())
    {
        rx_node_id one;
        if (!stream.read_id("id", one))
            return false;
    }
    if (!stream.end_object())
        return false;
    return true;
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
    objects_.emplace(what->meta_info().get_id(), what);
}

void domain_instance_data::remove_object (rx_object_ptr what)
{
    auto it = objects_.find(what->meta_info().get_id());
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

rx_result domain_instance_data::init_runtime (rx_domain_ptr what, runtime::runtime_init_context& ctx)
{
    what->get_instance_data().executer_ = sys_runtime::platform_runtime_manager::instance().resolve_domain_processor(what->get_instance_data());
    return true;
}

rx_result domain_instance_data::start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result domain_instance_data::deinit_runtime (rx_domain_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result domain_instance_data::stop_runtime (rx_domain_ptr what, runtime::runtime_stop_context& ctx)
{
    return true;
}


// Class rx_platform::runtime::items::application_instance_data 

application_instance_data::application_instance_data()
      : executer_(-1)
{
}



bool application_instance_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.write_int("processor", processor))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}

bool application_instance_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.read_int("processor", processor))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}

void application_instance_data::get_ports (api::query_result& result)
{
    result.items.reserve(ports_.size());
    for (const auto& one : ports_)
    {
        result.items.emplace_back(api::query_result_detail{ rx_port, one.second->meta_info() });
    }
}

void application_instance_data::add_port (rx_port_ptr what)
{
    ports_.emplace(what->meta_info().get_id(), what);
}

void application_instance_data::add_domain (rx_domain_ptr what)
{
    // domains can be from other thread so do the locking
    locks::auto_lock_t<decltype(domains_lock_)> _(&domains_lock_);
    domains_.emplace(what->meta_info().get_id(), what);
}

void application_instance_data::remove_port (rx_port_ptr what)
{
    auto it = ports_.find(what->meta_info().get_id());
    if (it != ports_.end())
        ports_.erase(it);
}

void application_instance_data::remove_domain (rx_domain_ptr what)
{
    // domains can be from other thread so do the locking
    locks::auto_lock_t<decltype(domains_lock_)> _(&domains_lock_);
    auto it = domains_.find(what->meta_info().get_id());
    if (it != domains_.end())
        domains_.erase(it);
}

void application_instance_data::get_domains (api::query_result& result)
{
    result.items.reserve(domains_.size());
    for (const auto& one : domains_)
    {
        result.items.emplace_back(api::query_result_detail{ rx_domain, one.second->meta_info() });
    }
}

rx_result application_instance_data::init_runtime (rx_application_ptr what, runtime::runtime_init_context& ctx)
{
    what->get_instance_data().executer_ = sys_runtime::platform_runtime_manager::instance().resolve_app_processor(what->get_instance_data());
    return true;
}

rx_result application_instance_data::start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result application_instance_data::deinit_runtime (rx_application_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result application_instance_data::stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx)
{
    return true;
}


// Class rx_platform::runtime::items::port_instance_data 

port_instance_data::port_instance_data()
      : executer_(-1)
{
}



bool port_instance_data::serialize (base_meta_writer& stream, uint8_t type) const
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.write_id("app", app_id))
        return false;
    if (!stream.write_item_reference("next_up", up_port))
        return false;
    if (!stream.end_object())
        return false;
    return true;
}

bool port_instance_data::deserialize (base_meta_reader& stream, uint8_t type)
{
    if (!stream.start_object("instance"))
        return false;
    if (!stream.read_id("app", app_id))
        return false;
    if (!stream.read_item_reference("next_up", up_port))
        return false;
    if (!stream.end_object())
        return false;
    return true;
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

rx_result port_instance_data::init_runtime (rx_port_ptr what, runtime::runtime_init_context& ctx)
{
    RX_ASSERT(what->get_instance_data().my_application_);
    if (what->get_instance_data().my_application_)
    {
        what->get_instance_data().executer_ = what->get_instance_data().my_application_->get_executer();
    }
    else
    {

    }
    return true;
}

rx_result port_instance_data::start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result port_instance_data::deinit_runtime (rx_port_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result port_instance_data::stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx)
{
    return true;
}


} // namespace items
} // namespace runtime
} // namespace rx_platform

