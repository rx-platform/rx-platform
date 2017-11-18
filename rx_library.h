

/****************************************************************************
*
*  rx_library.h
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


#ifndef rx_library_h
#define rx_library_h 1





// platform specific definitions
#include "os_itf/rx_ositf.h"

// C++ standard library and some mcro definitions
#include "lib/rx_std.h"

// smart pointers
#include "lib/rx_ptr.h"

// memory buffers
#include "lib/rx_mem.h"

// log related stuff
#include "lib/rx_log.h"




using namespace rx;



// for standard references
template<class T, typename... Args>
pointers::reference<T> rx_create_reference(Args... args)
{
	return pointers::reference<T>(args...);
}
template<class T>
pointers::reference<T> rx_create_reference()
{
	return pointers::reference<T>(pointers::_create_new);
}



#endif
