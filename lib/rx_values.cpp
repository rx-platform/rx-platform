

/****************************************************************************
*
*  lib\rx_values.cpp
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

#include "lib/rx_lib.h"

// rx_ser_lib
#include "lib/rx_ser_lib.h"
// rx_values
#include "lib/rx_values.h"



namespace rx {

namespace values {

// Class rx::values::rx_value 

rx_value::rx_value()
{
	_origin = RX_DEFUALT_ORIGIN;
	_quality = RX_DEFAULT_VALUE_QUALITY;
	_type = RX_NULL_TYPE;
}

rx_value::rx_value(const rx_value &right)
{
	memcpy(this, &right, sizeof(rx_value));
	_type = right._type;
	copy_union(_type, _value, right._value);
}

rx_value::rx_value (bool val, rx_time time, dword quality, dword origin)
{
	_type = RX_BOOL_TYPE;
	_value.bool_value = val;
	_origin = origin;
	_time = time;
	_quality = quality;
}

rx_value::rx_value (sbyte val, rx_time time, dword quality, dword origin)
{
	_type = RX_SBYTE_TYPE;
	_value.sbyte_value = val;
	_origin = origin;
	_time = time;
	_quality = quality;
}

rx_value::rx_value (byte val, rx_time time, dword quality, dword origin)
{
	_type = RX_BYTE_TYPE;
	_value.byte_value = val;
	_origin = origin;
	_time = time;
	_quality = quality;
}

rx_value::rx_value (const bit_string& val, rx_time time, dword quality, dword origin)
{
	_type = RX_BOOL_TYPE | RX_ARRAY_VALUE_MASK;
	size_t count = val.size();
	_value.array_value = new std::vector<rx_value_union>(count);
	for (size_t i = 0; i < count; i++)
	{
		_value.array_value->at(i).bool_value = val[i];
	}
	_origin = origin;
	_time = time;
	_quality = quality;
}

rx_value::rx_value (const std::vector<sbyte>& val, rx_time time, dword quality, dword origin)
{
	_type = RX_SBYTE_TYPE | RX_ARRAY_VALUE_MASK;
	size_t count = val.size();
	_value.array_value = new std::vector<rx_value_union>(count);
	for (size_t i = 0; i < count; i++)
	{
		_value.array_value->at(i).sbyte_value = val[i];
	}
	_origin = origin;
	_time = time;
	_quality = quality;
}

rx_value::rx_value (rx_value&& right)
{
	memcpy(this, &right, sizeof(rx_value));
	right._type = RX_NULL_TYPE;
}

rx_value::rx_value (sdword val, rx_time time, dword quality, dword origin)
{
	_type = RX_SDWORD_TYPE;
	_value.sdword_value = val;
	_origin = origin;
	_time = time;
	_quality = quality;
}

rx_value::rx_value (dword val, rx_time time, dword quality, dword origin)
{
	_type = RX_DWORD_TYPE;
	_value.dword_value = val;
	_origin = origin;
	_time = time;
	_quality = quality;
}

rx_value::rx_value (const string_type& val, rx_time time, dword quality, dword origin)
{
	_type = RX_STRING_TYPE;
	_value.string_value = new string_type(val);
	_origin = origin;
	_time = time;
	_quality = quality;
}


rx_value::~rx_value()
{
	clear_union(_type, _value);
}


rx_value & rx_value::operator=(const rx_value &right)
{
	clear_union(_type, _value);
	memcpy(this, &right, sizeof(rx_value));
	_type = right._type;
	copy_union(_type, _value, right._value);
	return *this;
}


bool rx_value::operator==(const rx_value &right) const
{
	return false;
}

bool rx_value::operator!=(const rx_value &right) const
{
	return !operator==(right);
}



void rx_value::get_string (string_type& val)
{
	switch (_type)
	{
		char buffer[0x20];

		case RX_NULL_TYPE:
		{
				val = "null";
		}
		break;
		case RX_BOOL_TYPE:
		{
				val =  _value.bool_value ? "true" : "false";
		}
		break;
		case RX_SBYTE_TYPE:
		case RX_SWORD_TYPE:
		case RX_SDWORD_TYPE:
			{
				snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), "%d", (dword)_value.byte_value);
				val = buffer;
			}
			break;
		case RX_BYTE_TYPE:
		case RX_WORD_TYPE:
		case RX_DWORD_TYPE:
			{
				snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), "%u", (dword)_value.byte_value);
				val = buffer;

			}
			break;
		case RX_SQWORD_TYPE:
			{

			}
			break;
		case RX_QWORD_TYPE:
			{

			}
			break;
		case RX_FLOAT_TYPE:
			{

			}
			break;
		case RX_DOUBLE_TYPE:
			{

			}
			break;
		case RX_STRING_TYPE:
			{
				val = *_value.string_value;
			}
			break;
		case RX_TIME_TYPE:
			{

			}
			break;
		case RX_UUID_TYPE:
			{

			}
			break;
		case RX_BSTRING_TYPE:
			{

			}
			break;
		case RX_COMPLEX_TYPE:
			{

			}
			break;
		default:
			{
				val = "internal error unknown type!!!";
				RX_ASSERT(false);
			}
	}
}

bool rx_value::is_good () const
{
	return ((_quality & RX_QUALITY_MASK) == RX_GOOD_QUALITY);
}

bool rx_value::is_bad () const
{
	return ((_quality & RX_QUALITY_MASK) == RX_BAD_QUALITY);
}

bool rx_value::is_uncertain () const
{
	return ((_quality & RX_QUALITY_MASK) == RX_UNCERTAIN_QUALITY);
}

bool rx_value::is_test () const
{
	return ((_origin & RX_TEST_ORIGIN_MASK) == RX_TEST_ORIGIN_MASK);
}

bool rx_value::is_substituted () const
{
	return ((_origin & RX_FORCED_ORIGIN_MASK) == RX_FORCED_ORIGIN_MASK);
}

void rx_value::clear_union (byte type, rx_value_union& value)
{
	if (IS_ARRAY_VALUE(type))
	{// array stuff
		for (auto& one : *(_value.array_value))
		{
			clear_union(type&RX_SIMPLE_VALUE_MASK, one);
		}
		delete _value.array_value;
	}
	else
	{// simple value stuff
		switch (type)
		{
		case RX_STRING_TYPE:
			delete value.string_value;
			break;
		case RX_UUID_TYPE:
			delete value.uuid_value;
			break;
		case RX_BSTRING_TYPE:
			delete value.bstring_value;
			break;
		case RX_COMPLEX_TYPE:
			delete value.complex_value;
			break;
		}
	}
}

void rx_value::copy_union (byte type, rx_value_union& to, const rx_value_union& from)
{
	memcpy(&to, &from, sizeof(rx_value_union));
	if (IS_ARRAY_VALUE(type))
	{// array stuff
		size_t size = from.array_value->size();
		to.array_value = new std::vector<rx_value_union>(size);
		for (size_t i = 0; i < size; i++)
		{
			copy_union(type&RX_SIMPLE_VALUE_MASK, from.array_value->at(i), to.array_value->at(i));
		}
	}
	else
	{// simple value stuff
		switch (type)
		{
		case RX_STRING_TYPE:
			to.string_value = new string_type(*from.string_value);
			break;
		case RX_UUID_TYPE:
			to.uuid_value = new rx_uuid(*from.uuid_value);
			break;
		case RX_BSTRING_TYPE:
			to.bstring_value = new byte_string(*from.bstring_value);
			break;
		case RX_COMPLEX_TYPE:
			to.complex_value = new complex_value_struct(*from.complex_value);
			break;
		}
	}
}

bool rx_value::is_array () const
{
	return IS_ARRAY_VALUE(_type);
}

bool rx_value::operator > (const rx_value& right)
{
	return _value.sdword_value < right._value.sdword_value;
}

bool rx_value::operator < (const rx_value& right)
{
	return _value.sdword_value > right._value.sdword_value;
}

rx_value::operator int () const
{
	return _value.sdword_value;
}

void rx_value::set_substituted ()
{
	_origin |= RX_FORCED_ORIGIN_MASK;
}

void rx_value::set_test ()
{
	_origin |= RX_TEST_ORIGIN_MASK;
}

void rx_value::get_type_string (string_type& val)
{
	switch (_type)
	{
	case RX_NULL_TYPE:
		{
			val = "null";
		}
		break;
	case RX_BOOL_TYPE:
		{
			val = "bit";
		}
		break;
	case RX_SBYTE_TYPE:
		{
			val = "int8";
		}
		break;
	case RX_BYTE_TYPE:
		{
			val = "unsigned int8";
		}
		break;
	case RX_SWORD_TYPE:
		{
			val = "int16";
		}
		break;
	case RX_WORD_TYPE:
		{
			val = "unsigned int16";
		}
		break;
	case RX_SDWORD_TYPE:
		{
			val = "int32";
		}
		break;
	case RX_DWORD_TYPE:
		{
			val = "unsigned int32";
		}
		break;
	case RX_SQWORD_TYPE:
		{
			val = "int64";
		}
		break;
	case RX_QWORD_TYPE:
		{
			val = "unsigned int64";
		}
		break;
	case RX_FLOAT_TYPE:
		{
			val = "float32";
		}
		break;
	case RX_DOUBLE_TYPE:
		{
			val = "float64";
		}
		break;
	case RX_STRING_TYPE:
		{
			val = "sting";
		}
		break;
	case RX_TIME_TYPE:
		{
			val = "time";
		}
		break;
	case RX_UUID_TYPE:
		{
			val = "uuid";
		}
		break;
	case RX_BSTRING_TYPE:
		{
			val = "byte string";
		}
		break;
	case RX_COMPLEX_TYPE:
		{
			val = "complaex";
		}
		break;
	default:
		{
			val = "internal error unknown type!!!";
			RX_ASSERT(false);
		}
	}
}

bool rx_value::serialize (base_meta_writter& stream) const
{
	if (!stream.write_uint("Quality", _quality))
		return false;
	if (!stream.write_time("Time", _time))
		return false;
	if (!stream.write_uint("Origin", _origin))
		return false;
	/*if (!stream.write_uint("TQuality", m_time_quality))
		return false;*/
	if (!stream.write_byte("Type", _type))
		return false;
	if (!serialize_value(stream, _type, _value))
		return false;
	return true;
}

