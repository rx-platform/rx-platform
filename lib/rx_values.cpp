

/****************************************************************************
*
*  lib\rx_values.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
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
*  along with rx-platform. It is also available in any rx-platform console
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

namespace values {
template<>
rx_value_t inner_get_type(tl::type2type<bool>)
{
	return RX_BOOL_TYPE;
}

template<>
rx_value_t inner_get_type(tl::type2type<char>)
{
	return RX_INT8_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<int8_t>)
{
	return RX_INT8_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<uint8_t>)
{
	return RX_UINT8_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<int16_t>)
{
	return RX_INT16_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<uint16_t>)
{
	return RX_UINT16_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<int32_t>)
{
	return RX_INT32_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<uint32_t>)
{
	return RX_UINT32_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<int64_t>)
{
	return RX_INT64_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<uint64_t>)
{
	return RX_UINT64_TYPE;
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
template<>
rx_value_t inner_get_type(tl::type2type<typename rx::rx_time>)
{
	return RX_TIME_TYPE;
}
template<>
rx_value_t inner_get_type(tl::type2type<typename rx::rx_node_id>)
{
	return RX_NODE_ID_TYPE;
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

rx::values::rx_value rx_value::from_simple (const rx_simple_value& value, rx_time ts)
{
	rx_value ret;
	ret.storage_ = value.get_storage();
	ret.time_ = ts;
	return ret;
}

rx::values::rx_value rx_value::from_simple (rx_simple_value&& value, rx_time ts)
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

rx::values::rx_value rx_value::operator + (const rx_value& right) const
{
	rx_value ret;
	ret.quality_ = RX_GOOD_QUALITY;
	ret.storage_ = storage_ + right.storage_;
	ret.handle_quality_after_arithmetic();
	return ret;
}

rx::values::rx_value rx_value::operator - (const rx_value& right) const
{
	rx_value ret;
	ret.quality_ = RX_GOOD_QUALITY;
	ret.storage_ = storage_ - right.storage_;
	ret.handle_quality_after_arithmetic();
	return ret;
}

rx::values::rx_value rx_value::operator * (const rx_value& right) const
{
	rx_value ret;
	ret.quality_ = quality_;
	ret.storage_ = storage_ * right.storage_;
	ret.handle_quality_after_arithmetic();
	return ret;
}

rx::values::rx_value rx_value::operator / (const rx_value& right) const
{
	rx_value ret;
	ret.quality_ = RX_GOOD_QUALITY;
	ret.storage_ = storage_ / right.storage_;
	ret.handle_quality_after_arithmetic();
	return ret;
}

rx::values::rx_value rx_value::operator % (const rx_value& right) const
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
	else
		val.set_good_locally();
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
	: value_type_(RX_NULL_TYPE)
{
}


rx_value_storage::~rx_value_storage()
{
	destroy_value(value_, value_type_);
}


bool rx_value_storage::operator==(const rx_value_storage &right) const
{
	if (is_simple_type(value_type_) && right.is_simple_type(right.value_type_))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() == right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() == right.get_float_value();
		}
		else
			return value_type_ == right.value_type_;
	}
	else if (value_type_ == right.value_type_)
	{
		if (value_type_ == RX_STRING_TYPE)
		{
			if (value_.string_value == nullptr && right.value_.string_value == nullptr)
				return true;
			else if (value_.string_value != nullptr && right.value_.string_value != nullptr)
				return *value_.string_value == *right.value_.string_value;
			else
				return false;
		}
		else if (value_type_ == RX_BYTES_TYPE)
		{
			if (value_.bytes_value->size() == right.value_.bytes_value->size())
				return memcmp(&(*(value_.bytes_value))[0], &(*(right.value_.bytes_value))[0], value_.bytes_value->size()) == 0;
			else
				return false;
		}
		else if (value_type_ == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			*value_.uuid_value == *right.value_.uuid_value;
#else
			value_.uuid_value == right.value_.uuid_value;
#endif
		}
	}
	return value_type_ == right.value_type_;
}

bool rx_value_storage::operator!=(const rx_value_storage &right) const
{
	return !operator==(right);
}


bool rx_value_storage::operator<(const rx_value_storage &right) const
{
	if (is_simple_type(value_type_) && right.is_simple_type(right.value_type_))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() < right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() < right.get_float_value();
		}
		else
			return value_type_ < right.value_type_;
	}
	else if (value_type_ == right.value_type_)
	{
		if (value_type_ == RX_STRING_TYPE)
		{
			if (value_.string_value == nullptr && right.value_.string_value == nullptr)
				return false;
			else if (value_.string_value != nullptr && right.value_.string_value != nullptr)
				return *value_.string_value < *right.value_.string_value;
			else
				return true;
		}
		else if (value_type_ == RX_BYTES_TYPE)
		{
			if (value_.bytes_value->size() == right.value_.bytes_value->size())
				return memcmp(&(*(value_.bytes_value))[0], &(*(right.value_.bytes_value))[0], value_.bytes_value->size()) < 0;
			else
				return value_.bytes_value->size() < right.value_.bytes_value->size();
		}
		else if (value_type_ == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			*value_.uuid_value < *right.value_.uuid_value;
#else
			value_.uuid_value < right.value_.uuid_value;
#endif
		}
	}
	return value_type_ < right.value_type_;
}

bool rx_value_storage::operator>(const rx_value_storage &right) const
{
	if (is_simple_type(value_type_) && right.is_simple_type(right.value_type_))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() > right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() > right.get_float_value();
		}
		else
			return value_type_ > right.value_type_;
	}
	else if (value_type_ == right.value_type_)
	{
		if (value_type_ == RX_STRING_TYPE)
		{
			if (value_.string_value == nullptr && right.value_.string_value == nullptr)
				return false;
			else if (value_.string_value != nullptr && right.value_.string_value != nullptr)
				return *value_.string_value > *right.value_.string_value;
			else
				return true;
		}
		else if (value_type_ == RX_BYTES_TYPE)
		{
			if (value_.bytes_value->size() == right.value_.bytes_value->size())
				return memcmp(&(*(value_.bytes_value))[0], &(*(right.value_.bytes_value))[0], value_.bytes_value->size()) > 0;
			else
				return value_.bytes_value->size() > right.value_.bytes_value->size();
		}
		else if (value_type_ == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			*value_.uuid_value > *right.value_.uuid_value;
#else
			value_.uuid_value > right.value_.uuid_value;
#endif
		}
	}
	return value_type_ > right.value_type_;
}

bool rx_value_storage::operator<=(const rx_value_storage &right) const
{
	if (is_simple_type(value_type_) && right.is_simple_type(right.value_type_))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() <= right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() <= right.get_float_value();
		}
		else
			return value_type_ <= right.value_type_;
	}
	else if (value_type_ == right.value_type_)
	{
		if (value_type_ == RX_STRING_TYPE)
		{
			if (value_.string_value == nullptr && right.value_.string_value == nullptr)
				return true;
			else if (value_.string_value != nullptr && right.value_.string_value != nullptr)
				return *value_.string_value <= *right.value_.string_value;
			else
				return value_.string_value == nullptr && right.value_.string_value != nullptr;
		}
		else if (value_type_ == RX_BYTES_TYPE)
		{
			if (value_.bytes_value->size() == right.value_.bytes_value->size())
				return memcmp(&(*(value_.bytes_value))[0], &(*(right.value_.bytes_value))[0], value_.bytes_value->size()) <= 0;
			else
				return value_.bytes_value->size() < right.value_.bytes_value->size();
		}
		else if (value_type_ == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			*value_.uuid_value <= *right.value_.uuid_value;
#else
			value_.uuid_value <= right.value_.uuid_value;
#endif
		}
	}
	return value_type_ <= right.value_type_;
}

bool rx_value_storage::operator>=(const rx_value_storage &right) const
{

	if (is_simple_type(value_type_) && right.is_simple_type(right.value_type_))
	{
		if (is_numeric() && right.is_numeric())
		{
			if (is_integer() && right.is_integer())
				return get_integer_value() >= right.get_integer_value();
			else// is_float() || is_complex()
				return get_float_value() >= right.get_float_value();
		}
		else
			return value_type_ >= right.value_type_;
	}
	else if (value_type_ == right.value_type_)
	{
		if (value_type_ == RX_STRING_TYPE)
		{
			if (value_.string_value == nullptr && right.value_.string_value == nullptr)
				return true;
			else if (value_.string_value != nullptr && right.value_.string_value != nullptr)
				return *value_.string_value >= *right.value_.string_value;
			else
				return value_.string_value != nullptr && right.value_.string_value == nullptr;
		}
		else if (value_type_ == RX_BYTES_TYPE)
		{
			if (value_.bytes_value->size() == right.value_.bytes_value->size())
				return memcmp(&(*(value_.bytes_value))[0], &(*(right.value_.bytes_value))[0], value_.bytes_value->size()) >= 0;
			else
				return value_.bytes_value->size() > right.value_.bytes_value->size();
		}
		else if (value_type_ == RX_UUID_TYPE)
		{
#ifndef RX_VALUE_SIZE_16
			*value_.uuid_value >= *right.value_.uuid_value;
#else
			value_.uuid_value >= right.value_.uuid_value;
#endif
		}
	}
	return value_type_ >= right.value_type_;
}



bool rx_value_storage::serialize (base_meta_writer& writer) const
{
	if (!writer.write_byte("type", value_type_))
		return false;
	if (!serialize_value(writer, value_, value_type_, "val"))
		return false;
	return true;
}

bool rx_value_storage::deserialize (base_meta_reader& reader)
{
	// first destroy eventual values already inside
	destroy_value(value_, value_type_);
	value_type_ = RX_NULL_TYPE;
	if (!reader.read_byte("type", value_type_))
		return false;
	if (!deserialize_value(reader, value_, value_type_))
		return false;
	return true;
}

string_type rx_value_storage::to_string (bool simple) const
{
	char buff[0x20];
	switch (value_type_)
	{
	case RX_NULL_TYPE:
		return "<null>";
	case RX_BOOL_TYPE:
		return (value_.bool_value ? "true" : "false");
	case RX_INT8_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.int8_value);
			if(ret.ec== std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if(!simple)
				ret_str += "b";
			return ret_str;
		}
	case RX_UINT8_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.uint8_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "ub";
			return ret_str;
		}
	case RX_INT16_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.int16_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "s";
			return ret_str;
		}
	case RX_UINT16_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.uint16_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "us";
			return ret_str;
		}
	case RX_INT32_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.int32_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "i";
			return ret_str;
		}
	case RX_UINT32_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.uint32_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "ui";
			return ret_str;
		}
	case RX_INT64_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.int64_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "l";
			return ret_str;
		}
	case RX_UINT64_TYPE:
		{
			auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.uint64_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';
			string_type ret_str(buff);
			if (!simple)
				ret_str += "ul";
			return ret_str;
		}
	case RX_FLOAT_TYPE:
		{
			/*auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.float_value, std::chars_format::general);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';*/
			sprintf(buff, "%g", (double)value_.float_value);
			string_type ret_str(buff);
			if (!simple)
				ret_str += "f";
			return ret_str;
		}
	case RX_DOUBLE_TYPE:
		{
			/*auto ret = std::to_chars(buff, buff + sizeof(buff) / sizeof(buff[0]), value_.double_value);
			if (ret.ec == std::errc())
				*ret.ptr = '\0';*/
			sprintf(buff, "%g", value_.double_value);
			string_type ret_str(buff);
			if (!simple)
				ret_str += "d";
			return ret_str;
		}
	case RX_STRING_TYPE:
		return "\""s + *value_.string_value + "\"";
	case RX_UUID_TYPE:
		return value_.uuid_value->to_string();
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
		assign_static(str);
	}
}

