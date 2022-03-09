

/****************************************************************************
*
*  lib\rx_ser_json.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_ser_json_h
#define rx_ser_json_h 1



// rx_ser_lib
#include "lib/rx_ser_lib.h"

#define RX_JSON_SERIALIZATION_TYPE "json"


namespace rx {

namespace serialization {
struct json_reader_data;
struct json_writer_data;
struct json_pretty_writer_data;





class json_reader : public base_meta_reader  
{
    friend struct json_reader_data;

  public:
      json_reader (int version = RX_CURRENT_SERIALIZE_VERSION);

      ~json_reader();


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

      bool read_value (const char* name, rx_value& val);

      bool read_int64 (const char* name, int64_t& val);

      bool read_uint64 (const string_type& name, uint64_t& val);

      bool read_bytes (const char* name, byte_string& val);

      bool parse_data (const string_type& data);

      bool read_version (const char* name, uint32_t& val);

      bool read_init_values (const char* name, data::runtime_values_data& values);

      string_array get_errors () const;

      bool is_string_based () const;

      bool read_item_reference (const char* name, rx_item_reference& ref);

      bool read_value_type (const char* name, rx_value_t& val);

      string_type get_error () const;


  protected:

  private:

      bool parse_version_string (uint32_t& result, const string_type& version);



      string_type result_;

      string_array errors_;

      std::unique_ptr<json_reader_data> data_;


};






template <class writerT>
class json_writer_type : public base_meta_writer  
{
    friend struct json_writer_data;
    friend struct json_pretty_writer_data;

  public:
      json_writer_type (int version = RX_CURRENT_SERIALIZE_VERSION);

      ~json_writer_type();


      bool write_id (const char* name, const rx_node_id& id);

      bool write_string (const char* name, const char* str);

      bool write_bool (const char* name, bool val);

      bool write_double (const char* name, double val);

      bool write_time (const char* name, const rx_time_struct_t& val);

      bool write_uuid (const char* name, const rx_uuid_t& val);

      bool write_int (const char* name, int val);

      bool write_uint (const char* name, uint32_t val);

      bool start_array (const char* name, size_t size);

      bool end_array ();

      bool write_header (int type, size_t size);

      bool write_footer ();

      bool start_object (const char* name);

      bool end_object ();

      bool write_byte (const char* name, uint8_t val);

      bool write_value (const char* name, const rx_value& val);

      bool write_int64 (const char* name, int64_t val);

      bool write_uint64 (const char* name, uint64_t val);

      bool write_bytes (const char* name, const uint8_t* val, size_t size);

      string_type get_string ();

      bool write_version (const char* name, uint32_t val);

      bool write_init_values (const char* name, const data::runtime_values_data& values);

      bool is_string_based () const;

      bool write_item_reference (const char* name, const rx_item_reference& ref);

      bool write_value_type (const char* name, rx_value_t val);

      string_type get_error () const;


  protected:

  private:

      bool is_current_array ();

      bool get_version_string (string_type& result, uint32_t version);

      bool write_null (const char* name);



      string_type result_;

      int type_;

      std::unique_ptr<writerT> data_;


};

#ifdef _DEBUG
typedef json_writer_type<json_pretty_writer_data> json_writer;
#else
typedef json_writer_type<json_writer_data> json_writer;
#endif// _DEBUG

typedef json_writer_type<json_pretty_writer_data> pretty_json_writer;

} // namespace serialization
} // namespace rx



#endif
