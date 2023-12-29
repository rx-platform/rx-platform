

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_var_types.cpp
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


// rx_var_types
#include "rx_var_types.h"

#include "platform_api/rx_abi.h"
#include "lib/rx_values.h"
#include "lib/rx_ptr.h"


typedef rx_platform_api::rx_source _rx_source_holder_stub;
typedef rx_platform_api::rx_mapper _rx_mapper_holder_stub;
typedef rx_platform_api::rx_filter _rx_filter_holder_stub;

extern "C"
{
    void c_get_code_info(void* whose, const char* name, string_value_struct* info);

    //!!! IMPORTANT rx_value_t is fast_uint8, so we have to convert
    rx_result_struct c_init_source_stub(void* self, init_ctx_ptr ctx, uint8_t value_type);
    rx_result_struct c_start_source_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_source_stub(void* self);
    rx_result_struct c_deinit_source_stub(void* self);
    rx_result_struct c_write_source_stub(void* self
        , runtime_transaction_id_t id, int test, rx_security_handle_t identity
        , typed_value_type val, runtime_ctx_ptr ctx);


    plugin_source_def_struct _g_source_def_
    {
        c_get_code_info
        ,c_init_source_stub
        ,c_start_source_stub
        ,c_stop_source_stub
        ,c_deinit_source_stub

        ,c_write_source_stub
    };


    rx_result_struct c_init_source(rx_platform_api::rx_source* self, init_ctx_ptr ctx, uint8_t value_type)
    {
        self->value_type_ = value_type;
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_source(ctx_obj).move();
    }
    rx_result_struct c_start_source(rx_platform_api::rx_source* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_source(ctx_obj).move();
    }
    rx_result_struct c_stop_source(rx_platform_api::rx_source* self)
    {
        return self->stop_source().move();
    }
    rx_result_struct c_deinit_source(rx_platform_api::rx_source* self)
    {
        return self->deinitialize_source().move();
    }
    rx_result_struct c_write_source(rx_platform_api::rx_source* self
        , runtime_transaction_id_t id, int test, rx_security_handle_t identity
        , typed_value_type val, runtime_ctx_ptr ctx)
    {
        rx_platform_api::rx_process_context pctx;
        pctx.bind(ctx);
        return self->source_write(id, test != 0, identity, rx_simple_value(val), pctx).move();
    }


    //!!! IMPORTANT rx_value_t is fast_uint8, so we have to convert
    rx_result_struct c_init_mapper_stub(void* self, init_ctx_ptr ctx, uint8_t value_type);
    rx_result_struct c_start_mapper_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_mapper_stub(void* self);
    rx_result_struct c_deinit_mapper_stub(void* self);
    void c_mapped_value_changed_stub(void* self, struct full_value_type val, runtime_ctx_ptr ctx);
    void c_mapper_result_received_stub(void* self, rx_result_struct result, runtime_transaction_id_t id, runtime_ctx_ptr ctx);
    void c_mapper_execute_result_received_stub(void* self, rx_result_struct result, runtime_transaction_id_t id, struct typed_value_type out_val, runtime_ctx_ptr ctx);


    plugin_mapper_def_struct3 _g_mapper_def3_
    {
        c_get_code_info
        ,c_init_mapper_stub
        ,c_start_mapper_stub
        ,c_stop_mapper_stub
        ,c_deinit_mapper_stub

        ,c_mapped_value_changed_stub
        ,c_mapper_result_received_stub
        ,c_mapper_execute_result_received_stub
    };


    rx_result_struct c_init_mapper(rx_platform_api::rx_mapper* self, init_ctx_ptr ctx, uint8_t value_type)
    {
        self->value_type_ = value_type;
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_mapper(ctx_obj).move();
    }
    rx_result_struct c_start_mapper(rx_platform_api::rx_mapper* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_mapper(ctx_obj).move();
    }
    rx_result_struct c_stop_mapper(rx_platform_api::rx_mapper* self)
    {
        return self->stop_mapper().move();
    }
    rx_result_struct c_deinit_mapper(rx_platform_api::rx_mapper* self)
    {
        return self->deinitialize_mapper().move();
    }
    void c_mapped_value_changed(rx_platform_api::rx_mapper* self, struct full_value_type val, runtime_ctx_ptr ctx)
    {
        rx_platform_api::rx_process_context pctx;
        pctx.bind(ctx);
        self->mapped_value_changed(val, pctx);
    }
    void c_mapper_result_received(rx_platform_api::rx_mapper* self, rx_result_struct result, runtime_transaction_id_t id, runtime_ctx_ptr ctx)
    {
        rx_platform_api::rx_process_context pctx;
        pctx.bind(ctx);
        self->mapper_result_received(result, id, pctx);
    }
    void c_mapper_execute_result_received(rx_platform_api::rx_mapper* self, rx_result_struct result, runtime_transaction_id_t id, struct typed_value_type out_val, runtime_ctx_ptr ctx)
    {
        rx_platform_api::rx_process_context pctx;
        pctx.bind(ctx);
        self->mapper_execute_result_received(result, out_val, id, pctx);
    }



    rx_result_struct c_init_filter_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_filter_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_filter_stub(void* self);
    rx_result_struct c_deinit_filter_stub(void* self);
    rx_result_struct c_filter_input_stub(void* self, struct full_value_type* val);
    rx_result_struct c_filter_output_stub(void* self, struct typed_value_type* val);


    plugin_filter_def_struct _g_filter_def_
    {
        c_get_code_info
        ,c_init_filter_stub
        ,c_start_filter_stub
        ,c_stop_filter_stub
        ,c_deinit_filter_stub

        ,c_filter_input_stub
        ,c_filter_output_stub
    };


    rx_result_struct c_init_filter(rx_platform_api::rx_filter* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_filter(ctx_obj).move();
    }
    rx_result_struct c_start_filter(rx_platform_api::rx_filter* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_filter(ctx_obj).move();
    }
    rx_result_struct c_stop_filter(rx_platform_api::rx_filter* self)
    {
        return self->stop_filter().move();
    }
    rx_result_struct c_deinit_filter(rx_platform_api::rx_filter* self)
    {
        return self->deinitialize_filter().move();
    }
    rx_result_struct c_filter_input(rx_platform_api::rx_filter* self, struct full_value_type* val)
    {
        rx_value temp_val(val);
        rx_result result = self->filter_input(temp_val);
        if (result)
            *val = temp_val.move();
        return result.move();
    }
    rx_result_struct c_filter_output(rx_platform_api::rx_filter* self, struct typed_value_type* val)
    {
        rx_simple_value temp_val(val);
        rx_result result = self->filter_output(temp_val);
        if (result)
            *val = temp_val.move();
        return result.move();
    }
}


