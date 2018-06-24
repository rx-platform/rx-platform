

/****************************************************************************
*
*  lib\rx_values.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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


template<>
rx_value_t inner_get_type(tl::type2type<bool>)
{
	return RX_BOOL_TYPE;
}

template<>
rx_value_t inner_get_type(tl::type2type<int8_t>)
{
	return RX_SBYTE_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<uint8_t>)
{
	return RX_BYTE_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<int16_t>)
{
	return RX_SWORD_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<uint16_t>)
{
	return RX_WORD_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<int32_t>)
{
	return RX_SDWORD_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<uint32_t>)
{
	return RX_DWORD_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<int64_t>)
{
	return RX_SQWORD_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<uint64_t>)
{
	return RX_QWORD_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<float>)
{
	return RX_FLOAT_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<double>)
{
	return RX_DOUBLE_TYPE;
}

template<>
rx_value_t inner_get_type(tl::type2type<typename std::string>)
{
	return RX_STRING_TYPE;
}


void rx_destroy_value_storage(rx_value_storage& storage, rx_value_t type)
{
	switch (type)
	{
	case RX_NULL_TYPE:
		break;
	case RX_BOOL_TYPE:
		storage.destroy_value<bool>();
		break;
	case RX_SBYTE_TYPE:
		storage.destroy_value<int8_t>();
		break;
	case RX_BYTE_TYPE:
		storage.destroy_value<uint8_t>();
		break;
	case RX_SWORD_TYPE:
		storage.destroy_value<int16_t>();
		break;
	case RX_WORD_TYPE:
		storage.destroy_value<uint16_t>();
		break;
	case RX_SDWORD_TYPE:
		storage.destroy_value<int32_t>();
		break;
	case RX_DWORD_TYPE:
		storage.destroy_value<uint32_t>();
		break;
	case RX_SQWORD_TYPE:
		storage.destroy_value<int64_t>();
		break;
	case RX_QWORD_TYPE:
		storage.destroy_value<uint64_t>();
		break;
	case RX_FLOAT_TYPE:
		storage.destroy_value<float>();
		break;
	case RX_DOUBLE_TYPE:
		storage.destroy_value<double>();
		break;
	case RX_STRING_TYPE:
		storage.destroy_value<string_type>();
		break;
	case RX_TIME_TYPE:
		storage.destroy_value<rx_time>();
		break;
	case RX_UUID_TYPE:
		storage.destroy_value<rx_uuid>();
		break;
	case RX_BSTRING_TYPE:
		storage.destroy_value<byte_string>();
		break;
	case RX_COMPLEX_TYPE:
		storage.destroy_value<std::complex<double> >();
		break;
	default:
		RX_ASSERT(false);
	}
}

// Class rx::values::rx_value 

rx_value::rx_value()
{
	origin_ = RX_DEFUALT_ORIGIN;
	quality_ = RX_DEFAULT_VALUE_QUALITY;
	type_ = RX_NULL_TYPE;
}

rx_value::rx_value(const rx_value &right)
{
	memcpy(this, &right, sizeof(rx_value));
	type_ = right.type_;
	copy_union(type_, value_, right.value_);
}

rx_value::rx_value (bool val, rx_time time, uint32_t quality, uint32_t origin)
{
	type_ = RX_BOOL_TYPE;
	value_.bool_value = val;
	origin_ = origin;
	time_ = time;
	quality_ = quality;
}

rx_value::rx_value (int8_t val, rx_time time, uint32_t quality, uint32_t origin)
{
	type_ = RX_SBYTE_TYPE;
	value_.int8_value = val;
	origin_ = origin;
	time_ = time;
	quality_ = quality;
}

rx_value::rx_value (uint8_t val, rx_time time, uint32_t quality, uint32_t origin)
{
	type_ = RX_BYTE_TYPE;
	value_.uint8_value = val;
	origin_ = origin;
	time_ = time;
	quality_ = quality;
}

rx_value::rx_value (const bit_string& val, rx_time time, uint32_t quality, uint32_t origin)
{
	type_ = RX_BOOL_TYPE | RX_ARRAY_VALUE_MASK;
	size_t count = val.size();
	value_.array_value = new std::vector<rx_value_union>(count);
	for (size_t i = 0; i < count; i++)
	{
		value_.array_value->at(i).bool_value = val[i];
	}
	origin_ = origin;
	time_ = time;
	quality_ = quality;
}

rx_value::rx_value (const std::vector<int8_t>& val, rx_time time, uint32_t quality, uint32_t origin)
{
	type_ = RX_SBYTE_TYPE | RX_ARRAY_VALUE_MASK;
	size_t count = val.size();
	value_.array_value = new std::vector<rx_value_union>(count);
	for (size_t i = 0; i < count; i++)
	{
		value_.array_value->at(i).int8_value = val[i];
	}
	origin_ = origin;
	time_ = time;
	quality_ = quality;
}

rx_value::rx_value (rx_value&& right)
{
	memcpy(this, &right, sizeof(rx_value));
	right.type_ = RX_NULL_TYPE;
}

rx_value::rx_value (int32_t val, rx_time time, uint32_t quality, uint32_t origin)
{
	type_ = RX_SDWORD_TYPE;
	value_.int32_value = val;
	origin_ = origin;
	time_ = time;
	quality_ = quality;
}

rx_value::rx_value (uint32_t val, rx_time time, uint32_t quality, uint32_t origin)
{
	type_ = RX_DWORD_TYPE;
	value_.uint32_value = val;
	origin_ = origin;
	time_ = time;
	quality_ = quality;
}

rx_value::rx_value (const string_type& val, rx_time time, uint32_t quality, uint32_t origin)
{
	type_ = RX_STRING_TYPE;
	value_.string_value = new string_type(val);
	origin_ = origin;
	time_ = time;
	quality_ = quality;
}


rx_value::~rx_value()
{
	clear_union(type_, value_);
}


rx_value & rx_value::operator=(const rx_value &right)
{
	clear_union(type_, value_);
	memcpy(this, &right, sizeof(rx_value));
	type_ = right.type_;
	copy_union(type_, value_, right.value_);
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
	switch (type_)
	{
		char buffer[0x20];

		case RX_NULL_TYPE:
		{
				val = "null";
		}
		break;
		case RX_BOOL_TYPE:
		{
				val =  value_.bool_value ? "true" : "false";
		}
		break;
		case RX_SBYTE_TYPE:
		case RX_SWORD_TYPE:
		case RX_SDWORD_TYPE:
			{
				snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), "%d", (int32_t)value_.int32_value);
				val = buffer;
			}
			break;
		case RX_BYTE_TYPE:
		case RX_WORD_TYPE:
		case RX_DWORD_TYPE:
			{
				snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), "%u", (uint32_t)value_.uint32_value);
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
				val = *value_.string_value;
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
	return ((quality_ & RX_QUALITY_MASK) == RX_GOOD_QUALITY);
}

bool rx_value::is_bad () const
{
	return ((quality_ & RX_QUALITY_MASK) == RX_BAD_QUALITY);
}

bool rx_value::is_uncertain () const
{
	return ((quality_ & RX_QUALITY_MASK) == RX_UNCERTAIN_QUALITY);
}

bool rx_value::is_test () const
{
	return ((origin_ & RX_TEST_ORIGIN) == RX_TEST_ORIGIN);
}

bool rx_value::is_substituted () const
{
	return ((origin_ & RX_FORCED_ORIGIN) == RX_FORCED_ORIGIN);
}

void rx_value::clear_union (uint8_t type, rx_value_union& value)
{
	if (IS_ARRAY_VALUE(type))
	{// array stuff
		for (auto& one : *(value_.array_value))
		{
			clear_union(type&RX_SIMPLE_VALUE_MASK, one);
		}
		delete value_.array_value;
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

void rx_value::copy_union (uint8_t type, rx_value_union& to, const rx_value_union& from)
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
	return IS_ARRAY_VALUE(type_);
}

bool rx_value::operator > (const rx_value& right)
{
	return value_.int32_value < right.value_.int32_value;
}

bool rx_value::operator < (const rx_value& right)
{
	return value_.int32_value > right.value_.int32_value;
}

rx_value::operator int () const
{
	return value_.int32_value;
}

void rx_value::set_substituted ()
{
	uint32_t dummy =origin_&RX_ORIGIN_MASK & RX_FORCED_ORIGIN;
	origin_ = dummy | (origin_^RX_ORIGIN_MASK);
}

void rx_value::set_test ()
{

	uint32_t dummy = origin_&RX_ORIGIN_MASK & RX_FORCED_ORIGIN;
	origin_ = dummy | (origin_^ RX_TEST_ORIGIN);
}

void rx_value::get_type_string (string_type& val)
{
	switch (type_)
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
			val = "uint8_t string";
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
	if (!stream.write_uint("Quality", quality_))
		return false;
	if (!stream.write_time("Time", time_))
		return false;
	if (!stream.write_uint("Origin", origin_))
		return false;
	/*if (!stream.write_uint("TQuality", m_time_quality))
		return false;*/
	if (!stream.write_byte("Type", type_))
		return false;
	if (!serialize_value(stream, type_, value_))
		return false;
	return true;
}

