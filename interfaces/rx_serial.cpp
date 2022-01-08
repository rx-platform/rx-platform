

/****************************************************************************
*
*  interfaces\rx_serial.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_serial
#include "interfaces/rx_serial.h"

#include "rx_endpoints.h"


namespace rx_internal {

namespace interfaces {

namespace serial {

// Class rx_internal::interfaces::serial::serial_endpoint 

serial_endpoint::serial_endpoint()
      : my_port_(nullptr),
        current_state_(serial_state::not_active),
        identity_(security::security_context_ptr::null_ptr)
{
    ITF_LOG_DEBUG("serial_endpoint", 200, "Serial endpoint created.");
    rx_protocol_stack_endpoint* mine_entry = &stack_endpoint_;
    rx_init_stack_entry(mine_entry, this);

    mine_entry->send_function = &serial_endpoint::send_function;
}


serial_endpoint::~serial_endpoint()
{
    ITF_LOG_DEBUG("serial_endpoint", 200, "Serial endpoint destroyed.");
}



rx_protocol_result_t serial_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    serial_endpoint* self = reinterpret_cast<serial_endpoint*>(reference->user_data);
    if (self->serial_port_)
        return self->send_packet(packet);
    else
        return RX_PROTOCOL_CLOSED;
}

rx_protocol_stack_endpoint* serial_endpoint::get_stack_endpoint ()
{
    return &stack_endpoint_;
}

runtime::items::port_runtime* serial_endpoint::get_port ()
{
    return my_port_;
}

rx_protocol_result_t serial_endpoint::send_packet (send_protocol_packet packet)
{
    if (my_port_)
    {
        /*char buff[0x100];
        memcpy(buff, packet.buffer->buffer_ptr, packet.buffer->size);
        buff[packet.buffer->size] = '\0';
        printf("\r\n***********Sent:%s\r\n", buff);*/

        auto io_buffer = my_port_->get_buffer();
        io_buffer->push_data(packet.buffer->buffer_ptr, packet.buffer->size);
        bool ret = serial_port_->write(io_buffer);
        if (!ret)
            my_port_->release_buffer(io_buffer);
        return ret ? RX_PROTOCOL_OK : RX_PROTOCOL_COLLECT_ERROR;
    }
    else
        return RX_PROTOCOL_INVALID_SEQUENCE;
}

void serial_endpoint::disconnected (rx_security_handle_t identity)
{
    if (serial_port_)
    {
        serial_port_->disconnect();
        serial_port_ = port_holder_t::smart_ptr::null_ptr;

        if (current_state_ == serial_state::opened)
        {
            auto session = rx_create_session(nullptr, nullptr, 0, 0, nullptr);
            auto proto_result = rx_notify_disconnected(&stack_endpoint_, &session, RX_PROTOCOL_OK);
        }

    }
    if (current_state_ == serial_state::opened)
    {
        suspend_timer();
        current_state_ = serial_state::not_active;
        start_timer(false);
    }
}

bool serial_endpoint::readed (const void* data, size_t count, rx_security_handle_t identity)
{
    /*char bufft[0x100];
    memcpy(bufft, data, count);
    bufft[count] = '\0';
    printf("\r\n***********Recevied:%s\r\n", bufft);
    */
    rx_const_packet_buffer buff{};
    rx_init_const_packet_buffer(&buff, data, count);
    recv_protocol_packet up = rx_create_recv_packet(0, &buff, 0, 0);
    auto result = rx_move_packet_up(&stack_endpoint_, up);
    if (result == RX_PROTOCOL_OK)
        return true;
    else
        return true;
}

rx_result serial_endpoint::open (const serial_port_data_t& port_data, security::security_context_ptr identity, serial_port* port)
{
    identity_ = identity;
    identity_->login();
    my_port_ = port;
    port_data_ = port_data;
    auto res = check_port_data();
    if (res)
    {
        timer_ = my_port_->create_timer_function([this]()
            {
                if (!tick())
                {
                    suspend_timer();
                }
            });
        start_timer(true);
    }
    return res;
}

rx_result serial_endpoint::close ()
{
    if (timer_)
    {
        timer_->cancel();
        timer_ = rx_timer_ptr::null_ptr;
    }
    if (serial_port_)
    {
        serial_port_->disconnect();
        serial_port_ = rx_reference<port_holder_t>::null_ptr;
    }
    current_state_ = serial_state::closed;
    return true;
}

bool serial_endpoint::tick ()
{
    switch (current_state_)
    {
    case serial_state::not_active:
        {
            RX_ASSERT(!serial_port_ && my_port_ != nullptr);
            rx_result result;
            serial_port_ = rx_create_reference<port_holder_t>(this);
            serial_port_->set_identity(stack_endpoint_.identity);
            if (!port_data_.port.empty())
            {
                result = serial_port_->open_serial(port_data_, infrastructure::server_runtime::instance().get_io_pool()->get_pool());
                if (result)
                {
                    ITF_LOG_INFO("serial_endpoint", 200, "Serial port "s + port_data_.port + " opened.");
                    current_state_ = serial_state::opened;
                    auto session = rx_create_session(nullptr, nullptr, 0, 0, nullptr);

                    auto proto_result = rx_notify_connected(&stack_endpoint_, &session);

                    return false;// we're done here nothing more to do
                }
                else
                {
                    if (serial_port_)
                    {
                        serial_port_->disconnect();
                        serial_port_ = port_holder_t::smart_ptr::null_ptr;
                    }
                    ITF_LOG_ERROR("serial_endpoint", 200, "Error opening serial port "s + port_data_.port + "." + result.errors_line());
                    if (current_state_ != serial_state::not_active)
                    {                        
                        current_state_ = serial_state::not_active;
                    }
                    return true;
                }
            }
            else
            {
                RX_ASSERT(false);// this should not happened
                return false;// stop the timer
            }
        }
    case serial_state::opened:
        {
            return false;
        }
    case serial_state::closed:
        {
            return false;
        }
    default:
        RX_ASSERT(false);
        return false;
    }
}

void serial_endpoint::start_timer (bool fire_now)
{
    if (my_port_ && timer_)
    {
        timer_->start(my_port_->get_reconnect_timeout(), fire_now);
    }
}

void serial_endpoint::suspend_timer ()
{
    if (timer_)
    {
        timer_->suspend();
    }
}

bool serial_endpoint::is_connected () const
{
    return current_state_ == serial_state::opened;
}

void serial_endpoint::release_buffer (buffer_ptr what)
{
    return my_port_->release_buffer(what);
}

rx_result serial_endpoint::check_port_data ()
{
    if (port_data_.port.empty())
        return "Invalid port name specified";

    return true;
}


void serial_endpoint::port_holder_t::release_buffer(buffer_ptr what)
{
    if (whose)
        return whose->release_buffer(what);
}
bool serial_endpoint::port_holder_t::readed(const void* data, size_t count, rx_security_handle_t identity)
{
    security::secured_scope _(identity);
    if (whose)
        whose->readed(data, count, identity);
    return true;
}
void serial_endpoint::port_holder_t::on_shutdown(rx_security_handle_t identity)
{
    if (whose)
    {
        security::secured_scope _(identity);
        whose->disconnected(identity);
    }
}
serial_endpoint::port_holder_t::port_holder_t(serial_endpoint* whose)
    : whose(whose)
{
}
serial_endpoint::port_holder_t::port_holder_t(port_holder_t&& right) noexcept
{
    whose = right.whose;
    right.whose = nullptr;
}
void serial_endpoint::port_holder_t::disconnect()
{
    whose = nullptr;
    initiate_shutdown();
}
// Class rx_internal::interfaces::serial::serial_port 

serial_port::serial_port()
      : recv_timeout_(2000),
        send_timeout_(1000),
        reconnect_timeout_(5000)
{
}



rx_result serial_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    

    port_data_.port = rx_gate::instance().resolve_serial_alias(ctx.get_item_static("Options.Port", ""s));
    port_data_.baud_rate = ctx.get_item_static<uint32_t>("Options.BaudRate", 19200);
    port_data_.data_bits = ctx.get_item_static<uint8_t>("Options.DataBits", 8);
    port_data_.stop_bits = ctx.get_item_static<uint8_t>("Options.StopBits", 0);
    port_data_.parity = ctx.get_item_static<uint8_t>("Options.Parity", 0);
    port_data_.handshake = ctx.get_item_static<bool>("Options.Handshake", false);
   
    return true;
}

uint32_t serial_port::get_reconnect_timeout () const
{
    return (uint32_t)reconnect_timeout_;
}

rx_result serial_port::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    auto session_timeout = recv_timeout_;
    endpoint_ = std::make_unique<serial_endpoint>();
    auto sec_result = create_security_context();
    if (!sec_result)
    {
        sec_result.register_error("Unable to create security context");
        return sec_result.errors();
    }
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
    {
        serial_endpoint* whose = reinterpret_cast<serial_endpoint*>(entry->user_data);
        whose->get_port()->disconnect_stack_endpoint(entry);
    };
    auto result = endpoint_->open(port_data_, sec_result.value(), this);
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

rx_result_with<port_connect_result> serial_port::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
    auto session_timeout = recv_timeout_;
    endpoint_ = std::make_unique<serial_endpoint>();
    auto sec_result = create_security_context();
    if (!sec_result)
    {
        sec_result.register_error("Unable to create security context");
        return sec_result.errors();
    }
    endpoint_->get_stack_endpoint()->closed_function = [](rx_protocol_stack_endpoint* entry, rx_protocol_result_t result)
    {
        serial_endpoint* whose = reinterpret_cast<serial_endpoint*>(entry->user_data);
        whose->get_port()->disconnect_stack_endpoint(entry);
    };
    auto result = endpoint_->open(port_data_, sec_result.value(), this);
    if (!result)
    {
        stop_passive();
        return result.errors();
    }
    return port_connect_result(endpoint_->get_stack_endpoint(), endpoint_->is_connected());
}

rx_result serial_port::stop_passive ()
{
    if (endpoint_)
        endpoint_->close();
    return true;
}

void serial_port::release_buffer (buffer_ptr what)
{
    locks::auto_lock_t _(&free_buffers_lock_);
    what->reinit();
    free_buffers_.push(what);
}

buffer_ptr serial_port::get_buffer ()
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


} // namespace serial
} // namespace interfaces
} // namespace rx_internal

