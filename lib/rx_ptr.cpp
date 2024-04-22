

/****************************************************************************
*
*  lib\rx_ptr.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


// rx_ptr
#include "lib/rx_ptr.h"


extern "C"
{
	void c_destroy_reference(rx::pointers::reference_object* self)
	{
		delete self;
	}

	lock_reference_def_struct _g_ref_def_struct
	{
		(reference_destroy_func_t)c_destroy_reference
	};
}


namespace rx {

namespace pointers {
_create_new_type _create_new;

// Parameterized Class rx::pointers::reference 


// Class rx::pointers::reference_object 

std::atomic<ref_counting_type> reference_object::g_objects_count;

reference_object::reference_object()
      : ref_count_(1),
        shared_ref_(false)
	, shared_data_(nullptr)
{
	g_objects_count++;
}

reference_object::reference_object (lock_reference_struct* shared_data)
      : ref_count_(1),
        shared_ref_(false)
	, shared_data_(shared_data)
{
	if (shared_data_)
	{
		rx_aquire_lock_reference(shared_data_);
	}
}


reference_object::~reference_object()
{
	if (shared_data_)
	{
		if (!shared_ref_)
			rx_release_lock_reference(shared_data_);
	}
	g_objects_count--;
}



void reference_object::bind_as_shared (lock_reference_struct* shared_data)
{
	RX_ASSERT(shared_data_ == nullptr);
	shared_data_ = shared_data;
	RX_ASSERT(shared_data_);
	if (shared_data_)
	{
		shared_ref_ = true;
		rx_init_lock_reference(shared_data_, this, &_g_ref_def_struct);
	}
}

lock_reference_struct* reference_object::get_shared_ref ()
{
	return shared_data_;
}

void reference_object::bind ()
{
	if (shared_ref_)
		rx_aquire_lock_reference(shared_data_);
	else
		ref_count_.fetch_add(1);
}

void reference_object::release ()
{
	if (shared_ref_)
	{
		rx_release_lock_reference(shared_data_);
	}
	else
	{
		if (1 == ref_count_.fetch_sub(1))
			delete this;
	}
}

size_t reference_object::get_objects_count ()
{
	return (size_t)g_objects_count.load();
}

void reference_object::fill_code_info (std::ostream& info, const string_type& name) const
{
}


} // namespace pointers
} // namespace rx