bool rx_value::deserialize (base_meta_reader& stream)
{
	clear_union(type_, value_);

	if (!stream.read_uint("Quality", quality_))
		return false;
	if (!stream.read_time("Time", time_))
		return false;
	if (!stream.read_uint("Origin", origin_))
		return false;
	/*if (!stream.read_uint("TQuality", m_time_quality))
		return false;*/
	if (!stream.read_byte("Type", type_))
		return false;
	if (!deserialize_value(stream, type_, value_))
		return false;
	return true;
}

bool rx_value::serialize_value (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const
{
	if (IS_ARRAY_VALUE(type))
	{
		if (!stream.start_array("Array", (uint32_t)value.array_value->size()))
			return false;
		uint8_t simple_type = (type&RX_SIMPLE_VALUE_MASK);
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
			return stream.write_byte("Value", value.int8_value);

		case RX_BYTE_TYPE:
			return stream.write_byte("Value", value.uint8_value);

		case RX_SWORD_TYPE:
			return stream.write_int("Value", (int)value.int16_value);

		case RX_WORD_TYPE:
			return stream.write_uint("Value", (uint32_t)value.uint16_value);

		case RX_SDWORD_TYPE:
			return stream.write_int("Value", (uint32_t)value.int32_value);

		case RX_DWORD_TYPE:
			return stream.write_uint("Value", value.uint32_value);

		case RX_SQWORD_TYPE:
			return stream.write_double("Value", (double)value.int64_value);

		case RX_QWORD_TYPE:
			return stream.write_double("Value", (double)value.uint64_value);

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

bool rx_value::deserialize_value (base_meta_reader& stream, uint8_t type, rx_value_union& value)
{
	if (IS_ARRAY_VALUE(type))
	{// array stuff
		if (!stream.start_array("Array"))
			return false;

		uint8_t simple_type = (type&RX_SIMPLE_VALUE_MASK);

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
				uint8_t temp;
				if (stream.read_byte("Value", temp))
				{
					value.int8_value = (char)temp;
					return true;
				}
				else
					return false;
			}
			break;
		case RX_BYTE_TYPE:
			return stream.read_byte("Value", value.uint8_value);

		case RX_SWORD_TYPE:
			{
				int temp;
				if (stream.read_int("Value", temp))
				{
					value.int16_value = (int16_t)temp;
					return true;
				}
				else
					return false;
			}
			break;
		case RX_WORD_TYPE:
			{
				uint32_t temp;
				if (stream.read_uint("Value", temp))
				{
					value.uint16_value = (uint16_t)temp;
					return true;
				}
				else
					return false;
			}
			break;
		case RX_SDWORD_TYPE:
			return stream.read_int("Value", value.int32_value);

		case RX_DWORD_TYPE:
			return stream.read_uint("Value", value.uint32_value);

		case RX_SQWORD_TYPE:
			{
				double temp;
				if (stream.read_double("Value", temp))
				{
					value.int64_value = (int64_t)temp;
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
					value.uint64_value = (uint64_t)temp;
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
	if (((origin_&RX_TEST_ORIGIN)!=0) ^ ((mode.raw_format&RX_MODE_MASK_TEST)==0))
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
	if (!stream.write_byte("Type", type_))
		return false;
	return serialize_value(stream, type_, value_);
}

bool rx_value::deserialize_value (base_meta_reader& stream)
{
	if (!stream.read_byte("Type", type_))
		return false;
	return deserialize_value(stream, type_, value_);
}

rx_value::operator uint32_t () const
{
	return value_.uint32_value;
}

rx_value::operator bool () const
{
	return value_.bool_value;
}

void rx_value::set_offline ()
{
	quality_ = RX_BAD_QUALITY_OFFLINE;
	origin_ |= RX_TEST_ORIGIN;
}

void rx_value::set_good_locally ()
{
	quality_ = RX_GOOD_QUALITY;
	origin_ = RX_LOCAL_ORIGIN;
}

rx_time rx_value::set_time (rx_time time)
{
	time_ = time;
	return time;
}


// Class rx::values::rx_simple_value 

rx_simple_value::rx_simple_value()
	: type_(RX_NULL_TYPE)
{
}

rx_simple_value::rx_simple_value(const rx_simple_value &right)
{
	assign_storage(storage_, right.storage_, right.type_);
}


rx_simple_value::~rx_simple_value()
{
	clear_storage(storage_, type_);
}


rx_simple_value & rx_simple_value::operator=(const rx_simple_value &right)
{
	clear_storage(storage_, type_);
	type_ = right.type_;
	assign_storage(storage_, right.storage_, right.type_);
	return *this;
}


bool rx_simple_value::operator==(const rx_simple_value &right) const
{
	return type_ == right.type_
		&& storage_.compare(right.storage_, type_);
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

bool rx_simple_value::is_array () const
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

void rx_simple_value::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	//TODONOW
	//storage_.g
	val.set_time(ts);
	if (mode.is_off())
		val.set_quality(RX_BAD_QUALITY_OFFLINE);
	else
		val.set_good_locally();
	if (mode.is_test())
		val.set_test();
}

void rx_simple_value::clear_storage (rx_value_storage& data, rx_value_t type)
{
	data.destroy_by_type(type);
}

void rx_simple_value::assign_storage (rx_value_storage& left, const rx_value_storage& right, rx_value_t type)
{
	left.assign_storage(right, type);
}

bool rx_simple_value::serialize (base_meta_writter& writter) const
{
	return storage_.serialize(writter, type_);
}

bool rx_simple_value::deserialize (base_meta_reader& reader)
{
	return storage_.deserialize(reader, type_);
}

void rx_simple_value::dump_to_stream (std::ostream& out) const
{
	storage_.dump_to_stream(out, type_);
}

void rx_simple_value::parse_from_stream (std::istream& in)
{
	storage_.parse_from_stream(in, type_);
}


rx_simple_value::rx_simple_value(rx_simple_value&& right) noexcept
{
	type_ = right.type_;
	storage_ = std::move(right.storage_);
}
// Class rx::values::rx_value_storage 


bool rx_value_storage::serialize (base_meta_writter& writter, rx_value_t type) const
{
	writter.start_object("_Val");
	writter.write_byte("Type", type);
	switch (type)
	{
	case RX_NULL_TYPE:
		break;
	case RX_BOOL_TYPE:
		writter.write_bool("Val", value<bool>());
		break;
	case RX_SBYTE_TYPE:
		writter.write_byte("Val", value<int8_t>());
		break;
	case RX_BYTE_TYPE:
		writter.write_byte("Val", value<uint8_t>());
		break;
	case RX_SWORD_TYPE:
		writter.write_int("Val", value<int16_t>());
		break;
	case RX_WORD_TYPE:
		writter.write_uint("Val", value<uint16_t>());
		break;
	case RX_SDWORD_TYPE:
		writter.write_int("Val", value<int32_t>());
		break;
	case RX_DWORD_TYPE:
		writter.write_uint("Val", value<uint32_t>());
		break;
	case RX_SQWORD_TYPE:
		writter.write_int64("Val", value<int64_t>());
		break;
	case RX_QWORD_TYPE:
		writter.write_uint64("Val", value<uint64_t>());
		break;
	case RX_FLOAT_TYPE:
		writter.write_double("Val", value<float>());
		break;
	case RX_DOUBLE_TYPE:
		writter.write_double("Val", value<double>());
		break;
	case RX_STRING_TYPE:
	{
		auto val = value<std::string>();
		writter.write_string("Val", val.c_str());
	}
		//writter.write_string("Val", value<std::string>().c_str());
		break;
	case RX_TIME_TYPE:
		writter.write_time("Val", value<rx::rx_time>());
		break;
	/*case RX_UUID_TYPE:
		writter.write_uuid("Val", value<rx::rx_uuid>());
		break;
	case RX_BSTRING_TYPE:
	{
	}
	break;
	case RX_COMPLEX_TYPE:
	{
	}
	break;*/
	}
	return writter.end_object();
}

bool rx_value_storage::deserialize (base_meta_reader& reader, rx_value_t& type)
{
	// first destroy eventual values allready inside
	destroy_by_type(type);
	if (!reader.start_object("_Val"))
		return false;
	if (!reader.read_byte("Type", type))
		return false;
	switch (type)
	{
	case RX_NULL_TYPE:
		break;
	case RX_BOOL_TYPE:
		reader.read_bool("Val", allocate_empty<bool>());
		break;
	case RX_SBYTE_TYPE:
		{
			uint8_t temp;
			reader.read_byte("Val", temp);
			allocate_empty<int8_t>() = temp;
		}		
		break;
	case RX_BYTE_TYPE:
		reader.read_byte("Val", allocate_empty<uint8_t>());
		break;
	case RX_SWORD_TYPE:
		{
			uint32_t temp;
			reader.read_uint("Val", temp);
			allocate_empty<int16_t>() = (int16_t)temp;
		}
		break;
	case RX_WORD_TYPE:
		{
			uint32_t temp;
			reader.read_uint("Val", temp);
			allocate_empty<uint16_t>() = (uint16_t)temp;
		}
		break;
	case RX_SDWORD_TYPE:
		reader.read_int("Val", allocate_empty<int32_t>());
		break;
	case RX_DWORD_TYPE:
		reader.read_uint("Val", allocate_empty<uint32_t>());
		break;
	case RX_SQWORD_TYPE:
		reader.read_int64("Val", allocate_empty<int64_t>());
		break;
	case RX_QWORD_TYPE:
		reader.read_uint64("Val", allocate_empty<uint64_t>());
		break;
	case RX_FLOAT_TYPE:
		{
			double temp;
			reader.read_double("Val", temp);
			allocate_empty<float>() = (float)temp;
		}
		break;
	case RX_DOUBLE_TYPE:
		reader.read_double("Val", allocate_empty<double>());
		break;
	case RX_STRING_TYPE:
	{
		string_type& val = allocate_empty<std::string>();
		reader.read_string("Val", val);
	}
	//reader,read_string("Val", value<std::string>().c_str());
	break;
	case RX_TIME_TYPE:
		reader.read_time("Val", allocate_empty<rx::rx_time>());
		break;
		/*case RX_UUID_TYPE:
		reader,read_uuid("Val", value<rx::rx_uuid>());
		break;
		case RX_BSTRING_TYPE:
		{
		}
		break;
		case RX_COMPLEX_TYPE:
		{
		}
		break;*/
	}
	if (!reader.end_object())
		return false;
	
	return true;
}

void rx_value_storage::dump_to_stream (std::ostream& out, rx_value_t type) const
{
	switch (type)
	{
	case RX_NULL_TYPE:
		out << "<null>";
		break;
	case RX_BOOL_TYPE:
		out << (value<bool>() ? "true" : "false");
		break;
	case RX_SBYTE_TYPE:
		out << value<int8_t>()<<"sb";
		break;
	case RX_BYTE_TYPE:
		out << value<uint8_t>() << "b";
		break;
	case RX_SWORD_TYPE:
		out << value<int16_t>() << "sw";
		break;
	case RX_WORD_TYPE:
		out << value<uint16_t>() << "w";
		break;
	case RX_SDWORD_TYPE:
		out << value<int32_t>() << "sdw";
		break;
	case RX_DWORD_TYPE:
		out << value<uint32_t>() << "dw";
		break;
	case RX_SQWORD_TYPE:
		out << value<int64_t>() << "sqw";
		break;
	case RX_QWORD_TYPE:
		out << value<uint64_t>() << "qw";
		break;
	case RX_FLOAT_TYPE:
		out << value<float>() << "f8";
		break;
	case RX_DOUBLE_TYPE:
		out << value<double>() << "f64";
		break;
	case RX_STRING_TYPE:
		auto val = value<std::string>();
		out << "\"" << val << "\"";
		break;
	}
}

void rx_value_storage::parse_from_stream (std::istream& in, rx_value_t type)
{
}

void rx_value_storage::destroy_by_type (const rx_value_t type)
{
	switch (type)
	{
	case RX_NULL_TYPE:
		break;
	case RX_BOOL_TYPE:
		destroy_value<bool>();
		break;
	case RX_SBYTE_TYPE:
		destroy_value<int8_t>();
		break;
	case RX_BYTE_TYPE:
		destroy_value<uint8_t>();
		break;
	case RX_SWORD_TYPE:
		destroy_value<int16_t>();
		break;
	case RX_WORD_TYPE:
		destroy_value<uint16_t>();
		break;
	case RX_SDWORD_TYPE:
		destroy_value<int32_t>();
		break;
	case RX_DWORD_TYPE:
		destroy_value<uint32_t>();
		break;
	case RX_SQWORD_TYPE:
		destroy_value<int64_t>();
		break;
	case RX_QWORD_TYPE:
		destroy_value<uint64_t>();
		break;
	case RX_FLOAT_TYPE:
		destroy_value<float>();
		break;
	case RX_DOUBLE_TYPE:
		destroy_value<double>();
		break;
	case RX_STRING_TYPE:
		destroy_value<string_type>();
		break;
	case RX_TIME_TYPE:
		destroy_value<rx::rx_time>();
		break;
	case RX_UUID_TYPE:
		destroy_value<rx::rx_uuid>();
		break;
	case RX_BSTRING_TYPE:
		destroy_value<byte_string>();
		break;
	case RX_COMPLEX_TYPE:
		assert(false);
		break;
	default:
		assert(false);
	}
}

void rx_value_storage::assign_storage (const rx_value_storage& right, rx_value_t type)
{
	switch (type)
	{
	case RX_NULL_TYPE:
		break;
	case RX_BOOL_TYPE:
		assign<bool>(right.value<bool>());
		break;
	case RX_SBYTE_TYPE:
		assign<int8_t>(right.value<int8_t>());
		break;
	case RX_BYTE_TYPE:
		assign<uint8_t>(right.value<uint8_t>());
		break;
	case RX_SWORD_TYPE:
		assign<int16_t>(right.value<int16_t>());
		break;
	case RX_WORD_TYPE:
		assign<uint16_t>(right.value<uint16_t>());
		break;
	case RX_SDWORD_TYPE:
		assign<int32_t>(right.value<int32_t>());
		break;
	case RX_DWORD_TYPE:
		assign<uint32_t>(right.value<uint32_t>());
		break;
	case RX_SQWORD_TYPE:
		assign<int64_t>(right.value<int64_t>());
		break;
	case RX_QWORD_TYPE:
		assign<uint64_t>(right.value<uint64_t>());
		break;
	case RX_FLOAT_TYPE:
		assign<float>(right.value<float>());
		break;
	case RX_DOUBLE_TYPE:
		assign<double>(right.value<double>());
		break;
	case RX_STRING_TYPE:
		assign<string_type>(right.value<string_type>());
		break;
	case RX_TIME_TYPE:
		assign<rx_time>(right.value<rx_time>());
		break;
	case RX_UUID_TYPE:
		assign<rx_uuid>(right.value<rx_uuid>());
		break;
	case RX_BSTRING_TYPE:
		assign<byte_string>(right.value<byte_string>());
		break;
	case RX_COMPLEX_TYPE:
		assert(false);
		break;
	default:
		assert(false);
	}
}

bool rx_value_storage::compare (const rx_value_storage& right, rx_value_t type) const
{
	switch (type)
	{
	case RX_NULL_TYPE:
		break;
	case RX_BOOL_TYPE:
		return value<bool>() == (right.value<bool>());
	case RX_SBYTE_TYPE:
		return value<int8_t>()==(right.value<int8_t>());
	case RX_BYTE_TYPE:
		return value<uint8_t>() == (right.value<uint8_t>());
	case RX_SWORD_TYPE:
		return value<int16_t>() == (right.value<int16_t>());
	case RX_WORD_TYPE:
		return value<uint16_t>() == (right.value<uint16_t>());
	case RX_SDWORD_TYPE:
		return value<int32_t>() == (right.value<int32_t>());
	case RX_DWORD_TYPE:
		return value<uint32_t>() == (right.value<uint32_t>());
	case RX_SQWORD_TYPE:
		return value<int64_t>() == (right.value<int64_t>());
	case RX_QWORD_TYPE:
		return value<uint64_t>() == (right.value<uint64_t>());
	case RX_FLOAT_TYPE:
		return value<float>() == (right.value<float>());
	case RX_DOUBLE_TYPE:
		return value<double>() == (right.value<double>());
	case RX_STRING_TYPE:
		return value<string_type>() == (right.value<string_type>());
	case RX_TIME_TYPE:
		return value<rx_time>() == (right.value<rx_time>());
	case RX_UUID_TYPE:
		return value<rx_uuid>() == (right.value<rx_uuid>());
	case RX_BSTRING_TYPE:
		return value<byte_string>() == (right.value<byte_string>());
	case RX_COMPLEX_TYPE:
		assert(false);
		break;
	default:
		assert(false);
	}
	return false;
}


// Class rx::values::rx_timed_value 

rx_timed_value::rx_timed_value()
      : default_time_compare_(time_compare_skip)
	, type_(RX_NULL_TYPE)
{
}

rx_timed_value::rx_timed_value(const rx_timed_value &right)
      : default_time_compare_(time_compare_skip)
{
}


rx_timed_value::~rx_timed_value()
{
	clear_storage(storage_, type_);
}


rx_timed_value & rx_timed_value::operator=(const rx_timed_value &right)
{
	clear_storage(storage_, type_);
	type_ = right.type_;
	assign_storage(storage_, right.storage_, right.type_);
	return *this;
}


bool rx_timed_value::operator==(const rx_timed_value &right) const
{
	return compare(right, default_time_compare_);
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

bool rx_timed_value::is_array () const
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

void rx_timed_value::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	//TODONOW
	//storage_.g
	val.set_time(ts);
	if (mode.is_off())
		val.set_quality(RX_BAD_QUALITY_OFFLINE);
	else
		val.set_good_locally();
	if (mode.is_test())
		val.set_test();
}

void rx_timed_value::clear_storage (rx_value_storage& data, rx_value_t type)
{
	data.destroy_by_type(type);
}

void rx_timed_value::assign_storage (rx_value_storage& left, const rx_value_storage& right, rx_value_t type)
{
	left.assign_storage(right, type);
}

bool rx_timed_value::serialize (base_meta_writter& writter) const
{
	if (!writter.start_object("Timed_"))
		return false;
	if (!storage_.serialize(writter, type_))
		return false;
	if (!writter.write_time("time", time_))
		return false;
	if (!writter.end_object())
		return false;
	return true;
}

bool rx_timed_value::deserialize (base_meta_reader& reader)
{
	if (!reader.start_object("Timed_"))
		return false;
	if (!storage_.deserialize(reader, type_))
		return false;
	if (!reader.read_time("time", time_))
		return false;
	if (!reader.end_object())
		return false;
	return true;
}

void rx_timed_value::dump_to_stream (std::ostream& out) const
{
	storage_.dump_to_stream(out, type_);
	out << " [" << time_.get_string() << "]";
}

void rx_timed_value::parse_from_stream (std::istream& in)
{
	storage_.parse_from_stream(in, type_);
}

rx_time rx_timed_value::set_time (rx_time time)
{
	time_ = time;
	return time;
}

bool rx_timed_value::compare (const rx_timed_value& right, time_compare_type time_compare) const
{
	switch (time_compare)
	{
	case time_compare_skip:
		return type_ == right.type_
			&& storage_.compare(right.storage_, type_);
	case time_compare_ms_accurate:
		return type_ == right.type_
			&& storage_.compare(right.storage_, type_)
				&& (time_.get_longlong_miliseconds() == right.time_.get_longlong_miliseconds());
	case time_compare_exact:
		return type_ == right.type_
			&& storage_.compare(right.storage_, type_)
				&& time_ == right.time_;
	default:
		return false;
	}
}


rx_timed_value::rx_timed_value(rx_timed_value&& right) noexcept
	: default_time_compare_(time_compare_skip)
{
	time_ = right.time_;
	type_ = right.type_;
	storage_ = std::move(right.storage_);
}
} // namespace values
} // namespace rx

