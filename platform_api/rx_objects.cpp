

/****************************************************************************
*
*  platform_api\rx_objects.cpp
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


// rx_objects
#include "platform_api/rx_objects.h"


typedef rx_platform_api::rx_application _rx_application_holder_stub;
rxRegisterApplicationRuntime_t api_reg_application_func;
typedef rx_platform_api::rx_domain _rx_domain_holder_stub;
rxRegisterDomainRuntime_t api_reg_domain_func;
typedef rx_platform_api::rx_object _rx_object_holder_stub;
rxRegisterObjectRuntime_t api_reg_object_func;


extern "C"
{
    void c_get_code_info(void* whose, const char* name, string_value_struct* info);

    rx_result_struct c_init_application_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_application_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_application_stub(void* self);
    rx_result_struct c_deinit_application_stub(void* self);


    plugin_application_def_struct _g_application_def_
    {
        c_get_code_info
        ,c_init_application_stub
        ,c_start_application_stub
        ,c_stop_application_stub
        ,c_deinit_application_stub
    };


    rx_result_struct c_init_application(rx_platform_api::rx_application* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_application(ctx_obj).move();
    }
    rx_result_struct c_start_application(rx_platform_api::rx_application* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_application(ctx_obj).move();
    }
    rx_result_struct c_stop_application(rx_platform_api::rx_application* self)
    {
        return self->stop_application().move();
    }
    rx_result_struct c_deinit_application(rx_platform_api::rx_application* self)
    {
        return self->deinitialize_application().move();
    }


    rx_result_struct c_init_domain_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_domain_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_domain_stub(void* self);
    rx_result_struct c_deinit_domain_stub(void* self);


    plugin_domain_def_struct _g_domain_def_
    {
        c_get_code_info
        ,c_init_domain_stub
        ,c_start_domain_stub
        ,c_stop_domain_stub
        ,c_deinit_domain_stub
    };


    rx_result_struct c_init_domain(rx_platform_api::rx_domain* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_domain(ctx_obj).move();
    }
    rx_result_struct c_start_domain(rx_platform_api::rx_domain* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_domain(ctx_obj).move();
    }
    rx_result_struct c_stop_domain(rx_platform_api::rx_domain* self)
    {
        return self->stop_domain().move();
    }
    rx_result_struct c_deinit_domain(rx_platform_api::rx_domain* self)
    {
        return self->deinitialize_domain().move();
    }


    rx_result_struct c_init_object_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_object_stub(void* self, start_ctx_ptr ctx);
    rx_result_struct c_stop_object_stub(void* self);
    rx_result_struct c_deinit_object_stub(void* self);


    plugin_object_def_struct _g_object_def_
    {
        c_get_code_info
        ,c_init_object_stub
        ,c_start_object_stub
        ,c_stop_object_stub
        ,c_deinit_object_stub
    };


    rx_result_struct c_init_object(rx_platform_api::rx_object* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_object(ctx_obj).move();
    }
    rx_result_struct c_start_object(rx_platform_api::rx_object* self, start_ctx_ptr ctx)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_object(ctx_obj).move();
    }
    rx_result_struct c_stop_object(rx_platform_api::rx_object* self)
    {
        return self->stop_object().move();
    }
    rx_result_struct c_deinit_object(rx_platform_api::rx_object* self)
    {
        return self->deinitialize_object().move();
    }
}


namespace rx_platform_api {

rx_result register_object_runtime(const rx_node_id& id, rx_object_constructor_t construct_func)
{
    RX_ASSERT(api_reg_object_func != nullptr);
    auto ret = api_reg_object_func(g_plugin, id.c_ptr(), construct_func);
    return ret;
}
rx_result register_domain_runtime(const rx_node_id& id, rx_domain_constructor_t construct_func)
{
    RX_ASSERT(api_reg_domain_func != nullptr);
    auto ret = api_reg_domain_func(g_plugin, id.c_ptr(), construct_func);
    return ret;
}
rx_result register_application_runtime(const rx_node_id& id, rx_application_constructor_t construct_func)
{
    RX_ASSERT(api_reg_application_func != nullptr);
    auto ret = api_reg_application_func(g_plugin, id.c_ptr(), construct_func);
    return ret;
}

// Class rx_platform_api::rx_object 

rx_item_type rx_object::type_id = rx_item_type::rx_object_type;

rx_item_type rx_object::runtime_type_id = rx_item_type::rx_object;

rx_object::rx_object()
{
	impl_.def = &_g_object_def_;
	bind_as_shared(&impl_.anchor);
}


rx_object::~rx_object()
{
}



rx_result rx_object::initialize_object (rx_init_context& ctx)
{
	return true;
}

rx_result rx_object::start_object (rx_start_context& ctx)
{
	return true;
}

rx_result rx_object::stop_object ()
{
	return true;
}

rx_result rx_object::deinitialize_object ()
{
	return true;
}


// Class rx_platform_api::rx_application 

rx_item_type rx_application::type_id = rx_item_type::rx_application_type;

rx_item_type rx_application::runtime_type_id = rx_item_type::rx_application;

rx_application::rx_application()
{
	impl_.def = &_g_application_def_;
	bind_as_shared(&impl_.anchor);
}


rx_application::~rx_application()
{
}



rx_result rx_application::initialize_application (rx_init_context& ctx)
{
	return true;
}

rx_result rx_application::start_application (rx_start_context& ctx)
{
	return true;
}

rx_result rx_application::stop_application ()
{
	return true;
}

rx_result rx_application::deinitialize_application ()
{
	return true;
}


// Class rx_platform_api::rx_domain 

rx_item_type rx_domain::type_id = rx_item_type::rx_domain_type;

rx_item_type rx_domain::runtime_type_id = rx_item_type::rx_domain;

rx_domain::rx_domain()
{
	impl_.def = &_g_domain_def_;
	bind_as_shared(&impl_.anchor);
}


rx_domain::~rx_domain()
{
}



rx_result rx_domain::initialize_domain (rx_init_context& ctx)
{
	return true;
}

rx_result rx_domain::start_domain (rx_start_context& ctx)
{
	return true;
}

rx_result rx_domain::stop_domain ()
{
	return true;
}

rx_result rx_domain::deinitialize_domain ()
{
	return true;
}


} // namespace rx_platform_api

