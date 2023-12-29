

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_logic_types.cpp
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


// rx_logic_types
#include "rx_logic_types.h"


#include "platform_api/rx_abi.h"
#include "lib/rx_values.h"
#include "lib/rx_ptr.h"


typedef rx_platform_api::rx_method _rx_method_holder_stub;
typedef rx_platform_api::rx_program _rx_program_holder_stub;

extern "C"
{
    void c_get_code_info(void* whose, const char* name, string_value_struct* info);




    rx_result_struct c_init_method_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_method_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_method_stub(void* self);
    rx_result_struct c_deinit_method_stub(void* self);
    rx_result_struct c_execute_method_stub(void* self
        , runtime_transaction_id_t id, int test, rx_security_handle_t identity
        , struct typed_value_type val, runtime_ctx_ptr ctx);


    plugin_method_def_struct _g_method_def_
    {
        c_get_code_info
        ,c_init_method_stub
        ,c_start_method_stub
        ,c_stop_method_stub
        ,c_deinit_method_stub
        , c_execute_method_stub
    };


    rx_result_struct c_init_method(rx_platform_api::rx_method* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_method(ctx_obj).move();
    }
    rx_result_struct c_start_method(rx_platform_api::rx_method* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_method(ctx_obj).move();
    }
    rx_result_struct c_stop_method(rx_platform_api::rx_method* self)
    {
        return self->stop_method().move();
    }
    rx_result_struct c_deinit_method(rx_platform_api::rx_method* self)
    {
        return self->deinitialize_method().move();
    }

    rx_result_struct c_execute_method(rx_platform_api::rx_method* self
        , runtime_transaction_id_t id, int test, rx_security_handle_t identity
        , struct typed_value_type val, runtime_ctx_ptr ctx)
    {
        rx_platform_api::rx_process_context pctx;
        pctx.bind(ctx);
        return self->method_execute(id, test, identity, std::move(val), pctx).move();
    }


    rx_result_struct c_init_program_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_program_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_program_stub(void* self);
    rx_result_struct c_deinit_program_stub(void* self);


    plugin_program_def_struct _g_program_def_
    {
        c_get_code_info
        ,c_init_program_stub
        ,c_start_program_stub
        ,c_stop_program_stub
        ,c_deinit_program_stub
    };


    rx_result_struct c_init_program(rx_platform_api::rx_program* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_program(ctx_obj).move();
    }
    rx_result_struct c_start_program(rx_platform_api::rx_program* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_program(ctx_obj).move();
    }
    rx_result_struct c_stop_program(rx_platform_api::rx_program* self)
    {
        return self->stop_program().move();
    }
    rx_result_struct c_deinit_program(rx_platform_api::rx_program* self)
    {
        return self->deinitialize_program().move();
    }


    rx_result_struct c_init_display_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_display_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_display_stub(void* self);
    rx_result_struct c_deinit_display_stub(void* self);


    plugin_display_def_struct _g_display_def_
    {
        c_get_code_info
        ,c_init_display_stub
        ,c_start_display_stub
        ,c_stop_display_stub
        ,c_deinit_display_stub
    };


    rx_result_struct c_init_display(rx_platform_api::rx_display* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_display(ctx_obj).move();
    }
    rx_result_struct c_start_display(rx_platform_api::rx_display* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_display(ctx_obj).move();
    }
    rx_result_struct c_stop_display(rx_platform_api::rx_display* self)
    {
        return self->stop_display().move();
    }
    rx_result_struct c_deinit_display(rx_platform_api::rx_display* self)
    {
        return self->deinitialize_display().move();
    }
}


namespace rx_platform_api {

// Class rx_platform_api::rx_method 

rx_method::rx_method()
{
    impl_.def = &_g_method_def_;
    bind_as_shared(&impl_.anchor);
}


rx_method::~rx_method()
{
}



rx_result rx_method::initialize_method (rx_init_context& ctx)
{
	return true;
}

rx_result rx_method::start_method (rx_start_context& ctx)
{
	return true;
}

rx_result rx_method::stop_method ()
{
	return true;
}

rx_result rx_method::deinitialize_method ()
{
	return true;
}

void rx_method::execute_result_received (rx_result&& result, runtime_transaction_id_t id, rx_simple_value val)
{
    impl_.host_def->method_executed(impl_.host, result.move(), id, val.move());
}


// Class rx_platform_api::rx_program 

rx_program::rx_program()
{
    impl_.def = &_g_program_def_;
    bind_as_shared(&impl_.anchor);
}


rx_program::~rx_program()
{
}



rx_result rx_program::initialize_program (rx_init_context& ctx)
{
	return true;
}

rx_result rx_program::start_program (rx_start_context& ctx)
{
	return true;
}

rx_result rx_program::stop_program ()
{
	return true;
}

rx_result rx_program::deinitialize_program ()
{
	return true;
}


// Class rx_platform_api::rx_display 

rx_display::rx_display()
{
    impl_.def = &_g_display_def_;
    bind_as_shared(&impl_.anchor);
}


rx_display::~rx_display()
{
}



rx_result rx_display::initialize_display (rx_init_context& ctx)
{
    return true;
}

rx_result rx_display::start_display (rx_start_context& ctx)
{
    return true;
}

rx_result rx_display::stop_display ()
{
    return true;
}

rx_result rx_display::deinitialize_display ()
{
    return true;
}


} // namespace rx_platform_api

