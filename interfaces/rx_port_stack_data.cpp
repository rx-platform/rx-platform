

/****************************************************************************
*
*  interfaces\rx_port_stack_data.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_port_stack_data
#include "interfaces/rx_port_stack_data.h"

#include "system/runtime/rx_objbase.h"
#include "system/runtime/rx_runtime_holder.h"
#include "system/runtime/rx_holder_algorithms.h"

using port_algorithm_t = rx_platform::runtime::algorithms::runtime_holder_algorithms<meta::object_types::port_type>;


namespace rx_internal {

namespace interfaces {

namespace port_stack {

// Class rx_internal::interfaces::port_stack::port_active_map 


rx_result port_active_map::register_endpoint (rx_protocol_stack_endpoint* what, rx_port_ptr whose, rx_port_ptr owner)
{
    int16_t count;
    {
        locks::auto_lock_t _(&map_lock_);
        endpoints_map_.emplace(what, whose);
        count = (int16_t)endpoints_map_.size();
    }
    active_endpoints = count;
    if(whose)
        RUNTIME_LOG_TRACE("port_passive_map", 500, "Port " + owner->meta_info().get_full_path() + " registered endpoint for " + whose->meta_info().get_full_path());
    else
        RUNTIME_LOG_TRACE("port_passive_map", 500, "Port " + owner->meta_info().get_full_path() + " registered endpoint");
    //dump_active_map(owner->meta_info().get_full_path());
    return true;
}

rx_result port_active_map::unregister_endpoint (rx_protocol_stack_endpoint* what, rx_port_ptr owner)
{
    int16_t count;
    map_lock_.lock();
    auto it = endpoints_map_.find(what);
    if (it != endpoints_map_.end())
    {
        auto whose = it->second;
        endpoints_map_.erase(what);
        count = (int16_t)endpoints_map_.size();
        map_lock_.unlock();
        active_endpoints = count;
        if(whose)
            RUNTIME_LOG_TRACE("port_passive_map", 500, "Port " + owner->meta_info().get_full_path() + " removed endpoint for " + whose->meta_info().get_full_path());
        else
            RUNTIME_LOG_TRACE("port_passive_map", 500, "Port " + owner->meta_info().get_full_path() + " removed endpoint");
        return true;
    }
    else
    {
        map_lock_.unlock();
        RX_ASSERT(false);
        return "Entry not found!";
    }
}

void port_active_map::close_all_endpoints ()
{
    map_lock_.lock();
    if (!endpoints_map_.empty())
    {
        map_lock_.unlock();
        int counter = (int)endpoints_map_.size();
        while (counter>0)
        {
            rx_protocol_stack_endpoint* ep = nullptr;
            map_lock_.lock();
            if (!endpoints_map_.empty())
            {
                ep = endpoints_map_.begin()->first;
            }
            map_lock_.unlock();
            active_endpoints = 0;
            if (ep)
            {
                counter--;
                rx_close(ep, RX_PROTOCOL_OK);
            }
            else
                break;
        }
        RX_ASSERT(counter <= 0);
    }
    else
    {
        map_lock_.unlock();
    }
}


// Class rx_internal::interfaces::port_stack::port_buffers 

port_buffers::port_buffers()
      : buffer_back_capacity(0x100),
        buffer_front_capacity(0x10),
        buffer_discard_size(0x200),
        buffer_count(0),
        discard_buffer_count(0)
{
}



rx_result_with<io::rx_io_buffer> port_buffers::alloc_io_buffer (rx_port_ptr& whose)
{
    rx_io_buffer ret;
    port_buffers& buffers = whose->get_instance_data().stack_data.buffers;
    buffers.buffers_lock_.lock();
    if (buffers.free_buffers.empty())
    {
        buffers.buffers_lock_.unlock();
        auto result = rx_init_packet_buffer(&ret, buffers.buffer_back_capacity, buffers.buffer_front_capacity);
        if (result != RX_PROTOCOL_OK)
        {
            memzero(&ret, sizeof(ret));
            return "Out of memory";
        }
        buffers.buffer_count += 1;
    }
    else
    {
        buffers.free_buffers.back().detach(&ret);
        buffers.free_buffers.pop_back();
        buffers.buffers_lock_.unlock();
    }
    return ret;
}

void port_buffers::release_io_buffer (rx_port_ptr& whose, io::rx_io_buffer buff)
{
    port_buffers& buffers = whose->get_instance_data().stack_data.buffers;
    if (buff.size > buffers.buffer_discard_size)
    {// this buffer is to big so discard it
        buffers.discard_buffer_count += 1;
        buffers.buffer_count -= 1;
    }
    else
    {
        rx_reinit_packet_buffer(&buff);
        locks::auto_lock_t _(&buffers.buffers_lock_);
        buffers.free_buffers.emplace_back(std::move(buff));
    }
}


// Class rx_internal::interfaces::port_stack::port_build_map 

port_build_map::port_build_map()
      : stack_ready(false)
{
}



rx_result port_build_map::register_port (rx_port_ptr who, rx_port_ptr owner)
{
    auto it = registered_.find(who);
    if (it == registered_.end())
    {
        registered_.emplace(who);
        RUNTIME_LOG_DEBUG("port_build_map", 900, "Port " + who->meta_info().get_full_path() + " connected to " + owner->meta_info().get_full_path());
        return true;
    }
    else
    {
        RX_ASSERT(false);
        return "Port already connected!";
    }
}

rx_result port_build_map::unregister_port (rx_port_ptr who, rx_port_ptr owner)
{
    auto it = registered_.find(who);
    if (it != registered_.end())
    {
        registered_.erase(it);
        stack_ready = !registered_.empty();
        RUNTIME_LOG_DEBUG("port_build_map", 900, "Port " + who->meta_info().get_full_path() + " disconnected from " + owner->meta_info().get_full_path());
        return true;
    }
    else
    {
        RX_ASSERT(false);
        return "Port not connected!";
    }
}

std::vector<rx_port_ptr> port_build_map::get_registered ()
{
    std::vector<rx_port_ptr> ret;
    if (!registered_.empty())
    {
        ret.reserve(registered_.size());
        for (auto& one : registered_)
        {
            ret.push_back(one);
        }
    }
    return ret;
}


// Class rx_internal::interfaces::port_stack::port_passive_map 

port_passive_map::port_passive_map()
      : stack_binded(false)
{
}



rx_result port_passive_map::register_passive (rx_port_ptr who, io::any_address& local_addr, io::any_address& remote_addr, rx_port_ptr owner)
{
    auto it_inv = inverse_passive_map_.find(who);
    if (it_inv == inverse_passive_map_.end())
    {
        if (local_addr.is_null() && remote_addr.is_null())
        {
            data::runtime_values_data data;
            port_algorithm_t::collect_data(data, runtime_value_type::simple_runtime_value, *who);
            owner->get_implementation()->extract_bind_address(data, local_addr, remote_addr);
        }
        addr_pair_t addrs(local_addr, remote_addr);
        map_lock_.lock();
        auto it = passive_map_.find(addrs);
        if (it != passive_map_.end())
        {
            map_lock_.unlock();
            std::ostringstream ss;
            ss << "Port "
                << who->meta_info().get_full_path()
                << " not registered to "
                << owner->meta_info().get_full_path()
                << ". Duplicate registration at "
                << "["
                << addrs.first.to_string()
                << ","
                << addrs.second.to_string()
                << "]"
                ;
            RUNTIME_LOG_ERROR("port_passive_map", 200, ss.str());
            return "Address already binded.";
        }
        passive_map_.emplace(addrs, who);
        map_lock_.unlock();
        inverse_passive_map_.emplace(who, addrs);
        std::ostringstream ss;
        ss << "Port "
            << who->meta_info().get_full_path()
            << " registered at "
            << owner->meta_info().get_full_path()
            << "["
            << addrs.first.to_string()
            << ","
            << addrs.second.to_string()
            <<"]"
            ;
        RUNTIME_LOG_TRACE("port_passive_map", 900, ss.str());

        return true;
    }
    else
    {
        std::ostringstream ss;
        ss << "Port "
            << who->meta_info().get_full_path()
            << " not registered to "
            << owner->meta_info().get_full_path()
            << ". Duplicate registration."
            ;
        RUNTIME_LOG_CRITICAL("port_passive_map", 200, ss.str());
        RX_ASSERT(false);
        return "Already registered";
    }
}

rx_result port_passive_map::unregister_passive (rx_port_ptr who, rx_port_ptr owner)
{
    RX_ASSERT(who != owner);
    auto it_inv = inverse_passive_map_.find(who);
    if (it_inv != inverse_passive_map_.end())
    {
        map_lock_.lock();
        addr_pair_t addrs(it_inv->second);
        auto it = passive_map_.find(addrs);
        if (it != passive_map_.end())
        {
            passive_map_.erase(it);
        }
        else
        {
            RX_ASSERT(false);
        }
        map_lock_.unlock();
        inverse_passive_map_.erase(it_inv);

        RUNTIME_LOG_DEBUG("port_passive_map", 900, "Port " + who->meta_info().get_full_path() + " unregistered passive from " + owner->meta_info().get_full_path());
        if (inverse_passive_map_.empty())
        {
            owner->get_implementation()->stop_passive();
        }
        return true;
    }
    else
    {
        RX_ASSERT(false);
        return "Not registered";
    }
}

rx_port_ptr port_passive_map::get_binded_port (const io::any_address& local_addr, const io::any_address& remote_addr)
{
    locks::auto_lock_t _(&map_lock_);
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

bool port_passive_map::empty () const
{
    return inverse_passive_map_.empty();
}


// Class rx_internal::interfaces::port_stack::port_stack_data 

port_stack_data::~port_stack_data()
{
}



rx_result port_stack_data::init_runtime_data (runtime::runtime_init_context& ctx)
{
    buffers.buffer_back_capacity = ctx.get_item_static<size_t>("Options.BuffBackCapacity", 0x100);
    buffers.buffer_front_capacity = ctx.get_item_static<size_t>("Options.BuffFrontCapacity", 0x10);
    buffers.buffer_discard_size = ctx.get_item_static<size_t>("Options.BuffDiscardSize", 0x1000);
    buffers.buffer_count.bind("Status.Buffers", ctx);
    buffers.discard_buffer_count.bind("Status.DropedBuffers", ctx);
    passive_map.stack_binded.bind("Status.Binded", ctx);
    build_map.stack_ready.bind("Status.Assembled", ctx);
    active_map.active_endpoints.bind("Status.Endpoints", ctx);
    return true;
}


} // namespace port_stack
} // namespace interfaces
} // namespace rx_internal

