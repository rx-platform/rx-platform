

/****************************************************************************
*
*  lib\rx_thread.cpp
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


// rx_job
#include "lib/rx_job.h"
// rx_thread
#include "lib/rx_thread.h"



namespace rx {
rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();

namespace threads {

extern "C" void execute_job_c(void* arg)
{
	execute_job(arg);
}
void execute_job(void* arg)
{
	job* pjob = (job*)arg;
	if (!pjob->is_canceled())
	{
		rx_security_handle_t sec = pjob->get_security_context();
		if (sec)
			rx_push_security_context(sec);
		pjob->process();
		if (sec)
			rx_pop_security_context();
	}
	pjob->release_unsafe_ptr();
}



// Class rx::threads::thread 

thread::thread()
      : m_thread_id(0)
{
	m_name = "<unnamed>";
}

thread::thread (const string_type& name)
      : m_thread_id(0)
{
	m_name = name;
}


thread::~thread()
{
	if (m_handle)
		rx_thread_close(m_handle);
}



void thread::_inner_handler (void* arg)
{
	thread *p = static_cast<thread *>(arg);

	p->handler();

}

void thread::start (int priority)
{
	if (m_handle)
		rx_thread_close(m_handle);

	m_handle = rx_thread_create(_inner_handler, this, priority, &m_thread_id);

	if (m_handle == 0)
	{// error occured
		char buff[0x100];
		snprintf(buff, sizeof(buff) / sizeof(buff[0]), "Error while creating %s thread. Error code: %x", m_name.c_str(), errno);
		throw svr_except(buff, errno);
	}

}


// Class rx::threads::job_thread 

job_thread::job_thread()
{
}


job_thread::~job_thread()
{
}



// Class rx::threads::job_aware_thread 

job_aware_thread::job_aware_thread()
{
}


job_aware_thread::~job_aware_thread()
{
}



// Class rx::threads::physical_job_thread 

physical_job_thread::physical_job_thread()
      : m_has_job(false)
{
}

physical_job_thread::physical_job_thread (const string_type& name)
      : m_has_job(false)
{
}


physical_job_thread::~physical_job_thread()
{
}



dword physical_job_thread::handler ()
{
	std::vector<job_ptr> queued;
	bool exit = false;

	while (!exit)
	{
		queued.clear();

		wait(queued);

		for (auto& obj : queued)
		{
			if (!obj)
			{
				exit = true;
				break;
			}

			if (!obj->is_canceled())
			{
				rx_security_handle_t sec = obj->get_security_context();
				if (sec)
					rx_push_security_context(sec);
				m_current = obj;
				obj->process();
				m_current = job_ptr::null_ptr;
				if (sec)
					rx_pop_security_context();
			}
		}
	}
	return 0;
}

void physical_job_thread::run (int priority)
{
	start(priority);
}

void physical_job_thread::end (dword timeout)
{
	stop(timeout);
}

void physical_job_thread::append (job_ptr pjob)
{
	locks::auto_lock dummy(this);

	m_queue.push(pjob);

	if (m_queue.size() == 1)
		m_has_job.set();
}

bool physical_job_thread::wait (std::vector<job_ptr>& queued, dword timeout)
{
	if (RX_WAIT_0 != m_has_job.wait_handle(timeout))
		return false;

	locks::auto_lock dummy(this);

	RX_ASSERT(!m_queue.empty());
	while (!m_queue.empty())
	{
		queued.emplace_back(m_queue.front());
		m_queue.pop();
	}

	return true;

}

void physical_job_thread::stop (dword timeout)
{
	append(job_ptr::null_ptr);
	if (m_current)
		m_current->cancel();
	wait_handle(timeout);
	locks::auto_lock dummy(this);
	while (!m_queue.empty())
	{
		if (m_queue.front())
		{
			m_queue.front()->cancel();
		}
		m_queue.pop();
	}
}

void physical_job_thread::virtual_bind ()
{
	bind();
}

void physical_job_thread::virtual_release ()
{
	release();
}


// Class rx::threads::dispatcher_pool 

dispatcher_pool::dispatcher_pool (int count, const string_type& name)
      : m_name(name)
{
	m_dispatcher = rx_create_kernel_dispathcer(count);
	for (int i = 0; i < count; i++)
		m_threads.emplace_back(new dispatcher_thread(name,m_dispatcher));
}


dispatcher_pool::~dispatcher_pool()
{
	rx_destroy_kernel_dispatcher(m_dispatcher);
	for (auto& one : m_threads)
		delete one;
}



void dispatcher_pool::run (int priority)
{
	for (auto& one : m_threads)
		one->start(priority);
}

void dispatcher_pool::end (dword timeout)
{
	size_t count = m_threads.size();
	for (size_t i = 0; i < count; i++)
		rx_dispatcher_signal_end(m_dispatcher);
	for (size_t i = 0; i < count; i++)
		m_threads[i]->wait_handle(timeout);
}

void dispatcher_pool::append (job_ptr pjob)
{
	job* job = pjob->get_unsafe_ptr();
	rx_dispatch_function(m_dispatcher,execute_job_c, job);
}

void dispatcher_pool::virtual_bind ()
{
	bind();
}

void dispatcher_pool::virtual_release ()
{
	release();
}


// Class rx::threads::dispatcher_thread 

dispatcher_thread::dispatcher_thread (const string_type& name, rx_kernel_dispather_t dispatcher)
  : thread(name), m_dispatcher(dispatcher)
{
}


dispatcher_thread::~dispatcher_thread()
{
}



dword dispatcher_thread::handler ()
{
	while (true)
	{
		if (!rx_dispatch_events(m_dispatcher))
			break;
	}
	return 0;
}


// Class rx::threads::timer 

timer::timer()
      : m_wake_up(false),
        m_should_exit(false)
{
}


timer::~timer()
{
}



dword timer::handler ()
{

	for (;;)
	{
		lock();

		dword max = 0 - 1;

		auto it = m_jobs.begin();
		while (it != m_jobs.end())
		{
			bool remove = false;
			timer_job_ptr one = *it;
			dword tick = rx_get_tick_count();
			if (!one->is_canceled())
			{
				dword temp = one->tick(tick,remove);
				if (!remove)
				{
					if (temp < max)
						max = temp;
					it++;
				}
			}
			else
			{
				remove = true;
			}
			if (remove)
			{
				one->lock();
				one->m_my_timer = nullptr;
				one->unlock();
				it = m_jobs.erase(it);
			}
		}


		unlock();

		m_wake_up.wait_handle(max);

		if (m_should_exit)
			break;

	}

	return 0;
}

void timer::stop ()
{
	m_should_exit = true;
	wake_up();
}

void timer::wake_up ()
{
	m_wake_up.set();
}

void timer::append_job (timer_job_ptr job, job_thread* executer, dword period, bool now)
{

	job->lock();
	job->m_my_timer = this;
	job->m_executer = executer;
	job->m_period = period;
	job->m_next = (now ? rx_get_tick_count() : rx_get_tick_count() + period);
	job->unlock();

	
	lock();

	m_jobs.emplace(job);
	wake_up();

	unlock();
}


} // namespace threads
} // namespace rx

