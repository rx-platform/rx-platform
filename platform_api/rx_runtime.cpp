

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_runtime.cpp
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


// rx_runtime
#include "rx_runtime.h"

#include "platform_api/rx_abi.h"
#include "lib/rx_values.h"
#include "lib/rx_ptr.h"


rxRegisterSourceRuntime_t api_reg_source_func;
rxRegisterMapperRuntime_t api_reg_mapper_func;
rxRegisterMapperRuntime3_t api_reg_mapper3_func;
rxRegisterFilterRuntime_t api_reg_filter_func;

rxRegisterVariableRuntime_t api_reg_variable_func;
rxRegisterStructRuntime_t api_reg_struct_func;
rxRegisterMethodRuntime_t api_reg_method_func;
rxRegisterProgramRuntime_t api_reg_program_func;
rxRegisterDisplayRuntime_t api_reg_display_func;

rxRegisterRelationRuntime_t api_reg_relation_func;

rxInitCtxBindItem_t api_bind_item_func;
rxInitCtxGetCurrentPath_t api_init_get_current_path;
rxInitCtxGetLocalValue_t api_init_get_local_value;
rxInitCtxSetLocalValue_t api_init_set_local_value;
rxInitCtxGetMappingValues_t api_get_mapping_values;
rxInitCtxGetSourceValues_t api_get_source_values;
rxInitCtxGetItemMeta_t api_item_meta;
rxInitCtxConnectItem_t api_connect_item_func;

rxStartCtxGetCurrentPath_t api_start_get_current_path;
rxStartCtxCreateTimer_t api_start_ctx_create_timer;
rxStartCtxGetLocalValue_t api_get_local_value;
rxStartCtxSetLocalValue_t api_set_local_value;
rxStartCtxSubscribeRelation_t api_subscribe_relation;

rxCtxGetValue_t api_get_value_func;
rxCtxSetValue_t api_set_value_func;
rxCtxWriteConnected_t api_write_connected_func;
rxCtxExecuteConnected_t api_execute_connected_func;
rxCtxSetAsyncPending_t api_set_async_pending_func;


extern "C"
{

    void c_get_code_info(void* whose, const char* name, string_value_struct* info)
    {
        rx_platform_api::rx_runtime* self = (rx_platform_api::rx_runtime*)whose;
        std::ostringstream ss;
        self->fill_code_info(ss, name);
        string_type temp_str = ss.str();
        if (temp_str.empty())
            temp_str = "No information available for "s + name + "!";
        rx_init_string_value_struct(info, temp_str.c_str(), -1);
    }


    rx_result_struct c_init_relation_stub(void* self, init_ctx_ptr ctx);
    rx_result_struct c_start_relation_stub(void* self, start_ctx_ptr ctx, int is_target);
    rx_result_struct c_stop_relation_stub(void* self, int is_target);
    rx_result_struct c_deinit_relation_stub(void* self);

    rx_result_struct c_make_target_relation_stub(void* whose, struct plugin_relation_runtime_struct_t** target);
    rx_result_struct c_relation_connected_stub(void* whose, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);
    rx_result_struct c_relation_disconnected_stub(void* whose, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to);

    plugin_relation_def_struct _g_relation_def_
    {
        c_get_code_info
        ,c_init_relation_stub
        ,c_start_relation_stub
        ,c_stop_relation_stub
        ,c_deinit_relation_stub

        ,c_make_target_relation_stub
        ,c_relation_connected_stub
        ,c_relation_disconnected_stub
    };



    rx_result_struct c_init_relation(rx_platform_api::rx_relation* self, init_ctx_ptr ctx)
    {
        self->bind_runtime(&self->impl_.host_def->runtime, self->impl_.host);
        rx_platform_api::rx_init_context ctx_obj(ctx);
        return self->initialize_relation(ctx_obj).move();
    }
    rx_result_struct c_start_relation(rx_platform_api::rx_relation* self, start_ctx_ptr ctx, int is_target)
    {
        rx_platform_api::rx_start_context ctx_obj(ctx, self->smart_this());
        return self->start_relation(ctx_obj, is_target).move();
    }
    rx_result_struct c_stop_relation(rx_platform_api::rx_relation* self, int is_target)
    {
        return self->stop_relation(is_target).move();
    }
    rx_result_struct c_deinit_relation(rx_platform_api::rx_relation* self)
    {
        return self->deinitialize_relation().move();
    }
    rx_result_struct c_make_target_relation(rx_platform_api::rx_relation* self, struct plugin_relation_runtime_struct_t** target)
    {
        auto ptr = self->make_target_relation();
        if (ptr)
        {
            ptr->bind_runtime(&ptr->impl_.host_def->runtime, self->impl_.host);
            // acquire a lock to object!!!
            rx_aquire_lock_reference(&ptr->impl_.anchor);
            *target = &ptr->impl_;
        }
        return rx_result(true).move();
    }
    rx_result_struct c_relation_connected(rx_platform_api::rx_relation* self, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to)
    {
        self->relation_connected(from, to);
        return rx_result(true).move();
    }
    rx_result_struct c_relation_disconnected(rx_platform_api::rx_relation* self, const struct rx_node_id_struct_t* from, const struct rx_node_id_struct_t* to)
    {
        self->relation_disconnected(from, to);
        return rx_result(true).move();
    }

}




