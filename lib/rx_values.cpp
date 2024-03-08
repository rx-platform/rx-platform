

/****************************************************************************
*
*  lib\rx_values.cpp
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

#include "rx_std.h"
#include "rx_lib.h"

// rx_ser_lib
#include "lib/rx_ser_lib.h"
// rx_values
#include "lib/rx_values.h"



namespace rx {

bool deserialize_value(base_meta_reader& reader, typed_value_type& val, const char* name)
{
	rx_value_t type;
	if (!reader.read_value_type("type", type))
		return false;

	if (RX_ARRAY_VALUE_MASK & type)
	{
		switch (type & RX_SIMPLE_VALUE_MASK)
		{
		case RX_NULL_TYPE:
			RX_ASSERT(false);
			return false;
		case RX_BOOL_TYPE:
			{
				bool ret;
				std::vector<uint_fast8_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					bool temp;
					if (!reader.read_bool(name, temp))
						return false;
					arr.emplace_back(temp ? 1 : 0);
				}
				if(arr.empty())
					ret = rx_init_bool_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_bool_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_INT8_TYPE:
			{
				bool ret;
				std::vector<int8_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					int8_t temp;
					if (!reader.read_sbyte(name, temp))
						return false;
					arr.emplace_back(temp);
				}
				if (arr.empty())
					ret = rx_init_int8_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_int8_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_UINT8_TYPE:
			{
				bool ret;
				std::vector<uint8_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					uint8_t temp;
					if (!reader.read_byte(name, temp))
						return false;
					arr.emplace_back(temp);
				}
				if (arr.empty())
					ret = rx_init_uint8_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_uint8_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_INT16_TYPE:
			{
				bool ret;
				std::vector<int16_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					int32_t temp;
					if (!reader.read_int(name, temp))
						return false;
					arr.emplace_back((int16_t)temp);
				}
				if (arr.empty())
					ret = rx_init_int16_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_int16_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_UINT16_TYPE:
			{
				bool ret;
				std::vector<uint16_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					uint32_t temp;
					if (!reader.read_uint(name, temp))
						return false;
					arr.emplace_back((uint16_t)temp);
				}
				if (arr.empty())
					ret = rx_init_uint16_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_uint16_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_INT32_TYPE:
			{
				bool ret;
				std::vector<int32_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					int32_t temp;
					if (!reader.read_int(name, temp))
						return false;
					arr.emplace_back(temp);
				}
				if (arr.empty())
					ret = rx_init_int32_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_int32_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_UINT32_TYPE:
			{
				bool ret;
				std::vector<uint32_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					uint32_t temp;
					if (!reader.read_uint(name, temp))
						return false;
					arr.emplace_back(temp);
				}
				if (arr.empty())
					ret = rx_init_uint32_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_uint32_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_INT64_TYPE:
			{
				bool ret;
				std::vector<int64_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					int64_t temp;
					if (!reader.read_int64(name, temp))
						return false;
					arr.emplace_back(temp);
				}
				if (arr.empty())
					ret = rx_init_int64_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_int64_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_UINT64_TYPE:
			{
				bool ret;
				std::vector<uint64_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					uint64_t temp;
					if (!reader.read_uint64(name, temp))
						return false;
					arr.emplace_back(temp);
				}
				if (arr.empty())
					ret = rx_init_uint64_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_uint64_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_FLOAT_TYPE:
			{
				bool ret;
				std::vector<float> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					double temp;
					if (!reader.read_double(name, temp))
						return false;
					arr.emplace_back((float)temp);
				}
				if (arr.empty())
					ret = rx_init_float_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_float_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_DOUBLE_TYPE:
			{
				bool ret;
				std::vector<double> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					double temp;
					if (!reader.read_double(name, temp))
						return false;
					arr.emplace_back(temp);
				}
				if (arr.empty())
					ret = rx_init_double_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_double_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_COMPLEX_TYPE:
			{
				bool ret;
				std::vector<complex_value_struct> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					if (!reader.start_object(name))
						return false;
					complex_value_struct temp{};
					if (!reader.read_double("Real", temp.real))
						return false;
					if (!reader.read_double("Imag", temp.imag))
						return false;
					if (!reader.end_object())
						return false;
					arr.push_back(temp);
				}
				if (arr.empty())
					ret = rx_init_complex_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_complex_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
			break;
		case RX_UUID_TYPE:
			{
				bool ret;
				std::vector<rx_uuid_t> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					rx_uuid_t temp{};
					if (!reader.read_uuid(name, temp))
						return false;
					arr.push_back(temp);
				}
				if (arr.empty())
					ret = rx_init_uuid_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_uuid_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
			break;
		case RX_NODE_ID_TYPE: 
			{
				bool ret;
				std::vector<rx_node_id_struct> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					rx_node_id temp;
					if (!reader.read_id(name, temp))
						return false;
					arr.push_back(temp.move());
				}
				if (arr.empty())
				{
					ret = rx_init_node_id_array_value(&val, nullptr, 0) == RX_OK;
				}
				else
				{
					ret = rx_init_node_id_array_value(&val, &arr[0], arr.size()) == RX_OK;
					// clean values
					for (auto& one : arr)
						rx_destory_node_id(&one);
				}
				return ret;
			}
		case RX_STRING_TYPE:
			{
				bool ret;
				string_array arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					string_type temp;
					if (!reader.read_string(name, temp))
						return false;
					arr.push_back(std::move(temp));
				}
				if (arr.empty())
				{
					ret = rx_init_string_array_value(&val, nullptr, 0) == RX_OK;
				}
				else
				{
					std::vector<const char*> helper;
					helper.reserve(arr.size());
					for (const auto& one : arr)
						helper.push_back(one.c_str());
					ret = rx_init_string_array_value(&val, &helper[0], arr.size()) == RX_OK;
				}
				return ret;
			}
		case RX_TIME_TYPE:
			{
				bool ret;
				std::vector<rx_time_struct> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					rx_time_struct temp{};
					if (!reader.read_time(name, temp))
						return false;
					arr.push_back(temp);
				}
				if (arr.empty())
					ret = rx_init_time_array_value(&val, nullptr, 0) == RX_OK;
				else
					ret = rx_init_time_array_value(&val, &arr[0], arr.size()) == RX_OK;
				return ret;
			}
		case RX_BYTES_TYPE:
			{
				bool ret;
				std::vector<byte_string> arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					byte_string temp{};
					if (!reader.read_bytes(name, temp))
						return false;
					arr.push_back(temp);
				}
				if (arr.empty())
				{
					ret = rx_init_bytes_array_value(&val, nullptr, nullptr, 0) == RX_OK;
				}
				else
				{
					std::vector<const uint8_t*> data;
					std::vector<size_t> sizes;
					data.reserve(arr.size());
					sizes.reserve(arr.size());
					for (auto& one : arr)
					{
						data.push_back((uint8_t*)&one[0]);
						sizes.push_back(one.size());
					}
					ret = rx_init_bytes_array_value(&val, &data[0], &sizes[0], arr.size()) == RX_OK;
				}
				return ret;
			}
		case RX_STRUCT_TYPE:
			{
				bool ret;
				std::vector<std::vector<typed_value_type> > arr;
				if (!reader.start_array(name))
					return false;
				while (!reader.array_end())
				{
					reader.start_array("values");
					int counter = 1;
					char name_buff[0x10];

					std::vector<typed_value_type> subs;

					while (!reader.array_end())
					{
						typed_value_type temp;
						sprintf(name_buff, "v%d", counter++);
						reader.start_object(name_buff);
						bool ret = deserialize_value(reader, temp, "val");
						if (!ret)
							return false;
						subs.push_back(temp);
						reader.end_object();
					}
					if (!subs.empty())
						arr.push_back(std::move(subs));
				}

				if (arr.empty())
				{
					ret = rx_init_struct_array_value_with_ptrs(&val, nullptr, 0) == RX_OK;
				}
				else
				{
					std::vector<struct_value_type> helper;
					helper.reserve(arr.size());
					for (auto& one : arr)
					{
						struct_value_type temp;
						temp.size = one.size();
						temp.values = &one[0];
						helper.push_back(temp);
					}
					ret = rx_init_struct_array_value(&val, &helper[0], arr.size()) == RX_OK;
				}
				return ret;
			}
			return false;
		default:
			RX_ASSERT(false);
			// shouldn't happened
		}
	}
	else
	{
		switch (type & RX_SIMPLE_VALUE_MASK)
		{
		case RX_NULL_TYPE:
			return rx_init_null_value(&val) == RX_OK;
		case RX_BOOL_TYPE:
			{
				bool temp;
				int ret = reader.read_bool(name, temp);
				if(ret)
					ret = rx_init_bool_value(&val, temp ? 1 : 0) == RX_OK;
				return ret;
			}
		case RX_INT8_TYPE:
			{
				int8_t temp;
				if (!reader.read_sbyte(name, temp))
					return false;
				return rx_init_int8_value(&val, (int8_t)temp) == RX_OK;
			}
		case RX_UINT8_TYPE:
			{
				uint8_t temp;
				if (!reader.read_byte(name, temp))
					return false;
				return rx_init_uint8_value(&val, temp) == RX_OK;
			}
		case RX_INT16_TYPE:
			{
				int32_t temp;
				if (!reader.read_int(name, temp))
					return false;
				return rx_init_int16_value(&val, (int16_t)temp) == RX_OK;
			}
		case RX_UINT16_TYPE:
			{
				uint32_t temp;
				if (!reader.read_uint(name, temp))
					return false;
				return rx_init_uint16_value(&val, (uint16_t)temp) == RX_OK;
			}
		case RX_INT32_TYPE:
			{
				int32_t temp;
				if (!reader.read_int(name, temp))
					return false;
				return rx_init_int32_value(&val, temp) == RX_OK;
			}
		case RX_UINT32_TYPE:
			{
				uint32_t temp;
				if (!reader.read_uint(name, temp))
					return false;
				return rx_init_uint32_value(&val, temp) == RX_OK;
			}
		case RX_INT64_TYPE:
			{
				int64_t temp;
				if (!reader.read_int64(name, temp))
					return false;
				return rx_init_int64_value(&val, temp) == RX_OK;
			}
		case RX_UINT64_TYPE:
			{
				uint64_t temp;
				if (!reader.read_uint64(name, temp))
					return false;
				return rx_init_uint64_value(&val, temp) == RX_OK;
			}
		case RX_FLOAT_TYPE:
			{
				double temp;
				if (!reader.read_double(name, temp))
					return false;
				return rx_init_float_value(&val, (float)temp) == RX_OK;
			}
		case RX_DOUBLE_TYPE:
			{
				double temp;
				if (!reader.read_double(name, temp))
					return false;
				return rx_init_double_value(&val, temp) == RX_OK;
			}
		case RX_NODE_ID_TYPE:
			{
				rx_node_id temp;
				if (!reader.read_id(name, temp))
					return false;
				return rx_init_node_id_value(&val, temp.c_ptr()) == RX_OK;
			}
		case RX_STRING_TYPE:
			{
				string_type temp;
				if (!reader.read_string(name, temp))
					return false;
				return rx_init_string_value(&val, temp.c_str(), -1);
			}
		case RX_TIME_TYPE:
			{
				rx_time_struct temp;
				if (!reader.read_time(name, temp))
					return false;
				return rx_init_time_value(&val, temp) == RX_OK;
			}
		case RX_BYTES_TYPE:
			{
				byte_string temp;
				if (!reader.read_bytes(name, temp))
					return false;
				if(temp.empty())
					return rx_init_bytes_value(&val, nullptr, 0);
				else
					return rx_init_bytes_value(&val, (uint8_t*)&temp[0], temp.size());
			}
			break;
		case RX_UUID_TYPE:
			{
				rx_uuid_t temp{};
				if (!reader.read_uuid(name, temp))
					return false;
				return rx_init_uuid_value(&val, &temp);
			}
			break;
		case RX_COMPLEX_TYPE:
			{
				complex_value_struct temp{};
				if (!reader.start_object(name))
					return false;
				if (!reader.read_double("Real", temp.real))
					return false;
				if (!reader.read_double("Imag", temp.imag))
					return false;
				if (!reader.end_object())
					return false;
				return rx_init_complex_value(&val, temp);
			}
			break;
		case RX_STRUCT_TYPE:
			{
				reader.start_array(name);
				int counter = 1;
				char name_buff[0x10];
				std::vector<typed_value_type> subs;

				while (!reader.array_end())
				{
					typed_value_type temp;
					sprintf(name_buff, "v%d", counter++);
					reader.start_object(name_buff);
					bool ret = deserialize_value(reader, temp, "val");
					if (!ret)
						return false;
					subs.push_back(temp);
					reader.end_object();
				}
				if(subs.empty())
					return rx_init_struct_value(&val, nullptr, 0);
				else
					return rx_init_struct_value(&val, &subs[0], subs.size());
			}
			break;
		default:
			RX_ASSERT(false);
			// shouldn't happened
		}
	}
	return true;
}
bool serialize_value(base_meta_writer& writer, const rx_value_union& who, rx_value_t type, const char* name)
{
	if (type & RX_ARRAY_VALUE_MASK)
	{// array of values
		writer.start_array(name, who.array_value.size);
		for (size_t i = 0; i < who.array_value.size; i++)
		{
			serialize_value(writer, who.array_value.values[i], type & RX_STRIP_ARRAY_MASK, "val");
		}
		writer.end_array();
	}
	else
	{
		switch (type & RX_SIMPLE_VALUE_MASK)
		{
		case RX_NULL_TYPE:
			break;
		case RX_BOOL_TYPE:
			writer.write_bool(name, who.bool_value);
			break;
		case RX_INT8_TYPE:
			writer.write_sbyte(name, who.int8_value);
			break;
		case RX_UINT8_TYPE:
			writer.write_byte(name, who.uint8_value);
			break;
		case RX_INT16_TYPE:
			writer.write_int(name, who.int16_value);
			break;
		case RX_UINT16_TYPE:
			writer.write_uint(name, who.uint16_value);
			break;
		case RX_INT32_TYPE:
			writer.write_int(name, who.int32_value);
			break;
		case RX_UINT32_TYPE:
			writer.write_uint(name, who.uint32_value);
			break;
		case RX_INT64_TYPE:
			writer.write_int64(name, who.int64_value);
			break;
		case RX_UINT64_TYPE:
			writer.write_uint64(name, who.uint64_value);
			break;
		case RX_FLOAT_TYPE:
			writer.write_double(name, who.float_value);
			break;
		case RX_DOUBLE_TYPE:
			writer.write_double(name, who.double_value);
			break;
		case RX_STRING_TYPE:
			writer.write_string(name, rx_c_str(&who.string_value));
			break;
		case RX_TIME_TYPE:
			writer.write_time(name, who.time_value);
			break;
		case RX_UUID_TYPE:
#ifndef RX_VALUE_SIZE_16
			writer.write_uuid(name, *who.uuid_value);
#else
			writer.write_uuid(name, who.uuid_value);
#endif
			break;
		case RX_BYTES_TYPE:
			{
				size_t size = 0;
				const std::byte* data = (const std::byte*)rx_c_ptr(&who.bytes_value, &size);
				writer.write_bytes(name, data, size);
			}
			break;
		case RX_STRUCT_TYPE:
			{
				size_t size = who.struct_value.size;
				writer.start_array(name, size);
				if (size)
				{
					int counter = 1;
					char name_buff[0x10];
					for (size_t i = 0; i < size; i++)
					{
						sprintf(name_buff, "v%d", counter++);

						if (!writer.start_object(name_buff))
							return false;
						if (!writer.write_value_type("type", who.struct_value.values[i].value_type))
							return false;
						if (!serialize_value(writer, who.struct_value.values[i].value, who.struct_value.values[i].value_type, "val"))
							return false;
						if (!writer.end_object())
							return false;
					}
				}
				writer.end_array();
			}
			break;
		case RX_COMPLEX_TYPE:
			{
				writer.start_object(name);
				writer.write_double("Real", who.complex_value->real);
				writer.write_double("Imag", who.complex_value->imag);
				writer.end_object();
			}
			break;
		case RX_NODE_ID_TYPE:
			writer.write_id(name, *who.node_id_value);
			break;
		}
	}
	return true;
}
void fill_quality_string(values::rx_value val, string_type& str)
{
	str = "-";
	str += " - - ";
	//if(is detailed quality)
	//	str += "--------------- --";
	//if(is high lo quality)
	//	str += "--";
	if (val.is_good())
		str[0] = 'g';
	else if (val.is_uncertain())
		str[0] = 'u';
	else if (val.is_bad())
		str[0] = 'b';
	if (val.is_test())
		str[2] = 't';
	if (val.is_substituted())
		str[4] = 's';
}
class rx_uuid;

namespace values {

rx_value_t inner_get_type(tl::type2type<bool>)
{
	return RX_BOOL_TYPE;
}
rx_value_t inner_get_type(tl::type2type<char>)
{
	return RX_INT8_TYPE;
}
rx_value_t inner_get_type(tl::type2type<int8_t>)
{
	return RX_INT8_TYPE;
}
rx_value_t inner_get_type(tl::type2type<uint8_t>)
{
	return RX_UINT8_TYPE;
}
rx_value_t inner_get_type(tl::type2type<int16_t>)
{
	return RX_INT16_TYPE;
}
rx_value_t inner_get_type(tl::type2type<uint16_t>)
{
	return RX_UINT16_TYPE;
}
rx_value_t inner_get_type(tl::type2type<int32_t>)
{
	return RX_INT32_TYPE;
}
rx_value_t inner_get_type(tl::type2type<uint32_t>)
{
	return RX_UINT32_TYPE;
}
rx_value_t inner_get_type(tl::type2type<int64_t>)
{
	return RX_INT64_TYPE;
}
rx_value_t inner_get_type(tl::type2type<uint64_t>)
{
	return RX_UINT64_TYPE;
}
rx_value_t inner_get_type(tl::type2type<float>)
{
	return RX_FLOAT_TYPE;
}
rx_value_t inner_get_type(tl::type2type<double>)
{
	return RX_DOUBLE_TYPE;
}
rx_value_t inner_get_type(tl::type2type<typename rx::rx_time>)
{
	return RX_TIME_TYPE;
}
rx_value_t inner_get_type(tl::type2type<typename rx::rx_node_id>)
{
	return RX_NODE_ID_TYPE;
}
rx_value_t inner_get_type(tl::type2type<const char*>)
{
	return RX_STRING_TYPE;
}
rx_value_t inner_get_type(tl::type2type<char*>)
{
	return RX_STRING_TYPE;
}
rx_value_t inner_get_type(tl::type2type<typename rx::rx_uuid>)
{
	return RX_UUID_TYPE;
}

rx_value_t inner_get_type(tl::type2type<uint8_t*>)
{
	return RX_BYTES_TYPE;

}

double complex_value::amplitude() const
{
	return sqrt(real*real + imag * imag);
}
string_type complex_value::to_string() const
{
	std::ostringstream stream;
	stream << real << " + " << imag << "j";
	return stream.str();
}
bool complex_value::parse_string(const string_type& str)
{
	try
	{
		real = (double)std::stod(str);
	}
	catch (std::exception&)
	{
		return false;
	}
	return true;
}



bool extract_value(const typed_value_type& from, const bool& default_value)
{
	if (from.value_type == RX_BOOL_TYPE)
	{
		return from.value.bool_value != 0;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		bool ret;
		if (rx_convert_value(&temp_val, RX_BOOL_TYPE))
			ret = temp_val.value.bool_value != 0;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
int8_t extract_value(const typed_value_type& from, const int8_t& default_value)
{
	if (from.value_type == RX_INT8_TYPE)
	{
		return from.value.int8_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		int8_t ret;
		if (rx_convert_value(&temp_val, RX_INT8_TYPE))
			ret = temp_val.value.int8_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
uint8_t extract_value(const typed_value_type& from, const uint8_t& default_value)
{
	if (from.value_type == RX_UINT8_TYPE)
	{
		return from.value.uint8_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		uint8_t ret;
		if (rx_convert_value(&temp_val, RX_UINT8_TYPE))
			ret = temp_val.value.uint8_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
int16_t extract_value(const typed_value_type& from, const int16_t& default_value)
{
	if (from.value_type == RX_INT16_TYPE)
	{
		return from.value.int16_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		int16_t ret;
		if (rx_convert_value(&temp_val, RX_INT16_TYPE))
			ret = temp_val.value.int16_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
uint16_t extract_value(const typed_value_type& from, const uint16_t& default_value)
{
	if (from.value_type == RX_UINT16_TYPE)
	{
		return from.value.uint16_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		uint16_t ret;
		if (rx_convert_value(&temp_val, RX_UINT16_TYPE))
			ret = temp_val.value.uint16_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
int32_t extract_value(const typed_value_type& from, const int32_t& default_value)
{
	if (from.value_type == RX_INT32_TYPE)
	{
		return from.value.int32_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		int32_t ret;
		if (rx_convert_value(&temp_val, RX_INT32_TYPE))
			ret = temp_val.value.int32_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
uint32_t extract_value(const typed_value_type& from, const uint32_t& default_value)
{
	if (from.value_type == RX_UINT32_TYPE)
	{
		return from.value.uint32_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		uint32_t ret;
		if (rx_convert_value(&temp_val, RX_UINT32_TYPE))
			ret = temp_val.value.uint32_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
int64_t extract_value(const typed_value_type& from, const int64_t& default_value)
{
	if (from.value_type == RX_INT64_TYPE)
	{
		return from.value.int64_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		int64_t ret;
		if (rx_convert_value(&temp_val, RX_INT64_TYPE))
			ret = temp_val.value.int64_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
uint64_t extract_value(const typed_value_type& from, const uint64_t& default_value)
{
	if (from.value_type == RX_UINT64_TYPE)
	{
		return from.value.uint64_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		uint64_t ret;
		if (rx_convert_value(&temp_val, RX_UINT64_TYPE))
			ret = temp_val.value.uint64_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
float extract_value(const typed_value_type& from, const float& default_value)
{
	if (from.value_type == RX_FLOAT_TYPE)
	{
		return from.value.float_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		float ret;
		if (rx_convert_value(&temp_val, RX_FLOAT_TYPE))
			ret = temp_val.value.float_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
double extract_value(const typed_value_type& from, const double& default_value)
{
	if (from.value_type == RX_DOUBLE_TYPE)
	{
		return from.value.double_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		double ret;
		if (rx_convert_value(&temp_val, RX_DOUBLE_TYPE))
			ret = temp_val.value.double_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
string_type extract_value(const typed_value_type& from, const string_type& default_value)
{
	if (from.value_type == RX_STRING_TYPE)
	{
		if (from.value.string_value.size > 0)
			return string_type(rx_c_str(&from.value.string_value));
		else
			return string_type();
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		string_type ret;
		if (rx_convert_value(&temp_val, RX_STRING_TYPE))
			ret = extract_value(temp_val, default_value);
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}
byte_string extract_value(const typed_value_type& from, const byte_string& default_value)
{
	if (from.value_type == RX_BYTES_TYPE)
	{
		if (from.value.bytes_value.size > 0)
		{
			size_t count = 0;
			auto data = (std::byte*)rx_c_ptr(&from.value.bytes_value, &count);
			return byte_string(data, data + count);
		}
		else
		{
			return byte_string();
		}
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		byte_string ret;
		if (rx_convert_value(&temp_val, RX_BYTES_TYPE))
			ret = extract_value(temp_val, default_value);
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}

rx_time_struct extract_value(const typed_value_type& from, const rx_time_struct& default_value)
{
	if (from.value_type == RX_TIME_TYPE)
	{
		return from.value.time_value;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		rx_time_struct ret;
		if (rx_convert_value(&temp_val, RX_TIME_TYPE))
			ret = temp_val.value.time_value;
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}

rx_uuid_t extract_value(const typed_value_type& from, const rx_uuid_t& default_value)
{
	if (from.value_type == RX_UUID_TYPE)
	{
#ifdef RX_VALUE_SIZE_16
		return from.value.uuid_value;
#else
		return *from.value.uuid_value;
#endif
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		rx_uuid_t ret;
		if (rx_convert_value(&temp_val, RX_UUID_TYPE))
#ifdef RX_VALUE_SIZE_16
			ret = temp_val.value.uuid_value;
#else
			ret = *temp_val.value.uuid_value;
#endif
		else
			ret = default_value;
		rx_destroy_value(&temp_val);
		return ret;
	}
	return default_value;
}


string_array extract_value(const typed_value_type& from, const string_array& default_value)
{
	if (from.value_type == RX_STRING_TYPE)
	{
		return string_array{ string_type(rx_c_str(&from.value.string_value)) };
	}
	else if (from.value_type == (RX_STRING_TYPE | RX_ARRAY_VALUE_MASK))
	{
		string_array ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				if (from.value.array_value.values[i].string_value.size > 0)
					ret.emplace_back(rx_c_str(&from.value.array_value.values[i].string_value));
				else
					ret.emplace_back(string_type());
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		string_array ret;
		if (rx_convert_value(&temp_val, RX_STRING_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					if (temp_val.value.array_value.values[i].string_value.size > 0)
						ret.emplace_back(rx_c_str(&temp_val.value.array_value.values[i].string_value));
					else
						ret.emplace_back(string_type());
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}

std::vector<bool> extract_value(const typed_value_type& from, const std::vector<bool>& default_value)
{
	if (from.value_type == RX_BOOL_TYPE)
	{
		return std::vector<bool>{ from.value.bool_value != 0 ? true : false };
	}
	else if (from.value_type == (RX_BOOL_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<bool> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].bool_value != 0 ? true : false);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<bool> ret;
		if (rx_convert_value(&temp_val, RX_BOOL_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].bool_value != 0 ? true : false);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<int8_t> extract_value(const typed_value_type& from, const std::vector<int8_t>& default_value)
{
	if (from.value_type == RX_INT8_TYPE)
	{
		return std::vector<int8_t>{ from.value.int8_value };
	}
	else if (from.value_type == (RX_INT8_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<int8_t> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].int8_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<int8_t> ret;
		if (rx_convert_value(&temp_val, RX_INT8_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].int8_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<uint8_t> extract_value(const typed_value_type& from, const std::vector<uint8_t>& default_value)
{
	if (from.value_type == RX_UINT8_TYPE)
	{
		return std::vector<uint8_t>{ from.value.uint8_value };
	}
	else if (from.value_type == (RX_UINT8_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<uint8_t> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].uint8_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<uint8_t> ret;
		if (rx_convert_value(&temp_val, RX_UINT8_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].uint8_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}

std::vector<int16_t> extract_value(const typed_value_type& from, const std::vector<int16_t>& default_value)
{
	if (from.value_type == RX_INT16_TYPE)
	{
		return std::vector<int16_t>{ from.value.int16_value };
	}
	else if (from.value_type == (RX_INT16_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<int16_t> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].int16_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<int16_t> ret;
		if (rx_convert_value(&temp_val, RX_INT16_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].int16_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<uint16_t> extract_value(const typed_value_type& from, const std::vector<uint16_t>& default_value)
{
	if (from.value_type == RX_UINT16_TYPE)
	{
		return std::vector<uint16_t>{ from.value.uint16_value };
	}
	else if (from.value_type == (RX_UINT16_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<uint16_t> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].uint16_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<uint16_t> ret;
		if (rx_convert_value(&temp_val, RX_UINT16_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].uint16_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}

std::vector<int32_t> extract_value(const typed_value_type& from, const std::vector<int32_t>& default_value)
{
	if (from.value_type == RX_INT32_TYPE)
	{
		return std::vector<int32_t>{ from.value.int32_value };
	}
	else if (from.value_type == (RX_INT32_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<int32_t> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].int32_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<int32_t> ret;
		if (rx_convert_value(&temp_val, RX_INT32_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].int32_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<uint32_t> extract_value(const typed_value_type& from, const std::vector<uint32_t>& default_value)
{
	if (from.value_type == RX_UINT32_TYPE)
	{
		return std::vector<uint32_t>{ from.value.uint32_value };
	}
	else if (from.value_type == (RX_UINT32_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<uint32_t> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].uint32_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<uint32_t> ret;
		if (rx_convert_value(&temp_val, RX_UINT32_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].uint32_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}

std::vector<int64_t> extract_value(const typed_value_type& from, const std::vector<int64_t>& default_value)
{
	if (from.value_type == RX_INT64_TYPE)
	{
		return std::vector<int64_t>{ from.value.int64_value };
	}
	else if (from.value_type == (RX_INT64_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<int64_t> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].int64_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<int64_t> ret;
		if (rx_convert_value(&temp_val, RX_INT64_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].int64_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<uint64_t> extract_value(const typed_value_type& from, const std::vector<uint64_t>& default_value)
{
	if (from.value_type == RX_UINT64_TYPE)
	{
		return std::vector<uint64_t>{ from.value.uint64_value };
	}
	else if (from.value_type == (RX_UINT64_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<uint64_t> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].uint64_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<uint64_t> ret;
		if (rx_convert_value(&temp_val, RX_UINT64_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].uint64_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<float> extract_value(const typed_value_type& from, const std::vector<float>& default_value)
{
	if (from.value_type == RX_FLOAT_TYPE)
	{
		return std::vector<float>{ from.value.float_value };
	}
	else if (from.value_type == (RX_FLOAT_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<float> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].float_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<float> ret;
		if (rx_convert_value(&temp_val, RX_FLOAT_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].float_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<double> extract_value(const typed_value_type& from, const std::vector<double>& default_value)
{
	if (from.value_type == RX_DOUBLE_TYPE)
	{
		return std::vector<double>{ from.value.double_value };
	}
	else if (from.value_type == (RX_DOUBLE_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<double> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].double_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<double> ret;
		if (rx_convert_value(&temp_val, RX_DOUBLE_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].double_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
	}
	return default_value;
}
std::vector<byte_string> extract_value(const typed_value_type& from, const std::vector<byte_string>& default_value)
{
	if (from.value_type == RX_BYTES_TYPE)
	{
		size_t size = 0;
		const std::byte* ptr = (std::byte*)rx_c_ptr(&from.value.bytes_value, &size);
		if(ptr)
			return std::vector<byte_string>{byte_string(ptr, ptr + size)};
		else
			return std::vector<byte_string>();
	}
	else if (from.value_type == (RX_BYTES_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<byte_string> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				size_t size = 0;
				const std::byte* ptr = (std::byte*)rx_c_ptr(&from.value.array_value.values[i].bytes_value, &size);
				if (ptr)
					ret.push_back(byte_string(ptr, ptr + size));
				else
					ret.push_back(byte_string());
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<byte_string> ret;
		if (rx_convert_value(&temp_val, RX_BYTES_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					size_t size = 0;
					const std::byte* ptr = (std::byte*)rx_c_ptr(&temp_val.value.array_value.values[i].bytes_value, &size);
					if (ptr)
						ret.push_back(byte_string(ptr, ptr + size));
					else
						ret.push_back(byte_string());
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
	}
	return default_value;
}
std::vector<rx_time_struct> extract_value(const typed_value_type& from, const std::vector<rx_time_struct>& default_value)
{
	if (from.value_type == RX_TIME_TYPE)
	{
		return std::vector<rx_time_struct>{ from.value.time_value };
	}
	else if (from.value_type == (RX_TIME_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<rx_time_struct> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].time_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<rx_time_struct> ret;
		if (rx_convert_value(&temp_val, RX_TIME_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.push_back(temp_val.value.array_value.values[i].time_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<rx_time> extract_value(const typed_value_type& from, const std::vector<rx_time>& default_value)
{
	if (from.value_type == RX_TIME_TYPE)
	{
		return std::vector<rx_time>{ from.value.time_value };
	}
	else if (from.value_type == (RX_TIME_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<rx_time> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.push_back(from.value.array_value.values[i].time_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<rx_time> ret;
		if (rx_convert_value(&temp_val, RX_TIME_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.emplace_back(temp_val.value.array_value.values[i].time_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<rx_node_id> extract_value(const typed_value_type& from, const std::vector<rx_node_id>& default_value)
{
	if (from.value_type == RX_NODE_ID_TYPE)
	{
		return std::vector<rx_node_id>{ from.value.node_id_value };
	}
	else if (from.value_type == (RX_NODE_ID_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<rx_node_id> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.emplace_back(*from.value.array_value.values[i].node_id_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<rx_node_id> ret;
		if (rx_convert_value(&temp_val, RX_NODE_ID_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.emplace_back(*temp_val.value.array_value.values[i].node_id_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}
std::vector<rx_uuid> extract_value(const typed_value_type& from, const std::vector<rx_uuid>& default_value)
{
	if (from.value_type == RX_UUID_TYPE)
	{
		return std::vector<rx_uuid>{ *from.value.uuid_value };
	}
	else if (from.value_type == (RX_UUID_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<rx_uuid> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.emplace_back(*from.value.array_value.values[i].uuid_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<rx_uuid> ret;
		if (rx_convert_value(&temp_val, RX_UUID_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.emplace_back(*temp_val.value.array_value.values[i].uuid_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}

std::vector<rx_uuid_t> extract_value(const typed_value_type& from, const std::vector<rx_uuid_t>& default_value)
{
	if (from.value_type == RX_UUID_TYPE)
	{
		return std::vector<rx_uuid_t>{ *from.value.uuid_value };
	}
	else if (from.value_type == (RX_UUID_TYPE | RX_ARRAY_VALUE_MASK))
	{
		std::vector<rx_uuid_t> ret;
		if (from.value.array_value.size > 0)
		{
			ret.reserve(from.value.array_value.size);
			for (size_t i = 0; i < from.value.array_value.size; i++)
			{
				ret.emplace_back(*from.value.array_value.values[i].uuid_value);
			}
		}
		return ret;
	}
	else
	{
		typed_value_type temp_val;
		rx_copy_value(&temp_val, &from);
		std::vector<rx_uuid_t> ret;
		if (rx_convert_value(&temp_val, RX_UUID_TYPE | RX_ARRAY_VALUE_MASK))
		{
			if (temp_val.value.array_value.size > 0)
			{
				ret.reserve(temp_val.value.array_value.size);
				for (size_t i = 0; i < temp_val.value.array_value.size; i++)
				{
					ret.emplace_back(*temp_val.value.array_value.values[i].uuid_value);
				}
			}
			rx_destroy_value(&temp_val);
		}
		else
		{
			ret = default_value;
		}
		return ret;
	}
	return default_value;
}


bool assign_value(typed_value_type& from, bool value)
{
	return rx_init_bool_value(&from, value ? 1 : 0);
}
bool assign_value(typed_value_type& from, int8_t value)
{
	return rx_init_int8_value(&from, value);
}
bool assign_value(typed_value_type& from, uint8_t value)
{
	return rx_init_uint8_value(&from, value);
}
bool assign_value(typed_value_type& from, int16_t value)
{
	return rx_init_int16_value(&from, value);
}
bool assign_value(typed_value_type& from, uint16_t value)
{
	return rx_init_uint16_value(&from, value);
}
bool assign_value(typed_value_type& from, int32_t value)
{
	return rx_init_int32_value(&from, value);
}
bool assign_value(typed_value_type& from, uint32_t value)
{
	return rx_init_uint32_value(&from, value);
}
bool assign_value(typed_value_type& from, int64_t value)
{
	return rx_init_int64_value(&from, value);
}
bool assign_value(typed_value_type& from, uint64_t value)
{
	return rx_init_uint64_value(&from, value);
}
bool assign_value(typed_value_type& from, float value)
{
	return rx_init_float_value(&from, value);
}
bool assign_value(typed_value_type& from, double value)
{
	return rx_init_double_value(&from, value);
}
bool assign_value(typed_value_type& from, string_view_type value)
{
	return rx_init_string_value(&from, &value[0], (int)value.size());
}
bool assign_value(typed_value_type& from, const string_type& value)
{
	return rx_init_string_value(&from, value.c_str(), -1);
}
bool assign_value(typed_value_type& from, const char* value)
{
	return rx_init_string_value(&from, value, -1);
}
bool assign_value(typed_value_type& from, const complex_value_struct& value)
{
	return rx_init_complex_value(&from, value);
}
bool assign_value(typed_value_type& from, rx_time_struct value)
{
	return rx_init_time_value(&from, value);
}
bool assign_value(typed_value_type& from, rx_time value)
{
	return rx_init_time_value(&from, value.c_data());
}
bool assign_value(typed_value_type& from, const rx_uuid_t& value)
{
	return rx_init_uuid_value(&from, &value);

}
bool assign_value(typed_value_type& from, const byte_string& value)
{
	if (value.empty())
		return rx_init_bytes_value(&from, NULL, 0);
	else
		return rx_init_bytes_value(&from, reinterpret_cast<const uint8_t*>(&value[0]), value.size());
}
bool assign_value(typed_value_type& from, const std::vector<rx_simple_value>& value)
{
	if (value.empty())
	{
		return rx_init_struct_value(&from, NULL, 0);
	}
	else
	{
		std::vector<const typed_value_type*> ptrs(value.size());
		int idx = 0;
		for (const auto& one : value)
			ptrs[idx++] = one.c_ptr();
		return rx_init_struct_value_with_ptrs(&from, &ptrs[0], value.size());
	}
}


bool assign_value(typed_value_type& from, const std::vector<bool>& value)
{
	if (value.empty())
	{
		return rx_init_bool_array_value(&from, NULL, 0);
	}
	else
	{
		std::vector<uint_fast8_t> temp;
		temp.reserve(value.size());
		for (auto one : value)
			temp.emplace_back(one ? 1 : 0);
		return rx_init_bool_array_value(&from, &temp[0], value.size());
	}
}
bool assign_value(typed_value_type& from, const std::vector<uint8_t>& value)
{
	if (value.empty())
		return rx_init_uint8_array_value(&from, NULL, 0);
	else
		return rx_init_uint8_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<uint16_t>& value)
{
	if (value.empty())
		return rx_init_uint16_array_value(&from, NULL, 0);
	else
		return rx_init_uint16_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<uint32_t>& value)
{
	if (value.empty())
		return rx_init_uint32_array_value(&from, NULL, 0);
	else
		return rx_init_uint32_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<uint64_t>& value)
{
	if (value.empty())
		return rx_init_uint64_array_value(&from, NULL, 0);
	else
		return rx_init_uint64_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<int8_t>& value)
{
	if (value.empty())
		return rx_init_int8_array_value(&from, NULL, 0);
	else
		return rx_init_int8_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<int16_t>& value)
{
	if (value.empty())
		return rx_init_int16_array_value(&from, NULL, 0);
	else
		return rx_init_int16_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<int32_t>& value)
{
	if (value.empty())
		return rx_init_int32_array_value(&from, NULL, 0);
	else
		return rx_init_int32_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<int64_t>& value)
{
	if (value.empty())
		return rx_init_int64_array_value(&from, NULL, 0);
	else
		return rx_init_int64_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<float>& value)
{
	if (value.empty())
		return rx_init_float_array_value(&from, NULL, 0);
	else
		return rx_init_float_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<double>& value)
{
	if (value.empty())
		return rx_init_double_array_value(&from, NULL, 0);
	else
		return rx_init_double_array_value(&from, &value[0], value.size());
}

bool assign_value(typed_value_type& from, const std::vector<rx_time_struct>& value)
{
	if (value.empty())
		return rx_init_time_array_value(&from, NULL, 0);
	else
		return rx_init_time_array_value(&from, &value[0], value.size());
}

bool assign_value(typed_value_type& from, const std::vector<rx_time>& value)
{
	if (value.empty())
	{
		return rx_init_time_array_value(&from, NULL, 0);
	}
	else
	{
		std::vector<rx_time_struct> temp;
		temp.reserve(value.size());
		for (const auto& one : value)
			temp.push_back(one.c_data());
		return rx_init_time_array_value(&from, &temp[0], temp.size());
	}
}
bool assign_value(typed_value_type& from, const std::vector<rx_uuid_t>& value)
{
	if (value.empty())
		return rx_init_uuid_array_value(&from, NULL, 0);
	else
		return rx_init_uuid_array_value(&from, &value[0], value.size());
}
bool assign_value(typed_value_type& from, const std::vector<rx_uuid>& value)
{
	if (value.empty())
		return rx_init_time_array_value(&from, NULL, 0);
	else
		return rx_init_uuid_array_value(&from, &value[0], value.size());
}

bool assign_value(typed_value_type& from, const std::vector<byte_string>& value)
{
	if (value.empty())
	{
		return rx_init_bytes_array_value(&from, NULL, 0, 0);
	}
	else
	{
		std::vector<const uint8_t*> vals;
		std::vector<size_t> sizes;
		vals.reserve(value.size());
		sizes.reserve(value.size());
		for (auto& one : value)
		{
			if (one.empty())
			{
				vals.push_back(nullptr);
				sizes.push_back(0);
			}
			else
			{
				vals.push_back((uint8_t*)&one[0]);
				sizes.push_back(one.size());
			}
		}
		return rx_init_bytes_array_value(&from, &vals[0], &sizes[0], value.size());
	}
}

bool assign_value(typed_value_type& from, const std::vector<rx_node_id>& value)
{
	if (value.empty())
		return rx_init_node_id_array_value(&from, NULL, 0);
	else
	{
		std::vector<rx_node_id_struct> vals;
		vals.reserve(value.size());
		for (auto& one : value)
		{
			vals.push_back(*one.c_ptr());
		}
		return rx_init_node_id_array_value(&from, &vals[0], value.size());
	}
}

bool assign_value(typed_value_type& from, const string_array& value)
{
	if (value.empty())
	{
		return rx_init_string_array_value(&from, nullptr, 0);
	}
	else
	{
		const char* static_vals[0x10];
		const char** vals = static_vals;
		size_t size = value.size();
		if(size>sizeof(static_vals)/sizeof(static_vals[0]))
			vals = new const char*[size];

		for (size_t i = 0; i < size; i++)
		{
			vals[i] = value[i].empty() ? nullptr : value[i].c_str();
		}
		bool ret = rx_init_string_array_value(&from, vals, size);
		if (size > sizeof(static_vals) / sizeof(static_vals[0]))
			delete[] vals;

		return ret;
	}
}
bool assign_value(typed_value_type& from, const std::vector<std::vector<rx_simple_value> >& value)
{
	return false;
	/*if (value.empty())
	{
		return rx_init_struct_array_value(&from, nullptr, 0);
	}
	else
	{
		if (value.empty())
		{
			return rx_init_struct_value(&from, NULL, 0);
		}
		else
		{
			std::vector<const struct_value_type*> ptrs(value.size());
			int idx = 0;
			for (const auto& one : value)
				ptrs[idx++] = one.c_ptr();
			return rx_init_struct_value_with_ptrs(&from, &ptrs[0], value.size());
		}
	}*/
}

