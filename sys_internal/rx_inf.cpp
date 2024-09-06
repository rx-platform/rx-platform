

/****************************************************************************
*
*  sys_internal\rx_inf.cpp
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


// rx_thread
#include "system/threads/rx_thread.h"
// rx_data_source
#include "runtime_internal/rx_data_source.h"
// rx_inf
#include "sys_internal/rx_inf.h"

#include "system/server/rx_server.h"
#include "interfaces/rx_io.h"
#include "api/rx_meta_api.h"
#include "model/rx_meta_internals.h"
#include "runtime_internal/rx_runtime_internal.h"
#include "upython/upy_internal.h"
#include "sys_internal/rx_async_functions.h"


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



rx_result server_runtime::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
	// register protocol constructors
	char buff[0x100];
	size_t cpu_count = 1;
	uint32_t start_cpu = 0;
	uint32_t end_cpu = 0;
	uint64_t cpu_mask = 0;
	rx_collect_processor_info(buff, sizeof(buff) / sizeof(buff[0]), &cpu_count);


	if (data.processor.io_pool_size <= 0)
		data.processor.io_pool_size = 1;

	data.processor.io_pool_size = std::min((int)cpu_count, data.processor.io_pool_size);

	if (cpu_count > 1)
	{
		cpu_mask = ~(((uint64_t)-1) << data.processor.io_pool_size);
	}

	io_pool_ = server_dispatcher_object::smart_ptr(data.processor.io_pool_size, IO_POOL_NAME, RX_DOMAIN_IO);

	if (cpu_count > (size_t)data.processor.io_pool_size)
	{
		start_cpu = data.processor.io_pool_size;
	}
	end_cpu = (uint32_t)cpu_count - 1;

	slow_pool_ = server_dispatcher_object::smart_ptr(5, "Slow", RX_DOMAIN_SLOW);

	meta_pool_ = rx_create_reference<physical_thread_object>(META_POOL_NAME, RX_DOMAIN_META);

	if (data.processor.has_unassigned_pool)
	{
		unassigned_pool_ = rx_create_reference<physical_thread_object>(UNASSIGNED_POOL_NAME, RX_DOMAIN_UNASSIGNED);
	}
	if (data.processor.workers_pool_size > 0)
	{
		for (auto& one : workers_)
		{
			one = domains_pool::smart_ptr(data.processor.workers_pool_size, start_cpu, end_cpu);
			one->reserve();
		}
	}
	general_timer_ = std::make_unique<threads::timer>("Timer", 0);
	if (data.processor.has_calculation_timer)
		calculation_timer_ = std::make_unique<threads::timer>("Calc",0);

	extern_executer_ = data.processor.extern_executer;

	auto result = sys_runtime::platform_runtime_manager::instance().initialize(host, data.processor);

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

	result = ids_manager_.initialize(host, data, smart_this());

	return result;
}

void server_runtime::deinitialize ()
{
	ids_manager_.deinitialize();

	if (extern_executer_)
		extern_executer_ = nullptr;

	if (io_pool_)
		io_pool_ = server_dispatcher_object::smart_ptr::null_ptr;

	if (slow_pool_)
		slow_pool_ = server_dispatcher_object::smart_ptr::null_ptr;
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

void server_runtime::append_timer_job (timer_job_ptr job, threads::job_thread* whose)
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
	if (slow_pool_)
		slow_pool_->get_pool().run(RX_PRIORITY_IDLE);
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
	low_priority_ = rx_create_reference<low_priority_house_keeping>();
	if (general_timer_)
	{
		general_timer_->append_job(dispatcher_timer_, &io_pool_->get_pool());
		dispatcher_timer_->start(io_data.io_timer_period);

		if(slow_pool_)
			general_timer_->append_job(low_priority_, &slow_pool_->get_pool());
		else
			general_timer_->append_job(low_priority_, &meta_pool_->get_pool());
		low_priority_->start(6000);
	}

	return true;
}

void server_runtime::stop ()
{

	if (io_pool_)
		io_pool_->get_pool().end();
	if (slow_pool_)
		slow_pool_->get_pool().end();
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

void server_runtime::append_job (job_ptr job)
{
	threads::job_thread* executer = get_executer(rx_thread_context());
	RX_ASSERT(executer);
	if (executer)
		executer->append(job);
}

threads::job_thread* server_runtime::get_executer (rx_thread_handle_t domain)
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
		case RX_DOMAIN_SLOW:
			if(slow_pool_)
				return &slow_pool_->get_pool();
			else if (unassigned_pool_)
				return &unassigned_pool_->get_pool();
			else
				return &meta_pool_->get_pool();
		case RX_DOMAIN_EXTERN:
			if (extern_executer_)
				return extern_executer_;
			else
				return &meta_pool_->get_pool();
//		case RX_DOMAIN_PYTHON:
//#ifdef UPYTHON_SUPPORT
//			return &rx_platform::python::upy_thread::instance();
//#endif
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

void server_runtime::append_calculation_job (timer_job_ptr job, threads::job_thread* whose)
{
	if (whose == nullptr)
		whose = get_executer(rx_thread_context());
	if(calculation_timer_)
		calculation_timer_->append_job(job, whose);
	if (general_timer_)
		general_timer_->append_job(job, whose);
}

void server_runtime::append_io_job (job_ptr job)
{
	io_pool_->get_pool().append(job);
}

void server_runtime::append_slow_job (job_ptr job)
{
	if (slow_pool_)
		slow_pool_->get_pool().append(job);
	else
		meta_pool_->get_pool().append(job);
}

void server_runtime::append_timer_io_job (timer_job_ptr job)
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


unique_ids_manager& server_runtime::get_ids_manager ()
{
  return ids_manager_;
}


// Class rx_internal::infrastructure::server_dispatcher_object 

server_dispatcher_object::server_dispatcher_object (int count, const string_type& name, rx_thread_handle_t rx_thread_id, uint64_t cpu_mask)
      : threads_count_(count),
        last_proc_time_(0),
        max_proc_time_(0),
        queue_size_(0),
        max_queue_size_(0)
	, pool_(count, name, rx_thread_id, cpu_mask)
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
		last_proc_time_.bind("Pool.LastProcTime", ctx);
		max_proc_time_.bind("Pool.MaxProcTime", ctx);
		queue_size_.bind("Pool.QueueSize", ctx);
		max_queue_size_.bind("Pool.MaxSize", ctx);
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

domains_pool::domains_pool (uint32_t pool_size, uint32_t start_cpu, uint32_t end_cpu)
      : pool_size_(pool_size),
        start_cpu_(pool_size),
        end_cpu_(pool_size),
        last_proc_time_(0),
        max_proc_time_(0),
        queue_size_(0),
        max_queue_size_(0)
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

void domains_pool::append (timer_job_ptr job, uint32_t domain)
{
	job_thread* thr = get_executer(domain);
	RX_ASSERT(thr);
	if (thr)
		thr->append(job);
}

threads::job_thread* domains_pool::get_executer (rx_thread_handle_t domain)
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
      : last_proc_time_(0),
        max_proc_time_(0),
        queue_size_(0),
        max_queue_size_(0)
	, pool_(name, rx_thread_id)
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
		last_proc_time_.bind("Pool.LastProcTime", ctx);
		max_proc_time_.bind("Pool.MaxProcTime", ctx);
		queue_size_.bind("Pool.QueueSize", ctx);
		max_queue_size_.bind("Pool.MaxSize", ctx);
	}
	return result;
}


// Class rx_internal::infrastructure::low_priority_house_keeping 

low_priority_house_keeping::low_priority_house_keeping()
{
}


low_priority_house_keeping::~low_priority_house_keeping()
{
}



void low_priority_house_keeping::process ()
{
	size_t result = rx_heap_house_keeping();
	if (result)
	{
		std::ostringstream ss;
		ss << "Heap claimed "
			<< result / 1024
			<< "KB of memory.";
		HOST_LOG_INFO("low_priority_house_keeping", 900, ss.str());
	}
}


// Class rx_internal::infrastructure::unique_ids_manager 

unique_ids_manager::unique_ids_manager()
      : first_available_(0),
        list_size_(0),
        claim_size_(0),
        border_size_(0),
        claiming_(false)
{
}


unique_ids_manager::~unique_ids_manager()
{
}



runtime_transaction_id_t unique_ids_manager::get_new_unique_id ()
{
	std::scoped_lock _(lock_);
	if (first_available_ > 0 && list_size_ > 0)
	{
		runtime_transaction_id_t ret = first_available_;
		first_available_++;
		list_size_--;
		if (list_size_ <= border_size_ && !claiming_)
		{
			do_the_claim(claim_size_ - list_size_);
		}			
		return ret;
	}
	else
	{
		return 0;
	}
}

rx_result unique_ids_manager::initialize (hosting::rx_platform_host* host, configuration_data_t& data, rx_reference_ptr anchor)
{
	anchor_ = anchor;
	auto ret = host->get_user_storage();
	if (ret)
	{
		storage_ = ret.move_value();
		first_available_ = 0;
		list_size_ = 0;
		claim_size_ = data.other.ids_prefetch;
		border_size_ = data.other.ids_prefetch_sp;
		do_the_claim(claim_size_);

		return true;
	}
	ret.register_error("Ids Manager unable to get storage object");
	return ret.errors();;
}

void unique_ids_manager::deinitialize ()
{
	if (storage_ && first_available_ > 0)
		storage_->set_next_unique_id(first_available_);
}

void unique_ids_manager::do_the_claim (size_t size)
{
	claiming_ = true;
	rx_post_function_to(RX_DOMAIN_SLOW, anchor_
		, [this, size]() mutable
		{
			auto ret = storage_->get_new_unique_ids(size);
			if (ret > 0)
			{
				std::scoped_lock _(lock_);
				first_available_ = ret;
				list_size_ += size;
				claiming_ = false;
			}
		});
}


} // namespace infrastructure
} // namespace rx_internal

