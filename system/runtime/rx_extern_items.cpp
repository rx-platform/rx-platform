

/****************************************************************************
*
*  system\runtime\rx_extern_items.cpp
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


// rx_extern_items
#include "system/runtime/rx_extern_items.h"

#include "lib/rx_ser_bin.h"
#include "sys_internal/rx_inf.h"

extern "C"
{
    rx_result_struct c_object_post_job(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        return rx_result(RX_NOT_IMPLEMENTED).move();
        
    }
    runtime_handle_t c_object_create_timer(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        rx_platform::runtime::items::extern_object_runtime* self = (rx_platform::runtime::items::extern_object_runtime*)whose;
        switch (type)
        {
        case RX_TIMER_REGULAR:
            return rx_platform::extern_timers::instance().create_timer(job, period, self->get_jobs_queue());
        case RX_TIMER_CALC:
            return rx_platform::extern_timers::instance().create_calc_timer(job, period, self->get_jobs_queue());
        case RX_TIMER_IO:
            return rx_platform::extern_timers::instance().create_timer(job, period, rx_internal::infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_IO));
        default:
            return 0;//unknown type
        }
    }
    void c_object_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::items::extern_object_runtime* self = (rx_platform::runtime::items::extern_object_runtime*)whose;
        self->start_timer(timer, period);
    }
    void c_object_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::items::extern_object_runtime* self = (rx_platform::runtime::items::extern_object_runtime*)whose;
        self->suspend_timer(timer);
    }
    void c_object_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::items::extern_object_runtime* self = (rx_platform::runtime::items::extern_object_runtime*)whose;
        self->destroy_timer(timer);
    }

    host_object_def_struct _g_object_def_
    {
        {
            c_object_post_job
            , c_object_create_timer
            , c_object_start_timer
            , c_object_suspend_timer
            , c_object_destroy_timer
        }
    };
    rx_result_struct c_application_post_job(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        return rx_result(RX_NOT_IMPLEMENTED).move();

    }
    runtime_handle_t c_application_create_timer(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        rx_platform::runtime::items::extern_application_runtime* self = (rx_platform::runtime::items::extern_application_runtime*)whose;
        switch (type)
        {
        case RX_TIMER_REGULAR:
            return rx_platform::extern_timers::instance().create_timer(job, period, self->get_jobs_queue());
        case RX_TIMER_CALC:
            return rx_platform::extern_timers::instance().create_calc_timer(job, period, self->get_jobs_queue());
        case RX_TIMER_IO:
            return rx_platform::extern_timers::instance().create_timer(job, period, rx_internal::infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_IO));
        default:
            return 0;//unknown type
        }
    }
    void c_application_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::items::extern_application_runtime* self = (rx_platform::runtime::items::extern_application_runtime*)whose;
        self->start_timer(timer, period);
    }
    void c_application_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::items::extern_application_runtime* self = (rx_platform::runtime::items::extern_application_runtime*)whose;
        self->suspend_timer(timer);
    }
    void c_application_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::items::extern_application_runtime* self = (rx_platform::runtime::items::extern_application_runtime*)whose;
        self->destroy_timer(timer);
    }

    host_application_def_struct _g_application_def_
    {
        {
            c_application_post_job
            , c_application_create_timer
            , c_application_start_timer
            , c_application_suspend_timer
            , c_application_destroy_timer
        }
    };

    rx_result_struct c_domain_post_job(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        return rx_result(RX_NOT_IMPLEMENTED).move();

    }
    runtime_handle_t c_domain_create_timer(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        rx_platform::runtime::items::extern_domain_runtime* self = (rx_platform::runtime::items::extern_domain_runtime*)whose;
        switch (type)
        {
        case RX_TIMER_REGULAR:
            return rx_platform::extern_timers::instance().create_timer(job, period, self->get_jobs_queue());
        case RX_TIMER_CALC:
            return rx_platform::extern_timers::instance().create_calc_timer(job, period, self->get_jobs_queue());
        case RX_TIMER_IO:
            return rx_platform::extern_timers::instance().create_timer(job, period, rx_internal::infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_IO));
        default:
            return 0;//unknown type
        }
    }
    void c_domain_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::items::extern_domain_runtime* self = (rx_platform::runtime::items::extern_domain_runtime*)whose;
        self->start_timer(timer, period);
    }
    void c_domain_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::items::extern_domain_runtime* self = (rx_platform::runtime::items::extern_domain_runtime*)whose;
        self->suspend_timer(timer);
    }
    void c_domain_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::items::extern_domain_runtime* self = (rx_platform::runtime::items::extern_domain_runtime*)whose;
        self->destroy_timer(timer);
    }

    host_domain_def_struct _g_domain_def_
    {
        {
            c_domain_post_job
            , c_domain_create_timer
            , c_domain_start_timer
            , c_domain_suspend_timer
            , c_domain_destroy_timer
        }
    };

    rx_result_struct c_alloc_buffer(void* whose, rx_packet_buffer* buffer)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        auto result = self->alloc_io_buffer();
        if (result)
        {
            result.value().detach(buffer);
            return rx_result(true).move();
        }
        else
        {
            return rx_result(result.errors()).move();
        }
        
    }

    rx_result_struct c_listen(void* whose, const struct protocol_address_def* local_address, const struct protocol_address_def* remote_address)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        return self->listen_extern(local_address, remote_address).move();
    }
    rx_result_struct c_connect(void* whose, const struct protocol_address_def* local_address, const struct protocol_address_def* remote_address)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        return self->connect_extern(local_address, remote_address).move();
    }
    rx_result_struct c_unbind_stack_endpoint(void* whose, struct rx_protocol_stack_endpoint* what)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        return self->unbind_stack_endpoint(what).move();
    }
    rx_result_struct c_disconnect_stack_endpoint(void* whose, struct rx_protocol_stack_endpoint* what)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        return self->disconnect_stack_endpoint(what).move();
    }

    void c_release_buffer(void* whose, rx_packet_buffer buffer)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        rx::io::rx_io_buffer temp;
        temp.attach(&buffer);
        self->release_io_buffer(std::move(temp));
    }
    rx_result_struct c_port_post_job(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        switch (type)
        {
        case RX_JOB_REGULAR:
            if (period == 0)
            {
                self->post_own_job(job);
                return rx_result(true).move();
            }
        case RX_JOB_SLOW:
            //return rx_platform::extern_timers::instance().create_calc_timer(job, period, self->get_jobs_queue());
        case RX_JOB_IO:
            //return rx_platform::extern_timers::instance().create_timer(job, period, rx_internal::infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_IO));
        default:
            return rx_result(RX_NOT_SUPPORTED).move();
        }
    }

    runtime_handle_t c_port_create_timer(void* whose, int type, plugin_job_struct* job, uint32_t period)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        switch (type)
        {
        case RX_TIMER_REGULAR:
            return rx_platform::extern_timers::instance().create_timer(job, period, self->get_jobs_queue());
        case RX_TIMER_CALC:
            return rx_platform::extern_timers::instance().create_calc_timer(job, period, self->get_jobs_queue());
        case RX_TIMER_IO:
            return rx_platform::extern_timers::instance().create_timer(job, period, rx_internal::infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_IO));
        default:
            return 0;//unknown type
        }
    }
    void c_port_start_timer(void* whose, runtime_handle_t timer, uint32_t period)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        self->start_timer(timer, period);
    }
    void c_port_suspend_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        self->suspend_timer(timer);
    }
    void c_port_destroy_timer(void* whose, runtime_handle_t timer)
    {
        rx_platform::runtime::items::extern_port_runtime* self = (rx_platform::runtime::items::extern_port_runtime*)whose;
        self->destroy_timer(timer);
    }

    host_port_def_struct _g_port_def_
    {
        c_alloc_buffer
        ,c_release_buffer

        ,c_listen
        ,c_connect

        ,c_unbind_stack_endpoint
        ,c_disconnect_stack_endpoint

        ,{
            c_port_post_job
            , c_port_create_timer
            , c_port_start_timer
            , c_port_suspend_timer
            , c_port_destroy_timer
        }
    };

}


namespace rx_platform {

namespace runtime {

namespace items {

// Class rx_platform::runtime::items::extern_object_runtime 

extern_object_runtime::extern_object_runtime (plugin_object_runtime_struct* impl)
      : impl_(impl)
    , object_runtime(&impl->anchor)
{
    impl_->host = this;
    impl_->host_def = &_g_object_def_;
}


extern_object_runtime::~extern_object_runtime()
{
}



void extern_object_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_object_runtime::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_object_runtime::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}

rx_result extern_object_runtime::initialize_runtime (runtime_init_context& ctx)
{
    return impl_->def->init_object(impl_->anchor.target, &ctx);
}

rx_result extern_object_runtime::start_runtime (runtime_start_context& ctx)
{
    return impl_->def->start_object(impl_->anchor.target, &ctx);
}

rx_result extern_object_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
    return impl_->def->deinit_object(impl_->anchor.target);
}

rx_result extern_object_runtime::stop_runtime (runtime_stop_context& ctx)
{
    return impl_->def->stop_object(impl_->anchor.target);
}

void extern_object_runtime::fill_code_info (std::ostream& info, const string_type& name)
{
    string_value_struct data;
    impl_->def->code_info(impl_->anchor.target, name.c_str(), &data);
    if (data.size > 0)
        info << rx_c_str(&data);
    else
        info << "No information available for " << name << "!";
}


// Class rx_platform::runtime::items::extern_port_runtime 

extern_port_runtime::extern_port_runtime (plugin_port_runtime_struct* impl)
      : impl_(impl)
    , port_runtime(&impl->anchor)
{
    impl_->host = this;
    impl_->host_def = &_g_port_def_;
}


extern_port_runtime::~extern_port_runtime()
{
}



void extern_port_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_port_runtime::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_port_runtime::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}

rx_result extern_port_runtime::initialize_runtime (runtime_init_context& ctx)
{
    return impl_->def->init_port(impl_->anchor.target, &ctx);
}

rx_result extern_port_runtime::start_runtime (runtime_start_context& ctx)
{
    return impl_->def->start_port(impl_->anchor.target, &ctx);
}

rx_result extern_port_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
    return impl_->def->deinit_port(impl_->anchor.target);
}

rx_result extern_port_runtime::stop_runtime (runtime_stop_context& ctx)
{
    return impl_->def->stop_port(impl_->anchor.target);
}

void extern_port_runtime::fill_code_info (std::ostream& info, const string_type& name)
{
    string_value_struct data;
    impl_->def->code_info(impl_->anchor.target, name.c_str(), &data);
    if (data.size > 0)
        info << rx_c_str(&data);
    else
        info << "No information available for " << name << "!";
}

void extern_port_runtime::stack_assembled ()
{
    impl_->def->stack_assembled(impl_->anchor.target);
}

void extern_port_runtime::stack_disassembled ()
{
    impl_->def->stack_disassembled(impl_->anchor.target);
}

rx_protocol_stack_endpoint* extern_port_runtime::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
    return impl_->def->construct_listener_endpoint(impl_->anchor.target, local_address, remote_address);
}

rx_protocol_stack_endpoint* extern_port_runtime::construct_initiator_endpoint ()
{
    return impl_->def->construct_initiator_endpoint(impl_->anchor.target);
}

void extern_port_runtime::destroy_endpoint (rx_protocol_stack_endpoint* what)
{
    impl_->def->destroy_endpoint(impl_->anchor.target, what);
}

rx_result extern_port_runtime::listen_extern (const protocol_address* local_address, const protocol_address* remote_address)
{
    return listen(local_address, remote_address);
}

rx_result extern_port_runtime::connect_extern (const protocol_address* local_address, const protocol_address* remote_address)
{
    return connect(local_address, remote_address);
}

void extern_port_runtime::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
    memory::std_buffer temp_buff;
    serialization::std_buffer_writer writer(temp_buff);
    if (writer.write_init_values(nullptr, binder_data))
    {
        impl_->def->extract_bind_address(impl_->anchor.target, (uint8_t*)temp_buff.get_data(), temp_buff.get_size(), &local_addr, &remote_addr);
    }
}

void extern_port_runtime::post_own_job (plugin_job_struct* what)
{
    auto job = rx_create_reference<extern_job>(what);
    get_context()->own_pending(job);
}


// Class rx_platform::runtime::items::extern_application_runtime 

extern_application_runtime::extern_application_runtime (plugin_application_runtime_struct* impl)
      : impl_(impl)
    , application_runtime(&impl->anchor)
{
    impl_->host = this;
    impl_->host_def = &_g_application_def_;
}


extern_application_runtime::~extern_application_runtime()
{
}



void extern_application_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_application_runtime::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_application_runtime::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}

rx_result extern_application_runtime::initialize_runtime (runtime_init_context& ctx)
{
    return impl_->def->init_application(impl_->anchor.target, &ctx);
}

rx_result extern_application_runtime::start_runtime (runtime_start_context& ctx)
{
    return impl_->def->start_application(impl_->anchor.target, &ctx);
}

rx_result extern_application_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
    return impl_->def->deinit_application(impl_->anchor.target);
}

rx_result extern_application_runtime::stop_runtime (runtime_stop_context& ctx)
{
    return impl_->def->stop_application(impl_->anchor.target);
}


// Class rx_platform::runtime::items::extern_domain_runtime 

extern_domain_runtime::extern_domain_runtime (plugin_domain_runtime_struct* impl)
      : impl_(impl)
    , domain_runtime(&impl->anchor)
{
    impl_->host = this;
    impl_->host_def = &_g_domain_def_;
}


extern_domain_runtime::~extern_domain_runtime()
{
}



void extern_domain_runtime::start_timer (runtime_handle_t handle, uint32_t period)
{
    rx_platform::extern_timers::instance().start_timer(handle, period);
}

void extern_domain_runtime::suspend_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().suspend_timer(handle);
}

void extern_domain_runtime::destroy_timer (runtime_handle_t handle)
{
    rx_platform::extern_timers::instance().destroy_timer(handle);
}

rx_result extern_domain_runtime::initialize_runtime (runtime_init_context& ctx)
{
    return impl_->def->init_domain(impl_->anchor.target, &ctx);
}

rx_result extern_domain_runtime::start_runtime (runtime_start_context& ctx)
{
    return impl_->def->start_domain(impl_->anchor.target, &ctx);
}

rx_result extern_domain_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
    return impl_->def->deinit_domain(impl_->anchor.target);
}

rx_result extern_domain_runtime::stop_runtime (runtime_stop_context& ctx)
{
    return impl_->def->stop_domain(impl_->anchor.target);
}


} // namespace items
} // namespace runtime
} // namespace rx_platform

