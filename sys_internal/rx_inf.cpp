

/****************************************************************************
*
*  sys_internal\rx_inf.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_thread
#include "lib/rx_thread.h"
// rx_data_source
#include "runtime_internal/rx_data_source.h"
// rx_inf
#include "sys_internal/rx_inf.h"

#include "system/server/rx_server.h"
#include "interfaces/rx_io.h"
#include "api/rx_meta_api.h"
#include "model/rx_meta_internals.h"
#include "runtime_internal/rx_runtime_internal.h"


namespace rx_internal {

namespace infrastructure {
namespace
{
server_runtime* g_object = nullptr;
}

// Class rx_internal::infrastructure::server_runtime 

server_runtime::server_runtime()
      : extern_executer_(nullptr)
{
}


server_runtime::~server_runtime()
{
}



rx_result server_runtime::initialize (hosting::rx_platform_host* host, runtime_data_t& data, const io_manager_data_t& io_data)
{
	// register protocol constructors
	
	if (data.io_pool_size <= 0)
		data.io_pool_size = 1;
	io_pool_ = server_dispatcher_object::smart_ptr(data.io_pool_size, IO_POOL_NAME, RX_DOMAIN_IO);
	meta_pool_ = rx_create_reference<physical_thread_object>(META_POOL_NAME, RX_DOMAIN_META);

	if (data.has_unassigned_pool)
	{
		unassigned_pool_ = rx_create_reference<physical_thread_object>(UNASSIGNED_POOL_NAME, RX_DOMAIN_UNASSIGNED);
	}
	if (data.workers_pool_size > 0)
	{
		for (auto& one : workers_)
		{
			one = domains_pool::smart_ptr(data.workers_pool_size);
			one->reserve();
		}
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

	result = model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
		RX_POOL_TYPE_ID, [this] {
			rx_object_impl_ptr ret = io_pool_;
			return ret;
		});

	result = model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
		RX_UNASSIGNED_POOL_TYPE_ID, [this] {
			rx_object_impl_ptr ret;
			if (unassigned_pool_)
				ret = unassigned_pool_;
			else
				ret = meta_pool_;
			return ret;
		});

	result = model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
		RX_META_POOL_TYPE_ID, [this] {
			rx_object_impl_ptr ret = meta_pool_;
			return ret;
		});

	return result;
}

void server_runtime::deinitialize ()
{
	if (extern_executer_)
		extern_executer_ = nullptr;

	if (io_pool_)
		io_pool_ = server_dispatcher_object::smart_ptr::null_ptr;
	if (unassigned_pool_)
		unassigned_pool_ = physical_thread_object::smart_ptr::null_ptr;
	if (meta_pool_)
		meta_pool_ = physical_thread_object::smart_ptr::null_ptr;

	for (auto& one : workers_)
	{
		if(one)
			one->clear();
	}

	if (general_timer_)		
		general_timer_.reset();
	if (calculation_timer_)
		calculation_timer_.reset();

	sys_runtime::platform_runtime_manager::instance().deinitialize();

	delete this;
}

void server_runtime::append_timer_job (rx::jobs::timer_job_ptr job, threads::job_thread* whose)
{
	if(whose==nullptr)
		whose = get_executer(rx_thread_context());
	if (general_timer_)
		general_timer_->append_job(job,whose);
}

rx_result server_runtime::start (hosting::rx_platform_host* host, const runtime_data_t& data, const io_manager_data_t& io_data)
{
	RX_ASSERT(general_timer_);
	RX_ASSERT(meta_pool_);
	RX_ASSERT(io_pool_);

	if (meta_pool_)
		meta_pool_->get_pool().run(RX_PRIORITY_IDLE);
	if (io_pool_)
		io_pool_->get_pool().run(RX_PRIORITY_HIGH);
	if (unassigned_pool_)
		unassigned_pool_->get_pool().run(RX_PRIORITY_LOW);
	
	if(workers_[(uint8_t)rx_domain_priority::low])
		workers_[(uint8_t)rx_domain_priority::low]->run(RX_PRIORITY_LOW);
	if (workers_[(uint8_t)rx_domain_priority::normal])
		workers_[(uint8_t)rx_domain_priority::normal]->run(RX_PRIORITY_NORMAL);
	if (workers_[(uint8_t)rx_domain_priority::high])
		workers_[(uint8_t)rx_domain_priority::high]->run(RX_PRIORITY_HIGH);
	if (workers_[(uint8_t)rx_domain_priority::realtime])
		workers_[(uint8_t)rx_domain_priority::realtime]->run(RX_PRIORITY_REALTIME);
	
	if (general_timer_)
		general_timer_->start(RX_PRIORITY_REALTIME);
	if (calculation_timer_)
		calculation_timer_->start(RX_PRIORITY_NORMAL);

	dispatcher_timer_ = rx_create_reference<dispatcher_subscribers_job>();
	if (general_timer_)
	{
		general_timer_->append_job(dispatcher_timer_, &io_pool_->get_pool());
		dispatcher_timer_->start(io_data.io_timer_period);
	}

	return true;
}

void server_runtime::stop ()
{
	
	if (io_pool_)
		io_pool_->get_pool().end();
	if (unassigned_pool_)
		unassigned_pool_->get_pool().end();

	for (auto& one : workers_)
	{
		if (one)
			one->end();
	}
	if (dispatcher_timer_)
	{
		dispatcher_timer_->cancel();
		dispatcher_timer_ = dispatcher_subscribers_job::smart_ptr::null_ptr;
	}
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
	if (meta_pool_)
		meta_pool_->get_pool().end();
}

void server_runtime::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "_Runtime";
	has_own_code_info = true;
}

void server_runtime::get_value (values::rx_value& val, const rx_time& ts, const rx_mode_type& mode) const
{
}

namespace_item_attributes server_runtime::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

void server_runtime::append_job (rx::jobs::job_ptr job)
{
	threads::job_thread* executer = get_executer(rx_thread_context());
	RX_ASSERT(executer);
	if (executer)
		executer->append(job);
}

rx::threads::job_thread* server_runtime::get_executer (rx_thread_handle_t domain)
{
	uint8_t priority = RX_PRIORITY_FROM_DOMAIN(domain);
	domain = (domain & RX_DOMAIN_TYPE_MASK);
	if (domain > RX_DOMAIN_UPPER_LIMIT)
	{
		RX_ASSERT(priority == 0);
		switch (domain)
		{
		case RX_DOMAIN_UNASSIGNED:
			if (unassigned_pool_)
				return &unassigned_pool_->get_pool();
			else
				return &meta_pool_->get_pool();
		case RX_DOMAIN_IO:
			return &io_pool_->get_pool();
		case RX_DOMAIN_META:
			return &meta_pool_->get_pool();
		case RX_DOMAIN_EXTERN:
			if (extern_executer_)
				return extern_executer_;
		default:
			if (unassigned_pool_)
				return &unassigned_pool_->get_pool();
			else
				return &meta_pool_->get_pool();
		}
	}
	else
	{
		RX_ASSERT(priority < (uint8_t)rx_domain_priority::priority_count);
		if(priority < (uint8_t)rx_domain_priority::priority_count && workers_[priority])
			return workers_[priority]->get_executer(domain);
		else
			return &io_pool_->get_pool();
	}
}

void server_runtime::append_calculation_job (rx::jobs::timer_job_ptr job)
{
	threads::job_thread* executer = get_executer(rx_thread_context());
	if(calculation_timer_)
		general_timer_->append_job(job, executer);
	if (general_timer_)
		general_timer_->append_job(job, executer);
}

void server_runtime::append_io_job (rx::jobs::job_ptr job)
{
	io_pool_->get_pool().append(job);
}

void server_runtime::append_timer_io_job (rx::jobs::timer_job_ptr job)
{
	if (general_timer_)
		general_timer_->append_job(job, &io_pool_->get_pool());
}

rx_time server_runtime::get_created_time (values::rx_value& val) const
{
	return rx_gate::instance().get_started();
}

rx_internal::sys_runtime::data_source::data_controler* server_runtime::get_data_controler (rx_thread_handle_t domain)
{
	uint8_t priority = RX_PRIORITY_FROM_DOMAIN(domain);
	domain = (domain & RX_DOMAIN_TYPE_MASK);
	if (domain > RX_DOMAIN_UPPER_LIMIT)
	{
		RX_ASSERT(priority == 0);
		switch (domain)
		{
		case RX_DOMAIN_UNASSIGNED:
			if (unassigned_pool_)
				return unassigned_pool_->get_data_controler();
			else
				return meta_pool_->get_data_controler();
		case RX_DOMAIN_META:
			return meta_pool_->get_data_controler();
		}
	}
	else
	{
		RX_ASSERT(priority < (uint8_t)rx_domain_priority::priority_count);
		if (priority < (uint8_t)rx_domain_priority::priority_count && workers_[priority])
			return workers_[priority]->get_data_controler(domain);			
	}
	return nullptr;
}

int server_runtime::get_CPU (rx_thread_handle_t domain) const
{
	if (domain < RX_DOMAIN_UPPER_LIMIT && workers_[0])
	{
		domain = (domain & RX_DOMAIN_TYPE_MASK);
		int jebiga = workers_[0]->get_CPU(domain) + (io_pool_ ? io_pool_->get_CPU(domain) : 0);
		return jebiga;
	}
	else
		return -1;
}

rx_result server_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = object_runtime::initialize_runtime(ctx);
	if (result)
	{
		ctx.tags->set_item_static("Runtime.IOThreads", io_pool_->get_pool_size(), ctx);
		ctx.tags->set_item_static("Runtime.Workers", workers_[0] ? workers_[0]->get_pool_size() : (uint16_t)0, ctx);
		ctx.tags->set_item_static("Runtime.CalcTimer", calculation_timer_.operator bool(), ctx);
	}
	return result;
}

server_runtime& server_runtime::instance ()
{
	if (g_object == nullptr)
		g_object = new server_runtime();
	return *g_object;
}

runtime_data_t server_runtime::get_cpu_data ()
{
	runtime_data_t ret;
	ret.has_calculation_timer = calculation_timer_.operator bool();
	ret.has_unassigned_pool = unassigned_pool_.operator bool();
	ret.io_pool_size = io_pool_->get_pool_size();
	if (workers_[0])
		ret.workers_pool_size = workers_[0]->get_pool_size();
	else
		ret.workers_pool_size = 0;
	return ret;
}


// Class rx_internal::infrastructure::server_dispatcher_object 

server_dispatcher_object::server_dispatcher_object (int count, const string_type& name, rx_thread_handle_t rx_thread_id)
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

rx_result server_dispatcher_object::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = object_runtime::initialize_runtime(ctx);
	if (result)
	{
		ctx.tags->set_item_static("Pool.Threads", get_pool_size(), ctx);
	}
	return result;
}

uint16_t server_dispatcher_object::get_pool_size () const
{
	return (uint16_t)threads_count_;
}


// Class rx_internal::infrastructure::dispatcher_subscribers_job 

dispatcher_subscribers_job::dispatcher_subscribers_job()
{
}


dispatcher_subscribers_job::~dispatcher_subscribers_job()
{
}



void dispatcher_subscribers_job::process ()
{
	interfaces::io_endpoints::dispatcher_subscriber::propagate_timer();
}


// Class rx_internal::infrastructure::domains_pool 

domains_pool::domains_pool (uint32_t pool_size)
      : pool_size_(pool_size)
{
}


domains_pool::~domains_pool()
{
	for (auto one : workers_)
		delete one;
	for (auto one : data_controlers_)
		delete one;
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

rx_internal::sys_runtime::data_source::data_controler* domains_pool::get_data_controler (rx_thread_handle_t domain)
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

uint16_t domains_pool::get_pool_size () const
{
	return (uint16_t)pool_size_;
}


// Class rx_internal::infrastructure::physical_thread_object 

physical_thread_object::physical_thread_object (const string_type& name, rx_thread_handle_t rx_thread_id)
	: pool_(name, rx_thread_id)
{
	data_controler_ = new sys_runtime::data_source::data_controler(&pool_);
}


physical_thread_object::~physical_thread_object()
{
	delete data_controler_;
}



rx_result physical_thread_object::initialize_runtime (runtime::runtime_init_context& ctx)
{
	auto result = object_runtime::initialize_runtime(ctx);
	if (result)
	{
		ctx.tags->set_item_static("Pool.Threads", 1 , ctx);
	}
	return result;
}


} // namespace infrastructure
} // namespace rx_internal

