

/****************************************************************************
*
*  lib\rx_values.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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

#include "lib/rx_lib.h"

// rx_ser_lib
#include "lib/rx_ser_lib.h"
// rx_values
#include "lib/rx_values.h"



namespace rx {
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
string_type rx_get_value_type_name(rx_value_t type)
{
	switch (type)
	{
	case RX_NULL_TYPE:
		return "null"s;
	case RX_BOOL_TYPE:
		return "bit"s;
	case RX_INT8_TYPE:
		return "int8";
	case RX_UINT8_TYPE:
		return "uint8"s;
	case RX_INT16_TYPE:
		return "int16";
	case RX_UINT16_TYPE:
		return "uint16"s;
	case RX_INT32_TYPE:
		return "int32";
	case RX_UINT32_TYPE:
		return "uint32"s;
	case RX_INT64_TYPE:
		return "int64";
	case RX_UINT64_TYPE:
		return "uint64"s;
	case RX_FLOAT_TYPE:
		return "float32";
	case RX_DOUBLE_TYPE:
		return "float64"s;
	case RX_STRING_TYPE:
		return "string";
	case RX_TIME_TYPE:
		return "time";
	case RX_UUID_TYPE:
		return "uuid";
	case RX_BYTES_TYPE:
		return "bytes";
	case RX_NODE_ID_TYPE:
		return "nodeid";
	case RX_COMPLEX_TYPE:
		return "complex";
	default:
		RX_ASSERT(false);// shouldn't happened
		return "internal error unknown type!!!";
	}
}

rx_result rx_parse_value_type_name(const string_type& strtype, rx_value_t& type)
{
	if (strtype == "null")
	{
		type = RX_NULL_TYPE;
		return true;
	}
	else if (strtype == "bit")
	{
		type = RX_BOOL_TYPE;
		return true;
	}
	else if (strtype == "int8")
	{
		type = RX_INT8_TYPE;
		return true;
	}
	else if (strtype == "uint8")
	{
		type = RX_UINT8_TYPE;
		return true;
	}
	else if (strtype == "int16")
	{
		type = RX_INT16_TYPE;
		return true;
	}
	else if (strtype == "uint16")
	{
		type = RX_UINT16_TYPE;
		return true;
	}
	else if (strtype == "int32")
	{
		type = RX_INT32_TYPE;
		return true;
	}
	else if (strtype == "uint32")
	{
		type = RX_UINT32_TYPE;
		return true;
	}
	else if (strtype == "int64")
	{
		type = RX_INT64_TYPE;
		return true;
	}
	else if (strtype == "uint64")
	{
		type = RX_UINT64_TYPE;
		return true;
	}
	else if (strtype == "float32")
	{
		type = RX_FLOAT_TYPE;
		return true;
	}
	else if (strtype == "float64")
	{
		type = RX_DOUBLE_TYPE;
		return true;
	}
	else if (strtype == "string")
	{
		type = RX_STRING_TYPE;
		return true;
	}
	else if (strtype == "time")
	{
		type = RX_TIME_TYPE;
		return true;
	}
	else if (strtype == "uuid")
	{
		type = RX_UUID_TYPE;
		return true;
	}
	else if (strtype == "nodeid")
	{
		type = RX_NODE_ID_TYPE;
		return true;
	}
	else if (strtype == "complex")
	{
		type = RX_COMPLEX_TYPE;
		return true;
	}
	else if (strtype == "bytes")
	{
		type = RX_BYTES_TYPE;
		return true;
	}
	else
	{
		return strtype + " is unknown value type!";
	}
}


namespace values {
namespace
{
void copy_string_value(string_value_struct& dest, const string_value_struct& src)
{
	dest = src;
	if (dest.size > 0)
	{
		dest.value = new char[dest.size + 1];
		memcpy(dest.value, src.value, dest.size + 1);
	}
	else
	{
		dest.value = nullptr;
	}
}
void assign_string_value(string_value_struct& dest, const char* src)
{
	size_t size = src != nullptr ? strlen(src) : 0;
	dest.size = size;
	if (size > 0)
	{
		dest.value = new char[size + 1];
		memcpy(dest.value, src, size + 1);
	}
	else
	{
		dest.value = nullptr;
	}
}

void copy_bytes_value(bytes_value_struct& dest, const bytes_value_struct& src)
{
	dest = src;
	if (dest.size > 0)
	{
		dest.value = new uint8_t[dest.size];
		memcpy(dest.value, src.value, dest.size);
	}
	else
	{
		dest.value = nullptr;
	}
}
}

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

double complex_value_struct::amplitude() const
{
	return sqrt(real*real + imag * imag);
}
string_type complex_value_struct::to_string() const
{
	std::ostringstream stream;
	stream << real << " + " << imag << "j";
	return stream.str();
}
bool complex_value_struct::parse_string(const string_type& str)
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

// Class rx::values::rx_value 

rx_value::rx_value (const rx_value_storage& storage)
	: default_time_compare_(time_compare_type::skip)
	, quality_(RX_DEFAULT_VALUE_QUALITY)
	, origin_(RX_DEFUALT_ORIGIN)
	, storage_(storage)
{
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

bool rx_value::is_array () const
{
  return storage_.is_array();

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

bool rx_value::serialize (const string_type& name, base_meta_writer& stream) const
{
	if (!stream.start_object(name.c_str()))
		return false;
	if (!storage_.serialize(stream))
		return false;
	if (!stream.write_time("ts", time_))
		return false;
	if (!stream.write_uint("quality", quality_))
		return false;
	if (!stream.write_uint("origin", origin_))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

bool rx_value::deserialize (const string_type& name, base_meta_reader& stream)
{
	if (!stream.start_object(name.c_str()))
		return false;
	if (!storage_.deserialize(stream))
		return false;
	if (!stream.read_time("ts", time_))
		return false;
	if (!stream.read_uint("quality", quality_))
		return false;
	if (!stream.read_uint("origin", origin_))
		return false;
	if (!stream.end_object())
		return false;
	return true;
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

rx_value rx_value::from_simple (const rx_simple_value& value, rx_time ts)
{
	rx_value ret;
	ret.storage_ = value.get_storage();
	ret.time_ = ts;
	return ret;
}

rx_value rx_value::from_simple (rx_simple_value&& value, rx_time ts)
{
	rx_value ret;
	ret.storage_ = value.move_storage();
	ret.time_ = ts;
	return ret;
}

rx_simple_value rx_value::to_simple () const
{
	return rx_simple_value(storage_);
}

bool rx_value::convert_to (rx_value_t type)
{
	return storage_.convert_to(type);
}

rx_value_t rx_value::get_type () const
{
	return storage_.get_value_type();
}

void rx_value::dump_to_stream (std::ostream& out) const
{
	out << storage_.to_string()
		<< " [" << time_.get_string() << "] ";
	string_type q;
	fill_quality_string(*this, q);
	out << q;
}

bool rx_value::is_null () const
{
  return storage_.is_null();

}

bool rx_value::is_complex () const
{
  return storage_.is_complex();

}

bool rx_value::is_numeric () const
{
  return storage_.is_numeric();

}

bool rx_value::is_integer () const
{
  return storage_.is_integer();

}

bool rx_value::is_float () const
{
  return storage_.is_float();

}

complex_value_struct rx_value::get_complex_value () const
{
  return storage_.get_complex_value();

}

double rx_value::get_float_value () const
{
  return storage_.get_float_value();

}

int64_t rx_value::get_integer_value (rx_value_t* min_type) const
{
  return storage_.get_integer_value();

}

bool rx_value::get_bool_value () const
{
  return storage_.get_bool_value();

}

bool rx_value::set_from_complex (const complex_value_struct& val, rx_value_t type)
{
  return storage_.set_from_complex(val,type);

}

bool rx_value::set_from_float (double val, rx_value_t type)
{
  return storage_.set_from_float(val,type);

}

bool rx_value::set_from_integer (int64_t val, rx_value_t type)
{
  return storage_.set_from_integer(val,type);

}

rx_value rx_value::operator + (const rx_value& right) const
{
	rx_value ret;
	ret.quality_ = RX_GOOD_QUALITY;
	ret.storage_ = storage_ + right.storage_;
	ret.handle_quality_after_arithmetic();
	return ret;
}

rx_value rx_value::operator - (const rx_value& right) const
{
	rx_value ret;
	ret.quality_ = RX_GOOD_QUALITY;
	ret.storage_ = storage_ - right.storage_;
	ret.handle_quality_after_arithmetic();
	return ret;
}

rx_value rx_value::operator * (const rx_value& right) const
{
	rx_value ret;
	ret.quality_ = quality_;
	ret.storage_ = storage_ * right.storage_;
	ret.handle_quality_after_arithmetic();
	return ret;
}

rx_value rx_value::operator / (const rx_value& right) const
{
	rx_value ret;
	ret.quality_ = RX_GOOD_QUALITY;
	ret.storage_ = storage_ / right.storage_;
	ret.handle_quality_after_arithmetic();
	return ret;
}

rx_value rx_value::operator % (const rx_value& right) const
{
	rx_value ret;
	ret.quality_ = RX_GOOD_QUALITY;
	ret.storage_ = storage_ % right.storage_;
	ret.handle_quality_after_arithmetic();
	return ret;
}

void rx_value::handle_quality_after_arithmetic ()
{
}

bool rx_value::is_dead () const
{
	return ((quality_ & RX_QUALITY_MASK) == RX_DEAD_QUALITY);
}

rx_value_storage&& rx_value::move_storage ()
{
	return std::move(storage_);
}

void rx_value::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	val = *this;
	if (mode.is_off())
		val.set_quality(RX_BAD_QUALITY_OFFLINE);
	if (mode.is_test())
		val.set_test();
}


rx_value::rx_value()
	: default_time_compare_(time_compare_type::skip)
	, quality_(RX_DEFAULT_VALUE_QUALITY)
	, origin_(RX_DEFUALT_ORIGIN)
{
	//rx_value temp;
	//temp.assign_static<bool>(false);
}
rx_value::rx_value(rx_value&& right) noexcept
	: storage_(std::move(right.storage_))
	, time_(std::move(right.time_))
	, default_time_compare_(time_compare_type::skip)
	, quality_(right.quality_)
	, origin_(right.origin_)
{
}

rx_value& rx_value::operator=(rx_value&& right) noexcept
{
	storage_ = std::move(right.storage_);
	time_ = std::move(right.time_);
	quality_ = right.quality_;
	origin_ = right.origin_;
	return *this;
}
rx_value::rx_value(const rx_value &right)
	: storage_(right.storage_)
	, time_(right.time_)
	, default_time_compare_(time_compare_type::skip)
	, quality_(right.quality_)
	, origin_(right.origin_)
{
}
rx_value & rx_value::operator=(const rx_value &right)
{
	storage_ = right.storage_;
	time_ = right.time_;
	quality_ = right.quality_;
	origin_ = right.origin_;
	return *this;
}

// Class rx::values::rx_simple_value 

rx_simple_value::rx_simple_value (const rx_value_storage& storage)
	: storage_(storage)
{
}


bool rx_simple_value::operator==(const rx_simple_value &right) const
{
	return storage_.exact_equality(right.storage_);
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
  return storage_.is_array();

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
	if (!storage_.serialize(writter))
		return false;
	if (!writter.end_object())
		return false;
	return true;
}

bool rx_simple_value::deserialize (const string_type& name, base_meta_reader& reader)
{
	if (!reader.start_object(name.c_str()))
		return false;
	if (!storage_.deserialize(reader))
		return false;
	if (!reader.end_object())
		return false;
	return true;
}

void rx_simple_value::dump_to_stream (std::ostream& out) const
{
	out << storage_.to_string();
}

void rx_simple_value::parse_from_stream (std::istream& in)
{
	string_type temp;
	in >> temp;
	storage_.parse(temp);
}

void rx_simple_value::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	val = rx_value::from_simple(*this, ts);
	if (mode.is_off())
		val.set_quality(RX_BAD_QUALITY_OFFLINE);
	else
		val.set_good_locally();
	if (mode.is_test())
		val.set_test();
}

bool rx_simple_value::convert_to (rx_value_t type)
{
	return storage_.convert_to(type);
}

rx_value_t rx_simple_value::get_type () const
{
	return storage_.get_value_type();
}

void rx_simple_value::parse (const string_type& str)
{
	storage_.parse(str);
}

bool rx_simple_value::is_null () const
{
  return storage_.is_null();

}

bool rx_simple_value::is_complex () const
{
  return storage_.is_complex();

}

bool rx_simple_value::is_numeric () const
{
  return storage_.is_numeric();

}

bool rx_simple_value::is_integer () const
{
  return storage_.is_integer();

}

bool rx_simple_value::is_float () const
{
  return storage_.is_float();

}

rx_value_storage&& rx_simple_value::move_storage ()
{
	return std::move(storage_);
}

double rx_simple_value::get_float_value () const
{
  return storage_.get_float_value();

}

bool rx_simple_value::set_from_float (double val, rx_value_t type)
{
  return storage_.set_from_float(val,type);

}

rx_simple_value::rx_simple_value(rx_simple_value&& right) noexcept
	: storage_(std::move(right.storage_))
{
}

rx_simple_value& rx_simple_value::operator=(rx_simple_value&& right) noexcept
{
	storage_ = std::move(right.storage_);
	return *this;
}
rx_simple_value::rx_simple_value(const rx_simple_value &right)
	: storage_(right.storage_)
{
}
rx_simple_value & rx_simple_value::operator=(const rx_simple_value &right)
{
	storage_ = right.storage_;
	return *this;
}
// Class rx::values::rx_value_storage 

rx_value_storage::rx_value_storage()
{
	value_.value_type = RX_NULL_TYPE;
}


rx_value_storage::~rx_value_storage()
{
	destroy_value(value_.value, value_.value_type);
}


bool rx_value_storage::operator==(const rx_value_storage &right) const
{
	if (is_simple_type(value_.value_type) && right.is_simple_type(right.value_.value_type))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() == right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() == right.get_float_value();
		}
		else
			return value_.value_type == right.value_.value_type;
	}
	else if (value_.value_type == right.value_.value_type)
	{
		if (value_.value_type == RX_STRING_TYPE)
		{
			if (value_.value.string_value.size == 0 && right.value_.value.string_value.size == 0)
				return true;
			else if (value_.value.string_value.size != 0 && right.value_.value.string_value.size != 0)
				return string_type(value_.value.string_value.value) == string_type(right.value_.value.string_value.value);
			else
				return false;
		}
		else if (value_.value_type == RX_BYTES_TYPE)
		{
			if (value_.value.bytes_value.size == right.value_.value.bytes_value.size)
				return memcmp(value_.value.bytes_value.value, right.value_.value.bytes_value.value, value_.value.bytes_value.size) == 0;
			else
				return false;
		}
		else if (value_.value_type == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			return memcpy(value_.value.uuid_value, right.value_.value.uuid_value, sizeof(rx_uuid_t)) == 0;
#else
            return memcpy(&value_.value.uuid_value, &right.value_.value.uuid_value, sizeof(rx_uuid_t)) == 0;
#endif
		}
	}
	return value_.value_type == right.value_.value_type;
}

bool rx_value_storage::operator!=(const rx_value_storage &right) const
{
	return !operator==(right);
}


bool rx_value_storage::operator<(const rx_value_storage &right) const
{
	if (is_simple_type(value_.value_type) && right.is_simple_type(right.value_.value_type))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() < right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() < right.get_float_value();
		}
		else
			return value_.value_type < right.value_.value_type;
	}
	else if (value_.value_type == right.value_.value_type)
	{
		if (value_.value_type == RX_STRING_TYPE)
		{

			if (value_.value.string_value.size == 0 && right.value_.value.string_value.size == 0)
				return false;
			else if (value_.value.string_value.size != 0 && right.value_.value.string_value.size != 0)
				return string_type(value_.value.string_value.value) < string_type(right.value_.value.string_value.value);
			else
				return true;
		}
		else if (value_.value_type == RX_BYTES_TYPE)
		{
			if (value_.value.bytes_value.size == right.value_.value.bytes_value.size)
				return memcmp(value_.value.bytes_value.value, right.value_.value.bytes_value.value, value_.value.bytes_value.size) < 0;
			else
				return value_.value.bytes_value.size < right.value_.value.bytes_value.size;
		}
		else if (value_.value_type == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			rx_uuid(*value_.value.uuid_value) < rx_uuid(*right.value_.value.uuid_value);
#else
			value_.value.uuid_value < right.value_.value.uuid_value;
#endif
		}
	}
	return value_.value_type < right.value_.value_type;
}

bool rx_value_storage::operator>(const rx_value_storage &right) const
{
	if (is_simple_type(value_.value_type) && right.is_simple_type(right.value_.value_type))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() > right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() > right.get_float_value();
		}
		else
			return value_.value_type > right.value_.value_type;
	}
	else if (value_.value_type == right.value_.value_type)
	{
		if (value_.value_type == RX_STRING_TYPE)
		{
			if (value_.value.string_value.size == 0 && right.value_.value.string_value.size == 0)
				return false;
			else if (value_.value.string_value.size != 0 && right.value_.value.string_value.size != 0)
				return string_type(value_.value.string_value.value) > string_type(right.value_.value.string_value.value);
			else
				return true;
		}
		else if (value_.value_type == RX_BYTES_TYPE)
		{
			if (value_.value.bytes_value.size == right.value_.value.bytes_value.size)
				return memcmp(value_.value.bytes_value.value, right.value_.value.bytes_value.value, value_.value.bytes_value.size) > 0;
			else
				return value_.value.bytes_value.size > right.value_.value.bytes_value.size;
		}
		else if (value_.value_type == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			rx_uuid(*value_.value.uuid_value) > rx_uuid(*right.value_.value.uuid_value);
#else
			value_.value.uuid_value > right.value_.value.uuid_value;
#endif
		}
	}
	return value_.value_type > right.value_.value_type;
}

bool rx_value_storage::operator<=(const rx_value_storage &right) const
{
	if (is_simple_type(value_.value_type) && right.is_simple_type(right.value_.value_type))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() <= right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() <= right.get_float_value();
		}
		else
			return value_.value_type <= right.value_.value_type;
	}
	else if (value_.value_type == right.value_.value_type)
	{
		if (value_.value_type == RX_STRING_TYPE)
		{
			if (value_.value.string_value.size == 0 && right.value_.value.string_value.size == 0)
				return true;
			else if (value_.value.string_value.size != 0 && right.value_.value.string_value.size != 0)
				return string_type(value_.value.string_value.value) <= string_type(right.value_.value.string_value.value);
			else
				return value_.value.string_value.size == 0 && right.value_.value.string_value.size != 0;
		}
		else if (value_.value_type == RX_BYTES_TYPE)
		{
			if (value_.value.bytes_value.size == right.value_.value.bytes_value.size)
				return memcmp(value_.value.bytes_value.value, right.value_.value.bytes_value.value, value_.value.bytes_value.size) <= 0;
			else
				return value_.value.bytes_value.size < right.value_.value.bytes_value.size;
		}
		else if (value_.value_type == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			rx_uuid(*value_.value.uuid_value) <= rx_uuid(*right.value_.value.uuid_value);
#else
			value_.value.uuid_value <= right.value_.value.uuid_value;
#endif
		}
	}
	return value_.value_type <= right.value_.value_type;
}

bool rx_value_storage::operator>=(const rx_value_storage &right) const
{

	if (is_simple_type(value_.value_type) && right.is_simple_type(right.value_.value_type))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() >= right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() >= right.get_float_value();
		}
		else
			return value_.value_type >= right.value_.value_type;
	}
	else if (value_.value_type == right.value_.value_type)
	{
		if (value_.value_type == RX_STRING_TYPE)
		{
			if (value_.value.string_value.size == 0 && right.value_.value.string_value.size == 0)
				return true;
			else if (value_.value.string_value.size != 0 && right.value_.value.string_value.size != 0)
				return string_type(value_.value.string_value.value) >= string_type(right.value_.value.string_value.value);
			else
				return value_.value.string_value.size != 0 && right.value_.value.string_value.size == 0;
		}
		else if (value_.value_type == RX_BYTES_TYPE)
		{
			if (value_.value.bytes_value.size == right.value_.value.bytes_value.size)
				return memcmp(value_.value.bytes_value.value, right.value_.value.bytes_value.value, value_.value.bytes_value.size) >= 0;
			else
				return value_.value.bytes_value.size > right.value_.value.bytes_value.size;
		}
		else if (value_.value_type == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			rx_uuid(*value_.value.uuid_value) >= rx_uuid(*right.value_.value.uuid_value);
#else
			value_.value.uuid_value >= right.value_.value.uuid_value;
#endif
		}
	}
	return value_.value_type >= right.value_.value_type;
}



bool rx_value_storage::serialize (base_meta_writer& writer) const
{
	if (!writer.write_value_type("type", value_.value_type))
		return false;
	if (!serialize_value(writer, value_.value, value_.value_type, "val"))
		return false;
	return true;
}

bool rx_value_storage::deserialize (base_meta_reader& reader)
{
	// first destroy eventual values already inside
	destroy_value(value_.value, value_.value_type);
	value_.value_type = RX_NULL_TYPE;
	if (!reader.read_value_type("type", value_.value_type))
		return false;
	if (!deserialize_value(reader, value_.value, value_.value_type))
		return false;
	return true;
}

string_type rx_value_storage::to_string (bool simple) const
{
	char buff[0x20];
	switch (value_.value_type)
	{
	case RX_NULL_TYPE:
		return "<null>";
	case RX_BOOL_TYPE:
		return (value_.value.bool_value ? "true" : "false");
	case RX_INT8_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.int8_value);
			if(ret.ec== std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if(!simple)
				ret_str += "b";
			return ret_str;
		}
	case RX_UINT8_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.uint8_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "ub";
			return ret_str;
		}
	case RX_INT16_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.int16_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "s";
			return ret_str;
		}
	case RX_UINT16_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.uint16_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "us";
			return ret_str;
		}
	case RX_INT32_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.int32_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "i";
			return ret_str;
		}
	case RX_UINT32_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.uint32_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "ui";
			return ret_str;
		}
	case RX_INT64_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.int64_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "l";
			return ret_str;
		}
	case RX_UINT64_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.uint64_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "ul";
			return ret_str;
		}
	case RX_FLOAT_TYPE:
		{
			/*auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.float_value, std::chars_format::general);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';*/
			sprintf(buff, "%g", (double)value_.value.float_value);
			string_type ret_str(buff);
			if (!simple)
				ret_str += "f";
			return ret_str;
		}
	case RX_DOUBLE_TYPE:
		{
			/*auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.value.double_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';*/
			sprintf(buff, "%g", value_.value.double_value);
			string_type ret_str(buff);
			if (!simple)
				ret_str += "d";
			return ret_str;
		}
	case RX_STRING_TYPE:
		return "\""s + string_type(value_.value.string_value.size > 0 ? value_.value.string_value.value : "") + "\"";
	case RX_UUID_TYPE:
		return rx_uuid(*value_.value.uuid_value).to_string();

	case RX_TIME_TYPE:
		{
			rx_time temp;
			temp = value_.value.time_value;
			return temp.get_string();
		}
	default:
		return "not valid jet";
	}
}

