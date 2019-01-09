

/****************************************************************************
*
*  system\serialization\rx_ser.h
*
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_ser_h
#define rx_ser_h 1


#include "third-party/jsoncpp/json/json.h"

// rx_ser_lib
#include "lib/rx_ser_lib.h"
// rx_mem
#include "lib/rx_mem.h"

using rx::memory::byte_order_type;


namespace rx_platform {

namespace serialization {





class json_reader : public rx::base_meta_reader  
{
	struct json_read_stack_data
	{
	public:
		json_read_stack_data(Json::Value& vval)
			: value(vval)
			, index(0)
		{
		}
		Json::Value& value;
		int index;
	};
	typedef std::stack<json_read_stack_data, std::vector<json_read_stack_data> > stack_type;

  public:
      json_reader();

      virtual ~json_reader();


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


  protected:

  private:

      Json::Value& get_current_value (int& index);

      bool safe_read_int (int idx, const string_type& name, int val, const Json::Value& object);

      bool safe_read_string (int idx, const string_type& name, string_type& val, const Json::Value& object);

      bool parse_version_string (uint32_t& result, const string_type& version);



      stack_type stack_;

      Json::Value envelope_;

      string_type result_;


};






class json_writer : public rx::base_meta_writer  
{
	struct json_write_stack_data
	{
	public:
		Json::Value value;
		bool is_array;
		std::string name;
	};
	typedef std::stack<json_write_stack_data> stack_type;

  public:
      json_writer (int version = RX_CURRENT_SERIALIZE_VERSION);

      virtual ~json_writer();


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

      bool get_string (string_type& result, bool decorated);

      bool write_version (const char* name, uint32_t val);


  protected:

  private:

      Json::Value& get_current_value (bool& is_array);

      bool get_version_string (string_type& result, uint32_t version);



      stack_type stack_;

      Json::Value envelope_;

      string_type result_;

      int type_;


};







template <typename allocT, bool swap_bytes>
class binary_reader : public rx::base_meta_reader  
{
	typedef rx::memory::memory_buffer_base<allocT, false> buffer_type;
	struct binary_read_stack_data
	{
	public:
		int array_size;
		int object_size;
	};
	typedef std::stack<binary_read_stack_data, std::vector<binary_read_stack_data> > stack_type;

  public:
      binary_reader (buffer_type& buffer);

      virtual ~binary_reader();


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

      bool read_version (const char* name, uint32_t& val);

      void dump_to_stream (std::ostream& out);


  protected:

  private:


      stack_type stack_;

      buffer_type& buffer_;

      int type_;


};


typedef binary_reader<memory::std_vector_allocator, false> std_buffer_reader;






template <typename allocT, bool swap_bytes>
class binary_writer : public rx::base_meta_writer  
{
	typedef rx::memory::memory_buffer_base<allocT, false> buffer_type;
	struct json_write_stack_data
	{
	public:
		Json::Value value;
		bool is_array;
		std::string name;
	};
	typedef std::stack<json_write_stack_data> stack_type;

  public:
      binary_writer (buffer_type& buffer, int version = RX_CURRENT_SERIALIZE_VERSION);

      virtual ~binary_writer();


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

      bool write_version (const char* name, uint32_t val);

      void dump_to_stream (std::ostream& out);

		bool is_string()
		{
			return false;
		}
		template<typename T>
		void read_element(const string_type& name,T& val)
		{
			(*this)>>val;
		}
		template <typename T>
		struct boxing_struct
		{
			T value;
		};
		binary_writer& operator >> (rx_uuid& val)
		{
			buffer_.read_data(&val,sizeof(val));
			return *this;
		}

  protected:

  private:


      stack_type stack_;

      int type_;

      buffer_type& buffer_;


};

typedef binary_writer<memory::std_vector_allocator, false> std_buffer_writer;

// Parameterized Class rx_platform::serialization::binary_reader 

template <typename allocT, bool swap_bytes>
binary_reader<allocT,swap_bytes>::binary_reader (buffer_type& buffer)
      : buffer_(buffer)
{
}


template <typename allocT, bool swap_bytes>
binary_reader<allocT,swap_bytes>::~binary_reader()
{
}



template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_id (const char* name, rx_node_id& id)
{
	uint8_t encoding;
	buffer_.read_data(encoding);
	switch (encoding & 0xf)
	{
	case 0:
	{// two uint8_ts
		uint8_t val;
		buffer_.read_data(val);
		buffer_.read_data(val);
		id = rx_node_id(val);
	}
	break;
	case 1:
	{// four uint8_ts
		uint8_t namesp;
		uint16_t val;
		buffer_.read_data(namesp);
		buffer_.read_data(val);
		id = rx_node_id(val, namesp);
	}
	break;
	case 2:
	{// full numeric uint8_ts
		uint16_t namesp;
		uint32_t val;
		buffer_.read_data(namesp);
		buffer_.read_data(val);
		id = rx_node_id(val, namesp);
	}
	break;
	case 3:
	{// string value
		uint16_t namesp;
		string_type val;
		buffer_.read_data(namesp);
		buffer_.read_data(val);
		id = rx_node_id(val.c_str(), namesp);
	}
	break;
	case 4:
	{// uuid value
		uint16_t namesp;
		rx_uuid_t val;
		buffer_.read_data(namesp);
		buffer_.read_data(val);
		id = rx_node_id(val, namesp);
	}
	break;
	case 5:
	{// binary value
		uint16_t namesp;
		byte_string val;
		buffer_.read_data(namesp);
		buffer_.read_data(val);
		id = rx_node_id(val, namesp);
		return false;// not implemented yet
					 //id=base::node_id(val,namesp);
	}
	break;
	default:
		return false;// unknown encoding value
	}
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_string (const char* name, string_type& str)
{
	buffer_.read_data(str);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_bool (const char* name, bool& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_double (const char* name, double& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_time (const char* name, rx_time_struct_t& val)
{
	buffer_.read_data(val.t_value);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_uuid (const char* name, rx_uuid_t& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_int (const char* name, int& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_uint (const char* name, uint32_t& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::start_array (const char* name)
{
	uint32_t size;
	buffer_.read_data(size);
	stack_.emplace(binary_read_stack_data{ (int)size,-1 });
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::array_end ()
{
	if (stack_.empty())
		return true;// no arrays here!!!
	binary_read_stack_data& last = stack_.top();
	if (last.array_size == 0)
	{// array end
	 // remove last array
		stack_.pop();
		return true;
	}
	else
	{
		last.array_size = last.array_size - 1;
		return false;
	}
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_header (int& type)
{
	bool ret = false;
	uint32_t version = 0;
	if (read_version("sversion", version))
	{
		set_version(version);
		buffer_.read_data(type_);
		switch (type_)
		{
		case STREAMING_TYPE_CLASS:
            ret=true;
            break;
		case STREAMING_TYPE_CLASSES:
            ret=true;
            break;
		case STREAMING_TYPE_OBJECT:
			ret = true;
			break;
		case STREAMING_TYPE_MESSAGE:
			ret = true;
			break;
		case STREAMING_TYPE_CHECKOUT:
			ret = true;
			break;
		case STREAMING_TYPE_OBJECTS:
			ret = true;
			break;
		case STREAMING_TYPE_DETAILS:
			ret = true;
			break;
        case STREAMING_TYPE_DIRECTORY:
            ret=true;
            break;
		case STREAMING_TYPE_VALUES:
			ret = true;
			break;
		}
	}
	if (ret)
	{
		type = type_;
		if (is_serialization_type_array(type_))
		{
			uint32_t size;
			buffer_.read_data(size);
			stack_.emplace(binary_read_stack_data { (int)size,-1 });
		}
	}
	return ret;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_footer ()
{
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::start_object (const char* name)
{
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::end_object ()
{
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_byte (const char* name, uint8_t& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_value (const char* name, rx_value& val)
{
	return false;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_int64 (const char* name, int64_t& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_uint64 (const string_type& name, uint64_t& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_bytes (const char* name, byte_string& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_version (const char* name, uint32_t& val)
{
	buffer_.read_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
void binary_reader<allocT,swap_bytes>::dump_to_stream (std::ostream& out)
{
	buffer_.dump_to_stream(out);
}


// Parameterized Class rx_platform::serialization::binary_writer 

template <typename allocT, bool swap_bytes>
binary_writer<allocT,swap_bytes>::binary_writer (buffer_type& buffer, int version)
      : buffer_(buffer)
	, base_meta_writer(version)
{
}


template <typename allocT, bool swap_bytes>
binary_writer<allocT,swap_bytes>::~binary_writer()
{
}



template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_id (const char* name, const rx_node_id& id)
{
	switch (id.get_node_type())
	{
	case numeric_rx_node_id:
	{
		uint16_t namesp = id.get_namespace();
		uint32_t val = 0;
		id.get_numeric(val);
		if (namesp == 0 && (val<0x100))
		{// two uint8_ts encoding 0
			uint8_t vals[]{ 0,(uint8_t)val };
			buffer_.push_data(vals[0]);
			buffer_.push_data(vals[1]);
		}
		else if (namesp<0x100 && val<0x10000)
		{// four uint8_ts encoding 0
			uint8_t vals[]{ 0,(uint8_t)namesp };
			uint16_t val16 = (uint16_t)val;
			buffer_.push_data(vals[0]);
			buffer_.push_data(vals[1]);
			buffer_.push_data(val16);
		}
		else
		{// regular id
			uint8_t type = 2;
			buffer_.push_data(type);
			buffer_.push_data(namesp);
			buffer_.push_data(val);
		}
	}
	break;
	case string_rx_node_id:
	{
		uint16_t namesp = id.get_namespace();
		string_type val;
		id.get_string(val);

		buffer_.push_data(((uint8_t)3));
		buffer_.push_data(namesp);
		buffer_.push_data(val);
	}
	break;
	case guid_rx_node_id:
	{
		uint16_t namesp = id.get_namespace();
		rx_uuid_t val;
		id.get_uuid(val);

		buffer_.push_data((uint8_t)4);
		buffer_.push_data(namesp);
		buffer_.push_data(val);
	}
	break;
	case bytes_rx_node_id:
	{
		return false;// not implemented yet!!!
	}
	break;
	}
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_string (const char* name, const char* str)
{
	buffer_.push_data(string_type(str));
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_bool (const char* name, bool val)
{
	buffer_.push_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_double (const char* name, double val)
{
	buffer_.push_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_time (const char* name, const rx_time_struct_t& val)
{
	buffer_.push_data(val.t_value);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_uuid (const char* name, const rx_uuid_t& val)
{
	buffer_.push_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_int (const char* name, int val)
{
	buffer_.push_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_uint (const char* name, uint32_t val)
{
	buffer_.push_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::start_array (const char* name, size_t size)
{
	return write_uint(name, (uint32_t)size);
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::end_array ()
{
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_header (int type, size_t size)
{
	buffer_.push_data((uint32_t)get_version());
	buffer_.push_data(type);
	type_ = type;
	if (rx::is_serialization_type_array(type))
	{
		buffer_.push_data((uint32_t)size);
	}
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_footer ()
{
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::start_object (const char* name)
{
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::end_object ()
{
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_byte (const char* name, uint8_t val)
{
	buffer_.push_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_value (const char* name, const rx_value& val)
{
	return false;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_int64 (const char* name, int64_t val)
{
	buffer_.push_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_uint64 (const char* name, uint64_t val)
{
	buffer_.push_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_bytes (const char* name, const uint8_t* val, size_t size)
{
	buffer_.push_data(val,size);
	return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_version (const char* name, uint32_t val)
{
	buffer_.push_data(val);
	return true;
}

template <typename allocT, bool swap_bytes>
void binary_writer<allocT,swap_bytes>::dump_to_stream (std::ostream& out)
{
	buffer_.dump_to_stream(out);
}


} // namespace serialization
} // namespace rx_platform



#endif
