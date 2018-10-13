

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
	if (!stream.start_object("Value"))
		return false;
	if (!storage_.serialize(stream))
		return false;
	if (!stream.write_time("TS", time_))
		return false;
	if (!stream.write_uint("Quality", quality_))
		return false;
	if (!stream.write_uint("Origin", origin_))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

bool rx_value::deserialize (base_meta_reader& stream)
{
	if (!stream.start_object("Value"))
		return false;
	if (!storage_.deserialize(stream))
		return false;
	if (!stream.read_time("TS", time_))
		return false;
	if (!stream.read_uint("Quality", quality_))
		return false;
	if (!stream.read_uint("Origin", origin_))
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

void rx_simple_value::get_full_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
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
	destory_value(value_, value_type_);
}



bool rx_value_storage::serialize (base_meta_writer& writer) const
{
	if (!writer.start_object("_Val"))
		return false;
	if (!writer.write_byte("Type", value_type_))
		return false;
	if (!serialize_value(writer, value_, value_type_))
		return false;
	if (!writer.end_object())
		return false;
	return true;
}

bool rx_value_storage::deserialize (base_meta_reader& reader)
{
	// first destroy eventual values already inside
	destory_value(value_, value_type_);
	value_type_ = RX_NULL_TYPE;
	if (!reader.start_object("_Val"))
		return false;
	if (!reader.read_byte("Type", value_type_))
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
	switch (value_type_)
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
	case RX_COMPLEX_TYPE:
		return "complex";
	default:
		return "internal error unknown type!!!";
		RX_ASSERT(false);
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

void rx_value_storage::assign(bool val)
{
	destory_value(value_, value_type_);
	value_type_ = RX_BOOL_TYPE;
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
		case RX_BITS_TYPE:
			left.bits_value = new bit_string(*right.bits_value);
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
				// shouldn't happend
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
	// memcpy is enougth this is move
	memcpy(&left, &right, sizeof(rx_value_union));
}
void rx_value_storage::destory_value(rx_value_union& who, rx_value_t type)
{
	if (type == RX_NULL_TYPE)
		return;

	if (type&RX_ARRAY_VALUE_MASK)
	{// we have an array
		for (auto& one : *who.array_value)
		{
			destory_value(one, RX_STRIP_ARRAY_MASK&type);
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
		case RX_BITS_TYPE:
			delete who.bits_value;
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
				// shouldn't happend
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
bool rx_value_storage::serialize_value(base_meta_writer& writer, const rx_value_union& who, rx_value_t type)
{
	if (type&RX_ARRAY_VALUE_MASK)
	{// array of values
		writer.start_array("Vals", who.array_value->size());
		for (const auto& one : *who.array_value)
		{
			writer.start_object("Item");
			serialize_value(writer, one, type&RX_STRIP_ARRAY_MASK);
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
			writer.write_bool("Val", who.bool_value);
			break;
		case RX_INT8_TYPE:
			writer.write_byte("Val", who.int8_value);
			break;
		case RX_UINT8_TYPE:
			writer.write_byte("Val", who.uint8_value);
			break;
		case RX_INT16_TYPE:
			writer.write_int("Val", who.int16_value);
			break;
		case RX_UINT16_TYPE:
			writer.write_uint("Val", who.uint16_value);
			break;
		case RX_INT32_TYPE:
			writer.write_int("Val", who.int32_value);
			break;
		case RX_UINT32_TYPE:
			writer.write_uint("Val", who.uint32_value);
			break;
		case RX_INT64_TYPE:
			writer.write_int64("Val", who.int64_value);
			break;
		case RX_UINT64_TYPE:
			writer.write_uint64("Val", who.uint64_value);
			break;
		case RX_FLOAT_TYPE:
			writer.write_double("Val", who.float_value);
			break;
		case RX_DOUBLE_TYPE:
			writer.write_double("Val", who.double_value);
			break;
		case RX_STRING_TYPE:
			writer.write_string("Val", who.string_value->c_str());
			break;
		case RX_TIME_TYPE:
			writer.write_time("Val", who.time_value);
			break;
		case RX_UUID_TYPE:
			writer.write_uuid("Val", who.uuid_value->uuid());
			break;
		case RX_BYTES_TYPE:
			writer.write_bytes("Val", &who.bytes_value->at(0),who.bytes_value->size());
			break;
		case RX_COMPLEX_TYPE:
			writer.write_double("Real", who.complex_value->real);
			writer.write_double("Imag", who.complex_value->imag);
			break;
		case RX_OBJECT_TYPE:
		case RX_CLASS_TYPE:
		{
			switch (type&RX_DEFINITION_TYPE_MASK)
			{
			case RX_BINARY_VALUE:
				writer.write_bytes("Val", &who.bytes_value->at(0), who.bytes_value->size());
				break;
			case RX_JSON_VALUE:
			case RX_SCRIPT_VALUE:
				writer.write_string("Val", who.string_value->c_str());
				break;
			default:
				RX_ASSERT(false);
				// shouldn't happend
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
			return reader.read_bool("Val", who.bool_value);
		case RX_INT8_TYPE:
		{
			uint8_t temp;
			if (!reader.read_byte("Val", temp))
				return false;
			who.int8_value = temp;
			return true;
		}
		case RX_UINT8_TYPE:
			return reader.read_byte("Val", who.uint8_value);
		case RX_INT16_TYPE:
		{
			int32_t temp;
			if (!reader.read_int("Val", temp))
				return false;
			who.int16_value = temp;
			return true;
		}
		case RX_UINT16_TYPE:
		{
			uint32_t temp;
			if(!reader.read_uint("Val", temp))
				return false;
			who.uint16_value = temp;
			return true;
		}
		case RX_INT32_TYPE:
			return reader.read_int("Val", who.int32_value);
		case RX_UINT32_TYPE:
			return reader.read_uint("Val", who.uint32_value);
		case RX_INT64_TYPE:
			return reader.read_int64("Val", who.int64_value);
		case RX_UINT64_TYPE:
			return reader.read_uint64("Val", who.uint64_value);
		case RX_FLOAT_TYPE:
		{
			double temp;
			if(!reader.read_double("Val", temp))
				return false;
			who.float_value = static_cast<float>(temp);
			return true;
		}
		case RX_DOUBLE_TYPE:
			return reader.read_double("Val", who.double_value);
		case RX_STRING_TYPE:
		{
			string_type val;
			if(!reader.read_string("Val", val))
				return false;
			who.string_value = new string_type(std::move(val));
			return true;
		}
		case RX_TIME_TYPE:
			return reader.read_time("Val", who.time_value);
		case RX_OBJECT_TYPE:
		case RX_CLASS_TYPE:
		{
			switch (type&RX_DEFINITION_TYPE_MASK)
			{
			case RX_BINARY_VALUE:
			{
				byte_string val;
				if (!reader.read_bytes("Val", val))
					return false;
				who.bytes_value = new byte_string(std::move(val));
				return true;
			}
			case RX_JSON_VALUE:
			case RX_SCRIPT_VALUE:
			{
				string_type val;
				if (!reader.read_string("Val", val))
					return false;
				who.string_value = new string_type(std::move(val));
				return true;
			}
			default:
				RX_ASSERT(false);
				// shouldn't happend
			}
		}
		break;
		default:
			RX_ASSERT(false);
			// shouldn't happend
		}
	}
	return true;
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
}
rx_value_storage& rx_value_storage::operator=(const rx_value_storage& right)
{
	destory_value(value_, value_type_);
	value_type_ = right.value_type_;
	assign_value(value_, right.value_, value_type_);
	return *this;
}
rx_value_storage& rx_value_storage::operator==(rx_value_storage&& right) noexcept
{
	destory_value(value_, value_type_);
	value_type_ = right.value_type_;
	assign_value(value_, std::move(right.value_), value_type_);
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
	if (!writter.start_object("Timed"))
		return false;
	if (!storage_.serialize(writter))
		return false;
	if (!writter.write_time("TS", time_))
		return false;
	if (!writter.end_object())
		return false;
	return true;
}

bool rx_timed_value::deserialize (base_meta_reader& reader)
{
	if (!reader.start_object("Timed_"))
		return false;
	if (!storage_.deserialize(reader))
		return false;
	if (!reader.read_time("time", time_))
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