void rx_value_storage::parse (const string_type& str)
{
	if (str == "true")
		assign_static(true);
	else if (str == "false")
		assign_static(false);
	else
	{
		char* end_ptr = nullptr;
		int64_t num = strtoll(str.c_str(), &end_ptr, 10);
		if (end_ptr != nullptr && *end_ptr == '\0')
		{
			assign_static(num);
			return;
		}
		end_ptr = nullptr;
		double dbl_num = strtod(str.c_str(), &end_ptr);
		if (end_ptr != nullptr && *end_ptr == '\0')
		{
			assign_static(dbl_num);
			return;
		}
		assign_static(str.c_str());
	}
}

rx_value_t rx_value_storage::get_value_type () const
{
	return value_.value_type;
}

string_type rx_value_storage::get_type_string () const
{
	if (value_.value_type&RX_ARRAY_VALUE_MASK)
	{
		return rx_get_value_type_name(value_.value_type & RX_SIMPLE_VALUE_MASK) + "[]";
	}
	else
	{
		return rx_get_value_type_name(value_.value_type & RX_SIMPLE_VALUE_MASK);
	}
/*	switch (value_.value_type&RX_SIMPLE_VALUE_MASK)
	{
	case RX_NULL_TYPE:
		return "null"s;
	case RX_BOOL_TYPE:
		return "bit"s;
	case RX_INT8_TYPE:
		return "int8";
	case RX_UINT8_TYPE:
		return "uint8"s;
	case RX_INT16_TYPE:
		return "int16";
	case RX_UINT16_TYPE:
		return "uint16"s;
	case RX_INT32_TYPE:
		return "int32";
	case RX_UINT32_TYPE:
		return "uint32"s;
	case RX_INT64_TYPE:
		return "int64";
	case RX_UINT64_TYPE:
		return "uint64"s;
	case RX_FLOAT_TYPE:
		return "float32";
	case RX_DOUBLE_TYPE:
		return "float64"s;
	case RX_STRING_TYPE:
		return "string";
	case RX_TIME_TYPE:
		return "time";
	case RX_UUID_TYPE:
		return "uuid";
	case RX_BYTES_TYPE:
		return "bytes";
	case RX_NODE_ID_TYPE:
		return "nodeid";
	case RX_COMPLEX_TYPE:
		return "complex";
	case RX_CLASS_TYPE:
	{
		switch (value_.value_type&RX_DEFINITION_TYPE_MASK)
		{
		case RX_BINARY_VALUE:
			return "binary type definition";
		case RX_JSON_VALUE:
			return "json type definition";
		case RX_SCRIPT_VALUE:
			return "script type definition";
			break;
		default:
			RX_ASSERT(false);// shouldn't happened
			return "internal error unknown type!!!";
		}
	}
	break;
	case RX_OBJECT_TYPE:
	{
		switch (value_.value_type&RX_DEFINITION_TYPE_MASK)
		{
		case RX_BINARY_VALUE:
			return "binary object data";
		case RX_JSON_VALUE:
			return "json object data";
		case RX_SCRIPT_VALUE:
			return "script object data";
			break;
		default:
			RX_ASSERT(false);// shouldn't happened
			return "internal error unknown type!!!";
		}
	}
	break;
	default:
		RX_ASSERT(false);// shouldn't happened
		return "internal error unknown type!!!";
	}*/
}

