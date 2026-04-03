

/****************************************************************************
*
*  lib\rx_io_buffers.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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

#include "lib/rx_values.h"


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

size_t rx_const_io_buffer::available_size ()
{
	return rx_get_packet_available_data(buffer_);
}

const void* rx_const_io_buffer::get_data (size_t count)
{
	rx_protocol_result_t result;
	const void* ret = rx_get_from_packet(buffer_, count, &result);
	if (result == RX_PROTOCOL_OK)
		return ret;
	else
		return nullptr;
}

rx_result rx_const_io_buffer::read_node_id (rx_node_id& val)
{
	uint8_t prefix = 0;
	auto result = read_from_buffer(prefix);
	if (!result)
		return result;

	switch (prefix)
	{
		case 0:
		{// null or 1-byte numeric with DEFAULT_NAMESPACE
			uint8_t num = 0;
			result = read_from_buffer(num);
			if (!result)
				return result;
			if (num == 0)
				val = rx_node_id();
			else
				val = rx_node_id((uint32_t)num, DEFAULT_NAMESPACE);
		}
		break;
		case 1:
		{// 1-byte namespace + 2-byte numeric
			uint8_t namesp = 0;
			uint16_t num = 0;
			result = read_from_buffer(namesp);
			if (!result)
				return result;
			result = read_from_buffer(num);
			if (!result)
				return result;
			val = rx_node_id((uint32_t)num, (uint16_t)namesp);
		}
		break;
		case 2:
		{// 2-byte namespace + 4-byte numeric
			uint16_t namesp = 0;
			uint32_t num = 0;
			result = read_from_buffer(namesp);
			if (!result)
				return result;
			result = read_from_buffer(num);
			if (!result)
				return result;
			val = rx_node_id(num, namesp);
		}
		break;
		case 3:
		{// 2-byte namespace + string
			uint16_t namesp = 0;
			string_type str;
			result = read_from_buffer(namesp);
			if (!result)
				return result;
			result = read_string(str);
			if (!result)
				return result;
			val = rx_node_id(str.c_str(), namesp);
		}
		break;
		case 4:
		{// 2-byte namespace + UUID
			uint16_t namesp = 0;
			rx_uuid_t uuid{};
			result = read_from_buffer(namesp);
			if (!result)
				return result;
			result = read_from_buffer(uuid);
			if (!result)
				return result;
			val = rx_node_id(uuid, namesp);
		}
		break;
		default:
			return "Unknown node id encoding prefix!";
	}
	return true;
}

rx_result rx_const_io_buffer::read_value (rx_simple_value& val)
{
	int type = 0;
	auto result = read_from_buffer(type);
	if (!result)
		return result;
	rx_value_t rx_type = (rx_value_t)type;
	typed_value_type struct_val{ 0 };
	struct_val.value_type = rx_type;
	result = read_union(struct_val.value, rx_type);
	val = rx_simple_value(std::move(struct_val));
	return result;
}

rx_result rx_const_io_buffer::read_union (rx_value_union& who, rx_value_t type)
{
	rx_result result(true);
	if (type & RX_ARRAY_VALUE_MASK)
	{// array of values

		int count = 0;
		auto result = read_from_buffer(count);
		if (!result)
			return result;

		if (count > 0)
		{
			who.array_value.size = (uint32_t)count;
			who.array_value.values = (rx_value_union*)rx_heap_alloc(count * sizeof(rx_value_union));
			for(int i = 0; i < count; i++)
			{
				result = read_union(who.array_value.values[i], type & RX_STRIP_ARRAY_MASK);
				if (!result)
					return result;
			}
		}
		else
		{
			who.array_value.size = 0;
			who.array_value.values = nullptr;
		}
	}
	else
	{
		switch (type & RX_SIMPLE_VALUE_MASK)
		{
			case RX_NULL_TYPE:
				break;
			case RX_BOOL_TYPE:
				{
					result = read_from_buffer(who.bool_value);
					if (!result)
						return result;
				}
				break;
			case RX_UINT8_TYPE:
				{
					result = read_from_buffer(who.uint8_value);
					if (!result)
						return result;
				}
				break;
			case RX_INT8_TYPE:
				{
					result = read_from_buffer(who.int8_value);
					if (!result)
						return result;
				}
				break;
			case RX_UINT16_TYPE:
				{
					result = read_from_buffer(who.uint16_value);
					if (!result)
						return result;
				}
				break;
			case RX_INT16_TYPE:
				{
					result = read_from_buffer(who.int16_value);
					if (!result)
						return result;
				}
				break;
			case RX_UINT32_TYPE:
				{
					result = read_from_buffer(who.uint32_value);
					if (!result)
						return result;
				}
				break;
			case RX_INT32_TYPE:
				{
					result = read_from_buffer(who.int32_value);
					if (!result)
						return result;
				}
				break;
			case RX_UINT64_TYPE:
				{
					result = read_from_buffer(who.uint64_value);
					if (!result)
						return result;
				}
				break;
			case RX_INT64_TYPE:
				{
					result = read_from_buffer(who.int64_value);
					if (!result)
						return result;
				}
				break;
			case RX_FLOAT_TYPE:
				{
					result = read_from_buffer(who.float_value);
					if (!result)
						return result;
				}
				break;
			case RX_DOUBLE_TYPE:
				{
					result = read_from_buffer(who.double_value);
					if (!result)
						return result;
				}
				break;
			case RX_STRING_TYPE:
				{
					string_type temp;
					result = read_string(temp);
					if (!result)
						return result;
					if (temp.empty())
					{
						who.string_value.size = 0;
						who.string_value.value = nullptr;
					}
					else
					{
						rx_init_string_value_struct(&who.string_value, temp.c_str(), -1);
					}
				}
				break;
			case RX_TIME_TYPE:
				{
					result = read_from_buffer(who.time_value);
					if (!result)
						return result;
				}
				break;
			case RX_UUID_TYPE:
				{
					rx_uuid_t temp;
					result = read_from_buffer(temp);
					if (!result)
						return result;

#ifndef RX_VALUE_SIZE_16
					who.uuid_value = (rx_uuid_t*)rx_heap_alloc(sizeof(rx_uuid_t));
					*who.uuid_value = temp;
#else
					who.uuid_value = temp;
#endif
				}
				break;
			case RX_NODE_ID_TYPE:
				{
					rx_node_id temp;
					result = read_node_id(temp);
					if (!result)
						return result;
					who.node_id_value = (rx_node_id_struct*)rx_heap_alloc(sizeof(rx_node_id_struct));
					*who.node_id_value = *temp.c_ptr();
				}
				break;
			case RX_BYTES_TYPE:
				{
					byte_string temp;
					int count = 0;
					result = read_from_buffer(count);
					if (!result)
						return result;
					if (count > 0)
					{
						temp.resize(count);
						result = read_data(temp.data(), count);
						if (!result)
							return result;

						rx_init_bytes_value_struct(&who.bytes_value, (uint8_t*)temp.data(), count);

					}
					else
					{
						who.bytes_value.size = 0;
						who.bytes_value.value = nullptr;
					}
				}
				break;

			case RX_STRUCT_TYPE:
				{
					int count = 0;
					result = read_from_buffer(count);
					if (!result)
						return result;
					if (count > 0)
					{
						who.struct_value.size = (uint32_t)count;
						who.struct_value.values = (typed_value_type*)rx_heap_alloc(count * sizeof(typed_value_type));
						for (int i = 0; i < count; i++)
						{
							int type = 0;
							auto result = read_from_buffer(type);
							if (!result)
								return result;

							who.struct_value.values[i].value_type = (rx_value_t)type;

							result = read_union(who.struct_value.values[i].value, who.struct_value.values[i].value_type);
							if (!result)
								return result;
						}
					}
					else
					{
						who.struct_value.size = 0;
						who.struct_value.values = nullptr;
					}
				}
				break;
			default:
				RX_ASSERT(false);
				break;
		}
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

rx_result rx_io_buffer::write_node_id (const rx_node_id& val)
{
	rx_result result;
	if (val.is_null())
	{
		uint8_t b0 = 0, b1 = 0;
		result = write_to_buffer(b0);
		if (!result)
			return result;
		result = write_to_buffer(b1);
		if (!result)
			return result;
	}
	else
	{
		switch (val.get_node_type())
		{
			case rx_node_id_numeric:
				{
					uint16_t namesp = val.get_namespace();
					uint32_t num = val.get_numeric();
					if (namesp == DEFAULT_NAMESPACE && (num < 0x100))
					{// two uint8_ts encoding 0
						uint8_t b0 = 0, b1 = (uint8_t)num;
						result = write_to_buffer(b0);
						if (!result)
							return result;
						result = write_to_buffer(b1);
						if (!result)
							return result;
					}
					else if (namesp < 0x100 && num < 0x10000)
					{// four uint8_ts encoding 1
						uint8_t b0 = 1, b1 = (uint8_t)namesp;
						uint16_t val16 = (uint16_t)num;
						result = write_to_buffer(b0);
						if (!result)
							return result;
						result = write_to_buffer(b1);
						if (!result)
							return result;
						result = write_to_buffer(val16);
						if (!result)
							return result;
					}
					else
					{// regular id encoding 2
						uint8_t type = 2;
						result = write_to_buffer(type);
						if (!result)
							return result;
						result = write_to_buffer(namesp);
						if (!result)
							return result;
						result = write_to_buffer(num);
						if (!result)
							return result;
					}
				}
				break;
			case rx_node_id_string:
				{
					uint16_t namesp = val.get_namespace();
					string_type str;
					val.get_string(str);

					uint8_t type = 3;
					result = write_to_buffer(type);
					if (!result)
						return result;
					result = write_to_buffer(namesp);
					if (!result)
						return result;
					result = write_string(str);
					if (!result)
						return result;
				}
				break;
			case rx_node_id_uuid:
				{
					uint16_t namesp = val.get_namespace();
					rx_uuid_t uuid = val.get_uuid();

					uint8_t type = 4;
					result = write_to_buffer(type);
					if (!result)
						return result;
					result = write_to_buffer(namesp);
					if (!result)
						return result;
					result = write_to_buffer(uuid);
					if (!result)
						return result;
				}
				break;
			case rx_node_id_bytes:
				{
					return false;// not implemented yet!!!
				}
				break;
		}
	}
	return true;
}

rx_result rx_io_buffer::write_value (const rx_simple_value& val)
{
	int type = 0;
	auto result = write_to_buffer((int)val.get_type());
	if (!result)
		return result;

	result = write_union(val.c_ptr()->value, val.get_type());

	return result;
}

rx_result rx_io_buffer::write_union (const rx_value_union& who, rx_value_t type)
{
	rx_result result(true);
	if (type & RX_ARRAY_VALUE_MASK)
	{// array of values

		int count = (int)who.array_value.size;
		auto result = write_to_buffer(count);
		if (!result)
			return result;

		if (count > 0)
		{
			for (int i = 0; i < count; i++)
			{
				result = write_union(who.array_value.values[i], type & RX_STRIP_ARRAY_MASK);
				if (!result)
					return result;
			}
		}
	}
	else
	{
		switch (type & RX_SIMPLE_VALUE_MASK)
		{
			case RX_NULL_TYPE:
				break;
			case RX_BOOL_TYPE:
				{
					result = write_to_buffer(who.bool_value);
					if (!result)
						return result;
				}
				break;
			case RX_UINT8_TYPE:
				{
					result = write_to_buffer(who.uint8_value);
					if (!result)
						return result;
				}
				break;
			case RX_INT8_TYPE:
				{
					result = write_to_buffer(who.int8_value);
					if (!result)
						return result;
				}
				break;
			case RX_UINT16_TYPE:
				{
					result = write_to_buffer(who.uint16_value);
					if (!result)
						return result;
				}
				break;
			case RX_INT16_TYPE:
				{
					result = write_to_buffer(who.int16_value);
					if (!result)
						return result;
				}
				break;
			case RX_UINT32_TYPE:
				{
					result = write_to_buffer(who.uint32_value);
					if (!result)
						return result;
				}
				break;
			case RX_INT32_TYPE:
				{
					result = write_to_buffer(who.int32_value);
					if (!result)
						return result;
				}
				break;
			case RX_UINT64_TYPE:
				{
					result = write_to_buffer(who.uint64_value);
					if (!result)
						return result;
				}
				break;
			case RX_INT64_TYPE:
				{
					result = write_to_buffer(who.int64_value);
					if (!result)
						return result;
				}
				break;
			case RX_FLOAT_TYPE:
				{
					result = write_to_buffer(who.float_value);
					if (!result)
						return result;
				}
				break;
			case RX_DOUBLE_TYPE:
				{
					result = write_to_buffer(who.double_value);
					if (!result)
						return result;
				}
				break;
			case RX_STRING_TYPE:
				{
					string_type temp;
					if(who.string_value.value && who.string_value.size > 0)
						temp.assign(rx_c_str(&who.string_value));
					result = write_string(temp);
					if (!result)
						return result;
				}
				break;
			case RX_TIME_TYPE:
				{
					result = write_to_buffer(who.time_value);
					if (!result)
						return result;
				}
				break;
			case RX_UUID_TYPE:
				{
					rx_uuid_t temp;
#ifndef RX_VALUE_SIZE_16
					temp = *who.uuid_value;
#else
					temp = who.uuid_value;
#endif
					result = write_to_buffer(temp);
					if (!result)
						return result;

				}
				break;
			case RX_NODE_ID_TYPE:
				{
					rx_node_id temp(who.node_id_value);
					result = write_node_id(temp);
					if (!result)
						return result;
				}
				break;
			case RX_BYTES_TYPE:
				{
					byte_string temp;
					int count = (int)who.bytes_value.size;
					result = write_to_buffer(count);
					if (!result)
						return result;
					if (count > 0)
					{
						if (who.bytes_value.value)
						{
							size_t size = 0;
							result = write(rx_c_ptr(&who.bytes_value, &size), count);
							if (!result)
								return result;
						}
					}
				}
				break;

			case RX_STRUCT_TYPE:
				{
					int count = (int)who.struct_value.size;
					result = write_to_buffer(count);
					if (!result)
						return result;
					if (count > 0)
					{
						for (int i = 0; i < count; i++)
						{
							int type = 0;
							auto result = write_to_buffer((int)who.struct_value.values[i].value_type);
							if (!result)
								return result;

							result = write_union(who.struct_value.values[i].value, who.struct_value.values[i].value_type);
							if (!result)
								return result;
						}
					}
				}
				break;
			default:
				RX_ASSERT(false);
		}
	}
	return true;
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