rx_value_t rx_value_storage::get_value_type () const
{
	return value_type_;
}

string_type rx_value_storage::get_type_string () const
{
	if (value_type_&RX_ARRAY_VALUE_MASK)
	{
		return get_type_string();
	}
	switch (value_type_&RX_SIMPLE_VALUE_MASK)
	{
	case RX_NULL_TYPE:
		return "null"s;
	case RX_BOOL_TYPE:
		return "bit"s;
	case RX_INT8_TYPE:
		return "int8";
	case RX_UINT8_TYPE:
		return "unsigned int8"s;
	case RX_INT16_TYPE:
		return "int16";
	case RX_UINT16_TYPE:
		return "unsigned int16"s;
	case RX_INT32_TYPE:
		return "int32";
	case RX_UINT32_TYPE:
		return "unsigned int32"s;
	case RX_INT64_TYPE:
		return "int64";
	case RX_UINT64_TYPE:
		return "unsigned int64"s;
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
		return "node id";
	case RX_COMPLEX_TYPE:
		return "complex";
	case RX_CLASS_TYPE:
	{
		switch (value_type_&RX_DEFINITION_TYPE_MASK)
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
		switch (value_type_&RX_DEFINITION_TYPE_MASK)
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
	}
}

bool rx_value_storage::expresion_equality (const rx_value_storage& right) const
{
	return false;
}

bool rx_value_storage::exact_equality (const rx_value_storage& right) const
{
	if (value_type_ != right.value_type_)
		return false;
	return exact_equality(value_, right.value_, value_type_);
}

bool rx_value_storage::convert_to (rx_value_t type)
{
	if (value_type_ == type)
		return true;// nothing to do
	if (type == RX_NULL_TYPE)
	{// clear value here
		destroy_value(value_, value_type_);
		value_type_ = RX_NULL_TYPE;
		return true;
	}
	bool target_is_array = (type&RX_ARRAY_VALUE_MASK) == RX_ARRAY_VALUE_MASK;
	bool current_is_array = (value_type_&RX_ARRAY_VALUE_MASK) == RX_ARRAY_VALUE_MASK;
	if (!target_is_array && !current_is_array)
	{// normal values
		if (!convert_union(value_, value_type_, type))
			return false;
		value_type_ = type;
		return true;
	}
	else if (target_is_array && current_is_array)
	{// both are arrays convert individual items
		for (auto& one : *value_.array_value)
		{
			if (!convert_union(one, value_type_&RX_STRIP_ARRAY_MASK, type&RX_STRIP_ARRAY_MASK))
				return false;
		}
		value_type_ = type;
		return true;
	}
	else if (!target_is_array && current_is_array)
	{// from array to simple value
		if (value_.array_value->empty())
		{// empty array only to null type, checked before
			return false;
		}
		if (!convert_union(value_.array_value->at(0), value_type_&RX_STRIP_ARRAY_MASK, type))
			return false;
		rx_value_union temp;
		assign_value(temp, value_.array_value->at(0), type);
		destroy_value(value_, type&RX_STRIP_ARRAY_MASK);
		assign_value(value_, std::move(temp), type);
		value_type_ = type;
		return true;
	}
	else //(target_is_array && !current_is_array)
	{// from simple value to array
		if (!convert_union(value_, value_type_, type&RX_STRIP_ARRAY_MASK))
			return false;
		std::vector<rx_value_union>* temp_array = new std::vector<rx_value_union>{ value_ };
		value_.array_value = temp_array;
		value_type_ = type;
		return true;
	}
}

bool rx_value_storage::is_complex () const
{
	return value_type_ == RX_COMPLEX_TYPE;
}

bool rx_value_storage::is_numeric () const
{
	return (value_type_ > RX_NULL_TYPE && value_type_ <= RX_COMPLEX_TYPE);
}

bool rx_value_storage::is_integer () const
{
	return value_type_ > RX_NULL_TYPE && value_type_ <= RX_UINT64_TYPE;
}

bool rx_value_storage::is_float () const
{
	return value_type_ == RX_FLOAT_TYPE || value_type_ == RX_DOUBLE_TYPE || value_type_ == RX_COMPLEX_TYPE;
}

bool rx_value_storage::is_null () const
{
	return value_type_ == RX_NULL_TYPE;
}

complex_value_struct rx_value_storage::get_complex_value () const
{
	if (is_complex())
	{

#ifdef RX_VALUE_SIZE_16
		return value_.complex_value;
#else
		return *value_.complex_value;
#endif
	}
	complex_value_struct ret;
	ret.imag = 0;
	ret.real = 0;
	if (is_integer())
		ret.real = (double)get_int_value(value_type_, value_);
	else if (is_float())
		ret.real = get_float_value();
	return ret;
}

double rx_value_storage::get_float_value () const
{
	return get_float_value(value_type_, value_);
}

int64_t rx_value_storage::get_integer_value (rx_value_t* min_type) const
{
	return get_int_value(value_type_, value_);
}

bool rx_value_storage::get_bool_value () const
{
	return get_bool_value(value_type_, value_);
}

int64_t rx_value_storage::get_int_value (rx_value_t type, const rx_value_union& value, rx_value_t* min_type, size_t idx) const
{
	if (IS_ARRAY_VALUE(type))
	{
		if (idx < value.array_value->size())
		{
			return get_int_value(RX_SIMPLE_TYPE(type), value.array_value->at(idx));
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
			if (value.string_value)
				return value.string_value->size();
			else
				return 0;
		case RX_BYTES_TYPE:
			if (min_type)
				*min_type = RX_UINT32_TYPE;
			return value.bytes_value->size();
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
		if (idx < value.array_value->size())
		{
			return get_float_value(RX_SIMPLE_TYPE(type), value.array_value->at(idx));
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
			if (value.string_value)
				return (double)value.string_value->size();
			else
				return 0;
		case RX_BYTES_TYPE:
			return (double)value.bytes_value->size();
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
	return IS_ARRAY_VALUE(value_type_);
}

bool rx_value_storage::get_bool_value (rx_value_t type, const rx_value_union& value, size_t idx) const
{
	if (IS_ARRAY_VALUE(type))
	{
		if (idx < value.array_value->size())
		{
			return get_bool_value(RX_SIMPLE_TYPE(type), value.array_value->at(idx));
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
			if (value.string_value)
				return !value.string_value->empty();
			else
				return false;
		case RX_BYTES_TYPE:
			return !value.bytes_value->empty();
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
	destroy_value(value_, value_type_);
	value_type_ = type;
	return set_from_complex(val, type, value_);
}

bool rx_value_storage::set_from_float (double val, rx_value_t type)
{
	destroy_value(value_, value_type_);
	value_type_ = type;
	return set_from_float(val, type, value_);
}

bool rx_value_storage::set_from_integer (int64_t val, rx_value_t type)
{
	destroy_value(value_, value_type_);
	value_type_ = type;
	return set_from_integer(val, type, value_);
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
			where.string_value = new string_type(buff);
		}
		break;
	case RX_BYTES_TYPE:
		where.bytes_value = new byte_string();
		return false;
		break;
	case RX_UUID_TYPE:
#ifdef RX_VALUE_SIZE_16
		where.uuid_value = rx_uuid::null_uuid();
#else
		where.uuid_value = new rx_uuid(rx_uuid::null_uuid());
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
			where.string_value = new string_type(buff);
		}
		break;
	case RX_BYTES_TYPE:
		where.bytes_value = new byte_string();
		return false;
		break;
	case RX_UUID_TYPE:
#ifdef RX_VALUE_SIZE_16
		where.uuid_value = rx_uuid::null_uuid();
#else
		where.uuid_value = new rx_uuid(rx_uuid::null_uuid());
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
			where.string_value = new string_type(buff);
		}
		break;
	case RX_BYTES_TYPE:
		where.bytes_value = new byte_string();
		return false;
		break;
	case RX_UUID_TYPE:
#ifdef RX_VALUE_SIZE_16
		where.uuid_value = rx_uuid::null_uuid();
#else
		where.uuid_value = new rx_uuid(rx_uuid::null_uuid());
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
	rx_value_t ret_type = get_arithmetic_result_type(value_type_, right.value_type_, true);
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
			result.value_type_ = RX_TIME_TYPE;
			result.value_.time_value = rx_time(value_.time_value) + right.get_integer_value();
		}
		else if (ret_type == RX_BYTES_TYPE && value_type_ == RX_BYTES_TYPE && right.value_type_ == RX_BYTES_TYPE)
		{
			result.value_type_ = RX_BYTES_TYPE;
			result.value_.bytes_value = new byte_string(*value_.bytes_value);
			result.value_.bytes_value->insert(result.value_.bytes_value->end(), right.value_.bytes_value->begin(), right.value_.bytes_value->end());
		}
		else if (ret_type == RX_STRING_TYPE && value_type_ == RX_STRING_TYPE)
		{
			result.value_type_ = RX_STRING_TYPE;
			result.value_.string_value = new string_type(*value_.string_value);
			if (right.value_type_ == RX_STRING_TYPE)
			{// do dynamic stuff
				*result.value_.string_value += *right.value_.string_value;
			}
			else
			{
				*result.value_.string_value += right.to_string();
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
	rx_value_t ret_type = get_arithmetic_result_type(value_type_, right.value_type_, false);
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
			result.value_type_ = RX_TIME_TYPE;
			result.value_.time_value = rx_time(value_.time_value) - right.get_integer_value();
		}
	}
	return result;
}

rx_value_storage rx_value_storage::operator * (const rx_value_storage& right) const
{
	rx_value_storage result;
	rx_value_t ret_type = get_arithmetic_result_type(value_type_, right.value_type_, false);
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
	rx_value_t ret_type = get_arithmetic_result_type(value_type_, right.value_type_, false);
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
	rx_value_t ret_type = get_arithmetic_result_type(value_type_, right.value_type_, false);
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
	if (value_type_ == RX_NODE_ID_TYPE)
	{
		return *value_.node_id_value;
	}
	else
	{
		return rx_node_id();
	}
}

string_type rx_value_storage::get_string_value () const
{
	if (value_type_ == RX_STRING_TYPE)
	{
		return *value_.string_value;
	}
	else
	{
		rx_value_storage conv(*this);
		if (conv.convert_to(RX_STRING_TYPE))
		{
			return *conv.value_.string_value;
		}
		else
		{
			return string_type();
		}
	}
}

bool rx_value_storage::weak_serialize_value (const string_type& name, base_meta_writer& writer) const
{
	if (!serialize_value(writer, value_, value_type_, name))
		return false;
	return true;
}

bool rx_value_storage::weak_deserialize_value (const string_type& name, base_meta_reader& reader)
{
	return false;
}

void rx_value_storage::assign(bool val)
{
	value_.bool_value = val;
}
void rx_value_storage::assign(int8_t val)
{
	value_.int8_value = val;
}
void rx_value_storage::assign(uint8_t val)
{
	value_.uint8_value = val;
}
void rx_value_storage::assign(int16_t val)
{
	value_.int16_value = val;
}
void rx_value_storage::assign(uint16_t val)
{
	value_.uint16_value = val;
}
void rx_value_storage::assign(int32_t val)
{
	value_.int32_value = val;
}
void rx_value_storage::assign(uint32_t val)
{
	value_.uint32_value = val;
}
void rx_value_storage::assign(int64_t val)
{
	value_.int64_value = val;
}
void rx_value_storage::assign(uint64_t val)
{
	value_.uint64_value = val;
}
void rx_value_storage::assign(float val)
{
	value_.float_value = val;
}
void rx_value_storage::assign(double val)
{
	value_.double_value = val;
}
void rx_value_storage::assign(std::complex<double> val)
{
#ifdef RX_SIZE_16
	value_.complex_value = complex_value_struct{ val.real(), val.imag() };
#else
	value_.complex_value = new complex_value_struct{ val.real(), val.imag() };
#endif
}
void rx_value_storage::assign(rx_time_struct val)
{
	value_.time_value = val;
}
void rx_value_storage::assign(rx_uuid val)
{
#ifdef RX_SIZE_16
	uuid_value = val;
#else
	value_.uuid_value = new rx_uuid(val);
#endif
}
void rx_value_storage::assign(string_type&& val)
{
	value_.string_value = new string_type(std::move(val));
}
void rx_value_storage::assign(const string_type& val)
{
	value_.string_value = new string_type(val);
}
void rx_value_storage::assign(const char* val)
{
	value_.string_value = new string_type(val);
}
void rx_value_storage::assign(byte_string&& val)
{
	value_.bytes_value = new byte_string(std::move(val));
}
void rx_value_storage::assign(const byte_string& val)
{
	value_.bytes_value = new byte_string(val);
}
void rx_value_storage::assign(bit_string&& val)
{
	value_.bits_value = new bit_string(std::move(val));
}
void rx_value_storage::assign(const bit_string& val)
{
	value_.bits_value = new bit_string(val);
}
void rx_value_storage::assign(std::vector<rx_value_union>&& val)
{
	value_.array_value = new std::vector<rx_value_union>(std::move(val));
}
void rx_value_storage::assign(const std::vector<rx_value_union>& val)
{
	value_.array_value = new std::vector<rx_value_union>(val);
}
void rx_value_storage::assign_value(rx_value_union& left, const rx_value_union& right, rx_value_t type)
{
	// now do the actual copy of pointer members
	if (type&RX_ARRAY_VALUE_MASK)
	{// we have an array
		left.array_value = new std::vector<rx_value_union>(*right.array_value);
		// check to see pointers stuff
		size_t count = left.array_value->size();
		for (size_t i = 0; i < count; i++)
		{
			assign_value(left.array_value->at(i), right.array_value->at(i), RX_STRIP_ARRAY_MASK&type);
		}
		delete left.array_value;
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
			left.uuid_value = new rx_uuid(*right.uuid_value);
			break;
#endif
		case RX_STRING_TYPE:
			left.string_value = new string_type(*right.string_value);
			break;
		case RX_BYTES_TYPE:
			left.bytes_value = new byte_string(*right.bytes_value);
			break;
		case RX_NODE_ID_TYPE:
			left.node_id_value = new rx_node_id(*right.node_id_value);
			break;
		case RX_OBJECT_TYPE:
		case RX_CLASS_TYPE:
		{
			switch (type&RX_DEFINITION_TYPE_MASK)
			{
			case RX_BINARY_VALUE:
				left.bytes_value = new byte_string(*right.bytes_value);
				break;
			case RX_JSON_VALUE:
			case RX_SCRIPT_VALUE:
				left.string_value = new string_type(*right.string_value);
				break;
			default:
				RX_ASSERT(false);
				// shouldn't happened
			}
		}
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
		for (auto& one : *who.array_value)
		{
			destroy_value(one, RX_STRIP_ARRAY_MASK&type);
		}
		delete who.array_value;
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
			delete who.string_value;
			break;
		case RX_BYTES_TYPE:
			delete who.bytes_value;
			break;
			break;
		case RX_NODE_ID_TYPE:
			delete who.node_id_value;
			break;
		case RX_OBJECT_TYPE:
		case RX_CLASS_TYPE:
		{
			switch (type&RX_DEFINITION_TYPE_MASK)
			{
			case RX_BINARY_VALUE:
				delete who.bytes_value;
				break;
			case RX_JSON_VALUE:
			case RX_SCRIPT_VALUE:
				delete who.string_value;
				break;
			default:
				RX_ASSERT(false);
				// shouldn't happened
			}
		}
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
		writer.start_array(name.c_str(), who.array_value->size());
		for (const auto& one : *who.array_value)
		{
			writer.start_object("item");
			serialize_value(writer, one, type&RX_STRIP_ARRAY_MASK, "val");
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
			writer.write_string(name.c_str(), who.string_value->c_str());
			break;
		case RX_TIME_TYPE:
			writer.write_time(name.c_str(), who.time_value);
			break;
		case RX_UUID_TYPE:
			writer.write_uuid(name.c_str(), who.uuid_value->uuid());
			break;
		case RX_BYTES_TYPE:
			writer.write_bytes(name.c_str(), &who.bytes_value->at(0),who.bytes_value->size());
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
		case RX_OBJECT_TYPE:
		case RX_CLASS_TYPE:
		{
			switch (type&RX_DEFINITION_TYPE_MASK)
			{
			case RX_BINARY_VALUE:
				writer.write_bytes(name.c_str(), &who.bytes_value->at(0), who.bytes_value->size());
				break;
			case RX_JSON_VALUE:
			case RX_SCRIPT_VALUE:
				writer.write_string(name.c_str(), who.string_value->c_str());
				break;
			default:
				RX_ASSERT(false);
				// shouldn't happened
			}
		}
		break;
		}
	}
	return true;
}
bool rx_value_storage::deserialize_value(base_meta_reader& reader, rx_value_union& who, rx_value_t type)
{
	if (RX_ARRAY_VALUE_MASK&type)
	{
		who.array_value = new std::vector<rx_value_union>;
		if (!reader.start_array("Vals"))
			return false;
		while(!reader.array_end())
		{
			rx_value_union temp;
			if (!deserialize_value(reader, temp, type&RX_STRIP_ARRAY_MASK))
				return false;
			who.array_value->emplace_back(temp);
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
		case RX_STRING_TYPE:
		{
			string_type val;
			if(!reader.read_string("val", val))
				return false;
			who.string_value = new string_type(std::move(val));
			return true;
		}
		case RX_TIME_TYPE:
			return reader.read_time("val", who.time_value);
		case RX_OBJECT_TYPE:
		case RX_CLASS_TYPE:
		{
			switch (type&RX_DEFINITION_TYPE_MASK)
			{
			case RX_BINARY_VALUE:
			{
				byte_string val;
				if (!reader.read_bytes("val", val))
					return false;
				who.bytes_value = new byte_string(std::move(val));
				return true;
			}
			case RX_JSON_VALUE:
			case RX_SCRIPT_VALUE:
			{
				string_type val;
				if (!reader.read_string("val", val))
					return false;
				who.string_value = new string_type(std::move(val));
				return true;
			}
			default:
				RX_ASSERT(false);
				// shouldn't happened
			}
		}
		break;
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
	if (simple == RX_OBJECT_TYPE || simple == RX_CLASS_TYPE)
	{
		switch (target&RX_DEFINITION_TYPE_MASK)
		{
		case RX_BINARY_VALUE:
			simple = RX_BYTES_TYPE;
			break;
		case RX_JSON_VALUE:
		case RX_SCRIPT_VALUE:
			simple = RX_STRING_TYPE;
			break;
		default:
			RX_ASSERT(false);
			// shouldn't happened
		}
	}
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
			bool temp = !what.uuid_value->is_null();
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
			bool temp = !what.string_value->empty();
			delete what.string_value;
			what.bool_value = temp;
			return true;
		}
		case RX_BYTES_TYPE:
		{
			bool temp = !what.bytes_value->empty();
			delete what.bytes_value;
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
			int8_t temp;
			try
			{
				temp = (int8_t)std::stoi(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
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
			int16_t temp;
			try
			{
				temp = (int16_t)std::stoi(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
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
			int32_t temp;
			try
			{
				temp = (int32_t)std::stoi(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
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
			int64_t temp;
			try
			{
				temp = std::stoll(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
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
			uint8_t temp;
			try
			{
				temp = (uint8_t)std::stoul(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
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
			uint16_t temp;
			try
			{
				temp = (uint16_t)std::stoul(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
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
			uint32_t temp;
			try
			{
				temp = (uint32_t)std::stoul(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
			what.uint32_value =temp;
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
			uint64_t temp;
			try
			{
				temp = (uint64_t)std::stoull(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
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
			try
			{
				temp = std::stof(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
			what.float_value =temp;
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
			try
			{
				temp = (double)std::stod(*what.string_value);
			}
			catch (std::exception&)
			{
				return false;
			}
			delete what.string_value;
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
			if (!temp.parse_string(*what.string_value))
				return false;
			delete what.string_value;
			what.complex_value = new complex_value_struct(std::move(temp));
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
			if (!temp.parse_string(*what.string_value))
				return false;
			delete what.string_value;
			what.complex_value = complex_value_struct(std::move(temp));
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
			rx_uuid temp = rx_uuid::create_from_string(*what.string_value);
			delete what.string_value;
			what.uuid_value = new rx_uuid(std::move(temp));
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
			rx_uuid temp = rx_uuid::create_from_string(*what.string_value);
			delete what.string_value;
			what.uuid_value = std::move(temp);
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
			rx_time temp = rx_time::from_IEC_string(what.string_value->c_str());
			if(temp.is_null())
				return false;
			delete what.string_value;
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
		switch (source)
		{
		case RX_BOOL_TYPE:
			what.string_value = new string_type(what.bool_value ? "true" : "false");
			return true;
		case RX_INT8_TYPE:
		{
			std::ostringstream stream;
			stream << what.int8_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_INT16_TYPE:
		{
			std::ostringstream stream;
			stream << what.int16_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_INT32_TYPE:
		{
			std::ostringstream stream;
			stream << what.int32_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_INT64_TYPE:
		{
			std::ostringstream stream;
			stream << what.int64_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_UINT8_TYPE:
		{
			std::ostringstream stream;
			stream << what.uint8_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_UINT16_TYPE:
		{
			std::ostringstream stream;
			stream << what.uint16_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_UINT32_TYPE:
		{
			std::ostringstream stream;
			stream << what.uint32_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_UINT64_TYPE:
		{
			std::ostringstream stream;
			stream << what.uint64_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_FLOAT_TYPE:
		{
			std::ostringstream stream;
			stream << what.float_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_DOUBLE_TYPE:
		{
			std::ostringstream stream;
			stream << what.double_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
		case RX_TIME_TYPE:
		{
			std::ostringstream stream;
			stream << what.float_value;
			what.string_value = new string_type(stream.str());
			return true;
		}
#ifndef RX_VALUE_SIZE_16
		case RX_COMPLEX_TYPE:
		{
			auto temp = new string_type(what.complex_value->to_string());
			delete what.complex_value;
			what.string_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
		{
			auto temp = new string_type(what.uuid_value->to_string());
			delete what.uuid_value;
			what.string_value = temp;
			return true;
		}
#else
		case RX_COMPLEX_TYPE:
		{
			auto temp = new string_type(what.complex_value.to_string());
			what.string_value = temp;
			return true;
		}
		case RX_UUID_TYPE:
		{
			auto temp = new string_type(what.uuid_value.to_string());
			delete what.uuid_value;
			what.string_value = temp;
			return true;
		}
#endif
		case RX_STRING_TYPE:
			return true;
		case RX_BYTES_TYPE:
		{
			string_type* temp = new string_type(what.bytes_value->begin()
				, what.bytes_value->end());
			delete what.bytes_value;
			what.string_value = temp;
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
			rx_node_id temp = rx_node_id::from_string(what.string_value->c_str());
			if (temp.is_null())
				return false;
			delete what.string_value;
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
			byte_string* temp = new byte_string(what.string_value->begin(),
				what.string_value->end());
			delete what.string_value;
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
	: value_type_(right.value_type_)
{
	assign_value(value_, right.value_, value_type_);
}
rx_value_storage::rx_value_storage(rx_value_storage&& right) noexcept
	: value_type_(right.value_type_)
{
	assign_value(value_, std::move(right.value_), value_type_);
	right.value_type_ = RX_NULL_TYPE;
}
rx_value_storage& rx_value_storage::operator=(const rx_value_storage& right)
{
	destroy_value(value_, value_type_);
	value_type_ = right.value_type_;
	assign_value(value_, right.value_, value_type_);
	return *this;
}
rx_value_storage& rx_value_storage::operator=(rx_value_storage&& right) noexcept
{
	destroy_value(value_, value_type_);
	value_type_ = right.value_type_;
	assign_value(value_, std::move(right.value_), value_type_);
	right.value_type_ = RX_NULL_TYPE;
	return *this;
}


template<>
bool extract_value(const rx_value_storage& from, const bool& default_value)
{
	if (from.get_value_type() == RX_BOOL_TYPE)
	{
		return from.value_.bool_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if(temp_val.convert_to(RX_BOOL_TYPE))
			return temp_val.value_.bool_value;
	}
	return default_value;
}

template<>
int8_t extract_value(const rx_value_storage& from, const int8_t& default_value)
{
	if (from.get_value_type() == RX_INT8_TYPE)
	{
		return from.value_.int8_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_INT8_TYPE))
			return temp_val.value_.int8_value;
	}
	return default_value;
}

template<>
uint8_t extract_value(const rx_value_storage& from, const uint8_t& default_value)
{
	if (from.get_value_type() == RX_UINT8_TYPE)
	{
		return from.value_.uint8_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_UINT8_TYPE))
			return temp_val.value_.uint8_value;
	}
	return default_value;
}

template<>
int16_t extract_value(const rx_value_storage& from, const int16_t& default_value)
{
	if (from.get_value_type() == RX_INT16_TYPE)
	{
		return from.value_.int16_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_INT16_TYPE))
			return temp_val.value_.int16_value;
	}
	return default_value;
}

template<>
uint16_t extract_value(const rx_value_storage& from, const uint16_t& default_value)
{
	if (from.get_value_type() == RX_UINT16_TYPE)
	{
		return from.value_.uint16_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_UINT16_TYPE))
			return temp_val.value_.uint16_value;
	}
	return default_value;
}

template<>
int32_t extract_value(const rx_value_storage& from, const int32_t& default_value)
{
	if (from.get_value_type() == RX_INT32_TYPE)
	{
		return from.value_.int32_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_INT32_TYPE))
			return temp_val.value_.int32_value;
	}
	return default_value;
}

template<>
uint32_t extract_value(const rx_value_storage& from, const uint32_t& default_value)
{
	if (from.get_value_type() == RX_UINT32_TYPE)
	{
		return from.value_.uint32_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_UINT32_TYPE))
			return temp_val.value_.uint32_value;
	}
	return default_value;
}

template<>
int64_t extract_value(const rx_value_storage& from, const int64_t& default_value)
{
	if (from.get_value_type() == RX_INT64_TYPE)
	{
		return from.value_.int64_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_INT64_TYPE))
			return temp_val.value_.int64_value;
	}
	return default_value;
}

template<>
uint64_t extract_value(const rx_value_storage& from, const uint64_t& default_value)
{
	if (from.get_value_type() == RX_UINT64_TYPE)
	{
		return from.value_.uint64_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_UINT64_TYPE))
			return temp_val.value_.uint64_value;
	}
	return default_value;
}


template<>
float extract_value(const rx_value_storage& from, const float& default_value)
{
	if (from.get_value_type() == RX_FLOAT_TYPE)
	{
		return from.value_.float_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_FLOAT_TYPE))
			return temp_val.value_.float_value;
	}
	return default_value;
}


template<>
double extract_value(const rx_value_storage& from, const double& default_value)
{
	if (from.get_value_type() == RX_DOUBLE_TYPE)
	{
		return from.value_.double_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_DOUBLE_TYPE))
			return temp_val.value_.double_value;
	}
	return default_value;
}


template<>
string_type extract_value(const rx_value_storage& from, const string_type& default_value)
{
	if (from.get_value_type() == RX_STRING_TYPE)
	{
		return *from.value_.string_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_STRING_TYPE))
			return std::move(*temp_val.value_.string_value);
	}
	return default_value;
}

template<>
byte_string extract_value(const rx_value_storage& from, const byte_string& default_value)
{
	if (from.get_value_type() == RX_BYTES_TYPE)
	{
		return *from.value_.bytes_value;
	}
	else
	{
		rx_value_storage temp_val(from);
		if (temp_val.convert_to(RX_BYTES_TYPE))
			return std::move(*temp_val.value_.bytes_value);
	}
	return default_value;
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

