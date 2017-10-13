

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
	size_t size = _buffer.size();
	g_memory_consuption -= (ref_counting_type)size;
}



void std_vector_allocator::allocate (size_t size)
{
	if (size == 0)
		size = 0x100;
	_buffer.resize(size);
	g_memory_consuption += (ref_counting_type)size;
}

void std_vector_allocator::deallocate ()
{
	if (!_buffer.empty())
	{
		size_t size = _buffer.size();
		_buffer.clear();
		_buffer.resize(0);
		g_memory_consuption -= (ref_counting_type)size;
	}
}

void std_vector_allocator::reallocate (size_t new_size)
{
	size_t buff_len = _buffer.size();
	if (new_size > buff_len)
	{
		size_t old_len = buff_len;
		while (buff_len < new_size)
			buff_len = buff_len * 2;
		_buffer.resize(buff_len);
		g_memory_consuption += (ref_counting_type)(buff_len - old_len);
	}
}

size_t std_vector_allocator::get_buffer_size () const
{
	return _buffer.size();
}

char* std_vector_allocator::get_char_buffer () const
{
	return (char*)&this->_buffer[0];
}


// Class rx::memory::backward_simple_allocator 

ref_counting_type backward_simple_allocator::g_memory_consuption;

backward_simple_allocator::backward_simple_allocator (size_t initial)
      : _size(initial)
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
	byte* old_ptr = _buffer;
	size_t old_size = _size;
	_size = new_size;
	_buffer = new byte[_size];
	memmove(&_buffer[_size - old_size - 1], old_ptr, old_size);
	if (old_ptr)
		delete[] old_ptr;
}

size_t backward_simple_allocator::get_buffer_size () const
{
	return _size;
}

char* backward_simple_allocator::get_char_buffer () const
{
	return (char*)_buffer;
}


} // namespace memory
} // namespace rx

