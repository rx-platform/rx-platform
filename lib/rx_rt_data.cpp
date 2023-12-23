

/****************************************************************************
*
*  lib\rx_rt_data.cpp
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


#include "pch.h"

#include "rx_lib.h"

// rx_rt_data
#include "lib/rx_rt_data.h"



namespace rx {

namespace data {

// Class rx::data::runtime_values_data 


void runtime_values_data::add_value (const string_type& name, const rx_simple_value& value)
{
	if (name.empty())
		return;
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		values.insert(std::make_pair(name, value));
	}
	else
	{
		string_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(my_path);
		if (child_it != children.end())
		{
			if (array_idx < 0 && std::holds_alternative<child_type>(child_it->second))
			{
				std::get<child_type>(child_it->second).add_value(name.substr(idx + 1), value);
			}
			else if (array_idx >= 0 && std::holds_alternative<array_child_type>(child_it->second)
				&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
			{
				std::get<array_child_type>(child_it->second)[array_idx].add_value(name.substr(idx + 1), value);
			}
		}
		else
		{
			auto vals = add_child(name.substr(0, idx));
			if(vals)
				vals->add_value(name.substr(idx + 1), value);
		}
	}
}

void runtime_values_data::add_value (const string_type& name, rx_simple_value&& value)
{
	if (name.empty())
		return;
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		values.insert(std::make_pair(name, std::move(value)));
	}
	else
	{
		string_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(my_path);
		if (child_it != children.end())
		{
			if (array_idx < 0 && std::holds_alternative<child_type>(child_it->second))
			{
				std::get<child_type>(child_it->second).add_value(name.substr(idx + 1), std::move(value));
			}
			else if (array_idx >= 0 && std::holds_alternative<array_child_type>(child_it->second)
				&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
			{
				std::get<array_child_type>(child_it->second)[array_idx].add_value(name.substr(idx + 1), std::move(value));
			}
		}
		else
		{
			auto vals = add_child(name.substr(0, idx));
			if(vals)
				vals->add_value(name.substr(idx + 1), std::move(value));
		}
	}
}

void runtime_values_data::add_value (const string_type& name, std::vector<rx_simple_value> value)
{
	if (name.empty())
		return;
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		values.insert(std::make_pair(name, std::move(value)));
	}
	else
	{
		string_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(my_path);
		if (child_it != children.end())
		{
			if (array_idx < 0 && std::holds_alternative<child_type>(child_it->second))
			{
				std::get<child_type>(child_it->second).add_value(name.substr(idx + 1), std::move(value));
			}
			else if (array_idx >= 0 && std::holds_alternative<array_child_type>(child_it->second)
				&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
			{
				std::get<array_child_type>(child_it->second)[array_idx].add_value(name.substr(idx + 1), std::move(value));
			}
		}
		else
		{
			auto vals = add_child(name.substr(0, idx));
			if(vals)
				vals->add_value(name.substr(idx + 1), std::move(value));
		}
	}
}

runtime_values_data* runtime_values_data::add_child (const string_type& name)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		auto child_it = children.find(string_type(name));
		if (child_it != children.end())
		{
			child_it->second = runtime_values_data();
			return &std::get<runtime_values_data>(child_it->second);
		}
		else
		{
			return &std::get<runtime_values_data>(children.emplace(name, runtime_values_data()).first->second);
		}
	}
	else
	{
		string_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(my_path);
		if (child_it != children.end())
		{
			if (array_idx < 0 && std::holds_alternative<child_type>(child_it->second))
			{
				return std::get<child_type>(child_it->second).add_child(name.substr(idx + 1));
			}
			else if (array_idx >= 0 && std::holds_alternative<array_child_type>(child_it->second)
				&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
			{
				return std::get<array_child_type>(child_it->second)[array_idx].add_child(name.substr(idx + 1));
			}
		}
		else
		{
			if (array_idx < 0)
			{
				auto vals = add_child(string_type(my_path));
				if (vals)
					return vals->add_child(name.substr(idx + 1));
			}
			else
			{
				auto vals = add_array_child(string_type(my_path), idx + 1);
				if (vals)
					return vals->at(array_idx).add_child(name.substr(idx + 1));
			}
		}
	}
	return nullptr;
}

void runtime_values_data::add_child (const string_type& name, runtime_values_data&& data)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		auto child_it = children.find(string_type(name));
		if (child_it != children.end())
		{
			child_it->second = std::move(data);
		}
		else
		{
			children.emplace(name, std::move(data));
		}
	}
	else
	{
		string_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(my_path);
		if (child_it != children.end())
		{
			if (array_idx < 0 && std::holds_alternative<child_type>(child_it->second))
			{
				std::get<child_type>(child_it->second).add_child(name.substr(idx + 1), std::move(data));
			}
			else if (array_idx >= 0 && std::holds_alternative<array_child_type>(child_it->second)
				&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
			{
				std::get<array_child_type>(child_it->second)[array_idx].add_child(name.substr(idx + 1), std::move(data));
			}
		}
		else
		{
			if (array_idx < 0)
			{
				auto vals = add_child(string_type(my_path));
				if(vals)
					vals->add_child(name.substr(idx + 1), std::move(data));
			}
			else
			{
				auto vals = add_array_child(string_type(my_path), idx + 1);
				if (vals)
					vals->at(array_idx).add_child(name.substr(idx + 1), std::move(data));
			}
		}
	}
}

std::vector<runtime_values_data>* runtime_values_data::add_array_child (const string_type& name, size_t size)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		auto child_it = children.find(string_type(name));
		if (child_it != children.end())
		{
			child_it->second = array_child_type(size);
			return &std::get<array_child_type>(child_it->second);
		}
		else
		{
			return &std::get<array_child_type>(children.emplace(name, array_child_type(size)).first->second);
		}
	}
	else
	{
		string_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(my_path);
		if (child_it != children.end())
		{
			if (array_idx < 0 && std::holds_alternative<child_type>(child_it->second))
			{
				return std::get<child_type>(child_it->second).add_array_child(name.substr(idx + 1), size);
			}
			else if (array_idx >= 0 && std::holds_alternative<array_child_type>(child_it->second)
				&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
			{
				return std::get<array_child_type>(child_it->second)[array_idx].add_array_child(name.substr(idx + 1), size);
			}
		}
		else
		{
			if (array_idx < 0)
			{
				auto vals = add_child(string_type(my_path));
				if (vals)
					return vals->add_array_child(name.substr(idx + 1), size);
			}
			else
			{
				auto vals = add_array_child(string_type(my_path), idx + 1);
				if (vals)
					return vals->at(array_idx).add_array_child(name.substr(idx + 1), size);
			}
		}
	}
	return nullptr;
}

void runtime_values_data::add_array_child (const string_type& name, std::vector<runtime_values_data> data)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		auto child_it = children.find(string_type(name));
		if (child_it != children.end())
		{
			child_it->second = array_child_type(std::move(data));
		}
		else
		{
			children.emplace(name, array_child_type(std::move(data)));
		}
	}
	else
	{
		string_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(my_path);
		if (child_it != children.end())
		{
			if (array_idx < 0 && std::holds_alternative<child_type>(child_it->second))
			{
				std::get<child_type>(child_it->second).add_array_child(name.substr(idx + 1), std::move(data));
			}
			else if (array_idx >= 0 && std::holds_alternative<array_child_type>(child_it->second)
				&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
			{
				std::get<array_child_type>(child_it->second)[array_idx].add_array_child(name.substr(idx + 1), std::move(data));
			}
		}
		else
		{
			if (array_idx < 0)
			{
				auto vals = add_child(string_type(my_path));
				if (vals)
					vals->add_array_child(name.substr(idx + 1), std::move(data));
			}
			else
			{
				auto vals = add_array_child(string_type(my_path), idx + 1);
				if (vals)
					vals->at(array_idx).add_array_child(name.substr(idx + 1), std::move(data));
			}
		}
	}
}

rx_simple_value runtime_values_data::get_value (const string_type& path) const
{
	if (path.empty())
		return rx_simple_value();
	auto idx = path.find(".");

	if (idx == string_type::npos)
	{// our value
		int array_idx = -1;
		string_view_type basic_path = extract_index(path, array_idx);
		if (!basic_path.empty())
		{
			auto val_it = values.find(string_type(basic_path));
			if (val_it != values.end())
			{
				if (array_idx < 0 && std::holds_alternative<value_type>(val_it->second))
					return std::get<value_type>(val_it->second);
				else if ((int)array_idx >= 0 && std::holds_alternative<array_value_type>(val_it->second)
					&& array_idx < (int)std::get<array_value_type>(val_it->second).size())
					return std::get<array_value_type>(val_it->second)[idx];

			}
		}
	}
	else
	{
		int array_idx = -1;
		string_type mine = path.substr(0, idx);
		string_view_type basic_path = extract_index(mine, array_idx);
		if (!basic_path.empty())
		{
			auto child_it = children.find(string_type(basic_path));
			if (child_it != children.end())
			{
				if (array_idx < 0 && std::holds_alternative<child_type>(child_it->second))
				{
					return std::get<child_type>(child_it->second).get_value(path.substr(idx + 1));
				}
				else if (array_idx >= 0 && std::holds_alternative<array_child_type>(child_it->second)
					&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
				{
					return std::get<array_child_type>(child_it->second)[array_idx].get_value(path.substr(idx + 1));
				}
			}
		}
	}
	return rx_simple_value();
}

bool runtime_values_data::get_array_value (const string_type& path, std::vector<rx_simple_value>& val) const
{
	if (path.empty())
		return false;
	auto idx = path.find(".");

	if (idx == string_type::npos)
	{// our value
		int array_idx = -1;
		string_view_type basic_path = extract_index(path, array_idx);
		if (!basic_path.empty())
		{
			auto val_it = values.find(string_type(basic_path));
			if (val_it != values.end())
			{
				if (array_idx < 0 && std::holds_alternative<array_value_type>(val_it->second))
				{
					val = std::get<array_value_type>(val_it->second);
					return true;
				}
				else if(array_idx<0)
				{
					auto temp = std::get<value_type>(val_it->second);
					if (temp.is_array())
					{
						size_t count = temp.array_size();
						val.reserve(count);
						for (size_t i = 0; i < count; i++)
						{
							val.push_back(temp[(int)i]);
						}
						return true;
					}
				}
			}
		}
	}
	else
	{
		int array_idx = -1;
		string_type mine = path.substr(0, idx);
		string_view_type basic_path = extract_index(mine, array_idx);
		if (!basic_path.empty())
		{
			auto child_it = children.find(string_type(basic_path));
			if (child_it != children.end())
			{
				if (array_idx < 0 && std::holds_alternative<child_type>(child_it->second))
				{
					return std::get<child_type>(child_it->second).get_array_value(path.substr(idx + 1), val);
				}
				else if (array_idx >= 0 && std::holds_alternative<array_child_type>(child_it->second)
					&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
				{
					return std::get<array_child_type>(child_it->second)[array_idx].get_array_value(path.substr(idx + 1), val);
				}
			}
		}
	}
	return false;
}

bool runtime_values_data::empty () const
{
	return children.empty() && values.empty();
}

void runtime_values_data::clear ()
{
	children.clear();
	values.clear();
}

string_view_type runtime_values_data::extract_index (string_view_type name, int& idx) const
{
	string_view_type ret;
	if (name.empty())
		return string_view_type();
	if (*name.rbegin() == ']')
	{
		size_t str_idx = name.rfind('[');
		if (str_idx != string_type::npos && str_idx < name.size() - 2)
		{
			uint16_t unsigned_idx;
			string_view_type number = name.substr(str_idx + 1, name.size() - str_idx /*We know this because of the upper condition*/ - 2);
			auto  result = std::from_chars(number.data(), number.data() + number.size(), unsigned_idx);
			if (result.ec != std::errc())
				return ret;//error
			idx = unsigned_idx;
			ret = name.substr(0, str_idx);
		}
		else
		{
			return string_view_type();
		}
	}
	else
	{
		idx = -1;
		ret = name;
	}
	return ret;
}


