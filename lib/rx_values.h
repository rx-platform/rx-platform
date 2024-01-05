

/****************************************************************************
*
*  lib\rx_values.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_values_h
#define rx_values_h 1


#include "type_lists.h"

#include "common/rx_common.h"


// dummy
#include "dummy.h"

namespace rx {
class base_meta_reader;
class base_meta_writer;

} // namespace rx


using namespace rx;


namespace rx {

enum class time_compare_type
{
	skip,
	ms_accurate,
	exact,
};

namespace values {



struct complex_value : public complex_value_struct
{
    double amplitude() const;
    string_type to_string() const;
    bool parse_string(const string_type& str);
};


#define DEFAULT_TIME_VAL (rx_time::now())



const size_t rx_pointer_optimization_size = 2*sizeof(void*);
class rx_value;
class rx_simple_value;
class rx_timed_value;


bool extract_value(const typed_value_type& from, const bool& default_value);
int8_t extract_value(const typed_value_type& from, const int8_t& default_value);
uint8_t extract_value(const typed_value_type& from, const uint8_t& default_value);
int16_t extract_value(const typed_value_type& from, const int16_t& default_value);
uint16_t extract_value(const typed_value_type& from, const uint16_t& default_value);
int32_t extract_value(const typed_value_type& from, const int32_t& default_value);
uint32_t extract_value(const typed_value_type& from, const uint32_t& default_value);
int64_t extract_value(const typed_value_type& from, const int64_t& default_value);
uint64_t extract_value(const typed_value_type& from, const uint64_t& default_value);
float extract_value(const typed_value_type& from, const float& default_value);
double extract_value(const typed_value_type& from, const double& default_value);
string_type extract_value(const typed_value_type& from, const string_type& default_value);
byte_string extract_value(const typed_value_type& from, const byte_string& default_value);
rx_time_struct extract_value(const typed_value_type& from, const rx_time_struct& default_value);
rx_uuid_t extract_value(const typed_value_type& from, const rx_uuid_t& default_value);
string_array extract_value(const typed_value_type& from, const string_array& default_value);

std::vector<bool> extract_value(const typed_value_type& from, const std::vector<bool>& default_value);
std::vector<int8_t> extract_value(const typed_value_type& from, const std::vector<int8_t>& default_value);
std::vector<uint8_t> extract_value(const typed_value_type& from, const std::vector<uint8_t>& default_value);
std::vector<int16_t> extract_value(const typed_value_type& from, const std::vector<int16_t>& default_value);
std::vector<uint16_t> extract_value(const typed_value_type& from, const std::vector<uint16_t>& default_value);
std::vector<int32_t> extract_value(const typed_value_type& from, const std::vector<int32_t>& default_value);
std::vector<uint32_t> extract_value(const typed_value_type& from, const std::vector<uint32_t>& default_value);
std::vector<int64_t> extract_value(const typed_value_type& from, const std::vector<int64_t>& default_value);
std::vector<uint64_t> extract_value(const typed_value_type& from, const std::vector<uint64_t>& default_value);
std::vector<float> extract_value(const typed_value_type& from, const std::vector<float>& default_value);
std::vector<double> extract_value(const typed_value_type& from, const std::vector<double>& default_value);
std::vector<string_type> extract_value(const typed_value_type& from, const std::vector<string_type>& default_value);
std::vector<byte_string> extract_value(const typed_value_type& from, const std::vector<byte_string>& default_value);
std::vector<rx_time_struct> extract_value(const typed_value_type& from, const std::vector<rx_time_struct>& default_value);
std::vector<rx_time> extract_value(const typed_value_type& from, const std::vector<rx_time>& default_value);
std::vector<rx_uuid_t> extract_value(const typed_value_type& from, const std::vector<rx_uuid_t>& default_value);
std::vector<rx_uuid> extract_value(const typed_value_type& from, const std::vector<rx_uuid>& default_value);
std::vector<rx_node_id> extract_value(const typed_value_type& from, const std::vector<rx_node_id>& default_value);

bool assign_value(typed_value_type& from, bool value);;
bool assign_value(typed_value_type& from, int8_t value);
bool assign_value(typed_value_type& from, uint8_t value);
bool assign_value(typed_value_type& from, int16_t value);
bool assign_value(typed_value_type& from, uint16_t value);
bool assign_value(typed_value_type& from, int32_t value);
bool assign_value(typed_value_type& from, uint32_t value);
bool assign_value(typed_value_type& from, int64_t value);
bool assign_value(typed_value_type& from, uint64_t value);
bool assign_value(typed_value_type& from, float value);
bool assign_value(typed_value_type& from, double value);
bool assign_value(typed_value_type& from, string_view_type value);
bool assign_value(typed_value_type& from, const string_type& value);
bool assign_value(typed_value_type& from, const char* value);
bool assign_value(typed_value_type& from, const complex_value_struct& value);
bool assign_value(typed_value_type& from, rx_time_struct value);
bool assign_value(typed_value_type& from, rx_time value);
bool assign_value(typed_value_type& from, const rx_uuid_t& value);
bool assign_value(typed_value_type& from, const byte_string& value);
bool assign_value(typed_value_type& from, const std::vector<rx_simple_value>& value);
bool assign_value(typed_value_type& from, const std::vector<bool>& value);
bool assign_value(typed_value_type& from, const std::vector<uint8_t>& value);
bool assign_value(typed_value_type& from, const std::vector<uint16_t>& value);
bool assign_value(typed_value_type& from, const std::vector<uint32_t>& value);
bool assign_value(typed_value_type& from, const std::vector<uint64_t>& value);
bool assign_value(typed_value_type& from, const std::vector<int8_t>& value);
bool assign_value(typed_value_type& from, const std::vector<int16_t>& value);
bool assign_value(typed_value_type& from, const std::vector<int32_t>& value);
bool assign_value(typed_value_type& from, const std::vector<int64_t>& value);
bool assign_value(typed_value_type& from, const std::vector<float>& value);
bool assign_value(typed_value_type& from, const std::vector<double>& value);
bool assign_value(typed_value_type& from, const std::vector<rx_time_struct>& value);
bool assign_value(typed_value_type& from, const std::vector<rx_time>& value);
bool assign_value(typed_value_type& from, const std::vector<rx_uuid_t>& value);
bool assign_value(typed_value_type& from, const std::vector<rx_uuid>& value);
bool assign_value(typed_value_type& from, const std::vector<byte_string>& value);
bool assign_value(typed_value_type& from, const std::vector<rx_node_id>& value);
bool assign_value(typed_value_type& from, const string_array& value);
bool assign_value(typed_value_type& from, const std::vector<std::vector<rx_simple_value> >& value);





class rx_simple_value 
{
  public:
      template<typename typeT>
	  void assign_static(typeT&& val, rx_time ts = rx_time::null_time(), uint32_t quality = RX_GOOD_QUALITY)
	  {
          rx_destroy_value(&data_);
		  assign_value(data_, std::forward<typeT>(val));
	  }
      template<typename typeT>
      typeT extract_static(const typeT& def) const
      {
          return extract_value(data_, def);
      }
      rx_simple_value();
      rx_simple_value(typed_value_type val) noexcept;
	  rx_simple_value(const rx_simple_value &right);
	  rx_simple_value(rx_simple_value&& right) noexcept;
	  rx_simple_value& operator=(rx_simple_value&& right) noexcept;
	  rx_simple_value & operator=(const rx_simple_value &right);

      typed_value_type move() noexcept;
      const typed_value_type* c_ptr() const noexcept;

  public:
      rx_simple_value (const typed_value_type* storage);

      ~rx_simple_value();

      bool operator==(const rx_simple_value &right) const;

      bool operator!=(const rx_simple_value &right) const;


      bool is_bad () const;

      bool is_uncertain () const;

      bool is_test () const;

      bool is_substituted () const;

      bool is_good () const;

      bool can_operate (bool test_mode) const;

      bool serialize (const string_type& name, base_meta_writer& writter) const;

      bool deserialize (const string_type& name, base_meta_reader& reader);

      void dump_to_stream (std::ostream& out) const;

      void get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const;

      rx_value_t get_type () const;

      bool convert_to (rx_value_t type);

      void parse (const string_type& str);

      bool is_array () const;

      size_t array_size () const;

      bool is_null () const;

      bool is_complex () const;

      bool is_numeric () const;

      bool is_integer () const;

      bool is_unassigned () const;

      bool is_float () const;

      bool is_string () const;

      string_type get_string (size_t idx = RX_INVALID_INDEX_VALUE) const;

      bool get_bool (size_t idx = RX_INVALID_INDEX_VALUE) const;

      int64_t get_integer (rx_value_t* type = NULL, size_t idx = RX_INVALID_INDEX_VALUE) const;

      uint64_t get_unassigned (rx_value_t* type = NULL, size_t idx = RX_INVALID_INDEX_VALUE) const;

      double get_float (rx_value_t* type = NULL, size_t idx = RX_INVALID_INDEX_VALUE) const;

      complex_value get_complex (size_t idx = RX_INVALID_INDEX_VALUE) const;

      string_type to_string () const;

      void set_integer (int64_t val, rx_value_t type, size_t idx = RX_INVALID_INDEX_VALUE);

      void set_unassigned (int64_t val, rx_value_t type, size_t idx = RX_INVALID_INDEX_VALUE);

      void set_float (double val, rx_value_t type, size_t idx = RX_INVALID_INDEX_VALUE);

      bool weak_serialize (const char* name, base_meta_writer& writter) const;

      bool weak_deserialize (const char* name, base_meta_reader& reader);

      bool is_byte_string () const;

      byte_string get_byte_string (size_t idx = RX_INVALID_INDEX_VALUE) const;

      rx_simple_value operator [] (int index) const;

      void assign_array (const std::vector<rx_simple_value>& from);

      bool is_struct () const;

      size_t struct_size () const;

      friend class rx_timed_value;
      friend class rx_value;
  protected:

  private:


      typed_value_type data_;


};







class rx_value 
{
public:
    template<typename typeT>
    void assign_static(typeT val, rx_time ts = rx_time::null_time())
    {
        rx_destroy_value(&data_.value);
        assign_value(data_.value, std::forward<typeT>(val));
        data_.time = ts.c_data();
    }
    template<typename typeT>
    typeT extract_static(const typeT& def) const
    {
        return extract_value(data_.value, def);
    }
	rx_value();
    rx_value(full_value_type right) noexcept;
	rx_value(const rx_value &right);
	rx_value(rx_value&& right) noexcept;
	rx_value& operator=(rx_value&& right) noexcept;
	rx_value & operator=(const rx_value &right);

    rx_value(rx_simple_value&& right, rx_time ts = rx_time::null_time(), uint32_t quality = RX_DEFAULT_VALUE_QUALITY) noexcept;
    rx_value(const rx_simple_value& right, rx_time ts = rx_time::null_time(), uint32_t quality = RX_DEFAULT_VALUE_QUALITY);

    rx_value(rx_timed_value&& right, uint32_t quality = RX_DEFAULT_VALUE_QUALITY) noexcept;
    rx_value(const rx_timed_value& right, uint32_t quality = RX_DEFAULT_VALUE_QUALITY);

    bool operator==(const rx_value& right) const;
    bool operator!=(const rx_value& right) const;

    full_value_type move() noexcept;
    const full_value_type* c_ptr() const noexcept;

  public:
      rx_value (const full_value_type* storage);

      rx_value (const typed_value_type* storage, rx_time ts, const rx_mode_type& mode);

      ~rx_value();


      bool is_bad () const;

      bool is_uncertain () const;

      bool is_test () const;

      bool is_substituted () const;

      bool is_good () const;

      bool can_operate (bool test_mode) const;

      bool serialize (const string_type& name, base_meta_writer& stream) const;

      bool deserialize (const string_type& name, base_meta_reader& stream);

      void dump_to_stream (std::ostream& out) const;

      void get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const;

      rx_value_t get_type () const;

      bool convert_to (rx_value_t type);

      void parse (const string_type& str);

      bool is_array () const;

      size_t array_size () const;

      bool is_null () const;

      bool is_complex () const;

      bool is_numeric () const;

      bool is_integer () const;

      bool is_unassigned () const;

      bool is_float () const;

      bool is_string () const;

      string_type get_string (size_t idx = RX_INVALID_INDEX_VALUE) const;

      bool get_bool (size_t idx = RX_INVALID_INDEX_VALUE) const;

      int64_t get_integer (rx_value_t* type = NULL, size_t idx = RX_INVALID_INDEX_VALUE) const;

      uint64_t get_unassigned (rx_value_t* type = NULL, size_t idx = RX_INVALID_INDEX_VALUE) const;

      double get_float (rx_value_t* type = NULL, size_t idx = RX_INVALID_INDEX_VALUE) const;

      complex_value get_complex (size_t idx = RX_INVALID_INDEX_VALUE) const;

      string_type to_string () const;

      void set_integer (int64_t val, rx_value_t type, size_t idx = RX_INVALID_INDEX_VALUE);

      void set_unassigned (int64_t val, rx_value_t type, size_t idx = RX_INVALID_INDEX_VALUE);

      void set_float (double val, rx_value_t type, size_t idx = RX_INVALID_INDEX_VALUE);

      rx_time set_time (rx_time time);

      rx_time get_time () const;

      bool compare (const rx_value& right, time_compare_type time_compare) const;

      rx_simple_value to_simple () const;

      void set_substituted ();

      void set_test ();

      bool is_dead () const;

      bool adapt_quality_to_mode (const rx_mode_type& mode);

      void set_offline ();

      void set_good_locally ();

      uint32_t get_quality () const;

      void set_quality (uint32_t val);

      bool is_byte_string () const;

      byte_string get_byte_string (size_t idx = RX_INVALID_INDEX_VALUE) const;

      void set_origin (uint32_t val);

      uint32_t increment_signal_level ();

      uint32_t get_signal_level () const;

      uint32_t get_origin () const;

      rx_simple_value operator [] (int index) const;

      void assign_array (const std::vector<rx_simple_value>& from, rx_time ts = rx_time::null_time(), uint32_t quality = RX_GOOD_QUALITY);

      bool is_struct () const;

      size_t struct_size () const;


  protected:

  private:


      full_value_type data_;


};







class rx_timed_value 
{
public:

    template<typename typeT>
    void assign_static(typeT val, rx_time ts = rx_time::null_time(), uint32_t quality = RX_GOOD_QUALITY)
    {
        rx_destroy_value(&data_.value);
        assign_value(data_.value, std::forward<typeT>(val));
        data_.time = ts.c_data();
    }
    template<typename typeT>
    typeT extract_static(const typeT& def) const
    {
        return extract_value(&data_.value, def);
    }
	rx_timed_value();
    rx_timed_value(timed_value_type right) noexcept;
	rx_timed_value(const rx_timed_value &right);
	rx_timed_value(rx_timed_value&& right) noexcept;
	rx_timed_value& operator=(rx_timed_value&& right) noexcept;
	rx_timed_value & operator=(const rx_timed_value &right) ;

    rx_timed_value(rx_simple_value&& right, rx_time ts = rx_time::null_time()) noexcept;
    rx_timed_value(const rx_simple_value& right, rx_time ts = rx_time::null_time());

    timed_value_type move() noexcept;
    const timed_value_type* c_ptr() const noexcept;

  public:
      rx_timed_value (const timed_value_type* storage);

      rx_timed_value (const typed_value_type* storage, rx_time ts);

      ~rx_timed_value();

      bool operator==(const rx_timed_value &right) const;

      bool operator!=(const rx_timed_value &right) const;


      bool is_bad () const;

      bool is_uncertain () const;

      bool is_test () const;

      bool is_substituted () const;

      bool is_good () const;

      bool can_operate (bool test_mode) const;

      bool serialize (const string_type& name, base_meta_writer& writter) const;

      bool deserialize (const string_type& name, base_meta_reader& reader);

      void dump_to_stream (std::ostream& out) const;

      void get_value (values::rx_value& val, rx_time ts, const rx_mode_type& mode) const;

      rx_value_t get_type () const;

      bool convert_to (rx_value_t type);

      void parse (const string_type& str);

      bool is_array () const;

      size_t array_size () const;

      bool is_null () const;

      bool is_complex () const;

      bool is_numeric () const;

      bool is_integer () const;

      bool is_unassigned () const;

      bool is_float () const;

      bool is_string () const;

      string_type get_string (size_t idx = RX_INVALID_INDEX_VALUE) const;

      bool get_bool (size_t idx = RX_INVALID_INDEX_VALUE) const;

      int64_t get_integer (rx_value_t* type = NULL, size_t idx = RX_INVALID_INDEX_VALUE) const;

      uint64_t get_unassigned (rx_value_t* type = NULL, size_t idx = RX_INVALID_INDEX_VALUE) const;

      double get_float (rx_value_t* type = NULL, size_t idx = RX_INVALID_INDEX_VALUE) const;

      complex_value get_complex (size_t idx = RX_INVALID_INDEX_VALUE) const;

      string_type to_string () const;

      void set_integer (int64_t val, rx_value_t type, size_t idx = RX_INVALID_INDEX_VALUE);

      void set_unassigned (int64_t val, rx_value_t type, size_t idx = RX_INVALID_INDEX_VALUE);

      void set_float (double val, rx_value_t type, size_t idx = RX_INVALID_INDEX_VALUE);

      rx_time set_time (rx_time time);

      rx_time get_time () const;

      bool compare (const rx_timed_value& right, time_compare_type time_compare) const;

      rx_simple_value to_simple () const;

      bool is_byte_string () const;

      byte_string get_byte_string (size_t idx = RX_INVALID_INDEX_VALUE) const;

      rx_simple_value operator [] (int index) const;

      void assign_array (const std::vector<rx_simple_value>& from, rx_time ts = rx_time::null_time());

      bool is_struct () const;

      size_t struct_size () const;


  protected:

  private:


      timed_value_type data_;


};






template <class valT>
class rx_value_holder 
{

  public:

  protected:

  private:


};


} // namespace values
} // namespace rx

namespace rx
{

template<typename T>
void rx_create_value_static_internal(std::vector<values::rx_simple_value>& vals, T t)
{
    values::rx_simple_value temp;
    temp.assign_static(t);
    vals.push_back(std::move(temp));
}
template<typename T, typename... Args>
void rx_create_value_static_internal(std::vector<values::rx_simple_value>& vals, T t, Args... args)
{
    values::rx_simple_value temp;
    temp.assign_static(t);
    vals.push_back(std::move(temp));
    rx_create_value_static_internal(vals, std::forward<Args>(args)...);
}
template<typename... Args>
values::rx_simple_value rx_create_value_static(Args... args)
{
    std::vector<values::rx_simple_value> vals;
    rx_create_value_static_internal(vals, std::forward<Args>(args)...);
    values::rx_simple_value ret;
    ret.assign_static(vals);
    return ret;
}

void fill_quality_string(values::rx_value val, string_type& q);
namespace values
{

rx_value_t inner_get_type(tl::type2type<bool>);

rx_value_t inner_get_type(tl::type2type<int8_t>);

rx_value_t inner_get_type(tl::type2type<char>);

rx_value_t inner_get_type(tl::type2type<uint8_t>);

rx_value_t inner_get_type(tl::type2type<int16_t>);

rx_value_t inner_get_type(tl::type2type<uint16_t>);

rx_value_t inner_get_type(tl::type2type<int32_t>);

rx_value_t inner_get_type(tl::type2type<uint32_t>);

rx_value_t inner_get_type(tl::type2type<int64_t>);

rx_value_t inner_get_type(tl::type2type<uint64_t>);

rx_value_t inner_get_type(tl::type2type<float>);

rx_value_t inner_get_type(tl::type2type<double>);

rx_value_t inner_get_type(tl::type2type<typename rx::rx_time>);

rx_value_t inner_get_type(tl::type2type<typename rx::rx_node_id>);

rx_value_t inner_get_type(tl::type2type<typename rx::rx_uuid>);

rx_value_t inner_get_type(tl::type2type<const char*>);

rx_value_t inner_get_type(tl::type2type<char*>);

template<size_t l>
rx_value_t inner_get_type(tl::type2type<char[l]>)
{
    return RX_STRING_TYPE;
}


template< class T >
struct cpp17_remove_cvref {
    typedef typename std::remove_cv_t<std::remove_reference_t<T>> type;
};

template<typename typeT>
rx_value_t get_type()
{
    return inner_get_type(tl::type2type<typename cpp17_remove_cvref<typeT>::type >());
}



}
}


#endif
