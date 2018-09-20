

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
enum time_compare_type
{
	time_compare_skip,
	time_compare_ms_accurate,
	time_compare_exact,
};

namespace values {
const size_t rx_pointer_optimization_size = 2*sizeof(void*);
typedef std::uint_fast8_t rx_value_t;
class rx_value;

template<typename typeT>
rx_value_t inner_get_type(tl::type2type<typeT>);

template<>
rx_value_t inner_get_type(tl::type2type<bool>);
template<>
rx_value_t inner_get_type(tl::type2type<int8_t>);
template<>
rx_value_t inner_get_type(tl::type2type<uint8_t>);
template<>
rx_value_t inner_get_type(tl::type2type<int16_t>);
template<>
rx_value_t inner_get_type(tl::type2type<uint16_t>);
template<>
rx_value_t inner_get_type(tl::type2type<int32_t>);
template<>
rx_value_t inner_get_type(tl::type2type<uint32_t>);
template<>
rx_value_t inner_get_type(tl::type2type<int64_t>);
template<>
rx_value_t inner_get_type(tl::type2type<uint64_t>);
template<>
rx_value_t inner_get_type(tl::type2type<float>);
template<>
rx_value_t inner_get_type(tl::type2type<double>);
template<>
rx_value_t inner_get_type(tl::type2type<typename std::string>);


template<typename typeT>
rx_value_t get_type()
{
	return inner_get_type(tl::type2type<std::remove_const_t<std::remove_reference_t<typeT> > >());
}


//	This is a class that only cares about storage of value.
//	It does not have enything to do with type safety.



class rx_value_storage
{
private:

	template<typename typeT>
	static constexpr bool is_stored_as_pointer() noexcept
	{
		return sizeof(typeT)>rx_pointer_optimization_size;
	}
	template<typename typeT>
	static constexpr bool static_check_size() noexcept
	{
		return (sizeof(typeT) < rx_pointer_optimization_size);
	}
#pragma pack(push,1)
	struct value_storgae_internal
	{
		uint8_t data[rx_pointer_optimization_size];
	};
	value_storgae_internal data_;
#pragma pack(pop)
public:
	rx_value_storage()
	{
		memzero(&data_, sizeof(data_));
	}
	rx_value_storage(const rx_value_storage& right) = delete;
	rx_value_storage(rx_value_storage&& right) noexcept
	{
		memcpy(data_.data, right.data_.data, sizeof(data_.data));
	}
	~rx_value_storage() = default;
	rx_value_storage& operator=(const rx_value_storage& right) = delete;
	rx_value_storage& operator=(rx_value_storage&& right) noexcept
	{
		if (this != &right)
		{
			memcpy(data_.data, &right.data_.data, sizeof(data_.data));
		}
		return *this;
	}

	template<typename typeT>
	rx_value_storage(typeT val)
	{
		// this should be turned off with optimizer
		if (is_stored_as_pointer<typeT>())
		{
			typeT** ptr = (reinterpret_cast<typeT**>(&data_.data[0]));
			*ptr= new typeT(val);
		}
		else
		{
			typeT* ptr = reinterpret_cast<typeT*>(&data_.data[0]);
			ptr = new(ptr)typeT(val);
		}
	}
	template<typename typeT>
	rx_value_storage(typeT val,rx_time time)
	{
		// this should be turned off with optimizer
		if (is_stored_as_pointer<typeT>())
		{
			typeT** ptr = (reinterpret_cast<typeT**>(&data_.data[0]));
			*ptr = new typeT(val);
		}
		else
		{
			typeT* ptr = reinterpret_cast<typeT*>(&data_.data[0]);
			ptr = new(ptr)typeT(val);
		}
	}
	template<typename typeT>
	typeT& value()
	{
		// this should be turned off with optimizer
		if (is_stored_as_pointer<typeT>())
		{
			typeT** ptr = reinterpret_cast<typeT**>(&data_.data[0]);
			return **ptr;
		}
		else
		{
			return *(reinterpret_cast<typeT*>(&data_.data[0]));
		}
	}
	template<typename typeT>
	const typeT& value() const
	{
		// this should be turned off with optimizer
		if (is_stored_as_pointer<const typeT>())
		{
			const typeT*const* ptr = (reinterpret_cast<const typeT*const*>(&data_.data[0]));
			return **ptr;
		}
		else
		{
			return *(reinterpret_cast<const typeT*>(&data_.data[0]));
		}
	}
	template<typename typeT>
	void destroy_value()
	{
		// this should be turned off with optimizer
		if (is_stored_as_pointer<typeT>())
		{
			delete (*(reinterpret_cast<typeT**>(data_.data)));
		}
		else
		{
			// call the destructor explicitly
			(*(reinterpret_cast<typeT*>(data_.data))).~typeT();
		}

	}
	template<typename typeT>
	void assign(const typeT& val)
	{
		// this should be turned off with optimizer
		if (is_stored_as_pointer<typeT>())
		{
			*(reinterpret_cast<typeT**>(data_.data)) = new typeT(val);
		}
		else
		{
			typeT* ptr = reinterpret_cast<typeT*>(data_.data);
			ptr = new(ptr)typeT(val);
		}
	}
	template<typename typeT>
	void assign(typeT&& val)
	{
		// this should be turned off with optimizer
		if (is_stored_as_pointer<typeT>())
		{
			*(reinterpret_cast<typeT**>(data_.data)) = new typeT(val);
		}
		else
		{
			typeT* ptr = reinterpret_cast<typeT*>(data_.data);
			ptr = new(ptr)typeT(val);
		}
	}
	template<typename typeT>
	typeT& allocate_empty()
	{
		// this should be turned off with optimizer
		if (is_stored_as_pointer<typeT>())
		{
			typeT** ptr = (reinterpret_cast<typeT**>(data_.data));
			*ptr = new typeT;
			return **ptr;
		}
		else
		{
			typeT* ptr = reinterpret_cast<typeT*>(data_.data);
			ptr = new(ptr)typeT();
			return *ptr;
		}
	}

  public:

      bool serialize (base_meta_writter& writter, rx_value_t type) const;

      bool deserialize (base_meta_reader& reader, rx_value_t& type);

      void dump_to_stream (std::ostream& out, rx_value_t type) const;

      void parse_from_stream (std::istream& in, rx_value_t type);

      void destroy_by_type (const rx_value_t type);

      void assign_storage (const rx_value_storage& right, rx_value_t type);

      bool compare (const rx_value_storage& right, rx_value_t type) const;


  protected:

  private:


};


void destroy_value_storage(rx_value_storage& storage, rx_value_t type);


struct complex_value_struct
{
	double real;
	double imag;
	double amplitude() const
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

	rx_value(rx_value&& right) noexcept;
	rx_value & operator=(rx_value&& right) noexcept;

	template<typename typeT>
	rx_value(typeT val) noexcept
		: storage_(std::move(val))
		, value_type_(get_type<typeT>())
		, default_time_compare_(time_compare_skip)
	{
	}
	template<typename typeT>
	rx_value(typeT val, rx_time time) noexcept
		: storage_(std::move(val))
		, value_type_(get_type<typeT>())
		, time_(time)
		, default_time_compare_(time_compare_skip)
	{
	}


  public:

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

      rx_time set_time (rx_time time);


      const rx_value_t get_value_type () const
      {
        return value_type_;
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



  protected:

  private:

      bool serialize_value (base_meta_writter& stream, uint8_t type, const rx_value_union& value) const;

      bool deserialize_value (base_meta_reader& stream, uint8_t type, rx_value_union& value);

      void clear_storage (rx_value_storage& data, rx_value_t type);

      void assign_storage (rx_value_storage& left, const rx_value_storage& right, rx_value_t type);



      void clear_union (uint8_t type, rx_value_union& value);

      void copy_union (uint8_t type, rx_value_union& to, const rx_value_union& from);


      rx_value_storage storage_;


      rx_value_union value_;

      rx_value_t value_type_;

      uint32_t quality_;

      rx_time time_;

      uint32_t origin_;

      time_compare_type default_time_compare_;


};






class rx_simple_value
{
  public:
	  template<typename typeT>
	  rx_simple_value(const typeT& val)
			: storage_(val)
			, value_type_(get_type<typeT>())
	  {
	  }
	  template<typename typeT>
	  rx_simple_value(typeT&& val) noexcept
			: storage_(std::move(val))
			, value_type_(get_type<typeT>())
	  {
	  }
	  template<typename typeT>
	  rx_simple_value& operator=(const typeT& val)
	  {
		  clear_storage(storage_, value_type_);
		  value_type_ = get_type<typeT>();
		  storage_.assign<typeT>(val);
		  return *this;
	  }
	  template<typename typeT>
	  rx_simple_value& operator=(typeT&& val) noexcept
	  {
		  clear_storage(storage_, value_type_);
		  value_type_ = get_type<typeT>();
		  storage_.assign<typeT>(std::move(val));
		  return *this;
	  }
	  rx_simple_value(rx_simple_value&& right) noexcept;

  public:
      rx_simple_value();

      rx_simple_value(const rx_simple_value &right);

      ~rx_simple_value();

      rx_simple_value & operator=(const rx_simple_value &right);

      bool operator==(const rx_simple_value &right) const;

      bool operator!=(const rx_simple_value &right) const;


      bool is_bad () const;

      bool is_uncertain () const;

      bool is_test () const;

      bool is_substituted () const;

      bool is_array () const;

      bool is_good () const;

      bool can_operate (bool test_mode) const;

      void get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const;

      bool serialize (base_meta_writter& writter) const;

      bool deserialize (base_meta_reader& reader);

      void dump_to_stream (std::ostream& out) const;

      void parse_from_stream (std::istream& in);


  protected:

  private:

      void clear_storage (rx_value_storage& data, rx_value_t type);

      void assign_storage (rx_value_storage& left, const rx_value_storage& right, rx_value_t type);



      rx_value_storage storage_;


      rx_value_t value_type_;


};






class rx_timed_value
{
public:
	template<typename typeT>
	rx_timed_value(const typeT& val)
		: storage_(val)
		, value_type_(get_type<typeT>())
		, default_time_compare_(time_compare_skip)
	{
	}
	template<typename typeT>
	rx_timed_value(typeT&& val) noexcept
		: storage_(std::move(val))
		, value_type_(get_type<typeT>())
		, default_time_compare_(time_compare_skip)
	{
	}
	template<typename typeT>
	rx_timed_value(typeT&& val,rx_time time) noexcept
		: storage_(std::move(val))
		, value_type_(get_type<typeT>())
		, time_(time)
		, default_time_compare_(time_compare_skip)
	{
	}
	template<typename typeT>
	rx_timed_value& operator=(const typeT& val)
	{
		clear_storage(storage_, value_type_);
		value_type_ = get_type<typeT>();
		storage_.assign<typeT>(val);
		time_ = val.time_;
		return *this;
	}
	template<typename typeT>
	rx_timed_value& operator=(typeT&& val) noexcept
	{
		clear_storage(storage_, value_type_);
		value_type_ = get_type<typeT>();
		storage_.assign<typeT>(std::move(val));
		time_ = val.time_;
		return *this;
	}
	rx_timed_value(rx_timed_value&& right) noexcept;

  public:
      rx_timed_value();

      rx_timed_value(const rx_timed_value &right);

      ~rx_timed_value();

      rx_timed_value & operator=(const rx_timed_value &right);

      bool operator==(const rx_timed_value &right) const;

      bool operator!=(const rx_timed_value &right) const;


      bool is_bad () const;

      bool is_uncertain () const;

      bool is_test () const;

      bool is_substituted () const;

      bool is_array () const;

      bool is_good () const;

      bool can_operate (bool test_mode) const;

      void get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const;

      bool serialize (base_meta_writter& writter) const;

      bool deserialize (base_meta_reader& reader);

      void dump_to_stream (std::ostream& out) const;

      void parse_from_stream (std::istream& in);

      rx_time set_time (rx_time time);

      bool compare (const rx_timed_value& right, time_compare_type time_compare) const;


  protected:

  private:

      void clear_storage (rx_value_storage& data, rx_value_t type);

      void assign_storage (rx_value_storage& left, const rx_value_storage& right, rx_value_t type);



      rx_value_storage storage_;


      rx_value_t value_type_;

      rx_time time_;

      time_compare_type default_time_compare_;


};


} // namespace values
} // namespace rx



#endif
