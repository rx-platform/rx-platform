

/****************************************************************************
*
*  system\serialization\rx_ser.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_ser
#include "system/serialization/rx_ser.h"



#ifdef _DEBUG
typedef Json::StyledWriter json_writer_type;
#else
typedef Json::FastWriter json_writer_type;
#endif// _DEBUG



namespace rx_platform {

namespace serialization {

// Class rx_platform::serialization::json_reader 

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

bool json_reader::read_string (const char* name, string_type& str)
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

bool json_reader::read_time (const char* name, rx_time_struct_t& val)
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

bool json_reader::read_uint (const char* name, uint32_t& val)
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
				stack_.push(new_data);
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
			stack_.push(new_data);
			return true;
		}
	}

	return false;
}

bool json_reader::array_end ()
{
	if (stack_.empty())
	{
		assert(false);
		return true;
	}

	json_read_stack_data& data = stack_.top();
	if (data.index >= 0 && data.index<(int)data.value.size())
		return false;

	assert(data.index >= 0);
	stack_.pop();

	return true;
}

bool json_reader::read_header (int& type)
{
	uint32_t version = 0;
	if (read_version("sversion", version))
	{
		set_version(version);
		if (envelope_.isMember("object"))
		{
			Json::Value& temp = envelope_["object"];
			if (temp.isObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				stack_.push(temps);
				type = STREAMING_TYPE_OBJECT;
				return true;
			}
		}
		else if (envelope_.isMember("type"))
		{
			Json::Value& temp = envelope_["type"];
			if (temp.isObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				stack_.push(temps);
				type = STREAMING_TYPE_TYPE;
				return true;
			}
		}
		if (envelope_.isMember("body"))
		{
			Json::Value& temp = envelope_["body"];
			if (temp.isObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				stack_.push(temps);
				type = STREAMING_TYPE_MESSAGE;
				return true;
			}
		}
		else if (envelope_.isMember("check_out"))
		{
			Json::Value& temp = envelope_["check_out"];
			if (temp.isObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				stack_.push(temps);
				type = STREAMING_TYPE_CHECKOUT;
				return true;
			}
		}
		else if (envelope_.isMember("objects"))
		{
			Json::Value& temp = envelope_["objects"];
			if (temp.isArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				stack_.push(temps);
				type = STREAMING_TYPE_OBJECTS;
				return true;
			}
		}

		else if (envelope_.isMember("types"))
		{
			Json::Value& temp = envelope_["types"];
			if (temp.isArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				stack_.push(temps);
				type = STREAMING_TYPE_TYPES;
				return true;
			}
		}
		else if (envelope_.isMember("details"))
		{
			Json::Value& temp = envelope_["details"];
			if (temp.isArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				stack_.push(temps);
				type = STREAMING_TYPE_DETAILS;
				return true;
			}
		}
		else if (envelope_.isMember("values"))
		{
			Json::Value& temp = envelope_["values"];
			if (temp.isArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				stack_.push(temps);
				type = STREAMING_TYPE_VALUES;
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
				stack_.push(new_data);
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
			stack_.push(new_data);
			return true;
		}
	}
	return false;
}

bool json_reader::end_object ()
{
	if (stack_.empty())
		return false;

	json_read_stack_data data = stack_.top();
	if (data.index >= 0)
		return false;

	stack_.pop();

	return true;
}

bool json_reader::read_byte (const char* name, uint8_t& val)
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
				val = (uint8_t)temp.asUInt();
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
				val = (uint8_t)temp.asUInt();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_value (const char* name, rx_value& val)
{
	if (!val.deserialize(name, *this))
		return false;

	return true;
}

bool json_reader::read_int64 (const char* name, int64_t& val)
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
				val = temp.asInt64();
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
				val = temp.asInt64();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_uint64 (const string_type& name, uint64_t& val)
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
				val = temp.asUInt64();
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
				val = temp.asUInt64();
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
				temp_val.push_back((uint8_t)one);
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
	if (stack_.empty())
	{
		index = -1;
		return envelope_;
	}
	else
	{
		json_read_stack_data& current = stack_.top();
		index = current.index;
		if (index >= 0)
			current.index = current.index + 1;
		return current.value;
	}
}

bool json_reader::parse_data (const string_type& data)
{
	errors_.clear();
	Json::Reader reader;
	auto ret = reader.parse(data, envelope_, false);
	if (!ret)
	{
		std::istringstream stream(reader.getFormattedErrorMessages());
		while (!stream.eof())
		{
			string_type temp;
			std::getline(stream, temp);
			if (!temp.empty())
				errors_.emplace_back(temp);
		}
	}
	return ret;
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
			else if (temp.isNull())
			{
				val.clear();
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
			else if (temp.isNull())
			{
				val.clear();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_version (const char* name, uint32_t& val)
{
	string_type str;
	if (!read_string(name, str))
		return false;
	if (!parse_version_string(val, str))
		return false;
	return true;
}

bool json_reader::parse_version_string (uint32_t& result, const string_type& version)
{
	size_t idx = version.find('.');
	if (idx != string_type::npos)
	{
		int major = atoi(version.substr(0, idx).c_str());
		int minor = atoi(version.substr(idx + 1).c_str());
		if (major >= 0 && minor >= 0 && major < 0xffff && minor < 0xffff)
		{
			result = (((uint32_t)major) << 16) | (((uint32_t)minor)&0xffff);
			return true;
		}
	}
	return false;
}

bool json_reader::read_init_values (const char* name, data::runtime_values_data& values)
{
	if (!start_object(name))
		return false;
	// now enumerate objects
	int index = 0;
	Json::Value& val = get_current_value(index);

	if (!val.isObject())
		return false;

	if (!internal_read_init_values(values, val))
		return false;


	if (!end_object())
		return false;
	return true;
}

bool json_reader::internal_read_init_values (data::runtime_values_data& values, Json::Value& val)
{
	auto names = val.getMemberNames();
	for (auto& one : names)
	{
		Json::Value& temp = val[one];
		if (temp.isBool())
		{
			values.add_value_static(one, temp.asBool());
		}
		else if (temp.isInt())
		{
			values.add_value_static(one, temp.asInt());
		}
		/*else if (temp.isInt64())
		{
			values.add_value_static(one, temp.asInt64());
		}*/
		else if (temp.isDouble())
		{
			values.add_value_static(one, temp.asDouble());
		}
		else if (temp.isString())
		{
			values.add_value_static(one, temp.asString());
		}
		else if (temp.isObject())
		{
			auto& child = values.add_child(one);
			if (!internal_read_init_values(child, temp))
				return false;
		}
		else
		{// unknown format
			return false;
		}
	}
	return true;
}

