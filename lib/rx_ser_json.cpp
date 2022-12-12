

/****************************************************************************
*
*  lib\rx_ser_json.cpp
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


#include "pch.h"


// rx_ser_json
#include "lib/rx_ser_json.h"


// placement new and MSVC debug heap problem
#ifdef _MSC_VER
#ifdef _DEBUG
#undef new
#endif
#endif
#include "third-party/rapidjson/include/rapidjson/document.h"
#include "third-party/rapidjson/include/rapidjson/error/en.h"
#include "third-party/rapidjson/include/rapidjson/prettywriter.h"

// placement new and MSVC debug heap problem
#ifdef _MSC_VER
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
#endif

#include "base64.h"



namespace rx {

namespace serialization {


namespace
{
string_type get_parser_error(rapidjson::ParseErrorCode err, size_t pos, const string_type& data)
{
	size_t row = 0, col = 0;
	size_t max = std::min(pos, data.size());
	size_t i = 0;
	while (i < max)
	{
		if (data[i] == '\r' || data[i] == '\n')
		{
			i++;
			if (i < max && (data[i] == '\r' || data[i] == '\n'))
				i++;
			col = 0;
			row++;
		}
		else
		{
			i++;
			col++;
		}
	}
	std::ostringstream ss;
	ss << "Parsing error at line " << row
		<< ", column " << col
		<< ": "
		<< rapidjson::GetParseError_En(err);
	return ss.str();
}
}

struct json_read_stack_data
{
public:
	json_read_stack_data(rapidjson::Value& vval)
		: value(vval)
		, index(0)
	{
	}
	string_type name;
	rapidjson::Value& value;
	int index;
};

struct json_reader_data
{
	std::vector<json_read_stack_data> stack;
	rapidjson::Document envelope;

	rapidjson::Value& get_current_value(int& index)
	{
		if (stack.empty())
		{
			index = -1;
			return envelope;
		}
		else
		{
			json_read_stack_data& current = *stack.rbegin();
			index = current.index;
			if (index >= 0)
				current.index = current.index + 1;
			return current.value;
		}
	}
	bool safe_read_int(int idx, const string_type& name, int val, const rapidjson::Value& object)
	{

		if (idx < 0)
		{
			if (object.HasMember(name.c_str()))
			{
				const rapidjson::Value& temp = object[name.c_str()];
				if (temp.IsInt())
				{
					val = temp.GetInt();
					return true;
				}
			}
		}
		else
		{
			if (object.IsArray() && (int)object.GetArray().Size() > idx)
			{
				const rapidjson::Value& temp = object[idx];
				if (temp.IsInt())
				{
					val = temp.GetInt();
					return true;
				}
			}
		}
		return false;
	}
	bool safe_read_string(int idx, const string_type& name, string_type& val, const rapidjson::Value& object)
	{

		if (idx < 0)
		{
			if (object.HasMember(name.c_str()))
			{
				const rapidjson::Value& temp = object[name.c_str()];
				if (temp.IsString())
				{
					val = temp.GetString();
					return true;
				}
				else if (temp.IsNull())
				{
					val.clear();
					return true;
				}
			}
		}
		else
		{
			if (object.IsArray() && (int)object.GetArray().Size() > idx)
			{
				const rapidjson::Value& temp = object[idx];
				if (temp.IsString())
				{
					val = temp.GetString();
					return true;
				}
				else if (temp.IsNull())
				{
					val.clear();
					return true;
				}
			}
		}
		return false;
	}
	bool internal_read_init_values(data::runtime_values_data& values, rapidjson::Value& val)
	{
		for (auto it = val.MemberBegin(); it != val.MemberEnd(); it++)
		{
			rapidjson::Value& temp = it->value;
			const char* name = it->name.GetString();
			if (temp.IsBool())
			{
				values.add_value_static(name, temp.GetBool());
			}
			else if (temp.IsInt())
			{
				values.add_value_static(name, temp.GetInt());
			}
			else if (temp.IsUint())
			{
				values.add_value_static(name, temp.GetUint());
			}
			else if (temp.IsInt64())
			{
				values.add_value_static(name, temp.GetInt64());
			}
			else if (temp.IsUint64())
			{
				values.add_value_static(name, temp.GetUint64());
			}
			else if (temp.IsDouble())
			{
				values.add_value_static(name, temp.GetDouble());
			}
			else if (temp.IsString())
			{
				values.add_value_static(name, temp.GetString());
			}
			else if (temp.IsArray())
			{
				auto len = temp.Size();
				if (len > 0)
				{
					auto& elem = temp[0];
					if (elem.IsBool())
					{
						std::vector<bool> arr;
						arr.reserve(len);
						for (decltype(len) i = 0; i < len; i++)
						{
							arr.push_back(temp[i].GetBool());
						}
						values.add_value_static(name, arr);
					}
					else if (elem.IsInt())
					{
						std::vector<int> arr;
						arr.reserve(len);
						for (decltype(len) i = 0; i < len; i++)
						{
							arr.push_back(temp[i].GetInt());
						}
						values.add_value_static(name, arr);
					}
					else if (elem.IsUint())
					{
						std::vector<unsigned int> arr;
						arr.reserve(len);
						for (decltype(len) i = 0; i < len; i++)
						{
							arr.push_back(temp[i].GetUint());
						}
						values.add_value_static(name, arr);
					}
					else if (elem.IsInt64())
					{
						std::vector<int64_t> arr;
						arr.reserve(len);
						for (decltype(len) i = 0; i < len; i++)
						{
							arr.push_back(temp[i].GetInt64());
						}
						values.add_value_static(name, arr);
					}
					else if (elem.IsUint64())
					{
						std::vector<uint64_t> arr;
						arr.reserve(len);
						for (decltype(len) i = 0; i < len; i++)
						{
							arr.push_back(temp[i].GetUint64());
						}
						values.add_value_static(name, arr);
					}
					else if (elem.IsDouble())
					{
						std::vector<double> arr;
						arr.reserve(len);
						for (decltype(len) i = 0; i < len; i++)
						{
							arr.push_back(temp[i].GetDouble());
						}
						values.add_value_static(name, arr);
					}
					else if (elem.IsString())
					{
						std::vector<string_type> arr;
						arr.reserve(len);
						for (decltype(len) i = 0; i < len; i++)
						{
							arr.emplace_back(temp[i].GetString());
						}
						values.add_value_static(name, arr);
					}
				}
				else
				{
					values.add_value(name, rx_simple_value());// add empty value to clear array
				}
			}
			else if (temp.IsObject())
			{
				auto child = values.add_child(name);
				if (!child)
					return false;
				if (!internal_read_init_values(*child, temp))
					return false;
			}
			else if (!temp.IsNull())
			{// unknown format
				return false;
			}
		}
		return true;
	}
};


// Class rx::serialization::json_reader

json_reader::json_reader (int version)
      : data_(std::make_unique<json_reader_data>())
	, base_meta_reader(version)
{
}


json_reader::~json_reader()
{
}



bool json_reader::read_id (const char* name, rx_node_id& id)
{
	string_type str;
	int index = 0;
	rapidjson::Value& val = data_->get_current_value(index);

	if (data_->safe_read_string(index, name, str, val))
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
	rapidjson::Value& val = data_->get_current_value(index);

	if (data_->safe_read_string(index, name, sstr, val))
	{
		str = sstr.c_str();
		return true;
	}
	return false;
}

bool json_reader::read_bool (const char* name, bool& val)
{
	int idx = 0;
	rapidjson::Value& object = data_->get_current_value(idx);
	if (idx<0)
	{
		if (object.HasMember(name))
		{
			rapidjson::Value& temp = object[name];
			if (temp.IsBool())
			{
				val = temp.GetBool();
				return true;
			}
		}
	}
	else
	{
		if (object.IsArray() && (int)object.GetArray().Size()>idx)
		{
			rapidjson::Value& temp = object[idx];
			if (temp.IsBool())
			{
				val = temp.GetBool();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_double (const char* name, double& val)
{
	int idx = 0;
	rapidjson::Value& object = data_->get_current_value(idx);
	if (idx<0)
	{
		if (object.HasMember(name))
		{
			rapidjson::Value& temp = object[name];
			if (temp.IsNumber())
			{
				val = temp.GetDouble();
				return true;
			}
		}
	}
	else
	{
		if (object.IsArray() && (int)object.GetArray().Size()>idx)
		{
			rapidjson::Value& temp = object[idx];
			if (temp.IsNumber())
			{
				val = temp.GetDouble();
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
	rapidjson::Value& object = data_->get_current_value(index);

	if (data_->safe_read_string(index, name, str, object))
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
	rapidjson::Value& object = data_->get_current_value(index);

	if (data_->safe_read_string(index, name, str, object))
	{
		val = rx_uuid::create_from_string(str.c_str());
		return true;
	}
	return false;
}

bool json_reader::read_int (const char* name, int& val)
{
	int idx = 0;
	rapidjson::Value& object = data_->get_current_value(idx);
	if (idx<0)
	{
		if (object.HasMember(name))
		{
			rapidjson::Value& temp = object[name];
			if (temp.IsInt())
			{
				val = temp.GetInt();
				return true;
			}
		}
	}
	else
	{
		if (object.IsArray() && (int)object.GetArray().Size()>idx)
		{
			rapidjson::Value& temp = object[idx];
			if (temp.IsInt())
			{
				val = temp.GetInt();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_uint (const char* name, uint32_t& val)
{
	int idx = 0;
	rapidjson::Value& object = data_->get_current_value(idx);
	if (idx<0)
	{
		if (object.HasMember(name))
		{
			rapidjson::Value& temp = object[name];
			if (temp.IsUint())
			{
				val = temp.GetUint();
				return true;
			}
		}
	}
	else
	{
		if (object.IsArray() && (int)object.GetArray().Size()>idx)
		{
			rapidjson::Value& temp = object[idx];
			if (temp.IsUint())
			{
				val = temp.GetUint();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::start_array (const char* name)
{
	int index = -1;
	rapidjson::Value& current = data_->get_current_value(index);
	if (index<0)
	{
		RX_ASSERT(!current.IsArray());
		if (current.HasMember(name))
		{
			rapidjson::Value& val = current[name];
			if (val.IsArray())
			{
				json_read_stack_data new_data(val);
				new_data.index = 0;
				new_data.name = name;
				data_->stack.push_back(new_data);
				return true;
			}
			else if (val.IsNull())
			{
				json_read_stack_data new_data(val);
				new_data.index = 0;
				new_data.name = name;
				data_->stack.push_back(new_data);
				return true;// empty array
			}
		}
	}
	else
	{// array stuff
		RX_ASSERT(current.IsArray());
		RX_ASSERT((int)current.GetArray().Size()>index);
		rapidjson::Value& val = current[index];
		if (val.IsArray())
		{
			json_read_stack_data new_data(val);
			new_data.index = 0;
			new_data.name = name;
			data_->stack.push_back(new_data);
			return true;
		}
		else if (val.IsNull())
		{
			json_read_stack_data new_data(val);
			new_data.index = 0;
			new_data.name = name;
			data_->stack.push_back(new_data);
			return true;// empty array
		}
	}
	errors_.emplace_back("Error reading array "s + name);
	return false;
}

bool json_reader::array_end ()
{
	if (data_->stack.empty())
	{
		RX_ASSERT(false);
		return true;
	}

	json_read_stack_data& data = *data_->stack.rbegin();
	if (data.value.IsNull())
	{
		RX_ASSERT(data.index == 0);
		data_->stack.pop_back();
		return true;
	}
	if (data.index >= 0 && data.value.IsArray() && data.index<(int)data.value.GetArray().Size())
		return false;

	RX_ASSERT(data.index >= 0);
	data_->stack.pop_back();

	return true;
}

bool json_reader::read_header (int& type)
{
	uint32_t version = 0;
	if (read_version("sversion", version))
	{
		this->set_version(version);
		if (data_->envelope.HasMember("object"))
		{
			rapidjson::Value& temp = data_->envelope["object"];
			if (temp.IsObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				temps.name = "object";
				data_->stack.push_back(temps);
				type = STREAMING_TYPE_OBJECT;
				return true;
			}
		}
		else if (data_->envelope.HasMember("type"))
		{
			rapidjson::Value& temp = data_->envelope["type"];
			if (temp.IsObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				temps.name = "type";
				data_->stack.push_back(temps);
				type = STREAMING_TYPE_TYPE;
				return true;
			}
		}
		if (data_->envelope.HasMember("body"))
		{
			rapidjson::Value& temp = data_->envelope["body"];
			if (temp.IsObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				temps.name = "body";
				data_->stack.push_back(temps);
				type = STREAMING_TYPE_MESSAGE;
				return true;
			}
		}
		else if (data_->envelope.HasMember("check_out"))
		{
			rapidjson::Value& temp = data_->envelope["check_out"];
			if (temp.IsObject())
			{
				json_read_stack_data temps(temp);
				temps.index = -1;
				temps.name = "check_out";
				data_->stack.push_back(temps);
				type = STREAMING_TYPE_CHECKOUT;
				return true;
			}
		}
		else if (data_->envelope.HasMember("objects"))
		{
			rapidjson::Value& temp = data_->envelope["objects"];
			if (temp.IsArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				temps.name = "objects";
				data_->stack.push_back(temps);
				type = STREAMING_TYPE_OBJECTS;
				return true;
			}
		}

		else if (data_->envelope.HasMember("types"))
		{
			rapidjson::Value& temp = data_->envelope["types"];
			if (temp.IsArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				temps.name = "types";
				data_->stack.push_back(temps);
				type = STREAMING_TYPE_TYPES;
				return true;
			}
		}
		else if (data_->envelope.HasMember("details"))
		{
			rapidjson::Value& temp = data_->envelope["details"];
			if (temp.IsArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				temps.name = "details";
				data_->stack.push_back(temps);
				type = STREAMING_TYPE_DETAILS;
				return true;
			}
		}
		else if (data_->envelope.HasMember("values"))
		{
			rapidjson::Value& temp = data_->envelope["values"];
			if (temp.IsArray())
			{
				json_read_stack_data temps(temp);
				temps.index = 0;
				temps.name = "values";
				data_->stack.push_back(temps);
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
	rapidjson::Value& current = data_->get_current_value(index);
	if (index<0)
	{
		if (current.HasMember(name))
		{
			rapidjson::Value& val = current[name];
			if (val.IsObject())
			{
				json_read_stack_data new_data(val);
				new_data.index = -1;
				new_data.name = name;
				data_->stack.push_back(new_data);
				return true;
			}
			else if (val.IsNull())
			{
				json_read_stack_data new_data(val);
				new_data.index = -1;
				new_data.name = name;
				data_->stack.push_back(new_data);
				return true;
			}
		}
	}
	else
	{
		RX_ASSERT(current.IsNull() || current.IsArray());
		RX_ASSERT(current.IsNull() || (int)current.GetArray().Size()>index);
		rapidjson::Value& val = current[index];
		if (val.IsObject())
		{
			json_read_stack_data new_data(val);
			new_data.index = -1;
			new_data.name = name;
			data_->stack.push_back(new_data);
			return true;
		}
		else if (val.IsNull())
		{
			json_read_stack_data new_data(val);
			new_data.index = -1;
			new_data.name = name;
			data_->stack.push_back(new_data);
			return true;
		}
	}
	errors_.emplace_back("Error reading object "s + name);
	return false;
}

bool json_reader::end_object ()
{
	if (data_->stack.empty())
		return false;

	json_read_stack_data data = *data_->stack.rbegin();
	if (data.index >= 0)
		return false;

	data_->stack.pop_back();

	return true;
}

bool json_reader::read_byte (const char* name, uint8_t& val)
{
	int idx = 0;
	rapidjson::Value& object = data_->get_current_value(idx);
	if (idx<0)
	{
		if (object.HasMember(name))
		{
			rapidjson::Value& temp = object[name];
			if (temp.IsUint())
			{
				val = (uint8_t)temp.GetUint();
				return true;
			}
		}
	}
	else
	{
		if (object.IsArray() && (int)object.GetArray().Size()>idx)
		{
			rapidjson::Value& temp = object[idx];
			if (temp.IsUint())
			{
				val = (uint8_t)temp.GetUint();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_value (const char* name, rx_value& val)
{
	return val.deserialize(name, *this);
}

bool json_reader::read_int64 (const char* name, int64_t& val)
{
	int idx = 0;
	rapidjson::Value& object = data_->get_current_value(idx);
	if (idx<0)
	{
		if (object.HasMember(name))
		{
			rapidjson::Value& temp = object[name];
			if (temp.IsInt64())
			{
				val = temp.GetInt64();
				return true;
			}
		}
	}
	else
	{
		if (object.IsArray() && (int)object.GetArray().Size()>idx)
		{
			rapidjson::Value& temp = object[idx];
			if (temp.IsInt64())
			{
				val = temp.GetInt64();
				return true;
			}
		}
	}
	return false;
}

bool json_reader::read_uint64 (const string_type& name, uint64_t& val)
{
	int idx = 0;
	rapidjson::Value& object = data_->get_current_value(idx);
	if (idx<0)
	{
		if (object.HasMember(name.c_str()))
		{
			rapidjson::Value& temp = object[name.c_str()];
			if (temp.IsUint64())
			{
				val = temp.GetUint64();
				return true;
			}
		}
	}
	else
	{
		if (object.IsArray() && (int)object.GetArray().Size()>idx)
		{
			rapidjson::Value& temp = object[idx];
			if (temp.IsUint64())
			{
				val = temp.GetUint64();
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
	rapidjson::Value& jval = data_->get_current_value(index);

	if (data_->safe_read_string(index, name, sstr, jval))
	{
		/*string_type temp_val = base64_decode(sstr);
		val.assign(temp_val.size(), 0);
		temp_val.copy((char*)&val[0], val.size());*/
		val = urke::get_data(sstr);
		return true;
	}
	return false;
}

