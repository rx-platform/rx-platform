

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\api_runtime_c.c
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



#include "common/rx_common.h"
#include "rx_abi.h"



struct _rx_source_holder_stub;
struct _rx_mapper_holder_stub;
struct _rx_filter_holder_stub;

struct _rx_struct_holder_stub;
struct _rx_variable_holder_stub;
struct _rx_event_holder_stub;

struct _rx_display_holder_stub;
struct _rx_method_holder_stub;
struct _rx_program_holder_stub;

struct _rx_object_holder_stub;
struct _rx_application_holder_stub;
struct _rx_domain_holder_stub;

struct _rx_port_holder_stub;

struct _rx_relation_holder_stub;

struct _rx_storage_holder_stub;

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
void c_mapper_execute_result_received(struct _rx_mapper_holder_stub* self, rx_result_struct result, runtime_transaction_id_t id, struct typed_value_type out_val, runtime_ctx_ptr ctx);

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
void c_mapper_execute_result_received_stub(void* self, rx_result_struct result, runtime_transaction_id_t id, struct typed_value_type out_val, runtime_ctx_ptr ctx)
{
    c_mapper_execute_result_received(self, result, id, out_val, ctx);
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




rx_result_struct c_init_struct(struct _rx_struct_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_struct(struct _rx_struct_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_struct(struct _rx_struct_holder_stub* self);
rx_result_struct c_deinit_struct(struct _rx_struct_holder_stub* self);

rx_result_struct c_init_struct_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_struct(self, ctx);
}

rx_result_struct c_start_struct_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_struct(self, ctx);
}
rx_result_struct c_stop_struct_stub(void* self)
{
    return c_stop_struct(self);
}
rx_result_struct c_deinit_struct_stub(void* self)
{
    return c_deinit_struct(self);
}



rx_result_struct c_init_variable(struct _rx_variable_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_variable(struct _rx_variable_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_variable(struct _rx_variable_holder_stub* self);
rx_result_struct c_deinit_variable(struct _rx_variable_holder_stub* self);

rx_result_struct c_init_variable_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_variable(self, ctx);
}

rx_result_struct c_start_variable_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_variable(self, ctx);
}
rx_result_struct c_stop_variable_stub(void* self)
{
    return c_stop_variable(self);
}
rx_result_struct c_deinit_variable_stub(void* self)
{
    return c_deinit_variable(self);
}


rx_result_struct c_init_event(struct _rx_event_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_event(struct _rx_event_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_event(struct _rx_event_holder_stub* self);
rx_result_struct c_deinit_event(struct _rx_event_holder_stub* self);

rx_result_struct c_init_event_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_event(self, ctx);
}

rx_result_struct c_start_event_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_event(self, ctx);
}
rx_result_struct c_stop_event_stub(void* self)
{
    return c_stop_event(self);
}
rx_result_struct c_deinit_event_stub(void* self)
{
    return c_deinit_event(self);
}



rx_result_struct c_init_method(struct _rx_method_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_method(struct _rx_method_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_method(struct _rx_method_holder_stub* self);
rx_result_struct c_deinit_method(struct _rx_method_holder_stub* self);
rx_result_struct c_execute_method(struct _rx_method_holder_stub* self
    , runtime_transaction_id_t id, int test, rx_security_handle_t identity
    , struct typed_value_type val, runtime_ctx_ptr ctx);

rx_result_struct c_init_method_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_method(self, ctx);
}

rx_result_struct c_start_method_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_method(self, ctx);
}
rx_result_struct c_stop_method_stub(void* self)
{
    return c_stop_method(self);
}
rx_result_struct c_deinit_method_stub(void* self)
{
    return c_deinit_method(self);
}

rx_result_struct c_execute_method_stub(void* self
    , runtime_transaction_id_t id, int test, rx_security_handle_t identity
    , struct typed_value_type val, runtime_ctx_ptr ctx)
{
    return c_execute_method(self, id, test, identity, val, ctx);
}

rx_result_struct c_init_program(struct _rx_program_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_program(struct _rx_program_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_program(struct _rx_program_holder_stub* self);
rx_result_struct c_deinit_program(struct _rx_program_holder_stub* self);

rx_result_struct c_init_program_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_program(self, ctx);
}

rx_result_struct c_start_program_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_program(self, ctx);
}
rx_result_struct c_stop_program_stub(void* self)
{
    return c_stop_program(self);
}
rx_result_struct c_deinit_program_stub(void* self)
{
    return c_deinit_program(self);
}



rx_result_struct c_init_display(struct _rx_display_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_display(struct _rx_display_holder_stub* self, start_ctx_ptr ctx);
rx_result_struct c_stop_display(struct _rx_display_holder_stub* self);
rx_result_struct c_deinit_display(struct _rx_display_holder_stub* self);

rx_result_struct c_init_display_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_display(self, ctx);
}

rx_result_struct c_start_display_stub(void* self, start_ctx_ptr ctx)
{
    return c_start_display(self, ctx);
}
rx_result_struct c_stop_display_stub(void* self)
{
    return c_stop_display(self);
}
rx_result_struct c_deinit_display_stub(void* self)
{
    return c_deinit_display(self);
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




rx_result_struct c_init_relation(struct _rx_relation_holder_stub* self, init_ctx_ptr ctx);
rx_result_struct c_start_relation(struct _rx_relation_holder_stub* self, start_ctx_ptr ctx, int is_target);
rx_result_struct c_stop_relation(struct _rx_relation_holder_stub* self, int is_target);
rx_result_struct c_deinit_relation(struct _rx_relation_holder_stub* self);

rx_result_struct c_make_target_relation(struct _rx_relation_holder_stub* self, struct plugin_relation_runtime_struct_t** target);
rx_result_struct c_relation_connected(struct _rx_relation_holder_stub* self, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);
rx_result_struct c_relation_disconnected(struct _rx_relation_holder_stub* self, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);


rx_result_struct c_init_relation_stub(void* self, init_ctx_ptr ctx)
{
    return c_init_relation(self, ctx);
}

rx_result_struct c_start_relation_stub(void* self, start_ctx_ptr ctx, int is_target)
{
    return c_start_relation(self, ctx, is_target);
}
rx_result_struct c_stop_relation_stub(void* self, int is_target)
{
    return c_stop_relation(self, is_target);
}
rx_result_struct c_deinit_relation_stub(void* self)
{
    return c_deinit_relation(self);
}

rx_result_struct c_make_target_relation_stub(void* whose, struct plugin_relation_runtime_struct_t** target)
{
    return c_make_target_relation(whose, target);
}
rx_result_struct c_relation_connected_stub(void* whose, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to)
{
    return c_relation_connected(whose, from, to);
}
rx_result_struct c_relation_disconnected_stub(void* whose, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to)
{
    return c_relation_disconnected(whose, from, to);
}


rx_result_struct c_init_storage(struct _rx_storage_holder_stub* self, const char* reference);
rx_result_struct c_deinit_storage(struct _rx_storage_holder_stub* self);


rx_result_struct c_init_storage_stub(void* self, const char* reference)
{
    return c_init_storage(self, reference);
}

rx_result_struct c_deinit_storage_stub(void* self)
{
    return c_deinit_storage(self);
}




