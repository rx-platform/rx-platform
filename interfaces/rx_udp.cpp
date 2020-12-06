

/****************************************************************************
*
*  interfaces\rx_udp.cpp
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


// rx_udp
#include "interfaces/rx_udp.h"

#include "system/server/rx_server.h"
#include "system/runtime/rx_port_stack_active.h"


namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {

// Class rx_internal::interfaces::ip_endpoints::udp_port 

udp_port::udp_port()
      : rx_recv_timeout_(0),
        rx_send_timeout_(0)
{
}


udp_port::~udp_port()
{
}



rx_result udp_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto bind_result = ctx.bind_item("Timeouts.ReceiveTimeout");
    if (bind_result)
        rx_recv_timeout_ = bind_result.value();
    else
        RUNTIME_LOG_ERROR("udp_port", 200, "Unable to bind to value Timeouts.ReceiveTimeout");
    bind_result = ctx.bind_item("Timeouts.SendTimeout");
    if (bind_result)
        rx_send_timeout_ = bind_result.value();
    else
        RUNTIME_LOG_ERROR("udp_port", 200, "Unable to bind to value Timeouts.SendTimeout");

    string_type addr = ctx.structure.get_root().get_local_as<string_type>("Bind.IPAddress", "");
    uint16_t port = ctx.structure.get_root().get_local_as<uint16_t>("Bind.IPPort", 0);
    bind_address_.parse(addr, port);

    return true;
}

void udp_port::timer_tick (uint32_t tick)
{
   /* std::vector<std::pair<io::ip4_address, rx_protocol_stack_endpoint*> > to_remove;
    sessions_lock_.lock();
    if (!sessions_.empty())
    {
        auto it = sessions_.begin();
        while (it != sessions_.end())
        {
            uint32_t diff = (it->second.last_tick + session_timeout_) - tick;
            if (diff & 0x80000000)
            {
                to_remove.emplace_back(it->first, it->second.entry);
                it = sessions_.erase(it);
            }
            else
            {
                it++;
            }
        }
    }
    sessions_lock_.unlock();
    if (!to_remove.empty())
    {
        for (const auto& one : to_remove)
        {
            std::ostringstream ss;
            ss << "UDP/IP4 session from "
                << one.first.to_string()
                << " time-outed.";
            ITF_LOG_TRACE("udp_port", 500, ss.str());
        }
    }*/
}

rx_result udp_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    auto session_timeout = get_binded_as(rx_recv_timeout_, 2000);
    auto ep = std::make_unique<udp_endpoint>();
    auto sec_result = create_security_context();
    if (!sec_result)
    {
        rx_result ret(sec_result.errors());
        ret.register_error("Unable to create security context");
    }
    auto result = ep->open(bind_address_, session_timeout, sec_result.value(), this);
    if (!result)
    {
        stop_passive();
        return result;
    }
    result = add_stack_endpoint(ep->get_stack_endpoint(), std::move(ep));
    return result;
}

rx_result udp_port::stop_passive ()
{
    close_all_endpoints();
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
}

rx_protocol_result_t udp_port::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    return RX_PROTOCOL_EMPTY;
}

bool udp_port::packet_arrived (const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity)
{
    return RX_PROTOCOL_EMPTY;
}


// Class rx_internal::interfaces::ip_endpoints::udp_endpoint 

udp_endpoint::udp_endpoint()
      : my_port_(nullptr),
        session_timeout_(5000),
        identity_(security::security_context_ptr::null_ptr)
{
    ITF_LOG_DEBUG("udp_endpoint", 200, "UDP endpoint created.");
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);

    mine_entry->send_function = &udp_endpoint::send_function;
    mine_entry->close_function = [](rx_protocol_stack_endpoint* ref, rx_protocol_result_t reason) ->rx_protocol_result_t
    {
        udp_endpoint* me = reinterpret_cast<udp_endpoint*>(ref->user_data);
        me->close();
        rx_notify_closed(&me->stack_endpoint_, 0);
        return RX_PROTOCOL_OK;
    };
}


udp_endpoint::~udp_endpoint()
{
    ITF_LOG_DEBUG("udp_endpoint", 200, "UDP endpoint destroyed.");
}



rx_result udp_endpoint::open (io::ip4_address addr, uint32_t session_timeout, security::security_context_ptr identity, udp_port* port)
{
    identity_ = identity;
    identity_->login();
    my_port_ = port;
    udp_socket_ = rx_create_reference<socket_holder_t>(this);
    bind_address_ = addr;
    udp_socket_->set_identity(identity);
    if (!bind_address_.is_null())
    {
        auto result = udp_socket_->bind_socket_udpip_4(bind_address_.get_ip4_address(), rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool());
        if (!result)
        {
            identity->logout();
            char buff[0x100];
            rx_last_os_error("Unable to bind to UDP/IP port.", buff, sizeof(buff));
            std::ostringstream ss;
            ss << "Unable to bind to address "
                << bind_address_.to_string()
                << ":";
            for (auto& one : result.errors())
                ss << one << ";";
            ITF_LOG_ERROR("udp_port", 500, ss.str()); 
        }
        else
        {
            udp_socket_->set_identity(identity_->get_handle());
        }
        return result;
    }
    else
    {
        string_type err_msg("Can't bind to an empty address!");
        ITF_LOG_ERROR("udp_port", 500, err_msg);
        return err_msg;
    }
}

rx_result udp_endpoint::close ()
{
    if (udp_socket_)
    {
        udp_socket_->disconnect();
        udp_socket_ = rx_reference<socket_holder_t>::null_ptr;
    }
    return true;
}

void udp_endpoint::disconnected (rx_security_handle_t identity)
{
    std::ostringstream ss;
    ss << "UDP port at IP4:"
        << bind_address_.to_string()
        << " disconnected.";
    ITF_LOG_TRACE("udp_endpoint", 500, ss.str());
    if (udp_socket_)
    {
        rx_session session = rx_create_session(nullptr, nullptr, 0, 0, nullptr);
        rx_notify_disconnected(&stack_endpoint_, &session, 0);
        rx_close(&stack_endpoint_, 0);
    }
}

bool udp_endpoint::readed (const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity)
{
    io::ip4_address ip4addr(addr);
    if (!ip4addr.is_valid())
        return false;
    security::secured_scope _(identity);
    rx_const_packet_buffer buff{};
    rx_init_const_packet_buffer(&buff, data, count);
    recv_protocol_packet up = rx_create_recv_packet(0, &buff, 0, 0);
    up.from_addr = &ip4addr;
    up.to_addr = &bind_address_;
    auto result = rx_move_packet_up(&stack_endpoint_, up);
    if (result == RX_PROTOCOL_OK)
        return true;
      
    return false;
}

rx_protocol_result_t udp_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    udp_endpoint* self = reinterpret_cast<udp_endpoint*>(reference->user_data);
    auto io_buffer = rx_create_reference<socket_holder_t::buffer_t>();
    io_buffer->push_data(packet.buffer->buffer_ptr, packet.buffer->size);
    io::ip4_address addr;
    addr.parse(packet.to_addr);
    bool ret = self->udp_socket_->write(io_buffer, addr.get_address(), sizeof(sockaddr_in));
    return ret ? RX_PROTOCOL_OK : RX_PROTOCOL_COLLECT_ERROR;
}

rx_protocol_stack_endpoint* udp_endpoint::get_stack_endpoint ()
{
    return &stack_endpoint_;
}

runtime::items::port_runtime* udp_endpoint::get_port ()
{
    return my_port_;
}


void udp_endpoint::socket_holder_t::release_buffer(buffer_ptr what)
{
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

