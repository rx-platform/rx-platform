

/****************************************************************************
*
*  lib\rx_rt_data.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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

#include "lib/rx_ser_bin.h"


namespace rx {

namespace data {

// Class rx::data::runtime_values_data 


void runtime_values_data::add_value (string_view_type name, const rx_simple_value& value)
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
		string_view_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(static_cast<string_type>(my_path));
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

void runtime_values_data::add_value (string_view_type name, rx_simple_value&& value)
{
	if (name.empty())
		return;
	auto idx = name.find(".");

	if (idx == string_view_type::npos)
	{// our value
		values.insert(std::make_pair(name, std::move(value)));
	}
	else
	{
		string_view_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(static_cast<string_type>(my_path));
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

void runtime_values_data::add_value (string_view_type name, std::vector<rx_simple_value> value)
{
	if (name.empty())
		return;
	auto idx = name.find(".");

	if (idx == string_view_type::npos)
	{// our value
		values.insert(std::make_pair(name, std::move(value)));
	}
	else
	{
		string_view_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(static_cast<string_type>(my_path));
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

runtime_values_data* runtime_values_data::add_child (string_view_type name)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		auto child_it = children.find(static_cast<string_type>(name));
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
		string_view_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(static_cast<string_type>(my_path));
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

void runtime_values_data::add_child (string_view_type name, runtime_values_data&& data)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		auto child_it = children.find(static_cast<string_type>(name));
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
		string_view_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(static_cast<string_type>(my_path));
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

std::vector<runtime_values_data>* runtime_values_data::add_array_child (string_view_type name, size_t size)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		auto child_it = children.find(static_cast<string_type>(name));
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
		string_view_type my_path = name.substr(0, idx);
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(static_cast<string_type>(my_path));
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
				auto vals = add_child(static_cast<string_type>(my_path));
				if (vals)
					return vals->add_array_child(name.substr(idx + 1), size);
			}
			else
			{
				auto vals = add_array_child(static_cast<string_type>(my_path), idx + 1);
				if (vals)
					return vals->at(array_idx).add_array_child(name.substr(idx + 1), size);
			}
		}
	}
	return nullptr;
}

void runtime_values_data::add_array_child (string_view_type name, std::vector<runtime_values_data> data)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		auto child_it = children.find(static_cast<string_type>(name));
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
		string_view_type my_path = string_type(name.substr(0, idx));
		int array_idx = -1;
		my_path = extract_index(my_path, array_idx);
		auto child_it = children.find(static_cast<string_type>(my_path));
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
				auto vals = add_child(static_cast<string_type>(my_path));
				if (vals)
					vals->add_array_child(name.substr(idx + 1), std::move(data));
			}
			else
			{
				auto vals = add_array_child(static_cast<string_type>(my_path), idx + 1);
				if (vals)
					vals->at(array_idx).add_array_child(name.substr(idx + 1), std::move(data));
			}
		}
	}
}

rx_simple_value runtime_values_data::get_value (string_view_type path) const
{
	if (path.empty())
		return rx_simple_value();
	auto idx = path.find(".");

	if (idx == string_view_type::npos)
	{// our value
		int array_idx = -1;
		string_view_type basic_path = extract_index(path, array_idx);
		if (!basic_path.empty())
		{
			auto val_it = values.find(static_cast<string_type>(basic_path));
			if (val_it != values.end())
			{
				if (array_idx < 0 && std::holds_alternative<value_type>(val_it->second))
					return std::get<value_type>(val_it->second);
				else if ((int)array_idx >= 0 && std::holds_alternative<array_value_type>(val_it->second)
					&& array_idx < (int)std::get<array_value_type>(val_it->second).size())
					return std::get<array_value_type>(val_it->second)[idx];

			}
			else
			{
				auto child_it= children.find(static_cast<string_type>(basic_path));
				if (child_it != children.end())
				{
					if (array_idx == -1)
					{
						if (std::holds_alternative<child_type>(child_it->second))
						{
							auto& child = std::get<child_type>(child_it->second);
							rx_simple_value temp;
							if (child.get_complex_value(temp))
								return temp;
						}
						else if (std::holds_alternative<array_child_type>(child_it->second))
						{
							auto& child = std::get<array_child_type>(child_it->second);
							typed_value_type temp_val;
							if (child.empty())
							{
								rx_init_complex_array_value(&temp_val, nullptr, 0);
							}
							else
							{
								std::vector<complex_value_struct> data;
								data.reserve(child.size());
								for (const auto& one_child : child)
								{
									rx_simple_value temp;
									if (!one_child.get_complex_value(temp))
										return rx_simple_value();
									data.push_back(temp.get_complex());
								}
								rx_init_complex_array_value(&temp_val, &data[0], data.size());
							}
							return rx_simple_value(std::move(temp_val));
						}
					}
					else if(std::holds_alternative<array_child_type>(child_it->second)
						&& (size_t)array_idx < std::get<array_child_type>(child_it->second).size())
					{
						auto& child = std::get<array_child_type>(child_it->second);
						rx_simple_value temp;
						if (child[array_idx].get_complex_value(temp))
							return temp;
					}
				}
			}
		}
	}
	else
	{
		int array_idx = -1;
		string_view_type mine = path.substr(0, idx);
		string_view_type basic_path = extract_index(mine, array_idx);
		if (!basic_path.empty())
		{
			auto child_it = children.find(static_cast<string_type>(basic_path));
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

bool runtime_values_data::get_array_value (string_view_type path, std::vector<rx_simple_value>& val) const
{
	if (path.empty())
		return false;
	auto idx = path.find(".");

	if (idx == string_view_type::npos)
	{// our value
		int array_idx = -1;
		string_view_type basic_path = extract_index(path, array_idx);
		if (!basic_path.empty())
		{
			auto val_it = values.find(static_cast<string_type>(basic_path));
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
		string_view_type mine = path.substr(0, idx);
		string_view_type basic_path = extract_index(mine, array_idx);
		if (!basic_path.empty())
		{
			auto child_it = children.find(static_cast<string_type>(basic_path));
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

bool runtime_values_data::get_complex_value (rx_simple_value& val) const
{
	if (values.size() == 2 && children.size() == 0)
	{
		auto it_real = values.find("Real");
		if (it_real != values.end()
			&& std::holds_alternative<value_type>(it_real->second)
			&& std::get<value_type>(it_real->second).is_numeric())
		{
			auto it_imag = values.find("Imag");
			if (it_imag != values.end()
				&& std::holds_alternative<value_type>(it_imag->second)
				&& std::get<value_type>(it_imag->second).is_numeric())
			{
				double rval = std::get<value_type>(it_real->second).get_float();
				double ival = std::get<value_type>(it_imag->second).get_float();
				complex_value_struct str{ rval, ival };
				typed_value_type temp;
				if (RX_OK == rx_init_complex_value(&temp, str))
				{
					val = rx_simple_value(std::move(temp));
					return true;
				}
			}
		}
	}
	return false;
}


// Class rx::data::runtime_data_model 


rx_result runtime_data_model::fill_runtime_value (data::runtime_values_data& data, const values::rx_value& val) const
{
	if (val.get_type() != RX_STRUCT_TYPE)
		return "Rx invalid format!";
	size_t idx = 0;
	std::vector<size_t> indexes;
	string_type path;
	for (const auto& str : elements)
	{
		indexes.push_back(idx);
		path = str.name;

		if (str.is_value())
		{
			rx_simple_value one_val;
			if (!val.get_struct_value(one_val, indexes))
				return rx_create_string("Invalid data structure at ", path);

			if (one_val.get_type() != str.get_value().get_type())
			{
				if(!one_val.convert_to(str.get_value().get_type()))
					return rx_create_string("Invalid data type at ", path);
			}

			data.add_value(str.name, std::move(one_val));
		}
		else if (str.is_complex())
		{
			data::runtime_values_data one_val;
			string_array paths{ path };

			auto result = fill_runtime_value_recursive(one_val, str.get_complex(), val, indexes, paths);
			if (!result)
			{
				std::ostringstream ss;
				ss << result.errors_line();
				ss << ", error at ";
				bool first = true;
				for (auto one : paths)
				{
					if (first)
						first = false;
					else
						ss << '.';
					ss << one;
				}
				return rx_result(ss.str());
			}	

			data.add_child(str.name, std::move(one_val));
		}
		else // if(str.is_complex_array())
		{
			RX_ASSERT(str.is_complex_array());

			size_t sz = val.array_size(indexes);

			if(sz==rx_value::npos)
				return rx_create_string("Invalid data structure at ", path);
			std::vector<runtime_values_data> one_val;
			string_array paths;

			for (size_t idx2 = 0 ; idx2<sz; idx2++)
			{
				path = rx_create_string(str.name, "[", idx2 ,"]");

				data::runtime_values_data single_val;
				indexes.push_back(idx2);
				paths.push_back(path);
				auto result = fill_runtime_value_recursive(single_val, str.get_complex_array(), val, indexes, paths);
				if (!result)
				{
					std::ostringstream ss;
					ss << result.errors_line();
					ss << ", error at ";
					bool first = true;
					for (auto one : paths)
					{
						if (first)
							first = false;
						else
							ss << '.';
						ss << one;
					}
					return rx_result(ss.str());
				}
				indexes.pop_back();
				paths.pop_back();

				one_val.push_back(std::move(single_val));
			}
			data.add_array_child(str.name, std::move(one_val));
		}


		indexes.pop_back();
		idx++;
	}
	return true;
	
}

rx_result runtime_data_model::fill_runtime_value (data::runtime_values_data& data, const values::rx_simple_value& val) const
{
	if (val.get_type() != RX_STRUCT_TYPE)
		return "Rx invalid format!";
	size_t idx = 0;
	std::vector<size_t> indexes;
	string_type path;
	for (const auto& str : elements)
	{
		indexes.push_back(idx);
		path = str.name;

		if (str.is_value())
		{
			rx_simple_value one_val;
			if (!val.get_struct_value(one_val, indexes))
				return rx_create_string("Invalid data structure at ", path);

			if (one_val.get_type() != str.get_value().get_type())
			{
				if (!one_val.convert_to(str.get_value().get_type()))
					return rx_create_string("Invalid data type at ", path);
			}

			data.add_value(str.name, std::move(one_val));
		}
		else if (str.is_complex())
		{
			data::runtime_values_data one_val;
			string_array paths{ path };

			auto result = fill_runtime_value_recursive(one_val, str.get_complex(), val, indexes, paths);
			if (!result)
				return result;

			data.add_child(str.name, std::move(one_val));
		}
		else // if(str.is_complex_array())
		{
			RX_ASSERT(str.is_complex_array());

			size_t sz = val.array_size(indexes);

			if (sz == rx_value::npos)
				return rx_create_string("Invalid data structure at ", path);
			std::vector<runtime_values_data> one_val;
			string_array paths;

			for (size_t idx2 = 0; idx2 < sz; idx2++)
			{
				path = rx_create_string(str.name, "[", idx2, "]");

				data::runtime_values_data single_val;
				indexes.push_back(idx2);
				paths.push_back(path);

				auto result = fill_runtime_value_recursive(single_val, str.get_complex_array(), val, indexes, paths);
				if (!result)
					return result;
				indexes.pop_back();
				paths.pop_back();

				one_val.push_back(std::move(single_val));
			}
			data.add_array_child(str.name, std::move(one_val));
		}


		indexes.pop_back();
		idx++;
	}
	return true;
}

rx_result runtime_data_model::fill_runtime_value_recursive (data::runtime_values_data& data, const data::runtime_data_model& model, const values::rx_value& val, std::vector<size_t>& indexes, string_array& paths) const
{
	size_t idx = 0;
	string_type path;
	for (const auto& str : model.elements)
	{
//		RX_ASSERT(str.name != "Temperature");
		indexes.push_back(idx);
		path = str.name;
		paths.push_back(path);

		if (str.is_value())
		{
			rx_simple_value one_val;
			if (!val.get_struct_value(one_val, indexes))
				return "Invalid data structure";

			if (one_val.get_type() != str.get_value().get_type())
				return "Invalid data type";

			data.add_value(str.name, std::move(one_val));
		}
		else if (str.is_complex())
		{
			data::runtime_values_data one_val;

			auto result = fill_runtime_value_recursive(one_val, str.get_complex(), val, indexes, paths);
			if (!result)
				return result;

			data.add_child(str.name, std::move(one_val));

		}
		else // if(str.is_complex_array())
		{
			RX_ASSERT(str.is_complex_array());


			size_t sz = val.array_size(indexes);

			if (sz == rx_value::npos)
				return rx_create_string("Invalid data structure at ", path);
			
			std::vector<runtime_values_data> one_val;
			for (size_t idx2 = 0; idx2 < sz; idx2++)
			{
				// strange but works perfectly
				paths.pop_back();
				path = rx_create_string(str.name, "[", idx2, "]");
				paths.push_back(path);
				data::runtime_values_data single_val;
				indexes.push_back(idx2);
				auto result = fill_runtime_value_recursive(single_val, str.get_complex_array(), val, indexes, paths);
				if (!result)
					return result;
				indexes.pop_back();
				paths.pop_back();
				paths.push_back(str.name);

				one_val.push_back(std::move(single_val));
			}
			data.add_array_child(str.name, std::move(one_val));
		}


		indexes.pop_back();
		paths.pop_back();
		idx++;
	}
	return true;
}

rx_result runtime_data_model::fill_runtime_value_recursive (data::runtime_values_data& data, const data::runtime_data_model& model, const values::rx_simple_value& val, std::vector<size_t>& indexes, string_array& paths) const
{
	return RX_NOT_IMPLEMENTED;
}

rx_result runtime_data_model::fill_simple_value (values::rx_simple_value& out_val, const data::runtime_values_data& in_data) const
{
	std::vector<rx_simple_value> vals;
	for (const auto& one : elements)
	{
		if (one.is_value())
		{
			rx_simple_value val = in_data.get_value(one.name);
			if (val.is_null())
				return rx_create_string("Element [", one.name, "] is missing!");

			vals.push_back(std::move(val));
		}
	}
	out_val.assign_static(std::move(vals));
	return true;
}

runtime_data_model::runtime_data_model(const runtime_data_model& right)
{
	if (!right.elements.empty())
	{
		size_t count = right.elements.size();
		elements.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			elements.push_back(right.elements[i]);
		}
	}
}
runtime_data_model::runtime_data_model(runtime_data_model&& right) noexcept
{
	if (!right.elements.empty())
	{
		size_t count = right.elements.size();
		elements.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			elements.push_back(std::move(right.elements[i]));
		}
		right.elements.clear();
	}
}
runtime_data_model& runtime_data_model::operator=(const runtime_data_model& right)
{
	if (this != &right)
	{
		elements.clear();
		if (!right.elements.empty())
		{
			size_t count = right.elements.size();
			elements.reserve(count);
			for (size_t i = 0; i < count; i++)
			{
				elements.push_back(right.elements[i]);
			}
		}
	}
	return *this;
}
runtime_data_model& runtime_data_model::operator=(runtime_data_model&& right) noexcept
{
	if (this != &right)
	{
		elements.clear();
		if (!right.elements.empty())
		{
			size_t count = right.elements.size();
			elements.reserve(count);
			for (size_t i = 0; i < count; i++)
			{
				elements.push_back(std::move(right.elements[i]));
			}
			right.elements.clear();
		}
	}
	return *this;
}
// Class rx::data::runtime_model_element 


bool runtime_model_element::is_value () const
{
	return std::holds_alternative<rx_simple_value>(value);
}

bool runtime_model_element::is_complex () const
{
	return std::holds_alternative<rt_model_wrapper<false> >(value);
}

bool runtime_model_element::is_complex_array () const
{
	return std::holds_alternative<rt_model_wrapper<true> >(value);
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
		return std::get<rt_model_wrapper<false> >(value).model;
	else
		return g_empty;
}

const runtime_data_model& runtime_model_element::get_complex_array () const
{
	static runtime_data_model g_empty;
	if (is_complex_array())
		return std::get<rt_model_wrapper<true> >(value).model;
	else
		return g_empty;
}


} // namespace data
} // namespace rx

