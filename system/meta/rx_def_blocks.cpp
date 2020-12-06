

/****************************************************************************
*
*  system\meta\rx_def_blocks.cpp
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


// rx_def_blocks
#include "system/meta/rx_def_blocks.h"

#include "system/meta/rx_obj_types.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/runtime/rx_objbase.h"
#include "lib/rx_rt_data.h"
#include "rx_meta_attr_algorithm.h"
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

rx_result complex_data_type::register_variable (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only)
{
	auto ret = check_name(name, (static_cast<int>(variables_.size() | variables_mask)));
	if(ret)
	{
		variables_.emplace_back(variable_attribute(name, id, std::move(value), read_only));
	}
	return ret;
}

rx_result complex_data_type::register_event (const string_type& name, const rx_node_id& id)
{
	auto ret = check_name(name, (static_cast<int>(events_.size() | complex_data_type::events_mask)));
	if (ret)
	{
		events_.emplace_back(name, id);
	}
	return ret;
}

rx_result complex_data_type::register_simple_value (const string_type& name, bool read_only, rx_simple_value&& val)
{
	auto ret = check_name(name, (static_cast<int>(simple_values_.size() | simple_values_mask)));
	if (ret)
	{
		simple_values_.emplace_back(name, read_only, std::move(val));
	}
	return ret;
}

rx_result complex_data_type::register_const_value (const string_type& name, rx_simple_value&& val)
{
	auto ret = check_name(name, (static_cast<int>(const_values_.size() | const_values_mask)));
	if (ret)
	{
		const_values_.emplace_back(name, std::move(val));
	}
	return ret;
}

rx_result complex_data_type::register_simple_value (const string_type& name, bool read_only, const rx_simple_value& val)
{
	auto ret = check_name(name, (static_cast<int>(simple_values_.size() | simple_values_mask)));
	if(ret)
	{
		simple_values_.emplace_back(name, read_only, val);
	}
	return ret;
}

rx_result complex_data_type::register_const_value (const string_type& name, const rx_simple_value& val)
{
	auto ret = check_name(name, (static_cast<int>(const_values_.size() | const_values_mask)));
	if(ret)
	{
		const_values_.emplace_back(name, val);
	}
	return ret;
}

rx_result complex_data_type::check_name (const string_type& name, int rt_index)
{
	if (!rx_is_valid_item_name(name))
		return name + "is invalid item name!";

	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
	{
		if(rt_index)
			names_cache_.emplace(name, rt_index);
		return true;
	}
	else
	{
		return name + " already exists!";
	}
}


// Class rx_platform::meta::def_blocks::const_value_def 

const_value_def::const_value_def (const string_type& name, rx_simple_value&& value)
	: name_(name)
	, storage_(std::move(value))
{
}

const_value_def::const_value_def (const string_type& name, const rx_simple_value& value)
	: name_(name)
	, storage_(value)
{
}



rx_result const_value_def::serialize_definition (base_meta_writer& stream) const
{
	if (!stream.write_string("name", name_.c_str()))
		return false;
	if (!storage_.serialize("value", stream))
		return false;
	return true;
}

rx_result const_value_def::deserialize_definition (base_meta_reader& stream)
{
	if (!stream.read_string("name", name_))
		return false;
	if (!storage_.deserialize("value", stream))
		return false;
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



rx_result event_attribute::serialize_definition (base_meta_writer& stream) const
{
	auto temp = meta_blocks_algorithm<event_attribute>::serialize_complex_attribute(*this, stream);
	return temp;
}

rx_result event_attribute::deserialize_definition (base_meta_reader& stream)
{
	auto temp = meta_blocks_algorithm<event_attribute>::deserialize_complex_attribute(*this, stream);
	return temp;
}

rx_result event_attribute::check (type_check_context& ctx)
{
	auto temp = meta_blocks_algorithm<event_attribute>::check_complex_attribute(*this, ctx);
	return temp;
}

rx_result event_attribute::construct (construct_context& ctx) const
{
	return meta_blocks_algorithm<event_attribute>::construct_complex_attribute(*this, ctx);
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



rx_result filter_attribute::serialize_definition (base_meta_writer& stream) const
{
	return meta_blocks_algorithm<filter_attribute>::serialize_complex_attribute(*this, stream);
}

rx_result filter_attribute::deserialize_definition (base_meta_reader& stream)
{
	return meta_blocks_algorithm<filter_attribute>::deserialize_complex_attribute(*this, stream);
}

rx_result filter_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<filter_attribute>::check_complex_attribute(*this, ctx);
}

rx_result filter_attribute::construct (construct_context& ctx) const
{
	return meta_blocks_algorithm<filter_attribute>::construct_complex_attribute(*this, ctx);
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



rx_result mapper_attribute::serialize_definition (base_meta_writer& stream) const
{
	return meta_blocks_algorithm<mapper_attribute>::serialize_complex_attribute(*this, stream);
}

rx_result mapper_attribute::deserialize_definition (base_meta_reader& stream)
{
	return meta_blocks_algorithm<mapper_attribute>::deserialize_complex_attribute(*this, stream);
}

rx_result mapper_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<mapper_attribute>::check_complex_attribute(*this, ctx);
}

rx_result mapper_attribute::construct (construct_context& ctx) const
{
	return meta_blocks_algorithm<mapper_attribute>::construct_complex_attribute(*this, ctx);
}


// Class rx_platform::meta::def_blocks::simple_value_def 

simple_value_def::simple_value_def (const string_type& name, bool read_only, rx_simple_value&& value)
	: name_(name)
	, storage_(std::move(value))
	, read_only_(read_only)
{
}

simple_value_def::simple_value_def (const string_type& name, bool read_only, const rx_simple_value& value)
	: name_(name)
	, storage_(value)
	, read_only_(read_only)
{
}



rx_result simple_value_def::serialize_definition (base_meta_writer& stream) const
{
	if (!stream.write_string("name", name_.c_str()))
		return false;
	if (!stream.write_bool("ro", read_only_))
		return false;
	if (!storage_.serialize("value", stream))
		return false;
	return true;
}

rx_result simple_value_def::deserialize_definition (base_meta_reader& stream)
{
	if (!stream.read_string("name", name_))
		return false;
	if (!stream.read_bool("ro", read_only_))
		return false;
	if (!storage_.deserialize("value", stream))
		return false;
	return true;
}

rx_timed_value simple_value_def::get_value (rx_time now) const
{
	return rx_timed_value::from_simple(storage_, now);
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



rx_result source_attribute::serialize_definition (base_meta_writer& stream) const
{
	return meta_blocks_algorithm<source_attribute>::serialize_complex_attribute(*this, stream);
}

rx_result source_attribute::deserialize_definition (base_meta_reader& stream)
{
	return meta_blocks_algorithm<source_attribute>::deserialize_complex_attribute(*this, stream);
}

rx_result source_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<source_attribute>::check_complex_attribute(*this, ctx);
}

rx_result source_attribute::construct (construct_context& ctx) const
{
	return meta_blocks_algorithm<source_attribute>::construct_complex_attribute(*this, ctx);
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



rx_result struct_attribute::serialize_definition (base_meta_writer& stream) const
{
	return meta_blocks_algorithm<struct_attribute>::serialize_complex_attribute(*this, stream);
}

rx_result struct_attribute::deserialize_definition (base_meta_reader& stream)
{
	return meta_blocks_algorithm<struct_attribute>::deserialize_complex_attribute(*this, stream);
}

rx_result struct_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<struct_attribute>::check_complex_attribute(*this, ctx);
}

rx_result struct_attribute::construct (construct_context& ctx) const
{
	return meta_blocks_algorithm<struct_attribute>::construct_complex_attribute(*this, ctx);
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



rx_result variable_attribute::serialize_definition (base_meta_writer& stream) const
{
	auto temp = meta_blocks_algorithm<variable_attribute>::serialize_complex_attribute(*this, stream);
	return temp;
}

rx_result variable_attribute::deserialize_definition (base_meta_reader& stream)
{
	auto temp = meta_blocks_algorithm<variable_attribute>::deserialize_complex_attribute(*this, stream);
	return temp;
}

rx_value variable_attribute::get_value (rx_time now) const
{
	return rx_value::from_simple(storage_, now);
}

rx_result variable_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<variable_attribute>::check_complex_attribute(*this, ctx);
}

rx_result variable_attribute::construct (construct_context& ctx) const
{
	return meta_blocks_algorithm<variable_attribute>::construct_complex_attribute(*this, ctx);
}


// Class rx_platform::meta::def_blocks::variable_data_type 


rx_result variable_data_type::register_source (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	auto ret = complex_data.check_name(name, (static_cast<int>(sources_.size() | complex_data_type::sources_mask)));
	if (ret)
	{
		sources_.emplace_back(name, id);
	}
	return ret;
}

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


} // namespace def_blocks
} // namespace meta
} // namespace rx_platform

