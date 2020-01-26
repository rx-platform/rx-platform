

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


namespace rx_platform {

namespace runtime {

namespace items {

// Class rx_platform::runtime::items::object_instance_data 


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

bool object_instance_data::connect_domain (rx_domain_ptr&& domain, const rx_object_ptr& whose)
{
    my_domain_ = std::move(domain);
    my_domain_->get_instance_data().add_object(whose);
    return true;
}


// Class rx_platform::runtime::items::domain_instance_data 


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

bool domain_instance_data::connect_application (rx_application_ptr&& app, const rx_domain_ptr& whose)
{
    my_application_ = std::move(app);
    my_application_->get_instance_data().add_domain(whose);
    return true;
}


// Class rx_platform::runtime::items::application_instance_data 


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
        if (one)
            result.items.emplace_back(api::query_result_detail{ rx_port, one->meta_info() });
    }
}

void application_instance_data::add_port (rx_port_ptr what)
{
    for (auto& one : ports_)
    {
        if (!one)
        {
            one = what;
            return;
        }
    }
    ports_.emplace_back(what);
}

void application_instance_data::add_domain (rx_domain_ptr what)
{
    for (auto& one : domains_)
    {
        if (!one)
        {
            one = what;
            return;
        }
    }
    domains_.emplace_back(what);
}

void application_instance_data::remove_port (rx_port_ptr what)
{
    for (auto& one : ports_)
    {
        if (one == what)
        {
            one = rx_port_ptr::null_ptr;
            return;
        }
    }
}

void application_instance_data::remove_domain (rx_domain_ptr what)
{
    for (auto& one : domains_)
    {
        if (one == what)
        {
            one = rx_domain_ptr::null_ptr;
            return;
        }
    }
}

void application_instance_data::get_domains (api::query_result& result)
{
    result.items.reserve(domains_.size());
    for (const auto& one : domains_)
    {
        if (one)
            result.items.emplace_back(api::query_result_detail{ rx_domain, one->meta_info() });
    }
}


// Class rx_platform::runtime::items::port_instance_data 


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

void port_instance_data::connect_application (rx_application_ptr&& app, const rx_port_ptr& whose)
{
    my_application_ = std::move(app);
    my_application_->get_instance_data().add_port(whose);
}


} // namespace items
} // namespace runtime
} // namespace rx_platform