bool set_integer_to_value(typed_value_type& to, int64_t val, rx_value_t type)
{
	rx_init_int64_value(&to, val);
	int ret_val = rx_convert_value(&to, type);
	return ret_val == RX_OK;
}

bool set_unassigned_to_value(typed_value_type& to, uint64_t val, rx_value_t type)
{
	rx_init_uint64_value(&to, val);
	int ret_val = rx_convert_value(&to, type);
	return ret_val == RX_OK;
}

bool set_float_to_value(typed_value_type& to, double val, rx_value_t type)
{
	rx_init_double_value(&to, val);
	int ret_val = rx_convert_value(&to, type);
	return ret_val == RX_OK;
}


// Class rx::values::rx_value 

rx_value::rx_value (const full_value_type* storage)
{
	rx_copy_value(&data_.value, &storage->value);
	data_.time = storage->time;
	data_.quality = storage->quality;
	data_.origin = storage->origin;
}

rx_value::rx_value (const typed_value_type* storage, rx_time ts, const rx_mode_type& mode)
{
	rx_copy_value(&data_.value, storage);
	data_.quality = mode.is_off() ? RX_BAD_QUALITY_OFFLINE : RX_GOOD_QUALITY;
	data_.origin = RX_DEFAULT_ORIGIN;
	if (mode.is_test())
		set_test();
	data_.time = ts.c_data();
}