bool rx_value_storage::expresion_equality (const rx_value_storage& right) const
{
	return false;
}

bool rx_value_storage::exact_equality (const rx_value_storage& right) const
{
	if (value_.value_type != right.value_.value_type)
		return false;
	return exact_equality(value_.value, right.value_.value, value_.value_type);
}

bool rx_value_storage::convert_to (rx_value_t type)
{
	if (value_.value_type == type)
		return true;// nothing to do
	if (type == RX_NULL_TYPE)
	{// clear value here
		destroy_value(value_.value, value_.value_type);
		value_.value_type = RX_NULL_TYPE;
		return true;
	}
	bool target_is_array = (type&RX_ARRAY_VALUE_MASK) == RX_ARRAY_VALUE_MASK;
	bool current_is_array = (value_.value_type&RX_ARRAY_VALUE_MASK) == RX_ARRAY_VALUE_MASK;
	if (!target_is_array && !current_is_array)
	{// normal values
		if (!convert_union(value_.value, value_.value_type, type))
			return false;
		value_.value_type = type;
		return true;
	}
	else if (target_is_array && current_is_array)
	{// both are arrays convert individual items
		for (size_t i =0; i<value_.value.array_value.size; i++)
		{
			if (!convert_union(value_.value.array_value.values[i], value_.value_type&RX_STRIP_ARRAY_MASK, type&RX_STRIP_ARRAY_MASK))
				return false;
		}
		value_.value_type = type;
		return true;
	}
	else if (!target_is_array && current_is_array)
	{// from array to simple value
		if (value_.value.array_value.size == 0)
		{// empty array only to null type, checked before
			return false;
		}
		if (!convert_union(value_.value.array_value.values[0], value_.value_type&RX_STRIP_ARRAY_MASK, type))
			return false;
		rx_value_union temp;
		assign_value(temp, value_.value.array_value.values[0], type);
		destroy_value(value_.value, type&RX_STRIP_ARRAY_MASK);
		assign_value(value_.value, std::move(temp), type);
		value_.value_type = type;
		return true;
	}
	else //(target_is_array && !current_is_array)
	{// from simple value to array
		if (!convert_union(value_.value, value_.value_type, type&RX_STRIP_ARRAY_MASK))
			return false;
		value_.value.array_value.values = new rx_value_union{ value_.value };
		value_.value.array_value.size = 1;
		value_.value_type = type;
		return true;
	}
}

bool rx_value_storage::is_complex () const
{
	return value_.value_type == RX_COMPLEX_TYPE;
}

bool rx_value_storage::is_numeric () const
{
	return (value_.value_type > RX_NULL_TYPE && value_.value_type <= RX_COMPLEX_TYPE);
}

bool rx_value_storage::is_integer () const
{
	return value_.value_type > RX_NULL_TYPE && value_.value_type <= RX_UINT64_TYPE;
}

bool rx_value_storage::is_float () const
{
	return value_.value_type == RX_FLOAT_TYPE || value_.value_type == RX_DOUBLE_TYPE || value_.value_type == RX_COMPLEX_TYPE;
}

bool rx_value_storage::is_null () const
{
	return value_.value_type == RX_NULL_TYPE;
}

complex_value_struct rx_value_storage::get_complex_value () const
{
	if (is_complex())
	{

#ifdef RX_VALUE_SIZE_16
		return value_.value.complex_value;
#else
		return *value_.value.complex_value;
#endif
	}
	complex_value_struct ret;
	ret.imag = 0;
	ret.real = 0;
	if (is_integer())
		ret.real = (double)get_int_value(value_.value_type, value_.value);
	else if (is_float())
		ret.real = get_float_value();
	return ret;
}

double rx_value_storage::get_float_value () const
{
	return get_float_value(value_.value_type, value_.value);
}

int64_t rx_value_storage::get_integer_value (rx_value_t* min_type) const
{
	return get_int_value(value_.value_type, value_.value);
}

bool rx_value_storage::get_bool_value () const
{
	return get_bool_value(value_.value_type, value_.value);
}

int64_t rx_value_storage::get_int_value (rx_value_t type, const rx_value_union& value, rx_value_t* min_type, size_t idx) const
{
	if (IS_ARRAY_VALUE(type))
	{
		if (idx < value.array_value.size)
		{
			return get_int_value(RX_SIMPLE_TYPE(type), value.array_value.values[idx]);
		}
		else
		{
			assert(false);
			return 0;
		}
	}
	else
	{
		switch (type)
		{
		case RX_NULL_TYPE:
			if (min_type)
				*min_type = RX_NULL_TYPE;
			return 0;
		case RX_BOOL_TYPE:
			if (min_type)
				*min_type = RX_BOOL_TYPE;
			return value.bool_value ? 1 : 0;
		case RX_INT8_TYPE:
			if (min_type)
				*min_type = RX_INT8_TYPE;
			return value.int8_value;
		case RX_UINT8_TYPE:
			if (min_type)
				*min_type = RX_UINT8_TYPE;
			return value.uint8_value;
		case RX_INT16_TYPE:
			if (min_type)
				*min_type = RX_INT16_TYPE;
			return value.int16_value;
		case RX_UINT16_TYPE:
			if (min_type)
				*min_type = RX_UINT16_TYPE;
			return value.uint16_value;
		case RX_INT32_TYPE:
			if (min_type)
				*min_type = RX_INT32_TYPE;
			return value.int32_value;
		case RX_UINT32_TYPE:
			if (min_type)
				*min_type = RX_UINT32_TYPE;
			return value.uint32_value;
		case RX_INT64_TYPE:
			if (min_type)
				*min_type = RX_INT64_TYPE;
			return value.int64_value;
		case RX_UINT64_TYPE:
			if (min_type)
				*min_type = RX_UINT64_TYPE;
			return value.uint64_value;
		case RX_DOUBLE_TYPE:
			if (min_type)
				*min_type = RX_INT64_TYPE;
			return (int64_t)value.double_value;
		case RX_COMPLEX_TYPE:
			if (min_type)
				*min_type = RX_INT64_TYPE;
			return (int64_t)value.complex_value->amplitude();
		case RX_FLOAT_TYPE:
			if (min_type)
				*min_type = RX_INT32_TYPE;
			return (int64_t)value.float_value;
		case RX_STRING_TYPE:
			if (min_type)
				*min_type = RX_UINT32_TYPE;
			return value.string_value.size;
		case RX_BYTES_TYPE:
			if (min_type)
				*min_type = RX_UINT32_TYPE;
			return value.bytes_value.size;
		case RX_UUID_TYPE:
			if (min_type)
				*min_type = RX_UINT32_TYPE;
			return 0;
		case RX_TIME_TYPE:
			if (min_type)
				*min_type = RX_UINT64_TYPE;
			return (rx_time(value.time_value).get_longlong_miliseconds());
		default:
			return 0;
		}
	}
}

double rx_value_storage::get_float_value (rx_value_t type, const rx_value_union& value, size_t idx) const
{
	if (IS_ARRAY_VALUE(type))
	{
		if (idx < value.array_value.size)
		{
			return get_float_value(RX_SIMPLE_TYPE(type), value.array_value.values[idx]);
		}
		else
		{
			assert(false);
			return 0;
		}
	}
	else
	{
		switch (type)
		{
		case RX_NULL_TYPE:
			return 0;
		case RX_BOOL_TYPE:
			return value.bool_value ? 1 : 0;
		case RX_INT8_TYPE:
			return value.int8_value;
		case RX_UINT8_TYPE:
			return value.uint8_value;
		case RX_INT16_TYPE:
			return value.int16_value;
		case RX_UINT16_TYPE:
			return value.uint16_value;
		case RX_INT32_TYPE:
			return value.int32_value;
		case RX_UINT32_TYPE:
			return value.uint32_value;
		case RX_INT64_TYPE:
			return (double)value.int64_value;
		case RX_UINT64_TYPE:
			return (double)value.uint64_value;
		case RX_DOUBLE_TYPE:
			return value.double_value;
		case RX_COMPLEX_TYPE:
			return value.complex_value->amplitude();
		case RX_FLOAT_TYPE:
			return value.float_value;
		case RX_STRING_TYPE:
			return (double)value.string_value.size;
		case RX_BYTES_TYPE:
			return (double)value.bytes_value.size;
		case RX_UUID_TYPE:
			return 0;
		case RX_TIME_TYPE:
			return (double)(rx_time(value.time_value).get_longlong_miliseconds());
		default:
			return 0;
		}
	}
}

bool rx_value_storage::is_array () const
{
	return IS_ARRAY_VALUE(value_.value_type);
}

bool rx_value_storage::get_bool_value (rx_value_t type, const rx_value_union& value, size_t idx) const
{
	if (IS_ARRAY_VALUE(type))
	{
		if (idx < value.array_value.size)
		{
			return get_bool_value(RX_SIMPLE_TYPE(type), value.array_value.values[idx]);
		}
		else
		{
			assert(false);
			return 0;
		}
	}
	else
	{
		switch (type)
		{
		case RX_NULL_TYPE:
			return 0;
		case RX_BOOL_TYPE:
			return value.bool_value;
		case RX_INT8_TYPE:
		case RX_UINT8_TYPE:
			return value.uint8_value != 0;
		case RX_INT16_TYPE:
		case RX_UINT16_TYPE:
			return value.uint16_value != 0;
		case RX_INT32_TYPE:
		case RX_UINT32_TYPE:
			return value.uint32_value != 0;
		case RX_INT64_TYPE:
		case RX_UINT64_TYPE:
			return value.uint64_value != 0;
		case RX_DOUBLE_TYPE:
			return (int64_t)value.double_value != 0;
		case RX_COMPLEX_TYPE:
			return (int64_t)value.complex_value->amplitude() != 0;;
		case RX_FLOAT_TYPE:
			return (int64_t)value.float_value != 0;;
		case RX_STRING_TYPE:
			return value.string_value.size > 0;
		case RX_BYTES_TYPE:
			return value.bytes_value.size > 0;
		case RX_UUID_TYPE:
			return false;
		case RX_TIME_TYPE:
			return value.time_value.t_value != 0;
		default:
			return false;
		}
	}
}

bool rx_value_storage::set_from_complex (const complex_value_struct& val, rx_value_t type)
{
	destroy_value(value_.value, value_.value_type);
	value_.value_type = type;
	return set_from_complex(val, type, value_.value);
}

bool rx_value_storage::set_from_float (double val, rx_value_t type)
{
	destroy_value(value_.value, value_.value_type);
	value_.value_type = type;
	return set_from_float(val, type, value_.value);
}

