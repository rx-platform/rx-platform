

/****************************************************************************
*
*  system\meta\rx_def_blocks.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_def_blocks
#include "system/meta/rx_def_blocks.h"

#include "system/meta/rx_obj_types.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/runtime/rx_objbase.h"
#include "lib/rx_rt_data.h"
#include "rx_meta_attr_algorithm.h"
#include "rx_meta_algorithm.h"
using namespace rx_platform::meta::meta_algorithm;


namespace rx_platform {
bool rx_is_valid_item_name(const string_type& name)
{
	if (name.empty() || name== RX_DEFAULT_VARIABLE_NAME)
		return false;
	for (const auto& one : name)
	{
		if (!((one >= 'a' && one <= 'z')
			|| (one >= 'A' && one <= 'Z')
			|| (one >= '0' && one <= '9')
			|| one == '_'))
			return false;
	}
	return true;
}

namespace meta {

namespace def_blocks {

// Class rx_platform::meta::def_blocks::complex_data_type 


rx_result complex_data_type::register_struct (const string_type& name, const rx_node_id& id)
{
	auto ret = check_name(name, (static_cast<int>(structs_.size() | structs_mask)));
	if (ret)
	{
		structs_.emplace_back(struct_attribute(name, id));
	}
	return ret;
}

rx_result complex_data_type::register_variable (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only, bool persistent)
{
	auto ret = check_name(name, (static_cast<int>(variables_.size() | variables_mask)));
	if(ret)
	{
		variables_.emplace_back(variable_attribute(name, id, std::move(value), read_only));
	}
	return ret;
}

rx_result complex_data_type::register_event (const def_blocks::event_attribute& what)
{
	auto ret = check_name(what.get_name(), (static_cast<int>(events_.size() | complex_data_type::events_mask)));
	if (ret)
	{
		events_.emplace_back(what);
	}
	return ret;
}

rx_result complex_data_type::register_simple_value (const string_type& name, rx_simple_value&& val, bool read_only, bool persistent)
{
	auto ret = check_name(name, (static_cast<int>(simple_values_.size() | simple_values_mask)));
	if (ret)
	{
		simple_values_.emplace_back(name, std::move(val), read_only, persistent);
	}
	return ret;
}

rx_result complex_data_type::register_const_value (const string_type& name, rx_simple_value&& val, bool config_only)
{
	auto ret = check_name(name, (static_cast<int>(const_values_.size() | const_values_mask)));
	if (ret)
	{
		const_values_.emplace_back(name, std::move(val), config_only);
	}
	return ret;
}

rx_result complex_data_type::register_simple_value (const string_type& name, const rx_simple_value& val, bool read_only, bool persistent)
{
	auto ret = check_name(name, (static_cast<int>(simple_values_.size() | simple_values_mask)));
	if(ret)
	{
		simple_values_.emplace_back(name, val, read_only, persistent);
	}
	return ret;
}

rx_result complex_data_type::register_const_value (const string_type& name, const rx_simple_value& val, bool config_only)
{
	auto ret = check_name(name, (static_cast<int>(const_values_.size() | const_values_mask)));
	if(ret)
	{
		const_values_.emplace_back(name, val, config_only);
	}
	return ret;
}

rx_result complex_data_type::check_name (const string_type& name, int rt_index)
{
	if (!rx_is_valid_item_name(name))
		return name + "is invalid item name!";

	for (const auto& one : names_cache_)
	{
		if(one.first == name)
			return name + " already exists!";
	}
	if(rt_index)
		names_cache_.emplace_back(name, rt_index);
	return true;
}


// Class rx_platform::meta::def_blocks::const_value_def 

const_value_def::const_value_def (const string_type& name, rx_simple_value&& value, bool config_only)
	: name_(name)
	, storage_(std::move(value))
	, config_only_(config_only)
{
}

const_value_def::const_value_def (const string_type& name, const rx_simple_value& value, bool config_only)
	: name_(name)
	, storage_(value)
	, config_only_(config_only)
{
}



rx_result const_value_def::serialize_definition (base_meta_writer& stream) const
{
	if (!stream.write_string("name", name_.c_str()))
		return stream.get_error();
	if (!storage_.serialize("value", stream))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", description_.c_str()))
			return stream.get_error();
		if (!stream.write_bool("config", config_only_))
			return stream.get_error();
	}
	return true;
}

rx_result const_value_def::deserialize_definition (base_meta_reader& stream)
{
	if (!stream.read_string("name", name_))
		return stream.get_error();
	if (!storage_.deserialize("value", stream))
		return stream.get_error();
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", description_))
			return stream.get_error();
		if (!stream.read_bool("config", config_only_))
			return stream.get_error();
}
	else
	{
		config_only_ = false;
	}
	return true;
}

rx_simple_value const_value_def::get_value () const
{
	return storage_;
}


// Class rx_platform::meta::def_blocks::event_attribute 

event_attribute::event_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_(id)
{
}

event_attribute::event_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_(target_name)
{
}



// Class rx_platform::meta::def_blocks::filter_attribute 

filter_attribute::filter_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_(id)
{
}

filter_attribute::filter_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_(target_name)
{
}



// Class rx_platform::meta::def_blocks::mapped_data_type 

mapped_data_type::mapped_data_type()
{
}


mapped_data_type::~mapped_data_type()
{
}



rx_result mapped_data_type::register_mapper (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	auto ret = complex_data.check_name(name, (static_cast<int>(mappers_.size() | complex_data_type::mappings_mask)));
	if(ret)
	{
		mappers_.emplace_back(name, id);
	}
	return ret;
}

rx_result mapped_data_type::register_source (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	auto ret = complex_data.check_name(name, (static_cast<int>(sources_.size() | complex_data_type::sources_mask)));
	if (ret)
	{
		sources_.emplace_back(name, id);
	}
	return ret;
}


// Class rx_platform::meta::def_blocks::mapper_attribute 

mapper_attribute::mapper_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_(id)
{
}

mapper_attribute::mapper_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_(target_name)
{
}



// Class rx_platform::meta::def_blocks::simple_value_def 

simple_value_def::simple_value_def (const string_type& name, rx_simple_value&& value, bool read_only, bool persistent)
	: name_(name)
	, storage_(std::move(value))
	, read_only_(read_only)
	, persistent_(persistent)
{
}

simple_value_def::simple_value_def (const string_type& name, const rx_simple_value& value, bool read_only, bool persistent)
	: name_(name)
	, storage_(value)
	, read_only_(read_only)
	, persistent_(persistent)
{
}



rx_result simple_value_def::serialize_definition (base_meta_writer& stream) const
{
	if (!stream.write_string("name", name_.c_str()))
		return stream.get_error();
	if (!stream.write_bool("ro", read_only_))
		return stream.get_error();
	if (!storage_.serialize("value", stream))
		return stream.get_error();
	if (stream.get_version() >= RX_PERSISTENCE_VERSION)
	{
		if (!stream.write_bool("persist", persistent_))
			return stream.get_error();
	}
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.write_string("description", description_.c_str()))
			return stream.get_error();
	}
	return true;
}

rx_result simple_value_def::deserialize_definition (base_meta_reader& stream)
{
	if (!stream.read_string("name", name_))
		return stream.get_error();
	if (!stream.read_bool("ro", read_only_))
		return stream.get_error();
	if (!storage_.deserialize("value", stream))
		return stream.get_error();
	if (stream.get_version() >= RX_PERSISTENCE_VERSION)
	{
		if (!stream.read_bool("persist", persistent_))
			return stream.get_error();
	}
	if (stream.get_version() >= RX_DESCRIPTIONS_VERSION)
	{
		if (!stream.read_string("description", description_))
			return stream.get_error();
	}
	return true;
}

rx_timed_value simple_value_def::get_value (rx_time now) const
{
	return rx_timed_value(storage_, now);
}


// Class rx_platform::meta::def_blocks::source_attribute 

source_attribute::source_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_(id)
{
}

source_attribute::source_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_(target_name)
{
}



// Class rx_platform::meta::def_blocks::struct_attribute 

struct_attribute::struct_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_(id)
{
}

struct_attribute::struct_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_(target_name)
{
}



// Class rx_platform::meta::def_blocks::variable_attribute 

variable_attribute::variable_attribute (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only)
      : name_(name)
	, target_(id)
	, storage_(std::move(value))
	, read_only_(read_only)
{
}

variable_attribute::variable_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_(target_name)
	, read_only_(false)
{
}



rx_value variable_attribute::get_value (rx_time now) const
{
	return rx_value(storage_, now);
}


// Class rx_platform::meta::def_blocks::variable_data_type 


rx_result variable_data_type::register_filter (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	auto ret = complex_data.check_name(name, (static_cast<int>(filters_.size() | complex_data_type::filters_mask)));
	if (ret)
	{
		filters_.emplace_back(name, id);
	}
	return ret;
}


// Class rx_platform::meta::def_blocks::io_attribute 


// Class rx_platform::meta::def_blocks::filtered_data_type 


rx_result filtered_data_type::register_filter (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	auto ret = complex_data.check_name(name, (static_cast<int>(filters_.size() | complex_data_type::filters_mask)));
	if (ret)
	{
		filters_.emplace_back(name, id);
	}
	return ret;
}


// Class rx_platform::meta::def_blocks::data_attribute 

data_attribute::data_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_(id)
{
}

data_attribute::data_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_(target_name)
{
}



// Class rx_platform::meta::def_blocks::method_attribute 

method_attribute::method_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_(id)
{
}

method_attribute::method_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_(target_name)
{
}



// Class rx_platform::meta::def_blocks::program_attribute 

program_attribute::program_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
{
}

program_attribute::program_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
{
}



// Class rx_platform::meta::def_blocks::display_attribute 

display_attribute::display_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_(id)
{
}

display_attribute::display_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_(target_name)
{
}



// Class rx_platform::meta::def_blocks::data_type_def 


rx_result data_type_def::register_child (const string_type& name, const rx_item_reference& ref)
{
	auto ret = check_name(name, (static_cast<int>(children_.size() | child_values_mask)));
	if (!ret)
		return ret;
	if (ref.is_node_id())
		children_.emplace_back(data_attribute(name, ref.get_node_id()));
	else
		children_.emplace_back(data_attribute(name, ref.get_path()));
	return true;
}

rx_result data_type_def::register_value (const string_type& name, rx_simple_value&& val)
{
	auto ret = check_name(name, (static_cast<int>(values_.size() | simple_values_mask)));
	if (!ret)
		return ret;
	values_.emplace_back(name, std::move(val), false);
	return true;
}

rx_result data_type_def::register_value (const string_type& name, const rx_simple_value& val)
{
	auto ret = check_name(name, (static_cast<int>(values_.size() | simple_values_mask)));
	if (!ret)
		return ret;
	values_.emplace_back(name, val, false);
	return true;
}

rx_result data_type_def::check_name (const string_type& name, int rt_index)
{
	if (!rx_is_valid_item_name(name))
		return name + "is invalid item name!";

	for (const auto& one : names_cache_)
	{
		if (one.first == name)
			return name + " already exists!";
	}
	if (rt_index)
		names_cache_.emplace_back(name, rt_index);
	return true;
}


} // namespace def_blocks
} // namespace meta
} // namespace rx_platform

