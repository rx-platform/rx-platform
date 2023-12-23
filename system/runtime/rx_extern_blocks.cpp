

/****************************************************************************
*
*  system\runtime\rx_extern_blocks.cpp
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


#include "pch.h"


// rx_extern_blocks
#include "system/runtime/rx_extern_blocks.h"



extern "C"
{
    rx_result_struct c_update_source(void* whose, full_value_type val)
    {
        rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>* self = (rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>*)whose;
        return self->update_source(rx_value(val)).move();
    }
    void c_result_update_source(void* whose, rx_result_struct result, runtime_transaction_id_t id)
    {
        rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>* self = (rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>*)whose;
        self->result_update_source(result, id);
    }
    void c_source_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>* self = (rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>*)whose;
        self->start_timer(timer, period);
    }
    void c_source_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>* self = (rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>*)whose;
        self->suspend_timer(timer);
    }
    void c_source_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>* self = (rx_platform::runtime::blocks::extern_source_runtime<plugin_source_runtime_struct>*)whose;
        self->destroy_timer(timer);
    }

    host_source_def_struct _g_source_def_
    { 
        {
            nullptr
            , nullptr
            , c_source_start_timer
            , c_source_suspend_timer
            , c_source_destroy_timer
        }
        , c_update_source
        , c_result_update_source
    };


    rx_result_struct c_mapper_write_pending(void* whose
        , runtime_transaction_id_t id, int test, rx_security_handle_t identity, typed_value_type val)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>* self 
            = (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>*)whose;
        return self->mapper_write(id, test!=0, identity, rx_simple_value(val)).move();
    }
    void c_mapper_map_current(void* whose)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>* self 
            = (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>*)whose;
        self->extern_map_current_value();
    }
    void c_mapper_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>* self
            = (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>*)whose;
        self->start_timer(timer, period);
    }
    void c_mapper_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>* self 
            = (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>*)whose;
        self->suspend_timer(timer);
    }
    void c_mapper_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>* self 
            = (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct>*)whose;
        self->destroy_timer(timer);
    }

    host_mapper_def_struct _g_mapper_def_
    {
        {
            nullptr
            , nullptr
            , c_mapper_start_timer
            , c_mapper_suspend_timer
            , c_mapper_destroy_timer
        }
        , c_mapper_write_pending
        , c_mapper_map_current
    };


    rx_result_struct c_mapper_write_pending3(void* whose
        , runtime_transaction_id_t id, int test, rx_security_handle_t identity, typed_value_type val)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>* self =
            (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>*)whose;
        return self->mapper_write(id, test != 0, identity, rx_simple_value(val)).move();
    }
    void c_mapper_map_current3(void* whose)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>* self =
            (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>*)whose;
        self->extern_map_current_value();
    }
    void c_mapper_start_timer3(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>* self =
            (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>*)whose;
        self->start_timer(timer, period);
    }
    void c_mapper_suspend_timer3(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>* self =
            (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>*)whose;
        self->suspend_timer(timer);
    }
    void c_mapper_destroy_timer3(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>* self =
            (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>*)whose;
        self->destroy_timer(timer);
    }
    rx_result_struct c_mapper_execute_pending3(void* whose
        , runtime_transaction_id_t id, int test, rx_security_handle_t identity, struct typed_value_type val)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>* self =
            (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>*)whose;
        return self->mapper_execute(id, test != 0, identity, rx_simple_value(val)).move();
    }

    void c_mapper_get_method_inputs_model3(void* whose, struct bytes_value_struct_t* data)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>* self =
            (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>*)whose;

    }

    void c_mapper_get_method_outputs_model3(void* whose, struct bytes_value_struct_t* data)
    {
        rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>* self =
            (rx_platform::runtime::blocks::extern_mapper_runtime<plugin_mapper_runtime_struct3>*)whose;

    }

    host_mapper_def_struct3 _g_mapper_def3_
    {
        {
            nullptr
            , nullptr
            , c_mapper_start_timer3
            , c_mapper_suspend_timer3
            , c_mapper_destroy_timer3
        }
        , c_mapper_write_pending3
        , c_mapper_map_current3
        , c_mapper_execute_pending3
        , c_mapper_get_method_inputs_model3
        , c_mapper_get_method_outputs_model3
    };

    rx_result_struct c_filter_changed(void* whose)
    {
        rx_platform::runtime::blocks::extern_filter_runtime* self = (rx_platform::runtime::blocks::extern_filter_runtime*)whose;
        return self->extern_filter_changed().move();
    }
    void c_filter_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::blocks::extern_filter_runtime* self = (rx_platform::runtime::blocks::extern_filter_runtime*)whose;
        self->start_timer(timer, period);
    }
    void c_filter_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_filter_runtime* self = (rx_platform::runtime::blocks::extern_filter_runtime*)whose;
        self->suspend_timer(timer);
    }
    void c_filter_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_filter_runtime* self = (rx_platform::runtime::blocks::extern_filter_runtime*)whose;
        self->destroy_timer(timer);
    }

    host_filter_def_struct _g_filter_def_
    { 
        {
            nullptr
            , nullptr
            , c_filter_start_timer
            , c_filter_suspend_timer
            , c_filter_destroy_timer
        }
        , c_filter_changed
    };



    void c_struct_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::blocks::extern_struct_runtime* self = (rx_platform::runtime::blocks::extern_struct_runtime*)whose;
        self->start_timer(timer, period);
    }
    void c_struct_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_struct_runtime* self = (rx_platform::runtime::blocks::extern_struct_runtime*)whose;
        self->suspend_timer(timer);
    }
    void c_struct_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_struct_runtime* self = (rx_platform::runtime::blocks::extern_struct_runtime*)whose;
        self->destroy_timer(timer);
    }

    host_struct_def_struct _g_struct_def_
    {
        {
            nullptr
            , nullptr
            , c_struct_start_timer
            , c_struct_suspend_timer
            , c_struct_destroy_timer
        }
    };


    void c_event_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::blocks::extern_event_runtime* self = (rx_platform::runtime::blocks::extern_event_runtime*)whose;
        self->start_timer(timer, period);
    }
    void c_event_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_event_runtime* self = (rx_platform::runtime::blocks::extern_event_runtime*)whose;
        self->suspend_timer(timer);
    }
    void c_event_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::blocks::extern_event_runtime* self = (rx_platform::runtime::blocks::extern_event_runtime*)whose;
        self->destroy_timer(timer);
    }
    void c_event_fired(void* whose, struct typed_value_type val, runtime_ctx_ptr ctx)
    {
        rx_platform::runtime::blocks::extern_event_runtime* self = (rx_platform::runtime::blocks::extern_event_runtime*)whose;
        self->extern_event_fired(val);
    }
    void c_event_get_model(void* whose, struct bytes_value_struct_t* data)
    {

    }


    host_event_def_struct _g_event_def_
    {
        {
            nullptr
            , nullptr
            , c_event_start_timer
            , c_event_suspend_timer
            , c_event_destroy_timer
        }
        , c_event_fired
        , c_event_get_model
    };
}


namespace rx_platform {

namespace runtime {

namespace blocks {

// Parameterized Class rx_platform::runtime::blocks::extern_source_runtime 

template <typename implT>
extern_source_runtime<implT>::extern_source_runtime (implT* impl)
      : impl_(impl)
    , source_runtime(&impl->anchor)
{
    impl_->host = this;
    impl_->host_def = &_g_source_def_;
}


template <typename implT>
extern_source_runtime<implT>::~extern_source_runtime()
{
    rx_release_lock_reference(&impl_->anchor);
}



template <typename implT>
rx_result extern_source_runtime<implT>::initialize_source (runtime::runtime_init_context& ctx)
{
    return impl_->def->init_source(impl_->anchor.target, &ctx, (uint8_t)get_value_type());
}

template <typename implT>
rx_result extern_source_runtime<implT>::start_source (runtime::runtime_start_context& ctx)
{
    return impl_->def->start_source(impl_->anchor.target, &ctx);
}

template <typename implT>
rx_result extern_source_runtime<implT>::stop_source (runtime::runtime_stop_context& ctx)
{
    return impl_->def->stop_source(impl_->anchor.target);
}

template <typename implT>
rx_result extern_source_runtime<implT>::deinitialize_source (runtime::runtime_deinit_context& ctx)
{
    return impl_->def->deinit_source(impl_->anchor.target);
}

template <typename implT>
bool extern_source_runtime<implT>::supports_input () const
{
    return (impl_->io_data & RX_IO_DATA_INPUT_MASK) != 0;
}

template <typename implT>
bool extern_source_runtime<implT>::supports_output () const
{
    return (impl_->io_data & RX_IO_DATA_OUTPUT_MASK) != 0;
}

template <typename implT>
rx_result extern_source_runtime<implT>::source_write (write_data&& data, runtime_process_context* ctx)
{
    return impl_->def->write_source(impl_->anchor.target, data.transaction_id, data.test ? 1 : 0, data.identity, data.value.move(), ctx);
}

template <typename implT>
rx_result extern_source_runtime<implT>::update_source (rx_value&& val)
{
    val.increment_signal_level();
    return source_value_changed(std::move(val));
}

template <typename implT>
void extern_source_runtime<implT>::result_update_source (rx_result&& result, runtime_transaction_id_t id)
{
    source_result_received(std::move(result), id);
}

template <typename implT>
void extern_source_runtime<implT>::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

template <typename implT>
void extern_source_runtime<implT>::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

template <typename implT>
void extern_source_runtime<implT>::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}

template class extern_source_runtime<plugin_source_runtime_struct>;

// Parameterized Class rx_platform::runtime::blocks::extern_mapper_runtime 

template <typename implT>
extern_mapper_runtime<implT>::extern_mapper_runtime (implT* impl)
      : impl_(impl)
{
}


template <typename implT>
extern_mapper_runtime<implT>::~extern_mapper_runtime()
{
}



template <typename implT>
rx_result extern_mapper_runtime<implT>::initialize_mapper (runtime::runtime_init_context& ctx)
{
    return impl_->def->init_mapper(impl_->anchor.target, &ctx, (uint8_t)get_value_type());
}

template <typename implT>
rx_result extern_mapper_runtime<implT>::start_mapper (runtime::runtime_start_context& ctx)
{
    return impl_->def->start_mapper(impl_->anchor.target, &ctx);
}

template <typename implT>
rx_result extern_mapper_runtime<implT>::stop_mapper (runtime::runtime_stop_context& ctx)
{
    return impl_->def->stop_mapper(impl_->anchor.target);
}

template <typename implT>
rx_result extern_mapper_runtime<implT>::deinitialize_mapper (runtime::runtime_deinit_context& ctx)
{
    return impl_->def->deinit_mapper(impl_->anchor.target);
}

template <typename implT>
bool extern_mapper_runtime<implT>::supports_read () const
{
    return (impl_->io_data & RX_IO_DATA_OUTPUT_MASK) != 0;
}

template <typename implT>
bool extern_mapper_runtime<implT>::supports_write () const
{
    return (impl_->io_data & RX_IO_DATA_INPUT_MASK) != 0;
}

template <typename implT>
void extern_mapper_runtime<implT>::mapped_value_changed (rx_value&& val, runtime_process_context* ctx)
{
    impl_->def->mapped_value_changed(impl_->anchor.target, val.move(), ctx);
}

template <typename implT>
void extern_mapper_runtime<implT>::mapper_result_received (rx_result&& result, runtime_transaction_id_t id, runtime_process_context* ctx)
{
    impl_->def->mapper_result_received(impl_->anchor.target, result.move(), id, ctx);
}

template <typename implT>
void extern_mapper_runtime<implT>::mapper_execute_result_received (rx_result&& result, values::rx_simple_value out_data, runtime_transaction_id_t id, runtime_process_context* ctx)
{
    impl_->def->mapper_execute_result_received(impl_->anchor.target, result.move(), id, out_data.move(), ctx);
}

template <typename implT>
void extern_mapper_runtime<implT>::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

template <typename implT>
void extern_mapper_runtime<implT>::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

template <typename implT>
void extern_mapper_runtime<implT>::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}

template <typename implT>
rx_result extern_mapper_runtime<implT>::mapper_write (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val)
{
    write_data data;
    data.identity = identity;
    data.internal = false;
    data.test = test;
    data.transaction_id = id;
    data.value = std::move(val);
    mapper_write_pending(std::move(data));
    return true;
}

template <typename implT>
rx_result extern_mapper_runtime<implT>::mapper_execute (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val)
{
    execute_data data;
    data.identity = identity;
    data.internal = false;
    data.test = test;
    data.transaction_id = id;
    data.value = std::move(val);
    mapper_execute_pending(std::move(data));
    return true;
}

template <typename implT>
void extern_mapper_runtime<implT>::extern_map_current_value () const
{
    map_current_value();
}

template <typename implT>
byte_string extern_mapper_runtime<implT>::extern_get_method_inputs ()
{
    return byte_string();
}

template <typename implT>
byte_string extern_mapper_runtime<implT>::extern_get_method_outputs ()
{
    return byte_string();
}


template <>
extern_mapper_runtime<plugin_mapper_runtime_struct_t>::extern_mapper_runtime(plugin_mapper_runtime_struct_t* impl)
    : impl_(impl)
    , mapper_runtime(&impl->anchor)
{
    impl_->host = this;
    impl_->host_def = &_g_mapper_def_;
}

template <>
extern_mapper_runtime<plugin_mapper_runtime_struct3_t>::extern_mapper_runtime(plugin_mapper_runtime_struct3_t* impl)
    : impl_(impl)
    , mapper_runtime(&impl->anchor)
{
    impl_->host = this;
    impl_->host_def = &_g_mapper_def3_;
}


template <>
void extern_mapper_runtime<plugin_mapper_runtime_struct_t>::mapper_execute_result_received(rx_result&& result, values::rx_simple_value out_data, runtime_transaction_id_t id, runtime_process_context* ctx)
{
}

template class extern_mapper_runtime< plugin_mapper_runtime_struct>;
template class extern_mapper_runtime< plugin_mapper_runtime_struct3>;
// Class rx_platform::runtime::blocks::extern_filter_runtime 

extern_filter_runtime::extern_filter_runtime (plugin_filter_runtime_struct* impl)
      : impl_(impl)
    , filter_runtime(&impl->anchor)
{
    impl_->host = this;
    impl_->host_def = &_g_filter_def_;
}


extern_filter_runtime::~extern_filter_runtime()
{
}



rx_result extern_filter_runtime::initialize_filter (runtime::runtime_init_context& ctx)
{
    return impl_->def->init_filter(impl_->anchor.target, &ctx);
}

rx_result extern_filter_runtime::start_filter (runtime::runtime_start_context& ctx)
{
    return impl_->def->start_filter(impl_->anchor.target, &ctx);
}

rx_result extern_filter_runtime::stop_filter (runtime::runtime_stop_context& ctx)
{
    return impl_->def->stop_filter(impl_->anchor.target);
}

rx_result extern_filter_runtime::deinitialize_filter (runtime::runtime_deinit_context& ctx)
{
    return impl_->def->deinit_filter(impl_->anchor.target);
}

bool extern_filter_runtime::supports_input () const
{
    return (impl_->io_data & RX_IO_DATA_INPUT_MASK) != 0;
}

bool extern_filter_runtime::supports_output () const
{
    return (impl_->io_data & RX_IO_DATA_OUTPUT_MASK) != 0;
}

rx_result extern_filter_runtime::filter_input (rx_value& val)
{
    full_value_type temp_val = val.move();
    rx_result result(impl_->def->filter_input(impl_->anchor.target, &temp_val));
    val = rx_value(temp_val);
    return result;
}

rx_result extern_filter_runtime::filter_output (rx_simple_value& val)
{
    typed_value_type temp_val = val.move();
    rx_result result(impl_->def->filter_output(impl_->anchor.target, &temp_val));
    val = rx_simple_value(temp_val);
    return result;
}

void extern_filter_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_filter_runtime::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_filter_runtime::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}

rx_result extern_filter_runtime::extern_filter_changed ()
{
    return filter_changed();
}


// Class rx_platform::runtime::blocks::extern_struct_runtime 

extern_struct_runtime::extern_struct_runtime (plugin_struct_runtime_struct* impl)
      : impl_(impl)
{
    impl_->host = this;
    impl_->host_def = &_g_struct_def_;
}


extern_struct_runtime::~extern_struct_runtime()
{
}



rx_result extern_struct_runtime::initialize_struct (runtime::runtime_init_context& ctx)
{
    return impl_->def->init_struct(impl_->anchor.target, &ctx);
}

rx_result extern_struct_runtime::start_struct (runtime::runtime_start_context& ctx)
{
    return impl_->def->start_struct(impl_->anchor.target, &ctx);
}

rx_result extern_struct_runtime::stop_struct (runtime::runtime_stop_context& ctx)
{
    return impl_->def->stop_struct(impl_->anchor.target);
}

rx_result extern_struct_runtime::deinitialize_struct (runtime::runtime_deinit_context& ctx)
{
    return impl_->def->deinit_struct(impl_->anchor.target);
}

void extern_struct_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_struct_runtime::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_struct_runtime::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}


// Class rx_platform::runtime::blocks::extern_event_runtime 

extern_event_runtime::extern_event_runtime (plugin_event_runtime_struct* impl)
      : impl_(impl)
    , event_runtime(&impl->anchor)
{
    impl_->host = this;
    impl_->host_def = &_g_event_def_;
}


extern_event_runtime::~extern_event_runtime()
{
}



rx_result extern_event_runtime::initialize_event (runtime::runtime_init_context& ctx)
{
    return impl_->def->init_event(impl_->anchor.target, &ctx);
}

rx_result extern_event_runtime::start_event (runtime::runtime_start_context& ctx)
{
    return impl_->def->start_event(impl_->anchor.target, &ctx);
}

rx_result extern_event_runtime::stop_event (runtime::runtime_stop_context& ctx)
{
    return impl_->def->stop_event(impl_->anchor.target);
}

rx_result extern_event_runtime::deinitialize_event (runtime::runtime_deinit_context& ctx)
{
    return impl_->def->deinit_event(impl_->anchor.target);
}

void extern_event_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_event_runtime::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_event_runtime::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}

byte_string extern_event_runtime::extern_get_arguments ()
{
    return byte_string();
}

void extern_event_runtime::extern_event_fired (rx_simple_value data)
{
    event_fired(std::move(data));
}


// Class rx_platform::runtime::blocks::extern_variable_runtime 

extern_variable_runtime::extern_variable_runtime (plugin_variable_runtime_struct* impl)
      : impl_(impl)
{
}


extern_variable_runtime::~extern_variable_runtime()
{
}



rx_result extern_variable_runtime::initialize_variable (runtime::runtime_init_context& ctx)
{
    return impl_->def->init_variable(impl_->anchor.target, &ctx);
}

rx_result extern_variable_runtime::start_variable (runtime::runtime_start_context& ctx)
{
    return impl_->def->start_variable(impl_->anchor.target, &ctx);
}

rx_result extern_variable_runtime::stop_variable (runtime::runtime_stop_context& ctx)
{
    return impl_->def->stop_variable(impl_->anchor.target);
}

rx_result extern_variable_runtime::deinitialize_variable (runtime::runtime_deinit_context& ctx)
{
    return impl_->def->deinit_variable(impl_->anchor.target);
}

void extern_variable_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_variable_runtime::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_variable_runtime::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}


} // namespace blocks
} // namespace runtime
} // namespace rx_platform