bool rx_value::deserialize (base_meta_reader& stream)
{
	clear_union(_type, _value);

	if (!stream.read_uint("Quality", _quality))
		return false;
	if (!stream.read_time("Time", _time))
		return false;
	if (!stream.read_uint("Origin", _origin))
		return false;
	/*if (!stream.read_uint("TQuality", m_time_quality))
		return false;*/
	if (!stream.read_byte("Type", _type))
		return false;
	if (!deserialize_value(stream, _type, _value))
		return false;
	return true;
}

bool rx_value::serialize_value (base_meta_writter& stream, byte type, const rx_value_union& value) const
{
	if (IS_ARRAY_VALUE(type))
	{
		if (!stream.start_array("Array", (dword)value.array_value->size()))
			return false;
		byte simple_type = (type&RX_SIMPLE_VALUE_MASK);
		for (const auto& one : *value.array_value)
		{
			if (!serialize_value(stream, simple_type, one))
				return false;
		}

		if (!stream.end_array())
			return false;

		return true;
	}
	else
	{
		switch (type)
		{
		case RX_NULL_TYPE:
			return true; /// no value
						 //  null do nothing
			break;
		case RX_BOOL_TYPE:
			return stream.write_bool("Value", value.bool_value);

		case RX_SBYTE_TYPE:
			return stream.write_byte("Value", value.sbyte_value);

		case RX_BYTE_TYPE:
			return stream.write_byte("Value", value.byte_value);

		case RX_SWORD_TYPE:
			return stream.write_int("Value", (int)value.sword_value);

		case RX_WORD_TYPE:
			return stream.write_uint("Value", (dword)value.word_value);

		case RX_SDWORD_TYPE:
			return stream.write_int("Value", (dword)value.sdword_value);

		case RX_DWORD_TYPE:
			return stream.write_uint("Value", value.dword_value);

		case RX_SQWORD_TYPE:
			return stream.write_double("Value", (double)value.sqword_value);

		case RX_QWORD_TYPE:
			return stream.write_double("Value", (double)value.qword_value);

		case RX_FLOAT_TYPE:
			return stream.write_double("Value", (double)value.float_value);

		case RX_DOUBLE_TYPE:
			return stream.write_double("Value", value.double_value);

		case RX_COMPLEX_TYPE:
			if (!stream.start_object("Value"))
				return false;
			if (!stream.write_double("Real", value.complex_value->real))
				return false;
			if (!stream.write_double("Imag", value.complex_value->imag))
				return false;
			if (!stream.end_object())
				return false;
			return true;

		case RX_STRING_TYPE:
			if (value.string_value)
				return stream.write_string("Value", value.string_value->c_str());
			else
				return stream.write_string("Value", "");

		case RX_TIME_TYPE:
			return stream.write_time("Value", value.time_value);

		case RX_UUID_TYPE:
			return stream.write_uuid("Value", value.uuid_value->uuid());

		case RX_BSTRING_TYPE:
			return stream.write_bytes("Value", value.bstring_value->data(), value.bstring_value->size());

			break;
		default:
			assert(false);
		}
		return false;
	}
}

