

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
#include "model/rx_meta.h"
#include "system/constructors/rx_construct.h"
#include "system/runtime/rx_objbase.h"
#include "system/runtime/rx_rt_data.h"


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
			if (!stream.write_string("Type",struct_type::type_name.c_str()))
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
			if (!stream.write_string("Type", variable_type::type_name.c_str()))
				return false;
			if (!variables_[one.second&index_mask].serialize_definition(stream, type))
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

void complex_data_type::construct (construct_context& ctx)
{
	for (const auto& one : names_cache_)
	{
		switch (one.second&type_mask)
		{
		// structures
		case structs_mask:
		{
			const auto& data = structs_[one.second&index_mask];
			auto temp = model::internal_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_defs::struct_type>().create_simple_runtime(data.get_target_id());
			ctx.runtime_data.add_struct(data.get_name(), std::move(temp));
		}
		break;
		// variables
		case variables_mask:
		{
			const auto& data = variables_[one.second&index_mask];
			auto temp = model::internal_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_defs::variable_type>().create_simple_runtime(data.get_target_id());
			temp.set_value(data.get_value(ctx.now));
			ctx.runtime_data.add_variable(data.get_name(), std::move(temp), data.get_value(ctx.now));
		}
		break;
		// constant values
		case const_values_mask:
		{
			ctx.runtime_data.add_const_value(
				const_values_[one.second&index_mask].get_name(),
				const_values_[one.second&index_mask].get_value());
		}
		break;
		case simple_values_mask:
		{
			ctx.runtime_data.add_value(
				simple_values_[one.second&index_mask].get_name(),
				simple_values_[one.second&index_mask].get_value(ctx.now));
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
	if (!stream.start_object(runtime::blocks::event_runtime::type_name.c_str()))
		return false;

	if (!serialize_complex_attribute(*this, stream))
		return false;

	if (!stream.end_object())
		return false;

	return true;
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
	if (!stream.start_object(runtime::blocks::filter_runtime::type_name.c_str()))
		return false;

	if (!serialize_complex_attribute(*this, stream))
		return false;

	if (!stream.end_object())
		return false;

	return true;
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

void mapped_data_type::construct (const names_cahce_type& names, construct_context& ctx)
{
	for (const auto& one : names)
	{
		switch (one.second&complex_data_type::type_mask)
		{
			// mappers
		case complex_data_type::mappings_mask:
		{
			const auto& data = mappers_[one.second&complex_data_type::index_mask];
			auto temp = model::internal_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_defs::mapper_type>().create_simple_runtime(data.get_target_id());
			ctx.runtime_data.add_mapper(data.get_name(), std::move(temp));
		}
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


// Class rx_platform::meta::def_blocks::mapper_attribute 

mapper_attribute::mapper_attribute (const string_type& name, const rx_node_id& id)
      : name_(name),
        target_id_(id)
{
}



bool mapper_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object(runtime::blocks::mapper_runtime::type_name.c_str()))
		return false;

	if (!serialize_complex_attribute(*this, stream))
		return false;

	if (!stream.end_object())
		return false;

	return true;
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
	if (!stream.start_object(runtime::blocks::source_runtime::type_name.c_str()))
		return false;

	if (!serialize_complex_attribute(*this, stream))
		return false;

	if (!stream.end_object())
		return false;

	return true;
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

variable_attribute::variable_attribute (const string_type& name, const rx_node_id& id, rx_simple_value&& value, bool read_only)
      : name_(name),
        target_id_(id)
	, storage_(std::move(value))
{
}



bool variable_attribute::serialize_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object(runtime::blocks::variable_runtime::type_name.c_str()))
		return false;

	stream.write_bool("RO", read_only_);
	if (!storage_.serialize(stream))
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

rx_value variable_attribute::get_value (rx_time now) const
{
	return rx_value::from_simple(storage_, now);
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



void variable_data_type::construct (runtime::variable_runtime_ptr& what, const names_cahce_type& names, construct_context& ctx)
{
	for (const auto& one : names)
	{
		switch (one.second&complex_data_type::type_mask)
		{
		// sources
		case complex_data_type::sources_mask:
		{
			const auto& data = sources_[one.second&complex_data_type::index_mask];
			auto temp = model::internal_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_defs::source_type>().create_simple_runtime(data.get_target_id());
			ctx.runtime_data.add_source(data.get_name(), std::move(temp));
		}
		break;
		// events
		case complex_data_type::filters_mask:
		{
			const auto& data = filters_[one.second&complex_data_type::index_mask];
			auto temp = model::internal_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_defs::filter_type>().create_simple_runtime(data.get_target_id());
			ctx.runtime_data.add_filter(data.get_name(), std::move(temp));
		}
		break;
		// filters
		case complex_data_type::events_mask:
		{
			const auto& data = events_[one.second&complex_data_type::index_mask];
			auto temp = model::internal_types_manager::instance().get_simple_type_cache<rx_platform::meta::basic_defs::event_type>().create_simple_runtime(data.get_target_id());
			ctx.runtime_data.add_event(data.get_name(), std::move(temp));
		}
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


// Class rx_platform::meta::def_blocks::construct_context 

construct_context::construct_context()
      : now(rx_time::now())
{
}



// Class rx_platform::meta::def_blocks::runtime_data_prototype 


void runtime_data_prototype::add_const_value (const string_type& name, rx_simple_value value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(const_values.size());
		const_values.push_back({ value });
		items.push_back({ name, (new_idx << rt_type_shift) | rt_const_index_type });
	}
}

void runtime_data_prototype::add_value (const string_type& name, rx_timed_value value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(values.size());
		values.push_back({ value });
		items.push_back({ name, (new_idx << rt_type_shift) | rt_value_index_type });
	}
}

bool runtime_data_prototype::check_name (const string_type& name) const
{
	for (const auto& one : items)
	{
		if (one.name == name)
			return false;
	}
	return true;
}

void runtime_data_prototype::add_mapper (const string_type& name, mapper_data&& value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(mappers.size());
		mappers.emplace_back(std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_mapper_index_type });
	}
}

void runtime_data_prototype::add_struct (const string_type& name, struct_data&& value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(structs.size());
		structs.emplace_back(std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_struct_index_type });
	}
}

void runtime_data_prototype::add_variable (const string_type& name, variable_data&& value, rx_value val)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(variables.size());
		value.set_value(std::move(val));
		variables.emplace_back(std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_variable_index_type });
	}
}

