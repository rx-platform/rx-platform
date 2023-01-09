

/****************************************************************************
*
*  lib\rx_mem.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


// rx_mem
#include "lib/rx_mem.h"



namespace rx {

namespace memory {


template<>
int8_t rx_byte_swap(int8_t val)
{
	return val;
}
template<>
uint8_t rx_byte_swap(uint8_t val)
{
	return val;
}
template<>
int16_t rx_byte_swap(int16_t val)
{
	return bswap_16(val);
}
template<>
uint16_t rx_byte_swap(uint16_t val)
{
	return bswap_16(val);
}
template<>
int32_t rx_byte_swap(int32_t val)
{
	return bswap_32(val);
}
template<>
uint32_t rx_byte_swap(uint32_t val)
{
	return bswap_32(val);
}
template<>
int64_t rx_byte_swap(int64_t val)
{
	return bswap_64(val);
}
template<>
uint64_t rx_byte_swap(uint64_t val)
{
	return bswap_64(val);
}
template<>
float rx_byte_swap(float val)
{
	return alias_cast<float>(bswap_32(alias_cast<uint32_t>(val)));
}
template<>
double rx_byte_swap(double val)
{
	return alias_cast<double>(bswap_64(alias_cast<uint64_t>(val)));
}

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



bool std_vector_allocator::allocate (size_t size)
{
	if (size == 0)
		size = 0x100;
	buffer_.resize(size);
	g_memory_consuption += (ref_counting_type)size;
	return true;
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

bool std_vector_allocator::reallocate (size_t new_size)
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
	return true;
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



bool backward_simple_allocator::allocate (size_t size)
{
	return false;
}

void backward_simple_allocator::deallocate ()
{
}

bool backward_simple_allocator::reallocate (size_t new_size)
{
	uint8_t* old_ptr = buffer_;
	size_t old_size = size_;
	size_ = new_size;
	buffer_ = new uint8_t[size_];
	if (!buffer_)
	{
		if (old_ptr)
			delete[] old_ptr;
		return false;
	}
	memmove(&buffer_[size_ - old_size - 1], old_ptr, old_size);
	if (old_ptr)
		delete[] old_ptr;
	return true;
}

size_t backward_simple_allocator::get_buffer_size () const
{
	return size_;
}

char* backward_simple_allocator::get_char_buffer () const
{
	return (char*)buffer_;
}


// Class rx::memory::page_aligned_buffer 

page_aligned_buffer::page_aligned_buffer()
	: size_(0)
	, buffer_(nullptr)
{
}

page_aligned_buffer::page_aligned_buffer (size_t size)
	: size_(size)
{
	if (size)
		buffer_ = static_cast<uint8_t*>(rx_allocate_os_memory(size));
	else
		buffer_ = nullptr;
}


page_aligned_buffer::~page_aligned_buffer()
{
	if (buffer_)
		rx_deallocate_os_memory(buffer_, size_);
}



bool page_aligned_buffer::alloc_buffer (size_t size)
{
	if (buffer_)
		free_buffer();
	size_ = size;
	if(size_)
		buffer_ = static_cast<uint8_t*>(rx_allocate_os_memory(size));
	return buffer_ != nullptr;
}

void page_aligned_buffer::free_buffer ()
{
	if (buffer_)
	{
		rx_deallocate_os_memory(buffer_, size_);
		buffer_ = nullptr;
		size_ = 0;
	}
}

uint8_t* page_aligned_buffer::buffer ()
{
	return buffer_;
}

const uint8_t* page_aligned_buffer::buffer () const
{
	return buffer_;
}

size_t page_aligned_buffer::get_size () const
{
	return size_;
}


} // namespace memory
} // namespace rx

namespace rx
{
namespace memory
{
template class std_strbuff<memory::std_vector_allocator>;
template class memory_buffer_base< rx::memory::std_vector_allocator>;
template class backward_memory_buffer_base< rx::memory::backward_simple_allocator>;
}
}
