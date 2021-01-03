

/****************************************************************************
*
*  interfaces\rx_tcp_server.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_tcp_server
#include "interfaces/rx_tcp_server.h"

#include "system/server/rx_server.h"
#include "sys_internal/rx_security/rx_platform_security.h"
#include "system/runtime/rx_operational.h"
#include "lib/security/rx_security.h"

namespace rx
{
rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();
}


namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {

// Class rx_internal::interfaces::ip_endpoints::tcp_server_endpoint 

tcp_server_endpoint::tcp_server_endpoint()
      : my_port_(nullptr),
        identity_(security::security_context_ptr::null_ptr)
{
    ITF_LOG_DEBUG("tcp_server_endpoint", 200, "TCP server endpoint created.");
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);
    mine_entry->send_function = &tcp_server_endpoint::send_function;

    mine_entry->close_function = [] (rx_protocol_stack_endpoint* ref, rx_protocol_result_t reason) ->rx_protocol_result_t
    {
        tcp_server_endpoint* me = reinterpret_cast<tcp_server_endpoint*>(ref->user_data);
        me->close();
        rx_notify_closed(&me->stack_endpoint_, 0);
        return RX_PROTOCOL_OK;
    };
}


tcp_server_endpoint::~tcp_server_endpoint()
{
    if(identity_)
        identity_->logout();
    ITF_LOG_DEBUG("tcp_server_endpoint", 200, "TCP server endpoint destroyed.");
}



rx_result_with<tcp_server_endpoint::socket_ptr> tcp_server_endpoint::open (tcp_server_port* my_port, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool& dispatcher, security::security_context_ptr identity)
{
    identity_ = identity;
    identity_->login();
    my_port_ = my_port;
    tcp_socket_ = rx_create_reference<socket_holder_t>(this, handle, addr, local_addr);
    tcp_socket_->set_identity(identity->get_handle());
    remote_address_ = addr;
    local_address_ = local_addr;
    stack_endpoint_.identity = identity_->get_handle();
    std::ostringstream ss;
    ss << "Client from IP4:"
        << remote_address_.to_string()
        << " connected IP4:"
        << local_address_.to_string()
        << ".";
    ITF_LOG_TRACE("tcp_server_port", 500, ss.str());

    return tcp_socket_;
}

rx_result tcp_server_endpoint::close ()
{
    if (tcp_socket_)
    {
        tcp_socket_->disconnect();
        tcp_socket_ = socket_holder_t::smart_ptr::null_ptr;
    }
    return true;
}

void tcp_server_endpoint::disconnected (rx_security_handle_t identity)
{
    std::ostringstream ss;
    ss << "Client from IP4:"
        << remote_address_.to_string()
        << " disconnected from IP4:"
        << local_address_.to_string()
        << ".";
    ITF_LOG_TRACE("tcp_server_port", 500, ss.str());
    if (tcp_socket_)
    {
        rx_session session = rx_create_session(&remote_address_, &local_address_, 0, 0, nullptr);
        rx_notify_disconnected(&stack_endpoint_,  &session, 0);
        rx_close(&stack_endpoint_, 0);
    }
}

bool tcp_server_endpoint::readed (const void* data, size_t count, rx_security_handle_t identity)
{
    rx_const_packet_buffer buffer{ (const uint8_t*)data, count, 0 };
    recv_protocol_packet up = rx_create_recv_packet(0, &buffer, 0, 0);
    {
        up.from_addr = &remote_address_;
        up.to_addr = &local_address_;
        security::secured_scope ctx(identity);
        auto res = rx_move_packet_up(&stack_endpoint_, up);
        if (res != RX_PROTOCOL_OK)
        {
            std::ostringstream ss;
            ss << "TCP server endpoint ["
                << local_address_.to_string()
                << ", "
                << remote_address_.to_string()
                << "] error moving packet up stack:"
                << rx_protocol_error_message(res);
            ITF_LOG_ERROR("tcp_server_endpoint", 200, ss.str());
            return false;
        }
    }
    return true;
}

rx_protocol_result_t tcp_server_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    tcp_server_endpoint* self = reinterpret_cast<tcp_server_endpoint*>(reference->user_data);
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

rx_protocol_stack_endpoint* tcp_server_endpoint::get_stack_endpoint ()
{
    return &stack_endpoint_;
}

void tcp_server_endpoint::set_receive_timeout (uint32_t val)
{
    tcp_socket_->set_receive_timeout(val);
}

void tcp_server_endpoint::set_send_timeout (uint32_t val)
{
    tcp_socket_->set_send_timeout(val);
}

runtime::items::port_runtime* tcp_server_endpoint::get_port ()
{
    return my_port_;
}

void tcp_server_endpoint::socket_holder_t::release_buffer(buffer_ptr what)
{
}
bool tcp_server_endpoint::socket_holder_t::readed(const void* data, size_t count, rx_security_handle_t identity)
{
    security::secured_scope _(identity);
    if (whose)
        return whose->readed(data, count, identity);
    else
        return false;
}
void tcp_server_endpoint::socket_holder_t::on_shutdown(rx_security_handle_t identity)
{
    if (whose)
    {
        security::secured_scope _(identity);
        whose->disconnected(identity);
    }
}
tcp_server_endpoint::socket_holder_t::socket_holder_t(tcp_server_endpoint* whose, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr)
    : whose(whose)
    , rx::io::tcp_socket_std_buffer(handle, addr, local_addr, rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool())
{
}
tcp_server_endpoint::socket_holder_t::socket_holder_t(socket_holder_t&& right) noexcept
{
    whose = right.whose;
    right.whose = nullptr;
}
void tcp_server_endpoint::socket_holder_t::disconnect()
{
    whose = nullptr;
    initiate_shutdown();
}
// Class rx_internal::interfaces::ip_endpoints::tcp_server_port 


rx_result tcp_server_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto bind_result = ctx.bind_item("Timeouts.ReceiveTimeout");
    if (bind_result)
        rx_recv_timeout_ = bind_result.value();
    else
        RUNTIME_LOG_ERROR("tcp_server_port", 200, "Unable to bind to value Timeouts.ReceiveTimeout");
    bind_result = ctx.bind_item("Timeouts.SendTimeout");
    if (bind_result)
        rx_send_timeout_ = bind_result.value();
    else
        RUNTIME_LOG_ERROR("tcp_server_port", 200, "Unable to bind to value Timeouts.SendTimeout");

    string_type addr = ctx.structure.get_root().get_local_as<string_type>("Bind.IPAddress", "");
    uint16_t port = ctx.structure.get_root().get_local_as<uint16_t>("Bind.IPPort", 0);
    bind_address_.parse(addr, port);

    return true;
}

rx_result tcp_server_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
     listen_socket_ = rx_create_reference<io::tcp_listent_std_buffer>(
        [this](sys_handle_t handle, sockaddr_in* his, sockaddr_in* mine, rx_security_handle_t identity) -> io::tcp_socket_std_buffer::smart_ptr
        {
             auto new_endpoint = std::make_unique<tcp_server_endpoint>();
             auto sec_result = create_security_context();
             if (!sec_result)
             {
                 rx_result ret(sec_result.errors());
                 ret.register_error("Unable to create security context");
             }
            auto ret_ptr = new_endpoint->open(this, handle, his, mine, rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool(), sec_result.value());
            if (ret_ptr)
            {
                uint32_t recv_timeout = get_binded_as(rx_recv_timeout_, 0);
                uint32_t send_timeout = get_binded_as(rx_send_timeout_, 0);
                if (recv_timeout)
                    new_endpoint->set_receive_timeout(recv_timeout);
                if (send_timeout)
                    new_endpoint->set_send_timeout(send_timeout);
                io::ip4_address local_addr(mine);
                io::ip4_address remote_addr(his);
                auto stack_ptr = new_endpoint->get_stack_endpoint();
                auto result = add_stack_endpoint(stack_ptr, std::move(new_endpoint), &local_addr, &remote_addr);
                if (!result)
                {
                    return io::tcp_socket_std_buffer::smart_ptr::null_ptr;
                }
            }
            return ret_ptr.value();

        });
    if (!bind_address_.is_null())
    {
        auto result = listen_socket_->start_tcpip_4(bind_address_.get_ip4_address(), rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool());
        if (!result)
        {
            char buff[0x100];
            rx_last_os_error("Unable to bind to TCP/IP port.", buff, sizeof(buff));
            std::ostringstream ss;
            ss << "Unable to bind to address "
                << bind_address_.to_string()
                << ":";
            for(auto& one : result.errors())
                ss << one << ";";
            ITF_LOG_ERROR("tcp_server_port", 500, ss.str());
        }
        return result;
    }
    else
    {
        string_type err_msg("Can't bind to an empty address!");
        ITF_LOG_ERROR("tcp_server_port", 500, err_msg);
        return err_msg;
    }
}

rx_result tcp_server_port::stop_passive ()
{
    if (listen_socket_)
    {
        listen_socket_->stop();
        listen_socket_ = rx_reference<io::tcp_listent_std_buffer>::null_ptr;
    }
    return true;
}

void tcp_server_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
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
        if(!addr_str.empty() || port_val!=0)
        {
            io::ip4_address ip_addr(addr_str, port_val);
            if (ip_addr.is_valid())
                local_addr = &ip_addr;
        }
    }
}


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal

