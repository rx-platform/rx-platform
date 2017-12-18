

/****************************************************************************
*
*  lib\rx_mem.cpp
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


// rx_mem
#include "lib/rx_mem.h"



namespace rx {

namespace memory {

// Class rx::memory::std_vector_allocator 

ref_counting_type std_vector_allocator::g_memory_consuption;

std_vector_allocator::std_vector_allocator()
{
}


std_vector_allocator::~std_vector_allocator()
{
	size_t size = buffer_.size();
	g_memory_consuption -= (ref_counting_type)size;
}



void std_vector_allocator::allocate (size_t size)
{
	if (size == 0)
		size = 0x100;
	buffer_.resize(size);
	g_memory_consuption += (ref_counting_type)size;
}

void std_vector_allocator::deallocate ()
{
	if (!buffer_.empty())
	{
		size_t size = buffer_.size();
		buffer_.clear();
		buffer_.resize(0);
		g_memory_consuption -= (ref_counting_type)size;
	}
}

void std_vector_allocator::reallocate (size_t new_size)
{
	size_t buff_len = buffer_.size();
	if (new_size > buff_len)
	{
		size_t old_len = buff_len;
		while (buff_len < new_size)
			buff_len = buff_len * 2;
		buffer_.resize(buff_len);
		g_memory_consuption += (ref_counting_type)(buff_len - old_len);
	}
}

size_t std_vector_allocator::get_buffer_size () const
{
	return buffer_.size();
}

char* std_vector_allocator::get_char_buffer () const
{
	return (char*)&this->buffer_[0];
}


// Class rx::memory::backward_simple_allocator 

ref_counting_type backward_simple_allocator::g_memory_consuption;

backward_simple_allocator::backward_simple_allocator (size_t initial)
      : size_(initial)
{
}


backward_simple_allocator::~backward_simple_allocator()
{
}



void backward_simple_allocator::allocate (size_t size)
{
}

void backward_simple_allocator::deallocate ()
{
}

void backward_simple_allocator::reallocate (size_t new_size)
{
	uint8_t* old_ptr = buffer_;
	size_t old_size = size_;
	size_ = new_size;
	buffer_ = new uint8_t[size_];
	memmove(&buffer_[size_ - old_size - 1], old_ptr, old_size);
	if (old_ptr)
		delete[] old_ptr;
}

size_t backward_simple_allocator::get_buffer_size () const
{
	return size_;
}

char* backward_simple_allocator::get_char_buffer () const
{
	return (char*)buffer_;
}


} // namespace memory
} // namespace rx

