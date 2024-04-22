

/****************************************************************************
*
*  lib\rx_ser_bin.h
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


#ifndef rx_ser_bin_h
#define rx_ser_bin_h 1



// rx_ser_lib
#include "lib/rx_ser_lib.h"
// rx_mem
#include "lib/rx_mem.h"

#define RX_BINARY_SERIALIZATION_TYPE "rx-bin"
using rx::memory::byte_order_type;


namespace rx {

namespace serialization {






template <typename allocT, bool swap_bytes>
class binary_reader : public base_meta_reader  
{
    typedef rx::memory::memory_buffer_base<allocT, false> buffer_type;
    struct binary_read_stack_data
    {
    public:
        int array_size;
        int object_size;
        string_type name;
    };
    typedef std::vector<binary_read_stack_data> stack_type;

  public:
      binary_reader (buffer_type& buffer, int version = RX_CURRENT_SERIALIZE_VERSION);

      ~binary_reader();


      bool read_id (const char* name, rx_node_id& id);

      bool read_string (const char* name, string_type& str);

      bool read_bool (const char* name, bool& val);

      bool read_double (const char* name, double& val);

      bool read_time (const char* name, rx_time_struct_t& val);

      bool read_uuid (const char* name, rx_uuid_t& val);

      bool read_int (const char* name, int& val);

      bool read_uint (const char* name, uint32_t& val);

      bool start_array (const char* name);

      bool array_end ();

      bool read_header (int& type);

      bool read_footer ();

      bool start_object (const char* name);

      bool end_object ();

      bool read_byte (const char* name, uint8_t& val);

      bool read_sbyte (const char* name, int8_t& val);

      bool read_value (const char* name, rx_value& val);

      bool read_int64 (const char* name, int64_t& val);

      bool read_uint64 (const string_type& name, uint64_t& val);

      bool read_bytes (const char* name, byte_string& val);

      bool read_version (const char* name, uint32_t& val);

      void dump_to_stream (std::ostream& out);

      bool read_init_values (const char* name, data::runtime_values_data& values);

      bool read_data_type (const char* name, data::runtime_data_model& values);

      bool is_string_based () const;

      bool read_item_reference (const char* name, rx_item_reference& ref);

      bool read_value_type (const char* name, rx_value_t& val);

      string_type get_error () const;


  protected:

  private:


      stack_type stack_;

      buffer_type& buffer_;

      int type_;

      bool swap_bytes_;


};

typedef binary_reader<memory::std_vector_allocator, false> std_buffer_reader;






template <typename allocT, bool swap_bytes>
class binary_writer : public base_meta_writer  
{
    typedef rx::memory::memory_buffer_base<allocT, false> buffer_type;
    struct json_write_stack_data
    {
    public:
        bool is_array;
        std::string name;
    };
    typedef std::vector<json_write_stack_data> stack_type;

  public:
      binary_writer (buffer_type& buffer, int version = RX_CURRENT_SERIALIZE_VERSION);

      ~binary_writer();


      bool write_header (int type, size_t size);

      bool write_footer ();

      bool start_object (const char* name);

      bool end_object ();

      bool start_array (const char* name, size_t size);

      bool end_array ();

      bool write_id (const char* name, const rx_node_id& id);

      bool write_string (const char* name, const char* str);

      bool write_bool (const char* name, bool val);

      bool write_double (const char* name, double val);

      bool write_time (const char* name, const rx_time_struct_t& val);

      bool write_uuid (const char* name, const rx_uuid_t& val);

      bool write_int (const char* name, int val);

      bool write_uint (const char* name, uint32_t val);

      bool write_byte (const char* name, uint8_t val);

      bool write_sbyte (const char* name, int8_t val);

      bool write_value (const char* name, const rx_value& val);

      bool write_int64 (const char* name, int64_t val);

      bool write_uint64 (const char* name, uint64_t val);

      bool write_bytes (const char* name, const std::byte* val, size_t size);

      bool write_version (const char* name, uint32_t val);

      void dump_to_stream (std::ostream& out);

      bool write_init_values (const char* name, const data::runtime_values_data& values);

      bool write_data_type (const char* name, const data::runtime_data_model& data);

      bool is_string_based () const;

      bool write_item_reference (const char* name, const rx_item_reference& ref);

      bool write_value_type (const char* name, rx_value_t val);

      string_type get_error () const;

      byte_string get_data ();

      bool is_string()
      {
          return false;
      }
      template<typename T>
      void read_element(const string_type& name, T& val)
      {
          (*this) >> val;
      }
      template <typename T>
      struct boxing_struct
      {
          T value;
      };
      /*binary_writer& operator >> (rx_uuid& val)
      {
          if(!buffer_.read_data(&val, sizeof(val)))
              return false;
          return *this;
      }*/

  protected:

  private:


      stack_type stack_;

      int type_;

      buffer_type& buffer_;


};

typedef binary_writer<memory::std_vector_allocator, false> std_buffer_writer;

} // namespace serialization
} // namespace rx



#endif