namespace rx_platform_api {

rx_result register_source_runtime(const rx_node_id& id, rx_source_constructor_t construct_func)
{
    RX_ASSERT(api_reg_source_func != nullptr);
    plugin_source_register_data data;
    data.constructor = construct_func;
    data.register_func = nullptr;
    data.unregister_func = nullptr;
    auto ret = api_reg_source_func(get_rx_plugin(), id.c_ptr(), data);
    return ret;
}
rx_result register_mapper_runtime(const rx_node_id& id, rx_mapper_constructor_t construct_func)
{
    RX_ASSERT(api_reg_mapper_func != nullptr);
    auto ret = api_reg_mapper_func(get_rx_plugin(), id.c_ptr(), construct_func);
    return ret;
}
rx_result register_mapper_runtime3(const rx_node_id& id, rx_mapper_constructor3_t construct_func)
{
    RX_ASSERT(api_reg_mapper3_func != nullptr);
    auto ret = api_reg_mapper3_func(get_rx_plugin(), id.c_ptr(), construct_func);
    return ret;
}
rx_result register_filter_runtime(const rx_node_id& id, rx_filter_constructor_t construct_func)
{
    RX_ASSERT(api_reg_filter_func != nullptr);
    auto ret = api_reg_filter_func(get_rx_plugin(), id.c_ptr(), construct_func);
    return ret;
}
rx_result register_struct_runtime(const rx_node_id& id, rx_struct_constructor_t construct_func)
{
    RX_ASSERT(api_reg_struct_func != nullptr);
    auto ret = api_reg_struct_func(get_rx_plugin(), id.c_ptr(), construct_func);
    return ret;
}
rx_result register_variable_runtime(const rx_node_id& id, rx_variable_constructor_t construct_func)
{
    RX_ASSERT(api_reg_variable_func != nullptr);
    auto ret = api_reg_variable_func(get_rx_plugin(), id.c_ptr(), construct_func);
    return ret;
}
rx_result register_method_runtime(const rx_node_id& id, rx_method_constructor_t construct_func)
{
    RX_ASSERT(api_reg_method_func != nullptr);
    auto ret = api_reg_method_func(get_rx_plugin(), id.c_ptr(), construct_func);
    return ret;
}
rx_result register_program_runtime(const rx_node_id& id, rx_program_constructor_t construct_func)
{
    RX_ASSERT(api_reg_program_func != nullptr);
    auto ret = api_reg_program_func(get_rx_plugin(), id.c_ptr(), construct_func);
    return ret;
}
rx_result register_display_runtime(const rx_node_id& id, rx_display_constructor_t construct_func)
{
    RX_ASSERT(api_reg_display_func != nullptr);
    auto ret = api_reg_display_func(get_rx_plugin(), id.c_ptr(), construct_func);
    return ret;
}
rx_result register_relation_runtime(const rx_node_id& id, rx_relation_constructor_t construct_func, rx_runtime_register_func_t reg_function, rx_runtime_unregister_func_t unreg_function)
{
    RX_ASSERT(api_reg_relation_func != nullptr);
    plugin_relation_register_data data;
    data.constructor = construct_func;
    data.register_func = reg_function;
    data.unregister_func = unreg_function;
    auto ret = api_reg_relation_func(get_rx_plugin(), id.c_ptr(), data);
    return ret;
}

// Class rx_platform_api::rx_relation 

rx_relation::rx_relation()
{
    impl_.def = &_g_relation_def_;
    bind_as_shared(&impl_.anchor);
}


rx_relation::~rx_relation()
{
}



rx_result rx_relation::initialize_relation (rx_init_context& ctx)
{
    return true;
}

rx_result rx_relation::deinitialize_relation ()
{
    return true;
}

rx_result rx_relation::start_relation (rx_start_context& ctx, bool is_target)
{
    return true;
}

rx_result rx_relation::stop_relation (bool is_target)
{
    return true;
}

void rx_relation::relation_connected (rx_node_id from, rx_node_id to)
{
}

void rx_relation::relation_disconnected (rx_node_id from, rx_node_id to)
{
}

rx_relation::smart_ptr rx_relation::make_target_relation ()
{
    return smart_ptr::null_ptr;
}


// Class rx_platform_api::rx_init_context 

rx_init_context::rx_init_context (init_ctx_ptr ctx)
      : impl_(ctx)
{
}



rx_result_with<runtime_handle_t> rx_init_context::bind_item (const char* path, runtime_ctx_ptr* rt_ctx, bind_callback_data* callback)
{
    runtime_handle_t handle = 0;
    rx_result result = api_bind_item_func(impl_, path, &handle, rt_ctx, callback);
    if (result)
        return handle;
    else
        return result.errors();
}

rx_result_with<runtime_handle_t> rx_init_context::connect_item (const char* path, uint32_t rate, runtime_ctx_ptr* rt_ctx, connect_callback_data* callback)
{
    if (api_connect_item_func == nullptr)
        return RX_NOT_SUPPORTED;
    runtime_handle_t handle = 0;
    rx_result result = api_connect_item_func(impl_, path, rate, &handle, rt_ctx, callback);
    if (result)
        return handle;
    else
        return result.errors();
}

const char* rx_init_context::get_current_path ()
{
    return api_init_get_current_path(impl_);
}

rx_result rx_init_context::get_local_value (const string_type& path, values::rx_simple_value& val) const
{
    typed_value_type temp;
    rx_result ret = api_init_get_local_value(impl_, path.c_str(), &temp);
    if (ret)
    {
        val = rx_simple_value(temp);
    }
    return ret;
}

rx_result rx_init_context::set_local_value (const string_type& path, rx_simple_value&& value)
{
    typed_value_type temp = value.move();
    rx_result ret = api_init_set_local_value(impl_, path.c_str(), temp);
    return ret;
}

std::vector<rx_simple_value> rx_init_context::get_mapping_values (const rx_node_id& id, const string_type& path)
{
    values_array_struct data;
    rx_result result = api_get_mapping_values(impl_, id.c_ptr(), path.c_str(), &data);
    if (result)
    {
        std::vector<rx_simple_value> ret;
        if (data.size > 0)
        {
            ret.reserve(data.size);
            for (size_t i = 0; i < data.size; i++)
            {
                ret.emplace_back(rx_simple_value(&data.values[i]));
            }
        }
        rx_destory_values_array_struct(&data);
        return ret;
    }
    else
    {
        return std::vector<rx_simple_value>();
    }
}

std::vector<rx_simple_value> rx_init_context::get_source_values (const rx_node_id& id, const string_type& path)
{
    values_array_struct data;
    rx_result result = api_get_source_values(impl_, id.c_ptr(), path.c_str(), &data);
    if (result)
    {
        std::vector<rx_simple_value> ret;
        if (data.size > 0)
        {
            ret.reserve(data.size);
            for (size_t i = 0; i < data.size; i++)
            {
                ret.emplace_back(rx_simple_value(&data.values[i]));
            }
        }
        rx_destory_values_array_struct(&data);
        return ret;
    }
    else
    {
        return std::vector<rx_simple_value>();
    }
}

rx_node_id rx_init_context::get_node_id ()
{
    const rx_node_id_struct* id = NULL;
    const char* path = NULL;
    const char* name = NULL;
    api_item_meta(impl_, &id, &path, &name);
    if (id)
        return rx_node_id(id);
    else
        return rx_node_id();
}

string_type rx_init_context::get_path ()
{
    const rx_node_id_struct* id = NULL;
    const char* path = NULL;
    const char* name = NULL;
    api_item_meta(impl_, &id, &path, &name);
    if (path)
        return path;
    else
        return string_type();
}

string_type rx_init_context::get_name ()
{
    const rx_node_id_struct* id = NULL;
    const char* path = NULL;
    const char* name = NULL;
    api_item_meta(impl_, &id, &path, &name);
    if (name)
        return name;
    else
        return string_type();
}

string_type rx_init_context::get_full_path ()
{
    const rx_node_id_struct* id = NULL;
    const char* path = NULL;
    const char* name = NULL;
    api_item_meta(impl_, &id, &path, &name);
    if (path && name)
        return string_type(path) + "/" + name;
    else
        return string_type();
}


// Class rx_platform_api::rx_start_context 

rx_start_context::rx_start_context (start_ctx_ptr ctx, rx_reference_ptr anchor)
      : impl_(ctx),
        anchor_(anchor)
{
}



const char* rx_start_context::get_current_path ()
{
    return api_start_get_current_path(impl_);
}

rx_result rx_start_context::get_local_value (const string_type& path, values::rx_simple_value& val) const
{
    typed_value_type temp;
    rx_result ret = api_get_local_value(impl_, path.c_str(), &temp);
    if (ret)
    {
        val = rx_simple_value(temp);
    }
    return ret;
}

rx_result rx_start_context::set_local_value (const string_type& path, rx_simple_value&& value)
{
    typed_value_type temp = value.move();
    rx_result ret = api_set_local_value(impl_, path.c_str(), temp);
    return ret;
}

rx_result rx_start_context::register_relation_subscriber (const string_type& name, relation_subscriber_data* callback)
{
    return api_subscribe_relation(impl_, name.c_str(), callback);
}

runtime_handle_t rx_start_context::create_timer_internal (int type, callback_data::smart_ptr callback, uint32_t period)
{
    return api_start_ctx_create_timer(impl_, type, callback->c_ptr(), period);
}


// Class rx_platform_api::rx_runtime 


void rx_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    if(target_ && impl_)
        impl_->start_timer(target_, handle, period);
}

