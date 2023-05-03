

/****************************************************************************
*
*  lib\rx_ser_bin.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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

      bool read_value (const char* name, rx_value& val);

      bool read_int64 (const char* name, int64_t& val);

      bool read_uint64 (const string_type& name, uint64_t& val);

      bool read_bytes (const char* name, byte_string& val);

      bool read_version (const char* name, uint32_t& val);

      void dump_to_stream (std::ostream& out);

      bool read_init_values (const char* name, data::runtime_values_data& values);

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

      bool write_value (const char* name, const rx_value& val);

      bool write_int64 (const char* name, int64_t val);

      bool write_uint64 (const char* name, uint64_t val);

      bool write_bytes (const char* name, const std::byte* val, size_t size);

      bool write_version (const char* name, uint32_t val);

      void dump_to_stream (std::ostream& out);

      bool write_init_values (const char* name, const data::runtime_values_data& values);

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

// Parameterized Class rx::serialization::binary_reader 

template <typename allocT, bool swap_bytes>
binary_reader<allocT,swap_bytes>::binary_reader (buffer_type& buffer, int version)
      : buffer_(buffer),
        swap_bytes_(false)
    , base_meta_reader(version)
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
    if (!buffer_.read_data(encoding))
        return false;
    switch (encoding & 0xf)
    {
    case 0:
        {// two uint8_ts
            uint8_t val;
            if(!buffer_.read_data(val))
                return false;
            if (val == 0u)
                id = rx_node_id();
            else
                id = rx_node_id(val);
        }
        break;
    case 1:
        {// four uint8_ts
            uint8_t namesp;
            uint16_t val;
            if(!buffer_.read_data(namesp))
                return false;
            if(!buffer_.read_data(val))
                return false;
            id = rx_node_id(val, namesp);
        }
        break;
    case 2:
        {// full numeric uint8_ts
            uint16_t namesp;
            uint32_t val;
            if(!buffer_.read_data(namesp))
                return false;
            if(!buffer_.read_data(val))
                return false;
            id = rx_node_id(val, namesp);
        }
        break;
    case 3:
        {// string value
            uint16_t namesp;
            string_type val;
            if(!buffer_.read_data(namesp))
                return false;
            if(!buffer_.read_data(val))
                return false;
            id = rx_node_id(val.c_str(), namesp);
        }
        break;
    case 4:
        {// uuid value
            uint16_t namesp;
            rx_uuid_t val;
            if(!buffer_.read_data(namesp))
                return false;
            if(!buffer_.read_data(val))
                return false;
            id = rx_node_id(val, namesp);
        }
        break;
    case 5:
        {// binary value
            uint16_t namesp;
            byte_string val;
            if(!buffer_.read_data(namesp))
                return false;
            if(!buffer_.read_data(val))
                return false;
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
    if(!buffer_.read_data(str))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_bool (const char* name, bool& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_double (const char* name, double& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_time (const char* name, rx_time_struct_t& val)
{
    if(!buffer_.read_data(val.t_value))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_uuid (const char* name, rx_uuid_t& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_int (const char* name, int& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_uint (const char* name, uint32_t& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::start_array (const char* name)
{
    uint32_t size;
    if(!buffer_.read_data(size))
        return false;
    stack_.emplace_back(binary_read_stack_data{ (int)size,-1, name });
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::array_end ()
{
    if (stack_.empty())
        return true;// no arrays here!!!
    binary_read_stack_data& last = *stack_.rbegin();
    if (last.array_size == 0)
    {// array end
     // remove last array
        stack_.pop_back();
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
        if(!buffer_.read_data(type_))
            return false;
        switch (type_)
        {
        case STREAMING_TYPE_TYPE:
            ret = true;
            break;
        case STREAMING_TYPE_TYPES:
            ret = true;
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
            ret = true;
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
            if(!buffer_.read_data(size))
                return false;
            stack_.emplace_back(binary_read_stack_data{ (int)size,-1, "root"});
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
    stack_.emplace_back(binary_read_stack_data{ -1, -1, name});
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::end_object ()
{
    RX_ASSERT(!stack_.empty() && stack_.rbegin()->array_size < 0);
    if (!stack_.empty())
    {
        stack_.pop_back();
        return true;
    }
    else
    {
        return false;
    }
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_byte (const char* name, uint8_t& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_value (const char* name, rx_value& val)
{
    return val.deserialize(name, *this);
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_int64 (const char* name, int64_t& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_uint64 (const string_type& name, uint64_t& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_bytes (const char* name, byte_string& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_version (const char* name, uint32_t& val)
{
    if(!buffer_.read_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
void binary_reader<allocT,swap_bytes>::dump_to_stream (std::ostream& out)
{
    buffer_.dump_to_stream(out);
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_init_values (const char* name, data::runtime_values_data& values)
{
    if (name)
    {
        if (!start_object(name))
            return false;
    }
    // reuse it, it's faster!
    string_type temp_name;
    // now enumerate objects
    uint32_t count = 0;
    if (!read_uint("count", count))
        return false;
    for (uint32_t i = 0; i < count; i++)
    {
        temp_name.clear();
        if (!read_string("name", temp_name))
            return false;
        data::runtime_values_data temp_data;
        if (!read_init_values(temp_name.c_str(), temp_data))
            return false;
        values.add_child(temp_name.c_str(), std::move(temp_data));
    }
    if (!read_uint("count", count))
        return false;
    for (uint32_t i = 0; i < count; i++)
    {
        temp_name.clear();
        if (!read_string("name", temp_name))
            return false;
        values::rx_simple_value temp_val;
        if (!temp_val.deserialize(temp_name.c_str(), *this))
            return false;
        values.add_value(temp_name.c_str(), std::move(temp_val));
    }

    if (name)
    {
        if (!end_object())
            return false;
    }
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::is_string_based () const
{
  return false;

}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_item_reference (const char* name, rx_item_reference& ref)
{
    if (!start_object(name))
        return false;

    uint8_t temp;
    if (!read_byte("isId", temp))
        return false;

    if (temp)
    {
        rx_node_id temp_id;
        if (!read_id("id", temp_id))
            return false;

        ref = temp_id;
    }
    else
    {
        string_type temp_str;
        if (!read_string("path", temp_str))
            return false;

        ref = temp_str;
    }
    if (!end_object())
        return false;

    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_reader<allocT,swap_bytes>::read_value_type (const char* name, rx_value_t& val)
{
    uint8_t temp;
    auto ret = read_byte(name, temp);
    if(ret)
        val = temp;
    return ret;
}

template <typename allocT, bool swap_bytes>
string_type binary_reader<allocT,swap_bytes>::get_error () const
{
    std::ostringstream ss;
    bool first = true;
    ss << "[";
    if (!stack_.empty())
    {
        for (auto& one : stack_)
        {
            if (first)
                first = false;
            else
                ss << '.';
            ss << one.name;
        }
    }
    ss << "]";
    return ss.str();
}


// Parameterized Class rx::serialization::binary_writer 

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
bool binary_writer<allocT,swap_bytes>::write_header (int type, size_t size)
{
    if (type == STREAMING_TYPE_MESSAGE)
    {
        json_write_stack_data data;
        data.is_array = false;
        data.name = "msg";
        stack_.push_back(json_write_stack_data{ false, "msg" });
        type_ = type;
        return true;
    }
    else
    {
        if (!buffer_.push_data((uint32_t)get_version()))
            return false;
        if(!buffer_.push_data(type))
            return false;
        type_ = type;
        if (rx::is_serialization_type_array(type))
        {
            if(!buffer_.push_data((uint32_t)size))
                return false;
        }
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
    stack_.emplace_back(json_write_stack_data{ false, name });
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::end_object ()
{
    RX_ASSERT(!stack_.empty() && !stack_.rbegin()->is_array);
    if (!stack_.empty())
    {
        stack_.pop_back();
        return true;
    }
    else
    {
        return false;
    }
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::start_array (const char* name, size_t size)
{
    stack_.emplace_back(json_write_stack_data{ true, name });
    return write_uint(name, (uint32_t)size);
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::end_array ()
{
    RX_ASSERT(!stack_.empty() && stack_.rbegin()->is_array);
    if (!stack_.empty())
    {
        stack_.pop_back();
        return true;
    }
    else
    {
        return false;
    }
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_id (const char* name, const rx_node_id& id)
{
    if (id.is_null())
    {
        uint8_t vals[]{ 0, 0 };
        if(!buffer_.push_data(vals[0]))
            return false;
        if(!buffer_.push_data(vals[1]))
            return false;
    }
    else
    {
        switch (id.get_node_type())
        {
        case rx_node_id_numeric:
            {
                uint16_t namesp = id.get_namespace();
                uint32_t val = id.get_numeric();
                if (namesp == DEFAULT_NAMESPACE && (val < 0x100))
                {// two uint8_ts encoding 0
                    uint8_t vals[]{ 0,(uint8_t)val };
                    if(!buffer_.push_data(vals[0]))
                        return false;
                    if(!buffer_.push_data(vals[1]))
                        return false;
                }
                else if (namesp < 0x100 && val < 0x10000)
                {// four uint8_ts encoding 0
                    uint8_t vals[]{ 1,(uint8_t)namesp };
                    uint16_t val16 = (uint16_t)val;
                    if(!buffer_.push_data(vals[0]))
                        return false;
                    if(!buffer_.push_data(vals[1]))
                        return false;
                    if(!buffer_.push_data(val16))
                        return false;
                }
                else
                {// regular id
                    uint8_t type = 2;
                    if(!buffer_.push_data(type))
                        return false;
                    if(!buffer_.push_data(namesp))
                        return false;
                    if(!buffer_.push_data(val))
                        return false;
                }
            }
            break;
        case rx_node_id_string:
            {
                uint16_t namesp = id.get_namespace();
                string_type val;
                id.get_string(val);

                if(!buffer_.push_data(((uint8_t)3)))
                    return false;
                if(!buffer_.push_data(namesp))
                    return false;
                if(!buffer_.push_data(val.c_str()))
                    return false;
            }
            break;
        case rx_node_id_uuid:
            {
                uint16_t namesp = id.get_namespace();
                auto val=id.get_uuid();

                if(!buffer_.push_data((uint8_t)4))
                    return false;
                if(!buffer_.push_data(namesp))
                    return false;
                if(!buffer_.push_data(val))
                    return false;
            }
            break;
        case rx_node_id_bytes:
            {
                return false;// not implemented yet!!!
            }
            break;
        }
    }
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_string (const char* name, const char* str)
{
    if (str)
    {
        if (!buffer_.push_data(string_type(str)))
            return false;
    }
    else
    {
        if (!buffer_.push_data(string_type()))
            return false;
    }
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_bool (const char* name, bool val)
{
    if(!buffer_.push_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_double (const char* name, double val)
{
    if(!buffer_.push_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_time (const char* name, const rx_time_struct_t& val)
{
    if(!buffer_.push_data(val.t_value))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_uuid (const char* name, const rx_uuid_t& val)
{
    if(!buffer_.push_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_int (const char* name, int val)
{
    if(!buffer_.push_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_uint (const char* name, uint32_t val)
{
    if(!buffer_.push_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_byte (const char* name, uint8_t val)
{
    if(!buffer_.push_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_value (const char* name, const rx_value& val)
{
    if (!val.serialize(name, *this))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_int64 (const char* name, int64_t val)
{
    if(!buffer_.push_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_uint64 (const char* name, uint64_t val)
{
    if(!buffer_.push_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_bytes (const char* name, const std::byte* val, size_t size)
{
    uint32_t len = (uint32_t)size;
    if (!buffer_.push_data(len))
        return false;
    if (len)
    {
        if (!buffer_.push_data(val, size))
            return false;
    }
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_version (const char* name, uint32_t val)
{
    if(!buffer_.push_data(val))
        return false;
    return true;
}

template <typename allocT, bool swap_bytes>
void binary_writer<allocT,swap_bytes>::dump_to_stream (std::ostream& out)
{
    buffer_.dump_to_stream(out);
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_init_values (const char* name, const data::runtime_values_data& values)
{
    if (name)
    {
        if (!start_object(name))
            return false;
    }
    uint32_t count = (uint32_t)values.children.size();
    if (!write_uint("children", count))
        return false;
    for (const auto& one : values.children)
    {
        if (!write_string("name", one.first.c_str()))
            return false;
        if (std::holds_alternative<data::runtime_values_data>(one.second))
        {
            if (!write_init_values(one.first.c_str(), std::get<data::runtime_values_data>(one.second)))
                return false;
        }
        else
        {
            auto& childs = std::get<std::vector<data::runtime_values_data> >(one.second);
            if (!start_array(one.first.c_str(), childs.size()))
                return false;
            for (size_t i = 0; i < childs.size(); i++)
            {
                if (!write_init_values(one.first.c_str(), childs[i]))
                    return false;
            }
            if (!end_array())
                return false;
        }
    }
    count = (uint32_t)values.values.size();
    if (!write_uint("values", count))
        return false;
    for (const auto& one : values.values)
    {
        if (!write_string("name", one.first.c_str()))
            return false;
        if (std::holds_alternative<rx_simple_value>(one.second))
        {
            if (!std::get<rx_simple_value>(one.second).serialize(one.first.c_str(), *this))
                return false;
        }
        else
        {
            auto& vals = std::get<std::vector<rx_simple_value> >(one.second);
            if (!start_array(one.first.c_str(), vals.size()))
                return false;
            for (size_t i = 0; i < vals.size(); i++)
            {
                if (!vals[i].serialize(one.first.c_str(), *this))
                    return false;
            }
            if (!end_array())
                return false;
        }
    }
    if (name)
    {
        if (!end_object())
            return false;
    }
    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::is_string_based () const
{
  return false;

}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_item_reference (const char* name, const rx_item_reference& ref)
{

    if (!start_object(name))
            return false;

    if (ref.is_node_id())
    {
        if (!write_byte("isId", 1))
            return false;
        if (!write_id("id", ref.get_node_id()))
            return false;
    }
    else
    {
        if (!write_byte("isId", 0))
            return false;
        if (!write_string("path", ref.get_path().c_str()))
            return false;
    }
    if (!end_object())
        return false;

    return true;
}

template <typename allocT, bool swap_bytes>
bool binary_writer<allocT,swap_bytes>::write_value_type (const char* name, rx_value_t val)
{
    return write_byte(name, val);
}

template <typename allocT, bool swap_bytes>
string_type binary_writer<allocT,swap_bytes>::get_error () const
{
    std::ostringstream ss;
    bool first = true;
    ss << "[";
    if (!stack_.empty())
    {
        for (auto& one : stack_)
        {
            if (first)
                first = false;
            else
                ss << '.';
            ss << one.name;
        }
    }
    ss << "]";
    return ss.str();
}

template <typename allocT, bool swap_bytes>
byte_string binary_writer<allocT,swap_bytes>::get_data ()
{
    if (buffer_.empty())
        return byte_string();
    std::byte* buffer = buffer_.template get_buffer<std::byte>();
    return byte_string(buffer, buffer + buffer_.get_size());
}


} // namespace serialization
} // namespace rx



#endif