// Class rx::data::runtime_data_model 


byte_string runtime_data_model::serialize () const
{
	return byte_string();
}

rx::data::runtime_data_model runtime_data_model::deserialize (byte_string& data)
{
	return runtime_data_model();
}


// Class rx::data::runtime_model_element 


bool runtime_model_element::is_value () const
{
	return std::holds_alternative<rx_simple_value>(value);
}

bool runtime_model_element::is_complex () const
{
	return std::holds_alternative<runtime_data_model>(value);
}

bool runtime_model_element::is_complex_array () const
{
	return std::holds_alternative<std::vector<runtime_data_model> >(value);
}

const values::rx_simple_value& runtime_model_element::get_value () const
{
	static rx_simple_value g_empty;
	if (is_value())
		return std::get<rx_simple_value>(value);
	else
		return g_empty;
}

const runtime_data_model& runtime_model_element::get_complex () const
{
	static runtime_data_model g_empty;
	if (is_complex())
		return std::get<runtime_data_model>(value);
	else
		return g_empty;
}

const std::vector<runtime_data_model>& runtime_model_element::get_complex_array () const
{
	static std::vector<runtime_data_model> g_empty;
	if (is_value())
		return std::get<std::vector<runtime_data_model>>(value);
	else
		return g_empty;
}


} // namespace data
} // namespace rx

