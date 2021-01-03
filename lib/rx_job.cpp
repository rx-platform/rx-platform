

/****************************************************************************
*
*  lib\rx_job.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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
// rx_job
#include "lib/rx_job.h"

#include "security/rx_security.h"


namespace rx {

rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();

namespace jobs {

// Class rx::jobs::job 

job::job()
      : canceled_(false)
	, security_context_(rx_security_context())
	, criticalness_(rx_criticalness::medium)
	, value_factor_(rx_default_value_factor)
	, complexity_(rx_default_complextiy)
	, access_type_(rx_access::full)
{
}



void job::cancel ()
{
	canceled_ = true;
}

bool job::is_canceled () const
{
	return canceled_;
}

void job::un_cancel ()
{
	canceled_ = false;
}

job* job::get_unsafe_ptr ()
{
	bind();
	return this;
}

void job::release_unsafe_ptr ()
{
	release();
}

void job::process_wrapper ()
{
    security::secured_scope _(security_context_);
    process();
}


// Class rx::jobs::timer_job 

timer_job::timer_job()
      : next_(0x0),
        period_(0x0),
        suspended_(true)
  , executer_(nullptr), my_timer_(nullptr)
{
}



void timer_job::set_executer (threads::job_thread* executer)
{
	locks::auto_lock dummy(&lock_);
	executer_ = executer;
}

void timer_job::lock ()
{
	lock_.lock();
}

void timer_job::unlock ()
{
	lock_.unlock();
}

rx_timer_ticks_t timer_job::get_random_time_offset ()
{
	if (my_timer_)
		return my_timer_->get_random_time_offset(*this);
	else
		return 0;
}

void timer_job::wake_timer ()
{
	if (my_timer_)
		my_timer_->wake_up();
}


// Class rx::jobs::post_period_job 

post_period_job::post_period_job()
{
}



rx_timer_ticks_t post_period_job::tick (rx_timer_ticks_t current_tick, rx_timer_ticks_t random_offset, bool& remove)
{
	if (((next_ - current_tick) & 0x80000000) || (next_ - current_tick) == 0)
	{

		// should be done
		executer_->append(smart_this());// add job to right thread

		remove = true;// we're done

		return 0;// return for how long
	}
	else
		return std::min(max_sleep_period, next_ - current_tick);// not jet so send how much more to timer
}

void post_period_job::start (uint32_t period)
{
	lock();
	{
		if (suspended_)
			suspended_ = false;
		period_ = ((rx_timer_ticks_t)period) * 1000;
		next_ = rx_get_us_ticks() + period_ + get_random_time_offset();
		wake_timer();
	}
	unlock();
}


// Class rx::jobs::periodic_job 

periodic_job::periodic_job()
{
}



rx_timer_ticks_t periodic_job::tick (rx_timer_ticks_t current_tick, rx_timer_ticks_t random_offset, bool& remove)
{
	if (suspended_)
		return 0;

	if (current_tick >= next_)
	{
		// should be done
		executer_->append(smart_this());// add job to right thread

		next_ += random_offset;
		do
		{
			next_ = next_ + period_;// new time

		} while (next_ < current_tick);

		return next_ - current_tick;// return for how long
	}
	else
		return next_ - current_tick;// not jet so send how match more to timer
}

void periodic_job::start (uint32_t period, bool now)
{
	lock();
	{
		if (suspended_)
			suspended_ = false;
		period_ = ((rx_timer_ticks_t)period) * 1000;
		next_ = (now ? rx_get_us_ticks() : rx_get_us_ticks() + period_ + get_random_time_offset());
		wake_timer();
	}
	unlock();
}

void periodic_job::suspend ()
{
	suspended_ = true;
}


} // namespace jobs
} // namespace rx

