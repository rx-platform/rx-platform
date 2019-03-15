

/****************************************************************************
*
*  lib\rx_thread.cpp
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


// rx_job
#include "lib/rx_job.h"
// rx_thread
#include "lib/rx_thread.h"

#include "lib/security/rx_security.h"


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

	p->handler();

}

void thread::start (int priority)
{
	if (handle_)
		rx_thread_close(handle_);

	handle_ = rx_thread_create(_inner_handler, this, priority, &thread_id_);

	if (handle_ == 0)
	{// error occurred
		char buff[0x100];
		snprintf(buff, sizeof(buff) / sizeof(buff[0]), "Error while creating %s thread. Error code: %x", name_.c_str(), errno);
	}

}


// Class rx::threads::job_thread 

job_thread::job_thread()
{
}


job_thread::~job_thread()
{
}



// Class rx::threads::physical_job_thread 

physical_job_thread::physical_job_thread (const string_type& name, rx_thread_handle_t rx_thread_id)
      : has_job_(false)
	, thread(name,rx_thread_id)
{
}


physical_job_thread::~physical_job_thread()
{
}



uint32_t physical_job_thread::handler ()
{
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
					rx_security_handle_t sec = obj->get_security_context();
					if (sec)
						rx_push_security_context(sec);
					current_ = obj;
					obj->process();
					current_ = job_ptr::null_ptr;
					if (sec)
						rx_pop_security_context();
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
	locks::auto_lock dummy(&lock_);

	bool was_empty = queue_.empty();

	queue_.push(pjob);

	if (was_empty)
		has_job_.set();
}

bool physical_job_thread::wait (std::vector<job_ptr>& queued, uint32_t timeout)
{
	if (RX_WAIT_0 != has_job_.wait_handle(timeout))
		return false;

	locks::auto_lock dummy(&lock_);

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
	locks::auto_lock dummy(&lock_);
	while (!queue_.empty())
	{
		if (queue_.front())
		{
			queue_.front()->cancel();
		}
		queue_.pop();
	}
}


// Class rx::threads::dispatcher_pool 

dispatcher_pool::dispatcher_pool (int count, const string_type& name, rx_thread_handle_t rx_thread_id)
      : name_(name)
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


// Class rx::threads::dispatcher_thread 

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


// Class rx::threads::timer 

timer::timer (const string_type& name, rx_thread_handle_t rx_thread_id)
      : wake_up_(false),
        should_exit_(false)
	, thread(name, rx_thread_id)
{
}


timer::~timer()
{
}



uint32_t timer::handler ()
{

	for (;;)
	{
		lock_.lock();

		uint64_t min = 0ull - 1ull;

		auto it = jobs_.begin();
		while (it != jobs_.end())
		{
			bool remove = false;
			timer_job_ptr one = *it;
			rx_timer_ticks_t tick = rx_get_us_ticks();
			if (!one->is_canceled())
			{
				rx_timer_ticks_t temp = one->tick(tick,remove);
				if (!remove)
				{
					if (temp < min)
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

void timer::append_job (timer_job_ptr job, job_thread* executer, uint32_t period, bool now)
{

	job->lock();
	job->my_timer_ = this;
	job->executer_ = executer;
	job->period_ = period*1000;
	job->next_ = (now ? rx_get_us_ticks() : rx_get_us_ticks() + job->period_);
	job->unlock();


	lock_.lock();

	jobs_.emplace(job);
	wake_up();

	lock_.unlock();
}


} // namespace threads
} // namespace rx

