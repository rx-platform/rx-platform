

/****************************************************************************
*
*  interfaces\rx_ethernet.cpp
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


// rx_ethernet
#include "interfaces/rx_ethernet.h"

#include "rx_endpoints.h"


namespace rx_internal {

namespace interfaces {

namespace ethernet {

// Class rx_internal::interfaces::ethernet::ethernet_endpoint 

ethernet_endpoint::ethernet_endpoint()
      : my_port_(nullptr),
        identity_(security::security_context_ptr::null_ptr),
        handle_(0),
        stop_(false)
    , thread("EthernetWorker", 0)
{
    ITF_LOG_DEBUG("ethernet_endpoint", 200, "Ethernet endpoint created.");
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);

    mine_entry->send_function = &ethernet_endpoint::send_function;
}


ethernet_endpoint::~ethernet_endpoint()
{
    ITF_LOG_DEBUG("ethernet_endpoint", 200, "Ethernet endpoint destroyed.");
}



rx_protocol_result_t ethernet_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    ethernet_endpoint* self = reinterpret_cast<ethernet_endpoint*>(reference->user_data);
    if (self->my_port_)
    {
        auto ret = self->send_packet(packet);
        if (ret == RX_PROTOCOL_OK)
        {
            self->my_port_->status.sent_packet(packet.buffer->size);
        }
        return ret;
    }
    else
        return RX_PROTOCOL_CLOSED;
}

rx_protocol_stack_endpoint* ethernet_endpoint::get_stack_endpoint ()
{
    return &stack_endpoint_;
}

runtime::items::port_runtime* ethernet_endpoint::get_port ()
{
    return my_port_;
}

rx_protocol_result_t ethernet_endpoint::send_packet (send_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

void ethernet_endpoint::release_buffer (buffer_ptr what)
{
    return my_port_->release_buffer(what);
}

rx_result ethernet_endpoint::open (const string_type& port_name, security::security_context_ptr identity, ethernet_port* port, const std::vector<uint16_t>& ether_types)
{
    RX_ASSERT(handle_ == 0);
    my_port_ = port;
    if (!rx_create_ethernet_socket(port_name.c_str(), &handle_))
    {
        char buff[0x100];
        auto code = rx_last_os_error("Error opening port", buff, sizeof(buff));
        my_port_ = nullptr;
        return buff;
    }
    stop_ = false;
    start(RX_PRIORITY_REALTIME);
    return true;
}

rx_result ethernet_endpoint::close ()
{
    if (my_port_)
    {
        stop_ = true;
        wait_handle();
        if (handle_)
        {
            rx_close_ethernet_socket(handle_);
            handle_ = 0;
        }
        my_port_ = nullptr;
    }
    return true;
}

bool ethernet_endpoint::is_connected () const
{
    return my_port_;
}

uint32_t ethernet_endpoint::handler ()
{
    string_type info_head = "Ethernet worker for card ";
    info_head += port_name_;
    ITF_LOG_INFO("ethernet_endpoint", 800, info_head + " started.");

    uint8_t* buffer = nullptr;
    size_t psize = 0;
    uint32_t ret;
    int more;
    timeval ts;

    while (!stop_)
    {
        do
        {
            ret = rx_recive_ethernet_packet(handle_, &buffer, &psize, &more, &ts);
            if (ret && psize)
            {
            }

        } while (more);

        rx_ms_sleep(1);
    }

    ITF_LOG_INFO("ethernet_endpoint", 800, info_head + " stopped.");
    return 0;
}


// Class rx_internal::interfaces::ethernet::ethernet_port 

ethernet_port::ethernet_port()
{
}



rx_result ethernet_port::initialize_runtime (runtime::runtime_init_context& ctx)
{

    auto result = status.initialize(ctx);

    port_name_ = rx_gate::instance().resolve_ethernet_alias(ctx.get_item_static("Options.Port", ""s));
    ether_types_ = ctx.get_item_static("Options.EtherTypes", std::vector<uint16_t>());

    return true;
}

rx_result ethernet_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    endpoint_ = std::make_unique<ethernet_endpoint>();
    auto sec_result = create_security_context();
    if (!sec_result)
    {
        sec_result.register_error("Unable to create security context");
        return sec_result.errors();
    }
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
    {
        ethernet_endpoint* whose = reinterpret_cast<ethernet_endpoint*>(entry->user_data);
        whose->get_port()->disconnect_stack_endpoint(entry);
    };
    auto result = endpoint_->open(port_name_, sec_result.value(), this, ether_types_);
    if (!result)
    {
        stop_passive();
        return result.errors();
    }
    result = add_stack_endpoint(endpoint_->get_stack_endpoint(), local_address, remote_address);
    if (!result)
    {
        stop_passive();
        return result.errors();
    }
    return true;
}

rx_result_with<port_connect_result> ethernet_port::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
    endpoint_ = std::make_unique<ethernet_endpoint>();
    auto sec_result = create_security_context();
    if (!sec_result)
    {
        sec_result.register_error("Unable to create security context");
        return sec_result.errors();
    }
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
    {
        ethernet_endpoint* whose = reinterpret_cast<ethernet_endpoint*>(entry->user_data);
        auto port = whose->get_port();
        if(port)
            port->disconnect_stack_endpoint(entry);
    };
    auto result = endpoint_->open(port_name_, sec_result.value(), this, ether_types_);
    if (!result)
    {
        stop_passive();
        return result.errors();
    }
    return port_connect_result(endpoint_->get_stack_endpoint(), endpoint_->is_connected());
}

rx_result ethernet_port::stop_passive ()
{
    if (endpoint_)
        endpoint_->close();
    return true;
}

void ethernet_port::release_buffer (buffer_ptr what)
{
    locks::auto_lock_t _(&free_buffers_lock_);
    what->reinit();
    free_buffers_.push(what);
}

buffer_ptr ethernet_port::get_buffer ()
{
    {
        locks::auto_lock_t _(&free_buffers_lock_);
        if (!free_buffers_.empty())
        {
            buffer_ptr ret = free_buffers_.top();
            free_buffers_.pop();
            return ret;
        }
    }
    return rx_create_reference<buffer_ptr::pointee_type>();
}


} // namespace ethernet
} // namespace interfaces
} // namespace rx_internal