rx_value::~rx_value()
{
	rx_destroy_value(&data_.value);
	static_assert(sizeof(rx_value) == sizeof(full_value_type), "Memory size has to be the same, no virtual functions or members");
}



bool rx_value::is_bad () const
{
	return ((data_.quality & RX_QUALITY_MASK) == RX_BAD_QUALITY);
}

bool rx_value::is_uncertain () const
{
	return ((data_.quality & RX_QUALITY_MASK) == RX_UNCERTAIN_QUALITY);
}

bool rx_value::is_test () const
{
	return ((data_.origin & RX_TEST_ORIGIN) == RX_TEST_ORIGIN);
}

bool rx_value::is_substituted () const
{
	return ((data_.origin & RX_FORCED_ORIGIN) == RX_FORCED_ORIGIN);
}

bool rx_value::is_good () const
{
	return ((data_.quality & RX_QUALITY_MASK) == RX_GOOD_QUALITY);
}

bool rx_value::can_operate (bool test_mode) const
{
    return test_mode ?
        is_good() && is_test()
        : is_good();
}

bool rx_value::serialize (const string_type& name, base_meta_writer& stream) const
{
	if (!stream.start_object(name.c_str()))
		return false;
	if (!stream.write_value_type("type", data_.value.value_type))
		return false;
	if (!serialize_value(stream, data_.value.value, data_.value.value_type, "val"))
		return false;
	if (!stream.write_time("ts", data_.time))
		return false;
	if (!stream.write_uint("quality", data_.quality))
		return false;
	if (!stream.write_uint("origin", data_.origin))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

bool rx_value::deserialize (const string_type& name, base_meta_reader& stream)
{
	if (!stream.start_object(name.c_str()))
		return false;
	// first destroy eventual values already inside
	rx_destroy_value(&data_.value);

	if (!deserialize_value(stream, data_.value, "val"))
		return false;
	if (!stream.read_time("ts", data_.time))
		return false;
	if (!stream.read_uint("quality", data_.quality))
		return false;
	if (!stream.read_uint("origin", data_.origin))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

void rx_value::dump_to_stream (std::ostream& out) const
{
	rx_string_wrapper str;
	rx_get_string_value(&data_.value, -1, &str);
	if(str.empty())
		out << str.c_str();
	rx_time temp(data_.time);
	out << " [" << temp.get_string() << "] ";
	string_type q;
	fill_quality_string(*this, q);
	out << q;
}

void rx_value::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	val = *this;
	if (mode.is_off())
		val.data_.quality = RX_BAD_QUALITY_OFFLINE;
	if (mode.is_test())
		val.set_test();
	if (rx_time(val.data_.time) < ts)
		val.data_.time = ts.c_data();
}

rx_value_t rx_value::get_type () const
{
	return data_.value.value_type;
}

bool rx_value::convert_to (rx_value_t type)
{
	int ret_val = rx_convert_value(&data_.value, type);
	if (ret_val != RX_OK)
	{
		data_.quality = RX_BAD_QUALITY_TYPE_MISMATCH;
	}
	return ret_val == RX_OK;
}

void rx_value::parse (const string_type& str)
{
	rx_destroy_value(&data_.value);
	if (!rx_parse_string(&data_.value, str.c_str()))
		rx_init_null_value(&data_.value);
}

bool rx_value::is_array () const
{
	return rx_is_array_value(&data_.value);
}

size_t rx_value::array_size () const
{
	size_t ret = 0;
	int ret_val = rx_get_array_size(&data_.value, &ret);
	if (ret_val)
		return ret;
	else
		throw std::invalid_argument("Value is not an array");
}

bool rx_value::is_null () const
{
	return rx_is_null_value(&data_.value);
}

bool rx_value::is_complex () const
{
	return rx_is_complex_value(&data_.value);
}

bool rx_value::is_numeric () const
{
	return rx_is_numeric_value(&data_.value);
}

bool rx_value::is_integer () const
{
	return rx_is_integer_value(&data_.value);
}

bool rx_value::is_unassigned () const
{
	return rx_is_unassigned_value(&data_.value);
}

bool rx_value::is_float () const
{
	return rx_is_float_value(&data_.value);
}

bool rx_value::is_string () const
{
	return rx_is_string_value(&data_.value);
}

string_type rx_value::get_string (size_t idx) const
{
	rx_string_wrapper str;
	rx_get_string_value(&data_.value, idx, &str);
	return str.to_string();
}

bool rx_value::get_bool (size_t idx) const
{
	int ret = 0;
	if (rx_get_bool_value(&data_.value, idx, &ret))
		return ret != 0;
	else
		return false;
}

int64_t rx_value::get_integer (rx_value_t* type, size_t idx) const
{
	int64_t ret = 0;
	if (rx_get_integer_value(&data_.value, idx, &ret, type))
		return ret;
	else
		return 0;
}

uint64_t rx_value::get_unassigned (rx_value_t* type, size_t idx) const
{
	uint64_t ret = 0;
	if (rx_get_unassigned_value(&data_.value, idx, &ret, type))
		return ret;
	else
		return 0;
}

double rx_value::get_float (rx_value_t* type, size_t idx) const
{
	double ret = 0;
	if (rx_get_float_value(&data_.value, idx, &ret, type))
		return ret;
	else
		return 0;
}

complex_value rx_value::get_complex (size_t idx) const
{
	complex_value ret{ 0,0 };
	if (rx_get_complex_value(&data_.value, idx, &ret))
		return ret;
	else
		return complex_value{ 0,0 };
}

string_type rx_value::to_string () const
{
	rx_string_wrapper str;
	rx_get_string_value(&data_.value, RX_INVALID_INDEX_VALUE, &str);
	return str.to_string();
}

void rx_value::set_integer (int64_t val, rx_value_t type, size_t idx)
{
	rx_destroy_value(&data_.value);
	if (!set_integer_to_value(data_.value, val, type))
		data_.quality = RX_BAD_QUALITY_TYPE_MISMATCH;
}

void rx_value::set_unassigned (int64_t val, rx_value_t type, size_t idx)
{
	rx_destroy_value(&data_.value);
	if (!set_unassigned_to_value(data_.value, val, type))
		data_.quality = RX_BAD_QUALITY_TYPE_MISMATCH;
}

void rx_value::set_float (double val, rx_value_t type, size_t idx)
{
	rx_destroy_value(&data_.value);
	if (!set_float_to_value(data_.value, val, type))
		data_.quality = RX_BAD_QUALITY_TYPE_MISMATCH;
}

rx_time rx_value::set_time (rx_time time)
{
	data_.time = time.c_data();
	return time;
}

rx_time rx_value::get_time () const
{
	return data_.time;
}

bool rx_value::compare (const rx_value& right, time_compare_type time_compare) const
{
	switch (time_compare)
	{
	case time_compare_type::skip:
		return data_.origin == right.data_.origin && data_.quality == right.data_.quality 
			&& rx_compare_values(&data_.value, &right.data_.value) == 0;
	case time_compare_type::ms_accurate:
		return data_.origin == right.data_.origin && data_.quality == right.data_.quality 
			&& rx_compare_values(&data_.value, &right.data_.value) == 0
			&& (rx_time(data_.time).get_longlong_miliseconds() == rx_time(right.data_.time).get_longlong_miliseconds());
	case time_compare_type::exact:
		return data_.origin == right.data_.origin && data_.quality == right.data_.quality 
			&& rx_compare_values(&data_.value, &right.data_.value) == 0
			&& data_.time.t_value == right.data_.time.t_value;
	default:
		return false;
	}
}

rx::values::rx_simple_value rx_value::to_simple () const
{
    return rx_simple_value(&data_.value);
}

void rx_value::set_substituted ()
{
	data_.origin = data_.origin | RX_FORCED_ORIGIN;
}

void rx_value::set_test ()
{
	data_.origin = data_.origin | RX_TEST_ORIGIN;
}

bool rx_value::is_dead () const
{
	return ((data_.quality & RX_QUALITY_MASK) == RX_DEAD_QUALITY);
}

bool rx_value::adapt_quality_to_mode (const rx_mode_type& mode)
{
	bool ret = false;
	if (((data_.origin&RX_TEST_ORIGIN)!=0) ^ ((mode.raw_format&RX_MODE_MASK_TEST)==0))
	{
		ret = true;
		if (mode.is_test())
			set_test();
	}
	if (is_good() && mode.is_good())
	{

	}
	return ret;
}

void rx_value::set_offline ()
{
	data_.quality = RX_BAD_QUALITY_OFFLINE;
	data_.origin |= RX_TEST_ORIGIN;
}

void rx_value::set_good_locally ()
{
	data_.quality = RX_GOOD_QUALITY;
	data_.origin = RX_LOCAL_ORIGIN;
}

uint32_t rx_value::get_quality () const
{
  return data_.quality;

}

void rx_value::set_quality (uint32_t val)
{
  data_.quality = val;

}

bool rx_value::is_byte_string () const
{
	return rx_is_bytes_value(&data_.value);
}

byte_string rx_value::get_byte_string (size_t idx) const
{
	rx_bytes_wrapper str;
	if (rx_get_bytes_value(&data_.value, idx, &str))
	{
		return str.to_bytes();
	}
	else
	{
		return byte_string();
	}
}

void rx_value::set_origin (uint32_t val)
{
	data_.origin = (data_.origin & ~RX_ORIGIN_MASK) | (val & RX_ORIGIN_MASK);
}

uint32_t rx_value::increment_signal_level ()
{
	uint32_t temp = data_.origin + 1;
	data_.origin = (data_.origin & ~RX_LEVEL_MASK) | (temp & RX_LEVEL_MASK);
	return temp;
}

uint32_t rx_value::get_signal_level () const
{
	return data_.origin & RX_LEVEL_MASK;
}

uint32_t rx_value::get_origin () const
{
	return data_.origin & RX_ORIGIN_MASK;
}

rx::values::rx_simple_value rx_value::operator [] (int index) const
{
	if (is_array())
	{
		if (index < 0 || (size_t)index >= array_size())
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		rx_simple_value ret;
		if (RX_OK != rx_get_array_value(index, &ret.data_, &data_.value))
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		return ret;
	}
	else if (is_struct())
	{
		if (index < 0 || (size_t)index >= struct_size())
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		rx_simple_value ret;
		if (RX_OK != rx_get_struct_value(index, &ret.data_, &data_.value))
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		return ret;
	}
	else
	{
		RX_ASSERT(false);
		return rx_simple_value();
	}
}

void rx_value::assign_array (const std::vector<rx_simple_value>& from, rx_time ts, uint32_t quality)
{
	if (from.empty())
		return;
	rx_value_t type = from[0].data_.value_type;
	for (const auto& val : from)
	{
		if (val.data_.value_type != type)
			return;
	}
	std::vector<const rx_value_union*> data(from.size());
	int idx = 0;
	for (const auto& val : from)
	{
		data[idx++] = &val.data_.value;
	}
	rx_destroy_value(&data_.value);
	rx_init_array_value_with_ptrs(&data_.value, type, &data[0], data.size());
}

bool rx_value::is_struct () const
{
	return rx_is_struct(&data_.value);
}

size_t rx_value::struct_size () const
{
	size_t ret = 0;
	int ret_val = rx_get_struct_size(&data_.value, &ret);
	if (ret_val)
		return ret;
	else
		throw std::invalid_argument("Value is not an array");
}


rx_value::rx_value()
{
	rx_init_null_value(&data_.value);
	data_.origin = RX_DEFAULT_ORIGIN;
	data_.quality = RX_DEFAULT_VALUE_QUALITY;
}
rx_value::rx_value(full_value_type right) noexcept
{
	rx_move_value(&data_.value, &right.value);
	data_.origin = right.origin;
	data_.quality = right.quality;
	data_.time = right.time;
}
rx_value::rx_value(rx_value&& right) noexcept
{
	rx_move_value(&data_.value, &right.data_.value);
	data_.origin = right.data_.origin;
	data_.quality = right.data_.quality;
	data_.time = right.data_.time;
}

rx_value& rx_value::operator=(rx_value&& right) noexcept
{
	rx_destroy_value(&data_.value);
	rx_move_value(&data_.value, &right.data_.value);
	data_.origin = right.data_.origin;
	data_.quality = right.data_.quality;
	data_.time = right.data_.time;
	return *this;
}
rx_value::rx_value(const rx_value &right)
{
	rx_copy_value(&data_.value, &right.data_.value);
	data_.origin = right.data_.origin;
	data_.quality = right.data_.quality;
	data_.time = right.data_.time;
}
rx_value& rx_value::operator=(const rx_value& right)
{
	rx_destroy_value(&data_.value);
	rx_copy_value(&data_.value, &right.data_.value);
	data_.origin = right.data_.origin;
	data_.quality = right.data_.quality;
	data_.time = right.data_.time;
	return *this;
}


rx_value::rx_value(rx_simple_value&& right, rx_time ts, uint32_t quality) noexcept
{
	typed_value_type temp = right.move();
	rx_move_value(&data_.value, &temp);
	data_.time = ts.c_data();
	data_.quality = quality;
	data_.origin = RX_DEFAULT_ORIGIN;
}
rx_value::rx_value(const rx_simple_value& right, rx_time ts, uint32_t quality)
{
	rx_copy_value(&data_.value, right.c_ptr());
	data_.time = ts.c_data();
	data_.quality = quality;
	data_.origin = RX_DEFAULT_ORIGIN;
}

rx_value::rx_value(rx_timed_value&& right, uint32_t quality) noexcept
{
	timed_value_type temp = right.move();
	rx_move_value(&data_.value, &temp.value);
	data_.time = temp.time;
	data_.quality = quality;
	data_.origin = RX_DEFAULT_ORIGIN;
}
rx_value::rx_value(const rx_timed_value& right, uint32_t quality)
{
	rx_copy_value(&data_.value, &right.c_ptr()->value);
	data_.time = right.get_time().c_data();
	data_.quality = quality;
	data_.origin = RX_DEFAULT_ORIGIN;
}

bool rx_value::operator==(const rx_value& right) const
{
	return data_.origin == right.data_.origin && data_.quality == right.data_.quality
		&& rx_compare_values(&data_.value, &right.data_.value) == 0;
}
bool rx_value::operator!=(const rx_value& right) const
{
	return !operator==(right);
}

full_value_type rx_value::move() noexcept
{
	full_value_type ret;
	rx_move_value(&ret.value, &data_.value);
	ret.time = data_.time;
	ret.origin = data_.origin;
	ret.quality = data_.quality;
	return ret;
}
const full_value_type* rx_value::c_ptr() const noexcept
{
	return &data_;
}
// Class rx::values::rx_simple_value 

rx_simple_value::rx_simple_value (const typed_value_type* storage)
{
	rx_copy_value(&data_, storage);
}


rx_simple_value::~rx_simple_value()
{
	rx_destroy_value(&data_);
	static_assert(sizeof(rx_simple_value) == sizeof(typed_value_type), "Memory size has to be the same, no virtual functions or members");
}


bool rx_simple_value::operator==(const rx_simple_value &right) const
{
	return rx_compare_values(&data_, &right.data_) == 0;
}

bool rx_simple_value::operator!=(const rx_simple_value &right) const
{
	return !operator==(right);
}



bool rx_simple_value::is_bad () const
{
	return false;
}

bool rx_simple_value::is_uncertain () const
{
	return false;
}

bool rx_simple_value::is_test () const
{
	return false;
}

bool rx_simple_value::is_substituted () const
{
	return false;
}

bool rx_simple_value::is_good () const
{
	return true;
}

bool rx_simple_value::can_operate (bool test_mode) const
{
	return true;
}

bool rx_simple_value::serialize (const string_type& name, base_meta_writer& writter) const
{
	if (!writter.start_object(name.c_str()))
		return false;
	if (!writter.write_value_type("type", data_.value_type))
		return false;
	if (!serialize_value(writter, data_.value, data_.value_type, "val"))
		return false;
	if (!writter.end_object())
		return false;
	return true;
}

bool rx_simple_value::deserialize (const string_type& name, base_meta_reader& reader)
{
	if (!reader.start_object(name.c_str()))
		return false;
	// first destroy eventual values already inside
	rx_destroy_value(&data_);
	if (!deserialize_value(reader, data_, "val"))
		return false;
	if (!reader.end_object())
		return false;
	return true;
}

void rx_simple_value::dump_to_stream (std::ostream& out) const
{
	rx_string_wrapper str;
	rx_get_string_value(&data_, -1, &str);
	if(!str.empty())
		out << str.c_str();
}

void rx_simple_value::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	val = rx_value(&data_, ts, mode);
}