bool rx_value_storage::set_from_integer (int64_t val, rx_value_t type)
{
	destroy_value(value_.value, value_.value_type);
	value_.value_type = type;
	return set_from_integer(val, type, value_.value);
}

bool rx_value_storage::set_from_complex (const complex_value_struct& val, rx_value_t type, rx_value_union& where)
{
	switch (type)
	{
	case RX_BOOL_TYPE:
		where.bool_value = val.imag != 0 || val.real != 0;
		break;
	case RX_INT8_TYPE:
		where.int8_value = (int8_t)val.real;
		break;
	case RX_UINT8_TYPE:
		where.uint8_value = (uint8_t)val.real;
		break;
	case RX_INT16_TYPE:
		where.int16_value = (int16_t)val.real;
		break;
	case RX_UINT16_TYPE:
		where.uint16_value = (uint16_t)val.real;
		break;
	case RX_INT32_TYPE:
		where.int32_value = (int32_t)val.real;
		break;
	case RX_UINT32_TYPE:
		where.uint32_value = (uint32_t)val.real;
		break;
	case RX_INT64_TYPE:
		where.int64_value = (int64_t)val.real;
		break;
	case RX_UINT64_TYPE:
		where.uint64_value = (uint64_t)val.real;
		break;
	case RX_DOUBLE_TYPE:
		where.double_value = (double)val.real;
		break;
	case RX_COMPLEX_TYPE:
#ifdef RX_VALUE_SIZE_16
		where.complex_value = val;
#else
		where.complex_value = new complex_value_struct(val);
#endif
		break;
	case RX_FLOAT_TYPE:
		where.float_value = (float)val.real;
		break;
	case RX_STRING_TYPE:
		{
			char buff[0x100];
			snprintf(buff, 0x100, "%.15g+%.15gi", val.real, val.imag);
			assign_string_value(where.string_value, buff);
		}
		break;
	case RX_BYTES_TYPE:
		where.bytes_value.size = 0;
		return false;
		break;
	case RX_UUID_TYPE:
#ifdef RX_VALUE_SIZE_16
		where.uuid_value = rx_uuid::null_uuid().uuid();
#else
		where.uuid_value = new rx_uuid_t(rx_uuid::null_uuid().uuid());
#endif
		return false;
		break;
	case RX_TIME_TYPE:
		where.time_value = rx_time((uint64_t)val.real);
		break;
	}
	return true;
}

bool rx_value_storage::set_from_float (double val, rx_value_t type, rx_value_union& where)
{
	switch (type)
	{
	case RX_BOOL_TYPE:
		where.bool_value = val != 0;
		break;
	case RX_INT8_TYPE:
		where.int8_value = (int8_t)val;
		break;
	case RX_UINT8_TYPE:
		where.uint8_value = (uint8_t)val;
		break;
	case RX_INT16_TYPE:
		where.int16_value = (int16_t)val;
		break;
	case RX_UINT16_TYPE:
		where.uint16_value = (uint16_t)val;
		break;
	case RX_INT32_TYPE:
		where.int32_value = (int32_t)val;
		break;
	case RX_UINT32_TYPE:
		where.uint32_value = (uint32_t)val;
		break;
	case RX_INT64_TYPE:
		where.int64_value = (int64_t)val;
		break;
	case RX_UINT64_TYPE:
		where.uint64_value = (uint64_t)val;
		break;
	case RX_DOUBLE_TYPE:
		where.double_value = (double)val;
		break;
	case RX_COMPLEX_TYPE:
#ifdef RX_VALUE_SIZE_16
		where.complex_value = complex_value_struct{ val, 0 };
#else
		where.complex_value = new complex_value_struct{ val, 0 };
#endif
		break;
	case RX_FLOAT_TYPE:
		where.float_value = (float)val;
		break;
	case RX_STRING_TYPE:
		{
			char buff[0x20];
			snprintf(buff, 0x20, "%.15g", val);
			assign_string_value(where.string_value, buff);
		}
		break;
	case RX_BYTES_TYPE:
		where.bytes_value.size = 0;
		return false;
		break;
	case RX_UUID_TYPE:
#ifdef RX_VALUE_SIZE_16
		where.uuid_value = rx_uuid::null_uuid();
#else
		where.uuid_value = new rx_uuid_t(rx_uuid::null_uuid().uuid());
#endif
		return false;
		break;
	case RX_TIME_TYPE:
		where.time_value = rx_time((uint64_t)val);
		break;
	}
	return true;
}

bool rx_value_storage::set_from_integer (int64_t val, rx_value_t type, rx_value_union& where)
{
	switch (type)
	{
	case RX_BOOL_TYPE:
		where.bool_value = val != 0;
		break;
	case RX_INT8_TYPE:
		where.int8_value = (int8_t)val;
		break;
	case RX_UINT8_TYPE:
		where.uint8_value = (uint8_t)val;
		break;
	case RX_INT16_TYPE:
		where.int16_value = (int16_t)val;
		break;
	case RX_UINT16_TYPE:
		where.uint16_value = (uint16_t)val;
		break;
	case RX_INT32_TYPE:
		where.int32_value = (int32_t)val;
		break;
	case RX_UINT32_TYPE:
		where.uint32_value = (uint32_t)val;
		break;
	case RX_INT64_TYPE:
		where.int64_value = val;
		break;
	case RX_UINT64_TYPE:
		where.uint64_value = (uint64_t)val;
		break;
	case RX_DOUBLE_TYPE:
		where.double_value = (double)val;
		break;
	case RX_COMPLEX_TYPE:
#ifdef RX_VALUE_SIZE_16
		where.complex_value = complex_value_struct{ (double)val, 0 };
#else
		where.complex_value = new complex_value_struct{ (double)val, 0 };
#endif
		break;
	case RX_FLOAT_TYPE:
		where.float_value = (float)val;
		break;
	case RX_STRING_TYPE:
		{
			char buff[0x20];
			snprintf(buff, 0x20, "%d", (int)val);
			assign_string_value(where.string_value, buff);
		}
		break;
	case RX_BYTES_TYPE:
		where.bytes_value.size = 0;
		return false;
		break;
	case RX_UUID_TYPE:
#ifdef RX_VALUE_SIZE_16
		where.uuid_value = rx_uuid::null_uuid();
#else
		where.uuid_value = new rx_uuid_t(rx_uuid::null_uuid().uuid());
#endif
		return false;
		break;
	case RX_TIME_TYPE:
		where.time_value = rx_time((uint64_t)val);
		break;
	}
	return true;
}

bool rx_value_storage::is_simple_type (rx_value_t type) const
{
	return !IS_ARRAY_VALUE(type) && type < RX_TIME_TYPE && type != RX_STRING_TYPE;
}

rx_value_storage rx_value_storage::operator + (const rx_value_storage& right) const
{
	rx_value_storage result;
	rx_value_t ret_type = get_arithmetic_result_type(value_.value_type, right.value_.value_type, true);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type > RX_NULL_TYPE && ret_type <= RX_UINT64_TYPE)
		{// integers stuff
			result.set_from_integer(get_integer_value() + right.get_integer_value(), ret_type);
		}
		else if (ret_type == RX_FLOAT_TYPE || ret_type == RX_DOUBLE_TYPE)
		{// floating point stuff
			result.set_from_float(get_float_value() + right.get_float_value(), ret_type);
		}
		else if (ret_type == RX_COMPLEX_TYPE)
		{
			complex_value_struct one = get_complex_value();
			complex_value_struct two = right.get_complex_value();
			one.imag = one.imag + two.imag;
			one.real = one.real + two.real;
			result.set_from_complex(one, RX_COMPLEX_TYPE);
		}
		else if (ret_type == RX_TIME_TYPE)
		{// time
			result.value_.value_type = RX_TIME_TYPE;
			result.value_.value.time_value = rx_time(value_.value.time_value) + right.get_integer_value();
		}
		else if (ret_type == RX_BYTES_TYPE && value_.value_type == RX_BYTES_TYPE && right.value_.value_type == RX_BYTES_TYPE)
		{
			result.value_.value_type = RX_BYTES_TYPE;
			size_t size1 = value_.value.bytes_value.value != nullptr ? value_.value.bytes_value.size : 0;

			size_t size2 = right.value_.value.bytes_value.value != nullptr ? right.value_.value.bytes_value.size : 0;
			size_t new_size = size1 + size2;
			result.value_.value.bytes_value.size = new_size;
			uint8_t* temp_buff = nullptr;
			if (new_size)
			{
				temp_buff = new uint8_t[new_size];
				if (size1 > 0)
					memcpy(temp_buff, value_.value.bytes_value.value, size1);

				if (size2 > 0)
					memcpy(&temp_buff[size1], right.value_.value.bytes_value.value, size2);
			}
			result.value_.value.bytes_value.value = temp_buff;
		}
		else if (ret_type == RX_STRING_TYPE && value_.value_type == RX_STRING_TYPE)
		{
			result.value_.value_type = RX_STRING_TYPE;
			size_t size1 = value_.value.string_value.value != nullptr ? value_.value.string_value.size : 0;
			if (right.value_.value_type == RX_STRING_TYPE)
			{// do dynamic stuff
				size_t size2 = right.value_.value.string_value.value != nullptr ? right.value_.value.string_value.size : 0;
				size_t new_size = size1 + size2;
				char* temp_buff = nullptr;
				result.value_.value.string_value.size = new_size;
				if (new_size)
				{
					temp_buff = new char[new_size + 1];
					if (size1 > 0)
						memcpy(temp_buff, value_.value.string_value.value, size1 + 1);
					else
						result.value_.value.string_value.value[0] = '\0';

					if (size2 > 0)
						strcat(temp_buff, right.value_.value.string_value.value);
				}
				result.value_.value.string_value.value = temp_buff;				
			}
			else
			{
				std::string right_str = right.to_string();
				size_t size2 = right_str.size();
				size_t new_size = size1 + size2;
				result.value_.value.string_value.size = new_size;
				if (new_size)
				{
					result.value_.value.string_value.value = new char[new_size + 1];
					if (size1 > 0)
						memcpy(result.value_.value.string_value.value, value_.value.string_value.value, size1 + 1);
					else
						result.value_.value.string_value.value[0] = '\0';

					if (size2 > 0)
						strcat(result.value_.value.string_value.value, right_str.c_str());
				}
				else
				{
					result.value_.value.string_value.value = nullptr;
				}
			}
		}

	}
	return result;
}

rx_value_t rx_value_storage::get_arithmetic_result_type (rx_value_t left, rx_value_t right, bool add) const
{
	if (left > RX_NULL_TYPE && left <= RX_UINT64_TYPE
		&& right > RX_NULL_TYPE && right <= RX_UINT64_TYPE)
	{// integer values
		return std::max(left, right);// just return max
	}
	else if (left > RX_NULL_TYPE && left <= RX_DOUBLE_TYPE
		&& right > RX_NULL_TYPE && right <= RX_DOUBLE_TYPE)
	{// has floating point values
		if (left == RX_DOUBLE_TYPE || right == RX_DOUBLE_TYPE)
			return RX_DOUBLE_TYPE;
		else
			return RX_FLOAT_TYPE;
	}
	else if (left == RX_COMPLEX_TYPE || right == RX_COMPLEX_TYPE)
	{
		return RX_COMPLEX_TYPE;
	}
	else if (left == RX_TIME_TYPE && right > RX_NULL_TYPE && right <= RX_DOUBLE_TYPE)
	{// do time and numeric
		return RX_TIME_TYPE;
	}
	else if (add)
	{
		if (left == RX_STRING_TYPE)
		{// add string and any other
			return RX_STRING_TYPE;
		}
		else if (left == RX_BYTES_TYPE && right == RX_BYTES_TYPE)
		{// add two byte arrays
			return RX_BYTES_TYPE;
		}
	}
	return RX_NULL_TYPE;
}

rx_value_storage rx_value_storage::operator - (const rx_value_storage& right) const
{
	rx_value_storage result;
	rx_value_t ret_type = get_arithmetic_result_type(value_.value_type, right.value_.value_type, false);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type > RX_NULL_TYPE && ret_type <= RX_UINT64_TYPE)
		{// integers stuff
			result.set_from_integer(get_integer_value() - right.get_integer_value(), ret_type);
		}
		else if (ret_type == RX_FLOAT_TYPE || ret_type == RX_DOUBLE_TYPE)
		{// floating point stuff
			result.set_from_float(get_float_value() - right.get_float_value(), ret_type);
		}
		else if (ret_type == RX_COMPLEX_TYPE)
		{
			complex_value_struct one = get_complex_value();
			complex_value_struct two = right.get_complex_value();
			one.imag = one.imag - two.imag;
			one.real = one.real - two.real;
			result.set_from_complex(one, RX_COMPLEX_TYPE);
		}
		else if (ret_type == RX_TIME_TYPE)
		{// time
			result.value_.value_type = RX_TIME_TYPE;
			result.value_.value.time_value = rx_time(value_.value.time_value) - right.get_integer_value();
		}
	}
	return result;
}

rx_value_storage rx_value_storage::operator * (const rx_value_storage& right) const
{
	rx_value_storage result;
	rx_value_t ret_type = get_arithmetic_result_type(value_.value_type, right.value_.value_type, false);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type > RX_NULL_TYPE && ret_type <= RX_UINT64_TYPE)
		{// integers stuff
			result.set_from_integer(get_integer_value() * right.get_integer_value(), ret_type);
		}
		else if (ret_type == RX_FLOAT_TYPE || ret_type == RX_DOUBLE_TYPE)
		{// floating point stuff
			result.set_from_float(get_float_value() * right.get_float_value(), ret_type);
		}
		else if (ret_type == RX_COMPLEX_TYPE)
		{
			complex_value_struct one = get_complex_value();
			complex_value_struct two = right.get_complex_value();
			complex_type z1{ one.real, one.imag };
			complex_type z2{ two.real, two.imag };
			z1 = z1 * z2;
			result.set_from_complex(complex_value_struct{ z1.real(), z1.imag() }, RX_COMPLEX_TYPE);
		}
	}
	return result;
}

rx_value_storage rx_value_storage::operator / (const rx_value_storage& right) const
{
	rx_value_storage result;
	rx_value_t ret_type = get_arithmetic_result_type(value_.value_type, right.value_.value_type, false);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type > RX_NULL_TYPE && ret_type <= RX_UINT64_TYPE)
		{// integers stuff
			auto temp = right.get_integer_value();
			if (temp != 0)
				result.set_from_integer(get_integer_value() / temp, ret_type);
			else
				throw std::runtime_error("Division by zero!");
		}
		else if (ret_type == RX_FLOAT_TYPE || ret_type == RX_DOUBLE_TYPE)
		{// floating point stuff
			auto temp = right.get_float_value();
			if (temp != 0)
				result.set_from_float(get_float_value() /temp , ret_type);
			else
				throw std::runtime_error("Division by zero!");
		}
		else if (ret_type == RX_COMPLEX_TYPE)
		{
			complex_value_struct one = get_complex_value();
			complex_value_struct two = right.get_complex_value();
			complex_type z1{ one.real, one.imag };
			complex_type z2{ two.real, two.imag };
			z1 = z1 / z2;
			result.set_from_complex(complex_value_struct{ z1.real(), z1.imag() }, RX_COMPLEX_TYPE);
		}
	}
	return result;
}

rx_value_storage rx_value_storage::operator % (const rx_value_storage& right) const
{
	rx_value_storage result;
	rx_value_t ret_type = get_arithmetic_result_type(value_.value_type, right.value_.value_type, false);
	if (ret_type != RX_NULL_TYPE)
	{
		if (ret_type > RX_NULL_TYPE && ret_type <= RX_UINT64_TYPE)
		{// integers stuff
			auto temp = right.get_integer_value();
			if (temp != 0)
				result.set_from_integer(get_integer_value() / temp, ret_type);
		}
	}
	return result;
}

rx_node_id rx_value_storage::get_id_value () const
{
	if (value_.value_type == RX_NODE_ID_TYPE)
	{
		return *value_.value.node_id_value;
	}
	else
	{
		return rx_node_id();
	}
}

string_type rx_value_storage::get_string_value () const
{
	if (value_.value_type == RX_STRING_TYPE)
	{
		if (value_.value.string_value.size == 0)
			return string_type();
		else
			return value_.value.string_value.value;
	}
	else
	{
		rx_value_storage conv(*this);
		if (conv.convert_to(RX_STRING_TYPE))
		{
			return conv.get_string_value();
		}
		else
		{
			return string_type();
		}
	}
}

bool rx_value_storage::weak_serialize_value (const string_type& name, base_meta_writer& writer) const
{
	if (!serialize_value(writer, value_.value, value_.value_type, name))
		return false;
	return true;
}

bool rx_value_storage::weak_deserialize_value (const string_type& name, base_meta_reader& reader)
{
	return false;
}

void rx_value_storage::assign(bool val)
{
	value_.value.bool_value = val;
}
void rx_value_storage::assign(int8_t val)
{
	value_.value.int8_value = val;
}
void rx_value_storage::assign(uint8_t val)
{
	value_.value.uint8_value = val;
}
void rx_value_storage::assign(int16_t val)
{
	value_.value.int16_value = val;
}
void rx_value_storage::assign(uint16_t val)
{
	value_.value.uint16_value = val;
}
void rx_value_storage::assign(int32_t val)
{
	value_.value.int32_value = val;
}
void rx_value_storage::assign(uint32_t val)
{
	value_.value.uint32_value = val;
}
void rx_value_storage::assign(int64_t val)
{
	value_.value.int64_value = val;
}
void rx_value_storage::assign(uint64_t val)
{
	value_.value.uint64_value = val;
}
void rx_value_storage::assign(float val)
{
	value_.value.float_value = val;
}
void rx_value_storage::assign(double val)
{
	value_.value.double_value = val;
}
void rx_value_storage::assign(std::complex<double> val)
{
#ifdef RX_SIZE_16
	value_.value.complex_value = complex_value_struct{ val.real(), val.imag() };
#else
	value_.value.complex_value = new complex_value_struct{ val.real(), val.imag() };
#endif
}
void rx_value_storage::assign(rx_time val)
{
	value_.value.time_value = val;
}
void rx_value_storage::assign(rx_uuid val)
{
#ifdef RX_SIZE_16
	uuid_value = val.uuid();
#else
	value_.value.uuid_value = new rx_uuid_t(val.uuid());
#endif
}
void rx_value_storage::assign(const char* val)
{
	assign_string_value(value_.value.string_value, val);
}
void rx_value_storage::assign(uint8_t* val, size_t size)
{
	value_.value.bytes_value.size = size;
	if (size > 0)
	{
		value_.value.bytes_value.value = new uint8_t[size];
		memcpy(value_.value.bytes_value.value, val, size);
	}
}
void rx_value_storage::assign(const rx_node_id& val)
{
	value_.value.node_id_value = new rx_node_id(val);
}
void rx_value_storage::assign(rx_node_id&& val)
{
	value_.value.node_id_value = new rx_node_id(std::move(val));
}
void rx_value_storage::assign(size_t size, rx_value_union* val)
{
	value_.value.array_value.size = size;
	if (size > 0)
	{
		value_.value.array_value.values = new rx_value_union[size];
		memcpy(value_.value.array_value.values, val, sizeof(rx_value_union) * size);
	}
	else
	{
		value_.value.array_value.values = nullptr;
	}
}
void rx_value_storage::assign_value(rx_value_union& left, const rx_value_union& right, rx_value_t type)
{
	// now do the actual copy of pointer members
	if (type&RX_ARRAY_VALUE_MASK)
	{// we have an array
		size_t count = left.array_value.size;
		left.array_value = right.array_value;
		if (count > 0)
		{
			left.array_value.values = new rx_value_union[count];
			for (size_t i = 0; i < count; i++)
			{
				assign_value(left.array_value.values[i], right.array_value.values[i], RX_STRIP_ARRAY_MASK & type);
			}
		}
	}
	else
	{//simple union stuff
		switch (type&RX_SIMPLE_VALUE_MASK)
		{
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
			left.complex_value = new complex_value_struct(*right.complex_value);
			break;
		case RX_UUID_TYPE:
			left.uuid_value = new rx_uuid_t(*right.uuid_value);
			break;
#endif
		case RX_STRING_TYPE:
			copy_string_value(left.string_value, right.string_value);
			break;
		case RX_BYTES_TYPE:
			copy_bytes_value(left.bytes_value, right.bytes_value);
			break;
		case RX_NODE_ID_TYPE:
			left.node_id_value = new rx_node_id(*right.node_id_value);
			break;
		break;
		default:
			// others are just plain copy
			memcpy(&left, &right, sizeof(rx_value_union));
		}
	}
}
void rx_value_storage::assign_value(rx_value_union& left, rx_value_union&& right, rx_value_t type)
{
	// memcpy is enough this is move
	memcpy(&left, &right, sizeof(rx_value_union));
}
void rx_value_storage::destroy_value(rx_value_union& who, rx_value_t type)
{
	if (type == RX_NULL_TYPE)
		return;

	if (type&RX_ARRAY_VALUE_MASK)
	{// we have an array
		if (who.array_value.size > 0)
		{
			for (size_t i = 0; i < who.array_value.size; i++)
			{
				destroy_value(who.array_value.values[i], RX_STRIP_ARRAY_MASK & type);
			}
			delete[] who.array_value.values;
		}
	}
	else
	{//simple union stuff
		switch (type&RX_SIMPLE_VALUE_MASK)
		{
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
			delete who.complex_value;
			break;
		case RX_UUID_TYPE:
			delete who.uuid_value;
			break;
#endif
		case RX_STRING_TYPE:
			if (who.string_value.size > 0)
				delete[] who.string_value.value;
			break;
		case RX_BYTES_TYPE:
			if (who.bytes_value.size > 0)
				delete[] who.bytes_value.value;
			break;
		case RX_NODE_ID_TYPE:
			delete who.node_id_value;
			break;
		}
	}
}
bool rx_value_storage::exact_equality(const rx_value_union& left, const rx_value_union& right, rx_value_t type)
{
	return true;
}
bool rx_value_storage::serialize_value(base_meta_writer& writer, const rx_value_union& who, rx_value_t type, const string_type& name)
{
	if (type&RX_ARRAY_VALUE_MASK)
	{// array of values
		writer.start_array(name.c_str(), who.array_value.size);
		for (size_t i=0; i< who.array_value.size; i++)
		{
			writer.start_object("item");
			serialize_value(writer, who.array_value.values[i], type&RX_STRIP_ARRAY_MASK, "val");
			writer.end_object();
		}
		writer.end_array();
	}
	else
	{
		switch (type&RX_SIMPLE_VALUE_MASK)
		{
		case RX_NULL_TYPE:
			break;
		case RX_BOOL_TYPE:
			writer.write_bool(name.c_str(), who.bool_value);
			break;
		case RX_INT8_TYPE:
			writer.write_byte(name.c_str(), who.int8_value);
			break;
		case RX_UINT8_TYPE:
			writer.write_byte(name.c_str(), who.uint8_value);
			break;
		case RX_INT16_TYPE:
			writer.write_int(name.c_str(), who.int16_value);
			break;
		case RX_UINT16_TYPE:
			writer.write_uint(name.c_str(), who.uint16_value);
			break;
		case RX_INT32_TYPE:
			writer.write_int(name.c_str(), who.int32_value);
			break;
		case RX_UINT32_TYPE:
			writer.write_uint(name.c_str(), who.uint32_value);
			break;
		case RX_INT64_TYPE:
			writer.write_int64(name.c_str(), who.int64_value);
			break;
		case RX_UINT64_TYPE:
			writer.write_uint64(name.c_str(), who.uint64_value);
			break;
		case RX_FLOAT_TYPE:
			writer.write_double(name.c_str(), who.float_value);
			break;
		case RX_DOUBLE_TYPE:
			writer.write_double(name.c_str(), who.double_value);
			break;
		case RX_STRING_TYPE:
			writer.write_string(name.c_str(), who.string_value.size > 0 ? who.string_value.value : "");
			break;
		case RX_TIME_TYPE:
			writer.write_time(name.c_str(), who.time_value);
			break;
		case RX_UUID_TYPE:
			writer.write_uuid(name.c_str(), *who.uuid_value);
			break;
		case RX_BYTES_TYPE:
			writer.write_bytes(name.c_str(), who.bytes_value.value,who.bytes_value.size);
			break;
		case RX_COMPLEX_TYPE:
		{
			writer.start_object(name.c_str());
			writer.write_double("Real", who.complex_value->real);
			writer.write_double("Imag", who.complex_value->imag);
			writer.end_object();
		}
		break;
		case RX_NODE_ID_TYPE:
			writer.write_id(name.c_str(), *who.node_id_value);
			break;
		}
	}
	return true;
}
bool rx_value_storage::deserialize_value(base_meta_reader& reader, rx_value_union& who, rx_value_t type)
{
	if (RX_ARRAY_VALUE_MASK&type)
	{
		std::vector<rx_value_union> temp_array;
		if (!reader.start_array("Vals"))
			return false;
		while(!reader.array_end())
		{
			rx_value_union temp;
			if (!deserialize_value(reader, temp, type&RX_STRIP_ARRAY_MASK))
				return false;
			temp_array.emplace_back(temp);
		}
		who.array_value.size = temp_array.size();
		if (!temp_array.empty())
		{
			who.array_value.values = new rx_value_union[who.array_value.size];
			memcpy(who.array_value.values, &temp_array[0], sizeof(rx_value_union)* who.array_value.size);
		}
		else
		{
			who.array_value.values = nullptr;
		}
	}
	else
	{
		switch (type&RX_SIMPLE_VALUE_MASK)
		{
		case RX_NULL_TYPE:
			break;
		case RX_BOOL_TYPE:
			return reader.read_bool("val", who.bool_value);
		case RX_INT8_TYPE:
		{
			uint8_t temp;
			if (!reader.read_byte("val", temp))
				return false;
			who.int8_value = temp;
			return true;
		}
		case RX_UINT8_TYPE:
			return reader.read_byte("val", who.uint8_value);
		case RX_INT16_TYPE:
		{
			int32_t temp;
			if (!reader.read_int("val", temp))
				return false;
			who.int16_value = temp;
			return true;
		}
		case RX_UINT16_TYPE:
		{
			uint32_t temp;
			if(!reader.read_uint("val", temp))
				return false;
			who.uint16_value = temp;
			return true;
		}
		case RX_INT32_TYPE:
			return reader.read_int("val", who.int32_value);
		case RX_UINT32_TYPE:
			return reader.read_uint("val", who.uint32_value);
		case RX_INT64_TYPE:
			return reader.read_int64("val", who.int64_value);
		case RX_UINT64_TYPE:
			return reader.read_uint64("val", who.uint64_value);
		case RX_FLOAT_TYPE:
		{
			double temp;
			if(!reader.read_double("val", temp))
				return false;
			who.float_value = static_cast<float>(temp);
			return true;
		}
		case RX_DOUBLE_TYPE:
			return reader.read_double("val", who.double_value);
		case RX_NODE_ID_TYPE:
			{
				rx_node_id val;
				if (!reader.read_id("val", val))
					return false;
				who.node_id_value = new rx_node_id(std::move(val));
				return true;
			}
		case RX_STRING_TYPE:
		{
			string_type val;
			if(!reader.read_string("val", val))
				return false;
			assign_string_value(who.string_value, val.c_str());
			return true;
		}
		case RX_TIME_TYPE:
			return reader.read_time("val", who.time_value);
		default:
			RX_ASSERT(false);
			// shouldn't happened
		}
	}
	return true;
}