bool rx_value::deserialize_value (base_meta_reader& stream, byte type, rx_value_union& value)
{
	if (IS_ARRAY_VALUE(type))
	{// array stuff
		if (!stream.start_array("Array"))
			return false;

		byte simple_type = (type&RX_SIMPLE_VALUE_MASK);

		bool succeeded = true;

		std::vector<rx_value_union>* temp_array = new std::vector<rx_value_union>;

		while (!stream.array_end())
		{
			rx_value_union temp;
			if (!deserialize_value(stream, simple_type, temp))
			{
				succeeded = false;
				break;
			}
			temp_array->emplace_back(temp);
		}
		if (!succeeded)
		{
			for (auto& one : *temp_array)
				clear_union(simple_type, one);
			delete temp_array;

			return false;
		}
		else
		{
			value.array_value = temp_array;
			return true;
		}
	}
	else
	{
		switch (type)
		{
		case RX_NULL_TYPE:
			return true;

		case RX_BOOL_TYPE:
			return stream.read_bool("Value", value.bool_value);

		case RX_SBYTE_TYPE:
			{
				byte temp;
				if (stream.read_byte("Value", temp))
				{
					value.sbyte_value = (char)temp;
					return true;
				}
				else
					return false;
			}
			break;
		case RX_BYTE_TYPE:
			return stream.read_byte("Value", value.byte_value);

		case RX_SWORD_TYPE:
			{
				int temp;
				if (stream.read_int("Value", temp))
				{
					value.sword_value = (sword)temp;
					return true;
				}
				else
					return false;
			}
			break;
		case RX_WORD_TYPE:
			{
				dword temp;
				if (stream.read_uint("Value", temp))
				{
					value.word_value = (word)temp;
					return true;
				}
				else
					return false;
			}
			break;
		case RX_SDWORD_TYPE:
			return stream.read_int("Value", value.sdword_value);

		case RX_DWORD_TYPE:
			return stream.read_uint("Value", value.dword_value);

		case RX_SQWORD_TYPE:
			{
				double temp;
				if (stream.read_double("Value", temp))
				{
					value.sqword_value = (sqword)temp;
					return true;
				}
				else
					return false;
			}
			break;
		case RX_QWORD_TYPE:
			{
				double temp;
				if (stream.read_double("Value", temp))
				{
					value.qword_value = (qword)temp;
					return true;
				}
				else
					return false;
			}
			break;
		case RX_FLOAT_TYPE:
			{
				double temp;
				if (stream.read_double("Value", temp))
				{
					value.float_value = (float)temp;
					return true;
				}
				else
					return false;
			}
			break;
		case RX_DOUBLE_TYPE:
			return stream.read_double("Value", value.double_value);

		case RX_COMPLEX_TYPE:
			if (!stream.start_object("Value"))
				return false;
			{
				value.complex_value = new complex_value_struct;
				if (!stream.read_double("Real", value.complex_value->real))
					return false;
				if (!stream.read_double("Imag", value.complex_value->imag))
					return false;
			}
			if (!stream.end_object())
				return false;
			return true;

		case RX_STRING_TYPE:
			{
				value.string_value = new string_type();
				if (stream.read_string("Value", *value.string_value))
				{
					return true;
				}
				else
				{
					delete value.string_value;
					return false;
				}
			}
			break;
		case RX_TIME_TYPE:
			return stream.read_time("Value", value.time_value);

		case RX_UUID_TYPE:
			{
				rx_uuid_t temp;
				
				if (stream.read_uuid("Value", temp))
				{
					value.uuid_value = new rx_uuid(temp);
					return true;
				}
				else
					return false;
			}

		case RX_BSTRING_TYPE:
			{
				value.bstring_value = new byte_string;
				if (stream.read_bytes("Value", *value.bstring_value))
				{
					return true;
				}
				else
				{
					delete value.bstring_value;
					return false;
				}
			}
			break;

		default:
			RX_ASSERT(false);
		}
		return false;
	}
}

bool rx_value::adapt_quality_to_mode (const rx_mode_type& mode)
{
	bool ret = false;
	if (((_origin&RX_TEST_ORIGIN_MASK)!=0) ^ ((mode.raw_format&RX_MODE_MASK_TEST)==0))
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

bool rx_value::serialize_value (base_meta_writter& stream) const
{
	if (!stream.write_byte("Type", _type))
		return false;
	return serialize_value(stream, _type, _value);
}

bool rx_value::deserialize_value (base_meta_reader& stream)
{
	if (!stream.read_byte("Type", _type))
		return false;
	return deserialize_value(stream, _type, _value);
}

rx_value::operator dword () const
{
	return _value.dword_value;
}

rx_value::operator bool () const
{
	return _value.bool_value;
}


} // namespace values
} // namespace rx