rx_value_t rx_simple_value::get_type () const
{
	return data_.value_type;
}

bool rx_simple_value::convert_to (rx_value_t type)
{
	int ret_val = rx_convert_value(&data_, type);
	return ret_val == RX_OK;
}

void rx_simple_value::parse (const string_type& str)
{
	rx_destroy_value(&data_);
	if (!rx_parse_string(&data_, str.c_str()))
		rx_init_null_value(&data_);
}

bool rx_simple_value::is_array () const
{
	return rx_is_array_value(&data_);
}

size_t rx_simple_value::array_size () const
{
	size_t ret = 0;
	int ret_val = rx_get_array_size(&data_, &ret);
	if (ret_val)
		return ret;
	else
		throw std::invalid_argument("Value is not an array");
}

bool rx_simple_value::is_null () const
{
	return rx_is_null_value(&data_);
}

bool rx_simple_value::is_complex () const
{
	return rx_is_complex_value(&data_);
}

bool rx_simple_value::is_numeric () const
{
	return rx_is_numeric_value(&data_);
}

bool rx_simple_value::is_integer () const
{
	return rx_is_integer_value(&data_);
}

bool rx_simple_value::is_unassigned () const
{
	return rx_is_unassigned_value(&data_);
}

bool rx_simple_value::is_float () const
{
	return rx_is_float_value(&data_);
}

