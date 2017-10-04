

/****************************************************************************
*
*  lib\rx_ptr.cpp
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


// rx_ptr
#include "lib/rx_ptr.h"



namespace rx {

namespace pointers {
_create_new_type _create_new;

// Parameterized Class rx::pointers::reference 


// Class rx::pointers::reference_object 

std::atomic<ref_counting_type> reference_object::g_objects_count;

reference_object::reference_object()
      : m_ref_count(1)
{
	g_objects_count++;
}

reference_object::reference_object(const reference_object &right)
      : m_ref_count(1)
{
	RX_ASSERT(false);
}


reference_object::~reference_object()
{
	g_objects_count--;
}


reference_object & reference_object::operator=(const reference_object &right)
{
	RX_ASSERT(false);
	return *this;
}



void reference_object::bind ()
{
	m_ref_count++;
}

void reference_object::release ()
{
	if (0 == --m_ref_count)
		delete this;
}

size_t reference_object::get_objects_count ()
{
	return (size_t)g_objects_count.load();
}


// Class rx::pointers::virtual_reference_object 

virtual_reference_object::virtual_reference_object()
{
}

virtual_reference_object::virtual_reference_object(const virtual_reference_object &right)
{
	RX_ASSERT(false);
}


virtual_reference_object::~virtual_reference_object()
{
}


virtual_reference_object & virtual_reference_object::operator=(const virtual_reference_object &right)
{
	RX_ASSERT(false);
	return *this;
}



// Parameterized Class rx::pointers::virtual_reference 


// Class rx::pointers::slim_reference 

slim_reference::slim_reference()
      : m_ref_count(1)
{
}


slim_reference::~slim_reference()
{
}



void slim_reference::bind ()
{
	m_ref_count++;
}

void slim_reference::release ()
{
	if (0 == --m_ref_count)
		delete this;
}


// Parameterized Class rx::pointers::interface_reference 


template <class ptrT>
void interface_reference<ptrT>::pera ()
{
}


// Class rx::pointers::interface_object 

interface_object::interface_object()
{
}

interface_object::interface_object(const interface_object &right)
{
	RX_ASSERT(false);
}


interface_object::~interface_object()
{
}


interface_object & interface_object::operator=(const interface_object &right)
{
	RX_ASSERT(false);
	return *this;
}



} // namespace pointers
} // namespace rx

