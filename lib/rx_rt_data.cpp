

/****************************************************************************
*
*  lib\rx_rt_data.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_rt_data
#include "lib/rx_rt_data.h"



namespace rx {

namespace data {

// Class rx::data::runtime_value 


// Class rx::data::runtime_values_data 


void runtime_values_data::add_value (const string_type& name, const rx_simple_value& value)
{
	if (name.empty())
		return;
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		values.insert(std::make_pair(name, runtime_value{ value }));
	}
	else
	{
		auto child_it = children.find(name.substr(0, idx));
		if (child_it != children.end())
		{
			child_it->second.add_value(name.substr(idx + 1), value);
		}
		else
		{
			auto& vals = add_child(name.substr(0, idx));
			vals.add_value(name.substr(idx + 1), value);
		}
	}
}

runtime_values_data& runtime_values_data::add_child (const string_type& name)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		return children.emplace(name, data::runtime_values_data()).first->second;
	}
	else
	{
		auto child_it = children.find(name.substr(0, idx));
		if (child_it != children.end())
		{
			return child_it->second.add_child(name.substr(idx + 1));
		}
		else
		{
			auto& vals = add_child(name.substr(0, idx));
			return vals.add_child(name.substr(idx + 1));
		}
	}
}

runtime_values_data& runtime_values_data::add_child (const string_type& name, runtime_values_data&& data)
{
	auto idx = name.find(".");

	if (idx == string_type::npos)
	{// our value
		return children.emplace(name, std::move(data)).first->second;
	}
	else
	{
		auto child_it = children.find(name.substr(0, idx));
		if (child_it != children.end())
		{
			return child_it->second.add_child(name.substr(idx + 1), std::move(data));
		}
		else
		{
			auto& vals = add_child(name.substr(0, idx));
			return vals.add_child(name.substr(idx + 1), std::move(data));
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
		auto val_it = values.find(path);
		if (val_it != values.end())
		{
			return val_it->second.value;
		}
	}
	else
	{
		auto child_it = children.find(path.substr(0, idx));
		if (child_it != children.end())
		{
			return child_it->second.get_value(path.substr(idx + 1));
		}
	}
	return rx_simple_value();
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


} // namespace data
} // namespace rx

