

/****************************************************************************
*
*  lib\rx_lock.cpp
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


// rx_lock
#include "lib/rx_lock.h"



namespace rx {

namespace locks {

// Class rx::locks::slim_lock 

slim_lock::slim_lock()
{
	rx_slim_lock_create(&_slim_lock);
}


slim_lock::~slim_lock()
{
	rx_slim_lock_destroy(&_slim_lock);
}



void slim_lock::lock ()
{
	rx_slim_lock_aquire(&_slim_lock);
}

void slim_lock::unlock ()
{
	rx_slim_lock_release(&_slim_lock);
}


// Class rx::locks::lockable 

lockable::lockable()
{
	rx_slim_lock_create(&_slim_lock);
}


lockable::~lockable()
{
	rx_slim_lock_destroy(&_slim_lock);
}



void lockable::lock ()
{
	rx_slim_lock_aquire(&_slim_lock);
}

void lockable::unlock ()
{
	rx_slim_lock_release(&_slim_lock);
}


// Class rx::locks::waitable 

waitable::waitable()
      : _handle(0)
{
}

waitable::waitable(const waitable &right)
      : _handle(0)
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
	return rx_handle_wait(_handle, timeout);
}


// Class rx::locks::event 

event::event (bool initial)
{
	_handle = rx_event_create(initial);
}


event::~event()
{
	if (_handle)
		rx_event_destroy(_handle);
}



void event::set ()
{
	rx_event_set(_handle);
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

