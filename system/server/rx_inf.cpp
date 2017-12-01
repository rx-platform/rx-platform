

/****************************************************************************
*
*  system\server\rx_inf.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "stdafx.h"


// rx_inf
#include "system/server/rx_inf.h"

#include "rx_server.h"
#include "lib/rx_io.h"


namespace rx_platform {

namespace runtime {

// Class rx_platform::runtime::server_rt 

server_rt::server_rt()
	: server_object(RX_NS_SERVER_RT_NAME, RX_NS_SERVER_RT_ID)
{
}


server_rt::~server_rt()
{
}



uint32_t server_rt::initialize (hosting::rx_platform_host* host, runtime_data_t& data)
{
	if (data.io_pool_size > 0)
	{
		_io_pool = server_dispatcher_object::smart_ptr(data.io_pool_size, IO_POOL_NAME, IO_POOL_ID, RX_DOMAIN_IO);
	}
	if (data.genereal_pool_size > 0)
	{
		_general_pool = server_dispatcher_object::smart_ptr(data.genereal_pool_size, GENERAL_POOL_NAME, GENERAL_POOL_ID, RX_DOMAIN_GENERAL);
	}
	if (data.workers_pool_size > 0)
	{
		_workers = domains_pool::smart_ptr(data.workers_pool_size);
		_workers->reserve();
	}
	_general_timer = rx_create_reference<rx::threads::timer>("Timer", 0);
	if (data.has_callculation_timer)
		_callculation_timer = rx_create_reference<rx::threads::timer>("Calc",0);


	return RX_OK;
}

uint32_t server_rt::deinitialize ()
{
	if (_io_pool)
		_io_pool = server_dispatcher_object::smart_ptr::null_ptr;
	if (_general_pool)
		_general_pool = server_dispatcher_object::smart_ptr::null_ptr;

	if(_workers)
		_workers->clear();

	if (_general_timer)
		_general_timer = rx::threads::timer::smart_ptr::null_ptr;
	if (_callculation_timer)
		_callculation_timer = rx::threads::timer::smart_ptr::null_ptr;

	return RX_OK;
}

void server_rt::append_timer_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now)
{
	rx::threads::job_thread* executer = get_executer(rx_thread_context());
	if (_general_timer)
		_general_timer->append_job(job,executer,period, now);
}

uint32_t server_rt::start (hosting::rx_platform_host* host, const runtime_data_t& data)
{
	if (_io_pool)
		_io_pool->get_pool()->run(RX_PRIORITY_ABOVE_NORMAL);
	if (_general_pool)
		_general_pool->get_pool()->run(RX_PRIORITY_NORMAL);
	if(_workers)
		_workers->run();
	if (_general_timer)
		_general_timer->start(RX_PRIORITY_HIGH);
	if (_callculation_timer)
		_callculation_timer->start(RX_PRIORITY_NORMAL);

	_dispatcher_timer = rx_create_reference<dispatcher_subscribers_job>();
	if (_callculation_timer)
		_callculation_timer->append_job(_dispatcher_timer, _general_pool->get_pool().unsafe_ptr(), data.io_timer_period);
	if (_general_timer)
		_general_timer->append_job(_dispatcher_timer, _general_pool->get_pool().unsafe_ptr(), data.io_timer_period);

	return RX_OK;
}

uint32_t server_rt::stop ()
{
	if (_dispatcher_timer)
	{
		_dispatcher_timer->cancel();
		_dispatcher_timer = dispatcher_subscribers_job::smart_ptr::null_ptr;
	}
	if (_io_pool)
		_io_pool->get_pool()->end();
	if (_general_pool)
		_general_pool->get_pool()->end();

	if(_workers)
		_workers->end();

	if (_general_timer)
	{
		_general_timer->stop();
		_general_timer->wait_handle();
	}
	if (_callculation_timer)
	{
		_callculation_timer->stop();
		_callculation_timer->wait_handle();
	}

	return RX_OK;
}

void server_rt::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	class_name = "_Runtime";
	has_own_code_info = true;
}

void server_rt::get_items (server_items_type& sub_items, const string_type& pattern) const
{
}

void server_rt::virtual_bind ()
{
}

void server_rt::virtual_release ()
{
}

void server_rt::get_value (values::rx_value& val, const rx_time& ts, const rx_mode_type& mode) const
{
}

namespace_item_attributes server_rt::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system | namespace_item_object);
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
		if(_general_pool)
			return _general_pool->get_pool().unsafe_ptr();
		else
			return _io_pool->get_pool().unsafe_ptr();
	case RX_DOMAIN_IO:
		return _io_pool->get_pool().unsafe_ptr();
	default:
		if(_workers)
			return _workers->get_executer(domain);
		else if(_general_pool)
			return _general_pool->get_pool().unsafe_ptr();
		else
			return _io_pool->get_pool().unsafe_ptr();
	}
}

void server_rt::append_calculation_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now)
{
	threads::job_thread* executer = get_executer(rx_thread_context());
	if(_callculation_timer)
		_general_timer->append_job(job, executer, period, now);
	if (_general_timer)
		_general_timer->append_job(job, executer, period, now);
}

void server_rt::append_io_job (rx::jobs::job_ptr job)
{
	_io_pool->get_pool()->append(job);
}

void server_rt::append_general_job (rx::jobs::job_ptr job)
{
	if (_general_pool)
		return _general_pool->get_pool()->append(job);
	else
		return _io_pool->get_pool()->append(job);
}

void server_rt::append_slow_job (rx::jobs::job_ptr job)
{
	if (_general_pool)
		return _general_pool->get_pool()->append(job);
	else
		return _io_pool->get_pool()->append(job);
}

void server_rt::append_timer_io_job (rx::jobs::timer_job_ptr job, uint32_t period, bool now)
{
	if (_general_timer)
		_general_timer->append_job(job, _io_pool->get_pool().unsafe_ptr(), period, now);
}


// Class rx_platform::runtime::server_dispatcher_object 

server_dispatcher_object::server_dispatcher_object (int count, const string_type& name, rx_thread_handle_t rx_thread_id, const rx_node_id& id)
  : _pool(count, name,rx_thread_id), server_object(name,id), _threads_count(count)
{
	register_const_value("count", _threads_count);
}


server_dispatcher_object::~server_dispatcher_object()
{
}



void server_dispatcher_object::get_items (server_items_type& sub_items, const string_type& pattern) const
{
	//sub_items.emplace_back(values::simple_const_value_ns_item<uint32_t>::smart_ptr(IO_POOL_NAME, ns::namespace_item_system_const_value, "-", get_created(), m_threads_count.get_value()));
}


// Class rx_platform::runtime::dispatcher_subscribers_job 

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


// Class rx_platform::runtime::domains_pool 

domains_pool::domains_pool (size_t pool_size)
      : _pool_size((uint32_t)pool_size)
	, server_object(WORKER_POOL_NAME, WORKER_POOL_ID)
{
	register_const_value("count", _pool_size);
}


domains_pool::~domains_pool()
{
}



void domains_pool::run (int priority)
{
	for (auto one : _workers)
		one->start(priority);
}

void domains_pool::end (uint32_t timeout)
{
	for (auto one : _workers)
		one->end();
}

void domains_pool::append (job_ptr pjob)
{
}

void domains_pool::reserve ()
{
	_workers.reserve(_pool_size.value());
	for (intptr_t i = 0; i < _pool_size.value(); i++)
		_workers.emplace_back(new threads::physical_job_thread("Worker",i));
}

void domains_pool::clear ()
{
	for (auto one : _workers)
		delete one;
	_workers.clear();
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
	uint32_t size = _pool_size.value();
	RX_ASSERT(size);
	if (size == 0)
		return nullptr;
	else
	{
		uint32_t real_index = domain%size;
		return _workers[real_index];
	}
}


} // namespace runtime
} // namespace rx_platform

