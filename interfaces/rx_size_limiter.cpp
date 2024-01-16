

/****************************************************************************
*
*  interfaces\rx_size_limiter.cpp
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


// rx_size_limiter
#include "interfaces/rx_size_limiter.h"

#include "interfaces/rx_endpoints.h"


namespace rx_internal {

namespace interfaces {

namespace ports_lib {

// Class rx_internal::interfaces::ports_lib::size_limiter_port 

size_limiter_port::size_limiter_port()
      : limit_(65536)
{
    construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
    {
        auto rt = std::make_unique<size_limiter_endpoint>(this);
        auto entry = rt->bind();
        return construct_func_type::result_type{ entry, std::move(rt) };
    };
}



rx_result size_limiter_port::initialize_runtime (runtime_init_context& ctx)
{
    rx_result result = status.initialize(ctx);

    auto one_result = limit_.bind("Options.Limit", ctx);
    if (!one_result)
        result.register_error("Error connecting Options.Limit:"s + one_result.errors_line());

    if (!result)
    {
        ITF_LOG_WARNING("size_limiter_port", 200, "Error initializing "s
            + ctx.context->meta_info.get_full_path() + " status:"s + result.errors_line());
    }

    return true;
}

uint32_t size_limiter_port::get_limit () const
{
    return limit_;
}


// Class rx_internal::interfaces::ports_lib::size_limiter_endpoint 

size_limiter_endpoint::size_limiter_endpoint (size_limiter_port* port)
      : port_(port)
{
    ITF_LOG_DEBUG("size_limiter_endpoint", 200, "Size Limiter endpoint created.");
    rx_init_stack_entry(&stack_endpoint_, this);
    stack_endpoint_.send_function = [](rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
    {
        size_limiter_endpoint* self = reinterpret_cast<size_limiter_endpoint*>(reference->user_data);
        return self->send_packet(packet);
    };
}


size_limiter_endpoint::~size_limiter_endpoint()
{
    ITF_LOG_DEBUG("size_limiter_endpoint", 200, "Size Limiter endpoint destroyed.");
}



rx_protocol_stack_endpoint* size_limiter_endpoint::bind ()
{
    return &stack_endpoint_;
}

runtime::items::port_runtime* size_limiter_endpoint::get_port ()
{
    return port_;
}

rx_protocol_result_t size_limiter_endpoint::send_packet (send_protocol_packet packet)
{
    if (packet.buffer && packet.buffer->size > port_->get_limit())
    {
        uint8_t* buff = packet.buffer->buffer_ptr;
        size_t count = packet.buffer->size;
        size_t sent = 0;
        while (sent < count)
        {
            size_t chunk_size = std::min<size_t>(count - sent, 0x10000);
            auto io_buffer = port_->alloc_io_buffer();
            if (io_buffer)
            {
                io_buffer.value().write(&buff[sent], chunk_size);
                send_protocol_packet one_packet = packet;
                packet.buffer = &io_buffer.value();
                auto result = rx_move_packet_down(&stack_endpoint_, one_packet);
                port_->release_io_buffer(io_buffer.move_value());
                if (result != RX_PROTOCOL_OK)
                    return result;
                sent += chunk_size;
                port_->status.sent_packet();
            }
        }
        return RX_PROTOCOL_OK;
    }
    else
    {
        auto result = rx_move_packet_down(&stack_endpoint_, packet);
        if(result==RX_PROTOCOL_OK)
            port_->status.sent_packet();
        return result;
    }
}


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal

