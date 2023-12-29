

/****************************************************************************
*
*  lib\rx_asn.h
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


#ifndef rx_asn_h
#define rx_asn_h 1



// rx_io_buffers
#include "lib/rx_io_buffers.h"


#define STATIC_ASN_BUFFER_SIZE 16

#define OID_STATIC_SIZE 0x10
#define OID_DYNAMIC_SIZE 0x40


namespace rx {

namespace asn {
struct null_asn_type
{
};





class oid_t 
{

  public:
      oid_t();

      oid_t(const oid_t &right);

      oid_t (const uint32_t* vals, size_t size);

      oid_t (const std::vector<uint32_t>& vals);

      oid_t (const char* vals);

      oid_t (const string_type& str);

      ~oid_t();

      oid_t & operator=(const oid_t &right);

      bool operator==(const oid_t &right) const;

      bool operator!=(const oid_t &right) const;

      bool operator<(const oid_t &right) const;

      bool operator>(const oid_t &right) const;

      bool operator<=(const oid_t &right) const;

      bool operator>=(const oid_t &right) const;


      size_t size () const;

      void push_back (uint32_t val);

      bool valid () const;

      bool empty () const;

      void to_string (string_type& str) const;

      const uint32_t& operator [] (const size_t& index) const;

      uint32_t& operator [] (const size_t& index);

      void clear ();


  protected:

  private:

      void init_from_string (const string_type& str);



      uint32_t static_storage_[OID_STATIC_SIZE];

      uint32_t* heap_storage_;

      size_t size_;

      uint32_t* data_;


};


struct asn_in_stack_data
{
    uint32_t type;
    uint32_t end_possition;
};





class rx_asn_in_stream 
{
    typedef std::stack<asn_in_stack_data, std::vector<asn_in_stack_data> > asn_in_stack_type;

    rx_asn_in_stream(const rx_asn_in_stream&) = delete;
    rx_asn_in_stream(rx_asn_in_stream&&) = delete;

  public:
      rx_asn_in_stream (io::rx_const_io_buffer* buffer);

      ~rx_asn_in_stream();


      bool read_next_tag (uint32_t& type);

      uint32_t get_length ();

      void skip_tag ();

      rx_asn_in_stream& operator >> (uint8_t& val);

      rx_asn_in_stream& operator >> (uint16_t& val);

      rx_asn_in_stream& operator >> (int16_t& val);

      bool eof () const;

      rx_asn_in_stream& operator >> (uint32_t& val);

      rx_asn_in_stream& operator >> (int32_t& val);

      uint32_t get_current_object_end_pos ();

      rx_asn_in_stream& operator >> (oid_t& val);

      void read_explicit (uint8_t& val);

      void read_explicit (uint16_t& val);

      void read_explicit (int16_t& val);

      void read_explicit (uint32_t& val);

      void read_explicit (int32_t& val);

      void read_explicit (oid_t& val);

      rx_asn_in_stream& operator >> (bit_string& val);

      void read_explicit (bit_string& val);

      rx_asn_in_stream& operator >> (byte_string& val);

      void read_explicit (string_type& val);

      rx_asn_in_stream& operator >> (string_type& val);

      rx_asn_in_stream& operator >> (bool& val);

      void read_explicit (bool& val);

      rx_asn_in_stream& operator >> (float& val);

      rx_asn_in_stream& operator >> (double& val);

      rx_asn_in_stream& operator >> (rx_time_stamp& val);

      rx_asn_in_stream& operator >> (null_asn_type& val);

      rx_asn_in_stream& operator >> (asn_binary_time& val);

      rx_asn_in_stream& operator >> (asn_generalized_time& val);

      rx_asn_in_stream& operator >> (uint64_t& val);

      rx_asn_in_stream& operator >> (int64_t& val);

      void read_explicit (uint64_t& val);

      void read_explicit (int64_t& val);


  protected:

  private:


      io::rx_const_io_buffer *buffer_;


      uint32_t current_length_;

      asn_in_stack_type stack_;

      bool complex_;


};


struct asn_out_stack_data
{
    uint32_t type;
    uint32_t current;
    uint32_t start_size;
};






class rx_asn_out_stream 
{
    typedef std::stack<asn_out_stack_data> data_stack_type;

  public:
      rx_asn_out_stream (io::rx_io_buffer* buffer, bool fixed = false);

      ~rx_asn_out_stream();


      void start_object (uint32_t type, uint32_t first);

      void end_object ();

      void start_object (uint32_t first);

      void skip ();

      void write_explicit (const uint8_t& val, uint32_t type);

      void write_explicit (const uint16_t& val, uint32_t type);

      void write_explicit (const int16_t& val, uint32_t type);

      void write_explicit (const uint32_t& val, uint32_t type);

      void write_explicit (const int32_t& val, uint32_t type);

      void write_explicit (const oid_t& val, uint32_t type);

      void write_explicit (const bit_string& val, uint32_t type);

      void write_explicit (const byte_string& val, uint32_t type);

      void write_explicit (const null_asn_type val, uint32_t type);

      void write_explicit (const string_type& val, uint32_t type);

      void write_explicit (float val, uint32_t type);

      void write_explicit (double val, uint32_t type);

      void write_explicit (const rx_time_stamp& val, uint32_t type);

      void write_explicit (bool val, uint32_t type);

      void write_explicit (const asn_binary_time& val, uint32_t type);

      void write_explicit (const asn_generalized_time& val, uint32_t type);

      void write_explicit (const int64_t& val, uint32_t type);

      void write_explicit (const uint64_t& val, uint32_t type);

      void write_explicit (const int8_t& val, uint32_t type);


      io::rx_io_buffer * get_buffer ()
      {
        return buffer_;
      }



      const bool is_fixed () const
      {
        return fixed_;
      }


      template<typename T>
      rx_asn_out_stream& operator << (const T& val)
      {
          write_explicit(val, create_stream_simple_type());
          return *this;
      }
  protected:

  private:

      uint32_t create_stream_struct_type (uint32_t type);

      uint32_t create_stream_simple_type (uint32_t type = 0);

      size_t write_length (uint32_t len, uint8_t* data);

      size_t write_type (uint32_t type, uint8_t* data);



      io::rx_io_buffer *buffer_;


      data_stack_type stack_;

      uint8_t data_[0x20];

      bool fixed_;


};


} // namespace asn
} // namespace rx



#endif
