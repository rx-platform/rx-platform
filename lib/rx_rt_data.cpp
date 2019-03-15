

/****************************************************************************
*
*  lib\rx_rt_data.cpp
*
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

#include "rx_configuration.h"


namespace rx {

namespace data {

// Class rx::data::runtime_value 


// Class rx::data::runtime_values_data 


void runtime_values_data::add_value (const string_type& name, const rx_simple_value& value)
{
	values.insert(std::make_pair(name, runtime_value{ value }));
}

runtime_values_data& runtime_values_data::add_child (const string_type& name)
{
	return children.emplace(name, data::runtime_values_data()).first->second;
}

rx_simple_value runtime_values_data::get_value (const string_type& path) const
{
	if (path.empty())
		return rx_simple_value();
	auto idx = path.find(RX_OBJECT_DELIMETER);
	
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


} // namespace data
} // namespace rx