namespace rx_platform_api {

// Class rx_platform_api::rx_filter 

rx_filter::rx_filter (bool input, bool output)
{
    impl_.io_data = 0;
    if (input)
        impl_.io_data |= RX_IO_DATA_INPUT_MASK;
    if (output)
        impl_.io_data |= RX_IO_DATA_OUTPUT_MASK;
    impl_.def = &_g_filter_def_;
    bind_as_shared(&impl_.anchor);
}


rx_filter::~rx_filter()
{
}



rx_result rx_filter::initialize_filter (rx_init_context& ctx)
{
    return true;
}

rx_result rx_filter::start_filter (rx_start_context& ctx)
{
    return true;
}

rx_result rx_filter::stop_filter ()
{
    return true;
}

rx_result rx_filter::deinitialize_filter ()
{
    return true;
}

rx_result rx_filter::filter_input (rx_value& val)
{
    return RX_NOT_SUPPORTED;
}

rx_result rx_filter::filter_output (rx_simple_value& val)
{
    return RX_NOT_SUPPORTED;
}

rx_result rx_filter::filter_changed ()
{
    return impl_.host_def->filter_changed(impl_.host);
}


// Class rx_platform_api::rx_mapper 

rx_mapper::rx_mapper (bool read, bool write)
      : value_type_(RX_NULL_TYPE)
{
    impl_.io_data = 0;
    if (write)
        impl_.io_data |= RX_IO_DATA_INPUT_MASK;
    if (read)
        impl_.io_data |= RX_IO_DATA_OUTPUT_MASK;
    impl_.def = &_g_mapper_def3_;
    bind_as_shared(&impl_.anchor);
}


rx_mapper::~rx_mapper()
{
}



rx_result rx_mapper::initialize_mapper (rx_init_context& ctx)
{
    return true;
}

rx_result rx_mapper::start_mapper (rx_start_context& ctx)
{
    return true;
}

rx_result rx_mapper::stop_mapper ()
{
    return true;
}

rx_result rx_mapper::deinitialize_mapper ()
{
    return true;
}

void rx_mapper::mapped_value_changed (rx_value&& val, rx_process_context& ctx)
{
}

void rx_mapper::mapper_result_received (rx_result&& result, runtime_transaction_id_t id, rx_process_context& ctx)
{
}

void rx_mapper::mapper_write_pending (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val)
{
    impl_.host_def->mapper_write_pending(impl_.host, id, test ? 1 : 0, identity, val.move());
}

void rx_mapper::map_current_value () const
{
    impl_.host_def->map_current_value(impl_.host);
}

void rx_mapper::mapper_execute_pending (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val)
{
}

void rx_mapper::mapper_execute_result_received (rx_result&& result, values::rx_simple_value out_data, runtime_transaction_id_t id, rx_process_context& ctx)
{
}

data::runtime_data_model rx_mapper::get_method_inputs ()
{
    return data::runtime_data_model();
}

data::runtime_data_model rx_mapper::get_method_outputs ()
{
    return data::runtime_data_model();
}


rx_value_t rx_mapper::get_value_type () const
{
  return value_type_;
}


// Class rx_platform_api::rx_source 

rx_source::rx_source (bool input, bool output)
      : value_type_(RX_NULL_TYPE)
{
    impl_.io_data = 0;
    if (input)
        impl_.io_data |= RX_IO_DATA_INPUT_MASK;
    if (output)
        impl_.io_data |= RX_IO_DATA_OUTPUT_MASK;
    impl_.def = &_g_source_def_;
    bind_as_shared(&impl_.anchor);
}


rx_source::~rx_source()
{
}



rx_result rx_source::initialize_source (rx_init_context& ctx)
{
    return true;
}

rx_result rx_source::start_source (rx_start_context& ctx)
{
    return true;
}

rx_result rx_source::stop_source ()
{
    return true;
}

rx_result rx_source::deinitialize_source ()
{
    return true;
}

rx_result rx_source::source_write (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val, rx_process_context& ctx)
{
    return RX_NOT_SUPPORTED;
}

rx_result rx_source::source_value_changed (rx_value&& val)
{
    return impl_.host_def->update_source(impl_.host, val.move());
}

void rx_source::source_result_received (rx_result&& result, runtime_transaction_id_t id)
{
    impl_.host_def->result_update_source(impl_.host, result.move(), id);
}


rx_value_t rx_source::get_value_type () const
{
  return value_type_;
}


} // namespace rx_platform_api

