

/****************************************************************************
*
*  lib\rx_io_buffers.cpp
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


// rx_io_buffers
#include "lib/rx_io_buffers.h"



namespace rx {

namespace io {

// Class rx::io::rx_const_io_buffer 

rx_const_io_buffer::rx_const_io_buffer (rx_const_packet_buffer* buffer)
	: buffer_(buffer)
{
}



rx_result rx_const_io_buffer::read_string (string_type& val)
{
	uint32_t length = 0;
	auto result = read_from_buffer(length);
	if (result)
	{
		rx_protocol_result_t ret;
		char* temp = (char*)rx_get_from_packet(buffer_, length, &ret);
		if (temp)
		{
			val.assign(temp, length);
		}
		else
			result = rx_protocol_error_message(ret);
	}
	return result;
}

rx_result rx_const_io_buffer::read_chars (string_type& val)
{
	auto available = rx_get_packet_available_data(buffer_);
	if (available > 0)
	{
		rx_protocol_result_t ret;
		char* temp = (char*)rx_get_from_packet(buffer_, available, &ret);
		if (temp)
		{
			val.assign(temp, available);
			return true;
		}
		else
			return rx_protocol_error_message(ret);
	}
	return "Buffer empty!";
}

rx_const_packet_buffer rx_const_io_buffer::create_from_chars (const string_type& str)
{
	rx_const_packet_buffer ret{ (const uint8_t*)str.c_str(), 0, str.size() };
	return ret;
}

rx_const_packet_buffer* rx_const_io_buffer::c_buffer ()
{
	return buffer_;
}

size_t rx_const_io_buffer::get_possition ()
{
	return buffer_->next_read;
}

rx_result rx_const_io_buffer::read_data (void* data, size_t count)
{
	auto available = rx_get_packet_available_data(buffer_);
	if (available > 0)
	{
		rx_protocol_result_t ret;
		const void* temp = (char*)rx_get_from_packet(buffer_, count, &ret);
		if (temp)
		{
			memcpy(data, temp, count);
			return true;
		}
		else
			return rx_protocol_error_message(ret);
	}
	return "Buffer empty!";
}

void rx_const_io_buffer::skip (size_t count)
{
	rx_protocol_result_t ret;
	rx_get_from_packet(buffer_, count, &ret);
}

bool rx_const_io_buffer::eof () const
{
	return rx_buffer_eof(buffer_) != 0;
}

rx_result rx_const_io_buffer::read_to_end (byte_string& data)
{
	auto available = rx_get_packet_available_data(buffer_);
	if (available > 0)
	{
		rx_protocol_result_t ret;
		std::byte* temp = (std::byte*)rx_get_from_packet(buffer_, available, &ret);
		if (temp)
		{
			data.assign(temp, temp + available);
		}
		else
		{
			return rx_protocol_error_message(ret);
		}
	}
	else
	{
		data.clear();
	}
	return true;
}


// Class rx::io::rx_io_buffer 

rx_io_buffer::rx_io_buffer()
{
	memzero(this, sizeof(rx_packet_buffer));
}

rx_io_buffer::rx_io_buffer (size_t initial_capacity, size_t initial_front_capacity)
{
	auto result = rx_init_packet_buffer(this, initial_capacity, initial_front_capacity);
	if (result != RX_PROTOCOL_OK)
	{
		memzero(this, sizeof(rx_packet_buffer));
	}
}


rx_io_buffer::~rx_io_buffer()
{
	if (buffer_ptr)
		rx_deinit_packet_buffer(this);
}



bool rx_io_buffer::valid () const
{
	return (buffer_ptr != nullptr);
}

void rx_io_buffer::attach (rx_packet_buffer* buffer)
{
	if (buffer_ptr)
		rx_deinit_packet_buffer(this);
	if (buffer)
		memcpy(this, buffer, sizeof(rx_packet_buffer));
	else
		memzero(this, sizeof(rx_packet_buffer));
}

void rx_io_buffer::detach (rx_packet_buffer* buffer)
{
	if (buffer)
		memcpy(buffer, this, sizeof(rx_packet_buffer));
	memzero(this, sizeof(rx_packet_buffer));
}

void rx_io_buffer::release ()
{
	if (buffer_ptr)
		rx_deinit_packet_buffer(this);
	memzero(this, sizeof(rx_packet_buffer));
}

rx_io_buffer::operator bool () const
{
	return valid();
}

rx_result rx_io_buffer::write_string (const string_type& val)
{
	uint32_t size = (uint32_t)val.size();
	auto result = write_to_buffer(size);
	if (result)
	{
		result = write(&val[0], size);
	}
	return result;
}

rx_result rx_io_buffer::write_chars (const string_type& val)
{
	return write(val.c_str(), val.size());
}

rx_result rx_io_buffer::write (const void* data, size_t size)
{
	rx_protocol_result_t result;

	result = rx_push_to_packet(this, data, size);

	if (result == RX_PROTOCOL_OK)
		return true;
	else
		return rx_protocol_error_message(result);
}

bool rx_io_buffer::empty () const
{
	return this->size == 0;
}

void rx_io_buffer::reinit ()
{
	rx_reinit_packet_buffer(this);
}

void rx_io_buffer::zero_memory ()
{
	memzero(this, sizeof(rx_packet_buffer));
}

rx_result rx_io_buffer::write_front (const void* data, size_t size)
{
	rx_protocol_result_t result;

	result = rx_push_to_packet_front(this, data, size);

	if (result == RX_PROTOCOL_OK)
		return true;
	else
		return rx_protocol_error_message(result);
}

rx_io_buffer::rx_io_buffer(rx_io_buffer&& right) noexcept
{
	if (right.buffer_ptr)
	{
		memcpy(this, &right, sizeof(rx_packet_buffer));
		right.zero_memory();
	}
	else
	{
		memzero(this, sizeof(rx_packet_buffer));
	}
}
rx_io_buffer& rx_io_buffer::operator=(rx_io_buffer&& right) noexcept
{
	if (buffer_ptr)
		rx_deinit_packet_buffer(this);
	if (right.buffer_ptr)
	{
		memcpy(this, &right, sizeof(rx_packet_buffer));
		right.zero_memory();
	}
	else
	{
		memzero(this, sizeof(rx_packet_buffer));
	}

	return *this;
}
} // namespace io
} // namespace rx

