

/****************************************************************************
*
*  system\runtime\rx_port_stack_data.cpp
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


// rx_port_stack_data
#include "system/runtime/rx_port_stack_data.h"

#include "rx_objbase.h"
#include "rx_runtime_holder.h"


namespace rx_platform {

namespace runtime {

namespace io_types {

// Class rx_platform::runtime::io_types::port_passive_map 


rx_result port_passive_map::register_port (rx_port_ptr who, rx_port_ptr owner)
{
    for (auto& one : registered_)
    {
        if (one.port == who)
        {
            RX_ASSERT(false);
            return "Already registered";
        }
    }
    registered_.emplace_back(registered_port_data{ who, addr_pair_t(), false });
    RUNTIME_LOG_DEBUG("port_passive_map", 900, "Port " + who->meta_info().get_full_path() + " registered to " + owner->meta_info().get_full_path());
    return true;
}

rx_result port_passive_map::register_passive (rx_port_ptr who, const io::any_address& local_addr, const io::any_address& remote_addr, rx_port_ptr owner)
{
    for (auto it = registered_.begin(); it != registered_.end(); it++)
    {
        if (it->port == who)
        {
            if (!it->binded)
            {// add binded
                addr_pair_t addrs(local_addr, remote_addr);
                auto it_port = passive_map_.find(addrs);
                if (it_port != passive_map_.end())
                    return "Address already binded.";
                passive_map_.emplace(addrs, who);
                it->binded = true;
                it->passive_entry = addrs;
                RUNTIME_LOG_DEBUG("port_passive_map", 900, "Port " + who->meta_info().get_full_path() + " registered passive to " + owner->meta_info().get_full_path());
                return true;
            }
            else
                return "Port already binded.";
        }
    }
    RX_ASSERT(false);
    return "Port not registered.";
}

rx_result port_passive_map::unregister_passive (rx_port_ptr who, rx_port_ptr owner)
{
    for (auto it = registered_.begin(); it != registered_.end(); it++)
    {
        if (it->port == who && it->binded)
        {// remove passive entry
            auto it_map = passive_map_.find(it->passive_entry);
            if (it_map != passive_map_.end())
            {
                passive_map_.erase(it_map);
                it->binded = false;
                RUNTIME_LOG_DEBUG("port_passive_map", 900, "Port " + who->meta_info().get_full_path() + " unregistered passive from " + owner->meta_info().get_full_path());
                return true;
            }
            else
                break;
        }
    }
    RX_ASSERT(false);
    return "Not registered";
}

rx_result port_passive_map::unregister_port (rx_port_ptr who, rx_port_ptr owner)
{
    for (auto it = registered_.begin(); it != registered_.end(); it++)
    {
        if (it->port == who)
        {
            if (it->binded)
            {// remove binded also
                auto it_map = passive_map_.find(it->passive_entry);
                if (it_map != passive_map_.end())
                {
                    it->binded = false;
                    passive_map_.erase(it_map);
                    RUNTIME_LOG_DEBUG("port_passive_map", 900, "Port " + who->meta_info().get_full_path() + " unregistered passive from " + owner->meta_info().get_full_path());
                }
                else
                    RX_ASSERT(false);
            }
            registered_.erase(it);
            RUNTIME_LOG_DEBUG("port_passive_map", 900, "Port " + who->meta_info().get_full_path() + " unregistered from " + owner->meta_info().get_full_path());
            return true;
        }
    }
    RX_ASSERT(false);
    return "Not registered";
}

rx_port_ptr port_passive_map::get_binded_port (const io::any_address& local_addr, const io::any_address& remote_addr)
{
    addr_pair_t addr(local_addr, remote_addr);
    auto it_port = passive_map_.find(addr);
    if (it_port != passive_map_.end())
        return it_port->second;
    if (!remote_addr.is_null())
    {
        addr = addr_pair_t(local_addr, nullptr);
        auto it_port = passive_map_.find(addr);
        if (it_port != passive_map_.end())
            return it_port->second;
    }
    if (!local_addr.is_null())
    {
        addr = addr_pair_t(nullptr, remote_addr);
        auto it_port = passive_map_.find(addr);
        if (it_port != passive_map_.end())
            return it_port->second;
    }
    if (!remote_addr.is_null() && !local_addr.is_null())
    {
        addr = addr_pair_t();
        auto it_port = passive_map_.find(addr);
        if (it_port != passive_map_.end())
            return it_port->second;
    }
    return rx_port_ptr::null_ptr;
}

std::vector<rx_port_ptr> port_passive_map::get_registered ()
{
    std::vector<rx_port_ptr> ret;
    if (!registered_.empty())
    {
        ret.reserve(registered_.size());
        for (auto& one : registered_)
        {
            ret.push_back(one.port);
        }
    }
    return ret;
}

std::vector<rx_port_ptr> port_passive_map::get_binded ()
{
    std::vector<rx_port_ptr> ret;
    if (!passive_map_.empty())
    {
        ret.reserve(passive_map_.size());
        for (auto& one : passive_map_)
        {
            ret.push_back(one.second);
        }
    }
    return ret;
}


// Class rx_platform::runtime::io_types::port_stack_data 


} // namespace io_types
} // namespace runtime
} // namespace rx_platform