bool rx_simple_value::is_string () const
{
	return rx_is_string_value(&data_);
}

string_type rx_simple_value::get_string (size_t idx) const
{
	rx_string_wrapper str;
	rx_get_string_value(&data_, idx, &str);
	return str.to_string();
}

bool rx_simple_value::get_bool (size_t idx) const
{
	int ret = 0;
	if (rx_get_bool_value(&data_, idx, &ret))
		return ret != 0;
	else
		return false;
}

int64_t rx_simple_value::get_integer (rx_value_t* type, size_t idx) const
{
	int64_t ret = 0;
	if (rx_get_integer_value(&data_, idx, &ret, type))
		return ret;
	else
		return 0;
}

uint64_t rx_simple_value::get_unassigned (rx_value_t* type, size_t idx) const
{
	uint64_t ret = 0;
	if (rx_get_unassigned_value(&data_, idx, &ret, type))
		return ret;
	else
		return 0;
}

double rx_simple_value::get_float (rx_value_t* type, size_t idx) const
{
	double ret = 0;
	if (rx_get_float_value(&data_, idx, &ret, type))
		return ret;
	else
		return 0;
}

complex_value rx_simple_value::get_complex (size_t idx) const
{
	complex_value ret{ 0,0 };
	if (rx_get_complex_value(&data_, idx, &ret))
		return ret;
	else
		return complex_value{ 0,0 };
}