bool json_reader::parse_data (const string_type& data)
{
	bool ret = false;
	errors_.clear();
	data_->envelope.Parse(data.c_str());
	if (!data_->envelope.HasParseError()) {
		ret = true;
	}
	else
	{
		auto err = data_->envelope.GetParseError();
		auto offset = data_->envelope.GetErrorOffset();

		errors_.emplace_back(get_parser_error(err, offset, data));
	}

	return ret;
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
	if (name)
	{
		if (!start_object(name))
			return false;
	}

	// now enumerate objects
	int index = 0;
	rapidjson::Value& val = data_->get_current_value(index);

	if (val.IsObject())
	{
		if (!data_->internal_read_init_values(values, val))
			return false;
	}
	else if(!val.IsNull())
	{
		return false;
	}

	if (name)
	{
		if (!end_object())
			return false;
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
	rapidjson::Value& val = data_->get_current_value(index);

	if (val.IsNull())
	{
		ref = rx_item_reference::null_ref;
		if (!end_object())
			return false;
		return true;
	}

	if (!val.IsObject())
		return false;

	if(val.ObjectEmpty())
	{
		ref = rx_item_reference::null_ref;
		if (!end_object())
			return false;
		return true;
	}

	if (val.HasMember("id"))
	{
		rx_node_id temp_id;
		if (!read_id("id", temp_id))
			return false;
		ref = std::move(temp_id);
	}
	else if (val.HasMember("path"))
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

bool json_reader::read_value_type (const char* name, rx_value_t& val)
{
	uint8_t temp;
	auto ret = read_byte(name, temp);
	if (!ret)
		return false;
	val = temp;
	return true;
}

string_type json_reader::get_error () const
{
	std::ostringstream ss;
	bool first = true;
	if (errors_.empty())
	{
		ss << "JSON stream error:";
		for (const auto& one : errors_)
		{
			if (first)
				first = false;
			else
                ss << ";";
            ss << one;
		}
		ss << " ";
		first = true;
	}
	ss << "[";
	if(!data_->stack.empty())
	{
		for (auto& one : data_->stack)
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

struct json_write_stack_data
{
public:
	bool is_array;
	std::string name;
};
struct json_writer_data
{
	std::vector<json_write_stack_data> stack;
	json_writer_data() : writer(stream) {}
	rapidjson::StringBuffer stream;
	rapidjson::Writer<rapidjson::StringBuffer> writer;
};
struct json_pretty_writer_data
{
	std::vector<json_write_stack_data> stack;
	json_pretty_writer_data() : writer(stream) {}
	rapidjson::StringBuffer stream;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer;
};
// Parameterized Class rx::serialization::json_writer_type

template <class writerT>
json_writer_type<writerT>::json_writer_type (int version)
      : data_(std::make_unique<writerT>())
	, base_meta_writer(version)
	, type_(0)
{
}


template <class writerT>
json_writer_type<writerT>::~json_writer_type()
{
}



template <class writerT>
bool json_writer_type<writerT>::write_id (const char* name, const rx_node_id& id)
{
	string_type buff;
	id.to_string(buff);

	if (!is_current_array())
		data_->writer.Key(name);
	if (buff.empty())
		data_->writer.String("");
	else
		data_->writer.String(buff.c_str());

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_string (const char* name, const char* str)
{

	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.String(str ? str : "");

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_bool (const char* name, bool val)
{

	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.Bool(val);

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_double (const char* name, double val)
{

	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.Double(val);

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_time (const char* name, const rx_time_struct_t& val)
{
	rx_time tval(val);
	string_type str = tval.get_IEC_string();


	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.String(str.c_str());

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_uuid (const char* name, const rx_uuid_t& val)
{
	string_type buff;
	rx_uuid vval(val);
	vval.to_string(buff);


	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.String(buff.c_str());

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_int (const char* name, int val)
{

	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.Int(val);

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_uint (const char* name, uint32_t val)
{

	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.Uint(val);

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::start_array (const char* name, size_t size)
{
	json_write_stack_data data;
	data.is_array = true;
	data.name = name;

	bool is_array = is_current_array();

	data_->stack.push_back(data);

	if(!is_array)
		data_->writer.Key(name);
	data_->writer.StartArray();

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::end_array ()
{
	if (data_->stack.empty())
		return false;

	json_write_stack_data& data = *data_->stack.rbegin();
	if (!data.is_array)
		return false;

	data_->stack.pop_back();
	data_->writer.EndArray();

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_header (int type, size_t size)
{
	data_->writer.StartObject();
	type_ = type;
	if (type_ == STREAMING_TYPE_MESSAGE)
	{
		json_write_stack_data data;
		data.is_array = false;
		data.name = "msg";
		data_->stack.push_back(data);
		return true;
	}

	string_type ver;
	if (!get_version_string(ver, (uint32_t)get_version()))
		return false;

	data_->writer.Key("sversion");
	data_->writer.String(ver.c_str());


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
		RX_ASSERT(false);
		return false;
	}

	data_->stack.push_back(data);

	data_->writer.Key(data.name.c_str());
	if (data.is_array)
	{
		data_->writer.StartArray();
	}
	else
	{
		data_->writer.StartObject();
	}

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_footer ()
{
	if (type_ == STREAMING_TYPE_MESSAGE)
	{
		data_->writer.EndObject();
		return true;
	}
	else
	{
		if (data_->stack.empty())
			return false;

		json_write_stack_data& data = *data_->stack.rbegin();
		if (data.is_array)
		{
			data_->writer.EndArray();
		}
		else
		{
			data_->writer.EndObject();
		}
		data_->writer.EndObject();

		return true;
	}
}

template <class writerT>
bool json_writer_type<writerT>::start_object (const char* name)
{
	json_write_stack_data data;
	data.is_array = false;
	data.name = name;

	bool is_array = is_current_array();

	data_->stack.push_back(data);

	if(!is_array)
		data_->writer.Key(name);
	data_->writer.StartObject();

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::end_object ()
{
	if (data_->stack.empty())
		return false;

	json_write_stack_data& data = *data_->stack.rbegin();
	if (data.is_array)
		return false;

	data_->stack.pop_back();
	data_->writer.EndObject();

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_byte (const char* name, uint8_t val)
{

	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.Uint(val);

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_value (const char* name, const rx_value& val)
{
	if (!val.serialize(name, *this))
		return false;
	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_int64 (const char* name, int64_t val)
{

	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.Int64(val);

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_uint64 (const char* name, uint64_t val)
{

	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.Uint64(val);

	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_bytes (const char* name, const std::byte* val, size_t size)
{
	//string_type temp = base64_encode(val, size);
	string_type temp = urke::get_base64(val, size);

	return write_string(name, temp.c_str());
}

template <class writerT>
bool json_writer_type<writerT>::is_current_array ()
{
	if (data_->stack.empty())
	{
		if(type_!= STREAMING_TYPE_MESSAGE)
			RX_ASSERT(false);
		return false;
	}
	else
	{
		json_write_stack_data& current = *data_->stack.rbegin();
		return current.is_array;
	}
}

template <class writerT>
string_type json_writer_type<writerT>::get_string ()
{
	return data_->stream.GetString();
}

template <class writerT>
bool json_writer_type<writerT>::write_version (const char* name, uint32_t val)
{
	string_type str;
	if (!get_version_string(str, val))
		return false;
	return write_string(name, str.c_str());
}

template <class writerT>
bool json_writer_type<writerT>::get_version_string (string_type& result, uint32_t version)
{
	int major = (version >> 16);
	int minor = (version & 0xffff);
	char buff[0x20];
	sprintf(buff, "%d.%d", major, minor);
	result = buff;
	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_init_values (const char* name, const data::runtime_values_data& values)
{
	if (name)
	{
		if (!start_object(name))
			return false;
	}
	for (const auto& one : values.children)
	{
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
	for (const auto& one : values.values)
	{
		if (std::holds_alternative<rx_simple_value>(one.second))
		{
			if (!std::get<rx_simple_value>(one.second).weak_serialize(one.first.c_str(), *this))
				return false;
		}
		else
		{
			auto& vals = std::get<std::vector<rx_simple_value> >(one.second);
			if (!start_array(one.first.c_str(), vals.size()))
				return false;
			for (size_t i = 0; i < vals.size(); i++)
			{
				if (!vals[i].weak_serialize(one.first.c_str(), *this))
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

template <class writerT>
bool json_writer_type<writerT>::is_string_based () const
{
  return true;

}

template <class writerT>
bool json_writer_type<writerT>::write_item_reference (const char* name, const rx_item_reference& ref)
{
	if (!ref.is_null())
	{
		if (!start_object(name))
			return false;

		if (ref.is_node_id())
		{
			if (!write_id("id", ref.get_node_id()))
				return false;
		}
		else
		{
			if (!write_string("path", ref.get_path().c_str()))
				return false;
		}
		if (!end_object())
			return false;
	}
	else
	{
		write_null(name);
	}
	return true;
}

template <class writerT>
bool json_writer_type<writerT>::write_value_type (const char* name, rx_value_t val)
{
	return write_byte(name, val);
}

template <class writerT>
bool json_writer_type<writerT>::write_null (const char* name)
{
	if (!is_current_array())
		data_->writer.Key(name);
	data_->writer.Null();
	return true;
}

template <class writerT>
string_type json_writer_type<writerT>::get_error () const
{
	std::ostringstream ss;
	bool first = true;
	ss << "[";
	if (!data_->stack.empty())
	{
		for (auto& one : data_->stack)
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


} // namespace serialization
} // namespace rx

namespace rx {
namespace serialization {
template class json_writer_type<json_writer_data>;
template class json_writer_type<json_pretty_writer_data>;
} // serialization
} // rx_platform