void rx_runtime::suspend_timer (runtime_handle_t handle)
{
    if (target_ && impl_)
        impl_->suspend_timer(target_, handle);
}

void rx_runtime::destroy_timer (runtime_handle_t handle)
{
    if (target_ && impl_)
    impl_->destroy_timer(target_, handle);
}

void rx_runtime::bind_runtime (host_runtime_def_struct* impl, void* target)
{
    target_ = target;
    impl_ = impl;
}

rx_result rx_runtime::post_job_internal (int type, callback_data::smart_ptr callback, uint32_t period)
{
    if (impl_->post_job)
        return impl_->post_job(target_, type, callback->c_ptr(), period);
    else
        return RX_NOT_SUPPORTED;
}

runtime_handle_t rx_runtime::create_timer_internal (int type, callback_data::smart_ptr callback, uint32_t period)
{
    if (impl_->create_timer)
        return impl_->create_timer(target_, type, callback->c_ptr(), period);
    else
        return 0;
}


// Class rx_platform_api::rx_process_context 

rx_process_context::rx_process_context()
      : impl_(nullptr)
{
}



void rx_process_context::bind (runtime_ctx_ptr ctx)
{
    impl_ = ctx;
}

bool rx_process_context::is_binded () const
{
    return impl_ != nullptr;
}