string_type rx_simple_value::to_string () const
{
	rx_string_wrapper str;
	rx_get_string_value(&data_, RX_INVALID_INDEX_VALUE, &str);
	return str.to_string();
}

void rx_simple_value::set_integer (int64_t val, rx_value_t type, size_t idx)
{
	rx_destroy_value(&data_);
	set_integer_to_value(data_, val, type);
}

void rx_simple_value::set_unassigned (int64_t val, rx_value_t type, size_t idx)
{
	rx_destroy_value(&data_);
	set_unassigned_to_value(data_, val, type);
}

void rx_simple_value::set_float (double val, rx_value_t type, size_t idx)
{
	rx_destroy_value(&data_);
	set_float_to_value(data_, val, type);
}

bool rx_simple_value::weak_serialize (const char* name, base_meta_writer& writter) const
{
	if (!serialize_value(writter, data_.value, data_.value_type, name))
		return false;
	return true;
}

bool rx_simple_value::weak_deserialize (const char* name, base_meta_reader& reader)
{
	if (!deserialize_value(reader, data_, name))
		return false;
	return true;
}

bool rx_simple_value::is_byte_string () const
{
	return rx_is_bytes_value(&data_);
}

byte_string rx_simple_value::get_byte_string (size_t idx) const
{
	rx_bytes_wrapper str;
	if (rx_get_bytes_value(&data_, idx, &str))
	{
		return str.to_bytes();
	}
	else
	{
		return byte_string();
	}
}

rx::values::rx_simple_value rx_simple_value::operator [] (int index) const
{
	if (is_array())
	{
		if (index < 0 || (size_t)index >= array_size())
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		rx_simple_value ret;
		if(RX_OK!=rx_get_array_value(index, &ret.data_, &data_))
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		return ret;
	}
	else if (is_struct())
	{
		if (index < 0 || (size_t)index >= struct_size())
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		rx_simple_value ret;
		if (RX_OK != rx_get_struct_value(index, &ret.data_, &data_))
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		return ret;
	}
	else
	{
		RX_ASSERT(false);
		return rx_simple_value();
	}
}

void rx_simple_value::assign_array (const std::vector<rx_simple_value>& from)
{
	if (from.empty())
	{
		rx_destroy_value(&data_);
		rx_init_array_value_with_ptrs(&data_, RX_STRUCT_TYPE, NULL, 0);
	}
	else
	{
		rx_value_t type = from[0].data_.value_type;
		for (const auto& val : from)
		{
			if (val.data_.value_type != type)
				return;
		}
		std::vector<const rx_value_union*> data(from.size());
		int idx = 0;
		for (const auto& val : from)
		{
			data[idx++] = &val.data_.value;
		}
		rx_destroy_value(&data_);
		rx_init_array_value_with_ptrs(&data_, type, &data[0], data.size());
	}
}

bool rx_simple_value::is_struct () const
{
	return rx_is_struct(&data_);
}

size_t rx_simple_value::struct_size () const
{
	size_t ret = 0;
	int ret_val = rx_get_struct_size(&data_, &ret);
	if (ret_val)
		return ret;
	else
		throw std::invalid_argument("Value is not an array");
}

rx_simple_value::rx_simple_value()
{
	rx_init_null_value(&data_);
}
rx_simple_value::rx_simple_value(typed_value_type val) noexcept
{
	rx_move_value(&data_, &val);
}
rx_simple_value::rx_simple_value(rx_simple_value&& right) noexcept
{
	rx_move_value(&data_, &right.data_);
}

rx_simple_value& rx_simple_value::operator=(rx_simple_value&& right) noexcept
{
	rx_destroy_value(&data_);
	rx_move_value(&data_, &right.data_);
	return *this;
}
rx_simple_value::rx_simple_value(const rx_simple_value &right)
{
	rx_copy_value(&data_, &right.data_);
}
rx_simple_value & rx_simple_value::operator=(const rx_simple_value &right)
{
	rx_destroy_value(&data_);
	rx_copy_value(&data_, &right.data_);
	return *this;
}

typed_value_type rx_simple_value::move() noexcept
{
	typed_value_type ret;
	rx_move_value(&ret, &data_);
	return ret;
}
const typed_value_type* rx_simple_value::c_ptr() const noexcept
{
	return &data_;
}
// Class rx::values::rx_timed_value 

rx_timed_value::rx_timed_value (const timed_value_type* storage)
{
	rx_copy_value(&data_.value, &storage->value);
	data_.time = storage->time;
}

rx_timed_value::rx_timed_value (const typed_value_type* storage, rx_time ts)
{
	rx_copy_value(&data_.value, storage);
	data_.time = ts.c_data();
}


rx_timed_value::~rx_timed_value()
{
	rx_destroy_value(&data_.value);
	static_assert(sizeof(rx_timed_value) == sizeof(timed_value_type), "Memory size has to be the same, no virtual functions or members");
}


bool rx_timed_value::operator==(const rx_timed_value &right) const
{
	return rx_compare_values(&data_.value, &right.data_.value) == 0;
}

bool rx_timed_value::operator!=(const rx_timed_value &right) const
{
	return !operator==(right);
}



