

/****************************************************************************
*
*  system\json\rx_ser.h
*
*  Copyright (c) 2017 Dusan Ciric
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


#ifndef rx_ser_h
#define rx_ser_h 1


#include "third-party/jsoncpp/json/json.h"

// rx_ser_lib
#include "lib/rx_ser_lib.h"



namespace server {

namespace serialization {
using namespace rx::values;






class json_reader : public rx::base_meta_reader  
{
	struct json_read_stack_data
	{
	public:
		json_read_stack_data(Json::Value& vval)
			: value(vval)
		{
		}
		Json::Value& value;
		int index;
	};

  public:
      json_reader();

      virtual ~json_reader();


      bool read_id (const char* name, rx_node_id& id);

      bool read_string (const char* name, string_type str);

      bool read_bool (const char* name, bool& val);

      bool read_double (const char* name, double& val);

      bool read_time (const char* name, rx_time_struct_t val);

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

      bool read_uint64 (const string_type& name, uint64_t val);

      bool read_bytes (const char* name, byte_string& val);

      bool parse_data (const string_type& data);

      bool read_version (const char* name, uint32_t& val);


  protected:

  private:

      Json::Value& get_current_value (int& index);

      bool safe_read_int (int idx, const string_type& name, int val, const Json::Value& object);

      bool safe_read_string (int idx, const string_type& name, string_type& val, const Json::Value& object);

      bool parse_version_string (uint32_t& result, const string_type& version);



      std::stack<json_read_stack_data> _stack;

      Json::Value _envelope;

      string_type _result;


};






class json_writter : public rx::base_meta_writter  
{
	struct json_write_stack_data
	{
	public:
		Json::Value value;
		bool is_array;
		std::string name;
	};

  public:
      json_writter (int version = RX_CURRENT_SERIALIZE_VERSION);

      virtual ~json_writter();


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

      bool write_header (int type);

      bool write_footer ();

      bool start_object (const char* name);

      bool end_object ();

      bool write_byte (const char* name, uint8_t val);

      bool write_value (const char* name, const rx_value& val);

      bool write_int64 (const char* name, int64_t val);

      bool write_uint64 (const char* name, uint64_t val);

      bool write_bytes (const char* name, const uint8_t* val, size_t size);

      bool get_string (string_type& result, bool decorated);

      bool write_version (const char* name, uint32_t val);


  protected:

  private:

      Json::Value& get_current_value (bool& is_array);

      bool get_version_string (string_type& result, uint32_t version);



      std::stack<json_write_stack_data> _stack;

      Json::Value _envelope;

      string_type _result;

      int _type;


};


} // namespace serialization
} // namespace server



#endif
