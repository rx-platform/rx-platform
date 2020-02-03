

/****************************************************************************
*
*  system\server\rx_inf.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
// rx_data_source
#include "runtime_internal/rx_data_source.h"
// rx_inf
#include "system/server/rx_inf.h"

#include "rx_server.h"
#include "lib/rx_io.h"
#include "api/rx_meta_api.h"
#include "model/rx_meta_internals.h"
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_platform {

namespace infrastructure {

// Class rx_platform::infrastructure::server_rt 

server_rt::server_rt()
      : extern_executer_(nullptr)
{
}


server_rt::~server_rt()
{
}



rx_result server_rt::initialize (hosting::rx_platform_host* host, runtime_data_t& data, const io_manager_data_t& io_data)
{
	// register protocol constructors
	
	if (data.io_pool_size > 0)
	{
		io_pool_ = server_dispatcher_object::smart_ptr(data.io_pool_size, IO_POOL_NAME, RX_DOMAIN_IO, IO_POOL_ID);
	}
	if (data.genereal_pool_size > 0)
	{
		general_pool_ = server_dispatcher_object::smart_ptr(data.genereal_pool_size, GENERAL_POOL_NAME, RX_DOMAIN_GENERAL, GENERAL_POOL_ID);
	}
	if (data.workers_pool_size > 0)
	{
		workers_ = domains_pool::smart_ptr(data.workers_pool_size);
		workers_->reserve();
	}
	general_timer_ = std::make_unique<rx::threads::timer>("Timer", 0);
	if (data.has_calculation_timer)
		calculation_timer_ = std::make_unique<rx::threads::timer>("Calc",0);

	extern_executer_ = data.extern_executer;

	auto result = sys_runtime::platform_runtime_manager::instance().initialize(host, data);

	// register I/O constructors
	result = model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
		RX_NS_SERVER_RT_TYPE_ID, [this] {
			rx_object_impl_ptr ret = smart_this();
			return ret;
		});

	return result;
}

void server_rt::deinitialize ()
{
	if (extern_executer_)
		extern_executer_ = nullptr;

	if (io_pool_)
		io_pool_ = server_dispatcher_object::smart_ptr::null_ptr;
	if (general_pool_)
		general_pool_ = server_dispatcher_object::smart_ptr::null_ptr;

	if(workers_)
		workers_->clear();

	if (general_timer_)
		general_timer_.release();
	if (calculation_timer_)
		calculation_timer_.release();
}

void server_rt::append_timer_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now)
{
	rx::threads::job_thread* executer = get_executer(rx_thread_context());
	if (general_timer_)
		general_timer_->append_job(job,executer,period, now);
}

rx_result server_rt::start (hosting::rx_platform_host* host, const runtime_data_t& data, const io_manager_data_t& io_data)
{
	if (io_pool_)
		io_pool_->get_pool().run(RX_PRIORITY_ABOVE_NORMAL);
	if (general_pool_)
		general_pool_->get_pool().run(RX_PRIORITY_NORMAL);
	if(workers_)
		workers_->run();
	if (general_timer_)
		general_timer_->start(RX_PRIORITY_HIGH);
	if (calculation_timer_)
		calculation_timer_->start(RX_PRIORITY_NORMAL);

	dispatcher_timer_ = rx_create_reference<dispatcher_subscribers_job>();
	if (calculation_timer_)
		calculation_timer_->append_job(dispatcher_timer_, &general_pool_->get_pool(), io_data.io_timer_period);
	if (general_timer_)
		general_timer_->append_job(dispatcher_timer_, &general_pool_->get_pool(), io_data.io_timer_period);

	return true;
}

void server_rt::stop ()
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
	if (calculation_timer_)
	{
		calculation_timer_->stop();
		calculation_timer_->wait_handle();
	}
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
	case RX_DOMAIN_EXTERN:
		if (extern_executer_)
			return extern_executer_;
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
	if(calculation_timer_)
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

sys_runtime::data_source::data_controler* server_rt::get_data_controler (rx_thread_handle_t domain)
{
	return workers_->get_data_controler(domain);
}

int server_rt::get_CPU (rx_thread_handle_t domain) const
{
	if (domain < RX_DOMAIN_UPPER_LIMIT && workers_)
		return workers_->get_CPU(domain) + io_pool_ ? io_pool_->get_CPU(domain) : 0;
	else
		return 0;
}


// Class rx_platform::infrastructure::server_dispatcher_object 

server_dispatcher_object::server_dispatcher_object (int count, const string_type& name, rx_thread_handle_t rx_thread_id, const rx_node_id& id)
      : threads_count_(count)
	, pool_(count, name, rx_thread_id)
{
	//register_const_value("count", count);
}


server_dispatcher_object::~server_dispatcher_object()
{
}



int server_dispatcher_object::get_CPU (rx_thread_handle_t domain) const
{
	return pool_.get_CPU(domain);
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
{
	size_t count = workers_.size();
	for (size_t i = 0; i < count; i++)
	{

	}
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
	data_controlers_.reserve(pool_size_);
	for (uint32_t i = 0; i < pool_size_; i++)
	{
		auto temp = new threads::physical_job_thread("Worker", i);
		workers_.push_back(temp);
		data_controlers_.push_back(new sys_runtime::data_source::data_controler(temp));
	}
}

void domains_pool::clear ()
{
	for (uint32_t i = 0; i < pool_size_; i++)
	{
		delete data_controlers_[i];
		delete workers_[i];
	}
	data_controlers_.clear();
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

sys_runtime::data_source::data_controler* domains_pool::get_data_controler (rx_thread_handle_t domain)
{
	uint32_t size = pool_size_;
	RX_ASSERT(size);
	if (size == 0)
		return nullptr;
	else
	{
		uint32_t real_index = domain % size;
		return data_controlers_[real_index];
	}
}

int domains_pool::get_CPU (rx_thread_handle_t domain) const
{
	uint32_t size = pool_size_;
	RX_ASSERT(size);
	if (size == 0)
		return 0;
	else
	{
		return domain % size;
	}
}


} // namespace infrastructure
} // namespace rx_platform

