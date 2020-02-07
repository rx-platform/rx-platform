

/****************************************************************************
*
*  lib\rx_values.h
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


#ifndef rx_values_h
#define rx_values_h 1


#include "rx_ptr.h"

// union's value types
#define RX_NULL_TYPE			0x00
#define RX_BOOL_TYPE			0x01
#define RX_INT8_TYPE			0x02
#define RX_UINT8_TYPE			0x03
#define RX_INT16_TYPE			0x04
#define RX_UINT16_TYPE			0x05
#define RX_INT32_TYPE			0x06
#define RX_UINT32_TYPE			0x07
#define RX_INT64_TYPE			0x08
#define RX_UINT64_TYPE			0x09
#define RX_FLOAT_TYPE			0x0a
#define RX_DOUBLE_TYPE			0x0b
#define RX_COMPLEX_TYPE			0x0c
#define RX_STRING_TYPE			0x0d
#define RX_TIME_TYPE			0x0e
#define RX_UUID_TYPE			0x0f
#define RX_BYTES_TYPE			0x10
#define RX_OBJECT_TYPE			0x11
#define RX_CLASS_TYPE			0x12
#define RX_NODE_ID_TYPE			0x13

#define RX_SIMPLE_VALUE_MASK	0x1f


// value types for object and class types
#define RX_DEFINITION_TYPE_MASK	0x60
#define RX_BINARY_VALUE			0x00
#define RX_JSON_VALUE			0x20
#define RX_SCRIPT_VALUE			0x40


#define RX_STRIP_ARRAY_MASK		0x7f
#define RX_ARRAY_VALUE_MASK		0x80

#define RX_SIMPLE_TYPE(t) ((RX_STRIP_ARRAY_MASK&t))
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
#define RX_DEAD_QUALITY			0xc0000000


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
class base_meta_writer;

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
rx_value_t inner_get_type(tl::type2type<char>);
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
template<>
rx_value_t inner_get_type(tl::type2type<typename rx::rx_time>);
template<>
rx_value_t inner_get_type(tl::type2type<typename rx::rx_node_id>);


template<typename typeT>
rx_value_t get_type()
{
	return inner_get_type(tl::type2type<std::remove_const_t<std::remove_reference_t<typeT> > >());
}


struct complex_value_struct
{
	double real;
	double imag;
	double amplitude() const;
	string_type to_string() const;
	bool parse_string(const string_type& str);
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

	rx_time_struct time_value;

#ifdef RX_VALUE_SIZE_16
	complex_value_struct complex_value;
	rx_uuid uuid_value;
#else
	complex_value_struct* complex_value;
	rx_uuid* uuid_value;
#endif

	string_type* string_value;
	byte_string* bytes_value;
	bit_string* bits_value;
	rx_node_id* node_id_value;

	std::vector<rx_value_union>* array_value;
};

#define DEFAULT_TIME_VAL (rx_time::now())

#define INVALID_INDEX_VALUE ((size_t)(-1))

class rx_value_storage;
template<typename typeT>
typeT extract_value(const rx_value_storage& from, const typeT& default_value);

//	This is a class that only cares about storage of value.



class rx_value_storage 
{
public:
	rx_value_storage(const rx_value_storage& right);
	rx_value_storage(rx_value_storage&& right) noexcept;
	rx_value_storage& operator=(const rx_value_storage& right);
	rx_value_storage& operator=(rx_value_storage&& right) noexcept;

	template<typename typeT>
	friend typeT extract_value(const rx_value_storage& from, const typeT& default_value);

  public:
      rx_value_storage();

      ~rx_value_storage();

      bool operator==(const rx_value_storage &right) const;

      bool operator!=(const rx_value_storage &right) const;

      bool operator<(const rx_value_storage &right) const;

      bool operator>(const rx_value_storage &right) const;

      bool operator<=(const rx_value_storage &right) const;

      bool operator>=(const rx_value_storage &right) const;


      bool serialize (base_meta_writer& writer) const;

      bool deserialize (base_meta_reader& reader);

      string_type to_string () const;

      void parse (const string_type& str);

      rx_value_t get_value_type () const;

      string_type get_type_string () const;

      bool expresion_equality (const rx_value_storage& right) const;

      bool exact_equality (const rx_value_storage& right) const;

      bool convert_to (rx_value_t type);

      bool is_complex () const;

      bool is_numeric () const;

      bool is_integer () const;

      bool is_float () const;

      bool is_null () const;

      complex_value_struct get_complex_value () const;

      double get_float_value () const;

      int64_t get_integer_value (rx_value_t* min_type = nullptr) const;

      bool get_bool_value () const;

      bool is_array () const;

      bool set_from_complex (const complex_value_struct& val, rx_value_t type);

      bool set_from_float (double val, rx_value_t type);

      bool set_from_integer (int64_t val, rx_value_t type);

      rx_value_storage operator + (const rx_value_storage& right) const;

      rx_value_storage operator - (const rx_value_storage& right) const;

      rx_value_storage operator * (const rx_value_storage& right) const;

      rx_value_storage operator / (const rx_value_storage& right) const;

      rx_value_storage operator % (const rx_value_storage& right) const;

      rx_node_id get_id_value () const;

      string_type get_string_value () const;

      bool weak_serialize_value (const string_type& name, base_meta_writer& writer) const;

      bool weak_deserialize_value (const string_type& name, base_meta_reader& reader);

	  template<typename T>
	  void assign_static(T&& right)
	  {
		  destroy_value(value_, value_type_);
		  value_type_ = get_type<T>();
		  assign(std::forward<T>(right));
	  }

  protected:

  private:

      int64_t get_int_value (rx_value_t type, const rx_value_union& value, rx_value_t* min_type = nullptr, size_t idx = INVALID_INDEX_VALUE) const;

      double get_float_value (rx_value_t type, const rx_value_union& value, size_t idx = INVALID_INDEX_VALUE) const;

      bool get_bool_value (rx_value_t type, const rx_value_union& value, size_t idx = INVALID_INDEX_VALUE) const;

      bool set_from_complex (const complex_value_struct& val, rx_value_t type, rx_value_union& where);

      bool set_from_float (double val, rx_value_t type, rx_value_union& where);

      bool set_from_integer (int64_t val, rx_value_t type, rx_value_union& where);

      bool is_simple_type (rx_value_t type) const;

      rx_value_t get_arithmetic_result_type (rx_value_t left, rx_value_t right, bool add) const;

	  void assign(bool val);
	  void assign(int8_t val);
	  void assign(uint8_t val);
	  void assign(int16_t val);
	  void assign(uint16_t val);
	  void assign(int32_t val);
	  void assign(uint32_t val);
	  void assign(int64_t val);
	  void assign(uint64_t val);

	  void assign(float val);
	  void assign(double val);
	  void assign(std::complex<double> val);

	  void assign(rx_time_struct val);
	  void assign(rx_uuid val);

	  void assign(string_type&& val);
	  void assign(const string_type& val);
	  void assign(const char* val);

	  void assign(byte_string&& val);
	  void assign(const byte_string& val);

	  void assign(bit_string&& val);
	  void assign(const bit_string& val);

	  void assign(std::vector<rx_value_union>&& val);
	  void assign(const std::vector<rx_value_union>& val);

	  static void assign_value(rx_value_union& left, const rx_value_union& right, rx_value_t type);
	  static void assign_value(rx_value_union& left, rx_value_union&& right, rx_value_t type);
	  static void destroy_value(rx_value_union& who, rx_value_t type);

	  static bool exact_equality(const rx_value_union& left, const rx_value_union& right, rx_value_t type);

	  static bool serialize_value(base_meta_writer& writer, const rx_value_union& who, rx_value_t type, const string_type& name);
	  static bool deserialize_value(base_meta_reader& reader, rx_value_union& who, rx_value_t type);

	  static bool convert_union(rx_value_union& what, rx_value_t source, rx_value_t target);

      rx_value_t value_type_;

      rx_value_union value_;


};






class rx_simple_value 
{
  public:template<typename typeT>
	  void assign_static(typeT&& val, rx_time ts = rx_time::null_time(), uint32_t quality = RX_GOOD_QUALITY)
	  {
		  storage_.assign_static(std::forward<typeT>(val));
	  }
	  ~rx_simple_value() = default;
	  rx_simple_value() = default;
	  rx_simple_value(const rx_simple_value &right);
	  rx_simple_value(rx_simple_value&& right) noexcept;
	  rx_simple_value& operator=(rx_simple_value&& right) noexcept;
	  rx_simple_value & operator=(const rx_simple_value &right);

  public:
      rx_simple_value (const rx_value_storage& storage);

      bool operator==(const rx_simple_value &right) const;

      bool operator!=(const rx_simple_value &right) const;


      bool is_bad () const;

      bool is_uncertain () const;

      bool is_test () const;

      bool is_substituted () const;

      bool is_array () const;

      bool is_good () const;

      bool can_operate (bool test_mode) const;

      bool serialize (const string_type& name, base_meta_writer& writter) const;

      bool deserialize (const string_type& name, base_meta_reader& reader);

      void dump_to_stream (std::ostream& out) const;

      void parse_from_stream (std::istream& in);

      void get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const;

      bool convert_to (rx_value_t type);

      rx_value_t get_type () const;

      void parse (const string_type& str);

      bool is_null () const;

      bool is_complex () const;

      bool is_numeric () const;

      bool is_integer () const;

      bool is_float () const;


      const rx_value_storage& get_storage () const
      {
        return storage_;
      }



  protected:

  private:


      rx_value_storage storage_;


};







class rx_value 
{
public:
	template<typename typeT>
	void assign_static(typeT&& val, rx_time ts = rx_time::now(), uint32_t quality = RX_GOOD_QUALITY)
	{
		time_ = ts;
		storage_.assign_static(std::forward<typeT>(val));
		quality_ = quality;
	}
	~rx_value() = default;
	rx_value();
	rx_value(const rx_value &right);
	rx_value(rx_value&& right) noexcept;
	rx_value& operator=(rx_value&& right) noexcept;
	rx_value & operator=(const rx_value &right);

	bool operator==(const rx_value& right) const
	{
		return storage_ == right.storage_;
	}
	bool operator!=(const rx_value& right) const
	{
		return storage_ != right.storage_;
	}
	bool operator>(const rx_value& right) const
	{
		return storage_ > right.storage_;
	}
	bool operator<(const rx_value& right) const
	{
		return storage_ < right.storage_;
	}
	bool operator>=(const rx_value& right) const
	{
		return storage_ >= right.storage_;
	}
	bool operator<=(const rx_value& right) const
	{
		return storage_ <= right.storage_;
	}

  public:

      bool is_good () const;

      bool is_bad () const;

      bool is_uncertain () const;

      bool is_test () const;

      bool is_substituted () const;

      bool is_array () const;

      void set_substituted ();

      void set_test ();

      bool serialize (const string_type& name, base_meta_writer& stream) const;

      bool deserialize (const string_type& name, base_meta_reader& stream);

      bool adapt_quality_to_mode (const rx_mode_type& mode);

      void set_offline ();

      void set_good_locally ();

      rx_time set_time (rx_time time);

      static rx_value from_simple (const rx_simple_value& value, rx_time ts);

      static rx_value from_simple (rx_simple_value&& value, rx_time ts);

      rx_simple_value to_simple () const;

      bool convert_to (rx_value_t type);

      rx_value_t get_type () const;

      void dump_to_stream (std::ostream& out) const;

      bool is_null () const;

      bool is_complex () const;

      bool is_numeric () const;

      bool is_integer () const;

      bool is_float () const;

      complex_value_struct get_complex_value () const;

      double get_float_value () const;

      int64_t get_integer_value (rx_value_t* min_type = nullptr) const;

      bool get_bool_value () const;

      bool set_from_complex (const complex_value_struct& val, rx_value_t type);

      bool set_from_float (double val, rx_value_t type);

      bool set_from_integer (int64_t val, rx_value_t type);

      rx_value operator + (const rx_value& right) const;

      rx_value operator - (const rx_value& right) const;

      rx_value operator * (const rx_value& right) const;

      rx_value operator / (const rx_value& right) const;

      rx_value operator % (const rx_value& right) const;

      bool is_dead () const;


      const rx_value_storage& get_storage () const
      {
        return storage_;
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


      const uint32_t get_origin () const
      {
        return origin_;
      }



  protected:

  private:

      void handle_quality_after_arithmetic ();



      rx_value_storage storage_;


      uint32_t quality_;

      rx_time time_;

      uint32_t origin_;

      time_compare_type default_time_compare_;


};







class rx_timed_value 
{
public:
	template<typename typeT>
	void assign_static(typeT&& val, rx_time ts = rx_time::now(), uint32_t quality = RX_GOOD_QUALITY)
	{
		time_ = ts;
		storage_.assign_static(std::forward<typeT>(val));
	}
	~rx_timed_value() = default;
	rx_timed_value();
	rx_timed_value(const rx_timed_value &right);
	rx_timed_value(rx_timed_value&& right) noexcept;
	rx_timed_value& operator=(rx_timed_value&& right) noexcept;
	rx_timed_value & operator=(const rx_timed_value &right) ;

  public:
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

      bool serialize (const string_type& name, base_meta_writer& writter) const;

      bool deserialize (const string_type& name, base_meta_reader& reader);

      void dump_to_stream (std::ostream& out) const;

      void parse_from_stream (std::istream& in);

      rx_time set_time (rx_time time);

      bool compare (const rx_timed_value& right, time_compare_type time_compare) const;

      static rx_timed_value from_simple (const rx_simple_value& value, rx_time ts);

      static rx_timed_value from_simple (rx_simple_value&& value, rx_time ts);

      rx_simple_value to_simple () const;

      bool convert_to (rx_value_t type);

      rx_value_t get_type () const;

      bool is_null () const;

      bool is_complex () const;

      bool is_numeric () const;

      bool is_integer () const;

      bool is_float () const;


      const rx_value_storage& get_storage () const
      {
        return storage_;
      }



      rx_time get_time () const
      {
        return time_;
      }



  protected:

  private:


      rx_value_storage storage_;


      rx_time time_;

      time_compare_type default_time_compare_;


};


} // namespace values
} // namespace rx

namespace rx
{
void fill_quality_string(values::rx_value val, string_type& q);
}


#endif
