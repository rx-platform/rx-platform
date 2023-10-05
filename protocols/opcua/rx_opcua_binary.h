

/****************************************************************************
*
*  protocols\opcua\rx_opcua_binary.h
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


#ifndef rx_opcua_binary_h
#define rx_opcua_binary_h 1


#include "rx_opcua_params.h"
#include "lib/rx_io_buffers.h"



using namespace protocols::opcua::common;

namespace protocols {
namespace opcua {
namespace binary {

class ua_binary_istream
{

public:
    ua_binary_istream(rx::io::rx_const_io_buffer* buffer);
    ~ua_binary_istream();
    ua_binary_istream& operator >> (bool& val);
    ua_binary_istream& operator >> (uint16_t& val);
    ua_binary_istream& operator >> (int16_t& val);
    ua_binary_istream& operator >> (uint32_t& val);
    ua_binary_istream& operator >> (int32_t& val);
    ua_binary_istream& operator >> (float& val);
    ua_binary_istream& operator >> (double& val);
    ua_binary_istream& operator >> (string_type& val);
    ua_binary_istream& operator >> (rx_time& val);
    ua_binary_istream& operator >> (rx_time_struct& val);
    ua_binary_istream& operator >> (uint8_t& val);
    ua_binary_istream& operator >> (byte_string& val);
    ua_binary_istream& operator >> (diagnostic_info& val);
    ua_binary_istream& operator >> (localized_text& val);
    ua_binary_istream& operator >> (rx_uuid_t& val);
    ua_binary_istream& operator >> (rx_node_id& val);
    template<typename baseType = ua_extension>
    std::unique_ptr<baseType> deserialize_extension(std::function<std::unique_ptr<baseType>(const rx_node_id& id)> creator_func)
    {
        rx_node_id id;
        (*this) >> id;
        std::unique_ptr<baseType> val = creator_func(id);
        uint8_t encoding;
        (*this) >> encoding;
        if (val != NULL)
        {
            if (encoding == 1)
            {
                int len;
                (*this) >> len;
                val->internal_deserialize_extension(*this);
            }

        }
        else
        {
            if (encoding != 0)
            {// do the dummy read for this stuff
                byte_string dummy;
                (*this) >> dummy;
            }
        }
        return val;
    }
    ua_binary_istream& operator >> (node_class_type& val);
    ua_binary_istream& operator >> (browse_direction_type& val);
    ua_binary_istream& operator >> (timestamps_return_type& val);
    ua_binary_istream& operator >> (attribute_id& val);
    ua_binary_istream& operator >> (data_value& val);
    ua_binary_istream& operator >> (qualified_name& val);
    ua_binary_istream& operator >> (variant_type& val);
    ua_binary_istream& operator >> (char& val);
    ua_binary_istream& operator >> (int64_t& val);
    ua_binary_istream& operator >> (uint64_t& val);
    ua_binary_istream& operator >> (change_trigger_type& val);
    ua_binary_istream& operator >> (monitoring_mode_t& val);

    template<typename T>
    ua_binary_istream& operator >> (typename std::vector<T>& val)
    {
        uint32_t len = 0;
        (*this) >> len;
        if (len)
        {
            val.resize(0);
            val.reserve(len);
            for (uint32_t i = 0; i < len; i++)
            {
                T temp;
                (*this) >> temp;
                val.push_back(std::move(temp));
            }
        }
        else
        {
            val.clear();
        }
        return *this;
    }


    template<typename T>
    void deserialize_array(typename std::vector<T>& val)
    {
        int len = 0;
        (*this) >> len;
        if (len >= 0)
        {
            val.resize(0);
            val.reserve(len);
            for (int i = 0; i < len; i++)
            {
                T temp;
                temp.deserialize(*this);
                val.push_back(std::move(temp));
            }
        }
        else
        {
            val.clear();
        }
    }

private:
    void deserialize_vunion(uint8_t type, vunion_type& vu);

    rx::io::rx_const_io_buffer* buffer_;

};


class ua_binary_ostream
{

public:
    ua_binary_ostream(rx::io::rx_io_buffer* buffer);
    ~ua_binary_ostream();
    ua_binary_ostream& operator << (bool val);
    ua_binary_ostream& operator << (uint16_t val);
    ua_binary_ostream& operator << (int16_t val);
    ua_binary_ostream& operator << (uint32_t val);
    ua_binary_ostream& operator << (int32_t val);
    ua_binary_ostream& operator << (float val);
    ua_binary_ostream& operator << (double val);
    ua_binary_ostream& operator << (const string_type& val);
    ua_binary_ostream& operator << (const rx_time_struct& val);
    ua_binary_ostream& operator << (const rx_time& val);
    ua_binary_ostream& operator << (const byte_string& val);
    ua_binary_ostream& operator << (uint8_t val);
    ua_binary_ostream& operator << (const diagnostic_info& val);
    ua_binary_ostream& operator << (const localized_text& val);
    ua_binary_ostream& operator << (const rx_uuid_t& val);
    ua_binary_ostream& operator << (const rx_node_id& val);
    void serialize_extension(const ua_extension* val);
    ua_binary_ostream& operator << (const node_class_type val);
    ua_binary_ostream& operator << (const browse_direction_type val);
    ua_binary_ostream& operator << (const timestamps_return_type val);
    ua_binary_ostream& operator << (const attribute_id val);
    ua_binary_ostream& operator << (const variant_type& val);
    ua_binary_ostream& operator << (const data_value& val);
    ua_binary_ostream& operator << (const qualified_name& val);
    ua_binary_ostream& operator << (char val);
    ua_binary_ostream& operator << (int64_t val);
    ua_binary_ostream& operator << (uint64_t val);
    ua_binary_ostream& operator << (change_trigger_type val);
    ua_binary_ostream& operator << (monitoring_mode_t val);

    template<typename T>
    ua_binary_ostream& operator << (const std::vector<T>& val)
    {
        uint32_t len = (uint32_t)val.size();
        (*this) << len;
        if (len)
        {
            for (uint32_t i = 0; i < len; i++)
            {
                (*this) << val[i];
            }
        }
        return *this;
    }

    template<typename T>
    void serialize_array(const std::vector<T>& val)
    {
        int len = (int)val.size();
        (*this) << len;
        if (len >= 0)
        {
            for (int i = 0; i < len; i++)
            {
                val[i].serialize(*this);
            }
        }
    }

private:
    void serialize_vunion(uint8_t type, const vunion_type& vu);

    rx::io::rx_io_buffer* buffer_;

};



} //namespace opcua
} //namespace binary
} //namespace protocols


#endif
