

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


namespace interfaces {

namespace ip_endpoints {

// Class interfaces::ip_endpoints::rx_udp_endpoint 

rx_udp_endpoint::rx_udp_endpoint()
{
}



rx_result rx_udp_endpoint::open (const string_type& addr, uint16_t port)
{
	return "Nisam jos implementirao";
}

rx_protocol_result_t rx_udp_endpoint::received_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer)
{
	return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_result rx_udp_endpoint::close ()
{
	return "Nisam jos implementirao";
}


// Class interfaces::ip_endpoints::udp_port 

udp_port::udp_port()
{
}



rx_result udp_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = physical_port::initialize_runtime(ctx);
	if (result)
	{
	}
	return result;
}

rx_result udp_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	rx_result result;

	result = physical_port::deinitialize_runtime(ctx);
	return result;
}

rx_result udp_port::start_runtime (runtime::runtime_start_context& ctx)
{
	auto result = physical_port::start_runtime(ctx);
	if (result)
	{
		udp_socket_ = rx_create_reference<socket_holder_t>(this);
		string_type addr = ctx.structure.get_root()->get_local_as<string_type>("Bind.IPAddress", "");
		uint16_t port = ctx.structure.get_root()->get_local_as<uint16_t>("Bind.IPPort", 0);
		result = udp_socket_->bind_socket_udpip_4(port, addr, rx_gate::instance().get_infrastructure().get_io_pool()->get_pool());
	}
	return result;
}

rx_result udp_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
	rx_result result;
	udp_socket_->close();
	udp_socket_->disconnect();
	udp_socket_ = rx_reference<socket_holder_t>::null_ptr;
	result = physical_port::stop_runtime(ctx);
	return result;
}


// Class interfaces::ip_endpoints::tcp_server_port 

tcp_server_port::tcp_server_port()
      : rx_recv_timeout_(0),
        rx_send_timeout_(0)
{
}



rx_result tcp_server_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = physical_port::initialize_runtime(ctx);
    if (result)
    {
        auto bind_result = ctx.tags->bind_item("Timeouts.ReceiveTimeout", ctx);
        if (bind_result)
            rx_recv_timeout_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Timeouts.ReceiveTimeout");
        bind_result = ctx.tags->bind_item("Timeouts.ReceiveTimeout", ctx);
        if (bind_result)
            rx_recv_timeout_ = bind_result.value();
        else
            RUNTIME_LOG_ERROR(meta_info().get_name(), 200, "Unable to bind to value Timeouts.ReceiveTimeout");
    }
    return result;
}

rx_result tcp_server_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    rx_result result;

    result = physical_port::deinitialize_runtime(ctx);
    return result;
}

rx_result tcp_server_port::start_runtime (runtime::runtime_start_context& ctx)
{
    auto result = physical_port::start_runtime(ctx);
    if (result)
    {
        string_type addr = ctx.structure.get_root()->get_local_as<string_type>("Bind.IPAddress", "");
        uint16_t port = ctx.structure.get_root()->get_local_as<uint16_t>("Bind.IPPort", 0);
        listen_socket_ = rx_create_reference<io::tcp_listent_std_buffer>([this](sys_handle_t handle, sockaddr_in* his, sockaddr_in* mine, rx_thread_handle_t thr)
            {
                connection_endpoint new_endpoint;
                auto ret_ptr = new_endpoint.open(this, handle, his, mine, rx_gate::instance().get_infrastructure().get_io_pool()->get_pool());
                if (ret_ptr)
                {
                    connections_.emplace(new_endpoint.get_tcp_socket(), std::move(new_endpoint));
                }
                return ret_ptr.value();
            });
        result = listen_socket_->start_tcpip_4(port, addr, rx_gate::instance().get_infrastructure().get_io_pool()->get_pool());

        //ctx.structure.get_root()->get_binded_as
    }
    return result;
}

rx_result tcp_server_port::stop_runtime (runtime::runtime_stop_context& ctx)
{
    listen_socket_->stop();
    for (auto& one : connections_)
    {
        one.second.close();
    }
    connections_.clear();

    auto result = physical_port::stop_runtime(ctx);
    return result;
}

void tcp_server_port::update_received_counters (size_t count)
{
}

void tcp_server_port::update_sent_counters (size_t count)
{
}

void tcp_server_port::remove_connection (const connection_endpoint& what)
{
    auto it = connections_.find(what.get_tcp_socket());
    if(it != connections_.end())
    {
        connections_.erase(it);
    }
}


// Class interfaces::ip_endpoints::connection_endpoint 

connection_endpoint::connection_endpoint()
    : my_port_(nullptr)
{
}

connection_endpoint::connection_endpoint (const string_type& remote_port, const string_type& local_port)
    : my_port_(nullptr)
{
}



rx_protocol_result_t connection_endpoint::received_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer)
{
    return true;
}

rx_result_with<connection_endpoint::endpoint_ptr> connection_endpoint::open (tcp_server_port* my_port, sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool& dispatcher)
{
    my_port_ = my_port;
    tcp_socket_ = rx_create_reference<socket_holder_t>(this, handle, addr, local_addr);

    remote_port_ = io::get_ip4_addr_string(addr);
    local_port_ = io::get_ip4_addr_string(local_addr);
    std::ostringstream ss;
    ss << "Client from IP4:"
        << remote_port_
        << " connected IP4:"
        << local_port_
        << ".";
    ITF_LOG_TRACE("tcp_server_port", 500, ss.str());

    return tcp_socket_;
}

rx_result connection_endpoint::close ()
{
    tcp_socket_->disconnect();
    my_port_ = nullptr;
    return true;
}

void connection_endpoint::disconnected ()
{
    std::ostringstream ss;
    ss << "Client from IP4:"
        << remote_port_
        << " disconnected from IP4:"
        << local_port_
        << ".";
    ITF_LOG_TRACE("tcp_server_port", 500, ss.str());
    if (my_port_)
        my_port_->remove_connection(*this);
}

bool connection_endpoint::readed (const void* data, size_t count)
{
    return true;
}


} // namespace ip_endpoints
} // namespace interfaces

