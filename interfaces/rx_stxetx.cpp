

/****************************************************************************
*
*  interfaces\rx_stxetx.cpp
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


// rx_stxetx
#include "interfaces/rx_stxetx.h"

#define ASCII_STX '\002'
#define ASCII_ETX '\003'


namespace rx_internal {

namespace interfaces {

namespace ports_lib {

// Class rx_internal::interfaces::ports_lib::stxetx_endpoint 

stxetx_endpoint::stxetx_endpoint (stxetx_port* port)
      : port_(port),
        collecting_(false)
{
    rx_init_stack_entry(&stack_endpoint_, this);

    stack_endpoint_.send_function = [](rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
    {
        stxetx_endpoint* self = reinterpret_cast<stxetx_endpoint*>(reference->user_data);
        return self->send_packet(packet);
    };
    stack_endpoint_.received_function = [](rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
    {
        stxetx_endpoint* self = reinterpret_cast<stxetx_endpoint*>(reference->user_data);
        return self->received_packet(packet);
    };
}


stxetx_endpoint::~stxetx_endpoint()
{
}



rx_protocol_stack_endpoint* stxetx_endpoint::bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
    return &stack_endpoint_;
}

runtime::items::port_runtime* stxetx_endpoint::get_port ()
{
    return port_;
}

rx_protocol_result_t stxetx_endpoint::send_packet (send_protocol_packet packet)
{
    rx_protocol_result_t result = RX_PROTOCOL_OK;
    if (port_)
    {
        auto send_size = rx_get_packet_usable_data(packet.buffer);
        if (send_size + 2 <= port_->max_buffer_size)
        {
            uint8_t temp = ASCII_STX;
            result = rx_push_to_packet_front(packet.buffer, &temp, sizeof(temp));
            if (result == RX_PROTOCOL_OK)
            {
                temp = ASCII_ETX;
                result = rx_push_to_packet(packet.buffer, &temp, sizeof(temp));
                if (result == RX_PROTOCOL_OK)
                {
                    result = rx_move_packet_down(&stack_endpoint_, packet);
                }
            }
        }
        else
        {
            return RX_PROTOCOL_NOT_IMPLEMENTED;
            runtime::io_types::rx_io_buffer temp_buffer;
            size_t used = 0;
            while (used < send_size)
            {

            }
        }
    }
    return result;
}

rx_protocol_result_t stxetx_endpoint::received_packet (recv_protocol_packet packet)
{
    size_t packet_size = 0;
    bool collect_in_buffer = false;
    bool wait_stx = false;
    bool wait_etx = false;
    size_t packet_pos = 0;
    size_t last_start_pos = 0;
    rx_protocol_result_t proto_result = RX_PROTOCOL_OK;
    packet_size = rx_get_packet_available_data(packet.buffer);
    if (collecting_)
    {// we are already collecting so use collect buffer
        collect_in_buffer = true;
        wait_etx = true;
    }
    else
    {
        wait_stx = true;
    }
    if (packet_size > 0)
    {
        const uint8_t* data = (const uint8_t*)rx_get_from_packet(packet.buffer, packet_size, &proto_result);
        if (proto_result == RX_PROTOCOL_OK)
        {
            while (packet_pos < packet_size)
            {
                if (wait_stx)
                {
                    while (packet_pos < packet_size && data[packet_pos] != ASCII_STX)
                        packet_pos++;
                    if (data[packet_pos] == ASCII_STX)
                    {
                        wait_stx = false;
                        wait_etx = true;
                        packet_pos++;
                        last_start_pos = packet_pos;
                    }
                }
                else if (wait_etx)
                {
                    while (packet_pos < packet_size && data[packet_pos] != ASCII_ETX)
                        packet_pos++;
                    if (data[packet_pos] == ASCII_ETX)
                    {
                        if (packet_pos - last_start_pos > 0)
                        {
                            if (collecting_)
                            {// we are collecting into the buffer so fill it with rest of data
                                proto_result = rx_push_to_packet(&collect_buffer_, &data[last_start_pos], packet_pos - last_start_pos);
                                rx_const_packet_buffer my_buffer{};
                                proto_result = rx_init_const_from_packet_buffer(&my_buffer, &collect_buffer_);
                                if (proto_result == RX_PROTOCOL_OK)
                                {
                                    packet.buffer = &my_buffer;
                                    proto_result = rx_move_packet_up(&stack_endpoint_, packet);
                                }
                                collecting_ = false;
                                collect_buffer_.reinit();
                            }
                            else
                            {// we are collecting from arrived buffer so resend it from there
                                rx_const_packet_buffer my_buffer{};
                                proto_result = rx_init_const_packet_buffer(&my_buffer, &data[last_start_pos], packet_pos - last_start_pos);
                                if (proto_result == RX_PROTOCOL_OK)
                                {
                                    packet.buffer = &my_buffer;
                                    proto_result = rx_move_packet_up(&stack_endpoint_, packet);
                                }
                            }
                        }
                        wait_stx = true;
                        wait_etx = false;
                        packet_pos++;
                    }
                }
            }
            if (wait_etx)
            {// we didn't get whole packet so push it to the collect buffer
                proto_result = rx_push_to_packet(&collect_buffer_, &data[last_start_pos], packet_pos - last_start_pos);
                if (proto_result == RX_PROTOCOL_OK)
                {
                    collecting_ = true;
                }
            }
        }
    }
    return proto_result;
}


// Class rx_internal::interfaces::ports_lib::stxetx_port 

stxetx_port::stxetx_port()
      : max_buffer_size(0x1000)
{
    construct_func = [this]()
    {
        auto rt = std::make_unique<stxetx_endpoint>(this);
        auto entry = rt->bind([this](int64_t count)
            {
            },
            [this](int64_t count)
            {
            });
        return construct_func_type::result_type{ entry, std::move(rt) };
    };
}



rx_result stxetx_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    max_buffer_size = ctx.get_item_static(".Options.MaxPacketsize", 0x1000u);
    return true;
}


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal

