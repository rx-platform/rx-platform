

/****************************************************************************
*
*  interfaces\rx_tcp_client.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_tcp_client
#include "interfaces/rx_tcp_client.h"

#include "rx_endpoints.h"
#include "lib/security/rx_security.h"


namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {

// Class rx_internal::interfaces::ip_endpoints::tcp_client_endpoint 

tcp_client_endpoint::tcp_client_endpoint()
      : my_port_(nullptr),
        current_state_(tcp_state::not_active),
        identity_(security::security_context_ptr::null_ptr)
{
    ITF_LOG_DEBUG("tcp_client_endpoint", 200, "TCP client endpoint created.");
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);
    mine_entry->send_function = &tcp_client_endpoint::send_function;

    mine_entry->close_function = [](rx_protocol_stack_endpoint* ref, rx_protocol_result_t reason) ->rx_protocol_result_t
    {
        tcp_client_endpoint* me = reinterpret_cast<tcp_client_endpoint*>(ref->user_data);
        me->stop();
        rx_notify_closed(&me->stack_endpoint_, 0);
        return RX_PROTOCOL_OK;
    };
}


tcp_client_endpoint::~tcp_client_endpoint()
{
    ITF_LOG_DEBUG("tcp_server_endpoint", 200, "TCP server endpoint destroyed.");
}



rx_result tcp_client_endpoint::stop ()
{
    if (timer_)
    {
        timer_->cancel();
        timer_ = rx_timer_ptr::null_ptr;
    }
    if (tcp_socket_)
    {
        tcp_socket_->disconnect();
        tcp_socket_ = socket_holder_t::smart_ptr::null_ptr;
    }
    current_state_ = tcp_state::stopped;
    return true;
}

void tcp_client_endpoint::disconnected (rx_security_handle_t identity)
{
    if (tcp_socket_)
    {
        tcp_socket_->disconnect();
        tcp_socket_ = socket_holder_t::smart_ptr::null_ptr;

        if (current_state_ == tcp_state::connected)
        {
            auto session = rx_create_session(&local_addr_, &remote_addr_, 0, 0, nullptr);
            auto proto_result = rx_notify_disconnected(&stack_endpoint_, &session, RX_PROTOCOL_OK);
        }

    }
    if (current_state_ == tcp_state::connected || current_state_ == tcp_state::connecting)
    {
        suspend_timer();
        bool fire_now = current_state_ == tcp_state::connected;
        current_state_ = tcp_state::not_connected;
        start_timer(false);
    }
}

bool tcp_client_endpoint::readed (const void* data, size_t count, rx_security_handle_t identity)
{
    rx_const_packet_buffer buffer{ (const uint8_t*)data, count, 0 };
    recv_protocol_packet up = rx_create_recv_packet(0, &buffer, 0, 0);
    {
        up.from_addr = &remote_addr_;
        up.to_addr = &local_addr_;
        security::secured_scope ctx(identity);
        auto res = rx_move_packet_up(&stack_endpoint_, up);
        if (res != RX_PROTOCOL_OK)
        {
            std::ostringstream ss;
            ss << "TCP server endpoint ["
                << local_addr_.to_string()
                << ", "
                << remote_addr_.to_string()
                << "] error moving packet up stack:"
                << rx_protocol_error_message(res);
            ITF_LOG_ERROR("tcp_client_endpoint", 200, ss.str());
            return false;
        }
    }
    return true;
}

rx_protocol_result_t tcp_client_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    tcp_client_endpoint* self = reinterpret_cast<tcp_client_endpoint*>(reference->user_data);
    if (self->tcp_socket_)
    {
        auto io_buffer = rx_create_reference<socket_holder_t::buffer_t>();
        io_buffer->push_data(packet.buffer->buffer_ptr, packet.buffer->size);
        bool ret = self->tcp_socket_->write(io_buffer);
        return ret ? RX_PROTOCOL_OK : RX_PROTOCOL_COLLECT_ERROR;
    }
    else
    {
        return RX_PROTOCOL_COLLECT_ERROR;
    }
}

bool tcp_client_endpoint::connected (sockaddr_in* addr, sockaddr_in* local_addr)
{
    if (current_state_ != tcp_state::connecting)
    {
        return false;
    }
    current_state_ = tcp_state::connected;

    std::ostringstream ss;
    ss << "IP4 client binded to:"
        << io::get_ip4_addr_string(local_addr)
        << " connected to:"
        << io::get_ip4_addr_string(addr)
        << ".";
    ITF_LOG_TRACE("tcp_client_port", 500, ss.str());

    auto session = rx_create_session(&local_addr_, &remote_addr_, 0, 0, nullptr);

    auto proto_result = rx_notify_connected(&stack_endpoint_, &session);

    return true;
}

bool tcp_client_endpoint::tick ()
{
    switch (current_state_)
    {
    case tcp_state::not_active:
    case tcp_state::not_connected:
        {
            RX_ASSERT(!tcp_socket_ && my_port_ != nullptr);
            rx_result result;
            tcp_socket_ = rx_create_reference<socket_holder_t>(this);
            tcp_socket_->set_identity(stack_endpoint_.identity);
            if(local_addr_.is_null())
                result = tcp_socket_->bind_socket_tcpip_4(nullptr);
            else
                result = tcp_socket_->bind_socket_tcpip_4(local_addr_.get_ip4_address());
            if (result)
            {
                current_state_ = tcp_state::connecting;
                result = tcp_socket_->connect_to(remote_addr_.get_address(), sizeof(sockaddr_in), infrastructure::server_runtime::instance().get_io_pool()->get_pool());
            }
            if (!result)
            {
                tcp_socket_->disconnect();
                tcp_socket_ = socket_holder_t::smart_ptr::null_ptr;
                current_state_ = tcp_state::not_connected;
                return true;
            }
            else
            {
                return false;
            }
        }
    case tcp_state::connecting:
        {
            return false;
        }
    case tcp_state::connected:
        {
            return false;
        }
    case tcp_state::stopped:
        {
            return false;
        }
    default:
        RX_ASSERT(false);
        return false;
    }
}

rx_result tcp_client_endpoint::start (const protocol_address* addr, const protocol_address* remote_addr, security::security_context_ptr identity, tcp_client_port* port)
{
    identity_ = identity;
    identity_->login();
    my_port_ = port;
    local_addr_.parse(addr);
    remote_addr_.parse(remote_addr);
    timer_ = my_port_->create_timer_function([this]()
        {
            if (!tick())
            {
                suspend_timer();
            }
        });
    start_timer(true);
    return true;
}

void tcp_client_endpoint::start_timer (bool fire_now)
{
    if (my_port_ && timer_)
    {
        timer_->start(my_port_->get_reconnect_timeout(), fire_now);
    }
}

void tcp_client_endpoint::suspend_timer ()
{
    if (timer_)
    {
        timer_->suspend();
    }
}

rx_protocol_stack_endpoint* tcp_client_endpoint::get_stack_endpoint ()
{
    return &stack_endpoint_;
}

runtime::items::port_runtime* tcp_client_endpoint::get_port ()
{
    return my_port_;
}


// Class rx_internal::interfaces::ip_endpoints::tcp_client_port 

tcp_client_port::tcp_client_port()
      : rx_recv_timeout_(0),
        rx_connect_timeout_(0),
        rx_reconnect_timeout_(0)
{
}



rx_result tcp_client_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto bind_result = ctx.bind_item("Timeouts.ReceiveTimeout");
    if (bind_result)
        rx_recv_timeout_ = bind_result.value();
    else
        RUNTIME_LOG_ERROR("tcp_server_port", 200, "Unable to bind to value Timeouts.ReceiveTimeout");
    bind_result = ctx.bind_item("Timeouts.ConnectTimeout");
    if (bind_result)
        rx_connect_timeout_ = bind_result.value();
    else
        RUNTIME_LOG_ERROR("tcp_server_port", 200, "Unable to bind to value Timeouts.ConnectTimeout");
    bind_result = ctx.bind_item("Timeouts.ReconnectTimeout");
    if (bind_result)
        rx_reconnect_timeout_ = bind_result.value();
    else
        RUNTIME_LOG_ERROR("tcp_server_port", 200, "Unable to bind to value Timeouts.ReconnectTimeout");

    string_type addr = ctx.structure.get_root().get_local_as<string_type>("Bind.IPAddress", "");
    uint16_t port = ctx.structure.get_root().get_local_as<uint16_t>("Bind.IPPort", 0);

    string_type remote_addr = ctx.structure.get_root().get_local_as<string_type>("Connect.IPAddress", "");
    uint16_t remote_port = ctx.structure.get_root().get_local_as<uint16_t>("Connect.IPPort", 0);

    bind_address_.parse(addr, port);
    connect_address_.parse(remote_addr, remote_port);

    return true;
}

uint32_t tcp_client_port::get_reconnect_timeout () const
{
    return get_binded_as<uint32_t>(rx_reconnect_timeout_, 5000);
}

rx_result_with<rx_protocol_stack_endpoint*> tcp_client_port::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
    auto session_timeout = get_binded_as(rx_recv_timeout_, 2000);
    endpoint_ = std::make_unique<tcp_client_endpoint>();
    auto sec_result = create_security_context();
    if (!sec_result)
    {
        sec_result.register_error("Unable to create security context");
        return sec_result.errors();
    }
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
    {
        tcp_client_endpoint* whose = reinterpret_cast<tcp_client_endpoint*>(entry->user_data);
        whose->get_port()->disconnect_stack_endpoint(entry);
    };
    auto result = endpoint_->start(&bind_address_, &connect_address_, sec_result.value(), this);
    if (!result)
    {
        stop_passive();
        return result.errors();
    }
    return endpoint_->get_stack_endpoint();
}

rx_result tcp_client_port::stop_passive ()
{
    if(endpoint_)
        endpoint_->stop();
    return true;
}

void tcp_client_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
    if (local_addr.is_null())
    {
        auto addr = binder_data.get_value("Bind.IPAddress");
        auto port = binder_data.get_value("Bind.IPPort");
        string_type addr_str;
        uint16_t port_val = 0;
        if (!addr.is_null())
            addr_str = addr.get_storage().get_string_value();
        if (!port.is_null())
            port_val = (uint16_t)port.get_storage().get_integer_value();
        if (!addr_str.empty() || port_val != 0)
        {
            io::ip4_address ip_addr(addr_str, port_val);
            if (ip_addr.is_valid())
                local_addr = &ip_addr;
        }
    }
    if (remote_addr.is_null())
    {
        auto addr = binder_data.get_value("Connect.IPAddress");
        auto port = binder_data.get_value("Connect.IPPort");
        string_type addr_str;
        uint16_t port_val = 0;
        if (!addr.is_null())
            addr_str = addr.get_storage().get_string_value();
        if (!port.is_null())
            port_val = (uint16_t)port.get_storage().get_integer_value();
        if (!addr_str.empty() || port_val != 0)
        {
            io::ip4_address ip_addr(addr_str, port_val);
            if (ip_addr.is_valid())
                remote_addr = &ip_addr;
        }
    }
}


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal

