

/****************************************************************************
*
*  lib\rx_values.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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
		RX_ASSERT(false);
		return false;// not supported yet
		//std::vector<rx_value_union> temp_array;
		//if (!reader.start_array(name))
		//	return false;
		//while (!reader.array_end())
		//{
		//	rx_value_union temp;
		//	if (!deserialize_value(reader, temp, type & RX_STRIP_ARRAY_MASK, "val"))
		//		return false;
		//	temp_array.emplace_back(temp);
		//}
		//who.array_value.size = temp_array.size();
		//if (!temp_array.empty())
		//{
		//	who.array_value.values = new rx_value_union[who.array_value.size];
		//	memcpy(who.array_value.values, &temp_array[0], sizeof(rx_value_union) * who.array_value.size);
		//}
		//else
		//{
		//	who.array_value.values = nullptr;
		//}
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
				uint8_t temp;
				if (!reader.read_byte(name, temp))
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
				rx_time temp;
				if (!reader.read_time(name, temp))
					return false;
				return rx_init_time_value(&val, temp) == RX_OK;
			}
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
			writer.start_object("item");
			serialize_value(writer, who.array_value.values[i], type & RX_STRIP_ARRAY_MASK, "val");
			writer.end_object();
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
			writer.write_byte(name, who.int8_value);
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
			writer.write_uuid(name, *who.uuid_value);
			break;
		case RX_BYTES_TYPE:
			{
				size_t size = 0;
				const std::byte* data = (const std::byte*)rx_c_ptr(&who.bytes_value, &size);
				writer.write_bytes(name, data, size);
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_BOOL_TYPE))
			return temp_val.value.bool_value != 0;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_INT8_TYPE))
			return temp_val.value.int8_value;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_UINT8_TYPE))
			return temp_val.value.uint8_value;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_INT64_TYPE))
			return temp_val.value.int16_value;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_UINT16_TYPE))
			return temp_val.value.uint16_value;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_INT32_TYPE))
			return temp_val.value.int32_value;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_UINT32_TYPE))
			return temp_val.value.uint32_value;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_INT64_TYPE))
			return temp_val.value.int64_value;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_UINT64_TYPE))
			return temp_val.value.uint64_value;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_FLOAT_TYPE))
			return temp_val.value.float_value;
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
		typed_value_type temp_val(from);
		if (rx_convert_value(&temp_val, RX_DOUBLE_TYPE))
			return temp_val.value.float_value;
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
		typed_value_type temp_val(from);
		if(rx_convert_value(&temp_val, RX_STRING_TYPE))
			return extract_value(temp_val, default_value);
	}
	return default_value;
}
byte_string extract_value(const typed_value_type& from, const byte_string& default_value)
{
	RX_ASSERT(false);
	return byte_string();
	/*if (from.get_value_type() == RX_BYTES_TYPE)
	{
		return *from.value_.value.bytes_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_BYTES_TYPE))
			return std::move(*temp_val.value_.value.bytes_value);
	}
	return default_value;*/
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
	return rx_init_time_value(&from, value);
}
bool assign_value(typed_value_type& from, const rx_uuid_t& value)
{
	return rx_init_uuid_value(&from, &value);

}
bool assign_value(typed_value_type& from, const byte_string& value)
{
	RX_ASSERT(false);
	return false;
	/*if (from.get_value_type() == RX_BYTES_TYPE)
	{
		return *from.value_.value.bytes_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_BYTES_TYPE))
			return std::move(*temp_val.value_.value.bytes_value);
	}
	return value;*/
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
	data_.time = ts;
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
	out << str.to_string_view();
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
}

rx_value_t rx_value::get_type () const
{
	return data_.value.value_type;
}

bool rx_value::convert_to (rx_value_t type)
{
	int ret_val = rx_convert_value(&data_.value, type);
	if (!ret_val)
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
	data_.time = time;
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
		return data_.quality == right.data_.quality && rx_compare_values(&data_.value, &right.data_.value) == 0;
	case time_compare_type::ms_accurate:
		return data_.quality == right.data_.quality && rx_compare_values(&data_.value, &right.data_.value) == 0
			&& (rx_time(data_.time).get_longlong_miliseconds() == rx_time(right.data_.time).get_longlong_miliseconds());
	case time_compare_type::exact:
		return data_.quality == right.data_.quality && rx_compare_values(&data_.value, &right.data_.value) == 0
			&& data_.time.t_value == right.data_.time.t_value;
	default:
		return false;
	}
}

rx_simple_value rx_value::to_simple () const
{
    return rx_simple_value(&data_.value);
}

void rx_value::set_substituted ()
{
	uint32_t dummy = data_.origin&RX_ORIGIN_MASK & RX_FORCED_ORIGIN;
	data_.origin = dummy | (data_.origin^RX_ORIGIN_MASK);
}

void rx_value::set_test ()
{
	uint32_t dummy = data_.origin&RX_ORIGIN_MASK & RX_FORCED_ORIGIN;
	data_.origin = dummy | (data_.origin^ RX_TEST_ORIGIN);
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
		if (is_test())
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
	data_.time = ts;
	data_.quality = quality;
	data_.origin = RX_DEFAULT_ORIGIN;
}
rx_value::rx_value(const rx_simple_value& right, rx_time ts, uint32_t quality)
{
	rx_copy_value(&data_.value, right.c_ptr());
	data_.time = ts;
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
	data_.time = right.get_time();
	data_.quality = quality;
	data_.origin = RX_DEFAULT_ORIGIN;
}

bool rx_value::operator==(const rx_value& right) const
{
	return data_.quality == right.data_.quality && rx_compare_values(&data_.value, &right.data_.value) == 0;
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
	out << str.to_string_view();
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
	data_.time = ts;
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
	out << str.to_string_view();
	out << " [" << rx_time(data_.time).get_string() << "]";
}

void rx_timed_value::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	ts.t_value = std::max(ts.t_value, data_.time.t_value);
	val = rx_value(&data_.value, ts, mode);
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
	data_.time = time;
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

rx_simple_value rx_timed_value::to_simple () const
{
	return rx_simple_value(&data_.value);
}


rx_timed_value::rx_timed_value()
{
	rx_init_null_value(&data_.value);
	data_.time = rx_time::null_time();
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
	data_.time = ts;
}
rx_timed_value::rx_timed_value(const rx_simple_value& right, rx_time ts)
{
	rx_copy_value(&data_.value, right.c_ptr());
	data_.time = ts;
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

