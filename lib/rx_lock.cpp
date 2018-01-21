

/****************************************************************************
*
*  lib\rx_lock.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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


// rx_lock
#include "lib/rx_lock.h"



namespace rx {

namespace locks {

// Class rx::locks::slim_lock 

slim_lock::slim_lock()
{
	rx_slim_lock_create(&slim_lock_);
}


slim_lock::~slim_lock()
{
	rx_slim_lock_destroy(&slim_lock_);
}



void slim_lock::lock ()
{
	rx_slim_lock_aquire(&slim_lock_);
}

void slim_lock::unlock ()
{
	rx_slim_lock_release(&slim_lock_);
}


// Class rx::locks::lockable 

lockable::lockable()
{
	rx_slim_lock_create(&slim_lock_);
}


lockable::~lockable()
{
	rx_slim_lock_destroy(&slim_lock_);
}



void lockable::lock ()
{
	rx_slim_lock_aquire(&slim_lock_);
}

void lockable::unlock ()
{
	rx_slim_lock_release(&slim_lock_);
}


// Class rx::locks::waitable 

waitable::waitable()
      : handle_(0)
{
}

waitable::waitable(const waitable &right)
      : handle_(0)
{
	RX_ASSERT(false);
}


waitable::~waitable()
{
}


waitable & waitable::operator=(const waitable &right)
{
	RX_ASSERT(false);
	return *this;
}



uint32_t waitable::wait_handle (uint32_t timeout)
{
	return rx_handle_wait(handle_, timeout);
}

uint32_t waitable::wait_handle_us (uint64_t timeout)
{
	return rx_handle_wait_us(handle_, timeout);
}


// Class rx::locks::event 

event::event (bool initial)
{
	handle_ = rx_event_create(initial);
}


event::~event()
{
	if (handle_)
		rx_event_destroy(handle_);
}



void event::set ()
{
	rx_event_set(handle_);
}


// Class rx::locks::empty_slim_lock 

empty_slim_lock::empty_slim_lock()
{
}


empty_slim_lock::~empty_slim_lock()
{
}



void empty_slim_lock::lock ()
{
}

void empty_slim_lock::unlock ()
{
}


} // namespace locks
} // namespace rx

