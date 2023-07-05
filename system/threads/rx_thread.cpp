

/****************************************************************************
*
*  system\threads\rx_thread.cpp
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


// rx_job
#include "system/threads/rx_job.h"
// rx_thread
#include "system/threads/rx_thread.h"

#include "security/rx_security.h"
using rx_platform::jobs::job;

#include "system/server/rx_server.h"

class rx_thread_data_object
{
	typedef typename std::unique_ptr<std::stack<intptr_t, std::vector<intptr_t> > > stack_ptr_t;
private:
	std::map<int, stack_ptr_t> m_objects;
	rx_thread_data_object();
public:
	~rx_thread_data_object();

	static rx_thread_data_object& instance();
	static void clear_extern_threads_data();

	bool push_object(int handle, intptr_t obj);
	bool pop_object(int handle);
	intptr_t get_object(int handle);
};

namespace
{
locks::slim_lock  g_allocated_thread_objects_locks;
std::set< rx_thread_data_object*> g_allocated_thread_objects;
}


rx_thread_data_object::rx_thread_data_object()
{
	locks::auto_lock_t _(&g_allocated_thread_objects_locks);
	g_allocated_thread_objects.emplace(this);
}

rx_thread_data_object::~rx_thread_data_object()
{
	locks::auto_lock_t _(&g_allocated_thread_objects_locks);
	g_allocated_thread_objects.erase(this);
}
rx_thread_data_object& rx_thread_data_object::instance()
{
	rx_thread_data_object* ptr = (rx_thread_data_object*)rx_get_thread_data(rx_tls);
	if (ptr == nullptr)
	{
		ptr = new rx_thread_data_object();
		rx_set_thread_data(rx_tls, ptr);
	}
	return *ptr;
}
void rx_thread_data_object::clear_extern_threads_data()
{
	locks::auto_lock_t _(&g_allocated_thread_objects_locks);
	auto it = g_allocated_thread_objects.begin();
	while (it != g_allocated_thread_objects.end())
	{
		delete *it;
		it = g_allocated_thread_objects.begin();
	}
}

bool rx_thread_data_object::push_object(int handle, intptr_t obj)
{
	auto it = m_objects.find(handle);
	if (it == m_objects.end())
	{
		stack_ptr_t temp = std::make_unique<std::stack<intptr_t, std::vector<intptr_t> > >();
		temp->push(obj);
		m_objects.emplace(handle, std::forward<stack_ptr_t>(temp));
		return true;
	}
	else
	{
		it->second->push(obj);
		return true;
	}
}
bool rx_thread_data_object::pop_object(int handle)
{
	auto it = m_objects.find(handle);
	if (it != m_objects.end())
	{
		RX_ASSERT(!it->second->empty());
		if (!it->second->empty())
			it->second->pop();
		return true;
	}
	return false;
}
intptr_t rx_thread_data_object::get_object(int handle)
{
	auto it = m_objects.find(handle);
	if (it == m_objects.end() || it->second->empty())
		return 0;
	else
		return it->second->top();
}



#define SECURITY_TLS_DATA 999


rx_security_handle_t rx_security_context()
{
	return rx_thread_data_object::instance().get_object(SECURITY_TLS_DATA);
}
bool rx_push_security_context(rx_security_handle_t obj)
{
	return rx_thread_data_object::instance().push_object(SECURITY_TLS_DATA, obj);
}
bool rx_pop_security_context()
{
	return rx_thread_data_object::instance().pop_object(SECURITY_TLS_DATA);
}


#define THREADING_TLS_DATA 998


rx_thread_handle_t rx_thread_context()
{
	return rx_thread_data_object::instance().get_object(THREADING_TLS_DATA);
}
bool rx_push_thread_context(rx_thread_handle_t obj)
{
	return rx_thread_data_object::instance().push_object(THREADING_TLS_DATA, obj);
}
bool rx_pop_thread_context()
{
	return rx_thread_data_object::instance().pop_object(THREADING_TLS_DATA);
}

namespace rx_platform
{

class rx_thread_data_object
{
	typedef typename std::unique_ptr<std::stack<intptr_t, std::vector<intptr_t> > > stack_ptr_t;
private:
	std::map<int, stack_ptr_t> m_objects;
	rx_thread_data_object();
public:
	~rx_thread_data_object();

	static rx_thread_data_object& instance();
	static void clear_extern_threads_data();

	bool push_object(int handle, intptr_t obj);
	bool pop_object(int handle);
	intptr_t get_object(int handle);
};

namespace
{
locks::slim_lock  g_allocated_thread_objects_locks;
std::set< rx_thread_data_object*> g_allocated_thread_objects;
}


rx_thread_data_object::rx_thread_data_object()
{
	locks::auto_lock_t _(&g_allocated_thread_objects_locks);
	g_allocated_thread_objects.emplace(this);
}

rx_thread_data_object::~rx_thread_data_object()
{
	locks::auto_lock_t _(&g_allocated_thread_objects_locks);
	g_allocated_thread_objects.erase(this);
}
rx_thread_data_object& rx_thread_data_object::instance()
{
	rx_thread_data_object* ptr = (rx_thread_data_object*)rx_get_thread_data(rx_tls);
	if (ptr == nullptr)
	{
		ptr = new rx_thread_data_object();
		rx_set_thread_data(rx_tls, ptr);
	}
	return *ptr;
}
void rx_thread_data_object::clear_extern_threads_data()
{
	locks::auto_lock_t _(&g_allocated_thread_objects_locks);
	auto it = g_allocated_thread_objects.begin();
	while (it != g_allocated_thread_objects.end())
	{
		delete* it;
		it = g_allocated_thread_objects.begin();
	}
}

bool rx_thread_data_object::push_object(int handle, intptr_t obj)
{
	auto it = m_objects.find(handle);
	if (it == m_objects.end())
	{
		stack_ptr_t temp = std::make_unique<std::stack<intptr_t, std::vector<intptr_t> > >();
		temp->push(obj);
		m_objects.emplace(handle, std::forward<stack_ptr_t>(temp));
		return true;
	}
	else
	{
		it->second->push(obj);
		return true;
	}
}
bool rx_thread_data_object::pop_object(int handle)
{
	auto it = m_objects.find(handle);
	if (it != m_objects.end())
	{
		RX_ASSERT(!it->second->empty());
		if (!it->second->empty())
			it->second->pop();
		return true;
	}
	return false;
}
intptr_t rx_thread_data_object::get_object(int handle)
{
	auto it = m_objects.find(handle);
	if (it == m_objects.end() || it->second->empty())
		return 0;
	else
		return it->second->top();
}



#define SECURITY_TLS_DATA 999


rx_security_handle_t rx_security_context()
{
	return rx_thread_data_object::instance().get_object(SECURITY_TLS_DATA);
}
bool rx_push_security_context(rx_security_handle_t obj)
{
	return rx_thread_data_object::instance().push_object(SECURITY_TLS_DATA, obj);
}
bool rx_pop_security_context()
{
	return rx_thread_data_object::instance().pop_object(SECURITY_TLS_DATA);
}


#define THREADING_TLS_DATA 998


rx_thread_handle_t rx_thread_context()
{
	return rx_thread_data_object::instance().get_object(THREADING_TLS_DATA);
}
bool rx_push_thread_context(rx_thread_handle_t obj)
{
	return rx_thread_data_object::instance().push_object(THREADING_TLS_DATA, obj);
}
bool rx_pop_thread_context()
{
	return rx_thread_data_object::instance().pop_object(THREADING_TLS_DATA);
}

}


namespace rx_platform {

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
		pjob->process_wrapper();
	}
	pjob->release_unsafe_ptr();
}

// Class rx_platform::threads::thread 

thread::thread (const string_type& name, rx_thread_handle_t rx_thread_id)
      : thread_id_(0),
        rx_thread_id_(rx_thread_id)
{
	name_ = name;
}


thread::~thread()
{
	if (handle_)
		rx_thread_close(handle_);
}



void thread::_inner_handler (void* arg)
{
	thread *p = static_cast<thread *>(arg);

	rx_push_thread_context(p->rx_thread_id_);
	{
		security::secured_scope _(p->security_context_);

		p->handler();
	}
	rx_thread_data_object* ptr = (rx_thread_data_object*)rx_get_thread_data(rx_tls);
	if (ptr)
		delete ptr;
}

void thread::start (int priority)
{
	if (handle_)
		rx_thread_close(handle_);


	security_context_ = rx_security_context();

	handle_ = rx_thread_create(_inner_handler, this, priority, &thread_id_, name_.c_str());

	if (handle_ == 0)
	{// error occurred
		char buff[0x100];
		snprintf(buff, sizeof(buff) / sizeof(buff[0]), "Error while creating %s thread. Error code: %x", name_.c_str(), errno);
	}

}

void thread::deinitialize ()
{
	rx_thread_data_object::clear_extern_threads_data();
}


// Class rx_platform::threads::job_thread 

job_thread::job_thread()
{
}


job_thread::~job_thread()
{
}



// Class rx_platform::threads::physical_job_thread 

physical_job_thread::physical_job_thread (const string_type& name, rx_thread_handle_t rx_thread_id, uint64_t cpu_mask)
      : has_job_(false),
        cpu_mask_(cpu_mask)
	, thread(name,rx_thread_id)
{
}


physical_job_thread::~physical_job_thread()
{
}



uint32_t physical_job_thread::handler ()
{
	if (cpu_mask_)
	{
		auto ret = rx_thread_set_afinity(rx_current_thread(), cpu_mask_);
	}
	std::vector<job_ptr> queued;
	bool exit = false;

	while (!exit)
	{
		queued.clear();

		if (wait(queued))
		{
			for (auto& obj : queued)
			{
				if (!obj)
				{
					exit = true;
					break;
				}

				if (!obj->is_canceled())
				{
					current_ = obj;
					obj->process_wrapper();
				}
			}
		}
	}
	return 0;
}

void physical_job_thread::run (int priority)
{
	start(priority);
}

void physical_job_thread::end (uint32_t timeout)
{
	stop(timeout);
}

void physical_job_thread::append (job_ptr pjob)
{
	bool was_empty = false;
	{
		locks::auto_lock_t dummy(&lock_);
		was_empty = queue_.empty();
		queue_.push(pjob);
	}

	if (was_empty)
		has_job_.set();
}

bool physical_job_thread::wait (std::vector<job_ptr>& queued, uint32_t timeout)
{
	if (RX_WAIT_0 != has_job_.wait_handle(timeout))
		return false;

	locks::auto_lock_t dummy(&lock_);

	RX_ASSERT(!queue_.empty());
	while (!queue_.empty())
	{
		queued.emplace_back(std::move(queue_.front()));
		queue_.pop();
	}

	return true;

}

void physical_job_thread::stop (uint32_t timeout)
{
	append(job_ptr::null_ptr);
	if (current_)
		current_->cancel();
	wait_handle(timeout);
	locks::auto_lock_t dummy(&lock_);
	while (!queue_.empty())
	{
		if (queue_.front())
		{
			queue_.front()->cancel();
		}
		queue_.pop();
	}
}


// Class rx_platform::threads::dispatcher_pool 

dispatcher_pool::dispatcher_pool (int count, const string_type& name, rx_thread_handle_t rx_thread_id, uint64_t cpu_mask)
      : name_(name),
        cpu_mask_(cpu_mask)
{
	dispatcher_ = rx_create_kernel_dispathcer(count);
	for (int i = 0; i < count; i++)
		threads_.emplace_back(std::make_unique<dispatcher_thread>(name,rx_thread_id,dispatcher_));
}


dispatcher_pool::~dispatcher_pool()
{
	rx_destroy_kernel_dispatcher(dispatcher_);
}



void dispatcher_pool::run (int priority)
{
	for (auto& one : threads_)
		one->start(priority);
}

void dispatcher_pool::end (uint32_t timeout)
{
	size_t count = threads_.size();
	for (size_t i = 0; i < count; i++)
		rx_dispatcher_signal_end(dispatcher_);
	for (size_t i = 0; i < count; i++)
		threads_[i]->wait_handle(timeout);
}

void dispatcher_pool::append (job_ptr pjob)
{
	job* job = pjob->get_unsafe_ptr();
	rx_dispatch_function(dispatcher_,execute_job_c, job);
}

int dispatcher_pool::get_CPU (rx_thread_handle_t domain) const
{
	return (int)this->threads_.size();
}


// Class rx_platform::threads::dispatcher_thread 

dispatcher_thread::dispatcher_thread (const string_type& name, rx_thread_handle_t rx_thread_id, rx_kernel_dispather_t dispatcher)
  : thread(name,rx_thread_id)
	, dispatcher_(dispatcher)
{
}


dispatcher_thread::~dispatcher_thread()
{
}



uint32_t dispatcher_thread::handler ()
{
	while (true)
	{
		if (!rx_dispatch_events(dispatcher_))
			break;
	}
	return 0;
}


// Class rx_platform::threads::timer 

timer::timer (const string_type& name, rx_thread_handle_t rx_thread_id)
      : wake_up_(false),
        should_exit_(false),
        soft_random_index_(0),
        medium_random_index_(0),
        real_time_(false)
	, thread(name, rx_thread_id)
{
	init_random_sequences();
}


timer::~timer()
{
}



uint32_t timer::handler ()
{

	for (;;)
	{
		lock_.lock();

		rx_timer_ticks_t min = 0ull - 1ull;

		auto it = jobs_.begin();
		rx_timer_ticks_t tick = rx_get_us_ticks();
		while (it != jobs_.end())
		{
			bool remove = false;
			timer_job_ptr one = *it;
			if (!one->is_canceled())
			{
				rx_timer_ticks_t temp = one->internal_tick(tick, get_random_time_offset(*one), remove);
				if (!remove)
				{
					if(temp && temp < min)
						min = temp;
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
				one->my_timer_ = nullptr;
				one->unlock();
				it = jobs_.erase(it);
			}
		}


		lock_.unlock();

		wake_up_.wait_handle_us(min);

		if (should_exit_)
			break;

	}

	return 0;
}

void timer::stop ()
{
	should_exit_ = true;
	wake_up();
}

void timer::wake_up ()
{
	wake_up_.set();
}

void timer::append_job (timer_job_ptr job, job_thread* executer)
{

	switch (job->get_criticalness())
	{
	case rx_criticalness::medium:
		if (real_time_)
			job->period_error_ = 0;
		else
			job->period_error_ = rx_medium_time_offset;
		break;
	case rx_criticalness::soft:
		job->period_error_ = rx_soft_time_offset;
		break;
	case rx_criticalness::hard:
        job->period_error_ = 0;
	}

	job->lock();
	job->my_timer_ = this;
	job->executer_ = executer;
	job->unlock();


	lock_.lock();
	jobs_.emplace(job);
	lock_.unlock();

	wake_up();
}

void timer::init_random_sequences ()
{
	srand((unsigned int)rx_get_tick_count());
	for (auto i = 0; i < RX_OFFSET_TIMES_SIZE; i++)
	{
		medium_randoms_[i] = (rx_timer_ticks_t)rx_border_rand(0, rx_medium_time_offset);
		soft_randoms_[i] = (rx_timer_ticks_t)rx_border_rand(0, rx_soft_time_offset);
	}
	real_time_ = rx_gate::instance().get_configuration().processor.real_time;
}

rx_timer_ticks_t timer::get_random_time_offset (job& whose)
{
	switch (whose.get_criticalness())
	{
	case rx_criticalness::hard:
		return 0;
	case rx_criticalness::medium:
		if (real_time_)
		{
			return 0;
		}
		else
		{
			medium_random_index_++;
			if (medium_random_index_ >= RX_OFFSET_TIMES_SIZE)
				medium_random_index_ = 0;
			return medium_randoms_[medium_random_index_];
		}
	case rx_criticalness::soft:
		soft_random_index_++;
		if (soft_random_index_ >= RX_OFFSET_TIMES_SIZE)
			soft_random_index_ = 0;
		return soft_randoms_[soft_random_index_];
	default:
		RX_ASSERT(false);
		return 0;
	}
}


} // namespace threads
} // namespace rx_platform

