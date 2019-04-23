

/****************************************************************************
*
*  lib\rx_values.cpp
*
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

bool rx_value::operator==(const rx_value &right) const
{
	return false;
}

bool rx_value::operator!=(const rx_value &right) const
{
	return !operator==(right);
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
	return IS_ARRAY_VALUE(storage_.get_value_type());
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

bool rx_value::serialize (base_meta_writer& stream) const
{
	if (!stream.start_object("value"))
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

bool rx_value::deserialize (base_meta_reader& stream)
{
	if (!stream.start_object("value"))
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
	ret.storage_ = std::move(value.get_storage());
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


rx_value::rx_value()
	: default_time_compare_(time_compare_skip)
	, quality_(RX_DEFAULT_VALUE_QUALITY)
	, origin_(RX_DEFUALT_ORIGIN)
{
	//rx_value temp;
	//temp.assign_static<bool>(false);
}
rx_value::rx_value(rx_value&& right) noexcept
	: storage_(std::move(right.storage_))
	, time_(std::move(right.time_))
	, default_time_compare_(time_compare_skip)
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
	, default_time_compare_(time_compare_skip)
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

bool rx_simple_value::serialize (base_meta_writer& writter) const
{
	return storage_.serialize(writter);
}

bool rx_simple_value::deserialize (base_meta_reader& reader)
{
	return storage_.deserialize(reader);
}

void rx_simple_value::dump_to_stream (std::ostream& out) const
{
	storage_.dump_to_stream(out);
}

void rx_simple_value::parse_from_stream (std::istream& in)
{
	storage_.parse_from_stream(in);
}

void rx_simple_value::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	val.set_time(ts);
	if (mode.is_off())
		val.set_quality(RX_BAD_QUALITY_OFFLINE);
	else
		val.set_good_locally();
	if (mode.is_test())
		val.set_test();
}

bool rx_simple_value::serialize_value (base_meta_writer& writer, const string_type& name) const
{
	if (!storage_.serialize_value(writer, name))
		return false;
	return true;
}

bool rx_simple_value::deserialize_value (base_meta_reader& reader, const string_type& name)
{
	if (!storage_.deserialize_value(reader, name))
		return false;
	return true;
}

bool rx_simple_value::convert_to (rx_value_t type)
{
	return storage_.convert_to(type);
}

rx_value_t rx_simple_value::get_type () const
{
	return storage_.get_value_type();
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



bool rx_value_storage::serialize (base_meta_writer& writer) const
{
	if (!writer.start_object("_val"))
		return false;
	if (!writer.write_byte("type", value_type_))
		return false;
	if (!serialize_value(writer, value_, value_type_, "val"))
		return false;
	if (!writer.end_object())
		return false;
	return true;
}

bool rx_value_storage::deserialize (base_meta_reader& reader)
{
	// first destroy eventual values already inside
	destroy_value(value_, value_type_);
	value_type_ = RX_NULL_TYPE;
	if (!reader.start_object("_val"))
		return false;
	if (!reader.read_byte("type", value_type_))
		return false;
	if (!deserialize_value(reader, value_, value_type_))
		return false;
	if (!reader.end_object())
		return false;
	return true;
}

void rx_value_storage::dump_to_stream (std::ostream& out) const
{
	switch (value_type_)
	{
	case RX_NULL_TYPE:
		out << "<null>";
		break;
	case RX_BOOL_TYPE:
		out << (value_.bool_value ? "true" : "false");
		break;
	case RX_INT8_TYPE:
		out << value_.int8_value <<"b";
		break;
	case RX_UINT8_TYPE:
		out << value_.uint8_value << "ub";
		break;
	case RX_INT16_TYPE:
		out << value_.int16_value << "s";
		break;
	case RX_UINT16_TYPE:
		out << value_.uint16_value << "us";
		break;
	case RX_INT32_TYPE:
		out << value_.int32_value << "i";
		break;
	case RX_UINT32_TYPE:
		out << value_.uint32_value << "ui";
		break;
	case RX_INT64_TYPE:
		out << value_.int64_value << "l";
		break;
	case RX_UINT64_TYPE:
		out << value_.uint64_value << "ul";
		break;
	case RX_FLOAT_TYPE:
		out << value_.float_value << "f";
		break;
	case RX_DOUBLE_TYPE:
		out << value_.double_value << "d";
		break;
	case RX_STRING_TYPE:
		out << "\"" << *value_.string_value << "\"";
		break;
	}
}

void rx_value_storage::parse_from_stream (std::istream& in)
{
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

bool rx_value_storage::serialize_value (base_meta_writer& writer, const string_type& name) const
{
	if (!serialize_value(writer, value_, value_type_, name))
		return false;
	return true;
}

bool rx_value_storage::deserialize_value (base_meta_reader& reader, const string_type& name)
{
	return false;
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
			what.bool_value = what.float_value != 0;
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

bool rx_timed_value::serialize (base_meta_writer& writter) const
{
	if (!writter.start_object("timed"))
		return false;
	if (!storage_.serialize(writter))
		return false;
	if (!writter.write_time("ts", time_))
		return false;
	if (!writter.end_object())
		return false;
	return true;
}

bool rx_timed_value::deserialize (base_meta_reader& reader)
{
	if (!reader.start_object("timed_"))
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
	storage_.dump_to_stream(out);
	out << " [" << time_.get_string() << "]";
}

void rx_timed_value::parse_from_stream (std::istream& in)
{
	storage_.parse_from_stream(in);
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
		return storage_.exact_equality(right.storage_);
	case time_compare_ms_accurate:
		return storage_.exact_equality(right.storage_)
				&& (time_.get_longlong_miliseconds() == right.time_.get_longlong_miliseconds());
	case time_compare_exact:
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


rx_timed_value::rx_timed_value()
	: default_time_compare_(time_compare_skip)
{
}
rx_timed_value::rx_timed_value(rx_timed_value&& right) noexcept
	: storage_(std::move(right.storage_))
	, time_(std::move(right.time_))
	, default_time_compare_(time_compare_skip)
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
	, default_time_compare_(time_compare_skip)
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

