

/****************************************************************************
*
*  system\runtime\rx_rt_item_helpers.cpp
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



#include "system/runtime/rx_rt_item_types.h"

namespace rx_platform {
namespace runtime {
namespace structure {

template class array_wrapper<block_data>;
template class array_wrapper<const_value_data>;

void collect_data_const(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper<const_value_data>& item)
{
	if (type != runtime_value_type::persistent_runtime_value)
	{
		if (!item.is_array())
		{
			data.add_value(name, item.get_item()->value);
		}
		else
		{
			std::vector<rx_simple_value> temp_vals;
			temp_vals.reserve(item.get_size());
			for (int i = 0; i < item.get_size(); i++)
			{
				temp_vals.push_back(item.get_item(i)->value);
			}
			data.add_value(name, std::move(temp_vals));
		}
	}
}
void collect_data_value(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper <value_data>& item)
{
	if (!item.is_array())
	{
		if (type != runtime_value_type::persistent_runtime_value
			|| item.get_item()->value_opt[runtime::structure::value_opt_persistent])
			data.add_value(name, item.get_item()->value.to_simple());
	}
	else
	{
		std::vector<rx_simple_value> temp_vals;
		temp_vals.reserve(item.get_size());
		for (int i = 0; i < item.get_size(); i++)
		{
			if (type != runtime_value_type::persistent_runtime_value
				|| item.get_item(i)->value_opt[runtime::structure::value_opt_persistent])
				temp_vals.push_back(item.get_item(i)->value.to_simple());
		}
		data.add_value(name, std::move(temp_vals));
	}
}
void collect_data_variable(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper<variable_data>& item)
{
	if (!item.is_array())
	{
		if (type == runtime_value_type::simple_runtime_value)
		{
			data.add_value(name, item.get_item()->value.to_simple());
		}
		else
		{
			data::runtime_values_data child_data;
			item.get_item()->collect_data(child_data, type);
			if (!child_data.empty())
				data.add_child(name, std::move(child_data));
		}
	}
	else
	{
		if (type == runtime_value_type::simple_runtime_value)
		{
			std::vector<rx_simple_value> temp_vals;
			temp_vals.reserve(item.get_size());
			for (int i = 0; i < item.get_size(); i++)
			{
				temp_vals.push_back(item.get_item(i)->value.to_simple());
			}
			data.add_value(name, std::move(temp_vals));
		}
		else
		{
			std::vector<data::runtime_values_data> childs;
			childs.reserve(item.get_size());
			for (int i = 0; i < item.get_size(); i++)
			{
				data::runtime_values_data child_data;
				item.get_item(i)->collect_data(child_data, type);
				if (!child_data.empty())
					childs.push_back(std::move(child_data));
			}
			if (!childs.empty())
				data.add_array_child(name, std::move(childs));
		}
	}
}
template<typename T>
void collect_data_arrayed(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper<T>& item)
{
	if (!item.is_array())
	{
		data::runtime_values_data child_data;
		item.get_item()->collect_data(child_data, type);
		if (!child_data.empty())
			data.add_child(name, std::move(child_data));
	}
	else
	{
		std::vector<data::runtime_values_data> childs;
		childs.reserve(item.get_size());
		for (int i = 0; i < item.get_size(); i++)
		{
			data::runtime_values_data child_data;
			item.get_item(i)->collect_data(child_data, type);
			if (!child_data.empty())
				childs.push_back(std::move(child_data));
		}
		if (!childs.empty())
			data.add_array_child(name, std::move(childs));
	}
}
template void collect_data_arrayed<struct_data>(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper<struct_data>& item);
template void collect_data_arrayed<value_block_data>(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper<value_block_data>& item);
template void collect_data_arrayed<variable_block_data>(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const array_wrapper<variable_block_data>& item);

template<typename T>
void collect_data_plain(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const T& item)
{
	if (type == runtime_value_type::persistent_runtime_value ||
		(type & T::runtime_type_ref) == T::runtime_type_ref)
	{
		data::runtime_values_data child_data;
		item.collect_data(child_data, type);
		if (!child_data.empty())
			data.add_child(name, std::move(child_data));
	}
}
template void collect_data_plain<source_data>(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const source_data& item);
template void collect_data_plain<mapper_data>(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const mapper_data& item);
template void collect_data_plain<filter_data>(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const filter_data& item);
template void collect_data_plain<event_data>(const string_type& name, data::runtime_values_data& data, runtime_value_type type, const event_data& item);


void fill_data_const(const string_type& name, const data::runtime_values_data& data, array_wrapper<const_value_data>& item)
{
	if (!item.is_array())
	{
		auto val = data.get_value(name);
		if (!val.is_null())
		{
			item.get_item()->set_value(std::move(val));
		}
	}
	else
	{
		std::vector<rx_simple_value> vals;
		if (data.get_array_value(name, vals))
		{
			size_t dim = std::min<size_t>(vals.size(), item.get_size());
			for (size_t i = 0; i < dim; i++)
			{
				item.get_item((int)i)->set_value(std::move(vals[i]));
			}
		}
	}
}
void fill_data_value(const string_type& name, const data::runtime_values_data& data, array_wrapper<value_data>& item)
{

	if (!item.is_array())
	{
		auto val = data.get_value(name);
		if (!val.is_null())
		{
			item.get_item()->set_value(std::move(val), rx_time::now());
		}
	}
	else
	{
		std::vector<rx_simple_value> vals;
		if (data.get_array_value(name, vals))
		{
			rx_time now = rx_time::now();
			size_t dim = std::min<size_t>(vals.size(), item.get_size());
			for (size_t i = 0; i < dim; i++)
			{
				item.get_item((int)i)->set_value(std::move(vals[i]), now);
			}
		}
	}
}
void fill_data_variable(const string_type& name, const data::runtime_values_data& data, array_wrapper<variable_data>& item)
{
	// check for simple value first
	if (!item.is_array())
	{
		auto val = data.get_value(name);
		if (!val.is_null())
		{
			item.get_item()->set_value(std::move(val));
		}
		auto it = data.children.find(name);
		if (it != data.children.end())
		{
			if (std::holds_alternative<data::runtime_values_data>(it->second))
			{
				item.get_item()->fill_data(
					std::get< data::runtime_values_data>(it->second));
			}
		}
	}
	else
	{
		std::vector<rx_simple_value> vals;
		if (data.get_array_value(name, vals))
		{
			size_t dim = std::min<size_t>(vals.size(), item.get_size());
			for (size_t i = 0; i < dim; i++)
			{
				item.get_item((int)i)->set_value(std::move(vals[i]));
			}
		}
		auto it = data.children.find(name);
		if (it != data.children.end())
		{
			if (std::holds_alternative<std::vector<data::runtime_values_data> >(it->second))
			{
				auto& childs = std::get<std::vector<data::runtime_values_data>>(it->second);
				size_t dim = std::min<size_t>(childs.size(), item.get_size());
				for (size_t i = 0; i < dim; i++)
				{
					item.get_item((int)i)->fill_data(childs[i]);
				}
			}
		}
	}
}
template<typename T>
void fill_data_arrayed(const string_type& name, const data::runtime_values_data& data, array_wrapper<T>& item)
{
	if (!item.is_array())
	{
		auto it = data.children.find(name);
		if (it != data.children.end())
		{
			if (std::holds_alternative<data::runtime_values_data>(it->second))
			{
				item.get_item()->fill_data(
					std::get< data::runtime_values_data>(it->second));
			}
		}
	}
	else
	{
		auto it = data.children.find(name);
		if (it != data.children.end())
		{
			if (std::holds_alternative<std::vector<data::runtime_values_data> >(it->second))
			{
				auto& childs = std::get<std::vector<data::runtime_values_data>>(it->second);
				size_t dim = std::min<size_t>(childs.size(), item.get_size());
				for (size_t i = 0; i < dim; i++)
				{
					item.get_item((int)i)->fill_data(childs[i]);
				}
			}
		}
	}
}
template void fill_data_arrayed<struct_data>(const string_type& name, const data::runtime_values_data& data, array_wrapper<struct_data>& item);
template void fill_data_arrayed<value_block_data>(const string_type& name, const data::runtime_values_data& data, array_wrapper<value_block_data>& item);
template void fill_data_arrayed<variable_block_data>(const string_type& name, const data::runtime_values_data& data, array_wrapper<variable_block_data>& item);


template<typename T>
void fill_data_plain(const string_type& name, const data::runtime_values_data& data, T& item)
{
	auto it = data.children.find(name);
	if (it != data.children.end() && std::holds_alternative<data::runtime_values_data>(it->second))
	{
		item.fill_data(std::get<data::runtime_values_data>(it->second));
	}
}


template void fill_data_plain<source_data>(const string_type& name, const data::runtime_values_data& data, source_data& item);
template void fill_data_plain<mapper_data>(const string_type& name, const data::runtime_values_data& data, mapper_data& item);
template void fill_data_plain<filter_data>(const string_type& name, const data::runtime_values_data& data, filter_data& item);
template void fill_data_plain<event_data>(const string_type& name, const data::runtime_values_data& data, event_data& item);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// collect_value


void collect_value_const(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper<const_value_data>& item)
{
	if (type != runtime_value_type::persistent_runtime_value)
	{
		if (!item.is_array())
		{
			data.push_back(item.get_item()->value);
		}
		else
		{
			std::vector<rx_simple_value> temp_vals;
			temp_vals.reserve(item.get_size());
			for (int i = 0; i < item.get_size(); i++)
			{
				temp_vals.push_back(item.get_item(i)->value);
			}
			rx_simple_value val;
			val.assign_array(temp_vals);
			data.push_back(std::move(val));
		}
	}
}

void collect_value_value(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper <value_data>& item)
{
	if (!item.is_array())
	{
		if (type != runtime_value_type::persistent_runtime_value
			|| item.get_item()->value_opt[runtime::structure::value_opt_persistent])
			data.push_back(item.get_item()->value.to_simple());
	}
	else
	{
		std::vector<rx_simple_value> temp_vals;
		temp_vals.reserve(item.get_size());
		for (int i = 0; i < item.get_size(); i++)
		{
			if (type != runtime_value_type::persistent_runtime_value
				|| item.get_item(i)->value_opt[runtime::structure::value_opt_persistent])
				temp_vals.push_back(item.get_item(i)->value.to_simple());
		}
		rx_simple_value val;
		val.assign_array(temp_vals);
		data.push_back(std::move(val));
	}
}

void collect_value_variable(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper<variable_data>& item)
{
	if (!item.is_array())
	{
		if (type == runtime_value_type::simple_runtime_value)
		{
			data.push_back(item.get_item()->value.to_simple());
		}
		else
		{
			values::rx_simple_value child_data;
			item.get_item()->collect_value(child_data, type);
			if (!child_data.is_null())
				data.push_back(std::move(child_data));
		}
	}
	else
	{
		if (type == runtime_value_type::simple_runtime_value)
		{
			std::vector<rx_simple_value> temp_vals;
			temp_vals.reserve(item.get_size());
			for (int i = 0; i < item.get_size(); i++)
			{
				temp_vals.push_back(item.get_item(i)->value.to_simple());
			}
			rx_simple_value val;
			val.assign_array(temp_vals);
			data.push_back(std::move(val));
		}
		else
		{
			std::vector<values::rx_simple_value> childs;
			childs.reserve(item.get_size());
			for (int i = 0; i < item.get_size(); i++)
			{
				values::rx_simple_value child_data;
				item.get_item(i)->collect_value(child_data, type);
				if (!child_data.is_null())
					childs.push_back(std::move(child_data));
			}
			if (!childs.empty())
			{				
				rx_simple_value val;
				val.assign_array(childs);
				data.push_back(std::move(val));
			}
		}
	}
}

template<typename T>
void collect_value_arrayed(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper<T>& item)
{
	if (!item.is_array())
	{
		values::rx_simple_value child_data;
		item.get_item()->collect_value(child_data, type);
		if (!child_data.is_null())
			data.push_back(std::move(child_data));
	}
	else
	{
		std::vector<values::rx_simple_value> childs;
		childs.reserve(item.get_size());
		for (int i = 0; i < item.get_size(); i++)
		{
			values::rx_simple_value child_data;
			item.get_item(i)->collect_value(child_data, type);
			if (!child_data.is_null())
				childs.push_back(std::move(child_data));
		}
		if (!childs.empty())
		{
			rx_simple_value val;
			val.assign_array(childs);
			data.push_back(std::move(val));
		}
	}
}
template void collect_value_arrayed<struct_data>(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper<struct_data>& item);
template void collect_value_arrayed<value_block_data>(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper<value_block_data>& item);
template void collect_value_arrayed<variable_block_data>(std::vector<values::rx_simple_value>& data, runtime_value_type type, const array_wrapper<variable_block_data>& item);

template<typename T>
void collect_value_plain(std::vector<values::rx_simple_value>& data, runtime_value_type type, const T& item)
{
	if (type == runtime_value_type::persistent_runtime_value ||
		(type & T::runtime_type_ref) == T::runtime_type_ref)
	{
		values::rx_simple_value child_data;
		item.collect_value(child_data, type);
		if (!child_data.is_null())
			data.push_back(std::move(child_data));
	}
}
template void collect_value_plain<source_data>(std::vector<values::rx_simple_value>& data, runtime_value_type type, const source_data& item);
template void collect_value_plain<mapper_data>(std::vector<values::rx_simple_value>& data, runtime_value_type type, const mapper_data& item);
template void collect_value_plain<filter_data>(std::vector<values::rx_simple_value>& data, runtime_value_type type, const filter_data& item);
template void collect_value_plain<event_data>(std::vector<values::rx_simple_value>& data, runtime_value_type type, const event_data& item);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string_view_type extract_index(string_view_type name, int& idx)
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
members_index_type internal_get_index(string_view_type name, int& idx, const const_size_vector<index_data>& items)
{
	int item_idx = -1;
	name = extract_index(name, item_idx);
	if (name.empty())
		return 0;
	for (const auto& one : items)
	{
		if (one.name == name)
		{
			idx = item_idx;
			return one.index;
		}
	}
	return 0;
}

members_index_type internal_split_get_index(string_view_type path, string_view_type& mine, string_view_type& bellow, int& idx, const const_size_vector<index_data>& items)
{
	size_t str_idx = path.find(RX_OBJECT_DELIMETER);
	if (str_idx != string_type::npos)
	{
		mine = path.substr(0, str_idx);
		bellow = path.substr(str_idx + 1);
	}
	else
	{
		mine = path;
	}
	int item_idx = -1;
	mine = extract_index(mine, item_idx);
	if (mine.empty())
		return 0;
	for (const auto& one : items)
	{
		if (one.name == mine)
		{
			idx = item_idx;
			return one.index;
		}
	}
	return 0;
}

bool is_value_index(members_index_type idx)
{
	auto temp = idx & rt_type_mask;
	return temp == rt_const_index_type || temp == rt_value_index_type || temp == rt_variable_index_type
		|| temp == rt_source_index_type || temp == rt_mapper_index_type;
}

bool is_complex_index(members_index_type idx)
{
	auto temp = idx & rt_type_mask;
	return temp != rt_const_index_type && temp != rt_value_index_type && temp;
}

rx_result get_value_const(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<const_value_data>& item)
{
	if (!bellow.empty())
		return RX_INVALID_PATH;
	if (array_idx < 0)
	{
		if (!item.is_array())
		{
			val = item.get_item()->get_value(ctx);
			return true;
		}
		else
		{
			rx_simple_value temp_val;
			uint32_t len;
			auto size = item.get_size();
			RX_ASSERT(size >= 0);
			if (size >= 0)
			{
				len = (uint32_t)size;
				temp_val.assign_static(len);
				val = ctx->adapt_value(temp_val);
				return true;
			}
			else
			{
				return RX_NOT_AN_ARRAY;
			}
		}
	}
	else
	{
		if (!item.is_array())
		{
			return RX_NOT_AN_ARRAY;
		}
		else if (array_idx < item.get_size())
		{
			val = item.get_item(array_idx)->get_value(ctx);
			return true;
		}
		else
		{
			return RX_OUT_OF_BOUNDS;
		}
	}
}
rx_result get_value_value(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<value_data>& item)
{
	if (!bellow.empty())
		return RX_INVALID_PATH;

	if (array_idx < 0)
	{
		if (!item.is_array())
		{
			val = item.get_item()->get_value(ctx);
			return true;
		}
		else
		{
			rx_simple_value temp_val;
			uint32_t len;
			auto size = item.get_size();
			RX_ASSERT(size >= 0);
			if (size >= 0)
			{
				len = (uint32_t)size;
				temp_val.assign_static(len);
				val = ctx->adapt_value(temp_val);
				return true;
			}
			else
			{
				return RX_NOT_AN_ARRAY;
			}
		}
	}
	else
	{
		if (!item.is_array())
		{
			return RX_NOT_AN_ARRAY;
		}
		else if (array_idx < item.get_size())
		{
			val = item.get_item(array_idx)->get_value(ctx);
			return true;
		}
		else
		{
			return RX_OUT_OF_BOUNDS;
		}
	}
}
rx_result get_value_variable(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<variable_data>& item)
{
	if (bellow.empty())
	{
		if (array_idx < 0)
		{
			if (!item.is_array())
			{
				val = item.get_item()->get_value(ctx);
				return true;
			}
			else
			{
				rx_simple_value temp_val;
				uint32_t len;
				auto size = item.get_size();
				RX_ASSERT(size >= 0);
				if (size >= 0)
				{
					len = (uint32_t)size;
					temp_val.assign_static(len);
					val = ctx->adapt_value(temp_val);
					return true;
				}
				else
				{
					return RX_NOT_AN_ARRAY;
				}
			}
		}
		else
		{
			if (!item.is_array())
			{
				return RX_NOT_AN_ARRAY;
			}
			else if (array_idx < item.get_size())
			{
				val = item.get_item(array_idx)->get_value(ctx);
				return true;
			}
			else
			{
				return RX_OUT_OF_BOUNDS;
			}
		}
	}
	else
	{
		if (array_idx < 0)
		{
			if (item.is_array())
				return "Expected array index!";
			else
				return item.get_item()->get_value(bellow, val, ctx);
		}
		else
		{
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			else if (array_idx < item.get_size())
				return item.get_item(array_idx)->get_value(bellow, val, ctx);
			else
				return RX_OUT_OF_BOUNDS;
		}
	}
}
template<typename T>
rx_result get_value_arrayed(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<T>& item)
{
	if (bellow.empty())
	{
		if (array_idx < 0)
		{
			if (!item.is_array())
			{
				rx_simple_value temp_val;
				rx_result result = item.get_item()->collect_value(temp_val, runtime_value_type::simple_runtime_value);
				if (!result)
				{
					return result;
				}
				val = ctx->adapt_value(temp_val);
				return true;
			}
			else
			{
				rx_simple_value temp_val;
				uint32_t len;
				auto size = item.get_size();
				RX_ASSERT(size >= 0);
				if (size >= 0)
				{
					len = (uint32_t)size;
					temp_val.assign_static(len);
					val = ctx->adapt_value(temp_val);
					return true;
				}
				else
				{
					return RX_NOT_AN_ARRAY;
				}
			}
		}
		else
		{
			if (!item.is_array())
			{
				return RX_NOT_AN_ARRAY;
			}
			else if (array_idx < item.get_size())
			{
				rx_simple_value temp_val;
				rx_result result = item.get_item(array_idx)->collect_value(temp_val, runtime_value_type::simple_runtime_value);
				if (!result)
				{
					return result;
				}
				val = rx_value(std::move(temp_val), rx_time::now(), RX_GOOD_QUALITY);
				val = ctx->adapt_value(temp_val);
				return true;
			}
			else
			{
				return RX_OUT_OF_BOUNDS;
			}
		}
	}
	else
	{
		if (array_idx < 0)
		{
			if (item.is_array())
				return "Expected array index!";
			else
				return item.get_item()->get_value(bellow, val, ctx);
		}
		else
		{
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			else if (array_idx < item.get_size())
				return item.get_item(array_idx)->get_value(bellow, val, ctx);
			else
				return RX_OUT_OF_BOUNDS;
		}
	}
}
template rx_result get_value_arrayed<struct_data>(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<struct_data>& item);
template rx_result get_value_arrayed<value_block_data>(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<value_block_data>& item);
template rx_result get_value_arrayed<variable_block_data>(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const array_wrapper<variable_block_data>& item);

template<typename T>
rx_result get_value_plain(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const T& item)
{
	if (array_idx < 0)
	{
		if (bellow.empty())
		{
			if constexpr (T::has_own_value)
			{
				val = item.get_value(ctx);
				return true;
			}
			else
			{
				return RX_INVALID_PATH;
			}
		}
		else
		{
			return item.get_value(bellow, val, ctx);
		}
	}
	else
	{
		return RX_NOT_AN_ARRAY;
	}
}
template rx_result get_value_plain<source_data>(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const source_data& item);
template rx_result get_value_plain<mapper_data>(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const mapper_data& item);
template rx_result get_value_plain<event_data>(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const event_data& item);
template rx_result get_value_plain<filter_data>(int array_idx, string_view_type bellow, rx_value& val, runtime_process_context* ctx, const filter_data& item);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rx_result get_local_value_const(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<const_value_data>& item)
{
	if (!bellow.empty())
		return RX_INVALID_PATH;
	if (array_idx < 0)
	{
		if (!item.is_array())
		{
			val = item.get_item()->simple_get_value();
			return true;
		}
		else
		{
			rx_simple_value temp_val;
			uint32_t len;
			auto size = item.get_size();
			RX_ASSERT(size >= 0);
			if (size >= 0)
			{
				len = (uint32_t)size;
				temp_val.assign_static(len);
				val = std::move(temp_val);
				return true;
			}
			else
			{
				return RX_NOT_AN_ARRAY;
			}
		}
	}
	else
	{
		if (!item.is_array())
		{
			return RX_NOT_AN_ARRAY;
		}
		else if (array_idx < item.get_size())
		{
			val = item.get_item(array_idx)->simple_get_value();
			return true;
		}
		else
		{
			return RX_OUT_OF_BOUNDS;
		}
	}
}
rx_result get_local_value_value(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<value_data>& item)
{
	if (!bellow.empty())
		return RX_INVALID_PATH;

	if (array_idx < 0)
	{
		if (!item.is_array())
		{
			val = item.get_item()->simple_get_value();
			return true;
		}
		else
		{
			rx_simple_value temp_val;
			uint32_t len;
			auto size = item.get_size();
			RX_ASSERT(size >= 0);
			if (size >= 0)
			{
				len = (uint32_t)size;
				temp_val.assign_static(len);
				val = std::move(temp_val);
				return true;
			}
			else
			{
				return RX_NOT_AN_ARRAY;
			}
		}
	}
	else
	{
		if (!item.is_array())
		{
			return RX_NOT_AN_ARRAY;
		}
		else if (array_idx < item.get_size())
		{
			val = item.get_item(array_idx)->simple_get_value();
			return true;
		}
		else
		{
			return RX_OUT_OF_BOUNDS;
		}
	}
}
rx_result get_local_value_variable(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<variable_data>& item)
{
	if (bellow.empty())
	{
		if (array_idx < 0)
		{
			if (!item.is_array())
			{
				val = item.get_item()->simple_get_value();
				return true;
			}
			else
			{
				rx_simple_value temp_val;
				uint32_t len;
				auto size = item.get_size();
				RX_ASSERT(size >= 0);
				if (size >= 0)
				{
					len = (uint32_t)size;
					temp_val.assign_static(len);
					val = std::move(temp_val);
					return true;
				}
				else
				{
					return RX_NOT_AN_ARRAY;
				}
			}
		}
		else
		{
			if (!item.is_array())
			{
				return RX_NOT_AN_ARRAY;
			}
			else if (array_idx < item.get_size())
			{
				val = item.get_item(array_idx)->simple_get_value();
				return true;
			}
			else
			{
				return RX_OUT_OF_BOUNDS;
			}
		}
	}
	else
	{
		if (array_idx < 0)
		{
			if (item.is_array())
				return "Expected array index!";
			else
				return item.get_item()->get_local_value(bellow, val);
		}
		else
		{
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			else if (array_idx < item.get_size())
				return item.get_item(array_idx)->get_local_value(bellow, val);
			else
				return RX_OUT_OF_BOUNDS;
		}
	}
}
template<typename T>
rx_result get_local_value_arrayed(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<T>& item)
{
	if (bellow.empty())
	{
		if (array_idx < 0)
		{
			if (!item.is_array())
			{
				rx_simple_value temp_val;
				rx_result result = item.get_item()->collect_value(temp_val, runtime_value_type::simple_runtime_value);
				if (!result)
				{
					return result;
				}
				val = std::move(temp_val);
				return true;
			}
			else
			{
				rx_simple_value temp_val;
				uint32_t len;
				auto size = item.get_size();
				RX_ASSERT(size >= 0);
				if (size >= 0)
				{
					len = (uint32_t)size;
					temp_val.assign_static(len);
					val = std::move(temp_val);
					return true;
				}
				else
				{
					return RX_NOT_AN_ARRAY;
				}
			}
		}
		else
		{
			if (!item.is_array())
			{
				return RX_NOT_AN_ARRAY;
			}
			else if (array_idx < item.get_size())
			{
				rx_simple_value temp_val;
				rx_result result = item.get_item(array_idx)->collect_value(temp_val, runtime_value_type::simple_runtime_value);
				if (!result)
				{
					return result;
				}
				val = rx_simple_value(std::move(temp_val));
				return true;
			}
			else
			{
				return RX_OUT_OF_BOUNDS;
			}
		}
	}
	else
	{
		if (array_idx < 0)
		{
			if (item.is_array())
				return "Expected array index!";
			else
				return item.get_item()->get_local_value(bellow, val);
		}
		else
		{
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			else if (array_idx < item.get_size())
				return item.get_item(array_idx)->get_local_value(bellow, val);
			else
				return RX_OUT_OF_BOUNDS;
		}
	}
}
template rx_result get_local_value_arrayed<struct_data>(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<struct_data>& item);
template rx_result get_local_value_arrayed<value_block_data>(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<value_block_data>& item);
template rx_result get_local_value_arrayed<variable_block_data>(int array_idx, string_view_type bellow, rx_simple_value& val, const array_wrapper<variable_block_data>& item);

template<typename T>
rx_result get_local_value_plain(int array_idx, string_view_type bellow, rx_simple_value& val, const T& item)
{
	if (array_idx < 0)
	{
		if (bellow.empty())
		{
			return RX_INVALID_PATH;
		}
		else
		{
			return item.get_local_value(bellow, val);
		}
	}
	else
	{
		return RX_NOT_AN_ARRAY;
	}
}
template rx_result get_local_value_plain<source_data>(int array_idx, string_view_type bellow, rx_simple_value& val, const source_data& item);
template rx_result get_local_value_plain<mapper_data>(int array_idx, string_view_type bellow, rx_simple_value& val, const mapper_data& item);
template rx_result get_local_value_plain<event_data>(int array_idx, string_view_type bellow, rx_simple_value& val, const event_data& item);
template rx_result get_local_value_plain<filter_data>(int array_idx, string_view_type bellow, rx_simple_value& val, const filter_data& item);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



rx_result get_value_ref_const(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<const_value_data>& item)
{
	if (!bellow.empty())
		return RX_INVALID_PATH;
	if (array_idx < 0)
	{
		ref.ref_type = rt_value_ref_type::rt_const_value;
		if (!item.is_array())
			ref.ref_value_ptr.const_value = item.get_item();
		else
			return RX_INVALID_PATH;
	}
	else
	{
		ref.ref_type = rt_value_ref_type::rt_const_value;
		if (!item.is_array())
			return RX_NOT_AN_ARRAY;
		else if (array_idx < item.get_size())
			ref.ref_value_ptr.const_value = item.get_item(array_idx);
		else
			return RX_OUT_OF_BOUNDS;
	}
	return true;
}
rx_result get_value_ref_value(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<value_data>& item)
{
	if (!bellow.empty())
		return RX_INVALID_PATH;
	if (array_idx < 0)
	{
		ref.ref_type = rt_value_ref_type::rt_value;
		if (!item.is_array())
			ref.ref_value_ptr.value = item.get_item();
		else
			return RX_INVALID_PATH;
	}
	else
	{
		ref.ref_type = rt_value_ref_type::rt_value;
		if (!item.is_array())
			return RX_NOT_AN_ARRAY;
		else if (array_idx < item.get_size())
			ref.ref_value_ptr.value = item.get_item(array_idx);
		else
			return RX_OUT_OF_BOUNDS;
	}
	return true;
}

rx_result get_value_ref_variable(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<variable_data>& item)
{
	if (bellow.empty())
	{
		if (array_idx < 0)
		{
			ref.ref_type = rt_value_ref_type::rt_variable;
			if (!item.is_array())
				ref.ref_value_ptr.variable = item.get_item();
			else
				return RX_INVALID_PATH;
		}
		else
		{
			ref.ref_type = rt_value_ref_type::rt_variable;
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			else if (array_idx < item.get_size())
				ref.ref_value_ptr.variable = item.get_item(array_idx);
			else
				return RX_OUT_OF_BOUNDS;
		}
		return true;
	}
	else
	{
		if (array_idx < 0)
		{
			if (!item.is_array())
				return item.get_item()->get_value_ref(bellow, ref);
			else
				return RX_INVALID_PATH;
		}
		else
		{
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			else if (array_idx < item.get_size())
				return item.get_item(array_idx)->get_value_ref(bellow, ref);
			else
				return RX_OUT_OF_BOUNDS;
		}
	}
}
template<typename T>
rx_result get_value_ref_arrayed(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<T>& item)
{
	if (bellow.empty())
	{
		if constexpr (T::has_own_value)
		{
			if (array_idx < 0)
			{
				if (!item.is_array())
					return item.get_item()->get_value_ref("", ref);
				else
					return RX_INVALID_PATH;
			}
			else
			{
				if (!item.is_array())
					return RX_NOT_AN_ARRAY;
				else if (array_idx < item.get_size())
					return item.get_item(array_idx)->get_value_ref("", ref);
				else
					return RX_OUT_OF_BOUNDS;
			}
		}
		else
		{
			return RX_INVALID_PATH;
		}
	}
	else
	{
		if (array_idx < 0)
		{
			if (!item.is_array())
				return item.get_item()->get_value_ref(bellow, ref);
			else
				return RX_INVALID_PATH;
		}
		else
		{
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			else if (array_idx < item.get_size())
				return item.get_item(array_idx)->get_value_ref(bellow, ref);
			else
				return RX_OUT_OF_BOUNDS;
		}
	}
}
template rx_result get_value_ref_arrayed<struct_data>(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<struct_data>& item);
template rx_result get_value_ref_arrayed<value_block_data>(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<value_block_data>& item);
template rx_result get_value_ref_arrayed<variable_block_data>(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, array_wrapper<variable_block_data>& item);
template<typename T>
rx_result get_value_ref_plain(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, T& item)
{
	if (bellow.empty())
	{
		if constexpr (T::has_own_value)
		{
			return item.get_value_ref("", ref);
		}
		else
		{
			return RX_INVALID_PATH;
		}
	}
	else
	{
		return item.get_value_ref(bellow, ref);
	}
}
template rx_result get_value_ref_plain<source_data>(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, source_data& item);
template rx_result get_value_ref_plain<mapper_data>(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, mapper_data& item);
template rx_result get_value_ref_plain<filter_data>(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, filter_data& item);
template rx_result get_value_ref_plain<event_data>(int array_idx, string_view_type bellow, string_view_type path, rt_value_ref& ref, bool is_var, event_data& item);


template<typename T>
void object_state_changed_arrayed(runtime_process_context* ctx, const_size_vector<array_wrapper<T> >& items)
{
	for (auto& item : items)
	{
		if (item.is_array())
		{
			for (int i = 0; i < item.get_size(); i++)
			{
				item.get_item(i)->object_state_changed(ctx);
			}
		}
		else
		{
			item.get_item()->object_state_changed(ctx);
		}
	}
}
template void object_state_changed_arrayed(runtime_process_context* ctx, const_size_vector<array_wrapper<value_data> >& item);
template void object_state_changed_arrayed(runtime_process_context* ctx, const_size_vector<array_wrapper<variable_data> >& item);
template void object_state_changed_arrayed(runtime_process_context* ctx, const_size_vector<array_wrapper<struct_data> >& item);
template void object_state_changed_arrayed(runtime_process_context* ctx, const_size_vector<array_wrapper<value_block_data> >& item);
template void object_state_changed_arrayed(runtime_process_context* ctx, const_size_vector<array_wrapper<variable_block_data> >& item);


template<typename T>
void object_state_changed_plain(runtime_process_context* ctx, const_size_vector<T>& items)
{
	for (auto& item : items)
	{
		item.object_state_changed(ctx);
	}
}
template void object_state_changed_plain(runtime_process_context* ctx, const_size_vector<source_data>& items);
template void object_state_changed_plain(runtime_process_context* ctx, const_size_vector<mapper_data>& items);
template void object_state_changed_plain(runtime_process_context* ctx, const_size_vector<filter_data>& items);
template void object_state_changed_plain(runtime_process_context* ctx, const_size_vector<event_data>& items);

template<typename T>
rx_result initialize_runtime_arrayed(const string_type name, runtime::runtime_init_context& ctx, array_wrapper<T>& item)
{
	rx_result ret;
	if (item.is_array())
	{
		char buff[0x100];
		for (int i = 0; i < item.get_size(); i++)
		{
			sprintf(buff, "%s[%d]", name.c_str(), i);
			ctx.path.push_to_path(buff);
			ret = item.get_item(i)->initialize_runtime(ctx);
			if (!ret)
				break;
			ctx.path.pop_from_path();
		}
	}
	else
	{
		ctx.path.push_to_path(name);
		ret = item.get_item()->initialize_runtime(ctx);
		ctx.path.pop_from_path();
	}
	return ret;
}
template rx_result initialize_runtime_arrayed<variable_data>(const string_type name, runtime::runtime_init_context& ctx, array_wrapper<variable_data>& item);
template rx_result initialize_runtime_arrayed<struct_data>(const string_type name, runtime::runtime_init_context& ctx, array_wrapper<struct_data>& item);
template rx_result initialize_runtime_arrayed<value_block_data>(const string_type name, runtime::runtime_init_context& ctx, array_wrapper<value_block_data>& item);
template rx_result initialize_runtime_arrayed<variable_block_data>(const string_type name, runtime::runtime_init_context& ctx, array_wrapper<variable_block_data>& item);


template<typename T>
rx_result initialize_runtime_plain(const string_type name, runtime::runtime_init_context& ctx, T& item)
{
	rx_result ret;
	ctx.path.push_to_path(name);
	ret = item.initialize_runtime(ctx);
	ctx.path.pop_from_path();
	return ret;
}
template rx_result initialize_runtime_plain<source_data>(const string_type name, runtime::runtime_init_context& ctx, source_data& item);
template rx_result initialize_runtime_plain<mapper_data>(const string_type name, runtime::runtime_init_context& ctx, mapper_data& item);
template rx_result initialize_runtime_plain<filter_data>(const string_type name, runtime::runtime_init_context& ctx, filter_data& item);
template rx_result initialize_runtime_plain<event_data>(const string_type name, runtime::runtime_init_context& ctx, event_data& item);


template<typename T>
rx_result deinitialize_runtime_arrayed(runtime::runtime_deinit_context& ctx, array_wrapper<T>& item)
{
	rx_result ret;
	if (item.is_array())
	{
		for (int i = 0; i < item.get_size(); i++)
		{
			ret = item.get_item(i)->deinitialize_runtime(ctx);
			if (!ret)
				break;
		}
	}
	else
	{
		ret = item.get_item()->deinitialize_runtime(ctx);
	}
	return ret;
}
template rx_result deinitialize_runtime_arrayed<variable_data>(runtime::runtime_deinit_context& ctx, array_wrapper<variable_data>& item);
template rx_result deinitialize_runtime_arrayed<struct_data>(runtime::runtime_deinit_context& ctx, array_wrapper<struct_data>& item);
template rx_result deinitialize_runtime_arrayed<value_block_data>(runtime::runtime_deinit_context& ctx, array_wrapper<value_block_data>& item);
template rx_result deinitialize_runtime_arrayed<variable_block_data>(runtime::runtime_deinit_context& ctx, array_wrapper<variable_block_data>& item);


template<typename T>
rx_result deinitialize_runtime_plain(runtime::runtime_deinit_context& ctx, T& item)
{
	rx_result ret;
	ret = item.deinitialize_runtime(ctx);
	return ret;
}
template rx_result deinitialize_runtime_plain<source_data>(runtime::runtime_deinit_context& ctx, source_data& item);
template rx_result deinitialize_runtime_plain<mapper_data>(runtime::runtime_deinit_context& ctx, mapper_data& item);
template rx_result deinitialize_runtime_plain<filter_data>(runtime::runtime_deinit_context& ctx, filter_data& item);
template rx_result deinitialize_runtime_plain<event_data>(runtime::runtime_deinit_context& ctx, event_data& item);


template<typename T>
rx_result start_runtime_arrayed(const string_type name, runtime::runtime_start_context& ctx, array_wrapper<T>& item)
{
	rx_result ret;
	if (item.is_array())
	{
		char buff[0x100];
		for (int i = 0; i < item.get_size(); i++)
		{
			sprintf(buff, "%s[%d]", name.c_str(), i);
			ctx.path.push_to_path(buff);
			ret = item.get_item(i)->start_runtime(ctx);
			if (!ret)
				break;
			ctx.path.pop_from_path();
		}
	}
	else
	{
		ctx.path.push_to_path(name);
		ret = item.get_item()->start_runtime(ctx);
		ctx.path.pop_from_path();
	}
	return ret;
}
template rx_result start_runtime_arrayed<variable_data>(const string_type name, runtime::runtime_start_context& ctx, array_wrapper<variable_data>& item);
template rx_result start_runtime_arrayed<struct_data>(const string_type name, runtime::runtime_start_context& ctx, array_wrapper<struct_data>& item);
template rx_result start_runtime_arrayed<value_block_data>(const string_type name, runtime::runtime_start_context& ctx, array_wrapper<value_block_data>& item);
template rx_result start_runtime_arrayed<variable_block_data>(const string_type name, runtime::runtime_start_context& ctx, array_wrapper<variable_block_data>& item);


template<typename T>
rx_result start_runtime_plain(const string_type name, runtime::runtime_start_context& ctx, T& item)
{
	rx_result ret;
	ctx.path.push_to_path(name);
	ret = item.start_runtime(ctx);
	ctx.path.pop_from_path();
	return ret;
}
template rx_result start_runtime_plain<source_data>(const string_type name, runtime::runtime_start_context& ctx, source_data& item);
template rx_result start_runtime_plain<mapper_data>(const string_type name, runtime::runtime_start_context& ctx, mapper_data& item);
template rx_result start_runtime_plain<filter_data>(const string_type name, runtime::runtime_start_context& ctx, filter_data& item);
template rx_result start_runtime_plain<event_data>(const string_type name, runtime::runtime_start_context& ctx, event_data& item);



template<typename T>
rx_result stop_runtime_arrayed(runtime::runtime_stop_context& ctx, array_wrapper<T>& item)
{
	rx_result ret;
	if (item.is_array())
	{
		for (int i = 0; i < item.get_size(); i++)
		{
			ret = item.get_item(i)->stop_runtime(ctx);
			if (!ret)
				break;
		}
	}
	else
	{
		ret = item.get_item()->stop_runtime(ctx);
	}
	return ret;
}
template rx_result stop_runtime_arrayed<variable_data>(runtime::runtime_stop_context& ctx, array_wrapper<variable_data>& item);
template rx_result stop_runtime_arrayed<struct_data>(runtime::runtime_stop_context& ctx, array_wrapper<struct_data>& item);
template rx_result stop_runtime_arrayed<value_block_data>(runtime::runtime_stop_context& ctx, array_wrapper<value_block_data>& item);
template rx_result stop_runtime_arrayed<variable_block_data>(runtime::runtime_stop_context& ctx, array_wrapper<variable_block_data>& item);


template<typename T>
rx_result stop_runtime_plain(runtime::runtime_stop_context& ctx, T& item)
{
	rx_result ret;
	ret = item.stop_runtime(ctx);
	return ret;
}
template rx_result stop_runtime_plain<source_data>(runtime::runtime_stop_context& ctx, source_data& item);
template rx_result stop_runtime_plain<mapper_data>(runtime::runtime_stop_context& ctx, mapper_data& item);
template rx_result stop_runtime_plain<filter_data>(runtime::runtime_stop_context& ctx, filter_data& item);
template rx_result stop_runtime_plain<event_data>(runtime::runtime_stop_context& ctx, event_data& item);


rx_result browse_items_const(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<const_value_data>& item)
{
	if (this_item)
	{
		runtime_item_attribute one_attr;
		one_attr.name = name;
		if (!item.is_array())
		{
			one_attr.value = item.get_item()->get_value(ctx);
			one_attr.type = rx_attribute_type::const_attribute_type;
		}
		else
		{
			one_attr.type = rx_attribute_type::const_array_attribute_type;
		}
		if (prefix.empty())
			one_attr.full_path = name;
		else
			one_attr.full_path = prefix + RX_OBJECT_DELIMETER + name;

		items.emplace_back(std::move(one_attr));

		return true;
	}
	else
	{
		if (item.is_array() && path.empty())
		{
			char temp_buff[0x20];
			for (int i = 0; i < item.get_size(); i++)
			{
				runtime_item_attribute one_attr;
				sprintf(temp_buff, "[%d]", i);
				one_attr.name = name + temp_buff;
				one_attr.type = rx_attribute_type::const_attribute_type;
				one_attr.value = item.get_item(i)->get_value(ctx);
				if (prefix.empty())
					one_attr.full_path = one_attr.name;
				else
					one_attr.full_path = prefix + temp_buff;
				items.emplace_back(std::move(one_attr));
			}
			return true;
		}
		else
		{
			return RX_INVALID_PATH;
		}
	}
}

rx_result browse_items_value(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<value_data>& item)
{
	if (this_item)
	{
		runtime_item_attribute one_attr;

		one_attr.name = name;
		if (!item.is_array())
		{
			one_attr.value = item.get_item()->get_value(ctx);
			one_attr.type = rx_attribute_type::value_attribute_type;
		}
		else
		{
			one_attr.type = rx_attribute_type::value_array_attribute_type;
		}

		if (prefix.empty())
			one_attr.full_path = name;
		else
			one_attr.full_path = prefix + RX_OBJECT_DELIMETER + name;

		items.emplace_back(std::move(one_attr));

		return true;
	}
	else
	{
		if (item.is_array() && path.empty())
		{
			char temp_buff[0x20];
			for (int i = 0; i < item.get_size(); i++)
			{
				runtime_item_attribute one_attr;
				sprintf(temp_buff, "[%d]", i);
				one_attr.name = name + temp_buff;
				one_attr.type = rx_attribute_type::value_attribute_type;
				one_attr.value = item.get_item(i)->get_value(ctx);
				if (prefix.empty())
					one_attr.full_path = one_attr.name;
				else
					one_attr.full_path = prefix + temp_buff;
				items.emplace_back(std::move(one_attr));
			}
			return true;
		}
		else
		{
			return RX_INVALID_PATH;
		}
	}
}
rx_result browse_items_variable(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<variable_data>& item)
{
	if (this_item)
	{
		runtime_item_attribute one_attr;
		one_attr.name = name;
		if (!item.is_array())
		{
			one_attr.value = item.get_item()->get_value(ctx);
			one_attr.type = rx_attribute_type::variable_attribute_type;
		}
		else
		{
			one_attr.type = rx_attribute_type::variable_array_attribute_type;
		}

		if (prefix.empty())
			one_attr.full_path = name;
		else
			one_attr.full_path = prefix + RX_OBJECT_DELIMETER + name;

		items.emplace_back(std::move(one_attr));

		return true;
	}
	else
	{
		if (array_idx < 0)
		{
			if (item.is_array())
			{
				if (path.empty())
				{
					for (int i = 0; i < item.get_size(); i++)
					{
						char temp_buff[0x20];
						runtime_item_attribute one_attr;
						sprintf(temp_buff, "[%d]", i);
						one_attr.name = name + temp_buff;
						one_attr.type = rx_attribute_type::variable_attribute_type;
						one_attr.value = item.get_item(i)->get_value(ctx);
						if (prefix.empty())
							one_attr.full_path = one_attr.name;
						else
							one_attr.full_path = prefix + temp_buff;
						items.emplace_back(std::move(one_attr));
					}
					return true;
				}
				else
				{
					return RX_INVALID_PATH;
				}
			}
			else
			{
				return item.get_item()->browse_items(prefix, path, filter, items, ctx);
			}
		}
		else
		{
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			if (array_idx < item.get_size())
			{
				char temp_buff[0x20];
				sprintf(temp_buff, "[%d]", array_idx);
				string_type down_prefix = prefix + temp_buff;
				return item.get_item(array_idx)->browse_items(down_prefix, path, filter, items, ctx);
			}
			else
			{
				return RX_OUT_OF_BOUNDS;
			}
		}
	}
}

template<typename T>
rx_result browse_items_arrayed(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<T>& item)
{
	if (this_item)
	{
		runtime_item_attribute one_attr;
		one_attr.name = name;
		if (!item.is_array())
		{
			if constexpr (T::has_own_value)
				one_attr.value = item.get_item()->get_value(ctx);
			one_attr.type = T::plain_attribute_type;
		}
		else
		{
			one_attr.type = T::array_attribute_type;
		}

		if (prefix.empty())
			one_attr.full_path = name;
		else
			one_attr.full_path = prefix + RX_OBJECT_DELIMETER + name;

		items.emplace_back(std::move(one_attr));

		return true;
	}
	else
	{
		if (array_idx < 0)
		{
			if (item.is_array())
			{
				if (path.empty())
				{
					for (int i = 0; i < item.get_size(); i++)
					{
						char temp_buff[0x20];
						runtime_item_attribute one_attr;
						sprintf(temp_buff, "[%d]", i);
						one_attr.name = name + temp_buff;
						one_attr.type = T::plain_attribute_type;
						if constexpr (T::has_own_value)
							one_attr.value = item.get_item(i)->get_value(ctx);

						if (prefix.empty())
							one_attr.full_path = one_attr.name;
						else
							one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
						items.emplace_back(std::move(one_attr));
					}
					return true;
				}
				else
				{
					return RX_INVALID_PATH;
				}
			}
			else
			{
				return item.get_item()->browse_items(prefix, path, filter, items, ctx);
			}
		}
		else
		{
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			if (array_idx < item.get_size())
			{

				char temp_buff[0x20];
				sprintf(temp_buff, "[%d]", array_idx);
				string_type down_prefix = prefix + temp_buff;
				return item.get_item(array_idx)->browse_items(down_prefix, path, filter, items, ctx);
			}
			else
			{
				return RX_OUT_OF_BOUNDS;
			}
		}
	}
}
rx_result browse_items_blocks(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<value_block_data>& item)
{
	if (this_item)
	{
		runtime_item_attribute one_attr;
		one_attr.name = name;
		if (!item.is_array())
		{
			one_attr.value = item.get_item()->get_value(ctx);
			if(item.get_item()->struct_value.value_opt[opt_is_constant])
				one_attr.type = rx_attribute_type::const_data_attribute_type;
			else
				one_attr.type = rx_attribute_type::value_data_attribute_type;
		}
		else
		{
			if (item.get_item()->struct_value.value_opt[opt_is_constant])
				one_attr.type = rx_attribute_type::const_data_array_attribute_type;
			else
				one_attr.type = rx_attribute_type::value_data_array_attribute_type;
		}

		if (prefix.empty())
			one_attr.full_path = name;
		else
			one_attr.full_path = prefix + RX_OBJECT_DELIMETER + name;

		items.emplace_back(std::move(one_attr));

		return true;
	}
	else
	{
		if (array_idx < 0)
		{
			if (item.is_array())
			{
				if (path.empty())
				{
					for (int i = 0; i < item.get_size(); i++)
					{
						char temp_buff[0x20];
						runtime_item_attribute one_attr;
						sprintf(temp_buff, "[%d]", i);
						one_attr.name = name + temp_buff;
						if (item.get_item(i)->struct_value.value_opt[opt_is_constant])
							one_attr.type = rx_attribute_type::const_data_attribute_type;
						else
							one_attr.type = rx_attribute_type::value_data_attribute_type;
						one_attr.value = item.get_item(i)->get_value(ctx);

						if (prefix.empty())
							one_attr.full_path = one_attr.name;
						else
							one_attr.full_path = prefix + RX_OBJECT_DELIMETER + one_attr.name;
						items.emplace_back(std::move(one_attr));
					}
					return true;
				}
				else
				{
					return RX_INVALID_PATH;
				}
			}
			else
			{
				return item.get_item()->browse_items(prefix, path, filter, items, ctx);
			}
		}
		else
		{
			if (!item.is_array())
				return RX_NOT_AN_ARRAY;
			if (array_idx < item.get_size())
			{
				return item.get_item(array_idx)->browse_items(prefix, path, filter, items, ctx);
			}
			else
			{
				return RX_OUT_OF_BOUNDS;
			}
		}
	}
}
template rx_result browse_items_arrayed<struct_data>(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<struct_data>& item);
template rx_result browse_items_arrayed<variable_block_data>(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const array_wrapper<variable_block_data>& item);


template<typename T>
rx_result browse_items_plain(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const T& item)
{
	if (this_item)
	{
		runtime_item_attribute one_attr;
		one_attr.name = name;

		one_attr.type = T::plain_attribute_type;

		if constexpr (T::has_own_value)
			one_attr.value = item.get_value(ctx);

		if (prefix.empty())
			one_attr.full_path = name;
		else
			one_attr.full_path = prefix + RX_OBJECT_DELIMETER + name;

		items.emplace_back(std::move(one_attr));

		return true;
	}
	else
	{
		if (array_idx < 0)
		{
			return item.browse_items(prefix, path, filter, items, ctx);
		}
		else
		{
			return RX_INVALID_PATH;
		}
	}
}
template rx_result browse_items_plain<source_data>(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const source_data& item);
template rx_result browse_items_plain<mapper_data>(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const mapper_data& item);
template rx_result browse_items_plain<filter_data>(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const filter_data& item);
template rx_result browse_items_plain<event_data>(bool this_item, const string_type& name, int array_idx, const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx, const event_data& item);


const runtime_item* get_child_item_variable(int array_idx, string_view_type path, const array_wrapper<variable_data>& item)
{
	if (array_idx < 0)
	{
		if (!item.is_array())
		{
			return item.get_item()->get_child_item(path);
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		if (!item.is_array())
		{
			return nullptr;
		}
		else if (array_idx < item.get_size())
		{
			return item.get_item(array_idx)->get_child_item(path);
		}
		else
		{
			return nullptr;
		}
	}
}
template<typename T>
const runtime_item* get_child_item_arrayed(int array_idx, string_view_type path, const array_wrapper<T>& item)
{
	if (array_idx < 0)
	{
		if (!item.is_array())
		{
			return item.get_item()->get_child_item(path);
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		if (!item.is_array())
		{
			return nullptr;
		}
		else if (array_idx < item.get_size())
		{
			return item.get_item(array_idx)->get_child_item(path);
		}
		else
		{
			return nullptr;
		}
	}
}
template const runtime_item* get_child_item_arrayed<struct_data>(int array_idx, const string_view_type path, const array_wrapper<struct_data>& item);
template const runtime_item* get_child_item_arrayed<variable_block_data>(int array_idx, const string_view_type path, const array_wrapper<variable_block_data>& item);
template const runtime_item* get_child_item_arrayed<value_block_data>(int array_idx, const string_view_type path, const array_wrapper<value_block_data>& item);

template<typename T>
const runtime_item* get_child_item_plain(int array_idx, string_view_type path, const T& item)
{
	if (array_idx < 0)
	{
		return item.get_child_item(path);
	}
	else
	{
		return nullptr;
	}
}
template const runtime_item* get_child_item_plain<source_data>(int array_idx, const string_view_type path, const source_data& item);
template const runtime_item* get_child_item_plain<mapper_data>(int array_idx, const string_view_type path, const mapper_data& item);
template const runtime_item* get_child_item_plain<filter_data>(int array_idx, const string_view_type path, const filter_data& item);
template const runtime_item* get_child_item_plain<event_data>(int array_idx, const string_view_type path, const event_data& item);


void object_state_changed(runtime_process_context* ctx);








} // namespace structure
} // namespace runtime
} // namespace rx_platform