void runtime_data_prototype::add_source (const string_type& name, source_data&& value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(sources.size());
		sources.emplace_back(std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_source_index_type });
	}
}

void runtime_data_prototype::add_filter (const string_type& name, filter_data&& value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(filters.size());
		filters.emplace_back(std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_filter_index_type });
	}
}

void runtime_data_prototype::add_event (const string_type& name, event_data&& value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(events.size());
		events.emplace_back(std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_event_index_type });
	}
}



template <class runtime_data_type>
runtime_item::smart_ptr create_runtime_data_from_prototype(runtime_data_prototype& prototype)
{
	std::unique_ptr<runtime_data_type> ret = std::make_unique<runtime_data_type>();
	if (runtime_data_type::has_variables())
	{
		ret->variables.copy_from(std::move(prototype.variables));
	}
	if (runtime_data_type::has_structs())
		ret->structs.copy_from(std::move(prototype.structs));
	if (runtime_data_type::has_sources())
		ret->sources.copy_from(std::move(prototype.sources));
	if (runtime_data_type::has_mappers())
		ret->mappers.copy_from(std::move(prototype.mappers));
	if (runtime_data_type::has_filters())
		ret->filters.copy_from(std::move(prototype.filters));
	if (runtime_data_type::has_events())
		ret->events.copy_from(std::move(prototype.events));

	ret->values = const_size_vector<value_data>(std::move(prototype.values));
	ret->const_values = const_size_vector<const_value_data>(std::move(prototype.const_values));
	ret->items = const_size_vector<index_data>(std::move(prototype.items));

	return ret;
}