bool rx_value_storage::convert_union(rx_value_union& what, rx_value_t source, rx_value_t target)
{
	rx_value_t simple = target & RX_SIMPLE_VALUE_MASK;
	target = simple;
	switch (target)
	{
	case RX_BOOL_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			return true;
		case RX_INT8_TYPE:
			what.bool_value = what.int8_value != 0;
			return true;
		case RX_INT16_TYPE:
			what.bool_value = what.int16_value != 0;
			return true;
		case RX_INT32_TYPE:
			what.bool_value = what.int32_value != 0;
			return true;
		case RX_INT64_TYPE:
			what.bool_value = what.int64_value != 0;
			return true;
		case RX_UINT8_TYPE:
			what.bool_value = what.uint8_value != 0;
			return true;
		case RX_UINT16_TYPE:
			what.bool_value = what.uint16_value != 0;
			return true;
		case RX_UINT32_TYPE:
			what.bool_value = what.uint32_value != 0;
			return true;
		case RX_UINT64_TYPE:
			what.bool_value = what.uint64_value != 0;
			return true;
		case RX_FLOAT_TYPE:
			what.bool_value = what.float_value != 0;
			return true;
		case RX_DOUBLE_TYPE:
			what.bool_value = what.double_value != 0;
			return true;
		case RX_TIME_TYPE:
			what.bool_value = what.time_value.t_value != 0;
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			bool temp = what.complex_value->real != 0 || what.complex_value->imag != 0;
			delete what.complex_value;
			what.bool_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
		{
			bool temp = !rx_uuid(*what.uuid_value).is_null();
			delete what.uuid_value;
			what.bool_value = temp;
			return true;
		}
#else
		case RX_COMPLEX_TYPE:
		{
			bool temp = what.complex_value.real != 0 || what.complex_value.imag != 0;
			what.bool_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
		{
			bool temp = !what.uuid_value.is_null();
			what.bool_value = temp;
			return true;
		}
#endif
		case RX_STRING_TYPE:
		{
			bool temp = what.string_value.size > 0;
			if (temp)
				delete[] what.string_value.value;
			what.bool_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
		{
			bool temp = what.bytes_value.size > 0;
			if (temp)
				delete[] what.bytes_value.value;
			what.bool_value = temp;
			return true;
		}
		case RX_NODE_ID_TYPE:
		{
			bool temp = !what.node_id_value->is_null();
			delete what.node_id_value;
			what.bool_value = temp;
			return true;
		}
		}
	}
	break;
	case RX_INT8_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.int8_value = what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			return true;
		case RX_INT16_TYPE:
			what.int8_value = (int8_t)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			what.int8_value = (int8_t)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			what.int8_value = (int8_t)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			what.int8_value = (int8_t)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			what.int8_value = (int8_t)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			what.int8_value = (int8_t)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			what.int8_value = (int8_t)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			what.int8_value = (int8_t)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			what.int8_value = (int8_t)what.double_value;
			return true;
		case RX_TIME_TYPE:
			what.int8_value = (int8_t)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = (int8_t)what.complex_value->amplitude();
			delete what.complex_value;
			what.int8_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (int8_t)what.complex_value->amplitude();
			what.int8_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			int8_t temp = 0;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = (int8_t)std::stoi(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.int8_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_INT16_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.int16_value = what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			what.int16_value = (int16_t)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			return true;
		case RX_INT32_TYPE:
			what.int16_value = (int16_t)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			what.int16_value = (int16_t)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			what.int16_value = (int16_t)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			what.int16_value = (int16_t)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			what.int16_value = (int16_t)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			what.int16_value = (int16_t)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			what.int16_value = (int16_t)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			what.int16_value = (int16_t)what.double_value;
			return true;
		case RX_TIME_TYPE:
			what.int16_value = (int16_t)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = (int16_t)what.complex_value->amplitude();
			delete what.complex_value;
			what.int16_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (int16_t)what.complex_value->amplitude();
			what.int16_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			int16_t temp = 0;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = (int16_t)std::stoi(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.int16_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_INT32_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.int32_value = what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			what.int32_value = (int32_t)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			what.int32_value = (int32_t)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			return true;
		case RX_INT64_TYPE:
			what.int32_value = (int32_t)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			what.int32_value = (int32_t)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			what.int32_value = (int32_t)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			what.int32_value = (int32_t)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			what.int32_value = (int32_t)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			what.int32_value = (int32_t)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			what.int32_value = (int32_t)what.double_value;
			return true;
		case RX_TIME_TYPE:
			what.int32_value = (int32_t)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_32
		case RX_COMPLEX_TYPE:
		{
			auto temp = (int32_t)what.complex_value->amplitude();
			delete what.complex_value;
			what.int32_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (int32_t)what.complex_value->amplitude();
			what.int32_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			int32_t temp = 0;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = (int32_t)std::stoi(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.int32_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_INT64_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.int64_value = what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			what.int64_value = (int64_t)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			what.int64_value = (int64_t)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			what.int64_value = (int64_t)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			return true;
		case RX_UINT8_TYPE:
			what.int64_value = (int64_t)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			what.int64_value = (int64_t)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			what.int64_value = (int64_t)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			what.int64_value = (int64_t)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			what.int64_value = (int64_t)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			what.int64_value = (int64_t)what.double_value;
			return true;
		case RX_TIME_TYPE:
			what.int64_value = (int64_t)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_32
		case RX_COMPLEX_TYPE:
		{
			auto temp = (int64_t)what.complex_value->amplitude();
			delete what.complex_value;
			what.int64_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (int64_t)what.complex_value->amplitude();
			what.int64_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			int64_t temp = 0;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = std::stoll(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.int64_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_UINT8_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.uint8_value = what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			what.uint8_value = (uint8_t)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			what.uint8_value = (uint8_t)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			what.uint8_value = (uint8_t)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			what.uint8_value = (uint8_t)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			return true;
		case RX_UINT16_TYPE:
			what.uint8_value = (uint8_t)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			what.uint8_value = (uint8_t)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			what.uint8_value = (uint8_t)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			what.uint8_value = (uint8_t)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			what.uint8_value = (uint8_t)what.double_value;
			return true;
		case RX_TIME_TYPE:
			what.uint8_value = (uint8_t)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint8_t)what.complex_value->amplitude();
			delete what.complex_value;
			what.uint8_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint8_t)what.complex_value->amplitude();
			what.uint8_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			uint8_t temp = 0;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = (uint8_t)std::stoul(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.uint8_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_UINT16_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.uint16_value = what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			what.uint16_value = (uint16_t)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			what.uint16_value = (uint16_t)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			what.uint16_value = (uint16_t)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			what.uint16_value = (uint16_t)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			what.uint16_value = (uint16_t)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			return true;
		case RX_UINT32_TYPE:
			what.uint16_value = (uint16_t)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			what.uint16_value = (uint16_t)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			what.uint16_value = (uint16_t)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			what.uint16_value = (uint16_t)what.double_value;
			return true;
		case RX_TIME_TYPE:
			what.uint16_value = (uint16_t)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint16_t)what.complex_value->amplitude();
			delete what.complex_value;
			what.uint16_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint16_t)what.complex_value->amplitude();
			what.uint16_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			uint16_t temp = 0;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = (uint16_t)std::stoul(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.uint16_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_UINT32_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.uint32_value =what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			what.uint32_value =(uint32_t)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			what.uint32_value =(uint32_t)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			what.uint32_value =(uint32_t)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			what.uint32_value =(uint32_t)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			what.uint32_value = (uint32_t)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			what.uint32_value =(uint32_t)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			return true;
		case RX_UINT64_TYPE:
			what.uint32_value =(uint32_t)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			what.uint32_value =(uint32_t)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			what.uint32_value =(uint32_t)what.double_value;
			return true;
		case RX_TIME_TYPE:
			what.uint32_value =(uint32_t)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint32_t)what.complex_value->amplitude();
			delete what.complex_value;
			what.uint32_value =temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint32_t)what.complex_value->amplitude();
			what.uint32_value =temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			uint32_t temp = 0;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = (uint32_t)std::stoul(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.uint32_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_UINT64_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.uint64_value = what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			what.uint64_value = (uint64_t)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			what.uint64_value = (uint64_t)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			what.uint64_value = (uint64_t)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			what.uint64_value = (uint64_t)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			what.uint64_value = (uint64_t)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			what.uint64_value = (uint64_t)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			what.uint64_value = (uint64_t)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			return true;
		case RX_FLOAT_TYPE:
			what.uint64_value = (uint64_t)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			what.uint64_value = (uint64_t)what.double_value;
			return true;
		case RX_TIME_TYPE:
			what.uint64_value = (uint64_t)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint64_t)what.complex_value->amplitude();
			delete what.complex_value;
			what.uint64_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint64_t)what.complex_value->amplitude();
			what.uint64_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			uint64_t temp = 0;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = (uint64_t)std::stoull(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.uint64_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_FLOAT_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.float_value =what.bool_value ? (float)1 : (float)0;
			return true;
		case RX_INT8_TYPE:
			what.float_value = (float)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			what.float_value =(float)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			what.float_value =(float)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			what.float_value =(float)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			what.float_value =(float)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			what.float_value =(float)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			what.float_value =(float)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			what.float_value =(float)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			return true;
		case RX_DOUBLE_TYPE:
			what.float_value =(float)what.double_value;
			return true;
		case RX_TIME_TYPE:
			what.float_value =(float)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = (float)what.complex_value->amplitude();
			delete what.complex_value;
			what.float_value =temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (float)what.complex_value->amplitude();
			what.float_value =temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			float temp;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = std::stof(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.float_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_DOUBLE_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.double_value = what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			what.double_value = (double)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			what.double_value = (double)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			what.double_value = (double)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			what.double_value = (double)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			what.double_value = (double)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			what.double_value = (double)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			what.double_value = (double)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			what.double_value = (double)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			what.double_value = (double)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			return true;
		case RX_TIME_TYPE:
			what.double_value = (double)what.time_value.t_value;
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = (double)what.complex_value->amplitude();
			delete what.complex_value;
			what.double_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (double)what.complex_value->amplitude();
			what.double_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			double temp;
			if (what.string_value.size > 0)
			{
				try
				{
					temp = std::stod(what.string_value.value);
				}
				catch (std::exception&)
				{
					return false;
				}
				delete[] what.string_value.value;
			}
			what.double_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
#ifndef RX_VALUE_SIZE_16
	case RX_COMPLEX_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.complex_value = new complex_value_struct{ what.bool_value ? 1.0 : 0.0, 0.0 };
			return true;
		case RX_INT8_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.int8_value, 0.0 };
			return true;
		case RX_INT16_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.int16_value, 0.0 };
			return true;
		case RX_INT32_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.int32_value, 0.0 };
			return true;
		case RX_INT64_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.int64_value, 0.0 };
			return true;
		case RX_UINT8_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.uint8_value, 0.0 };
			return true;
		case RX_UINT16_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.uint16_value, 0.0 };
			return true;
		case RX_UINT32_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.uint32_value, 0.0 };
			return true;
		case RX_UINT64_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.uint64_value, 0.0 };
			return true;
		case RX_FLOAT_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.float_value, 0.0 };
			return true;
		case RX_DOUBLE_TYPE:
			what.complex_value = new complex_value_struct{ what.double_value, 0.0 };
			return true;
		case RX_TIME_TYPE:
			what.complex_value = new complex_value_struct{ (double)what.time_value.t_value, 0.0 };
			return true;
		case RX_COMPLEX_TYPE:
			return true;
		case RX_UUID_TYPE:
			return false;
		case RX_STRING_TYPE:
		{
			complex_value_struct temp;
			if (what.string_value.size > 0)
			{
				if (!temp.parse_string(what.string_value.value))
					return false;
				delete[] what.string_value.value;
			}
			what.complex_value = new complex_value_struct(temp);
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
#else
	case RX_COMPLEX_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.complex_value = complex_value_struct{ what.bool_value ? 1.0 : 0.0, 0.0 };
			return true;
		case RX_INT8_TYPE:
			what.complex_value = complex_value_struct{ (double)what.int8_value, 0.0 };
			return true;
		case RX_INT16_TYPE:
			what.complex_value = complex_value_struct{ (double)what.int16_value, 0.0 };
			return true;
		case RX_INT32_TYPE:
			what.complex_value = complex_value_struct{ (double)what.int32_value, 0.0 };
			return true;
		case RX_INT64_TYPE:
			what.complex_value = complex_value_struct{ (double)what.int64_value, 0.0 };
			return true;
		case RX_UINT8_TYPE:
			what.complex_value = complex_value_struct{ (double)what.uint8_value, 0.0 };
			return true;
		case RX_UINT16_TYPE:
			what.complex_value = complex_value_struct{ (double)what.uint16_value, 0.0 };
			return true;
		case RX_UINT32_TYPE:
			what.complex_value = complex_value_struct{ (double)what.uint32_value, 0.0 };
			return true;
		case RX_UINT64_TYPE:
			what.complex_value = complex_value_struct{ (double)what.uint64_value, 0.0 };
			return true;
		case RX_FLOAT_TYPE:
			what.complex_value = complex_value_struct{ (double)what.float_value, 0.0 };
			return true;
		case RX_DOUBLE_TYPE:
			what.complex_value = complex_value_struct{ what.double_value, 0.0 };
			return true;
		case RX_TIME_TYPE:
			what.complex_value = complex_value_struct{ (double)what.time_value.t_value, 0.0 };
			return true;
		case RX_COMPLEX_TYPE:
			return true;
		case RX_UUID_TYPE:
			return false;
		case RX_STRING_TYPE:
		{
			complex_value_struct temp;
			if (what.string_value.size > 0)
			{
				if (!temp.parse_string(what.string_value.value))
					return false;
				delete[] what.string_value.value;
			}
			what.complex_value = complex_value_struct(temp);
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
#endif
#ifndef RX_VALUE_SIZE_16
	case RX_UUID_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
		case RX_INT8_TYPE:
		case RX_INT16_TYPE:
		case RX_INT32_TYPE:
		case RX_INT64_TYPE:
		case RX_UINT8_TYPE:
		case RX_UINT16_TYPE:
		case RX_UINT32_TYPE:
		case RX_UINT64_TYPE:
		case RX_FLOAT_TYPE:
		case RX_DOUBLE_TYPE:
		case RX_TIME_TYPE:
		case RX_COMPLEX_TYPE:
		case RX_UUID_TYPE:
			return false;
		case RX_STRING_TYPE:
		{
			rx_uuid temp;
			if (what.string_value.size > 0)
			{
				temp = rx_uuid::create_from_string(what.string_value.value);
				delete[] what.string_value.value;
			}
			what.uuid_value = new rx_uuid_t(temp.uuid());
			return true;
		}
		case RX_BYTES_TYPE:
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
#else
	case RX_UUID_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
		case RX_INT8_TYPE:
		case RX_INT16_TYPE:
		case RX_INT32_TYPE:
		case RX_INT64_TYPE:
		case RX_UINT8_TYPE:
		case RX_UINT16_TYPE:
		case RX_UINT32_TYPE:
		case RX_UINT64_TYPE:
		case RX_FLOAT_TYPE:
		case RX_DOUBLE_TYPE:
		case RX_TIME_TYPE:
		case RX_COMPLEX_TYPE:
		case RX_UUID_TYPE:
			return false;
		case RX_STRING_TYPE:
		{
			rx_uuid temp;
			if (what.string_value.size > 0)
			{
				rx_uuid temp = rx_uuid::create_from_string(what.string_value.value);
				delete[] what.string_value.value;
			}
			what.uuid_value = temp.uuid();
			return true;
		}
		case RX_BYTES_TYPE:
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
#endif
	case RX_TIME_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.time_value.t_value = what.bool_value ? 1 : 0;
			return true;
		case RX_INT8_TYPE:
			what.time_value.t_value = (uint64_t)what.int8_value;
			return true;
		case RX_INT16_TYPE:
			what.time_value.t_value = (uint64_t)what.int16_value;
			return true;
		case RX_INT32_TYPE:
			what.time_value.t_value = (uint64_t)what.int32_value;
			return true;
		case RX_INT64_TYPE:
			what.time_value.t_value = (uint64_t)what.int64_value;
			return true;
		case RX_UINT8_TYPE:
			what.time_value.t_value = (uint64_t)what.uint8_value;
			return true;
		case RX_UINT16_TYPE:
			what.time_value.t_value = (uint64_t)what.uint16_value;
			return true;
		case RX_UINT32_TYPE:
			what.time_value.t_value = (uint64_t)what.uint32_value;
			return true;
		case RX_UINT64_TYPE:
			what.time_value.t_value = (uint64_t)what.uint64_value;
			return true;
		case RX_FLOAT_TYPE:
			what.time_value.t_value = (uint64_t)what.float_value;
			return true;
		case RX_DOUBLE_TYPE:
			what.time_value.t_value = (uint64_t)what.double_value;
			return true;
		case RX_TIME_TYPE:
			return true;
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint64_t)what.complex_value->amplitude();
			delete what.complex_value;
			what.time_value.t_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = (uint64_t)what.complex_value->amplitude();
			what.time_value.t_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
			return false;
#endif
		case RX_STRING_TYPE:
		{
			if (what.string_value.size == 0)
				return false;
			rx_time temp = rx_time::from_IEC_string(what.string_value.value);
			if(temp.is_null())
				return false;
			delete[] what.string_value.value;
			what.time_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
			return false;
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_STRING_TYPE:
	{
		char temp_buff[0x20];

		switch (source)
		{
		case RX_BOOL_TYPE:
			if (what.bool_value)
			{
				what.string_value.size = 4;
				what.string_value.value = new char[4 + 1];
				memcpy(what.string_value.value, "true", 4 + 1);
			}
			else
			{
				what.string_value.size = 4;
				what.string_value.value = new char[4 + 1];
				memcpy(what.string_value.value, "true", 4 + 1);
			}
			return true;
		case RX_INT8_TYPE:
		{
			auto fc_result = std::to_chars(temp_buff, &temp_buff[sizeof(temp_buff) - 1], what.int8_value);
			if (fc_result.ec != std::errc{})
				return false;
			// add null termination
			*fc_result.ptr = '\0';
			what.string_value.size = fc_result.ptr - temp_buff;
			what.string_value.value = new char[what.string_value.size + 1];
			memcpy(what.string_value.value, temp_buff, what.string_value.size + 1);
			return true;
		}
		case RX_INT16_TYPE:
		{
				auto fc_result = std::to_chars(temp_buff, &temp_buff[sizeof(temp_buff) - 1], what.int16_value);
				if (fc_result.ec != std::errc{})
					return false;
				// add null termination
				*fc_result.ptr = '\0';
				what.string_value.size = fc_result.ptr - temp_buff;
				what.string_value.value = new char[what.string_value.size + 1];
				memcpy(what.string_value.value, temp_buff, what.string_value.size + 1);
				return true;
		}
		case RX_INT32_TYPE:
		{
			auto fc_result = std::to_chars(temp_buff, &temp_buff[sizeof(temp_buff) - 1], what.int32_value);
			if (fc_result.ec != std::errc{})
				return false;
			// add null termination
			*fc_result.ptr = '\0';
			what.string_value.size = fc_result.ptr - temp_buff;
			what.string_value.value = new char[what.string_value.size + 1];
			memcpy(what.string_value.value, temp_buff, what.string_value.size + 1);
			return true;
		}
		case RX_INT64_TYPE:
		{
			auto fc_result = std::to_chars(temp_buff, &temp_buff[sizeof(temp_buff) - 1], what.int64_value);
			if (fc_result.ec != std::errc{})
				return false;
			// add null termination
			*fc_result.ptr = '\0';
			what.string_value.size = fc_result.ptr - temp_buff;
			what.string_value.value = new char[what.string_value.size + 1];
			memcpy(what.string_value.value, temp_buff, what.string_value.size + 1);
			return true;
		}
		case RX_UINT8_TYPE:
		{
			auto fc_result = std::to_chars(temp_buff, &temp_buff[sizeof(temp_buff) - 1], what.uint8_value);
			if (fc_result.ec != std::errc{})
				return false;
			// add null termination
			*fc_result.ptr = '\0';
			what.string_value.size = fc_result.ptr - temp_buff;
			what.string_value.value = new char[what.string_value.size + 1];
			memcpy(what.string_value.value, temp_buff, what.string_value.size + 1);
			return true;
		}
		case RX_UINT16_TYPE:
		{
			auto fc_result = std::to_chars(temp_buff, &temp_buff[sizeof(temp_buff) - 1], what.uint16_value);
			if (fc_result.ec != std::errc{})
				return false;
			// add null termination
			*fc_result.ptr = '\0';
			what.string_value.size = fc_result.ptr - temp_buff;
			what.string_value.value = new char[what.string_value.size + 1];
			memcpy(what.string_value.value, temp_buff, what.string_value.size + 1);
			return true;
		}
		case RX_UINT32_TYPE:
		{
			auto fc_result = std::to_chars(temp_buff, &temp_buff[sizeof(temp_buff) - 1], what.uint32_value);
			if (fc_result.ec != std::errc{})
				return false;
			// add null termination
			*fc_result.ptr = '\0';
			what.string_value.size = fc_result.ptr - temp_buff;
			what.string_value.value = new char[what.string_value.size + 1];
			memcpy(what.string_value.value, temp_buff, what.string_value.size + 1);
			return true;
		}
		case RX_UINT64_TYPE:
		{
			auto fc_result = std::to_chars(temp_buff, &temp_buff[sizeof(temp_buff) - 1], what.uint64_value);
			if (fc_result.ec != std::errc{})
				return false;
			// add null termination
			*fc_result.ptr = '\0';
			what.string_value.size = fc_result.ptr - temp_buff;
			what.string_value.value = new char[what.string_value.size + 1];
			memcpy(what.string_value.value, temp_buff, what.string_value.size + 1);
			return true;
		}
		case RX_FLOAT_TYPE:
		{
			// GCC does not implement std::to_chars for floating point values
			std::ostringstream stream;
			stream << what.float_value;
			assign_string_value(what.string_value, stream.str().c_str());
			return true;
		}
		case RX_DOUBLE_TYPE:
		{
			// GCC does not implement std::to_chars for floating point values
			std::ostringstream stream;
			stream << what.double_value;
			assign_string_value(what.string_value, stream.str().c_str());
			return true;
		}
		case RX_TIME_TYPE:
		{
			rx_time tm(what.time_value);
			auto temp = tm.get_IEC_string();
			assign_string_value(what.string_value, temp.c_str());
			return true;
		}
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = what.complex_value->to_string();
			delete what.complex_value;
			assign_string_value(what.string_value, temp.c_str());
			return true;
		}
		case RX_UUID_TYPE:
		{
			auto temp = rx_uuid(*what.uuid_value).to_string();
			delete what.uuid_value;
			assign_string_value(what.string_value, temp.c_str());
			return true;
		}
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = what.complex_value.to_string();
			assign_string_value(what.string_value, temp.c_str());
			return true;
		}
		case RX_UUID_TYPE:
		{
			auto temp = rx_uuid(what.uuid_value).to_string();
			assign_string_value(what.string_value, temp.c_str());
			return true;
		}
#endif
		case RX_STRING_TYPE:
			return true;
		case RX_BYTES_TYPE:
		{
			string_type temp;
			if (what.bytes_value.size > 0)
				temp = string_type(&what.bytes_value.value[0]
					, &what.bytes_value.value[what.bytes_value.size]);
			delete[] what.bytes_value.value;
			assign_string_value(what.string_value, temp.c_str());
			return true;
		}
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_NODE_ID_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
		case RX_INT8_TYPE:
		case RX_INT16_TYPE:
		case RX_INT32_TYPE:
		case RX_INT64_TYPE:
		case RX_UINT8_TYPE:
		case RX_UINT16_TYPE:
		case RX_UINT32_TYPE:
		case RX_UINT64_TYPE:
		case RX_FLOAT_TYPE:
		case RX_DOUBLE_TYPE:
		case RX_TIME_TYPE:
		case RX_COMPLEX_TYPE:
		case RX_UUID_TYPE:
			return false;
		case RX_STRING_TYPE:
		{
			rx_node_id temp;
			if(what.string_value.size > 0)
				temp = rx_node_id::from_string(what.string_value.value);
			if (temp.is_null())
				return false;
			if (what.string_value.value != nullptr)
				delete[] what.string_value.value;
			what.node_id_value = new rx_node_id(std::move(temp));
			return true;
		}
		case RX_BYTES_TYPE:
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	case RX_BYTES_TYPE:
	{
		switch (source)
		{
		case RX_BOOL_TYPE:
		case RX_INT16_TYPE:
		case RX_INT32_TYPE:
		case RX_INT64_TYPE:
		case RX_UINT8_TYPE:
		case RX_UINT16_TYPE:
		case RX_UINT32_TYPE:
		case RX_UINT64_TYPE:
		case RX_FLOAT_TYPE:
		case RX_DOUBLE_TYPE:
		case RX_TIME_TYPE:
		case RX_COMPLEX_TYPE:
		case RX_UUID_TYPE:
			return false;
		case RX_STRING_TYPE:
		{
			bytes_value_struct temp;
			if (what.string_value.size == 0)
			{
				temp.size = 0;
			}
			else
			{
				temp.value = new uint8_t[what.string_value.size];
				memcpy(temp.value, what.string_value.value, what.string_value.size);
				delete[] what.string_value.value;
			}
			what.bytes_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
		case RX_NODE_ID_TYPE:
			return false;
		}
	}
	break;
	default:
		RX_ASSERT(false);
	}
	return false;
}
rx_value_storage::rx_value_storage(const rx_value_storage& right)
{
	value_.value_type = right.value_.value_type;
	assign_value(value_.value, right.value_.value, value_.value_type);
}
rx_value_storage::rx_value_storage(rx_value_storage&& right) noexcept
{
	value_.value_type = right.value_.value_type;
	assign_value(value_.value, std::move(right.value_.value), value_.value_type);
	right.value_.value_type = RX_NULL_TYPE;
}
rx_value_storage& rx_value_storage::operator=(const rx_value_storage& right)
{
	destroy_value(value_.value, value_.value_type);
	value_.value_type = right.value_.value_type;
	assign_value(value_.value, right.value_.value, value_.value_type);
	return *this;
}
rx_value_storage& rx_value_storage::operator=(rx_value_storage&& right) noexcept
{
	destroy_value(value_.value, value_.value_type);
	value_.value_type = right.value_.value_type;
	assign_value(value_.value, std::move(right.value_.value), value_.value_type);
	right.value_.value_type = RX_NULL_TYPE;
	return *this;
}


template<>
bool extract_value(const rx_value_storage& from, const bool& default_value)
{
	if (from.get_value_type() == RX_BOOL_TYPE)
	{
		return from.value_.value.bool_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if(temp_val.convert_to(RX_BOOL_TYPE))
			return temp_val.value_.value.bool_value;
	}
	return default_value;
}

template<>
int8_t extract_value(const rx_value_storage& from, const int8_t& default_value)
{
	if (from.get_value_type() == RX_INT8_TYPE)
	{
		return from.value_.value.int8_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_INT8_TYPE))
			return temp_val.value_.value.int8_value;
	}
	return default_value;
}

