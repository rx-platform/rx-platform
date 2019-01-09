

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

#include "system/meta/rx_obj_classes.h"
#include "sys_internal/rx_internal_ns.h"
#include "model/rx_meta.h"
#include "system/constructors/rx_construct.h"
#include "system/runtime/rx_objbase.h"
using namespace rx;


namespace rx_platform {

namespace meta {

namespace def_blocks {
template<typename T>
bool serialize_complex_attribute(const T& whose, base_meta_writer& stream)
{
	if (!stream.write_id("Id", whose.target_id_))
		return false;
	if (!stream.write_string("Name", whose.name_.c_str()))
		return false;
	return true;
}
template<typename T>
bool deserialize_complex_attribute(const T& whose, base_meta_reader& stream)
{
	if (!stream.read_id("Id", whose.target_id_))
		return false;
	if (!stream.read_string("Name", whose.name_.c_str()))
		return false;
	return true;
}

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
			if (!stream.write_string("Type",struct_class::type_name.c_str()))
				return false;
			if (!structs_[one.second&index_mask]->serialize_definition(stream,type))
				return false;
			if (!stream.end_object())
				return false;
		}
		break;
		case variables_mask:
		{

			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Type", variable_class::type_name.c_str()))
				return false;
			if (!variables_[one.second&index_mask]->serialize_definition(stream, type))
				return false;
			if (!stream.end_object())
				return false;
		}

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
		structs_.emplace_back(std::make_unique<struct_attribute>(name, id));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::register_variable (const string_type& name, const rx_node_id& id)
{
	if (check_name(name, (static_cast<int>(variables_.size()|variables_mask))))
	{
		variables_.emplace_back(std::make_unique<variable_attribute>(name, id));
		return true;
	}
	else
	{
		return false;
	}
}

bool complex_data_type::check_name (const string_type& name, int rt_index)
{
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

void complex_data_type::construct (runtime::blocks::complex_runtime_item& what, construct_context& ctx)
{
	for (const auto& one : names_cache_)
	{
		switch (one.second&type_mask)
		{
		// structures
		case structs_mask:
		{
			const auto& data = structs_[one.second&index_mask];
			runtime::blocks::struct_runtime::smart_ptr temp= model::internal_types_manager::instance().get_type_cache<rx_platform::meta::basic_defs::struct_class>().create_simple_runtime(data->get_target_id());
			what.register_sub_item(data->get_name(), std::move(temp));
		}
		break;
		// variables
		case variables_mask:
		{
			const auto& data = variables_[one.second&index_mask];
			runtime::blocks::variable_runtime::smart_ptr temp = model::internal_types_manager::instance().get_type_cache<rx_platform::meta::basic_defs::variable_class>().create_simple_runtime(data->get_target_id());
			what.register_sub_item(data->get_name(), std::move(temp));
		}
		break;
		// constant values
		case const_values_mask:
		{
			what.register_const_value(
				const_values_[one.second&index_mask].get_name(),
				const_values_[one.second&index_mask].get_value());
		}
		break;
		case simple_values_mask:
		{
			what.register_value(
				simple_values_[one.second&index_mask].get_name(),
				simple_values_[one.second&index_mask].get_value((rx_time::now())));
		}
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
	return false;
}

bool event_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::def_blocks::filter_attribute 

filter_attribute::filter_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool filter_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool filter_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
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

bool mapped_data_type::register_mapper (const mapper_attribute& item, complex_data_type& complex_data)
{
	if (complex_data.check_name(item.get_name(), (static_cast<int>(mappers_.size()&complex_data_type::mappings_mask))))
	{
		mappers_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

void mapped_data_type::construct (runtime::blocks::struct_runtime& what, const names_cahce_type& names, construct_context& ctx)
{
	construct_internal(what, names, ctx);
}

void mapped_data_type::construct (runtime::blocks::variable_runtime& what, const names_cahce_type& names, construct_context& ctx)
{
	construct_internal(what, names, ctx);
}

template<typename T>
void mapped_data_type::construct_internal(T& what, const names_cahce_type& names, construct_context& ctx)
{
	for (const auto& one : names)
	{
		switch (one.second&complex_data_type::type_mask)
		{
			// mappers
		case complex_data_type::mappings_mask:
		{
			const auto& data = mappers_[one.second&complex_data_type::index_mask];
			auto temp = model::internal_types_manager::instance().get_type_cache<rx_platform::meta::basic_defs::mapper_class>().create_simple_runtime(data.get_target_id());
			what.register_sub_item(data.get_name(), std::move(temp));
		}
		break;
		}
	}
}
// Class rx_platform::meta::def_blocks::mapper_attribute 

mapper_attribute::mapper_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool mapper_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool mapper_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
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
	return false;
}

bool source_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::meta::def_blocks::struct_attribute 

struct_attribute::struct_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool struct_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object(runtime::blocks::struct_runtime::type_name.c_str()))
		return false;

	if (!serialize_complex_attribute(*this, stream))
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

bool struct_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	/*if (!stream.start_object(runtime::struct_runtime::type_name.c_str()))
		return false;

	if (!complex_class_attribute::deserialize_definition(stream, type))
		return false;

	if (!stream.end_object())
		return false;*/

	return false;
}


// Class rx_platform::meta::def_blocks::variable_attribute 

variable_attribute::variable_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool variable_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object(runtime::blocks::variable_runtime::type_name.c_str()))
		return false;

	if (!serialize_complex_attribute(*this, stream))
		return false;

	if (!stream.end_object())
		return false;

	return true;
}

bool variable_attribute::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	return false;
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



bool variable_data_type::register_source (const source_attribute& item, complex_data_type& complex_data)
{
	if (complex_data.check_name(item.get_name(), (static_cast<int>(sources_.size()&complex_data_type::sources_mask))))
	{
		sources_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

bool variable_data_type::register_filter (const filter_attribute& item, complex_data_type& complex_data)
{
	if (complex_data.check_name(item.get_name(), (static_cast<int>(filters_.size()&complex_data_type::filters_mask))))
	{
		filters_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

bool variable_data_type::register_event (const event_attribute& item, complex_data_type& complex_data)
{
	if (complex_data.check_name(item.get_name(), (static_cast<int>(events_.size()&complex_data_type::events_mask))))
	{
		events_.emplace_back(item);
		return true;
	}
	else
	{
		return false;
	}
}

void variable_data_type::construct (runtime::blocks::variable_runtime_ptr& what, const names_cahce_type& names, construct_context& ctx)
{
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


// Class rx_platform::meta::def_blocks::construct_context 

construct_context::construct_context()
      : now(rx_time::now())
{
}



} // namespace def_blocks
} // namespace meta
} // namespace rx_platform

