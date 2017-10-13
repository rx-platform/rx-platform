

/****************************************************************************
*
*  system\json\rx_ser.cpp
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


#include "stdafx.h"


// rx_ser
#include "system/json/rx_ser.h"



#ifdef _DEBUG
typedef Json::StyledWriter json_writer_type;
#else
typedef Json::FastWriter json_writer_type;
#endif// _DEBUG



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
	string_type str;
	int index = 0;
	Json::Value& val = get_current_value(index);

	if (safe_read_string(index, name, str, val))
	{
		id = rx_node_id::from_string(str.c_str());
		return true;
	}
	return false;
}

bool json_reader::read_string (const char* name, string_type str)
{
	string_type sstr;
	int index = 0;
	Json::Value& val = get_current_value(index);

	if (safe_read_string(index, name, sstr, val))
	{
		str = sstr.c_str();
		return true;
	}
	return false;
}

bool json_reader::read_bool (const char* name, bool& val)
{
	int idx = 0;
	Json::Value& object = get_current_value(idx);
	if (idx<0)
	{
		if (object.isMember(name))
		{
			Json::Value& temp = object[name];
			if (temp.isBool())
			{
				val = temp.asBool();
				return true;
			}
		}
	}
	else
	{
		if (object.isArray() && (int)object.size()>idx)
		{
			Json::Value& temp = object[idx];
			if (temp.isBool())
			{
				val = temp.asBool();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_double (const char* name, double& val)
{
	int idx = 0;
	Json::Value& object = get_current_value(idx);
	if (idx<0)
	{
		if (object.isMember(name))
		{
			Json::Value& temp = object[name];
			if (temp.isNumeric())
			{
				val = temp.asDouble();
				return true;
			}
		}
	}
	else
	{
		if (object.isArray() && (int)object.size()>idx)
		{
			Json::Value& temp = object[idx];
			if (temp.isNumeric())
			{
				val = temp.asDouble();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_time (const char* name, rx_time_struct_t val)
{
	string_type str;
	int index = 0;
	Json::Value& object = get_current_value(index);

	if (safe_read_string(index, name, str, object))
	{
		rx_time temp = rx_time::from_IEC_string(str.c_str());
		if (temp.is_null())
		{
			temp = rx_time::now();
		}
		val = temp;
		return true;
	}
	return false;
}

bool json_reader::read_uuid (const char* name, rx_uuid_t& val)
{
	string_type str;
	int index = 0;
	Json::Value& object = get_current_value(index);

	if (safe_read_string(index, name, str, object))
	{
		val = rx_uuid::create_from_string(str.c_str()).uuid();
		return true;
	}
	return false;
}

bool json_reader::read_int (const char* name, int& val)
{
	int idx = 0;
	Json::Value& object = get_current_value(idx);
	if (idx<0)
	{
		if (object.isMember(name))
		{
			Json::Value& temp = object[name];
			if (temp.isIntegral())
			{
				val = temp.asInt();
				return true;
			}
		}
	}
	else
	{
		if (object.isArray() && (int)object.size()>idx)
		{
			Json::Value& temp = object[idx];
			if (temp.isIntegral())
			{
				val = temp.asInt();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_uint (const char* name, dword& val)
{
	int idx = 0;
	Json::Value& object = get_current_value(idx);
	if (idx<0)
	{
		if (object.isMember(name))
		{
			Json::Value& temp = object[name];
			if (temp.isIntegral())
			{
				val = temp.asUInt();
				return true;
			}
		}
	}
	else
	{
		if (object.isArray() && (int)object.size()>idx)
		{
			Json::Value& temp = object[idx];
			if (temp.isIntegral())
			{
				val = temp.asUInt();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::start_array (const char* name)
{
	int index = -1;
	Json::Value& current = get_current_value(index);
	if (index<0)
	{
		assert(!current.isArray());
		if (current.isMember(name))
		{
			Json::Value& val = current[name];
			if (val.isArray())
			{
				json_read_stack_data new_data(val);
				new_data.index = 0;
				_stack.push(new_data);
				return true;
			}
		}
	}
	else
	{// array stuff
		assert(current.isArray());
		assert((int)current.size()>index);
		Json::Value& val = current[index];
		if (val.isArray())
		{
			json_read_stack_data new_data(val);
			new_data.index = 0;
			_stack.push(new_data);
			return true;
		}
	}

	return false;
}

bool json_reader::array_end ()
{
	if (_stack.empty())
	{
		assert(false);
		return true;
	}

	json_read_stack_data& data = _stack.top();
	if (data.index >= 0 && data.index<(int)data.value.size())
		return false;

	assert(data.index >= 0);
	_stack.pop();

	return true;
}

bool json_reader::read_header (int& type)
{
	int version = 0;
	int idx = -1;
	if (safe_read_int(idx, "sversion", version, _envelope))
	{
		set_version(version);
		if (_envelope.isMember("object"))
		{
			Json::Value& temp = _envelope["object"];
			if (temp.isObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				_stack.push(temps);
				type = STREAMING_TYPE_OBJECT;
				return true;
			}
		}
		if (_envelope.isMember("body"))
		{
			Json::Value& temp = _envelope["body"];
			if (temp.isObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				_stack.push(temps);
				type = STREAMING_TYPE_MESSAGE;
				return true;
			}
		}
		else if (_envelope.isMember("check_out"))
		{
			Json::Value& temp = _envelope["check_out"];
			if (temp.isObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				_stack.push(temps);
				type = STREAMING_TYPE_CHECKOUT;
				return true;
			}
		}
		else if (_envelope.isMember("objects"))
		{
			Json::Value& temp = _envelope["objects"];
			if (temp.isArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				_stack.push(temps);
				type = STREAMING_TYPE_OBJECTS;
				return true;
			}
		}
		else if (_envelope.isMember("details"))
		{
			Json::Value& temp = _envelope["details"];
			if (temp.isArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				_stack.push(temps);
				type = STREAMING_TYPE_DETAILS;
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_footer ()
{
	return true;
}

bool json_reader::start_object (const char* name)
{
	int index = false;
	Json::Value& current = get_current_value(index);
	if (index<0)
	{
		if (current.isMember(name))
		{
			Json::Value& val = current[name];
			if (val.isObject())
			{
				json_read_stack_data new_data(val);
				new_data.index = -1;
				_stack.push(new_data);
				return true;
			}
		}
	}
	else
	{
		assert(current.isArray());
		assert((int)current.size()>index);
		Json::Value& val = current[index];
		if (val.isObject())
		{
			json_read_stack_data new_data(val);
			new_data.index = -1;
			_stack.push(new_data);
			return true;
		}
	}
	return false;
}

bool json_reader::end_object ()
{
	if (_stack.empty())
		return false;

	json_read_stack_data data = _stack.top();
	if (data.index >= 0)
		return false;

	_stack.pop();

	return true;
}

bool json_reader::read_byte (const char* name, byte& val)
{
	int idx = 0;
	Json::Value& object = get_current_value(idx);
	if (idx<0)
	{
		if (object.isMember(name))
		{
			Json::Value& temp = object[name];
			if (temp.isIntegral())
			{
				val = (byte)temp.asUInt();
				return true;
			}
		}
	}
	else
	{
		if (object.isArray() && (int)object.size()>idx)
		{
			Json::Value& temp = object[idx];
			if (temp.isIntegral())
			{
				val = (byte)temp.asUInt();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_value (const char* name, rx_value& val)
{
	if (!start_object(name))
		return false;

	if (!val.deserialize(*this))
		return false;

	if (!end_object())
		return false;

	return true;
}

bool json_reader::read_int64 (const char* name, sqword& val)
{
	int idx = 0;
	Json::Value& object = get_current_value(idx);
	if (idx<0)
	{
		if (object.isMember(name))
		{
			Json::Value& temp = object[name];
			if (temp.isIntegral())
			{
				val = temp.asInt();
				return true;
			}
		}
	}
	else
	{
		if (object.isArray() && (int)object.size()>idx)
		{
			Json::Value& temp = object[idx];
			if (temp.isIntegral())
			{
				val = temp.asInt();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_uint64 (const string_type& name, qword& val)
{
	int idx = 0;
	Json::Value& object = get_current_value(idx);
	if (idx<0)
	{
		if (object.isMember(name))
		{
			Json::Value& temp = object[name];
			if (temp.isIntegral())
			{
				val = temp.asUInt();
				return true;
			}
		}
	}
	else
	{
		if (object.isArray() && (int)object.size()>idx)
		{
			Json::Value& temp = object[idx];
			if (temp.isIntegral())
			{
				val = temp.asUInt();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_bytes (const char* name, byte_string& val)
{
	string_type sstr;
	int index = 0;
	Json::Value& jval = get_current_value(index);

	if (safe_read_string(index, name, sstr, jval))
	{
		byte_string temp_val;
		char buff[8];
		buff[2] = L'\0';
		size_t count = sstr.size();
		if (count > 1)
		{
			temp_val.reserve(count / 2 + 1);
			for (size_t i = 0; i < count; i += 2)
			{
				buff[0] = sstr[i];
				if (i + 1 < count)
					buff[1] = sstr[i + 1];
				else
					buff[1] = L'\0';
				char* endptr = NULL;
				unsigned long one = strtoul(buff, &endptr, 16);
				temp_val.push_back((byte)one);
			}
			val = temp_val;
		}
		else
			val = byte_string();

		return true;
	}
	return false;
}

Json::Value& json_reader::get_current_value (int& index)
{
	if (_stack.empty())
	{
		assert(false);
		index = -1;
		return _envelope;
	}
	else
	{
		json_read_stack_data& current = _stack.top();
		index = current.index;
		if (index >= 0)
			current.index = current.index + 1;
		return current.value;
	}
}

bool json_reader::parse_data (const string_type& data)
{
	Json::Reader reader;
	return reader.parse(data, _envelope, false);
}

bool json_reader::safe_read_int (int idx, const string_type& name, int val, const Json::Value& object)
{
	if (idx<0)
	{
		if (object.isMember(name))
		{
			const Json::Value& temp = object[name];
			if (temp.isIntegral())
			{
				val = temp.asInt();
				return true;
			}
		}
	}
	else
	{
		if (object.isArray() && (int)object.size()>idx)
		{
			const Json::Value& temp = object[idx];
			if (temp.isIntegral())
			{
				val = temp.asInt();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::safe_read_string (int idx, const string_type& name, string_type& val, const Json::Value& object)
{
	if (idx<0)
	{
		if (object.isMember(name))
		{
			const Json::Value& temp = object[name];
			if (temp.isString())
			{
				val = temp.asCString();
				return true;
			}
		}
	}
	else
	{
		if (object.isArray() && (int)object.size()>idx)
		{
			const Json::Value& temp = object[idx];
			if (temp.isString())
			{
				val = temp.asCString();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_version (const char* name, dword& val)
{
	string_type str;
	if (!read_string(name, str))
		return false;
	if (!parse_version_string(val, str))
		return false;
	return true;
}

bool json_reader::parse_version_string (dword& result, const string_type& version)
{
	return false;
}


// Class server::serialization::json_writter 

json_writter::json_writter (int version)
	: base_meta_writter(version)
{
}


json_writter::~json_writter()
{
}



bool json_writter::write_id (const char* name, const rx_node_id& id)
{
	string_type buff;
	id.to_string(buff);

	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(buff);
	else
		value[name] = buff;

	return true;
}

bool json_writter::write_string (const char* name, const char* str)
{
	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(str);
	else
		value[name] = str;

	return true;
}

bool json_writter::write_bool (const char* name, bool val)
{
	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(val);
	else
		value[name] = val;

	return true;
}

bool json_writter::write_double (const char* name, double val)
{
	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(val);
	else
		value[name] = val;

	return true;
}

bool json_writter::write_time (const char* name, const rx_time_struct_t& val)
{
	rx_time tval(val);
	string_type str = tval.get_IEC_string();

	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(str.c_str());
	else
		value[name] = str.c_str();

	return true;
}

bool json_writter::write_uuid (const char* name, const rx_uuid_t& val)
{
	string_type buff;
	rx_uuid vval(val);
	vval.to_string(buff);

	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(buff);
	else
		value[name] = buff;

	return true;
}

bool json_writter::write_int (const char* name, int val)
{
	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(val);
	else
		value[name] = val;

	return true;
}

bool json_writter::write_uint (const char* name, dword val)
{
	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(val);
	else
		value[name] = val;

	return true;
}

bool json_writter::start_array (const char* name, size_t size)
{
	json_write_stack_data data;
	data.is_array = true;
	data.name = name;

	_stack.push(data);

	return true;
}

bool json_writter::end_array ()
{
	if (_stack.empty())
		return false;

	json_write_stack_data data = _stack.top();
	if (!data.is_array)
		return false;

	_stack.pop();

	bool is_array;
	Json::Value& value = get_current_value(is_array);

	if (is_array)
		value.append(data.value);
	else
		value[data.name] = data.value;

	return true;
}

bool json_writter::write_header (int type)
{
	_type = type;
	string_type ver;
	if (!get_version_string(ver, get_version()))
		return false;

	_envelope["sversion"] = ver;


	json_write_stack_data data;

	switch (_type)
	{
	case STREAMING_TYPE_CLASS:
		data.is_array = false;
		data.name = "class";
		break;
	case STREAMING_TYPE_CLASSES:
		data.is_array = false;
		data.name = "classes";
		break;
	case STREAMING_TYPE_OBJECT:
		data.is_array = false;
		data.name = "object";
		break;
	case STREAMING_TYPE_MESSAGE:
		data.is_array = false;
		data.name = "body";
		break;
	case STREAMING_TYPE_OBJECTS:
		data.is_array = false;
		data.name = "objects";
		break;
	case STREAMING_TYPE_CHECKOUT:
		data.is_array = false;
		data.name = "check_out";
		break;
	case STREAMING_TYPE_DETAILS:
		data.is_array = true;
		data.name = "details";
		break;
	default:
		assert(false);
		return false;
	}

	_stack.push(data);

	return true;
}

bool json_writter::write_footer ()
{
	if (_stack.empty())
		return false;

	json_write_stack_data data = _stack.top();
	_envelope[data.name] = data.value;

	return true;
}

bool json_writter::start_object (const char* name)
{
	json_write_stack_data data;
	data.is_array = false;
	data.name = name;

	_stack.push(data);

	return true;
}

bool json_writter::end_object ()
{
	if (_stack.empty())
		return false;

	json_write_stack_data data = _stack.top();
	if (data.is_array)
		return false;

	_stack.pop();

	bool is_array;
	Json::Value& value = get_current_value(is_array);

	if (is_array)
		value.append(data.value);
	else
		value[data.name] = data.value;

	return true;
}

bool json_writter::write_byte (const char* name, byte val)
{
	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append((dword)val);
	else
		value[name] = val;

	return true;
}

bool json_writter::write_value (const char* name, const rx_value& val)
{
	if (!start_object(name))
		return false;

	if (!val.serialize(*this))
		return false;

	if (!end_object())
		return false;

	return true;
}

bool json_writter::write_int64 (const char* name, sqword val)
{
	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append((double)val);
	else
		value[name] = (double)val;

	return true;
}

bool json_writter::write_uint64 (const char* name, qword val)
{
	bool is_array;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append((double)val);
	else
		value[name] = (double)val;

	return true;
}

bool json_writter::write_bytes (const char* name, const byte* val, size_t size)
{
	char buff[8];
	string_type temp;
	for (size_t i = 0; i < size; i++)
	{
		snprintf(buff, sizeof(buff) / sizeof(buff[0]), "%02x", (int)val[i]);
		temp += buff;
	}
	return write_string(name, temp.c_str());
}

Json::Value& json_writter::get_current_value (bool& is_array)
{
	if (_stack.empty())
	{
		assert(false);
		return _envelope;
	}
	else
	{
		json_write_stack_data& current = _stack.top();
		is_array = current.is_array;
		return current.value;
	}
}

bool json_writter::get_string (string_type& result, bool decorated)
{
	if (decorated)
	{
		Json::StyledWriter writer;
		result = writer.write(_envelope);
	}
	else
	{
		json_writer_type writer;
		result = writer.write(_envelope);
	}
	return true;
}

bool json_writter::write_version (const char* name, dword val)
{
	string_type str;
	if (!get_version_string(str, val))
		return false;
	return write_string(name, str.c_str());
}

bool json_writter::get_version_string (string_type& result, dword version)
{
	int major = (version >> 24);
	int minor = ((version >> 16) & 0xff);
	int build = (version & 0xffff);
	char buff[0x20];
	sprintf(buff, "%d.%d.%d", major, minor, build);
	result = buff;
	return true;
}


} // namespace serialization
} // namespace server

