

/****************************************************************************
*
*  lib\rx_asn.cpp
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


// rx_asn
#include "lib/rx_asn.h"


#define ASN_READ_INT_OVERFLOW "ASN read int overflow!"
#define ASN_READ_BITSTR_ERROR "ASN read bit string error overflow!"
#define ASN_EXPLICIT_READ_TAG_ERROR "ASN explicit read tag mismatch"
#define ASN_EXPLICIT_LENGTH_ERROR "ASN explicit read length mismatch"
#define ASN_READ_UNSUPPORTED_LEN "ASN Unsupported length of tag"


#define ASN_READ_FLOAT_UNSUPPORTED "ASN unsupported float value"
#define ASN_READ_WRONG_TIME_VALUE "ASN read time value size not match"
#define ASN_READ_WRONG_GTIME_VALUE "ASN read generalized time error"
#define ASN_READ_NULL_VAL_ERROR "ASN read NULL value error"



namespace rx {

namespace asn {

// Class rx::asn::oid_t 

oid_t::oid_t()
      : heap_storage_(NULL)
{
    size_ = 0;
    data_ = static_storage_;
}

oid_t::oid_t(const oid_t &right)
      : heap_storage_(NULL)
{
	size_ = right.size_;
	if (size_ > OID_STATIC_SIZE)
	{
		heap_storage_ = new uint32_t[OID_DYNAMIC_SIZE];
		data_ = heap_storage_;
	}
	else
	{// static storage
		data_ = static_storage_;
	}
	memcpy(data_, right.data_, size_ * sizeof(uint32_t));
}

oid_t::oid_t (const uint32_t* vals, size_t size)
      : heap_storage_(NULL)
{
	size_ = size;
	if (size_ > OID_STATIC_SIZE)
	{
		heap_storage_ = new uint32_t[OID_DYNAMIC_SIZE];
		data_ = heap_storage_;
	}
	else
	{// static storage
		data_ = static_storage_;
	}
	memcpy(data_, vals, size_ * sizeof(uint32_t));
}

oid_t::oid_t (const std::vector<uint32_t>& vals)
      : heap_storage_(NULL)
{
	size_ = vals.size();
	if (size_ > OID_STATIC_SIZE)
	{
		heap_storage_ = new uint32_t[OID_DYNAMIC_SIZE];
		data_ = heap_storage_;
	}
	else
	{// static storage
		data_ = static_storage_;
	}
	memcpy(data_, &vals[0], size_ * sizeof(uint32_t));
}

oid_t::oid_t (const char* vals)
      : heap_storage_(NULL)
{
	std::string str(vals);
	init_from_string(str);
}

oid_t::oid_t (const string_type& str)
      : heap_storage_(NULL)
{
	init_from_string(str);
}


oid_t::~oid_t()
{
	if (heap_storage_)
		delete[] heap_storage_;
}


oid_t & oid_t::operator=(const oid_t &right)
{
	if (heap_storage_)
	{
		delete[] heap_storage_;
		heap_storage_ = NULL;
	}
	size_ = right.size_;
	if (size_ > OID_STATIC_SIZE)
	{
		heap_storage_ = new uint32_t[OID_DYNAMIC_SIZE];
		data_ = heap_storage_;
	}
	else
	{// static storage
		data_ = static_storage_;
	}
	memcpy(data_, right.data_, size_ * sizeof(uint32_t));
	return *this;
}


bool oid_t::operator==(const oid_t &right) const
{
	if (size_ == right.size_)
		return memcmp(data_, right.data_, size_ * sizeof(uint32_t)) == 0;
	else
		return false;
}

bool oid_t::operator!=(const oid_t &right) const
{
	return !operator==(right);
}


bool oid_t::operator<(const oid_t &right) const
{
	if (size_ == right.size_)
		return memcmp(data_, right.data_, size_ * sizeof(uint32_t)) < 0;
	else
		return size_ < right.size_;
}

bool oid_t::operator>(const oid_t &right) const
{
	if (size_ == right.size_)
		return memcmp(data_, right.data_, size_ * sizeof(uint32_t)) > 0;
	else
		return size_ > right.size_;
}

bool oid_t::operator<=(const oid_t &right) const
{
	if (size_ == right.size_)
		return memcmp(data_, right.data_, size_ * sizeof(uint32_t)) <= 0;
	else
		return size_ < right.size_;
}

bool oid_t::operator>=(const oid_t &right) const
{
	if (size_ == right.size_)
		return memcmp(data_, right.data_, size_ * sizeof(uint32_t)) >= 0;
	else
		return size_ > right.size_;
}



size_t oid_t::size () const
{
	return size_;
}

void oid_t::push_back (uint32_t val)
{
	size_++;
	if (heap_storage_ == NULL)
	{// we're static
		if (size_ > OID_STATIC_SIZE)
		{// we should be dynamic :( to large
			heap_storage_ = new uint32_t[OID_DYNAMIC_SIZE];
			memcpy(heap_storage_, static_storage_, sizeof(static_storage_));
			data_ = heap_storage_;
		}
	}
	data_[size_ - 1] = val;
}

bool oid_t::valid () const
{
	return size_ > 1;
}

bool oid_t::empty () const
{
	return size_ == 0;
}

void oid_t::to_string (string_type& str) const
{
}

void oid_t::init_from_string (const string_type& str)
{

	std::vector<uint32_t> arr;

	size_t idx1 = 0;
	while (idx1 != std::string::npos)
	{
		size_t idx2 = str.find('.', idx1);
		std::string one;
		if (idx2 == std::string::npos)
		{
			one = str.substr(idx1);
			idx1 = idx2;
		}
		else
		{
			one = str.substr(idx1, idx2 - idx1);
			idx1 = idx2 + 1;
		}
		char* endptr = NULL;
		uint32_t oned = strtoul(one.c_str(), &endptr, 10);
		arr.push_back(oned);
	}

	size_ = arr.size();
	if (size_ > OID_STATIC_SIZE)
	{
		heap_storage_ = new uint32_t[OID_DYNAMIC_SIZE];
		data_ = heap_storage_;
	}
	else
	{// static storage
		data_ = static_storage_;
	}
	memcpy(data_, &arr[0], size_ * sizeof(uint32_t));
}

const uint32_t& oid_t::operator [] (const size_t& index) const
{
	if (((size_t)index) < size_)
	{
		return data_[index];
	}
	else
		throw std::runtime_error("oid_t: Index out of bounds!");
}

uint32_t& oid_t::operator [] (const size_t& index)
{
	if (((size_t)index) < size_)
	{
		return data_[index];
	}
	else
		throw std::runtime_error("oid_t: Index out of bounds!");
}

void oid_t::clear ()
{
	if (heap_storage_)
		delete[] heap_storage_;
	size_ = 0;
}


// Class rx::asn::rx_asn_in_stream 

rx_asn_in_stream::rx_asn_in_stream (io::rx_const_io_buffer* buffer)
      : buffer_(buffer),
        current_length_(0),
        complex_(false)
{
}


rx_asn_in_stream::~rx_asn_in_stream()
{
}



bool rx_asn_in_stream::read_next_tag (uint32_t& type)
{

	if (eof())
		return false;
	if (stack_.empty() || buffer_->get_possition() < stack_.top().end_possition)
	{
		uint8_t temp[2];
		buffer_->read_data(temp, sizeof(temp));
		if ((temp[0] & 0x1f) == 0x1f)
		{// more then one uint8_t
			type = (((uint32_t)temp[0]) << 8);
			type = type + temp[1];
			buffer_->read_data(&temp[1], 1);
		}
		else
		{// one uint8_t exactly
			type = temp[0];
		}
		// now encode length
		if ((temp[1] & 0x80) == 0x80)
		{// multi uint8_t length
			uint8_t lenlen = temp[1] & 0x7f;
			switch (lenlen)
			{
			case 1:
				{
					uint8_t temp1;
					buffer_->read_data(&temp1, sizeof(temp1));
					current_length_ = temp1;
				}
				break;
			case 2:
				{
					uint8_t temp1[2];
					buffer_->read_data(temp1, sizeof(temp1));
					current_length_ = (temp1[0] << 8) + temp1[1];
				}
				break;
			case 3:
				{
					uint8_t temp1[3];
					buffer_->read_data(temp1, sizeof(temp1));
					current_length_ = (temp1[0] << 16) | (temp1[1] << 8) | temp1[2];
				}
				break;
			case 4:
				{
					uint8_t temp1[4];
					buffer_->read_data(temp1, sizeof(temp1));
					current_length_ = (temp1[0] << 24) | (temp1[1] << 16) | (temp1[2] << 8) | temp1[3];
				}
				break;
			default:
				throw std::runtime_error(ASN_READ_UNSUPPORTED_LEN);
			}
		}
		else
		{// single uint8_t length
			current_length_ = temp[1];
		}

		if ((temp[0] & 0x20) == 0x20)
		{//complex type
			// create new stack value
			asn_in_stack_data tempd;
			tempd.end_possition = (uint32_t)buffer_->get_possition() + current_length_;
			tempd.type = temp[0];
			stack_.push(tempd);
			complex_ = true;
		}
		else
			complex_ = false;
		return true;
	}
	else
	{// we're done so do stack pop
		stack_.pop();
		return false;
	}
}

uint32_t rx_asn_in_stream::get_length ()
{
	return current_length_;
}

void rx_asn_in_stream::skip_tag ()
{
	buffer_->skip(current_length_);
	if (complex_ && !stack_.empty())
		stack_.pop();
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (uint8_t& val)
{
	if (current_length_ == 1)
	{
		buffer_->read_data(&val, sizeof(val));
	}
	else
	{
		throw std::runtime_error(ASN_READ_INT_OVERFLOW);
	}
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (uint16_t& val)
{
	switch (current_length_)
	{
	case 1:
		{
			uint8_t temp;
			buffer_->read_data(&temp, sizeof(temp));
			val = temp;
		}
		break;
	case 2:
		{
			buffer_->read_data(&val, sizeof(val));
			val = ntohs(val);
		}
		break;
	case 3:
		{
			uint8_t one = 0;
			buffer_->read_data(&one, 1);
			buffer_->read_data(&val, sizeof(val));
			val = ntohs(val);
		}
		break;
	default:
		throw std::runtime_error(ASN_READ_INT_OVERFLOW);
	}
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (int16_t& val)
{
	RX_ASSERT(false);// not implemented
	return *this;
}

bool rx_asn_in_stream::eof () const
{
	return buffer_->eof();
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (uint32_t& val)
{
	switch (current_length_)
	{
	case 1:
		{
			uint8_t temp;
			buffer_->read_data(&temp, sizeof(temp));
			val = temp;
		}
		break;
	case 2:
		{
			uint8_t temp[2];
			buffer_->read_data(&temp, 2);
			val = (((uint32_t)temp[0]) << 8) | temp[1];
		}
		break;
	case 3:
		{
			uint8_t temp[3];
			buffer_->read_data(temp, 3);
			val = (((uint32_t)temp[0]) << 16) | (((uint32_t)temp[1]) << 8) | temp[2];
		}
		break;
	case 4:
		{
			buffer_->read_data(&val, sizeof(val));
			val = ntohl(val);
		}
		break;
	case 5:
		{
			uint8_t one = 0;
			buffer_->read_data(&one, 1);
			buffer_->read_data(&val, sizeof(val));
			val = ntohl(val);
		}
		break;
	default:
		throw std::runtime_error(ASN_READ_INT_OVERFLOW);
	}
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (int32_t& val)
{
	switch (current_length_)
	{
	case 1:
		{
			int8_t temp;
			buffer_->read_data(&temp, sizeof(temp));
			val = temp;
		}
		break;
	case 2:
		{
			int16_t temp;
			buffer_->read_data(&temp, 2);
			temp = ntohs(temp);
			val = (int32_t)temp;
		}
		break;
	case 3:
		{
			uint8_t temp[3];
			buffer_->read_data(temp, 3);
			val = (((uint32_t)temp[0]) << 16) | (((uint32_t)temp[1]) << 8) | temp[2];
		}
		break;
	case 4:
		{
			buffer_->read_data(&val, sizeof(val));
			val = ntohl(val);
		}
		break;
	default:
		throw std::runtime_error(ASN_READ_INT_OVERFLOW);
	}
	return *this;
}

uint32_t rx_asn_in_stream::get_current_object_end_pos ()
{
	RX_ASSERT(!stack_.empty());
	if (!stack_.empty())
		return stack_.top().end_possition;
	else
		return 0;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (oid_t& val)
{
	uint8_t temp[4];
	val.clear();
	buffer_->read_data(temp, 1);
	val.push_back(temp[0] / 40);
	val.push_back(temp[0] % 40);
	uint32_t i = 1;
	while (i < current_length_)
	{
		buffer_->read_data(temp, 1);
		i++;
		if (temp[0] < 0x80)
			val.push_back(temp[0]);
		else
		{
			buffer_->read_data(&temp[1], 1);
			i++;
			val.push_back(((temp[0] & 0x7f) << 7) | (temp[1] & 0x7f));
		}
	}
	return *this;
}

void rx_asn_in_stream::read_explicit (uint8_t& val)
{
	uint8_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x2)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

void rx_asn_in_stream::read_explicit (uint16_t& val)
{
	uint8_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x2)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

void rx_asn_in_stream::read_explicit (int16_t& val)
{
	uint8_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x2)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

void rx_asn_in_stream::read_explicit (uint32_t& val)
{
	uint8_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x2)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

void rx_asn_in_stream::read_explicit (int32_t& val)
{
	uint8_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x2)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

void rx_asn_in_stream::read_explicit (oid_t& val)
{
	uint8_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x6)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (bit_string& val)
{
	if (current_length_ < 2)
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	uint8_t unused;
	buffer_->read_data(&unused, sizeof(unused));
	if (unused > 7)
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	// total bits count
	uint32_t count = ((current_length_ - 1) << 3) - unused;
	uint32_t written = 0;
	val.clear();
	val.reserve(count);
	for (uint32_t i = 0; i < current_length_ - 1/*unused mark*/; i++)
	{
		uint8_t read;
		buffer_->read_data(&read, sizeof(read));
		uint8_t mask = 0x80;
		for (uint32_t b = 0; b < 8; b++)
		{
			if (written >= count)
				break;
			val.push_back((read & mask) != 0);
			written++;
			mask >>= 1;
		}
	}
	return *this;
}

