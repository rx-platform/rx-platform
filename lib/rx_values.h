

/****************************************************************************
*
*  lib\rx_values.h
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


#ifndef rx_values_h
#define rx_values_h 1


#include "rx_ptr.h"

#define RX_NULL_TYPE		0
#define RX_BOOL_TYPE		1
#define RX_SBYTE_TYPE		2
#define RX_BYTE_TYPE		3
#define RX_SWORD_TYPE		4
#define RX_WORD_TYPE		5
#define RX_SDWORD_TYPE		6
#define RX_DWORD_TYPE		7
#define RX_SQWORD_TYPE		8
#define RX_QWORD_TYPE		9
#define RX_FLOAT_TYPE		10
#define RX_DOUBLE_TYPE		11
#define RX_STRING_TYPE		12
#define RX_TIME_TYPE		13
#define RX_UUID_TYPE		14
#define RX_BSTRING_TYPE		15
#define RX_COMPLEX_TYPE		16


#define RX_SIMPLE_VALUE_MASK	0x7f
#define RX_ARRAY_VALUE_MASK		0x80

#define IS_ARRAY_VALUE(t) ((t&RX_ARRAY_VALUE_MASK)==RX_ARRAY_VALUE_MASK)

#define RX_INVALID_INDEX_VALUE ((size_t)(-1l))

#define PI_CONST 3.14159265359
#define FLOAT_EPSILON 0.00000000001

#define RX_QUALITY_MASK			0xc0000000
#define RX_QUALITY_DETAILS_MASK	0x3fffffff
#define RX_ORIGIN_MASK			0xfff60000
#define RX_LIMIT_MASK			0x00030000
#define RX_LEVEL_MASK			0x0000ffff

// master quality
#define RX_GOOD_QUALITY			0x00000000
#define RX_UNCERTAIN_QUALITY	0x40000000
#define RX_BAD_QUALITY			0x80000000


// bad , uncertain quality codes
#define RX_Q_OUT_OF_RANGE		0x00000004
#define RX_Q_BAD_REFERENCE		0x00000008
#define RX_Q_OSCILLATORY		0x00000010


// bad quality codes
#define RX_Q_QUALITY_INVALID	0x00000001
#define RX_Q_OVERFLOW			0x00000002
#define RX_Q_FAILURE			0x00000020
#define RX_Q_DEVICE_FAILURE		0x00000040
#define RX_Q_CONFIG_ERROR		0x00000080
#define RX_Q_NOT_CONNECTED		0x00000100
#define RX_Q_TYPE_MISMATCH		0x00000200
#define RX_Q_SYNTAX_ERROR		0x00000400
#define RX_Q_DIVISION_BY_ZERO	0x00000800
#define RX_Q_OFFLINE			0x00001000

// uncertain quality codes
#define RX_Q_OLD_DATA			0x00000001
#define RX_Q_INCONSISTENT		0x00000002
#define RX_Q_INACURATE			0x00000020
#define RX_Q_INITIAL_VALUE		0x00000040

// combined quality values

#define RX_BAD_QUALITY_QUALITY_INVALID	(RX_BAD_QUALITY|RX_Q_QUALITY_INVALID)
#define RX_BAD_QUALITY_OVERFLOW			(RX_BAD_QUALITY|RX_Q_OVERFLOW)
#define RX_BAD_QUALITY_OUT_OF_RANGE		(RX_BAD_QUALITY|RX_Q_OUT_OF_RANGE)
#define RX_BAD_QUALITY_BAD_REFERENCE	(RX_BAD_QUALITY|RX_Q_BAD_REFERENCE)
#define RX_BAD_QUALITY_OSCILLATORY		(RX_BAD_QUALITY|RX_Q_OSCILLATORY)
#define RX_BAD_QUALITY_FAILURE			(RX_BAD_QUALITY|RX_Q_FAILURE)
#define RX_BAD_QUALITY_DEVICE_FAILURE	(RX_BAD_QUALITY|RX_Q_DEVICE_FAILURE)
#define RX_BAD_QUALITY_CONFIG_ERROR		(RX_BAD_QUALITY|RX_Q_CONFIG_ERROR)
#define RX_BAD_QUALITY_NOT_CONNECTED	(RX_BAD_QUALITY|RX_Q_NOT_CONNECTED)
#define RX_BAD_QUALITY_TYPE_MISMATCH	(RX_BAD_QUALITY|RX_Q_TYPE_MISMATCH)
#define RX_BAD_QUALITY_SYNTAX_ERROR		(RX_BAD_QUALITY|RX_Q_SYNTAX_ERROR)
#define RX_BAD_QUALITY_SYNTAX_ERROR		(RX_BAD_QUALITY|RX_Q_SYNTAX_ERROR)
#define RX_BAD_QUALITY_DIVISION_BY_ZERO	(RX_BAD_QUALITY|RX_Q_DIVISION_BY_ZERO)
#define RX_BAD_QUALITY_OFFLINE			(RX_BAD_QUALITY|RX_Q_OFFLINE)

#define RX_UNCERTAIN_QUALITY_OLD_DATA		(RX_UNCERTAIN_QUALITY|RX_Q_OLD_DATA)
#define RX_UNCERTAIN_QUALITY_INCONSISTENT	(RX_UNCERTAIN_QUALITY|RX_Q_INCONSISTENT)
#define RX_UNCERTAIN_QUALITY_OUT_OF_RANGE	(RX_UNCERTAIN_QUALITY|RX_Q_OUT_OF_RANGE)
#define RX_UNCERTAIN_QUALITY_BAD_REFERENCE	(RX_UNCERTAIN_QUALITY|RX_Q_BAD_REFERENCE)
#define RX_UNCERTAIN_QUALITY_OSCILLATORY	(RX_UNCERTAIN_QUALITY|RX_Q_OSCILLATORY)
#define RX_UNCERTAIN_QUALITY_INACURATE		(RX_UNCERTAIN_QUALITY|RX_Q_INACURATE)
#define RX_UNCERTAIN_QUALITY_INITIAL_VALUE	(RX_UNCERTAIN_QUALITY|RX_Q_INITIAL_VALUE)


#define RX_DEFAULT_VALUE_QUALITY		RX_UNCERTAIN_QUALITY_INITIAL_VALUE
#define RX_CONFIG_ERROR_QUALITY			RX_BAD_QUALITY_CONFIG_ERROR
#define RX_NOT_CONNECTED_QUALITY		RX_BAD_QUALITY_NOT_CONNECTED


#define RX_DEFUALT_ORIGIN		0x0
#define RX_DEFUALT_TEST_ORIGIN	0x20000000

// origin bits
#define RX_FORCED_ORIGIN		0x80000000
#define RX_BLOCKED_ORIGIN		0x40000000
#define RX_TEST_ORIGIN			0x20000000
#define RX_CALLCUALTED_ORIGIN	0x10000000
#define RX_ESTIMATED_ORIGIN		0x08000000
#define RX_LOCAL_ORIGIN			0x04000000



namespace rx {
class base_meta_reader;
class base_meta_writter;

} // namespace rx


using namespace rx;


namespace rx {

namespace values {
typedef std::uint_fast8_t rx_value_t;
class rx_value;


struct complex_value_struct
{
	double real;
	double imag;
	double amplitude()
	{
		return sqrt(real*real + imag*imag);
	}
};

union rx_value_union
{
	bool bool_value;
	uint8_t uint8_value;
	int8_t int8_value;
	uint16_t uint16_value;
	int16_t int16_value;
	uint32_t uint32_value;
	int32_t int32_value;
	uint64_t uint64_value;
	int64_t int64_value;
	float float_value;
	double double_value;
	string_type* string_value;
	byte_string* bstring_value;
	bit_string* bits_value;
	rx_time_struct time_value;
	rx_uuid* uuid_value;
	complex_value_struct* complex_value;
	std::vector<rx_value_union>* array_value;
};

#define DEFAULT_TIME_VAL (rx_time::now())






class rx_value
{

  public:
      rx_value();

      rx_value(const rx_value &right);

      rx_value (bool val, rx_time time = DEFAULT_TIME_VAL, uint32_t quality = RX_GOOD_QUALITY, uint32_t origin = RX_DEFUALT_ORIGIN);

      rx_value (int8_t val, rx_time time = DEFAULT_TIME_VAL, uint32_t quality = RX_GOOD_QUALITY, uint32_t origin = RX_DEFUALT_ORIGIN);

      rx_value (uint8_t val, rx_time time = DEFAULT_TIME_VAL, uint32_t quality = RX_GOOD_QUALITY, uint32_t origin = RX_DEFUALT_ORIGIN);

      rx_value (const bit_string& val, rx_time time = DEFAULT_TIME_VAL, uint32_t quality = RX_GOOD_QUALITY, uint32_t origin = RX_DEFUALT_ORIGIN);

      rx_value (const std::vector<int8_t>& val, rx_time time = DEFAULT_TIME_VAL, uint32_t quality = RX_GOOD_QUALITY, uint32_t origin = RX_DEFUALT_ORIGIN);

      rx_value (rx_value&& right);

      rx_value (int32_t val, rx_time time = DEFAULT_TIME_VAL, uint32_t quality = RX_GOOD_QUALITY, uint32_t origin = RX_DEFUALT_ORIGIN);

      rx_value (uint32_t val, rx_time time = DEFAULT_TIME_VAL, uint32_t quality = RX_GOOD_QUALITY, uint32_t origin = RX_DEFUALT_ORIGIN);

      rx_value (const string_type& val, rx_time time = DEFAULT_TIME_VAL, uint32_t quality = RX_GOOD_QUALITY, uint32_t origin = RX_DEFUALT_ORIGIN);

      ~rx_value();

      rx_value & operator=(const rx_value &right);

      bool operator==(const rx_value &right) const;

      bool operator!=(const rx_value &right) const;


      void get_string (string_type& val);

      bool is_good () const;

      bool is_bad () const;

      bool is_uncertain () const;

      bool is_test () const;

      bool is_substituted () const;

      bool is_array () const;

      bool operator > (const rx_value& right);

      bool operator < (const rx_value& right);

      operator int () const;

      void set_substituted ();

      void set_test ();

      void get_type_string (string_type& val);

      bool serialize (base_meta_writter& stream) const;

      bool deserialize (base_meta_reader& stream);

      bool adapt_quality_to_mode (const rx_mode_type& mode);

      bool serialize_value (base_meta_writter& stream) const;

      bool deserialize_value (base_meta_reader& stream);

      operator uint32_t () const;

      operator bool () const;

      void set_offline ();

      void set_good_locally ();


      const rx_value_t get_type () const
      {
        return type_;
      }


      const uint32_t get_quality () const
      {
        return quality_;
      }

      void set_quality (uint32_t value)
      {
        quality_ = value;
      }


      const rx_time get_time () const
      {
        return time_;
      }

      void set_time (rx_time value)
      {
        time_ = value;
      }



  protected:

  private:

      bool serialize_value (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const;

      bool deserialize_value (base_meta_reader& stream, uint8_t type, rx_value_union& value);



      void clear_union (uint8_t type, rx_value_union& value);

      void copy_union (uint8_t type, rx_value_union& to, const rx_value_union& from);


      rx_value_union value_;

      rx_value_t type_;

      uint32_t quality_;

      rx_time time_;

      uint32_t origin_;


};






class const_values_storage
{
  protected:
	uint8_t buffer[sizeof(pointers::reference_object)];
public:
	template<typename typeT>
	const_values_storage(const typeT& val)
	{
		if (allways_good_value<typeT>::is_large())
		{

		}
		else
		{
			ZeroMemory(buffer, sizeof(buffer));
			memcpy(buffer, &val, sizeof(typeT));
		}
	}

  public:
      const_values_storage();


  protected:

  private:


};







template <typename valT>
class allways_good_value : public const_values_storage  
{
public:
	static constexpr bool is_large() noexcept
	{
		return sizeof(valT)>sizeof(const_values_storage);
	}

  public:
      allways_good_value (const valT&  right);

      allways_good_value (valT&& right);


      operator valT ()
      {
		  return value_;
      }

      valT& operator = (const valT& right);

      bool is_bad () const;

      bool is_uncertain () const;

      bool is_test () const;

      bool is_substituted () const;

      bool is_array () const;

      bool is_good () const;

      bool can_operate (bool test_mode) const;

      operator valT () const
      {
		  return value_;
      }

      void get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const;


  protected:

  private:

      valT& value ();



};


// Parameterized Class rx::values::allways_good_value

template <typename valT>
allways_good_value<valT>::allways_good_value (const valT&  right)
{
}

template <typename valT>
allways_good_value<valT>::allways_good_value (valT&& right)
{
}



template <typename valT>
valT& allways_good_value<valT>::operator = (const valT& right)
{
	return value_;
}

template <typename valT>
bool allways_good_value<valT>::is_bad () const
{
  return false;
}

template <typename valT>
bool allways_good_value<valT>::is_uncertain () const
{
  return false;
}

template <typename valT>
bool allways_good_value<valT>::is_test () const
{
  return false;
}

template <typename valT>
bool allways_good_value<valT>::is_substituted () const
{
  return false;
}

template <typename valT>
bool allways_good_value<valT>::is_array () const
{
  rx_value temp(value_);
  return temp.is_array();
}

template <typename valT>
bool allways_good_value<valT>::is_good () const
{
  return true;
}

template <typename valT>
bool allways_good_value<valT>::can_operate (bool test_mode) const
{
  return true;
}

template <typename valT>
void allways_good_value<valT>::get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const
{
	if(is_large())
		val = rx_value(value());
	else
		val=
	val.set_time(ts);
	if(mode.is_off())
		val.set_quality(RX_BAD_QUALITY_OFFLINE);
	else
		val.set_good_locally();
	if(mode.is_test())
		val.set_test();
}

template <typename valT>
valT& allways_good_value<valT>::value ()
{
	if (is_large())
	{
		return *(*(reinterpret_cast<valT*>(&buffer)));
	}
	else
	{
		return *(reinterpret_cast<valT*>(&buffer))
	}
}


} // namespace values
} // namespace rx



#endif


// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
	value_ = right.value_;
	time_stamp_ = right.time_stamp_;

	value_ = value;
	time_stamp_ = time_stamp;

	return value_;

	return value_ == right;

	return value_ != right;

	return value_ > right;

	return value_ < right.value_;

	value_ = right.value_;
	time_stamp_ == right.m_timestamp;

	rx_value_ = rx_value(value);

	rx_value_ = right;

	rx_value_ = rx_value(right);

	return rx_value_;

  return rx_value_==rx_value(right);

  return rx_value_!=rx_value(right);

  return rx_value_>rx_value(right);

  return rx_value_<rx_value(right);

	return static_cast<valT>(rx_value_);

	rx_value_ = rx_value(right);
	return rx_value_;

	return rx_value_.is_bad();

	return rx_value_.is_bad();

	return rx_value_.is_test();

	return rx_value_.is_substituted();

	return rx_value_.is_array();

	return rx_value_.is_good();

#endif
