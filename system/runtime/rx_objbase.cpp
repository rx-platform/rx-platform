

/****************************************************************************
*
*  system\runtime\rx_objbase.cpp
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


// rx_objbase
#include "system/runtime/rx_objbase.h"

#include "rx_configuration.h"
#include "runtime_internal/rx_runtime_instance.h"
#include "sys_internal/rx_inf.h"
#include "rx_runtime_holder.h"
#include "interfaces/rx_port_stack_construction.h"
#include "interfaces/rx_port_stack_passive.h"
#include "interfaces/rx_port_stack_active.h"


namespace rx_platform {
void rx_split_item_path(const string_type& full_path, string_type& object_path, string_type& item_path)
{
	auto idx = full_path.rfind(RX_DIR_DELIMETER);
	if (idx == string_type::npos)
	{// no directory stuff
		idx = full_path.find(RX_OBJECT_DELIMETER);
		if (idx == string_type::npos)
		{// just plain object name
			object_path = full_path;
		}
		else
		{
			object_path = full_path.substr(0, idx);
			item_path = full_path.substr(idx + 1);
		}
	}
	else
	{// we have directory stuff
		idx = full_path.find(RX_OBJECT_DELIMETER, idx + 1);
		if (idx == string_type::npos)
		{// just object path
			object_path = full_path;
		}
		else
		{
			object_path = full_path.substr(0, idx);
			item_path = full_path.substr(idx + 1);
		}
	}
}

namespace runtime {

namespace items {

// Class rx_platform::runtime::items::object_runtime 

rx_item_type object_runtime::type_id = rx_item_type::rx_object;

object_runtime::object_runtime()
      : context_(nullptr),
        executer_(-1)
{
}

object_runtime::object_runtime (lock_reference_struct* extern_data)
      : context_(nullptr),
        executer_(-1)
	, reference_object(extern_data)
{
}


object_runtime::~object_runtime()
{
}



rx_result object_runtime::initialize_runtime (runtime_init_context& ctx)
{
    context_ = ctx.context;
	return true;
}

rx_result object_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
	return true;
}

rx_result object_runtime::start_runtime (runtime_start_context& ctx)
{
	return true;
}

rx_result object_runtime::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}

threads::job_thread* object_runtime::get_jobs_queue ()
{
	return rx_internal::infrastructure::server_runtime::instance().get_executer(executer_);
}

void object_runtime::add_periodic_job (jobs::periodic_job::smart_ptr job)
{
	rx_internal::infrastructure::server_runtime::instance().append_timer_job(job, get_jobs_queue());
}


// Class rx_platform::runtime::items::application_runtime 

rx_item_type application_runtime::type_id = rx_item_type::rx_application;

application_runtime::application_runtime()
      : context_(nullptr),
        executer_(-1)
{
}

application_runtime::application_runtime (lock_reference_struct* extern_data)
      : context_(nullptr),
        executer_(-1)
	, reference_object(extern_data)
{
}


application_runtime::~application_runtime()
{
}



rx_result application_runtime::initialize_runtime (runtime_init_context& ctx)
{
    return true;
}

rx_result application_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
	return true;
}

rx_result application_runtime::start_runtime (runtime_start_context& ctx)
{
	return true;
}

rx_result application_runtime::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}

threads::job_thread* application_runtime::get_jobs_queue ()
{
	return rx_internal::infrastructure::server_runtime::instance().get_executer(executer_);
}

void application_runtime::add_periodic_job (jobs::periodic_job::smart_ptr job)
{
	rx_internal::infrastructure::server_runtime::instance().append_timer_job(job, get_jobs_queue());
}


// Class rx_platform::runtime::items::domain_runtime 

rx_item_type domain_runtime::type_id = rx_item_type::rx_domain;

domain_runtime::domain_runtime()
      : context_(nullptr),
        executer_(-1)
{
}

domain_runtime::domain_runtime (lock_reference_struct* extern_data)
      : context_(nullptr),
        executer_(-1)
	, reference_object(extern_data)
{
}


domain_runtime::~domain_runtime()
{
}



rx_result domain_runtime::initialize_runtime (runtime_init_context& ctx)
{
	return true;
}

rx_result domain_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
	return true;
}

rx_result domain_runtime::start_runtime (runtime_start_context& ctx)
{
	return true;
}

rx_result domain_runtime::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}

threads::job_thread* domain_runtime::get_jobs_queue ()
{
	return rx_internal::infrastructure::server_runtime::instance().get_executer(executer_);
}

void domain_runtime::add_periodic_job (jobs::periodic_job::smart_ptr job)
{
	rx_internal::infrastructure::server_runtime::instance().append_timer_job(job, get_jobs_queue());
}

// has to be before call

// Class rx_platform::runtime::items::port_runtime 

rx_item_type port_runtime::type_id = rx_item_type::rx_port;

port_runtime::port_runtime()
      : context_(nullptr),
        executer_(-1)
{
}

port_runtime::port_runtime (lock_reference_struct* extern_data)
      : context_(nullptr),
        executer_(-1)
	, reference_object(extern_data)
{
}


port_runtime::~port_runtime()
{
}



rx_result port_runtime::initialize_runtime (runtime_init_context& ctx)
{
	return true;
}

rx_result port_runtime::start_runtime (runtime_start_context& ctx)
{
	return true;
}

rx_result port_runtime::deinitialize_runtime (runtime_deinit_context& ctx)
{
	return true;
}

rx_result port_runtime::stop_runtime (runtime_stop_context& ctx)
{
	return true;
}

void port_runtime::stack_assembled ()
{
}

void port_runtime::stack_disassembled ()
{
}

rx_result port_runtime::listen (const protocol_address* local_address, const protocol_address* remote_address)
{
	return rx_internal::interfaces::port_stack::stack_passive::passive_builder::send_listen(runtime_, local_address, remote_address);
}

rx_result port_runtime::connect (const protocol_address* local_address, const protocol_address* remote_address)
{
	return rx_internal::interfaces::port_stack::stack_passive::passive_builder::send_connect(runtime_, local_address, remote_address);
}

rx_result port_runtime::start_listen (const protocol_address* local_address, const protocol_address* remote_address)
{
    return RX_NOT_SUPPORTED;
}

rx_result_with<port_connect_result> port_runtime::start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint)
{
	return RX_NOT_SUPPORTED;
}

rx_result port_runtime::stop_passive ()
{
	return RX_NOT_SUPPORTED;
}

rx_protocol_stack_endpoint* port_runtime::construct_listener_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
	RX_ASSERT(false);
	return nullptr;
}

rx_protocol_stack_endpoint* port_runtime::construct_initiator_endpoint (const protocol_address* local_address, const protocol_address* remote_address)
{
	RX_ASSERT(false);
	return nullptr;
}

rx_result port_runtime::add_stack_endpoint (rx_protocol_stack_endpoint* what, const io::any_address& local_addr, const io::any_address& remote_addr)
{
	return rx_internal::interfaces::port_stack::stack_active::active_builder::add_stack_endpoint(runtime_, what, local_addr, remote_addr);
}

rx_result port_runtime::register_routing_endpoint (rx_protocol_stack_endpoint* what)
{
	return rx_internal::interfaces::port_stack::stack_active::active_builder::register_routing_endpoint(runtime_, what);
}

rx_result port_runtime::close_all_endpoints ()
{
	rx_internal::interfaces::port_stack::stack_active::active_builder::close_all_endpoints(runtime_);
	return true;
}

rx_result port_runtime::unbind_stack_endpoint (rx_protocol_stack_endpoint* what)
{
	return rx_internal::interfaces::port_stack::stack_active::active_builder::unbind_stack_endpoint(runtime_, what);
}

rx_result port_runtime::disconnect_stack_endpoint (rx_protocol_stack_endpoint* what)
{
	return rx_internal::interfaces::port_stack::stack_active::active_builder::disconnect_stack_endpoint(runtime_, what);
}

threads::job_thread* port_runtime::get_jobs_queue ()
{
    return rx_internal::infrastructure::server_runtime::instance().get_executer(executer_);
}

void port_runtime::add_periodic_job (jobs::periodic_job::smart_ptr job)
{
	rx_internal::infrastructure::server_runtime::instance().append_timer_job(job, get_jobs_queue());
}

threads::job_thread* port_runtime::get_io_queue ()
{
	return rx_internal::infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_META);
}

void port_runtime::extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr)
{
}

security::security_context_ptr port_runtime::get_security_context ()
{
	return runtime_->get_instance_data().get_security_context();
}

rx_result_with<io::rx_io_buffer> port_runtime::alloc_io_buffer ()
{
	return rx_internal::interfaces::port_stack::port_buffers::alloc_io_buffer(runtime_);
}

void port_runtime::release_io_buffer (io::rx_io_buffer buff)
{
	return rx_internal::interfaces::port_stack::port_buffers::release_io_buffer(runtime_, std::move(buff));
}


} // namespace items
} // namespace runtime
} // namespace rx_platform

