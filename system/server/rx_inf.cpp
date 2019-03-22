

/****************************************************************************
*
*  system\server\rx_inf.cpp
*
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_thread
#include "lib/rx_thread.h"
// rx_inf
#include "system/server/rx_inf.h"

#include "rx_server.h"
#include "lib/rx_io.h"
#include "api/rx_meta_api.h"
#include "model/rx_meta_internals.h"


namespace rx_platform {

namespace infrastructure {

// Class rx_platform::infrastructure::server_rt 

server_rt::server_rt()
	: server_object(runtime::objects::object_creation_data{
		RX_NS_SERVER_RT_NAME
		, RX_NS_SYSTEM_DOM_ID
		, RX_NS_SYSTEM_DOM_TYPE_ID
		, true
		, RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_OBJ_NAME RX_DIR_DELIMETER_STR RX_NS_SYSTEM_OBJ_NAME RX_DIR_DELIMETER_STR RX_NS_SERVER_RT_NAME
		, rx_application_ptr::null_ptr
		, rx_domain_ptr::null_ptr
	})
{
}


server_rt::~server_rt()
{
}



rx_result server_rt::initialize (hosting::rx_platform_host* host, runtime_data_t& data)
{
	if (data.io_pool_size > 0)
	{
		io_pool_ = server_dispatcher_object::smart_ptr(data.io_pool_size, IO_POOL_NAME, IO_POOL_ID, RX_DOMAIN_IO);
	}
	if (data.genereal_pool_size > 0)
	{
		general_pool_ = server_dispatcher_object::smart_ptr(data.genereal_pool_size, GENERAL_POOL_NAME, GENERAL_POOL_ID, RX_DOMAIN_GENERAL);
	}
	if (data.workers_pool_size > 0)
	{
		workers_ = domains_pool::smart_ptr(data.workers_pool_size);
		workers_->reserve();
	}
	general_timer_ = std::make_unique<rx::threads::timer>("Timer", 0);
	if (data.has_callculation_timer)
		callculation_timer_ = std::make_unique<rx::threads::timer>("Calc",0);


	return true;
}

rx_result server_rt::deinitialize ()
{
	if (io_pool_)
		io_pool_ = server_dispatcher_object::smart_ptr::null_ptr;
	if (general_pool_)
		general_pool_ = server_dispatcher_object::smart_ptr::null_ptr;

	if(workers_)
		workers_->clear();

	if (general_timer_)
		general_timer_.release();
	if (callculation_timer_)
		callculation_timer_.release();

	return true;
}

void server_rt::append_timer_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now)
{
	rx::threads::job_thread* executer = get_executer(rx_thread_context());
	if (general_timer_)
		general_timer_->append_job(job,executer,period, now);
}

rx_result server_rt::start (hosting::rx_platform_host* host, const runtime_data_t& data)
{
	if (io_pool_)
		io_pool_->get_pool().run(RX_PRIORITY_ABOVE_NORMAL);
	if (general_pool_)
		general_pool_->get_pool().run(RX_PRIORITY_NORMAL);
	if(workers_)
		workers_->run();
	if (general_timer_)
		general_timer_->start(RX_PRIORITY_HIGH);
	if (callculation_timer_)
		callculation_timer_->start(RX_PRIORITY_NORMAL);

	dispatcher_timer_ = rx_create_reference<dispatcher_subscribers_job>();
	if (callculation_timer_)
		callculation_timer_->append_job(dispatcher_timer_, &general_pool_->get_pool(), data.io_timer_period);
	if (general_timer_)
		general_timer_->append_job(dispatcher_timer_, &general_pool_->get_pool(), data.io_timer_period);

	return true;
}

rx_result server_rt::stop ()
{
	if (dispatcher_timer_)
	{
		dispatcher_timer_->cancel();
		dispatcher_timer_ = dispatcher_subscribers_job::smart_ptr::null_ptr;
	}
	if (io_pool_)
		io_pool_->get_pool().end();
	if (general_pool_)
		general_pool_->get_pool().end();

	if(workers_)
		workers_->end();

	if (general_timer_)
	{
		general_timer_->stop();
		general_timer_->wait_handle();
	}
	if (callculation_timer_)
	{
		callculation_timer_->stop();
		callculation_timer_->wait_handle();
	}

	return true;
}

void server_rt::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "_Runtime";
	has_own_code_info = true;
}

void server_rt::get_value (values::rx_value& val, const rx_time& ts, const rx_mode_type& mode) const
{
}

namespace_item_attributes server_rt::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

void server_rt::append_job (rx::jobs::job_ptr job)
{
	threads::job_thread* executer = get_executer(rx_thread_context());
	RX_ASSERT(executer);
	if (executer)
		executer->append(job);
}

rx::threads::job_thread* server_rt::get_executer (rx_thread_handle_t domain)
{
	switch (domain)
	{
	case RX_DOMAIN_GENERAL:
		if(general_pool_)
			return &general_pool_->get_pool();
		else
			return &io_pool_->get_pool();
	case RX_DOMAIN_IO:
		return &io_pool_->get_pool();
	case RX_DOMAIN_META:
		return &model::platform_types_manager::instance().get_worker();
	default:
		if(workers_)
			return workers_->get_executer(domain);
		else if(general_pool_)
			return &general_pool_->get_pool();
		else
			return &io_pool_->get_pool();
	}
}

void server_rt::append_calculation_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now)
{
	threads::job_thread* executer = get_executer(rx_thread_context());
	if(callculation_timer_)
		general_timer_->append_job(job, executer, period, now);
	if (general_timer_)
		general_timer_->append_job(job, executer, period, now);
}

void server_rt::append_io_job (rx::jobs::job_ptr job)
{
	io_pool_->get_pool().append(job);
}

void server_rt::append_general_job (rx::jobs::job_ptr job)
{
	if (general_pool_)
		return general_pool_->get_pool().append(job);
	else
		return io_pool_->get_pool().append(job);
}

void server_rt::append_slow_job (rx::jobs::job_ptr job)
{
	if (general_pool_)
		return general_pool_->get_pool().append(job);
	else
		return io_pool_->get_pool().append(job);
}

void server_rt::append_timer_io_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now)
{
	if (general_timer_)
		general_timer_->append_job(job, &io_pool_->get_pool(), period, now);
}

rx_time server_rt::get_created_time (values::rx_value& val) const
{
	return rx_gate::instance().get_started();
}


// Class rx_platform::infrastructure::server_dispatcher_object 

server_dispatcher_object::server_dispatcher_object (int count, const string_type& name, rx_thread_handle_t rx_thread_id, const rx_node_id& id)
      : threads_count_(count)
	, server_object(runtime::objects::object_creation_data{
		name
		, RX_NS_SYSTEM_DOM_ID
		, RX_NS_SYSTEM_DOM_TYPE_ID
		, true
		, ""s + RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_OBJ_NAME RX_DIR_DELIMETER_STR RX_NS_SYSTEM_OBJ_NAME RX_DIR_DELIMETER_STR + name
		, rx_application_ptr::null_ptr
		, rx_domain_ptr::null_ptr
		})
	, pool_(count, name, rx_thread_id)
{
	//register_const_value("count", count);
}


server_dispatcher_object::~server_dispatcher_object()
{
}



// Class rx_platform::infrastructure::dispatcher_subscribers_job 

dispatcher_subscribers_job::dispatcher_subscribers_job()
{
}


dispatcher_subscribers_job::~dispatcher_subscribers_job()
{
}



void dispatcher_subscribers_job::process ()
{
	io::dispatcher_subscriber::propagate_timer();
}


// Class rx_platform::infrastructure::domains_pool 

domains_pool::domains_pool (uint32_t pool_size)
      : pool_size_(pool_size)
	, server_object(runtime::objects::object_creation_data{
		WORKER_POOL_NAME
		, RX_NS_SYSTEM_DOM_ID
		, RX_NS_SYSTEM_DOM_TYPE_ID
		, true
		, RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_OBJ_NAME RX_DIR_DELIMETER_STR RX_NS_SYSTEM_OBJ_NAME RX_DIR_DELIMETER_STR WORKER_POOL_NAME
		, rx_application_ptr::null_ptr
		, rx_domain_ptr::null_ptr
		})
{
}


domains_pool::~domains_pool()
{
}



void domains_pool::run (int priority)
{
	for (auto one : workers_)
		one->start(priority);
}

void domains_pool::end (uint32_t timeout)
{
	for (auto one : workers_)
		one->end();
}

void domains_pool::append (job_ptr pjob)
{
}

void domains_pool::reserve ()
{
	workers_.reserve(pool_size_);
	for (uint32_t i = 0; i < pool_size_; i++)
		workers_.push_back(new threads::physical_job_thread("Worker",i));
}

void domains_pool::clear ()
{
	for (auto one : workers_)
		delete one;
	workers_.clear();
}

void domains_pool::append (rx::jobs::timer_job_ptr job, uint32_t domain)
{
	job_thread* thr = get_executer(domain);
	RX_ASSERT(thr);
	if (thr)
		thr->append(job);
}

rx::threads::job_thread* domains_pool::get_executer (rx_thread_handle_t domain)
{
	uint32_t size = pool_size_;
	RX_ASSERT(size);
	if (size == 0)
		return nullptr;
	else
	{
		uint32_t real_index = domain%size;
		return workers_[real_index];
	}
}


} // namespace infrastructure
} // namespace rx_platform

