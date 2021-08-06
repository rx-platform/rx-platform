

/****************************************************************************
*
*  interfaces\rx_udp.cpp
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


// rx_udp
#include "interfaces/rx_udp.h"

#include "system/server/rx_server.h"
#include "system/runtime/rx_port_stack_active.h"


namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {

// Class rx_internal::interfaces::ip_endpoints::udp_port

udp_port::udp_port()
      : recv_timeout_(2000),
        send_timeout_(1000),
        reconnect_timeout_(5000)
{
}


udp_port::~udp_port()
{
}



rx_result udp_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    string_type addr = ctx.structure.get_root().get_local_as<string_type>("Bind.IPAddress", "");
    uint16_t port = ctx.structure.get_root().get_local_as<uint16_t>("Bind.IPPort", 0);
    bind_address_.parse(addr, port);

    return true;
}

uint32_t udp_port::get_reconnect_timeout () const
{
    return (uint32_t)reconnect_timeout_;
}

rx_result udp_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    auto session_timeout = recv_timeout_;
    endpoint_ = std::make_unique<udp_endpoint>();
    auto sec_result = create_security_context();
    if (!sec_result)
    {
        sec_result.register_error("Unable to create security context");
        return sec_result.errors();
    }
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
    {
        udp_endpoint* whose = reinterpret_cast<udp_endpoint*>(entry->user_data);
        whose->get_port()->disconnect_stack_endpoint(entry);
    };
    auto result = endpoint_->open(bind_address_, sec_result.value(), this);
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

rx_result_with<port_connect_result> udp_port::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
    auto session_timeout = recv_timeout_;
    endpoint_ = std::make_unique<udp_endpoint>();
    auto sec_result = create_security_context();
    if (!sec_result)
    {
        sec_result.register_error("Unable to create security context");
        return sec_result.errors();
    }
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
    {
        udp_endpoint* whose = reinterpret_cast<udp_endpoint*>(entry->user_data);
        whose->get_port()->disconnect_stack_endpoint(entry);
    };
    auto result = endpoint_->open(bind_address_, sec_result.value(), this);
    if (!result)
    {
        stop_passive();
        return result.errors();
    }
    return port_connect_result(endpoint_->get_stack_endpoint(), endpoint_->is_connected());
}

rx_result udp_port::stop_passive ()
{
    if (endpoint_)
        endpoint_->close();
    return true;
}

void udp_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
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

void udp_port::release_buffer (buffer_ptr what)
{
    locks::auto_lock_t _(&free_buffers_lock_);
    what->reinit();
    free_buffers_.push(what);
}

buffer_ptr udp_port::get_buffer ()
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


// Class rx_internal::interfaces::ip_endpoints::udp_endpoint

udp_endpoint::udp_endpoint()
      : my_port_(nullptr),
        current_state_(udp_state::not_active),
        identity_(security::security_context_ptr::null_ptr)
{
    ITF_LOG_DEBUG("udp_endpoint", 200, "UDP endpoint created.");
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);

    mine_entry->send_function = &udp_endpoint::send_function;
}


udp_endpoint::~udp_endpoint()
{
    ITF_LOG_DEBUG("udp_endpoint", 200, "UDP endpoint destroyed.");
}



rx_protocol_result_t udp_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    udp_endpoint* self = reinterpret_cast<udp_endpoint*>(reference->user_data);
    if (self->udp_socket_)
        return self->send_packet(packet);
    else
        return RX_PROTOCOL_CLOSED;
}

rx_protocol_stack_endpoint* udp_endpoint::get_stack_endpoint ()
{
    return &stack_endpoint_;
}

runtime::items::port_runtime* udp_endpoint::get_port ()
{
    return my_port_;
}

rx_protocol_result_t udp_endpoint::send_packet (send_protocol_packet packet)
{
    if (my_port_)
    {
        auto io_buffer = my_port_->get_buffer();
        io_buffer->push_data(packet.buffer->buffer_ptr, packet.buffer->size);
        io::ip4_address addr;
        addr.parse(packet.to_addr);
        bool ret = udp_socket_->write(io_buffer, addr.get_address(), sizeof(sockaddr_in));
        if (!ret)
            my_port_->release_buffer(io_buffer);
        return ret ? RX_PROTOCOL_OK : RX_PROTOCOL_COLLECT_ERROR;
    }
    else
        return RX_PROTOCOL_INVALID_SEQUENCE;
}

void udp_endpoint::disconnected (rx_security_handle_t identity)
{
    if (udp_socket_)
    {
        udp_socket_->disconnect();
        udp_socket_ = socket_holder_t::smart_ptr::null_ptr;

        if (current_state_ == udp_state::binded)
        {
            auto session = rx_create_session(&bind_address_, nullptr, 0, 0, nullptr);
            auto proto_result = rx_notify_disconnected(&stack_endpoint_, &session, RX_PROTOCOL_OK);
        }

    }
    if (current_state_ == udp_state::binded || current_state_ == udp_state::not_binded)
    {
        suspend_timer();
        current_state_ = udp_state::not_binded;
        start_timer(false);
    }
}

bool udp_endpoint::readed (const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity)
{
    io::ip4_address ip4addr(addr);
    if (!ip4addr.is_valid())
        return false;
    security::secured_scope auto_sec(identity);
    rx_const_packet_buffer buff{};
    rx_init_const_packet_buffer(&buff, data, count);
    recv_protocol_packet up = rx_create_recv_packet(0, &buff, 0, 0);
    up.from_addr = &ip4addr;
    up.to_addr = &bind_address_;
    auto result = rx_move_packet_up(&stack_endpoint_, up);
    if (result == RX_PROTOCOL_OK)
        return true;
    else
        return true;
}

rx_result udp_endpoint::open (io::ip4_address addr, security::security_context_ptr identity, udp_port* port)
{
    identity_ = identity;
    identity_->login();
    my_port_ = port;
    bind_address_ = addr;
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

rx_result udp_endpoint::close ()
{
    if (timer_)
    {
        timer_->cancel();
        timer_ = rx_timer_ptr::null_ptr;
    }
    if (udp_socket_)
    {
        udp_socket_->disconnect();
        udp_socket_ = rx_reference<socket_holder_t>::null_ptr;
    }
    current_state_ = udp_state::stopped;
    return true;
}

bool udp_endpoint::tick ()
{
    switch (current_state_)
    {
    case udp_state::not_active:
    case udp_state::not_binded:
        {
            RX_ASSERT(!udp_socket_ && my_port_ != nullptr);
            rx_result result;
            udp_socket_ = rx_create_reference<socket_holder_t>(this);
            udp_socket_->set_identity(stack_endpoint_.identity);
            if (!bind_address_.is_null())
            {
                result = udp_socket_->bind_socket_udpip_4(bind_address_.get_ip4_address(), infrastructure::server_runtime::instance().get_io_pool()->get_pool());
                if (result)
                {
                    ITF_LOG_INFO("udp_endpoint", 200, "UDP port opened at "s + bind_address_.to_string());
                    current_state_ = udp_state::binded;
                    auto session = rx_create_session(&bind_address_, nullptr, 0, 0, nullptr);

                    auto proto_result = rx_notify_connected(&stack_endpoint_, &session);

                    return false;// we're done here nothing more to do
                }
                else
                {
                    if (udp_socket_)
                    {
                        udp_socket_->disconnect();
                        udp_socket_ = socket_holder_t::smart_ptr::null_ptr;
                    }
                    if (current_state_ != udp_state::not_binded)
                    {

                        ITF_LOG_ERROR("udp_endpoint", 200, "Error binding UDP port at "s + bind_address_.to_string() + "." + result.errors_line());
                        current_state_ = udp_state::not_binded;
                    }
                    return true;
                }
            }
            else
            {
                RX_ASSERT(false);// this should not happened
                return false;// stop the timer
            }
        }
    case udp_state::binded:
        {
            return false;
        }
    case udp_state::stopped:
        {
            return false;
        }
    default:
        RX_ASSERT(false);
        return false;
    }
}

void udp_endpoint::start_timer (bool fire_now)
{
    if (my_port_ && timer_)
    {
        timer_->start(my_port_->get_reconnect_timeout(), fire_now);
    }
}

void udp_endpoint::suspend_timer ()
{
    if (timer_)
    {
        timer_->suspend();
    }
}

bool udp_endpoint::is_connected () const
{
    return current_state_ == udp_state::binded;
}

void udp_endpoint::release_buffer (buffer_ptr what)
{
    return my_port_->release_buffer(what);
}


void udp_endpoint::socket_holder_t::release_buffer(buffer_ptr what)
{
    if (whose)
        return whose->release_buffer(what);
}
bool udp_endpoint::socket_holder_t::readed(const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity)
{
    security::secured_scope _(identity);
    if (whose)
        whose->readed(data, count, addr, identity);
    return true;
}
void udp_endpoint::socket_holder_t::on_shutdown(rx_security_handle_t identity)
{
    if (whose)
    {
        security::secured_scope _(identity);
        whose->disconnected(identity);
    }
}
udp_endpoint::socket_holder_t::socket_holder_t(udp_endpoint* whose)
    : whose(whose)
{
}
udp_endpoint::socket_holder_t::socket_holder_t(socket_holder_t&& right) noexcept
{
    whose = right.whose;
    right.whose = nullptr;
}
void udp_endpoint::socket_holder_t::disconnect()
{
    whose = nullptr;
    initiate_shutdown();
}
} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal

