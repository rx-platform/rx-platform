

/****************************************************************************
*
*  lib\rx_ser_lib.h
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


#ifndef rx_ser_lib_h
#define rx_ser_lib_h 1


#include "lib/rx_lib.h"

// rx_values
#include "lib/rx_values.h"

using namespace rx::values;



// streaming version

#define RX_FIRST_SERIZALIZE_VERSION 0x00009

#define RX_CURRENT_SERIALIZE_VERSION RX_FIRST_SERIZALIZE_VERSION

//////////////////////////////////////////////////
// streaming constants from header
#define STREAMING_TYPE_CLASS		0x01
#define STREAMING_TYPE_CLASSES		0x02
#define STREAMING_TYPE_OBJECT		0x03
#define STREAMING_TYPE_OBJECTS		0x04
#define STREAMING_TYPE_CHECKOUT		0x05
#define STREAMING_TYPE_DETAILS		0x06
#define STREAMING_TYPE_MESSAGE		0x07
#define STREAMING_TYPE_DIRECTORY	0x08
#define STREAMING_TYPE_VALUES		0x09



namespace rx {
bool is_serialization_type_array(int type);






class base_meta_writer 
{
public:
	template<typename T>
	bool check_out(base_meta_reader& stream, T what)
	{
		return false;
	}

  public:
      base_meta_writer (int version);

      virtual ~base_meta_writer();


      virtual bool write_id (const char* name, const rx_node_id& id) = 0;

      virtual bool write_string (const char* name, const char* str) = 0;

      virtual bool write_bool (const char* name, bool val) = 0;

      virtual bool write_double (const char* name, double val) = 0;

      virtual bool write_time (const char* name, const rx_time_struct_t& val) = 0;

      virtual bool write_uuid (const char* name, const rx_uuid_t& val) = 0;

      virtual bool write_int (const char* name, int val) = 0;

      virtual bool write_uint (const char* name, uint32_t val) = 0;

      virtual bool start_array (const char* name, size_t size) = 0;

      virtual bool end_array () = 0;

      virtual bool write_header (int type, size_t size) = 0;

      virtual bool write_footer () = 0;

      virtual bool start_object (const char* name) = 0;

      virtual bool end_object () = 0;

      virtual bool write_byte (const char* name, uint8_t val) = 0;

      virtual bool write_value (const char* name, const rx_value& val) = 0;

      virtual bool write_int64 (const char* name, int64_t val) = 0;

      virtual bool write_uint64 (const char* name, uint64_t val) = 0;

      virtual bool write_bytes (const char* name, const uint8_t* val, size_t size) = 0;

      virtual bool write_version (const char* name, uint32_t val) = 0;


      const size_t get_version () const
      {
        return version_;
      }

      void set_version (size_t value)
      {
        version_ = value;
      }



  protected:

  private:
      base_meta_writer(const base_meta_writer &right);

      base_meta_writer & operator=(const base_meta_writer &right);



      size_t version_;


};







class base_meta_reader 
{

  public:
      base_meta_reader();

      virtual ~base_meta_reader();


      virtual bool read_id (const char* name, rx_node_id& id) = 0;

      virtual bool read_string (const char* name, string_type& str) = 0;

      virtual bool read_bool (const char* name, bool& val) = 0;

      virtual bool read_double (const char* name, double& val) = 0;

      virtual bool read_time (const char* name, rx_time_struct_t& val) = 0;

      virtual bool read_uuid (const char* name, rx_uuid_t& val) = 0;

      virtual bool read_int (const char* name, int& val) = 0;

      virtual bool read_uint (const char* name, uint32_t& val) = 0;

      virtual bool start_array (const char* name) = 0;

      virtual bool array_end () = 0;

      virtual bool read_header (int& type) = 0;

      virtual bool read_footer () = 0;

      virtual bool start_object (const char* name) = 0;

      virtual bool end_object () = 0;

      virtual bool read_byte (const char* name, uint8_t& val) = 0;

      virtual bool read_value (const char* name, rx_value& val) = 0;

      virtual bool read_int64 (const char* name, int64_t& val) = 0;

      virtual bool read_uint64 (const string_type& name, uint64_t& val) = 0;

      virtual bool read_bytes (const char* name, byte_string& val) = 0;

      virtual bool read_version (const char* name, uint32_t& val) = 0;


      const uint32_t get_version () const
      {
        return version_;
      }

      void set_version (uint32_t value)
      {
        version_ = value;
      }


	  template<class T>
	  bool read_object(T& who,uint8_t type)
	  {
		  return who->deserialize_definition(*this, type);
	  }
  protected:

  private:
      base_meta_reader(const base_meta_reader &right);

      base_meta_reader & operator=(const base_meta_reader &right);



      uint32_t version_;


};


} // namespace rx



#endif
