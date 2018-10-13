

/****************************************************************************
*
*  lib\rx_ptr.cpp
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


#include "pch.h"


// rx_ptr
#include "lib/rx_ptr.h"



namespace rx {

namespace pointers {
_create_new_type _create_new;

// Parameterized Class rx::pointers::reference 


// Class rx::pointers::reference_object 

std::atomic<ref_counting_type> reference_object::g_objects_count;

reference_object::reference_object()
      : ref_count_(1)
{
	g_objects_count++;
}


reference_object::~reference_object()
{
	g_objects_count--;
}



void reference_object::bind ()
{
	ref_count_.fetch_add(1, std::memory_order_relaxed);
}

void reference_object::release ()
{
	if (1 == ref_count_.fetch_sub(1, std::memory_order_acq_rel))
		delete this;
}

size_t reference_object::get_objects_count ()
{
	return (size_t)g_objects_count.load();
}


// Class rx::pointers::virtual_reference_object 

virtual_reference_object::~virtual_reference_object()
{
}



// Parameterized Class rx::pointers::virtual_reference 


// Class rx::pointers::struct_reference 


void struct_reference::bind ()
{
	ref_count_++;
}

void struct_reference::release ()
{
	if (0 == --ref_count_)
		delete this;
}


// Parameterized Class rx::pointers::interface_reference 


// Class rx::pointers::interface_object 

interface_object::~interface_object()
{
}



} // namespace pointers
} // namespace rx

