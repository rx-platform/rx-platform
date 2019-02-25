

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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
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

namespace meta {

namespace def_blocks {

// Class rx_platform::meta::def_blocks::complex_data_type 

complex_data_type::complex_data_type()
{
}

complex_data_type::complex_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



bool complex_data_type::serialize_complex_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.write_bool("Sealed", sealed_))
		return false;

	if (!stream.write_bool("Abstract", abstract_))
		return false;

	if (!stream.start_array("Items", names_cache_.size()))
		return false;
	for (const auto& one : names_cache_)
	{
		switch (one.second&type_mask)
		{
		case structs_mask:
		{

			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Type",basic_types::struct_type::type_name.c_str()))
				return false;
			if (!structs_[one.second&index_mask].serialize_definition(stream,type))
				return false;
			if (!stream.end_object())
				return false;
		}
		break;
		case variables_mask:
		{

			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Type", basic_types::variable_type::type_name.c_str()))
				return false;
			if (!variables_[one.second&index_mask].serialize_definition(stream, type))
				return false;
			if (!stream.end_object())
				return false;
		}
		break;
		case simple_values_mask:
		{
			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Type", simple_value_def::type_name.c_str()))
				return false;
			if (!simple_values_[one.second&index_mask].serialize_definition(stream, type))
				return false;
			if (!stream.end_object())
				return false;
		}
		break;
		case const_values_mask:
		{
			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Type", const_value_def::type_name.c_str()))
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

bool complex_data_type::deserialize_complex_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_bool("Sealed", sealed_))
		return false;

	if (!stream.start_array("Items"))
		return false;

	//for (const auto& one : sub_items_)
	//{
	//	if (!one->deserialize_definition(stream, type))
	//		return false;
	//}

	if (!stream.array_end())
		return false;

	return false;//!!!! NOT DONE
}

bool complex_data_type::register_struct (const string_type& name, const rx_node_id& id)
{
	if (check_name(name, (static_cast<int>(structs_.size()|structs_mask))))
	{
		structs_.emplace_back(struct_attribute(name, id));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::register_variable (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only)
{
	if (check_name(name, (static_cast<int>(variables_.size()|variables_mask))))
	{
		variables_.emplace_back(variable_attribute(name, id, std::move(value), read_only));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::check_name (const string_type& name, int rt_index)
{
	if (name == RX_DEFAULT_VARIABLE_NAME)
		return false;

	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
	{
		names_cache_.emplace(name, rt_index);
		return true;
	}
	else
	{
		return false;
	}
}

void complex_data_type::construct (construct_context& ctx) const
{
	for (const auto& one : names_cache_)
	{
		switch (one.second&type_mask)
		{
		// constant values
		case const_values_mask:
		{
			ctx.runtime_data.add_const_value(
				const_values_[one.second&index_mask].get_name(),
				const_values_[one.second&index_mask].get_value());
		}
		break;
		// simple values
		case simple_values_mask:
		{
			ctx.runtime_data.add_value(
				simple_values_[one.second&index_mask].get_name(),
				simple_values_[one.second&index_mask].get_value(ctx.now));
		}
		break;
		// structures
		case structs_mask:
			structs_[one.second&index_mask].construct(ctx);
			break;
		// variables
		case variables_mask:
			variables_[one.second&index_mask].construct(ctx);
			break;
		}
	}
}

bool complex_data_type::register_simple_value (const string_type& name, bool read_only, rx_simple_value&& val)
{
	if (check_name(name, (static_cast<int>(simple_values_.size()|simple_values_mask))))
	{
		simple_values_.emplace_back(name, read_only, std::move(val));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::register_const_value (const string_type& name, rx_simple_value&& val)
{
	if (check_name(name, (static_cast<int>(const_values_.size()|const_values_mask))))
	{
		const_values_.emplace_back(name, std::move(val));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::register_simple_value (const string_type& name, bool read_only, const rx_simple_value& val)
{
	if (check_name(name, (static_cast<int>(simple_values_.size()|simple_values_mask))))
	{
		simple_values_.emplace_back(name, read_only, val);
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::register_const_value (const string_type& name, const rx_simple_value& val)
{
	if (check_name(name, (static_cast<int>(const_values_.size()|const_values_mask))))
	{
		const_values_.emplace_back(name, val);
		return true;
	}
	else
	{
		return false;
	}
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



bool const_value_def::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	stream.write_string("Name", name_.c_str());
	if (!get_value().serialize(stream))
		return false;
	return true;
}

bool const_value_def::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}

rx_simple_value const_value_def::get_value () const
{
	return storage_;
}


// Class rx_platform::meta::def_blocks::event_attribute 

event_attribute::event_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool event_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	auto temp = meta_blocks_algorithm<event_attribute>::serialize_complex_attribute(*this, stream);
	return temp;
}

bool event_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	auto temp = meta_blocks_algorithm<event_attribute>::deserialize_complex_attribute(*this, stream);
	return temp;
}

bool event_attribute::check (type_check_context& ctx)
{
	auto temp = meta_blocks_algorithm<event_attribute>::check_complex_attribute(*this, ctx);
	return temp;
}

void event_attribute::construct (construct_context& ctx) const
{
	meta_blocks_algorithm<event_attribute>::construct_complex_attribute(*this, ctx);
}


// Class rx_platform::meta::def_blocks::filter_attribute 

filter_attribute::filter_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool filter_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return meta_blocks_algorithm<filter_attribute>::serialize_complex_attribute(*this, stream);
}

bool filter_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return meta_blocks_algorithm<filter_attribute>::deserialize_complex_attribute(*this, stream);
}

bool filter_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<filter_attribute>::check_complex_attribute(*this, ctx);
}

void filter_attribute::construct (construct_context& ctx) const
{
	meta_blocks_algorithm<filter_attribute>::construct_complex_attribute(*this, ctx);
}


// Class rx_platform::meta::def_blocks::mapped_data_type 

mapped_data_type::mapped_data_type()
{
}

mapped_data_type::mapped_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



bool mapped_data_type::serialize_mapped_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_array("Mappers", mappers_.size()))
		return false;
	for (const auto& one : mappers_)
	{
		if (!stream.start_object("Item"))
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

bool mapped_data_type::deserialize_mapped_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}

void mapped_data_type::construct (const names_cahce_type& names, construct_context& ctx) const
{
	for (const auto& one : names)
	{
		switch (one.second&complex_data_type::type_mask)
		{
		// mappers
		case complex_data_type::mappings_mask:
			mappers_[one.second&complex_data_type::index_mask].construct(ctx);
			break;
		}
	}
}

bool mapped_data_type::register_mapper (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	if (complex_data.check_name(name, (static_cast<int>(mappers_.size()|complex_data_type::mappings_mask))))
	{
		mappers_.emplace_back(name, id);
		return true;
	}
	else
	{
		return false;
	}
}

bool mapped_data_type::check_type (type_check_context& ctx)
{
	bool ret = true;
	for (auto& one : mappers_)
		ret &= one.check(ctx);
	return ret;
}


// Class rx_platform::meta::def_blocks::mapper_attribute 

mapper_attribute::mapper_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool mapper_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return meta_blocks_algorithm<mapper_attribute>::serialize_complex_attribute(*this, stream);
}

bool mapper_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return meta_blocks_algorithm<mapper_attribute>::deserialize_complex_attribute(*this, stream);
}

bool mapper_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<mapper_attribute>::check_complex_attribute(*this, ctx);
}

void mapper_attribute::construct (construct_context& ctx) const
{
	meta_blocks_algorithm<mapper_attribute>::construct_complex_attribute(*this, ctx);
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



bool simple_value_def::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	stream.write_string("Name", name_.c_str());
	stream.write_bool("RO", read_only_);
	if (!storage_.serialize(stream))
		return false;
	return true;
}

bool simple_value_def::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}

rx_timed_value simple_value_def::get_value (rx_time now) const
{
	return rx_timed_value::from_simple(storage_, now);
}


// Class rx_platform::meta::def_blocks::source_attribute 

source_attribute::source_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool source_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return meta_blocks_algorithm<source_attribute>::serialize_complex_attribute(*this, stream);
}

bool source_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return meta_blocks_algorithm<source_attribute>::deserialize_complex_attribute(*this, stream);
}

bool source_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<source_attribute>::check_complex_attribute(*this, ctx);
}

void source_attribute::construct (construct_context& ctx) const
{
	meta_blocks_algorithm<source_attribute>::construct_complex_attribute(*this, ctx);
}


// Class rx_platform::meta::def_blocks::struct_attribute 

struct_attribute::struct_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool struct_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return meta_blocks_algorithm<struct_attribute>::serialize_complex_attribute(*this, stream);
}

bool struct_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return meta_blocks_algorithm<struct_attribute>::deserialize_complex_attribute(*this, stream);
}

bool struct_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<struct_attribute>::check_complex_attribute(*this, ctx);
}

void struct_attribute::construct (construct_context& ctx) const
{
	meta_blocks_algorithm<struct_attribute>::construct_complex_attribute(*this, ctx);
}


// Class rx_platform::meta::def_blocks::variable_attribute 

variable_attribute::variable_attribute (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only)
      : name_(name),
        target_id_(id)
	, storage_(std::move(value))
{
}



bool variable_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	auto temp = meta_blocks_algorithm<variable_attribute>::serialize_complex_attribute(*this, stream);
	return temp;
}

bool variable_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	auto temp = meta_blocks_algorithm<variable_attribute>::deserialize_complex_attribute(*this, stream);
	return temp;
}

rx_value variable_attribute::get_value (rx_time now) const
{
	return rx_value::from_simple(storage_, now);
}

bool variable_attribute::check (type_check_context& ctx)
{
	return meta_blocks_algorithm<variable_attribute>::check_complex_attribute(*this, ctx);
}

void variable_attribute::construct (construct_context& ctx) const
{
	meta_blocks_algorithm<variable_attribute>::construct_complex_attribute(*this, ctx);
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



void variable_data_type::construct (const names_cahce_type& names, construct_context& ctx) const
{
	for (const auto& one : names)
	{
		switch (one.second&complex_data_type::type_mask)
		{
		// sources
		case complex_data_type::sources_mask:
			sources_[one.second&complex_data_type::index_mask].construct(ctx);
			break;
		// filters
		case complex_data_type::filters_mask:
			filters_[one.second&complex_data_type::index_mask].construct(ctx);
			break;
		// events
		case complex_data_type::events_mask:
			events_[one.second&complex_data_type::index_mask].construct(ctx);
			break;
		}
	}
}

bool variable_data_type::serialize_variable_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_array("Sources", sources_.size()))
		return false;
	for (const auto& one : sources_)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;
	if (!stream.start_array("Events", events_.size()))
		return false;
	for (const auto& one : events_)
	{
		if (!stream.start_object("Item"))
			return false;
		if (!one.serialize_definition(stream, type))
			return false;
		if (!stream.end_object())
			return false;
	}
	if (!stream.end_array())
		return false;
	if (!stream.start_array("Filters", filters_.size()))
		return false;
	for (const auto& one : filters_)
	{
		if (!stream.start_object("Item"))
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

bool variable_data_type::deserialize_variable_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}

bool variable_data_type::register_source (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	if (complex_data.check_name(name, (static_cast<int>(sources_.size()|complex_data_type::sources_mask))))
	{
		sources_.emplace_back(name, id);
		return true;
	}
	else
	{
		return false;
	}
}

bool variable_data_type::register_filter (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	if (complex_data.check_name(name, (static_cast<int>(filters_.size()|complex_data_type::filters_mask))))
	{
		filters_.emplace_back(name, id);
		return true;
	}
	else
	{
		return false;
	}
}

bool variable_data_type::register_event (const string_type& name, const rx_node_id& id, complex_data_type& complex_data)
{
	if (complex_data.check_name(name, (static_cast<int>(events_.size()|complex_data_type::events_mask))))
	{
		events_.emplace_back(name, id);
		return true;
	}
	else
	{
		return false;
	}
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


} // namespace def_blocks
} // namespace meta
} // namespace rx_platform