rx_result rx_process_context::get_value (runtime_handle_t handle, values::rx_simple_value& val) const
{
    typed_value_type temp_val;
    rx_result result= api_get_value_func(impl_, handle, &temp_val);
    if (result)
        val = temp_val;
    return result;
}

rx_result rx_process_context::set_value (runtime_handle_t handle, values::rx_simple_value&& val)
{
    return api_set_value_func(impl_, handle, val.move());
}

rx_result rx_process_context::write_connected (runtime_handle_t handle, values::rx_simple_value&& val, runtime_transaction_id_t trans_id)
{
    if (api_write_connected_func == nullptr)
        return RX_NOT_SUPPORTED;
    return api_write_connected_func(impl_, handle, val.move(), trans_id);
}

rx_result rx_process_context::execute_connected (runtime_handle_t handle, values::rx_simple_value&& val, runtime_transaction_id_t trans_id)
{
    if (api_execute_connected_func == nullptr)
        return RX_NOT_SUPPORTED;
    return api_execute_connected_func(impl_, handle, val.move(), trans_id);
}

void rx_process_context::set_async_pending (runtime_handle_t handle, values::rx_simple_value&& val)
{
    return api_set_async_pending_func(impl_, handle, val.move());
}


// Class rx_platform_api::rx_relation_subscriber 


} // namespace rx_platform_api

