

/****************************************************************************
*
*  protocols\http\rx_web_socket_mapping.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


// rx_web_socket_mapping
#include "protocols/http/rx_web_socket_mapping.h"



namespace protocols {

namespace rx_http {

// Class protocols::rx_http::rx_web_socket_endpoint 

rx_web_socket_endpoint::rx_web_socket_endpoint (rx_web_socket_port* port)
      : port_(port)
{
}


rx_web_socket_endpoint::~rx_web_socket_endpoint()
{
}



rx_protocol_result_t rx_web_socket_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t rx_web_socket_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t rx_web_socket_endpoint::transport_connected (rx_protocol_stack_endpoint* reference, const protocol_address* local_address, const protocol_address* remote_address)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

void rx_web_socket_endpoint::close_endpoint ()
{
}


// Class protocols::rx_http::rx_web_socket_port 

rx_web_socket_port::rx_web_socket_port()
{
    construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
    {
        auto rt = std::make_unique<rx_web_socket_endpoint>(this);
        return construct_func_type::result_type{ &rt->stack_entry, std::move(rt) };
    };
}



rx_result rx_web_socket_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = rx_web_socket_port_base::initialize_runtime(ctx);

    return result;
}

rx_result rx_web_socket_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    auto result = rx_web_socket_port_base::deinitialize_runtime(ctx);

    return result;
}

void rx_web_socket_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
    if (local_addr.is_null())
    {
        string_type str_addr;

        auto ascii = binder_data.get_value("Bind.Endpoint");
        if (!ascii.is_null() && ascii.is_string())
        {
            str_addr = ascii.get_string();
            io::string_address addr(str_addr);
            local_addr = &addr;
        }
    }
}


} // namespace rx_http
} // namespace protocols

