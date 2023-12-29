

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_basic_types.cpp
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


// rx_basic_types
#include "rx_basic_types.h"


#include "platform_api/rx_abi.h"
#include "lib/rx_values.h"
#include "lib/rx_ptr.h"


typedef rx_platform_api::rx_struct _rx_struct_holder_stub;
typedef rx_platform_api::rx_variable _rx_variable_holder_stub;
typedef rx_platform_api::rx_event _rx_event_holder_stub;

extern "C"
{
    void c_get_code_info(void* whose, const char* name, string_value_struct* info);



    rx_result_struct c_init_struct_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_struct_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_struct_stub(void* self);
    rx_result_struct c_deinit_struct_stub(void* self);
    rx_result_struct c_struct_input_stub(void* self, struct full_value_type* val);
    rx_result_struct c_struct_output_stub(void* self, struct typed_value_type* val);


    plugin_struct_def_struct _g_struct_def_
    {
        c_get_code_info
        ,c_init_struct_stub
        ,c_start_struct_stub
        ,c_stop_struct_stub
        ,c_deinit_struct_stub

    };


    rx_result_struct c_init_struct(rx_platform_api::rx_struct* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_struct(ctx_obj).move();
    }
    rx_result_struct c_start_struct(rx_platform_api::rx_struct* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_struct(ctx_obj).move();
    }
    rx_result_struct c_stop_struct(rx_platform_api::rx_struct* self)
    {
        return self->stop_struct().move();
    }
    rx_result_struct c_deinit_struct(rx_platform_api::rx_struct* self)
    {
        return self->deinitialize_struct().move();
    }



    rx_result_struct c_init_variable_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_variable_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_variable_stub(void* self);
    rx_result_struct c_deinit_variable_stub(void* self);


    plugin_variable_def_struct _g_variable_def_
    {
        c_get_code_info
        ,c_init_variable_stub
        ,c_start_variable_stub
        ,c_stop_variable_stub
        ,c_deinit_variable_stub
    };


    rx_result_struct c_init_variable(rx_platform_api::rx_variable* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_variable(ctx_obj).move();
    }
    rx_result_struct c_start_variable(rx_platform_api::rx_variable* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_variable(ctx_obj).move();
    }
    rx_result_struct c_stop_variable(rx_platform_api::rx_variable* self)
    {
        return self->stop_variable().move();
    }
    rx_result_struct c_deinit_variable(rx_platform_api::rx_variable* self)
    {
        return self->deinitialize_variable().move();
    }



    rx_result_struct c_init_event_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_event_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_event_stub(void* self);
    rx_result_struct c_deinit_event_stub(void* self);


    plugin_event_def_struct _g_event_def_
    {
        c_get_code_info
        ,c_init_event_stub
        ,c_start_event_stub
        ,c_stop_event_stub
        ,c_deinit_event_stub
    };


    rx_result_struct c_init_event(rx_platform_api::rx_event* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_event(ctx_obj).move();
    }
    rx_result_struct c_start_event(rx_platform_api::rx_event* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_event(ctx_obj).move();
    }
    rx_result_struct c_stop_event(rx_platform_api::rx_event* self)
    {
        return self->stop_event().move();
    }
    rx_result_struct c_deinit_event(rx_platform_api::rx_event* self)
    {
        return self->deinitialize_event().move();
    }
}


namespace rx_platform_api {

// Class rx_platform_api::rx_struct 

rx_struct::rx_struct()
{
    impl_.def = &_g_struct_def_;
    bind_as_shared(&impl_.anchor);
}


rx_struct::~rx_struct()
{
}



rx_result rx_struct::initialize_struct (rx_init_context& ctx)
{
    return true;
}

rx_result rx_struct::start_struct (rx_start_context& ctx)
{
    return true;
}

rx_result rx_struct::stop_struct ()
{
    return true;
}

rx_result rx_struct::deinitialize_struct ()
{
    return true;
}


// Class rx_platform_api::rx_variable 

rx_variable::rx_variable()
{
    impl_.def = &_g_variable_def_;
    bind_as_shared(&impl_.anchor);
}


rx_variable::~rx_variable()
{
}



rx_result rx_variable::initialize_variable (rx_init_context& ctx)
{
    return true;
}

rx_result rx_variable::start_variable (rx_start_context& ctx)
{
    return true;
}

rx_result rx_variable::stop_variable ()
{
    return true;
}

rx_result rx_variable::deinitialize_variable ()
{
    return true;
}


// Class rx_platform_api::rx_event 

rx_event::rx_event()
{
}


rx_event::~rx_event()
{
}



rx_result rx_event::initialize_event (rx_init_context& ctx)
{
    return true;
}

rx_result rx_event::start_event (rx_start_context& ctx)
{
    return true;
}

rx_result rx_event::stop_event ()
{
    return true;
}

rx_result rx_event::deinitialize_event ()
{
    return true;
}

void rx_event::event_fired (rx_simple_value data)
{
}

data::runtime_data_model rx_event::get_arguemnts ()
{
    return data::runtime_data_model();
}


} // namespace rx_platform_api

