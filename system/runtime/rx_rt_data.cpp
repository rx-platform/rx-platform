

/****************************************************************************
*
*  system\runtime\rx_rt_data.cpp
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_rt_data
#include "system/runtime/rx_rt_data.h"



namespace rx_platform {

namespace runtime {

namespace data {

// Class rx_platform::runtime::data::runtime_value 


bool runtime_value::serialize (base_meta_writer& stream, const string_type& name) const
{
	if (!value.serialize_value(stream, name))
		return false;
	return true;
}

bool runtime_value::deserialize (base_meta_reader& stream)
{
	return false;
}


// Class rx_platform::runtime::data::runtime_values_data 


bool runtime_values_data::serialize (base_meta_writer& stream, const string_type& name) const
{
	if (!stream.start_object(name.c_str()))
		return false;
	for (const auto& one : children)
	{
		if (!one.second.serialize(stream, one.first))
			return false;
	}
	for (const auto& one : values)
	{
		if (!one.second.serialize(stream, one.first))
			return false;
	}
	if (!stream.end_object())
		return false;
	return true;
}

bool runtime_values_data::deserialize (base_meta_reader& stream)
{
	return false;
}

void runtime_values_data::add_value (const string_type& name, const rx_simple_value& value)
{
	values.insert(std::make_pair(name, runtime_value{ value }));
}

runtime_values_data& runtime_values_data::add_child (const string_type& name)
{
	return children.emplace(name, data::runtime_values_data()).first->second;
}


} // namespace data
} // namespace runtime
} // namespace rx_platform