void rx_asn_in_stream::read_explicit (bit_string& val)
{
	uint32_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x3)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (byte_string& val)
{
	val.resize(current_length_);
	if (current_length_ > 0)
		buffer_->read_data(&val[0], current_length_);
	return *this;
}

void rx_asn_in_stream::read_explicit (string_type& val)
{
	RX_ASSERT(false);// not implemented
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (string_type& val)
{
	val.resize(current_length_);
	buffer_->read_data(&val[0], current_length_);
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (bool& val)
{
	if (current_length_ == 1)
	{
		uint8_t temp;
		buffer_->read_data(&temp, sizeof(temp));
		val = (temp != 0);
	}
	else
	{
		throw std::runtime_error(ASN_READ_INT_OVERFLOW);
	}
	return *this;
}

void rx_asn_in_stream::read_explicit (bool& val)
{
	uint8_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x2)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (float& val)
{
	uint8_t data[9];
	if (current_length_ > 9)
	{
		assert(false);
		throw std::runtime_error(ASN_READ_FLOAT_UNSUPPORTED);
	}
	buffer_->read_data(data, current_length_);
	if (current_length_ == 5 && data[0] == 8)
	{// float value
		uint8_t* valb = (uint8_t*)&val;
		valb[0] = data[4];
		valb[1] = data[3];
		valb[2] = data[2];
		valb[3] = data[1];
	}
	else if (current_length_ == 9 && data[0] == 11)
	{// double value
		double temp;
		uint8_t* valb = (uint8_t*)&temp;
		valb[0] = data[8];
		valb[1] = data[7];
		valb[2] = data[6];
		valb[3] = data[5];
		valb[4] = data[4];
		valb[5] = data[3];
		valb[6] = data[2];
		valb[7] = data[1];
		val = (float)temp;
	}
	else
	{
		assert(false);
		throw std::runtime_error(ASN_READ_FLOAT_UNSUPPORTED);
	}
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (double& val)
{
	uint8_t data[9];
	if (current_length_ > 9)
	{
		assert(false);
		throw std::runtime_error(ASN_READ_FLOAT_UNSUPPORTED);
	}
	buffer_->read_data(data, current_length_);
	if (current_length_ == 5 && data[0] == 8)
	{// float value
		float temp;
		uint8_t* valb = (uint8_t*)&temp;
		valb[0] = data[4];
		valb[1] = data[3];
		valb[2] = data[2];
		valb[3] = data[1];
		val = (double)temp;
	}
	else if (current_length_ == 9 && data[0] == 11)
	{// double value
		uint8_t* valb = (uint8_t*)&val;
		valb[0] = data[8];
		valb[1] = data[7];
		valb[2] = data[6];
		valb[3] = data[5];
		valb[4] = data[4];
		valb[5] = data[3];
		valb[6] = data[2];
		valb[7] = data[1];
	}
	else
	{
		assert(false);
		throw std::runtime_error(ASN_READ_FLOAT_UNSUPPORTED);
	}
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (rx_time_stamp& val)
{
	if (current_length_ == 8)
	{// date time

		timeval tv;

		uint32_t temps;
		uint8_t tempus[4];
		tempus[0] = 0;
		buffer_->read_data(&temps, sizeof(temps));
		buffer_->read_data(&tempus[1], 3);
		tv.tv_sec = ntohl(temps);
		int64_t templ = ntohl(*((uint32_t*)tempus));
		templ = templ * 1000000 / 0x1000000;
		tv.tv_usec = (long)templ;
		uint8_t quality;
		buffer_->read_data(&quality, 1);
		val.time = rx_time(tv);
		val.quality = quality;
	}
	else
		throw std::runtime_error(ASN_READ_WRONG_TIME_VALUE);
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (null_asn_type& val)
{
	if (current_length_ != 0)
		throw std::runtime_error(ASN_READ_NULL_VAL_ERROR);
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (asn_binary_time& val)
{
	if (current_length_ == 4)
	{// binary time without day
		buffer_->read_data(&val.mstime, sizeof(val.mstime));
		val.mstime = ntohl(val.mstime);
		val.full = false;
	}
	else if (current_length_ == 6)
	{// binary time with day
		buffer_->read_data(&val.mstime, sizeof(val.mstime));
		buffer_->read_data(&val.days, sizeof(val.days));
		val.mstime = ntohl(val.mstime);
		val.days = ntohs(val.days);
		val.full = true;
	}
	else
		throw std::runtime_error(ASN_READ_WRONG_TIME_VALUE);
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (asn_generalized_time& val)
{
	string_type temp;
	(*this) >> temp;
	//check size first
	if (temp.size() < 15)
		throw std::runtime_error(ASN_READ_WRONG_GTIME_VALUE);
	//now check for ending character
	if (*temp.rbegin() != 'z' || *temp.rbegin() != 'Z')
		throw std::runtime_error(ASN_READ_WRONG_GTIME_VALUE);

	uint32_t ifraction = 0;
	int yi = atoi(temp.substr(0, 4).c_str());
	int mi = atoi(temp.substr(4, 2).c_str());
	int di = atoi(temp.substr(6, 2).c_str());
	if (yi == 0 || mi == 0 || di == 0)
		throw std::runtime_error(ASN_READ_WRONG_GTIME_VALUE);

	int hi = atoi(temp.substr(8, 2).c_str());
	int mini = atoi(temp.substr(10, 2).c_str());
	int si = atoi(temp.substr(12, 2).c_str());
	if (temp[14] == '.')
	{// he have fraction
		int fr = atoi(temp.substr(15, (temp.size() - 1) - 15).c_str());
		if (fr == 0)
			throw std::runtime_error(ASN_READ_WRONG_GTIME_VALUE);
		ifraction = (uint32_t)(1.0 / fr * 1000000.0);
	}

	val.year = yi;
	val.month = mi;
	val.day = di;
	val.hour = hi;
	val.minute = mini;
	val.second = si;
	val.fraction = ifraction;

	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (uint64_t& val)
{
	switch (current_length_)
	{
	case 1:
		{
			uint8_t temp;
			buffer_->read_data(&temp, sizeof(temp));
			val = temp;
		}
		break;
	case 2:
		{
			uint8_t temp[2];
			buffer_->read_data(&temp, 2);
			val = (((uint64_t)temp[0]) << 8) | temp[1];
		}
		break;
	case 3:
		{
			uint8_t temp[3];
			buffer_->read_data(temp, 3);
			val = (((uint64_t)temp[0]) << 16) | (((uint64_t)temp[1]) << 8) | temp[2];
		}
		break;
	case 4:
		{
			uint8_t temp[4];
			buffer_->read_data(temp, 4);
			val = (((uint64_t)temp[0]) << 24) | (((uint64_t)temp[1]) << 16) | (((uint64_t)temp[2]) << 8) | temp[3];
		}
		break;
	case 5:
		{
			uint8_t temp[5];
			buffer_->read_data(temp, 5);
			val = (((uint64_t)temp[0]) << 32) | (((uint64_t)temp[1]) << 24) | (((uint64_t)temp[2]) << 16) | (((uint64_t)temp[3]) << 8) | temp[4];
		}
		break;
	case 6:
		{
			uint8_t temp[6];
			buffer_->read_data(temp, 6);
			val = (((uint64_t)temp[0]) << 40) | (((uint64_t)temp[1]) << 32) | (((uint64_t)temp[2]) << 24) | (((uint64_t)temp[3]) << 16) | (((uint64_t)temp[4]) << 8) | temp[5];
		}
		break;
	case 7:
		{
			uint8_t temp[7];
			buffer_->read_data(temp, 7);
			val = (((uint64_t)temp[0]) << 48) | (((uint64_t)temp[1]) << 40) | (((uint64_t)temp[2]) << 32) | (((uint64_t)temp[3]) << 24) | (((uint64_t)temp[4]) << 16) | (((uint64_t)temp[5]) << 8) | temp[6];
		}
		break;
	case 8:
		{
			uint8_t temp[8];
			buffer_->read_data(temp, 8);
			val = (((uint64_t)temp[0]) << 56) | (((uint64_t)temp[1]) << 48) | (((uint64_t)temp[2]) << 40) | (((uint64_t)temp[3]) << 32) | (((uint64_t)temp[4]) << 24) | (((uint64_t)temp[5]) << 16) | (((uint64_t)temp[6]) << 8) | temp[7];
			//val = ntohll(val);
		}
		break;
	default:
		throw std::runtime_error(ASN_READ_INT_OVERFLOW);
	}
	return *this;
}

rx_asn_in_stream& rx_asn_in_stream::operator >> (int64_t& val)
{
	switch (current_length_)
	{
	case 1:
		{
			int8_t temp;
			buffer_->read_data(&temp, sizeof(temp));
			val = temp;
		}
		break;
	case 2:
		{
			uint8_t temp[2];
			buffer_->read_data(&temp, 2);
			val = (((uint64_t)temp[0]) << 8) | temp[1];
		}
		break;
	case 3:
		{
			uint8_t temp[3];
			buffer_->read_data(temp, 3);
			val = (((uint64_t)temp[0]) << 16) | (((uint64_t)temp[1]) << 8) | temp[2];
		}
		break;
	case 4:
		{
			uint8_t temp[4];
			buffer_->read_data(temp, 4);
			val = (((uint64_t)temp[0]) << 24) | (((uint64_t)temp[1]) << 16) | (((uint64_t)temp[2]) << 8) | temp[3];
		}
		break;
	case 5:
		{
			uint8_t temp[5];
			buffer_->read_data(temp, 5);
			val = (((uint64_t)temp[0]) << 32) | (((uint64_t)temp[1]) << 24) | (((uint64_t)temp[2]) << 16) | (((uint64_t)temp[3]) << 8) | temp[4];
		}
		break;
	case 6:
		{
			uint8_t temp[6];
			buffer_->read_data(temp, 6);
			val = (((uint64_t)temp[0]) << 40) | (((uint64_t)temp[1]) << 32) | (((uint64_t)temp[2]) << 24) | (((uint64_t)temp[3]) << 16) | (((uint64_t)temp[4]) << 8) | temp[5];
		}
		break;
	case 7:
		{
			uint8_t temp[7];
			buffer_->read_data(temp, 7);
			val = (((uint64_t)temp[0]) << 48) | (((uint64_t)temp[1]) << 40) | (((uint64_t)temp[2]) << 32) | (((uint64_t)temp[3]) << 24) | (((uint64_t)temp[4]) << 16) | (((uint64_t)temp[5]) << 8) | temp[6];
		}
		break;
	case 8:
		{
			uint8_t temp[8];
			buffer_->read_data(temp, sizeof(8));
			val = (((uint64_t)temp[0]) << 56) | (((uint64_t)temp[1]) << 48) | (((uint64_t)temp[2]) << 40) | (((uint64_t)temp[3]) << 32) | (((uint64_t)temp[4]) << 24) | (((uint64_t)temp[5]) << 16) | (((uint64_t)temp[6]) << 8) | temp[7];
			//val = ntohll(val);
		}
		break;
	default:
		throw std::runtime_error(ASN_READ_INT_OVERFLOW);
	}
	return *this;
}

void rx_asn_in_stream::read_explicit (uint64_t& val)
{
	uint8_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x2)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

void rx_asn_in_stream::read_explicit (int64_t& val)
{
	uint8_t temp[2];
	buffer_->read_data(temp, sizeof(temp));
	if (temp[0] != 0x2)// integer value
		throw std::runtime_error(ASN_EXPLICIT_READ_TAG_ERROR);
	if (temp[1] > 0x7f)
		throw std::runtime_error(ASN_EXPLICIT_LENGTH_ERROR);
	current_length_ = temp[1];
	operator>>(val);
}

/*
// Class rx::asn::rx_asn_out_stream 

rx_asn_out_stream::rx_asn_out_stream (server::backward_binary_buffer* buffer, bool fixed)
{
}


rx_asn_out_stream::~rx_asn_out_stream()
{
}



dword rx_asn_out_stream::create_stream_struct_type (dword type)
{
}

dword rx_asn_out_stream::create_stream_simple_type (dword type)
{
}

void rx_asn_out_stream::start_object (dword type, dword first)
{
}

void rx_asn_out_stream::end_object ()
{
}

void rx_asn_out_stream::start_object (dword first)
{
}

void rx_asn_out_stream::skip ()
{
}

void rx_asn_out_stream::write_explicit (const byte& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const word& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const sword& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const dword& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const sdword& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const oid_t& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const bit_string& val, dword type)
{
}

size_t rx_asn_out_stream::write_length (dword len, byte* data)
{
}

size_t rx_asn_out_stream::write_type (dword type, byte* data)
{
}

void rx_asn_out_stream::write_explicit (const byte_string& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const null_asn_type val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const string_type& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (float val, dword type)
{
}

void rx_asn_out_stream::write_explicit (double val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const server::time_stamp& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (bool val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const server::asn_binary_time& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const server::asn_generalized_time& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const sqword& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const qword& val, dword type)
{
}

void rx_asn_out_stream::write_explicit (const sbyte& val, dword type)
{
}

  */
} // namespace asn
} // namespace rx

