

/****************************************************************************
*
*  interfaces\rx_tcp_server.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


// rx_tcp_server
#include "interfaces/rx_tcp_server.h"

#include "system/server/rx_server.h"
#include "sys_internal/rx_security/rx_platform_security.h"
#include "system/runtime/rx_operational.h"
#include "security/rx_security.h"
#include "rx_endpoints.h"
#include "discovery/rx_discovery_main.h"

namespace rx_platform
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
        closing_(false)
{
    ITF_LOG_DEBUG("tcp_server_endpoint", 200, "TCP server endpoint created.");
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);
    mine_entry->send_function = &tcp_server_endpoint::send_function;


    mine_entry->disconnect_function = []  (struct rx_protocol_stack_endpoint* reference
        , struct rx_session_def* session
        , rx_protocol_result_t reason) -> rx_protocol_result_t
        {
            tcp_server_endpoint* me = reinterpret_cast<tcp_server_endpoint*>(reference->user_data);
			if (me->tcp_socket_)
			{
				me->tcp_socket_->write(buffer_ptr::null_ptr);
			}
            return RX_PROTOCOL_OK;
        };
    mine_entry->close_function = [] (rx_protocol_stack_endpoint* ref, rx_protocol_result_t reason) ->rx_protocol_result_t
    {
        tcp_server_endpoint* me = reinterpret_cast<tcp_server_endpoint*>(ref->user_data);
        if (me->tcp_socket_ || me->closing_)
        {
            me->close();
        }
        return RX_PROTOCOL_OK;
    };
}


tcp_server_endpoint::~tcp_server_endpoint()
{

    {
        //locks::auto_lock_t _(&state_lock_);
        if (tcp_socket_)
        {
            tcp_socket_->detach();
        }
    }
    ITF_LOG_DEBUG("tcp_server_endpoint", 200, "TCP server endpoint destroyed.");
}



rx_result_with<tcp_server_endpoint::socket_ptr> tcp_server_endpoint::open (tcp_server_port* my_port, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool& dispatcher, security::security_context_ptr identity, uint32_t keep_alive)
{
    identity_ = identity;
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
    if (tcp_socket_ || closing_)
    {
        socket_holder_t::smart_ptr temp_socket;
        {
            locks::auto_lock_t _(&state_lock_);
            closing_ = false;
            temp_socket = tcp_socket_;
            if(temp_socket)
                temp_socket->whose = nullptr;
            tcp_socket_ = socket_holder_t::smart_ptr::null_ptr;
        }
        if (temp_socket)
        {
            temp_socket->write(buffer_ptr::null_ptr);
            //temp_socket->detach();
        }
        rx_notify_closed(&stack_endpoint_, 0);
    }
    return true;
}

void tcp_server_endpoint::disconnected (rx_security_handle_t identity)
{
    socket_holder_t::smart_ptr temp_socket;
    {
        locks::auto_lock_t _(&state_lock_);
        temp_socket = tcp_socket_;
        if (temp_socket)
        {
            closing_ = true;
            temp_socket->whose = nullptr;
        }
        tcp_socket_ = socket_holder_t::smart_ptr::null_ptr;
    }
    if (temp_socket)
    {
        std::ostringstream ss;
        ss << "Client from IP4:"
            << remote_address_.to_string()
            << " disconnected from IP4:"
            << local_address_.to_string()
            << ".";
        ITF_LOG_TRACE("tcp_server_port", 500, ss.str());

        rx_session session = rx_create_session(&remote_address_, &local_address_, 0, 0, nullptr);
        rx_notify_disconnected(&stack_endpoint_, &session, 0);
        rx_close(&stack_endpoint_, 0);
    }
}

bool tcp_server_endpoint::readed (const void* data, size_t count, rx_security_handle_t identity)
{
    if (my_port_)
    {
        my_port_->status.received_packet(count);
    }

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
    socket_holder_t::smart_ptr temp_socket;
    {
        locks::auto_lock_t _(&self->state_lock_);
        temp_socket = self->tcp_socket_;
    }
    if (self->my_port_ && temp_socket)
    {
        uint8_t* buff = packet.buffer->buffer_ptr;
        size_t count = packet.buffer->size;
        size_t sent = 0;
        while (sent < count)
        {
            size_t chunk_size = std::min<size_t>(count - sent, 0x10000);
            auto io_buffer = self->my_port_->get_buffer();
            io_buffer->push_data(&buff[sent], chunk_size);
            bool ret = temp_socket->write(io_buffer);
            if (!ret)
            {
                self->my_port_->release_buffer(io_buffer);
                return RX_PROTOCOL_COLLECT_ERROR;
            }
            self->my_port_->status.sent_packet(chunk_size);
            sent += chunk_size;
        }
        return RX_PROTOCOL_OK;
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

void tcp_server_endpoint::release_buffer (buffer_ptr what)
{
    if (my_port_)
        return my_port_->release_buffer(what);
}

void tcp_server_endpoint::socket_holder_t::release_buffer(buffer_ptr what)
{
    if (whose)
        return whose->release_buffer(what);
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
        auto saved = whose;
        whose = nullptr;
        security::secured_scope _(identity);
        saved->disconnected(identity);
    }
}
tcp_server_endpoint::socket_holder_t::socket_holder_t(tcp_server_endpoint* whose, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr)
    : whose(whose)
    , tcp_socket_std_buffer(handle, addr, local_addr, rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool())
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
void tcp_server_endpoint::socket_holder_t::detach()
{
    whose = nullptr;
}
// Class rx_internal::interfaces::ip_endpoints::tcp_server_port 


rx_result tcp_server_port::initialize_runtime (runtime::runtime_init_context& ctx)
{

    auto result = status.initialize(ctx);
    if (!result)
        return result;

    auto bind_result = recv_timeout_.bind("Timeouts.ReceiveTimeout", ctx);
    if (!bind_result)
        RUNTIME_LOG_ERROR("tcp_server_port", 200, "Unable to bind to value Timeouts.ReceiveTimeout"); ;
    bind_result = keep_alive_.bind("Options.KeepAlive", ctx);
    if (!bind_result)
        RUNTIME_LOG_ERROR("tcp_server_port", 200, "Unable to bind to value Options.KeepAlive");
    bind_result = send_timeout_.bind("Timeouts.SendTimeout", ctx);
    if (!bind_result)
        RUNTIME_LOG_ERROR("tcp_server_port", 200, "Unable to bind to value Timeouts.ReceiveTimeout");

    string_type addr = ctx.structure.get_root().get_local_as<string_type>("Bind.IPAddress", "");
    addr = rx_gate::instance().resolve_ip4_alias(addr);
    uint16_t port = ctx.structure.get_root().get_local_as<uint16_t>("Bind.IPPort", 0);
    bind_address_.parse(addr, port);

    return true;
}

rx_result tcp_server_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
     listen_socket_ = rx_create_reference<tcp_listent_std_buffer>(
        [this](sys_handle_t handle, sockaddr_in* his, sockaddr_in* mine, rx_security_handle_t identity) -> tcp_socket_std_buffer::smart_ptr
        {
             auto new_endpoint = std::make_unique<tcp_server_endpoint>();
             auto sec_ctx = get_security_context();
            auto ret_ptr = new_endpoint->open(this, handle, his, mine, rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool(), sec_ctx, keep_alive_);
            if (ret_ptr)
            {
                new_endpoint->set_receive_timeout(recv_timeout_);
                new_endpoint->set_send_timeout(send_timeout_);
                io::ip4_address local_addr(mine);
                io::ip4_address remote_addr(his);
                auto stack_ptr = new_endpoint->get_stack_endpoint();
                auto result = add_stack_endpoint(stack_ptr, std::move(new_endpoint), &local_addr, &remote_addr);
                if (!result)
                {
                    return tcp_socket_std_buffer::smart_ptr::null_ptr;
                }
            }
            return ret_ptr.value();

        });
    if (!bind_address_.is_null())
    {
        auto result = listen_socket_->start_tcpip_4(bind_address_.get_ip4_address(), rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool(), keep_alive_);
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
        listen_socket_ = rx_reference<tcp_listent_std_buffer>::null_ptr;
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
        {
            addr_str = addr.get_string();
            addr_str = rx_gate::instance().resolve_ip4_alias(addr_str);
        }
        if (!port.is_null())
            port_val = (uint16_t)port.get_unassigned();
        if(!addr_str.empty() || port_val!=0)
        {
            io::ip4_address ip_addr(addr_str, port_val);
            if (ip_addr.is_valid())
                local_addr = &ip_addr;
        }
    }
}

void tcp_server_port::release_buffer (buffer_ptr what)
{
    locks::auto_lock_t _(&free_buffers_lock_);
    what->reinit();
    free_buffers_.push(what);
}

buffer_ptr tcp_server_port::get_buffer ()
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


// Class rx_internal::interfaces::ip_endpoints::system_http_port 


uint16_t system_http_port::get_configuration_port () const
{
    auto ret = rx_gate::instance().get_configuration().other.http_port;
    if (ret == 0)
        ret = 0x7ABD;
    return ret;
}


// Class rx_internal::interfaces::ip_endpoints::system_rx_port 


uint16_t system_rx_port::get_configuration_port () const
{
    auto ret = rx_gate::instance().get_configuration().other.rx_port;
    if (ret == 0)
        ret = 0x7ABC;
    return 0;
}


// Class rx_internal::interfaces::ip_endpoints::system_server_port_base 


rx_result system_server_port_base::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto port = get_configuration_port();
    if(port)
        ctx.set_item_static("Bind.IPPort", port);
    system_port_ = 0;
    auto result = tcp_server_port::initialize_runtime(ctx);
    return result;
}

rx_result system_server_port_base::start_runtime (runtime_start_context& ctx)
{
    auto result = tcp_server_port::start_runtime(ctx);
    if (!result)
        return result;

    subs_id_ = discovery::discovery_manager::instance().subscribe_to_port([this](uint16_t port)
        {
            if (system_port_ != port)
            {
                if (listening_)
                {
                    if (system_port_)
                        tcp_server_port::stop_passive();

                    if (port)
                    {

                        if (get_configuration_port() == 0)
                        {
                            bind_address_ = io::ip4_address("", port);
                            ITF_LOG_INFO("system_server_port_base", 900, "System TCP server port binded at "s + bind_address_.to_string());
                        }
                        tcp_server_port::start_listen(nullptr, nullptr);
                    }
                }
                else
                {
                    if (get_configuration_port() == 0)
                    {
                        bind_address_ = io::ip4_address("", port);
                        ITF_LOG_INFO("system_server_port_base", 900, "System TCP server port binded at "s + bind_address_.to_string());
                    }
                }
                system_port_ = port;
            }

        }, smart_this());

    return result;
}

rx_result system_server_port_base::stop_runtime (runtime_stop_context& ctx)
{
    auto result = tcp_server_port::stop_runtime(ctx);
    if (!result)
        return result;

    discovery::discovery_manager::instance().unsubscribe_from_port(subs_id_);

    return result;
}

rx_result system_server_port_base::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    RX_ASSERT(!listening_);
    listening_ = true;
    if (system_port_)
    {
        tcp_server_port::start_listen(nullptr, nullptr);
    }
    return true;
}

rx_result system_server_port_base::stop_passive ()
{
    if(listening_)
    {
        listening_ = false;
        if (system_port_)
        {
            tcp_server_port::stop_passive();
        }
    }
    return true;
}

void system_server_port_base::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
    if (local_addr.is_null())
    {
        auto addr = binder_data.get_value("Bind.IPAddress");
        auto port = binder_data.get_value("Bind.IPPort");
        string_type addr_str;
        uint16_t port_val = 0;
        if (!addr.is_null())
        {
            addr_str = addr.get_string();
            addr_str = rx_gate::instance().resolve_ip4_alias(addr_str);
        }
        if (!port.is_null())
            port_val = (uint16_t)port.get_unassigned();
        if (!addr_str.empty() || port_val != 0)
        {
            io::ip4_address ip_addr(addr_str, port_val);
            if (ip_addr.is_valid())
                local_addr = &ip_addr;
        }
    }
}


// Class rx_internal::interfaces::ip_endpoints::system_opcua_port 


uint16_t system_opcua_port::get_configuration_port () const
{
    auto ret = rx_gate::instance().get_configuration().other.opcua_port;
    if (ret == 0)
        ret = 0x7ABE;
    return ret;
}


// Class rx_internal::interfaces::ip_endpoints::system_wsrx_port 


uint16_t system_wsrx_port::get_configuration_port () const
{
    auto ret = rx_gate::instance().get_configuration().other.rx_wsport;
    if (ret == 0)
        ret = 0x7ABF;
    return ret;
}


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal

