

/****************************************************************************
*
*  system\meta\rx_def_blocks.cpp
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


// rx_def_blocks
#include "system/meta/rx_def_blocks.h"

#include "system/meta/rx_obj_types.h"
#include "sys_internal/rx_internal_ns.h"
#include "system/constructors/rx_construct.h"
#include "system/runtime/rx_objbase.h"
#include "lib/rx_rt_data.h"
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

complex_data_type::complex_data_type()
{
}

complex_data_type::complex_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



rx_result complex_data_type::serialize_complex_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.write_bool("sealed", sealed_))
		return false;

	if (!stream.write_bool("abstract", abstract_))
		return false;

	if (!stream.start_array("items", names_cache_.size()))
		return false;
	for (const auto& one : names_cache_)
	{
		switch (one.second&type_mask)
		{
		case structs_mask:
		{

			if (!stream.start_object("item"))
				return false;
			if (!stream.write_string("type",basic_types::struct_type::type_name.c_str()))
				return false;
			if (!structs_[one.second&index_mask].serialize_definition(stream,type))
				return false;
			if (!stream.end_object())
				return false;
		}
		break;
		case variables_mask:
		{

			if (!stream.start_object("item"))
				return false;
			if (!stream.write_string("type", basic_types::variable_type::type_name.c_str()))
				return false;
			if (!variables_[one.second&index_mask].serialize_definition(stream, type))
				return false;
			if (!stream.end_object())
				return false;
		}
		break;
		case simple_values_mask:
		{
			if (!stream.start_object("item"))
				return false;
			if (!stream.write_string("type", simple_value_def::type_name.c_str()))
				return false;
			if (!simple_values_[one.second&index_mask].serialize_definition(stream, type))
				return false;
			if (!stream.end_object())
				return false;
		}
		break;
		case const_values_mask:
		{
			if (!stream.start_object("item"))
				return false;
			if (!stream.write_string("type", const_value_def::type_name.c_str()))
				return false;
			if (!const_values_[one.second&index_mask].serialize_definition(stream, type))
				return false;
			if (!stream.end_object())
				return false;
		}
		break;

		}
	}
	if (!stream.end_array())
		return false;

	return true;
}

rx_result complex_data_type::deserialize_complex_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_bool("sealed", sealed_))
		return false;

	if (!stream.read_bool("abstract", abstract_))
		return false;

	if (!stream.start_array("items"))
		return false;

	while(!stream.array_end())	
	{
		if (!stream.start_object("item"))
			return false;
		string_type item_type;
		if (!stream.read_string("type", item_type))
			return false;
		if (item_type == RX_CONST_VALUE_TYPE_NAME)
		{
			const_value_def temp;
			if (!temp.deserialize_definition(stream, type))
				return false;
			auto ret = check_name(temp.get_name(), (static_cast<int>(const_values_.size() | const_values_mask)));
			if (ret)
			{
				const_values_.emplace_back(std::move(temp));
			}
		}
		else if (item_type == RX_VALUE_TYPE_NAME)
		{
			simple_value_def temp;
			if (!temp.deserialize_definition(stream, type))
				return false;
			auto ret = check_name(temp.get_name(), (static_cast<int>(simple_values_.size() | simple_values_mask)));
			if (ret)
			{
				simple_values_.emplace_back(std::move(temp));
			}
		}
		else if (item_type == RX_CPP_STRUCT_CLASS_TYPE_NAME)
		{
			meta::def_blocks::struct_attribute temp;
			if (!temp.deserialize_definition(stream, type))
				return false;
			auto ret = check_name(temp.get_name(), (static_cast<int>(structs_.size() | structs_mask)));
			if (ret)
			{
				structs_.emplace_back(std::move(temp));
			}
		}
		else if (item_type == RX_CPP_VARIABLE_CLASS_TYPE_NAME)
		{
			meta::def_blocks::variable_attribute temp;
			if (!temp.deserialize_definition(stream, type))
				return false;
			auto ret = check_name(temp.get_name(), (static_cast<int>(variables_.size() | variables_mask)));
			if (ret)
			{
				variables_.emplace_back(std::move(temp));
			}
		}
		else
			return item_type + "is unknown type!";

		if (!stream.end_object())
			return false;
	}
	return true;//!!!! NOT DONE
}

rx_result complex_data_type::register_struct (const string_type& name, const rx_node_id& id)
{
	auto ret = check_name(name, (static_cast<int>(variables_.size() | structs_mask)));
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

rx_result complex_data_type::check_name (const string_type& name, int rt_index)
{
	if (!rx_is_valid_item_name(name))
		return name + "is invalid item name!";

	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
	{
		names_cache_.emplace(name, rt_index);
		return true;
	}
	else
	{
		return name + " already exists!";
	}
}

rx_result complex_data_type::construct (construct_context& ctx) const
{
	for (const auto& one : names_cache_)
	{
		switch (one.second&type_mask)
		{
		// constant values
		case const_values_mask:
		{
			ctx.runtime_data.add_const_value(
				one.first,
				const_values_[one.second&index_mask].get_value());
		}
		break;
		// simple values
		case simple_values_mask:
		{
			ctx.runtime_data.add_value(
				one.first,
				simple_values_[one.second&index_mask].get_value(ctx.now));
		}
		break;
		// structures
		case structs_mask:
		{
			rx_result ret = structs_[one.second&index_mask].construct(ctx);
			if (!ret)
			{
				ret.register_error("Unable to create struct "s +one.first + "!");
				return ret;
			}
			break;
		}
		// variables
		case variables_mask:
			rx_result ret = variables_[one.second&index_mask].construct(ctx);
			if (!ret)
			{
				ret.register_error("Unable to create variable "s + one.first + "!");
				return ret;
			}
			break;
		}
	}
	return true;
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

bool complex_data_type::check_type (type_check_context& ctx)
{
	bool ret = true;
	for (auto& one : structs_)
		ret &= one.check(ctx);
	for (auto& one : variables_)
		ret &= one.check(ctx);
	return ret;
}

rx_result complex_data_type::resolve (rx_directory_ptr dir)
{
	for (auto& one : structs_)
	{
		auto result = one.resolve(dir);
		if (!result)
			return result;
	}
	for (auto& one : variables_)
	{
		auto result = one.resolve(dir);
		if (!result)
			return result;
	}
	return true;
}


// Class rx_platform::meta::def_blocks::const_value_def 

string_type const_value_def::type_name = RX_CONST_VALUE_TYPE_NAME;

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



rx_result const_value_def::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.write_string("name", name_.c_str()))
		return false;
	if (!storage_.serialize(stream))
		return false;
	return true;
}

rx_result const_value_def::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_string("name", name_))
		return false;
	if (!storage_.deserialize(stream))
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
	, target_id_(id)
{
}

event_attribute::event_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_name_(target_name)
{
}



rx_result event_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	auto temp = meta_blocks_algorithm<event_attribute>::serialize_complex_attribute(*this, stream);
	return temp;
}

rx_result event_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
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

rx_result event_attribute::resolve (rx_directory_ptr dir)
{
	return meta_blocks_algorithm<event_attribute>::resolve_complex_attribute(*this, dir);
}


// Class rx_platform::meta::def_blocks::filter_attribute 

filter_attribute::filter_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_id_(id)
{
}

filter_attribute::filter_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_name_(target_name)
{
}



rx_result filter_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return meta_blocks_algorithm<filter_attribute>::serialize_complex_attribute(*this, stream);
}

rx_result filter_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
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

rx_result filter_attribute::resolve (rx_directory_ptr dir)
{
	return meta_blocks_algorithm<filter_attribute>::resolve_complex_attribute(*this, dir);
}


// Class rx_platform::meta::def_blocks::mapped_data_type 

mapped_data_type::mapped_data_type()
{
}

mapped_data_type::mapped_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



rx_result mapped_data_type::serialize_mapped_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_array("mappers", mappers_.size()))
		return false;
	for (const auto& one : mappers_)
	{
		if (!stream.start_object("item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;
	return true;
}

rx_result mapped_data_type::deserialize_mapped_definition (base_meta_reader& stream, uint8_t type, complex_data_type& complex_data)
{
	if (!stream.start_array("mappers"))
		return false;
	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return false;
		string_type item_type;
		if (!stream.read_string("type", item_type))
			return false;

		if (item_type != RX_CPP_MAPPER_CLASS_TYPE_NAME)
			return item_type + " is wrong item type for a mapper!";

		mapper_attribute temp;
		if (!temp.deserialize_definition(stream, type))
			return false;
		auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(mappers_.size() | complex_data_type::mappings_mask)));
		if (ret)
		{
			mappers_.emplace_back(std::move(temp));
		}
		else
			return ret;

		if (!stream.end_object())
			return false;
	}
	return true;
}

rx_result mapped_data_type::construct (const names_cahce_type& names, construct_context& ctx) const
{
	for (const auto& one : names)
	{
		switch (one.second&complex_data_type::type_mask)
		{
		// mappers
		case complex_data_type::mappings_mask:
		{
			rx_result ret = mappers_[one.second&complex_data_type::index_mask].construct(ctx);
			if (!ret)
			{
				ret.register_error("Unable to create mapper "s + one.first + "!");
				return ret;
			}
			break;
		}
		}
	}
	return true;
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

bool mapped_data_type::check_type (type_check_context& ctx)
{
	bool ret = true;
	for (auto& one : mappers_)
	{
		ret &= one.check(ctx);
	}
	return ret;
}

rx_result mapped_data_type::resolve (rx_directory_ptr dir)
{
	for (auto& one : mappers_)
	{
		auto result = one.resolve(dir);
		if (!result)
			return result;
	}
	return true;
}


// Class rx_platform::meta::def_blocks::mapper_attribute 

mapper_attribute::mapper_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_id_(id)
{
}

mapper_attribute::mapper_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_name_(target_name)
{
}



rx_result mapper_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return meta_blocks_algorithm<mapper_attribute>::serialize_complex_attribute(*this, stream);
}

rx_result mapper_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
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

rx_result mapper_attribute::resolve (rx_directory_ptr dir)
{
	return meta_blocks_algorithm<mapper_attribute>::resolve_complex_attribute(*this, dir);
}


// Class rx_platform::meta::def_blocks::simple_value_def 

string_type simple_value_def::type_name = RX_VALUE_TYPE_NAME;

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



rx_result simple_value_def::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.write_string("name", name_.c_str()))
		return false;
	if (!stream.write_bool("ro", read_only_))
		return false;
	if (!storage_.serialize(stream))
		return false;
	return true;
}

rx_result simple_value_def::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_string("name", name_))
		return false;
	if (!stream.read_bool("ro", read_only_))
		return false;
	if (!storage_.deserialize(stream))
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
	, target_id_(id)
{
}

source_attribute::source_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_name_(target_name)
{
}



rx_result source_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return meta_blocks_algorithm<source_attribute>::serialize_complex_attribute(*this, stream);
}

rx_result source_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
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

rx_result source_attribute::resolve (rx_directory_ptr dir)
{
	return meta_blocks_algorithm<source_attribute>::resolve_complex_attribute(*this, dir);
}


// Class rx_platform::meta::def_blocks::struct_attribute 

struct_attribute::struct_attribute (const string_type& name, const rx_node_id& id)
      : name_(name)
	, target_id_(id)
{
}

struct_attribute::struct_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_name_(target_name)
{
}



rx_result struct_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return meta_blocks_algorithm<struct_attribute>::serialize_complex_attribute(*this, stream);
}

rx_result struct_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
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

rx_result struct_attribute::resolve (rx_directory_ptr dir)
{
	return meta_blocks_algorithm<struct_attribute>::resolve_complex_attribute(*this, dir);
}


// Class rx_platform::meta::def_blocks::variable_attribute 

variable_attribute::variable_attribute (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only)
      : name_(name)
	, target_id_(id)
	, storage_(std::move(value))
{
}

variable_attribute::variable_attribute (const string_type& name, const string_type& target_name)
      : name_(name)
	, target_name_(target_name)
{
}



rx_result variable_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	auto temp = meta_blocks_algorithm<variable_attribute>::serialize_complex_attribute(*this, stream);
	return temp;
}

rx_result variable_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
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

rx_result variable_attribute::resolve (rx_directory_ptr dir)
{
	return meta_blocks_algorithm<variable_attribute>::resolve_complex_attribute(*this, dir);
}


// Class rx_platform::meta::def_blocks::variable_data_type 

variable_data_type::variable_data_type()
{
}

variable_data_type::variable_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}


variable_data_type::~variable_data_type()
{
}



rx_result variable_data_type::construct (const names_cahce_type& names, construct_context& ctx) const
{
	for (const auto& one : names)
	{
		switch (one.second&complex_data_type::type_mask)
		{
		// sources
		case complex_data_type::sources_mask:
		{
			rx_result ret = sources_[one.second&complex_data_type::index_mask].construct(ctx);
			if (!ret)
			{
				ret.register_error("Unable to create source "s + one.first + "!");
				return ret;
			}
			break;
		}
		// filters
		case complex_data_type::filters_mask:
		{
			rx_result ret = filters_[one.second&complex_data_type::index_mask].construct(ctx);
			if (!ret)
			{
				ret.register_error("Unable to create filter "s + one.first + "!");
				return ret;
			}
			break;
		}
		// events
		case complex_data_type::events_mask:
		{
			rx_result ret = events_[one.second&complex_data_type::index_mask].construct(ctx);
			if (!ret)
			{
				ret.register_error("Unable to create event "s + one.first + "!");
				return ret;
			}
			break;
		}
		}
	}
	return true;
}

rx_result variable_data_type::serialize_variable_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_array("sources", sources_.size()))
		return false;
	for (const auto& one : sources_)
	{
		if (!stream.start_object("item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	if (!stream.start_array("filters", filters_.size()))
		return false;
	for (const auto& one : filters_)
	{
		if (!stream.start_object("item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	if (!stream.start_array("events", events_.size()))
		return false;
	for (const auto& one : events_)
	{
		if (!stream.start_object("item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;

	return true;
}

rx_result variable_data_type::deserialize_variable_definition (base_meta_reader& stream, uint8_t type, complex_data_type& complex_data)
{
	if (!stream.start_array("sources"))
		return false;
	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return false;
		string_type item_type;
		if (!stream.read_string("type", item_type))
			return false;
		if (item_type != RX_CPP_SOURCE_TYPE_NAME)
			return item_type + " is wrong item type for a source!";

		source_attribute temp;
		if (!temp.deserialize_definition(stream, type))
			return false;
		auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(sources_.size() | complex_data_type::sources_mask)));
		if (ret)
		{
			sources_.emplace_back(std::move(temp));
		}
		else
			return ret;

		if (!stream.end_object())
			return false;
	}

	if (!stream.start_array("filters"))
		return false;
	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return false;
		string_type item_type;
		if (!stream.read_string("type", item_type))
			return false;
		if (item_type != RX_CPP_FILTER_TYPE_NAME)
			return item_type + " is wrong item type for a filter!";

		filter_attribute temp;
		if (!temp.deserialize_definition(stream, type))
			return false;
		auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(filters_.size() | complex_data_type::filters_mask)));
		if (ret)
		{
			filters_.emplace_back(std::move(temp));
		}
		else
			return ret;

		if (!stream.end_object())
			return false;
	}

	if (!stream.start_array("events"))
		return false;
	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return false;
		string_type item_type;
		if (!stream.read_string("type", item_type))
			return false;
		if (item_type != RX_CPP_EVENT_TYPE_NAME)
			return item_type + " is wrong item type for a event!";

		event_attribute temp;
		if (!temp.deserialize_definition(stream, type))
			return false;
		auto ret = complex_data.check_name(temp.get_name(), (static_cast<int>(events_.size() | complex_data_type::events_mask)));
		if (ret)
		{
			events_.emplace_back(std::move(temp));
		}
		else
			return ret;

		if (!stream.end_object())
			return false;
	}
	return true;
}

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

rx_result variable_data_type::register_event (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	auto ret = complex_data.check_name(name, (static_cast<int>(events_.size() | complex_data_type::events_mask)));
	if (ret)
	{
		events_.emplace_back(name, id);
	}
	return ret;
}

bool variable_data_type::check_type (type_check_context& ctx)
{
	bool ret = true;
	for (auto& one : sources_)
		ret &= one.check(ctx);
	for (auto& one : filters_)
		ret &= one.check(ctx);
	for (auto& one : events_)
		ret &= one.check(ctx);
	return ret;
}

rx_result variable_data_type::resolve (rx_directory_ptr dir)
{
	for (auto& one : sources_)
	{
		auto result = one.resolve(dir);
		if (!result)
			return result;
	}
	for (auto& one : filters_)
	{
		auto result = one.resolve(dir);
		if (!result)
			return result;
	}
	for (auto& one : events_)
	{
		auto result = one.resolve(dir);
		if (!result)
			return result;
	}
	return true;
}


} // namespace def_blocks
} // namespace meta
} // namespace rx_platform