string_array json_reader::get_errors () const
{
	return errors_;
}

bool json_reader::is_string_based () const
{
  return true;

}

bool json_reader::read_item_reference (const char* name, rx_item_reference& ref)
{

	if (!start_object(name))
		return false;

	int index = 0;
	Json::Value& val = get_current_value(index);

	if (!val.isObject())
		return false;

	if (val.isMember("id"))
	{
		rx_node_id temp_id;
		if (!read_id("id", temp_id))
			return false;
		ref = std::move(temp_id);
	}
	else if (val.isMember("path"))
	{
		string_type temp_str;
		if (!read_string("path", temp_str))
			return false;
		ref = std::move(temp_str);
	}

	if (!end_object())
		return false;
	return true;

}

bool json_reader::read_value (const char* name, rx_simple_value& val)
{

	if (!val.deserialize(name, *this))
		return false;

	return true;
}


// Class rx_platform::serialization::json_writer 

json_writer::json_writer (int version)
	: base_meta_writer(version)
	, type_(0)
{
}


json_writer::~json_writer()
{
}



bool json_writer::write_id (const char* name, const rx_node_id& id)
{
	string_type buff;
	id.to_string(buff);

	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(buff);
	else
		value[name] = buff;

	return true;
}

bool json_writer::write_string (const char* name, const string_type& str)
{
	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(str);
	else
		value[name] = str;

	return true;
}

bool json_writer::write_bool (const char* name, bool val)
{
	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(val);
	else
		value[name] = val;

	return true;
}

bool json_writer::write_double (const char* name, double val)
{
	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(val);
	else
		value[name] = val;

	return true;
}

bool json_writer::write_time (const char* name, const rx_time_struct_t& val)
{
	rx_time tval(val);
	string_type str = tval.get_IEC_string();

	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(str.c_str());
	else
		value[name] = str.c_str();

	return true;
}

bool json_writer::write_uuid (const char* name, const rx_uuid_t& val)
{
	string_type buff;
	rx_uuid vval(val);
	vval.to_string(buff);

	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(buff);
	else
		value[name] = buff;

	return true;
}

bool json_writer::write_int (const char* name, int val)
{
	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(val);
	else
		value[name] = val;

	return true;
}

bool json_writer::write_uint (const char* name, uint32_t val)
{
	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append(val);
	else
		value[name] = val;

	return true;
}

bool json_writer::start_array (const char* name, size_t size)
{
	json_write_stack_data data;
	data.is_array = true;
	data.name = name;

	stack_.push(data);

	return true;
}

bool json_writer::end_array ()
{
	if (stack_.empty())
		return false;

	json_write_stack_data data = stack_.top();
	if (!data.is_array)
		return false;

	stack_.pop();

	bool is_array = false;
	Json::Value& value = get_current_value(is_array);

	if (is_array)
		value.append(data.value);
	else
		value[data.name] = data.value;

	return true;
}