bool rx_timed_value::is_bad () const
{
	return false;
}

bool rx_timed_value::is_uncertain () const
{
	return false;
}

bool rx_timed_value::is_test () const
{
	return false;
}

bool rx_timed_value::is_substituted () const
{
	return false;
}

bool rx_timed_value::is_good () const
{
	return true;
}

bool rx_timed_value::can_operate (bool test_mode) const
{
	return true;
}

bool rx_timed_value::serialize (const string_type& name, base_meta_writer& writter) const
{
	if (!writter.start_object(name.c_str()))
		return false;
	if (!writter.write_value_type("type", data_.value.value_type))
		return false;
	if (!serialize_value(writter, data_.value.value, data_.value.value_type, "val"))
		return false;
	if (!writter.write_time("ts", data_.time))
		return false;
	if (!writter.end_object())
		return false;
	return true;
}

bool rx_timed_value::deserialize (const string_type& name, base_meta_reader& reader)
{
	if (!reader.start_object(name.c_str()))
		return false;
	// first destroy eventual values already inside
	rx_destroy_value(&data_.value);
	if (!deserialize_value(reader, data_.value, "val"))
		return false;
	if (!reader.read_time("ts", data_.time))
		return false;
	if (!reader.end_object())
		return false;
	return true;
}

void rx_timed_value::dump_to_stream (std::ostream& out) const
{
	rx_string_wrapper str;
	rx_get_string_value(&data_.value, -1, &str);
	if(!str.empty())
		out << str.c_str();
	out << " [" << rx_time(data_.time).get_string() << "]";
}

void rx_timed_value::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	val = rx_value(&data_.value, std::max(ts, rx_time(data_.time)), mode);
}

rx_value_t rx_timed_value::get_type () const
{
	return data_.value.value_type;
}

bool rx_timed_value::convert_to (rx_value_t type)
{
	int ret_val = rx_convert_value(&data_.value, type);
	return ret_val == RX_OK;
}

void rx_timed_value::parse (const string_type& str)
{
	rx_destroy_value(&data_.value);
	if (!rx_parse_string(&data_.value, str.c_str()))
		rx_init_null_value(&data_.value);
}

bool rx_timed_value::is_array () const
{
	return rx_is_array_value(&data_.value);
}

size_t rx_timed_value::array_size () const
{
	size_t ret = 0;
	int ret_val = rx_get_array_size(&data_.value, &ret);
	if (ret_val)
		return ret;
	else
		throw std::invalid_argument("Value is not an array");
}

bool rx_timed_value::is_null () const
{
	return rx_is_null_value(&data_.value);
}

bool rx_timed_value::is_complex () const
{
	return rx_is_complex_value(&data_.value);
}

bool rx_timed_value::is_numeric () const
{
	return rx_is_numeric_value(&data_.value);
}

bool rx_timed_value::is_integer () const
{
	return rx_is_integer_value(&data_.value);
}

bool rx_timed_value::is_unassigned () const
{
	return rx_is_unassigned_value(&data_.value);
}

bool rx_timed_value::is_float () const
{
	return rx_is_float_value(&data_.value);
}

bool rx_timed_value::is_string () const
{
	return rx_is_string_value(&data_.value);
}

string_type rx_timed_value::get_string (size_t idx) const
{
	rx_string_wrapper str;
	rx_get_string_value(&data_.value, idx, &str);
	return str.to_string();
}

bool rx_timed_value::get_bool (size_t idx) const
{
	int ret = 0;
	if (rx_get_bool_value(&data_.value, idx, &ret))
		return ret != 0;
	else
		return false;
}

int64_t rx_timed_value::get_integer (rx_value_t* type, size_t idx) const
{
	int64_t ret = 0;
	if (rx_get_integer_value(&data_.value, idx, &ret, type))
		return ret;
	else
		return 0;
}

uint64_t rx_timed_value::get_unassigned (rx_value_t* type, size_t idx) const
{
	uint64_t ret = 0;
	if (rx_get_unassigned_value(&data_.value, idx, &ret, type))
		return ret;
	else
		return 0;
}

double rx_timed_value::get_float (rx_value_t* type, size_t idx) const
{
	double ret = 0;
	if (rx_get_float_value(&data_.value, idx, &ret, type))
		return ret;
	else
		return 0;
}

complex_value rx_timed_value::get_complex (size_t idx) const
{
	complex_value ret{ 0,0 };
	if (rx_get_complex_value(&data_.value, idx, &ret))
		return ret;
	else
		return complex_value{ 0,0 };
}

string_type rx_timed_value::to_string () const
{
	rx_string_wrapper str;
	rx_get_string_value(&data_.value, RX_INVALID_INDEX_VALUE, &str);
	return str.to_string();
}

void rx_timed_value::set_integer (int64_t val, rx_value_t type, size_t idx)
{
	rx_destroy_value(&data_.value);
	set_integer_to_value(data_.value, val, type);
}

void rx_timed_value::set_unassigned (int64_t val, rx_value_t type, size_t idx)
{
	rx_destroy_value(&data_.value);
	set_unassigned_to_value(data_.value, val, type);
}

void rx_timed_value::set_float (double val, rx_value_t type, size_t idx)
{
	rx_destroy_value(&data_.value);
	set_float_to_value(data_.value, val, type);
}

rx_time rx_timed_value::set_time (rx_time time)
{
	data_.time = time.c_data();
	return time;
}

rx_time rx_timed_value::get_time () const
{
	return data_.time;
}

bool rx_timed_value::compare (const rx_timed_value& right, time_compare_type time_compare) const
{
	switch (time_compare)
	{
	case time_compare_type::skip:
		return rx_compare_values(&data_.value, &right.data_.value) == 0;
	case time_compare_type::ms_accurate:
		return rx_compare_values(&data_.value, &right.data_.value) == 0
				&& (rx_time(data_.time).get_longlong_miliseconds() == rx_time(right.data_.time).get_longlong_miliseconds());
	case time_compare_type::exact:
		return rx_compare_values(&data_.value, &right.data_.value) == 0
				&& data_.time.t_value == right.data_.time.t_value;
	default:
		return false;
	}
}

rx::values::rx_simple_value rx_timed_value::to_simple () const
{
	return rx_simple_value(&data_.value);
}

bool rx_timed_value::is_byte_string () const
{
	return rx_is_bytes_value(&data_.value);
}

byte_string rx_timed_value::get_byte_string (size_t idx) const
{
	rx_bytes_wrapper str;
	if (rx_get_bytes_value(&data_.value, idx, &str))
	{
		return str.to_bytes();
	}
	else
	{
		return byte_string();
	}
}

rx::values::rx_simple_value rx_timed_value::operator [] (int index) const
{
	/*if (is_array())
	{
		if (index < 0 || (size_t)index >= array_size())
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		rx_simple_value ret;
		if (RX_OK != rx_get_array_value(index, &ret.data_, &data_))
		{
			RX_ASSERT(false);
			return rx_simple_value();
		}
		return ret;
	}
	else*/
	{
		RX_ASSERT(false);
		return rx_simple_value();
	}
}

void rx_timed_value::assign_array (const std::vector<rx_simple_value>& from, rx_time ts)
{
	if (from.empty())
		return;
	rx_value_t type = from[0].data_.value_type;
	for (const auto& val : from)
	{
		if (val.data_.value_type != type)
			return;
	}
	std::vector<const rx_value_union*> data(from.size());
	int idx = 0;
	for (const auto& val : from)
	{
		data[idx++] = &val.data_.value;
	}
	rx_destroy_value(&data_.value);
	rx_init_array_value_with_ptrs(&data_.value, type, &data[0], data.size());
}

bool rx_timed_value::is_struct () const
{
	return rx_is_struct(&data_.value);
}

size_t rx_timed_value::struct_size () const
{
	size_t ret = 0;
	int ret_val = rx_get_struct_size(&data_.value, &ret);
	if (ret_val)
		return ret;
	else
		throw std::invalid_argument("Value is not an array");
}


rx_timed_value::rx_timed_value()
{
	rx_init_null_value(&data_.value);
	data_.time = rx_time::null_time().c_data();
}

rx_timed_value::rx_timed_value(timed_value_type right) noexcept
{
	rx_move_value(&data_.value, &right.value);
	data_.time = right.time;
}

rx_timed_value::rx_timed_value(rx_timed_value&& right) noexcept
{
	rx_move_value(&data_.value, &right.data_.value);
	data_.time = right.data_.time;
}

rx_timed_value& rx_timed_value::operator=(rx_timed_value&& right) noexcept
{
	rx_destroy_value(&data_.value);
	rx_move_value(&data_.value, &right.data_.value);
	data_.time = right.data_.time;
	return *this;
}

rx_timed_value::rx_timed_value(const rx_timed_value &right)
{
	rx_copy_value(&data_.value, &right.data_.value);
	data_.time = right.data_.time;
}
rx_timed_value & rx_timed_value::operator=(const rx_timed_value &right)
{
	rx_destroy_value(&data_.value);
	rx_copy_value(&data_.value, &right.data_.value);
	data_.time = right.data_.time;
	return *this;
}


rx_timed_value::rx_timed_value(rx_simple_value&& right, rx_time ts) noexcept
{
	typed_value_type temp = right.move();
	rx_move_value(&data_.value, &temp);
	data_.time = ts.c_data();
}
rx_timed_value::rx_timed_value(const rx_simple_value& right, rx_time ts)
{
	rx_copy_value(&data_.value, right.c_ptr());
	data_.time = ts.c_data();
}


timed_value_type rx_timed_value::move() noexcept
{
	timed_value_type ret;
	rx_move_value(&ret.value, &data_.value);
	ret.time = data_.time;
	return ret;
}
const timed_value_type* rx_timed_value::c_ptr() const noexcept
{
	return &data_;
}

// Parameterized Class rx::values::rx_value_holder 


} // namespace values
} // namespace rx