runtime_item::smart_ptr create_runtime_data(runtime_data_prototype& prototype)
{
	uint_fast8_t effective_type = (prototype.variables.empty() ? rt_bit_none : rt_bit_has_variables)
		| (prototype.structs.empty() ? rt_bit_none : rt_bit_has_structs)
		| (prototype.sources.empty() ? rt_bit_none : rt_bit_has_sources)
		| (prototype.mappers.empty() ? rt_bit_none : rt_bit_has_mappers)
		| (prototype.filters.empty() ? rt_bit_none : rt_bit_has_filters)
		| (prototype.events.empty() ? rt_bit_none : rt_bit_has_events);

	switch (effective_type)
	{
	case 0x00:
		return create_runtime_data_from_prototype<runtime_data_type00>(prototype);
	case 0x01:
		return create_runtime_data_from_prototype<runtime_data_type01>(prototype);
	case 0x02:
		return create_runtime_data_from_prototype<runtime_data_type02>(prototype);
	case 0x03:
		return create_runtime_data_from_prototype<runtime_data_type03>(prototype);
	case 0x04:
		return create_runtime_data_from_prototype<runtime_data_type04>(prototype);
	case 0x05:
		return create_runtime_data_from_prototype<runtime_data_type05>(prototype);
	case 0x06:
		return create_runtime_data_from_prototype<runtime_data_type06>(prototype);
	case 0x07:
		return create_runtime_data_from_prototype<runtime_data_type07>(prototype);
	case 0x08:
		return create_runtime_data_from_prototype<runtime_data_type08>(prototype);
	case 0x09:
		return create_runtime_data_from_prototype<runtime_data_type09>(prototype);
	case 0x0a:
		return create_runtime_data_from_prototype<runtime_data_type0a>(prototype);
	case 0x0b:
		return create_runtime_data_from_prototype<runtime_data_type0b>(prototype);
	case 0x0c:
		return create_runtime_data_from_prototype<runtime_data_type0c>(prototype);
	case 0x0d:
		return create_runtime_data_from_prototype<runtime_data_type0d>(prototype);
	case 0x0e:
		return create_runtime_data_from_prototype<runtime_data_type0e>(prototype);
	case 0x0f:
		return create_runtime_data_from_prototype<runtime_data_type0f>(prototype);
	case 0x10:
		return create_runtime_data_from_prototype<runtime_data_type10>(prototype);
	case 0x11:
		return create_runtime_data_from_prototype<runtime_data_type11>(prototype);
	case 0x12:
		return create_runtime_data_from_prototype<runtime_data_type12>(prototype);
	case 0x13:
		return create_runtime_data_from_prototype<runtime_data_type13>(prototype);
	case 0x14:
		return create_runtime_data_from_prototype<runtime_data_type14>(prototype);
	case 0x15:
		return create_runtime_data_from_prototype<runtime_data_type15>(prototype);
	case 0x16:
		return create_runtime_data_from_prototype<runtime_data_type16>(prototype);
	case 0x17:
		return create_runtime_data_from_prototype<runtime_data_type17>(prototype);
	case 0x18:
		return create_runtime_data_from_prototype<runtime_data_type18>(prototype);
	case 0x19:
		return create_runtime_data_from_prototype<runtime_data_type19>(prototype);
	case 0x1a:
		return create_runtime_data_from_prototype<runtime_data_type1a>(prototype);
	case 0x1b:
		return create_runtime_data_from_prototype<runtime_data_type1b>(prototype);
	case 0x1c:
		return create_runtime_data_from_prototype<runtime_data_type1c>(prototype);
	case 0x1d:
		return create_runtime_data_from_prototype<runtime_data_type1d>(prototype);
	case 0x1e:
		return create_runtime_data_from_prototype<runtime_data_type1e>(prototype);
	case 0x1f:
		return create_runtime_data_from_prototype<runtime_data_type1f>(prototype);
	case 0x20:
		return create_runtime_data_from_prototype<runtime_data_type20>(prototype);
	case 0x21:
		return create_runtime_data_from_prototype<runtime_data_type21>(prototype);
	case 0x22:
		return create_runtime_data_from_prototype<runtime_data_type22>(prototype);
	case 0x23:
		return create_runtime_data_from_prototype<runtime_data_type23>(prototype);
	case 0x24:
		return create_runtime_data_from_prototype<runtime_data_type24>(prototype);
	case 0x25:
		return create_runtime_data_from_prototype<runtime_data_type25>(prototype);
	case 0x26:
		return create_runtime_data_from_prototype<runtime_data_type26>(prototype);
	case 0x27:
		return create_runtime_data_from_prototype<runtime_data_type27>(prototype);
	case 0x28:
		return create_runtime_data_from_prototype<runtime_data_type28>(prototype);
	case 0x29:
		return create_runtime_data_from_prototype<runtime_data_type29>(prototype);
	case 0x2a:
		return create_runtime_data_from_prototype<runtime_data_type2a>(prototype);
	case 0x2b:
		return create_runtime_data_from_prototype<runtime_data_type2b>(prototype);
	case 0x2c:
		return create_runtime_data_from_prototype<runtime_data_type2c>(prototype);
	case 0x2d:
		return create_runtime_data_from_prototype<runtime_data_type2d>(prototype);
	case 0x2e:
		return create_runtime_data_from_prototype<runtime_data_type2e>(prototype);
	case 0x2f:
		return create_runtime_data_from_prototype<runtime_data_type2f>(prototype);
	case 0x30:
		return create_runtime_data_from_prototype<runtime_data_type30>(prototype);
	case 0x31:
		return create_runtime_data_from_prototype<runtime_data_type31>(prototype);
	case 0x32:
		return create_runtime_data_from_prototype<runtime_data_type32>(prototype);
	case 0x33:
		return create_runtime_data_from_prototype<runtime_data_type33>(prototype);
	case 0x34:
		return create_runtime_data_from_prototype<runtime_data_type34>(prototype);
	case 0x35:
		return create_runtime_data_from_prototype<runtime_data_type35>(prototype);
	case 0x36:
		return create_runtime_data_from_prototype<runtime_data_type36>(prototype);
	case 0x37:
		return create_runtime_data_from_prototype<runtime_data_type37>(prototype);
	case 0x38:
		return create_runtime_data_from_prototype<runtime_data_type38>(prototype);
	case 0x39:
		return create_runtime_data_from_prototype<runtime_data_type39>(prototype);
	case 0x3a:
		return create_runtime_data_from_prototype<runtime_data_type3a>(prototype);
	case 0x3b:
		return create_runtime_data_from_prototype<runtime_data_type3b>(prototype);
	case 0x3c:
		return create_runtime_data_from_prototype<runtime_data_type3c>(prototype);
	case 0x3d:
		return create_runtime_data_from_prototype<runtime_data_type3d>(prototype);
	case 0x3e:
		return create_runtime_data_from_prototype<runtime_data_type3e>(prototype);
	case 0x3f:
		return create_runtime_data_from_prototype<runtime_data_type3f>(prototype);
	}
	return runtime_item::smart_ptr();

}
} // namespace def_blocks
} // namespace meta
} // namespace rx_platform

