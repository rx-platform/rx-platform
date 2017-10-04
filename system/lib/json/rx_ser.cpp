
//	  %X% %Q% %Z% %W%

/****************************************************************************
*
*  system\lib\json\rx_ser.cpp - Copyright (c) 2017 Dusan Ciric
*  
*  This file is part of rx-platform
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


#include "stdafx.h"


// rx_ser
#include "system/lib/json/rx_ser.h"



namespace server {

namespace serialization {

// Class server::serialization::json_reader 

json_reader::json_reader()
{
}


json_reader::~json_reader()
{
}



bool json_reader::read_id (const char* name, rx_node_id& id)
{
}

bool json_reader::read_string (const char* name, string_type str)
{
}

bool json_reader::read_bool (const char* name, bool& val)
{
}

bool json_reader::read_double (const char* name, double& val)
{
}

bool json_reader::read_time (const char* name, rx_time_struct_t val)
{
}

bool json_reader::read_uuid (const char* name, rx_uuid_t& val)
{
}

bool json_reader::read_int (const char* name, int& val)
{
}

bool json_reader::read_uint (const char* name, dword& val)
{
}

bool json_reader::start_array (const char* name)
{
}

bool json_reader::array_end ()
{
}

bool json_reader::read_header (int& type)
{
}

bool json_reader::read_footer ()
{
}

bool json_reader::start_object (const char* name)
{
}

bool json_reader::end_object ()
{
}

bool json_reader::read_byte (const char* name, byte& val)
{
}

bool json_reader::read_value (const char* name, rx_value& val)
{
}

bool json_reader::read_int64 (const char* name, sqword& val)
{
}

bool json_reader::read_uint64 (const string_type& name, qword& val)
{
}

bool json_reader::read_bytes (const char* name, byte_string& val)
{
}

Json::Value& json_reader::get_current_value (int& index)
{
}

bool json_reader::parse_data (const string_type& data)
{
}

bool json_reader::safe_read_int (int idx, const string_type& name, int val, const Json::Value& object)
{
}

bool json_reader::safe_read_string (int idx, const string_type& name, string_type& val, const Json::Value& object)
{
}

bool json_reader::read_version (const char* name, dword& val)
{
}

bool json_reader::parse_version_string (dword& result, const string_type& version)
{
}


// Class server::serialization::json_writter 

json_writter::json_writter (int version)
{
}


json_writter::~json_writter()
{
}



bool json_writter::write_id (const char* name, const rx_node_id& id)
{
}

bool json_writter::write_string (const char* name, const char* str)
{
}

bool json_writter::write_bool (const char* name, bool val)
{
}

bool json_writter::write_double (const char* name, double val)
{
}

bool json_writter::write_time (const char* name, const rx_time_struct_t& val)
{
}

bool json_writter::write_uuid (const char* name, const rx_uuid_t& val)
{
}

bool json_writter::write_int (const char* name, int val)
{
}

bool json_writter::write_uint (const char* name, dword val)
{
}

bool json_writter::start_array (const char* name, size_t size)
{
}

bool json_writter::end_array ()
{
}

bool json_writter::write_header (int type)
{
}

bool json_writter::write_footer ()
{
}

bool json_writter::start_object (const char* name)
{
}

bool json_writter::end_object ()
{
}

bool json_writter::write_byte (const char* name, byte val)
{
}

bool json_writter::write_value (const char* name, const rx_value& val)
{
}

bool json_writter::write_int64 (const char* name, sqword val)
{
}

bool json_writter::write_uint64 (const char* name, qword val)
{
}

bool json_writter::write_bytes (const char* name, const byte* val, size_t size)
{
}

Json::Value& json_writter::get_current_value (bool& is_array)
{
}

bool json_writter::get_string (string_type& result, bool decorated)
{
}

bool json_writter::write_version (const char* name, dword val)
{
}

bool json_writter::get_version_string (string_type& result, dword version)
{
}


} // namespace serialization
} // namespace server

