

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


namespace server {

namespace runtime {

// Class server::runtime::server_rt 

server_rt::server_rt()
	: server_object(RX_NS_SERVER_RT_NAME, RX_NS_SERVER_RT_ID)
{
}


server_rt::~server_rt()
{
}



dword server_rt::initialize (rx_server_host* host, runtime_data_t& data)
{
	if (data.io_pool_size > 0)
	{
		m_io_pool = server_dispatcher_object::smart_ptr(data.io_pool_size, IO_POOL_NAME, IO_POOL_ID);
	}
	if (data.genereal_pool_size > 0)
	{
		m_general_pool = server_dispatcher_object::smart_ptr(data.genereal_pool_size, GENERAL_POOL_NAME, GENERAL_POOL_ID);
	}
	if (data.workers_pool_size > 0)
	{
		m_workers = domains_pool::smart_ptr(data.workers_pool_size);
		m_workers->reserve();
	}
	m_general_timer = rx::threads::timer::smart_ptr(rx::pointers::_create_new);
	if (data.has_callculation_timer)
		m_callculation_timer = rx::threads::timer::smart_ptr(rx::pointers::_create_new);


	return RX_OK;
}

dword server_rt::deinitialize ()
{
	if (m_io_pool)
		m_io_pool = server_dispatcher_object::smart_ptr::null_ptr;
	if (m_general_pool)
		m_general_pool = server_dispatcher_object::smart_ptr::null_ptr;

	if(m_workers)
		m_workers->clear();

	if (m_general_timer)
		m_general_timer = rx::threads::timer::smart_ptr::null_ptr;
	if (m_callculation_timer)
		m_callculation_timer = rx::threads::timer::smart_ptr::null_ptr;

	return RX_OK;
}

void server_rt::append_timer_job (rx::jobs::timer_job_ptr job, dword domain, dword period, bool now)
{
	rx::threads::job_thread* executer = get_executer(domain);
	if (m_general_timer)
		m_general_timer->append_job(job,executer,period, now);
}

dword server_rt::start (rx_server_host* host, const runtime_data_t& data)
{
	if (m_io_pool)
		m_io_pool->get_pool()->run(RX_PRIORITY_ABOVE_NORMAL);
	if (m_general_pool)
		m_general_pool->get_pool()->run(RX_PRIORITY_NORMAL);
	if(m_workers)
		m_workers->run();
	if (m_general_timer)
		m_general_timer->start(RX_PRIORITY_HIGH);
	if (m_callculation_timer)
		m_callculation_timer->start(RX_PRIORITY_NORMAL);

	m_dispatcher_timer = dispatcher_subscribers_job::smart_ptr(pointers::_create_new);
	if (m_callculation_timer)
		m_callculation_timer->append_job(m_dispatcher_timer, &*m_general_pool->get_pool(), data.io_timer_period);
	if (m_general_timer)
		m_general_timer->append_job(m_dispatcher_timer, &*m_general_pool->get_pool(), data.io_timer_period);

	return RX_OK;
}

dword server_rt::stop ()
{
	if (m_dispatcher_timer)
	{
		m_dispatcher_timer->cancel();
		m_dispatcher_timer = dispatcher_subscribers_job::smart_ptr::null_ptr;
	}
	if (m_io_pool)
		m_io_pool->get_pool()->end();
	if (m_general_pool)
		m_general_pool->get_pool()->end();

	if(m_workers)
		m_workers->end();

	if (m_general_timer)
	{
		m_general_timer->stop();
		m_general_timer->wait_handle();
	}
	if (m_callculation_timer)
	{
		m_callculation_timer->stop();
		m_callculation_timer->wait_handle();
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

void server_rt::append_job (rx::jobs::timer_job_ptr job, dword domain)
{
	threads::job_thread* executer = get_executer(domain);
	RX_ASSERT(executer);
	if (executer)
		executer->append(job);
}

rx::threads::job_thread* server_rt::get_executer (dword domain)
{
	switch (domain)
	{
	case RX_DOMAIN_GENERAL:
		if(m_general_pool)
			return m_general_pool->get_pool().unsafe_ptr();
		else
			return m_io_pool->get_pool().unsafe_ptr();
	case RX_DOMAIN_IO:
		return m_io_pool->get_pool().unsafe_ptr();
	default:
		if(m_workers)
			return m_workers->get_executer(domain);
		else if(m_general_pool)
			return m_general_pool->get_pool().unsafe_ptr();
		else
			return m_io_pool->get_pool().unsafe_ptr();
	}
}


// Class server::runtime::server_dispatcher_object 

server_dispatcher_object::server_dispatcher_object (int count, const string_type& name, const rx_node_id& id)
  : m_pool(count, name), server_object(name,id), m_threads_count(count)
{
	register_const_value("count", m_threads_count);
}


server_dispatcher_object::~server_dispatcher_object()
{
}



void server_dispatcher_object::get_items (server_items_type& sub_items, const string_type& pattern) const
{
	//sub_items.emplace_back(values::simple_const_value_ns_item<dword>::smart_ptr(IO_POOL_NAME, ns::namespace_item_system_const_value, "-", get_created(), m_threads_count.get_value()));
}


// Class server::runtime::dispatcher_subscribers_job 

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


// Class server::runtime::domains_pool 

domains_pool::domains_pool (size_t pool_size)
      : m_pool_size((dword)pool_size)
	, server_object(WORKER_POOL_NAME, WORKER_POOL_ID)
{
	register_const_value("count", m_pool_size);
}


domains_pool::~domains_pool()
{
}



void domains_pool::run (int priority)
{
	for (auto one : m_workers)
		one->start(priority);
}

void domains_pool::end (dword timeout)
{
	for (auto one : m_workers)
		one->end();
}

void domains_pool::append (job_ptr pjob)
{
}

void domains_pool::reserve ()
{
	m_workers.reserve(m_pool_size.value());
	for (dword i = 0; i < m_pool_size.value(); i++)
		m_workers.emplace_back(new threads::physical_job_thread("Worker"));
}

void domains_pool::clear ()
{
	for (auto one : m_workers)
		delete one;
	m_workers.clear();
}

void domains_pool::append (rx::jobs::timer_job_ptr job, dword domain)
{
	job_thread* thr = get_executer(domain);
	RX_ASSERT(thr);
	if (thr)
		thr->append(job);
}

rx::threads::job_thread* domains_pool::get_executer (dword domain)
{
	dword size = m_pool_size.value();
	RX_ASSERT(size);
	if (size == 0)
		return nullptr;
	else
	{
		dword real_index = domain%size;
		return m_workers[real_index];
	}
}


} // namespace runtime
} // namespace server

