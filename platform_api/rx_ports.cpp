

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_ports.cpp
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


// rx_ports
#include "rx_ports.h"

#include "lib/rx_mem.h"
#include "lib/rx_ser_bin.h"
#include "lib/rx_io_addr.h"

typedef rx_platform_api::rx_port _rx_port_holder_stub;
rxRegisterPortRuntime_t api_reg_port_func;



extern "C"
{

    void c_get_code_info(void* whose, const char* name, string_value_struct* info);

    rx_result_struct c_init_port_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_port_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_port_stub(void* self);
    rx_result_struct c_deinit_port_stub(void* self);

    void c_stack_assembled_stub(void* whose);
    void c_stack_disassembled_stub(void* whose);

   void c_rx_destroy_endpoint_stub(void* whose, struct rx_protocol_stack_endpoint* endpoint);
   void c_rx_extract_bind_address_stub(void* whose, const uint8_t* binder_data, size_t binder_data_size, protocol_address* local_addr, protocol_address* remote_addr);


    rx_protocol_stack_endpoint* c_construct_listener_endpoint_stub(void* whose
        , const struct protocol_address_def* local_address, const struct protocol_address_def* remote_address);
    rx_protocol_stack_endpoint* c_construct_initiator_endpoint_stub(void* whose);

    plugin_port_def_struct _g_port_def_
    {
        c_get_code_info
        ,c_init_port_stub
        ,c_start_port_stub
        ,c_stop_port_stub
        ,c_deinit_port_stub

        ,c_stack_assembled_stub
        ,c_stack_disassembled_stub

        ,c_rx_extract_bind_address_stub

        ,c_rx_destroy_endpoint_stub

        ,c_construct_listener_endpoint_stub
        ,c_construct_initiator_endpoint_stub
    };


    rx_result_struct c_init_port(rx_platform_api::rx_port* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_port(ctx_obj).move();
    }
    rx_result_struct c_start_port(rx_platform_api::rx_port* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_port(ctx_obj).move();
    }
    rx_result_struct c_stop_port(rx_platform_api::rx_port* self)
    {
        return self->stop_port().move();
    }
    rx_result_struct c_deinit_port(rx_platform_api::rx_port* self)
    {
        return self->deinitialize_port().move();
    }

    void c_stack_assembled(rx_platform_api::rx_port* self)
    {
        self->stack_assembled();
    }
    void c_stack_disassembled(rx_platform_api::rx_port* self)
    {
        return self->stack_disassembled();
    }

    void c_extract_bind_address(rx_platform_api::rx_port* self, const uint8_t* binder_data, size_t binder_data_size, protocol_address* local_addr, protocol_address* remote_addr)
    {
        self->extern_extract_bind_address(binder_data, binder_data_size, local_addr, remote_addr);
    }
    void c_destroy_endpoint(rx_platform_api::rx_port* self, struct rx_protocol_stack_endpoint* endpoint)
    {
        self->destroy_endpoint(endpoint);
    }

    rx_protocol_stack_endpoint* c_construct_listener_endpoint(rx_platform_api::rx_port* self
        , const struct protocol_address_def* local_address, const protocol_address_def* remote_address)
    {
        return self->construct_listener_endpoint(local_address, remote_address);
    }
    rx_protocol_stack_endpoint* c_construct_initiator_endpoint(rx_platform_api::rx_port* self)
    {
        return self->construct_initiator_endpoint(nullptr, nullptr);
    }
}


namespace rx_platform_api {

rx_result register_port_runtime(const rx_node_id& id, rx_port_constructor_t construct_func, rx_runtime_register_func_t reg_function, rx_runtime_unregister_func_t unreg_function)
{
    RX_ASSERT(api_reg_port_func != nullptr);
    plugin_port_register_data data;
    data.constructor = construct_func;
    data.register_func = reg_function;
    data.unregister_func = unreg_function;
    auto ret = api_reg_port_func(get_rx_plugin(), id.c_ptr(), data);
    return ret;
}

// Class rx_platform_api::rx_port 

rx_port::rx_port()
{
	impl_.def = &_g_port_def_;
	bind_as_shared(&impl_.anchor);
}


rx_port::~rx_port()
{
}



rx_result rx_port::initialize_port (rx_init_context& ctx)
{
	return true;
}

rx_result rx_port::start_port (rx_start_context& ctx)
{
	return true;
}

rx_result rx_port::stop_port ()
{
	return true;
}

rx_result rx_port::deinitialize_port ()
{
	return true;
}

rx_result_with<io::rx_io_buffer> rx_port::alloc_io_buffer ()
{
    io::rx_io_buffer buffer;
    rx_result result = impl_.host_def->alloc_buffer(impl_.host, &buffer);
    if (result)
    {
        return buffer;
    }
    else
    {
        return result.errors();
    }
}

void rx_port::release_io_buffer (io::rx_io_buffer buff)
{
    rx_packet_buffer temp;
    buff.detach(&temp);
    impl_.host_def->release_buffer(impl_.host, temp);
}

void rx_port::stack_assembled ()
{
}

void rx_port::stack_disassembled ()
{
}

void rx_port::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
}

void rx_port::extern_extract_bind_address (const uint8_t* binder_data, size_t binder_data_size, protocol_address* local_addr, protocol_address* remote_addr)
{
    memory::std_buffer temp_buff(binder_data, binder_data_size);
    serialization::std_buffer_reader reader(temp_buff);

    data::runtime_values_data data;

    if (reader.read_init_values(nullptr, data))
    {
        io::any_address local;
        io::any_address remote;
        extract_bind_address(data, local, remote);

        rx_copy_address(local_addr, &local);
        rx_copy_address(remote_addr, &remote);
    }
}

rx_protocol_stack_endpoint* rx_port::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    RX_ASSERT(false);
    return nullptr;
}

rx_protocol_stack_endpoint* rx_port::construct_initiator_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    RX_ASSERT(false);
    return nullptr;
}

rx_result rx_port::listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    return impl_.host_def->listen(impl_.host, local_address, remote_address);
}

rx_result rx_port::connect (const protocol_address* local_address, const protocol_address* remote_address)
{
    return impl_.host_def->connect(impl_.host, local_address, remote_address);
}

rx_result rx_port::unbind_stack_endpoint (rx_protocol_stack_endpoint* what)
{
    return impl_.host_def->unbind_stack_endpoint(impl_.host, what);
}

rx_result rx_port::disconnect_stack_endpoint (rx_protocol_stack_endpoint* what)
{
    return impl_.host_def->disconnect_stack_endpoint(impl_.host, what);
}

rx_result rx_port::add_stack_endpoint (rx_protocol_stack_endpoint* what, const protocol_address* local_addr, const protocol_address* remote_addr)
{
    return impl_.host_def->bind_listener_endpoint(impl_.host, what, local_addr, remote_addr);
}


} // namespace rx_platform_api

