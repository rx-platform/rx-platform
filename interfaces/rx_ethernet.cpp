

/****************************************************************************
*
*  interfaces\rx_ethernet.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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
#include "rx_ether_defs.h"


namespace rx_internal {

namespace interfaces {

namespace ethernet {
std::map<string_type, byte_string> local_mac;
locks::slim_lock mac_lock;

void rebuild_addresses()
{
    ETH_interface* interfaces = nullptr;
    size_t count = 0;
    HOST_LOG_INFO("ethernet", 200, "Listing ethernet adapters...");
    auto res = rx_list_eth_cards(&interfaces, &count);
    if (res == RX_OK)
    {
        local_mac.clear();
        for (size_t i = 0; i < count; i++)
        {
            std::ostringstream ss;
            ss << "Ethernet card detected: ";

            for (int j = 0; j < MAC_ADDR_SIZE; j++)
            {
                if (j > 0)
                    ss << '-';
                char buff[0x04];
                sprintf(buff, "%02x", (int)interfaces[i].mac_address[j]);
                ss << buff;
            }
            ss << "; name:"
                << interfaces[i].name
                << " - "
                << interfaces[i].description;

            local_mac.emplace(interfaces[i].name, byte_string((std::byte*)interfaces[i].mac_address, (std::byte*)interfaces[i].mac_address + MAC_ADDR_SIZE));
            HOST_LOG_INFO("ethernet", 200, ss.str());
        }
        HOST_LOG_INFO("ethernet", 200, "Ethernet adapters list done.");
    }
    else
    {
        HOST_LOG_ERROR("ethernet", 200, "Error listing ethernet adapters.");
    }
}
bool fill_mac_address_internal(const char* port, void* buff, bool first)
{
    std::scoped_lock _(mac_lock);
    auto it = local_mac.find(port);
    if (it != local_mac.end())
    {
        memcpy(buff, &it->second[0], MAC_ADDR_SIZE);
        return true;
    }
    else if(first)
    {
        rebuild_addresses();
        return fill_mac_address_internal(port, buff, false);
    }
    return false;
}
bool fill_mac_address(const char* port, void* buff)
{
    return fill_mac_address_internal(port, buff, true);
}

// Class rx_internal::interfaces::ethernet::ethernet_endpoint

ethernet_endpoint::ethernet_endpoint()
      : my_port_(nullptr),
        identity_(security::security_context_ptr::null_ptr),
        handle_(0),
        stop_(false)
    , thread("EthernetWorker", 0)
    , local_mac_(MAC_ADDR_SIZE)
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
    if (packet.buffer)
    {
        if (packet.to_addr && packet.to_addr->type == protocol_address_mac)
        {
            // append the addresses
            rx_protocol_result_t result;
            uint8_t* front = (uint8_t*)rx_alloc_from_packet_front(packet.buffer, 2 * MAC_ADDR_SIZE/*two mac addresses*/, &result);
            memcpy(front, &packet.to_addr->value.mac_address, MAC_ADDR_SIZE);
            /*if (packet.from_addr && packet.from_addr->type == protocol_address_mac)
            {
                memcpy(front + MAC_ADDR_SIZE, &packet.from_addr->value.mac_address, MAC_ADDR_SIZE);
            }
            else*/
            {
                memcpy(front + MAC_ADDR_SIZE, &local_mac_[0], MAC_ADDR_SIZE);
            }
            auto ret = rx_send_ethernet_packet(handle_, packet.buffer->buffer_ptr, packet.buffer->size);
            if (ret != RX_OK)
            {
                return RX_PROTOCOL_DISCONNECTED;
            }
        }
        else
        {
            return RX_PROTOCOL_INVALID_ADDR;
        }
    }
    return RX_PROTOCOL_OK;
}

void ethernet_endpoint::release_buffer (buffer_ptr what)
{
    return my_port_->release_buffer(what);
}

rx_result ethernet_endpoint::open (const string_type& port_name, security::security_context_ptr identity, ethernet_port* port, const std::vector<uint16_t>& ether_types)
{
    RX_ASSERT(handle_ == 0);
    my_port_ = port;
    ether_types_ = ether_types;
    if (!rx_create_ethernet_socket(port_name.c_str(), &handle_))
    {
        char buff[0x100];
        rx_last_os_error(("Error opening port "s + port_name+".").c_str(), buff, sizeof(buff));
        ITF_LOG_ERROR("ethernet_endpoint", 900, "Error opening ethernet card:"s + buff);
        my_port_ = nullptr;
        return buff;
    }
    if (!fill_mac_address(port_name.c_str(), &local_mac_[0]))
    {

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
                process_packet(buffer, psize);
            }

        } while (more);

        rx_ms_sleep(1);
    }

    ITF_LOG_INFO("ethernet_endpoint", 800, info_head + " stopped.");
    return 0;
}

void ethernet_endpoint::process_packet (const uint8_t* buffer, size_t size)
{
    static const uint16_t vlan_eth_type = ntohs(ETH_TYPE_VLAN);
    const uint8_t* packet_rest;
    size_t rest_count = 0;
    uint16_t vlan = 0;
    uint16_t priority = 0;
    bool dei = false;
    uint16_t ether_type = 0;

    if (size > sizeof(eth_vlan_header))
    {
        // QinQ not supported for now!!!
        eth_vlan_header* eth_head = (eth_vlan_header*)buffer;
        if (eth_head->vlan_type == vlan_eth_type)
        {// we have VLAN tag
            vlan = eth_head->vlan & 0x3fff;
            priority = eth_head->vlan & 0xe000;
            dei = eth_head->vlan & 0x1000;
            packet_rest = &buffer[sizeof(eth_vlan_header) - sizeof(uint16_t)];
            rest_count = size - (sizeof(eth_vlan_header) - sizeof(uint16_t));
            ether_type = ntohs(eth_head->eth_type);
        }
        else
        {// no VLAN tag
            packet_rest = &buffer[sizeof(eth_header) - sizeof(uint16_t)];
            rest_count = size - (sizeof(eth_header) - sizeof(uint16_t));
            ether_type = ntohs(eth_head->vlan_type);
        }
        for (auto one : ether_types_)
        {
            if (one == ether_type)
            {// found matching ethernet type

                io::mac_address from(eth_head->source_mac);
                io::mac_address to(eth_head->destination_mac);

                rx_const_packet_buffer packet_buffer{};
                rx_init_const_packet_buffer(&packet_buffer, packet_rest, rest_count);

                auto packet = rx_create_recv_packet(0, &packet_buffer, 0, 0);
                packet.from_addr = &from;
                packet.to_addr = &to;

                my_port_->status.received_packet(size);

                auto stack_result = rx_move_packet_up(&stack_endpoint_, packet);
            }
        }
    }
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
    auto sec_ctx = get_security_context();
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
    {
        ethernet_endpoint* whose = reinterpret_cast<ethernet_endpoint*>(entry->user_data);
        auto port = whose->get_port();
        if(port)
            port->disconnect_stack_endpoint(entry);
    };
    auto result = endpoint_->open(port_name_, sec_ctx, this, ether_types_);
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
    auto sec_ctx = get_security_context();
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
    {
        ethernet_endpoint* whose = reinterpret_cast<ethernet_endpoint*>(entry->user_data);
        auto port = whose->get_port();
        if(port)
            port->disconnect_stack_endpoint(entry);
    };
    auto result = endpoint_->open(port_name_, sec_ctx, this, ether_types_);
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