bool json_writer::write_header (int type, size_t size)
{
	type_ = type;
	if (type_ == STREAMING_TYPE_MESSAGE)
	{
		json_write_stack_data data;
		data.is_array = false;
		data.name = "perica";
		stack_.push(data);
		return true;
	}

	string_type ver;
	if (!get_version_string(ver, (uint32_t)get_version()))
		return false;

	envelope_["sversion"] = ver;


	json_write_stack_data data;

	switch (type_)
	{
	case STREAMING_TYPE_TYPE:
		data.is_array = false;
		data.name = "type";
		break;
	case STREAMING_TYPE_TYPES:
		data.is_array = false;
		data.name = "types";
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
	case STREAMING_TYPE_VALUES:
		data.is_array = true;
		data.name = "values";
		break;
	default:
		assert(false);
		return false;
	}

	stack_.push(data);

	return true;
}

bool json_writer::write_footer ()
{
	if (type_ == STREAMING_TYPE_MESSAGE)
	{
		return true;
	}
	else
	{
		if (stack_.empty())
			return false;

		json_write_stack_data data = stack_.top();
		envelope_[data.name] = data.value;

		return true;
	}
}

bool json_writer::start_object (const char* name)
{
	json_write_stack_data data;
	data.is_array = false;
	data.name = name;

	stack_.push(data);

	return true;
}

bool json_writer::end_object ()
{
	if (stack_.empty())
		return false;

	json_write_stack_data data = stack_.top();
	if (data.is_array)
		return false;

	stack_.pop();

	bool is_array = false;
	Json::Value& value = get_current_value(is_array);

	if (is_array)
		value.append(data.value);
	else
		value[data.name] = data.value;

	return true;
}

bool json_writer::write_byte (const char* name, uint8_t val)
{
	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append((uint32_t)val);
	else
		value[name] = val;

	return true;
}

bool json_writer::write_value (const char* name, const rx_value& val)
{
	if (!val.serialize(name, *this))
		return false;
	return true;
}

bool json_writer::write_int64 (const char* name, int64_t val)
{
	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append((double)val);
	else
		value[name] = (double)val;

	return true;
}

bool json_writer::write_uint64 (const char* name, uint64_t val)
{
	bool is_array = false;
	Json::Value& value = get_current_value(is_array);
	if (is_array)
		value.append((double)val);
	else
		value[name] = (double)val;

	return true;
}

bool json_writer::write_bytes (const char* name, const uint8_t* val, size_t size)
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

Json::Value& json_writer::get_current_value (bool& is_array)
{
	if (stack_.empty())
	{
		if(type_!= STREAMING_TYPE_MESSAGE)
			assert(false);
		return envelope_;
	}
	else
	{
		json_write_stack_data& current = stack_.top();
		is_array = current.is_array;
		return current.value;
	}
}

bool json_writer::get_string (string_type& result, bool decorated)
{
	if (type_ == STREAMING_TYPE_MESSAGE)
	{
		json_writer_type writer;

		if (stack_.empty())
			return false;

		json_write_stack_data data = stack_.top();

		result = writer.write(data.value);
	}
	else
	{
		if (decorated)
		{
			Json::StyledWriter writer;
			result = writer.write(envelope_);
		}
		else
		{
			json_writer_type writer;
			result = writer.write(envelope_);
		}
	}
	return true;
}

bool json_writer::write_version (const char* name, uint32_t val)
{
	string_type str;
	if (!get_version_string(str, val))
		return false;
	return write_string(name, str.c_str());
}

bool json_writer::get_version_string (string_type& result, uint32_t version)
{
	int major = (version >> 16);
	int minor = (version & 0xffff);
	char buff[0x20];
	sprintf(buff, "%d.%d", major, minor);
	result = buff;
	return true;
}

bool json_writer::write_init_values (const char* name, const data::runtime_values_data& values)
{
	if (!start_object(name))
		return false;
	for (const auto& one : values.children)
	{
		if (!write_init_values(one.first.c_str(), one.second))
			return false;
	}
	for (const auto& one : values.values)
	{
		if (!one.second.value.get_storage().weak_serialize_value(one.first, *this))
			return false;
	}
	if (!end_object())
		return false;
	return true;
}

bool json_writer::is_string_based () const
{
  return true;

}

bool json_writer::write_item_reference (const char* name, const rx_item_reference& ref)
{
	if (!start_object(name))
		return false;
	if (!ref.is_null())
	{
		if (ref.is_node_id())
		{
			if (!write_id("id", ref.get_node_id()))
				return false;
		}
		else
		{
			if (!write_string("path", ref.get_path()))
				return false;
		}
	}
	if (!end_object())
		return false;
	return true;
}

bool json_writer::write_value (const char* name, const rx_simple_value& val)
{
	if (!val.serialize(name, *this))
		return false;
	return true;
}


} // namespace serialization
} // namespace rx_platform

