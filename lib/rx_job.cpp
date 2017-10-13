

/****************************************************************************
*
*  lib\rx_job.cpp
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



namespace rx {

rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();

namespace jobs {

// Class rx::jobs::job 

job::job()
      : _canceled(false)
{
	_security_context = rx_security_context();
}


job::~job()
{
}



void job::cancel ()
{
	_canceled = true;
}

bool job::is_canceled () const
{
	return _canceled;
}

void job::un_cancel ()
{
	_canceled = false;
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


// Class rx::jobs::timer_job 

timer_job::timer_job()
      : _next(0x0),
        _period(0x0)
  , _executer(nullptr), _my_timer(nullptr)
{
}


timer_job::~timer_job()
{
}



void timer_job::set_executer (threads::job_thread* executer)
{
	locks::auto_lock dummy(this);
	_executer = executer;
}


// Class rx::jobs::post_period_job 

post_period_job::post_period_job()
{
}


post_period_job::~post_period_job()
{
}



dword post_period_job::tick (dword current_tick, bool& remove)
{
	if (((_next - current_tick) & 0x80000000) || (_next - current_tick) == 0)
	{

		// should be done
		_executer->append(smart_this());// add job to right thread

		remove = true;// we're done

		return 0;// return for how long
	}
	else
		return _next - current_tick;// not jet so send how mutch more to timer
}


// Class rx::jobs::periodic_job 

periodic_job::periodic_job()
{
}


periodic_job::~periodic_job()
{
}



dword periodic_job::tick (dword current_tick, bool& remove)
{
	if (((_next - current_tick) & 0x80000000) || (_next - current_tick) == 0)
	{
		//if (m_next>count)
		//   return m_next-count;// not jet so send how mutch more to timer

		// should be done
		_executer->append(smart_this());// add job to right thread

		_next = current_tick + _period;// new time

		return _period;// return for how long
	}
	else
		return _next - current_tick;// not jet so send how mutch more to timer
}


} // namespace jobs
} // namespace rx