template<>
uint8_t extract_value(const rx_value_storage& from, const uint8_t& default_value)
{
	if (from.get_value_type() == RX_UINT8_TYPE)
	{
		return from.value_.value.uint8_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_UINT8_TYPE))
			return temp_val.value_.value.uint8_value;
	}
	return default_value;
}

template<>
int16_t extract_value(const rx_value_storage& from, const int16_t& default_value)
{
	if (from.get_value_type() == RX_INT16_TYPE)
	{
		return from.value_.value.int16_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_INT16_TYPE))
			return temp_val.value_.value.int16_value;
	}
	return default_value;
}

template<>
uint16_t extract_value(const rx_value_storage& from, const uint16_t& default_value)
{
	if (from.get_value_type() == RX_UINT16_TYPE)
	{
		return from.value_.value.uint16_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_UINT16_TYPE))
			return temp_val.value_.value.uint16_value;
	}
	return default_value;
}

template<>
int32_t extract_value(const rx_value_storage& from, const int32_t& default_value)
{
	if (from.get_value_type() == RX_INT32_TYPE)
	{
		return from.value_.value.int32_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_INT32_TYPE))
			return temp_val.value_.value.int32_value;
	}
	return default_value;
}

template<>
uint32_t extract_value(const rx_value_storage& from, const uint32_t& default_value)
{
	if (from.get_value_type() == RX_UINT32_TYPE)
	{
		return from.value_.value.uint32_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_UINT32_TYPE))
			return temp_val.value_.value.uint32_value;
	}
	return default_value;
}

