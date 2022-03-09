

/****************************************************************************
*
*  platform_api\api_runtime_c.c
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



#include "common/rx_common.h"
#include "rx_abi.h"



struct _rx_source_holder_stub;
struct _rx_mapper_holder_stub;
struct _rx_filter_holder_stub;

struct _rx_object_holder_stub;
struct _rx_application_holder_stub;
struct _rx_domain_holder_stub;

struct _rx_port_holder_stub;


rx_result_struct c_init_source(struct _rx_source_holder_stub* self, init_ctx_ptr ctx, uint8_t value_type);
rx_result_struct c_start_source(struct _rx_source_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_source(struct _rx_source_holder_stub* self);
rx_result_struct c_deinit_source(struct _rx_source_holder_stub* self);
rx_result_struct c_write_source(struct _rx_source_holder_stub* self
    , runtime_transaction_id_t id, int test, rx_security_handle_t identity
    , struct typed_value_type val, runtime_ctx_ptr ctx);

rx_result_struct c_init_source_stub(void* self, init_ctx_ptr ctx, uint8_t value_type)
{
    return c_init_source(self, ctx, value_type);
}

rx_result_struct c_start_source_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_source(self, ctx);
}
rx_result_struct c_stop_source_stub(void* self)
{
    return c_stop_source(self);
}
rx_result_struct c_deinit_source_stub(void* self)
{
    return c_deinit_source(self);
}
rx_result_struct c_write_source_stub(void* self
    , runtime_transaction_id_t id, int test, rx_security_handle_t identity
    , struct typed_value_type val, runtime_ctx_ptr ctx)
{
    return c_write_source(self, id, test, identity, val, ctx);
}


rx_result_struct c_init_filter(struct _rx_filter_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_filter(struct _rx_filter_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_filter(struct _rx_filter_holder_stub* self);
rx_result_struct c_deinit_filter(struct _rx_filter_holder_stub* self);
rx_result_struct c_filter_input(struct _rx_filter_holder_stub* self, struct full_value_type* val);
rx_result_struct c_filter_output(struct _rx_filter_holder_stub* self, struct typed_value_type* val);

rx_result_struct c_init_filter_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_filter(self, ctx);
}

rx_result_struct c_start_filter_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_filter(self, ctx);
}
rx_result_struct c_stop_filter_stub(void* self)
{
    return c_stop_filter(self);
}
rx_result_struct c_deinit_filter_stub(void* self)
{
    return c_deinit_filter(self);
}
rx_result_struct c_filter_input_stub(void* self, struct full_value_type* val)
{
    return c_filter_input(self, val);
}
rx_result_struct c_filter_output_stub(void* self, struct typed_value_type* val)
{
    return c_filter_output(self, val);
}

rx_result_struct c_init_mapper(struct _rx_mapper_holder_stub* self, init_ctx_ptr ctx, uint8_t value_type);
rx_result_struct c_start_mapper(struct _rx_mapper_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_mapper(struct _rx_mapper_holder_stub* self);
rx_result_struct c_deinit_mapper(struct _rx_mapper_holder_stub* self);
void c_mapped_value_changed(struct _rx_mapper_holder_stub* self, struct full_value_type val, runtime_ctx_ptr ctx);
void c_mapper_result_received(struct _rx_mapper_holder_stub* self, rx_result_struct result, runtime_transaction_id_t id, runtime_ctx_ptr ctx);

rx_result_struct c_init_mapper_stub(void* self, init_ctx_ptr ctx, uint8_t value_type)
{
    return c_init_mapper(self, ctx, value_type);
}

rx_result_struct c_start_mapper_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_mapper(self, ctx);
}
rx_result_struct c_stop_mapper_stub(void* self)
{
    return c_stop_mapper(self);
}
rx_result_struct c_deinit_mapper_stub(void* self)
{
    return c_deinit_mapper(self);
}
void c_mapped_value_changed_stub(void* self, struct full_value_type val, runtime_ctx_ptr ctx)
{
    c_mapped_value_changed(self, val, ctx);
}
void c_mapper_result_received_stub(void* self, rx_result_struct result, runtime_transaction_id_t id, runtime_ctx_ptr ctx)
{
    c_mapper_result_received(self, result, id, ctx);
}


rx_result_struct c_init_port(struct _rx_port_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_port(struct _rx_port_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_port(struct _rx_port_holder_stub* self);
rx_result_struct c_deinit_port(struct _rx_port_holder_stub* self);

void c_stack_assembled(struct _rx_port_holder_stub* self);
void c_stack_disassembled(struct _rx_port_holder_stub* self);

void c_extract_bind_address(struct _rx_port_holder_stub* self, const uint8_t* binder_data, size_t binder_data_size, protocol_address* local_addr, protocol_address* remote_addr);

void c_destroy_endpoint(struct _rx_port_holder_stub* self, struct rx_protocol_stack_endpoint* endpoint);

struct rx_protocol_stack_endpoint* c_construct_listener_endpoint(struct _rx_port_holder_stub* self
    , const protocol_address* local_address, const protocol_address* remote_address);
struct rx_protocol_stack_endpoint* c_construct_initiator_endpoint(struct _rx_port_holder_stub* self);

rx_result_struct c_init_port_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_port(self, ctx);
}

rx_result_struct c_start_port_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_port(self, ctx);
}
rx_result_struct c_stop_port_stub(void* self)
{
    return c_stop_port(self);
}
rx_result_struct c_deinit_port_stub(void* self)
{
    return c_deinit_port(self);
}

void c_stack_assembled_stub(void* whose)
{
    c_stack_assembled(whose);
}
void c_stack_disassembled_stub(void* whose)
{
    c_stack_disassembled(whose);
}
void c_rx_extract_bind_address_stub(void* whose, const uint8_t* binder_data, size_t binder_data_size, protocol_address* local_addr, protocol_address* remote_addr)
{
    c_extract_bind_address(whose, binder_data, binder_data_size, local_addr, remote_addr);
}
void c_rx_destroy_endpoint_stub(void* whose, struct rx_protocol_stack_endpoint* endpoint)
{
    c_destroy_endpoint(whose, endpoint);
}

struct rx_protocol_stack_endpoint* c_construct_listener_endpoint_stub(void* whose
    , const protocol_address* local_address, const protocol_address* remote_address)
{
    return c_construct_listener_endpoint(whose, local_address, remote_address);
}
struct rx_protocol_stack_endpoint* c_construct_initiator_endpoint_stub(void* whose)
{
    return c_construct_initiator_endpoint(whose);
}


rx_result_struct c_init_object(struct _rx_object_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_object(struct _rx_object_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_object(struct _rx_object_holder_stub* self);
rx_result_struct c_deinit_object(struct _rx_object_holder_stub* self);

rx_result_struct c_init_object_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_object(self, ctx);
}

rx_result_struct c_start_object_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_object(self, ctx);
}
rx_result_struct c_stop_object_stub(void* self)
{
    return c_stop_object(self);
}
rx_result_struct c_deinit_object_stub(void* self)
{
    return c_deinit_object(self);
}



rx_result_struct c_init_domain(struct _rx_domain_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_domain(struct _rx_domain_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_domain(struct _rx_domain_holder_stub* self);
rx_result_struct c_deinit_domain(struct _rx_domain_holder_stub* self);

rx_result_struct c_init_domain_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_domain(self, ctx);
}

rx_result_struct c_start_domain_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_domain(self, ctx);
}
rx_result_struct c_stop_domain_stub(void* self)
{
    return c_stop_domain(self);
}
rx_result_struct c_deinit_domain_stub(void* self)
{
    return c_deinit_domain(self);
}


rx_result_struct c_init_application(struct _rx_application_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_application(struct _rx_application_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_application(struct _rx_application_holder_stub* self);
rx_result_struct c_deinit_application(struct _rx_application_holder_stub* self);

rx_result_struct c_init_application_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_application(self, ctx);
}

rx_result_struct c_start_application_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_application(self, ctx);
}
rx_result_struct c_stop_application_stub(void* self)
{
    return c_stop_application(self);
}
rx_result_struct c_deinit_application_stub(void* self)
{
    return c_deinit_application(self);
}


