

/****************************************************************************
*
*  protocols\opcua\rx_opcua_security.cpp
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


// rx_opcua_security
#include "protocols/opcua/rx_opcua_security.h"

#include "protocols/opcua/rx_opcua_mapping.h"
#include "rx_opcua_resources.h"


namespace protocols {

namespace opcua {

namespace opcua_transport {

// Class protocols::opcua::opcua_transport::opcua_sec_none_endpoint 

opcua_sec_none_endpoint::opcua_sec_none_endpoint (opcua_sec_none_port* port)
      : port_(port)
{
    OPCUA_LOG_DEBUG("opcua_sec_none_endpoint", 200, "OPC UA server Security None endpoint created.");
    opcua_sec_none_protocol_type* mine_entry = this;

    rx_protocol_result_t res = opcua_bin_init_sec_none_server_transport(mine_entry);
    if (res == RX_PROTOCOL_OK)
    {
        mine_entry->stack_entry.received_function = &opcua_sec_none_endpoint::received_function;
        mine_entry->stack_entry.send_function = &opcua_sec_none_endpoint::send_function;
        mine_entry->transport_connected = &opcua_sec_none_endpoint::transport_connected;
    }
}


opcua_sec_none_endpoint::~opcua_sec_none_endpoint()
{
    OPCUA_LOG_DEBUG("opcua_sec_none_endpoint", 200, "OPC UA server Security None endpoint destroyed.");
}



rx_protocol_result_t opcua_sec_none_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    return opcua_bin_sec_none_bytes_received(reference, packet);
}

rx_protocol_result_t opcua_sec_none_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    return opcua_bin_sec_none_bytes_send(reference, packet);
}

rx_protocol_result_t opcua_sec_none_endpoint::transport_connected (rx_protocol_stack_endpoint* reference, const protocol_address* local_address, const protocol_address* remote_address)
{
    opcua_sec_none_endpoint* me = reinterpret_cast<opcua_sec_none_endpoint*>(reference->user_data);
    if (local_address)
    {
        const char* addr = nullptr;
        rx_extract_string_address(local_address, &addr);
        if (addr)
        {
            opcua_split_url(addr, me->url_address_, me->url_path_);
        }
    }
    io::string_address addr(me->url_path_);
    auto result = me->port_->stack_endpoint_connected(reference, &addr, remote_address);
    if (!result)
    {
        std::ostringstream ss;
        ss << "Error binding connected endpoint ";
        ss << result.errors_line();
        OPCUA_LOG_ERROR("opcua_sec_none_endpoint", 200, ss.str().c_str());
        return RX_PROTOCOL_INVALID_ADDR;
    }
    return RX_PROTOCOL_OK;
}


// Class protocols::opcua::opcua_transport::opcua_sec_none_port 

std::map<rx_node_id, opcua_sec_none_port::smart_ptr> opcua_sec_none_port::runtime_instances;

opcua_sec_none_port::opcua_sec_none_port()
{
    construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
    {
        auto rt = std::make_unique<opcua_sec_none_endpoint>(this);
        return construct_func_type::result_type{ &rt->stack_entry, std::move(rt) };
    };
}



rx_result opcua_sec_none_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    auto result = opcua_sec_none_base::initialize_runtime(ctx);
    if (result)
    {
        registered_endpoint_data data;

        data.security_mode = security_mode_t::none;
        data.policy_uri = "http://opcfoundation.org/UA/SecurityPolicy#None";
        data.security_level = 0;
        data.transport_profile_uri = "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";

        opcua_resources_repository::instance().register_endpoint(ctx.meta.get_full_path(), data);
    }
    return result;
}

rx_result opcua_sec_none_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    auto result = opcua_sec_none_base::deinitialize_runtime(ctx);

    opcua_resources_repository::instance().unregister_endpoint(ctx.meta.get_full_path());

    return result;
}

void opcua_sec_none_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
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


// Class protocols::opcua::opcua_transport::opcua_sec_none_client_port 

std::map<rx_node_id, opcua_sec_none_client_port::smart_ptr> opcua_sec_none_client_port::runtime_instances;

opcua_sec_none_client_port::opcua_sec_none_client_port()
{
    construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
    {
        auto rt = std::make_unique<opcua_sec_none_client_endpoint>(this);
        return construct_func_type::result_type{ &rt->stack_entry, std::move(rt) };
    };
}



void opcua_sec_none_client_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
    if (remote_addr.is_null())
    {
        string_type str_addr;

        auto ascii = binder_data.get_value("Connect.Endpoint");
        if (!ascii.is_null() && ascii.is_string())
        {
            str_addr = ascii.get_string();
            io::string_address addr(str_addr);
            remote_addr = &addr;
        }
    }
}


// Class protocols::opcua::opcua_transport::opcua_sec_none_client_endpoint 

opcua_sec_none_client_endpoint::opcua_sec_none_client_endpoint (opcua_sec_none_client_port* port)
      : port_(port)
{
    OPCUA_LOG_DEBUG("opcua_sec_none_client_endpoint", 200, "OPC UA server Security None client endpoint created.");
    opcua_sec_none_protocol_type* mine_entry = this;

    rx_protocol_result_t res = opcua_bin_init_sec_none_client_transport(mine_entry);
    if (res == RX_PROTOCOL_OK)
    {
        mine_entry->stack_entry.received_function = &opcua_sec_none_client_endpoint::received_function;
        mine_entry->stack_entry.send_function = &opcua_sec_none_client_endpoint::send_function;
        mine_entry->stack_entry.disconnected_function = &opcua_sec_none_client_endpoint::disconnected_function;
    }
}


opcua_sec_none_client_endpoint::~opcua_sec_none_client_endpoint()
{
    OPCUA_LOG_DEBUG("opcua_sec_none_client_endpoint", 200, "OPC UA server Security None client endpoint destroyed.");
}



rx_protocol_result_t opcua_sec_none_client_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    return opcua_bin_sec_none_bytes_received(reference, packet);
}

rx_protocol_result_t opcua_sec_none_client_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    return opcua_bin_sec_none_bytes_send(reference, packet);
}

rx_protocol_result_t opcua_sec_none_client_endpoint::disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason)
{
    auto ret = opcua_sec_none_client_disconnected(reference, session, reason);
    if (ret == RX_PROTOCOL_OK)
    {
        ret = rx_notify_disconnected(reference, session, reason);
    }
    return ret;
}


} // namespace opcua_transport
} // namespace opcua
} // namespace protocols

