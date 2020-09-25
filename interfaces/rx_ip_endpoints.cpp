

/****************************************************************************
*
*  interfaces\rx_ip_endpoints.cpp
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


// rx_ip_endpoints
#include "interfaces/rx_ip_endpoints.h"

#include "system/server/rx_server.h"
#include "sys_internal/rx_security/rx_platform_security.h"
#include "system/runtime/rx_operational.h"


namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {

// Class rx_internal::interfaces::ip_endpoints::udp_port 

udp_port::udp_port()
      : session_timeout_(5000),
        rx_recv_timeout_(0),
        rx_send_timeout_(0)
{
}



rx_result udp_port::initialize_runtime (runtime::runtime_init_context& ctx)
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
    return true;
}

rx_result udp_port::start_runtime (runtime::runtime_start_context& ctx)
{
    session_timeout_ = ctx.context->get_binded_as(rx_recv_timeout_, session_timeout_);
	udp_socket_ = rx_create_reference<socket_holder_t>(this);
	string_type addr = ctx.structure.get_root().get_local_as<string_type>("Bind.IPAddress", "");
	uint16_t port = ctx.structure.get_root().get_local_as<uint16_t>("Bind.IPPort", 0);
    udp_socket_->set_identity(get_identity());
		
    auto result = udp_socket_->bind_socket_udpip_4(port, addr, rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool());

	return result;
}

rx_result udp_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
	udp_socket_->close();
	udp_socket_->disconnect();
	udp_socket_ = rx_reference<socket_holder_t>::null_ptr;
	return true;
}

bool udp_port::packet_arrived (const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity)
{
    return false;// cao
    //auto ticks = rx_get_tick_count();

    //security::secured_scope _(identity);

    //io::ip4_address ip4addr(addr);
    //if (!ip4addr.is_valid())
    //    return false;
    //rx_protocol_stack_endpoint* entry = nullptr;
    //// WARNING manual locking in code, be very careful!!!!!
    //sessions_lock_.lock();
    //auto it = sessions_.find(ip4addr);
    //if (it == sessions_.end())
    //{
    //    std::unique_ptr<udp_endpoint> ep = std::make_unique<udp_endpoint>();

    //    auto result = ep->open(ip4addr, this);
    //    if (result)
    //    {
    //        io::ip4_address local;
    //        entry = bind_stack_endpoint(std::move(ep), local, ip4addr);
    //        if (entry)
    //        {
    //            std::ostringstream ss;
    //            ss << "Registered UDP/IP4 session from "
    //                << ip4addr.to_string()
    //                << ".";
    //            ITF_LOG_TRACE("udp_port", 500, ss.str());
    //            sessions_.emplace(ip4addr, session_data_t{ entry, ticks });
    //        }
    //        else
    //        {
    //            ep->close();
    //        }
    //    }
    //}
    //else
    //{
    //    it->second.last_tick = ticks;
    //    entry = it->second.entry;
    //}
    //sessions_lock_.unlock();
    //if (entry)
    //{
    //    rx_const_packet_buffer buff{};
    //    rx_init_const_packet_buffer(&buff, data, count);
    //    recv_protocol_packet up = rx_create_recv_packet(0, &buff, 0, 0);
    //    auto result = rx_move_packet_up(entry, up);
    //    if (result == RX_PROTOCOL_OK)
    //        return true;
    //}
    //return false;
}

rx_protocol_result_t udp_port::send_function (rx_packet_buffer* buffer, const io::ip4_address& addr)
{
    auto io_buffer = rx_create_reference<socket_holder_t::buffer_t>();
    io_buffer->push_data(buffer->buffer_ptr, buffer->size);
    bool ret = udp_socket_->write(io_buffer, addr.get_address(), sizeof(sockaddr_in));
    return ret ? RX_PROTOCOL_OK : RX_PROTOCOL_COLLECT_ERROR;
}

void udp_port::timer_tick (uint32_t tick)
{
    std::vector<std::pair<io::ip4_address, rx_protocol_stack_endpoint*> > to_remove;
    sessions_lock_.lock();
    if (!sessions_.empty())
    {
        auto it = sessions_.begin();
        while(it!=sessions_.end())
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
    if(!to_remove.empty())
    {
        for (auto one : to_remove)
        {
            std::ostringstream ss;
            ss << "UDP/IP4 session from "
                << one.first.to_string()
                << " time-outed.";
            ITF_LOG_TRACE("udp_port", 500, ss.str());
        }
    }
}


// Class rx_internal::interfaces::ip_endpoints::udp_endpoint 

udp_endpoint::udp_endpoint()
    : my_port_(nullptr)
{
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);

    mine_entry->send_function = &udp_endpoint::send_function;
}


udp_endpoint::~udp_endpoint()
{
}



rx_result udp_endpoint::open (io::ip4_address addr, udp_port* port)
{
    addr_ = addr;
    my_port_ = port;
    return true;
}

rx_result udp_endpoint::close ()
{
    my_port_ = nullptr;
    return true;
}

rx_protocol_result_t udp_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    udp_endpoint* self = reinterpret_cast<udp_endpoint*>(reference->user_data);
    if (self->my_port_)
        return self->my_port_->send_function(packet.buffer, self->addr_);
    else
        return RX_PROTOCOL_WRONG_STATE;
}

rx_protocol_stack_endpoint* udp_endpoint::get_stack_endpoint ()
{
    return &stack_endpoint_;
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
// Class rx_internal::interfaces::ip_endpoints::tcp_server_endpoint 

tcp_server_endpoint::tcp_server_endpoint()
      : my_port_(nullptr)
{
    ITF_LOG_DEBUG("tcp_server_endpoint", 200, "TCP server endpoint created.");
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);
    mine_entry->send_function = &tcp_server_endpoint::send_function;
}


tcp_server_endpoint::~tcp_server_endpoint()
{
    ITF_LOG_DEBUG("tcp_server_endpoint", 200, "TCP server endpoint destroyed.");
}



rx_result_with<tcp_server_endpoint::socket_ptr> tcp_server_endpoint::open (tcp_server_port* my_port, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool& dispatcher, rx_security_handle_t identity)
{
    my_port_ = my_port;
    tcp_socket_ = rx_create_reference<socket_holder_t>(this, handle, addr, local_addr);
    tcp_socket_->set_identity(identity);
    remote_address_ = addr;
    local_address_ = local_addr;
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
    tcp_socket_->disconnect();
    my_port_ = nullptr;
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
    rx_notify_disconnected(&stack_endpoint_, nullptr, 0);
    rx_close(&stack_endpoint_, 0);
    if (my_port_)
        my_port_->remove_endpoint(&stack_endpoint_);
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
    }
    return true;
}

rx_protocol_result_t tcp_server_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    tcp_server_endpoint* self = reinterpret_cast<tcp_server_endpoint*>(reference->user_data);
    auto io_buffer = rx_create_reference<socket_holder_t::buffer_t>();
    io_buffer->push_data(packet.buffer->buffer_ptr, packet.buffer->size);
    bool ret = self->tcp_socket_->write(io_buffer);
    return ret ? RX_PROTOCOL_OK : RX_PROTOCOL_COLLECT_ERROR;
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

            auto ret_ptr = new_endpoint->open(this, handle, his, mine, rx_internal::infrastructure::server_runtime::instance().get_io_pool()->get_pool(), identity);
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
                active_endpoints_.emplace(stack_ptr, std::move(new_endpoint));
                auto result = bind_stack_endpoint(stack_ptr, &local_addr, &remote_addr);
                if (!result)
                {
                    active_endpoints_.erase(stack_ptr);
                    return io::tcp_socket_std_buffer::smart_ptr::null_ptr;
                }
            }
            return ret_ptr.value();

        });
    listen_socket_->set_identity(get_identity());
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

void tcp_server_port::stack_disassembled ()
{
    if(listen_socket_)
        listen_socket_->stop();
    for (auto& one : active_endpoints_)
    {
        one.second->close();
        rx_close(one.first, RX_PROTOCOL_OK);
    }
    active_endpoints_.clear();
}


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal

