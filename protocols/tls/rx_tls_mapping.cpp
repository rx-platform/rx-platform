

/****************************************************************************
*
*  protocols\tls\rx_tls_mapping.cpp
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


// rx_tls_mapping
#include "protocols/tls/rx_tls_mapping.h"

#include "os_itf/rx_ositf.h"
#include "sys_internal/rx_security/rx_x509_security.h"


namespace protocols {

namespace rx_tls {

// Class protocols::rx_tls::tls_transport_endpoint

tls_transport_endpoint::tls_transport_endpoint (tls_transport_port* port, x509_certificate_ptr& cert)
      : port_(port),
        executer_(-1),
        connected_(false),
        aquired_(false),
        out_buffer_(nullptr)
{
    TLS_LOG_DEBUG("tls_transport_endpoint", 200, "TLS communication server endpoint created.");

	rx_init_stack_entry(&stack_entry_, this);
	stack_entry_.received_function = &tls_transport_endpoint::received_function;
    stack_entry_.send_function = &tls_transport_endpoint::send_function;

	executer_ = port->get_executer();


    rx_time_struct time;
    if (RX_OK == rx_aquire_cert_credentials(&cred_, &time, cert->get_certificate_object()))
    {
        rx_init_auth_context(&auth_ctx_);
        aquired_ = true;
        out_buffer_ = rx_allocate_os_memory(cred_.buffer_size);
    }


}


tls_transport_endpoint::~tls_transport_endpoint()
{
    if (peer_context_)
    {
        peer_context_->logout();
        peer_context_ = x509_security_context::smart_ptr::null_ptr;
    }
    if (aquired_)
    {
        size_t buff_size=cred_.buffer_size;
        rx_deinit_auth_context(&auth_ctx_);
        rx_release_credentials(&cred_);
        aquired_ = false;
        rx_deallocate_os_memory(out_buffer_, buff_size);
    }
    TLS_LOG_DEBUG("tls_transport_endpoint", 200, "TLS communication server endpoint destroyed.");
}



rx_protocol_result_t tls_transport_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    tls_transport_endpoint* me = reinterpret_cast<tls_transport_endpoint*>(reference->user_data);
    if (!me->connected_)
    {
        size_t out_size = me->cred_.buffer_size;
        auto res = rx_accept_credentials(&me->cred_, &me->auth_ctx_, packet.buffer->buffer_ptr, rx_get_packet_available_data(packet.buffer), me->out_buffer_, &out_size);
        if (res == RX_OK)
        {
            rx_protocol_result_t result = RX_PROTOCOL_OK;
            if (out_size > 0)
            {
                auto buffer = me->port_->alloc_io_buffer();
                if (buffer)
                {
                    buffer.value().write(me->out_buffer_, out_size);
                    auto send_packet = rx_create_send_packet(0, &buffer.value(), 0, 0);
                    result = rx_move_packet_down(reference, send_packet);

                    me->port_->release_io_buffer(buffer.move_value());
                }
                else
                {
                    return RX_PROTOCOL_OUT_OF_MEMORY;
                }
            }
            if (result == RX_PROTOCOL_OK && rx_is_auth_context_complete(&me->auth_ctx_))
            {
                rx_certificate_t cert;
                if (RX_OK == rx_get_peer_certificate(&me->auth_ctx_, &cert))
                {
                    auto cert_ctx = rx_create_reference<x509_security_context>();
                    cert_ctx->set_certificate(&cert);
                    if (cert_ctx->login())
                    {
                        me->peer_context_ = cert_ctx;
                    }
                }
                security::secured_scope _(me->peer_context_);
                res = transport_connected(reference, packet.to_addr, packet.from_addr);
                if (res != RX_PROTOCOL_OK)
                    return res;
                me->connected_ = true;


            }
            return RX_PROTOCOL_OK;
        }
        else
        {
            return RX_PROTOCOL_INSUFFICIENT_DATA;
        }
    }
    else
    {
        size_t out_size = me->cred_.buffer_size;;

        if (RX_OK == rx_decrypt_message(&me->auth_ctx_, packet.buffer->buffer_ptr, rx_get_packet_available_data(packet.buffer), me->out_buffer_, &out_size))
        {
            rx_const_packet_buffer up_buffer{ 0 };
            rx_init_const_packet_buffer(&up_buffer, me->out_buffer_, out_size);
            auto up_pack = rx_create_recv_packet(0, &up_buffer, 0, 0);
            security::secured_scope _(me->peer_context_);
            return rx_move_packet_up(reference, up_pack);
        }
        else
        {
            return RX_PROTOCOL_INSUFFICIENT_DATA;
        }
    }
}

rx_protocol_result_t tls_transport_endpoint::send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet)
{
    tls_transport_endpoint* me = reinterpret_cast<tls_transport_endpoint*>(reference->user_data);
    if (!me->connected_)
    {
        return RX_PROTOCOL_DISCONNECTED;
    }
    else
    {
        size_t processed = 0;
        size_t packet_size = rx_get_packet_usable_data(packet.buffer);

        rx_protocol_result_t result = RX_PROTOCOL_OUT_OF_MEMORY;

        while (processed < packet_size)
        {
            auto buffer = me->port_->alloc_io_buffer();
            result = RX_PROTOCOL_OUT_OF_MEMORY;
            if (buffer)
            {
                result = RX_PROTOCOL_OK;
                size_t out_size = me->cred_.buffer_size;
                void* buffer_ptr = rx_alloc_from_packet(&buffer.value(), out_size, &result);
                if (buffer_ptr && result == RX_PROTOCOL_OK)
                {
                    size_t proc_temp = 0;
                    if (RX_OK == rx_encrypt_message(&me->auth_ctx_, packet.buffer->buffer_ptr + processed, packet_size - processed, &proc_temp, buffer_ptr, &out_size))
                    {
                        processed += proc_temp;
                        buffer.value().size = out_size;
                        auto down_pack = rx_create_send_packet(0, &buffer.value(), 0, 0);
                        result = rx_move_packet_down(reference, down_pack);
                    }
                    else
                    {
                        result = RX_PROTOCOL_INSUFFICIENT_DATA;
                    }
                }
                me->port_->release_io_buffer(buffer.move_value());
                if (result != RX_PROTOCOL_OK)
                    break;
            }
        }
        return result;
    }
    return rx_move_packet_down(reference, packet);
}

rx_protocol_stack_endpoint* tls_transport_endpoint::bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{
	return &stack_entry_;
}

void tls_transport_endpoint::close_endpoint ()
{
    if (peer_context_)
    {
        peer_context_->logout();
        peer_context_ = x509_security_context::smart_ptr::null_ptr;
    }
}

rx_protocol_result_t tls_transport_endpoint::transport_connected (rx_protocol_stack_endpoint* reference, const protocol_address* local_address, const protocol_address* remote_address)
{
    tls_transport_endpoint* me = reinterpret_cast<tls_transport_endpoint*>(reference->user_data);
    if (local_address)
    {
        const char* addr = nullptr;
        rx_extract_string_address(local_address, &addr);
        if (addr)
        {
        }
    }
    io::string_address addr("");
    auto result = me->port_->stack_endpoint_connected(reference, &addr, remote_address);
    if (!result)
    {
        std::ostringstream ss;
        ss << "Error binding connected endpoint ";
        ss << result.errors_line();
        //OPCUA_LOG_ERROR("opcua_sec_none_endpoint", 200, ss.str().c_str());
        return RX_PROTOCOL_INVALID_ADDR;
    }
    return RX_PROTOCOL_OK;
}


// Class protocols::rx_tls::tls_transport_port

tls_transport_port::tls_transport_port()
{
	construct_func = [this](const protocol_address* local_address, const protocol_address* remote_address)
	{
        if (cert_)
        {
            auto rt = std::make_unique<tls_transport_endpoint>(this, cert_);
            auto entry = rt->bind_endpoint([this](int64_t count)
                {
                },
                [this](int64_t count)
                {
                });
            return construct_func_type::result_type{ entry, std::move(rt) };
        }
        else
        {
            io::any_address temp_addr1(remote_address);
            io::any_address temp_addr2(local_address);
            TLS_LOG_ERROR("", 300, "No valid certificate defined on endpoint "s + temp_addr2.to_string() + " for remote client "s + temp_addr1.to_string());
            return construct_func_type::result_type{ nullptr, std::unique_ptr<tls_transport_endpoint>() };
        }
	};
}


tls_transport_port::~tls_transport_port()
{
}



rx_result tls_transport_port::initialize_runtime (runtime::runtime_init_context& ctx)
{
    string_type cert_name = ctx.get_item_static<string_type>("Options.Certificate", "");
    if (!cert_name.empty())
    {
        auto result = rx_internal::rx_security::x509::x509_certificates::instance().get_certificate(cert_name);
        if(result)
        {
            cert_ = result.move_value();
        }
        else
        {
            TLS_LOG_WARNING("", 500, "Can not find certificate "s + cert_name + " (" + result.errors_line() + ") for TLS endpoint " + ctx.meta.get_full_path());
        }
    }
    else
    {
        TLS_LOG_WARNING("", 500, "No certificate defined for TLS endpoint " + ctx.meta.get_full_path());
    }
    return true;
}

rx_result tls_transport_port::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    return true;
}


} // namespace rx_tls
} // namespace protocols