template<>
int64_t extract_value(const rx_value_storage& from, const int64_t& default_value)
{
	if (from.get_value_type() == RX_INT64_TYPE)
	{
		return from.value_.value.int64_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_INT64_TYPE))
			return temp_val.value_.value.int64_value;
	}
	return default_value;
}

template<>
uint64_t extract_value(const rx_value_storage& from, const uint64_t& default_value)
{
	if (from.get_value_type() == RX_UINT64_TYPE)
	{
		return from.value_.value.uint64_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_UINT64_TYPE))
			return temp_val.value_.value.uint64_value;
	}
	return default_value;
}


template<>
float extract_value(const rx_value_storage& from, const float& default_value)
{
	if (from.get_value_type() == RX_FLOAT_TYPE)
	{
		return from.value_.value.float_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_FLOAT_TYPE))
			return temp_val.value_.value.float_value;
	}
	return default_value;
}


template<>
double extract_value(const rx_value_storage& from, const double& default_value)
{
	if (from.get_value_type() == RX_DOUBLE_TYPE)
	{
		return from.value_.value.double_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_DOUBLE_TYPE))
			return temp_val.value_.value.double_value;
	}
	return default_value;
}


template<>
string_type extract_value(const rx_value_storage& from, const string_type& default_value)
{
	if (from.get_value_type() == RX_STRING_TYPE)
	{
		if (from.value_.value.string_value.size > 0)
			return from.value_.value.string_value.value;
		else
			return string_type();
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_STRING_TYPE))
			return extract_value(temp_val, default_value);
	}
	return default_value;
}

template<>
byte_string extract_value(const rx_value_storage& from, const byte_string& default_value)
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
// Class rx::values::rx_timed_value 

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
  return storage_.is_array();

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
	val = rx_value::from_simple(to_simple(), std::max(ts, time_));
	if (mode.is_off())
		val.set_quality(RX_BAD_QUALITY_OFFLINE);
	else
		val.set_good_locally();
	if (mode.is_test())
		val.set_test();
}

bool rx_timed_value::serialize (const string_type& name, base_meta_writer& writter) const
{
	if (!writter.start_object(name.c_str()))
		return false;
	if (!storage_.serialize(writter))
		return false;
	if (!writter.write_time("ts", time_))
		return false;
	if (!writter.end_object())
		return false;
	return true;
}

bool rx_timed_value::deserialize (const string_type& name, base_meta_reader& reader)
{
	if (!reader.start_object(name.c_str()))
		return false;
	if (!storage_.deserialize(reader))
		return false;
	if (!reader.read_time("ts", time_))
		return false;
	if (!reader.end_object())
		return false;
	return true;
}

void rx_timed_value::dump_to_stream (std::ostream& out) const
{
	out << storage_.to_string();
	out << " [" << time_.get_string() << "]";
}

void rx_timed_value::parse_from_stream (std::istream& in)
{
	string_type temp;
	in >> temp;
	storage_.parse(temp);
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
	case time_compare_type::skip:
		return storage_.exact_equality(right.storage_);
	case time_compare_type::ms_accurate:
		return storage_.exact_equality(right.storage_)
				&& (time_.get_longlong_miliseconds() == right.time_.get_longlong_miliseconds());
	case time_compare_type::exact:
		return storage_.exact_equality(right.storage_)
				&& time_ == right.time_;
	default:
		return false;
	}
}

rx_timed_value rx_timed_value::from_simple (const rx_simple_value& value, rx_time ts)
{
	rx_timed_value ret;
	ret.storage_ = value.get_storage();
	ret.time_ = ts;
	return ret;
}

rx_timed_value rx_timed_value::from_simple (rx_simple_value&& value, rx_time ts)
{
	rx_timed_value ret;
	ret.storage_ = std::move(value.get_storage());
	ret.time_ = ts;
	return ret;
}

rx_simple_value rx_timed_value::to_simple () const
{
	return rx_simple_value(storage_);
}

bool rx_timed_value::convert_to (rx_value_t type)
{
	return storage_.convert_to(type);
}

rx_value_t rx_timed_value::get_type () const
{
	return storage_.get_value_type();
}

bool rx_timed_value::is_null () const
{
  return storage_.is_null();

}

bool rx_timed_value::is_complex () const
{
  return storage_.is_complex();

}

bool rx_timed_value::is_numeric () const
{
  return storage_.is_numeric();

}

bool rx_timed_value::is_integer () const
{
  return storage_.is_integer();

}

bool rx_timed_value::is_float () const
{
  return storage_.is_float();

}

rx_value_storage&& rx_timed_value::move_storage ()
{
	return std::move(storage_);
}


rx_timed_value::rx_timed_value()
	: default_time_compare_(time_compare_type::skip)
{
}
rx_timed_value::rx_timed_value(rx_timed_value&& right) noexcept
	: storage_(std::move(right.storage_))
	, time_(std::move(right.time_))
	, default_time_compare_(time_compare_type::skip)
{
}

rx_timed_value& rx_timed_value::operator=(rx_timed_value&& right) noexcept
{
	storage_ = std::move(right.storage_);
	time_ = std::move(right.time_);
	return *this;
}
rx_timed_value::rx_timed_value(const rx_timed_value &right)
	: storage_(right.storage_)
	, time_(right.time_)
	, default_time_compare_(time_compare_type::skip)
{
}
rx_timed_value & rx_timed_value::operator=(const rx_timed_value &right)
{
	storage_ = right.storage_;
	time_ = right.time_;
	return *this;
}

} // namespace values
} // namespace rx

