

/****************************************************************************
*
*  system\meta\rx_meta_support.cpp
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


// rx_meta_support
#include "system/meta/rx_meta_support.h"

#include "system/runtime/rx_rt_struct.h"
#include "system/runtime/rx_blocks.h"
#include "rx_def_blocks.h"
#include "model/rx_meta_internals.h"
using namespace rx_platform::runtime::structure;


namespace rx_platform {

namespace meta {

// Class rx_platform::meta::type_check_context 

type_check_context::type_check_context ()
{
}



bool type_check_context::is_check_ok () const
{
	return records_.empty();
}

void type_check_context::add_error (const string_type& msg, rx_error_code_t code, rx_error_severity_t severity)
{
	records_.push_back({ check_record_type::error, code, severity, current_source(), msg });
}

void type_check_context::reinit ()
{
	records_.clear();
}

void type_check_context::add_error (const string_type& msg, rx_error_code_t code, rx_error_severity_t severity, const rx_result& error)
{
	std::ostringstream ss;
	ss << msg << "\r\n";
	for (const auto& one : error.errors())
		ss << one << "\r\n";
	records_.push_back({ check_record_type::error, code, severity, current_source() , ss.str() });
}

const check_records_type& type_check_context::get_records () const
{
	return records_;
}

check_records_type&& type_check_context::move_records ()
{
	return std::move(records_);
}

void type_check_context::push_source (const string_type& source)
{
	sources_stack_.push(source);
}

void type_check_context::pop_source ()
{
	if (!sources_stack_.empty())
		sources_stack_.pop();
	else
		RX_ASSERT(false);
}

const string_type& type_check_context::current_source () const
{
	static string_type g_empty;
	if (!sources_stack_.empty())
		return sources_stack_.top();
	else
		RX_ASSERT(false);
	return g_empty;
}

void type_check_context::add_warning (const string_type& msg, rx_error_code_t code, rx_error_severity_t severity)
{
	records_.push_back({ check_record_type::warning, code, severity, current_source(), msg });
}

void type_check_context::add_warning (const string_type& msg, rx_error_code_t code, rx_error_severity_t severity, const rx_result& error)
{
	std::ostringstream ss;
	ss << msg << "\r\n";
	for (const auto& one : error.errors())
		ss << one << "\r\n";
	records_.push_back({ check_record_type::warning, code, severity, current_source() , ss.str() });
}

void type_check_context::add_info (const string_type& msg)
{
	records_.push_back({ check_record_type::error, 0, 0, current_source(), msg });
}

rx_result_erros_t type_check_context::get_errors () const
{
	rx_result_erros_t errors;
	for (const auto& one : records_)
	{
		errors.emplace_back(one.text);
	}
	return errors;
}


// Class rx_platform::meta::construct_context 

construct_context::construct_context (const string_type& name)
      : now(rx_time::now()),
        state_(active_state_t::regular)
{
	rt_names_.push_back(name);
	runtime_data_.push_back(object_data_prototype());
}



void construct_context::reinit ()
{
	overrides_.clear();
	RX_ASSERT(overrides_stack_.empty());
	while(!overrides_stack_.empty())
		overrides_stack_.pop();
}

void construct_context::push_overrides (const string_type& name, const data::runtime_values_data* vals)
{
	if (overrides_stack_.empty())
	{
		RX_ASSERT(name.empty());
		overrides_ = *vals;
		overrides_stack_.push(&overrides_);
	}
	else
	{
		RX_ASSERT(!name.empty());
		auto& new_one = overrides_stack_.top()->add_child(name);
		new_one = *vals;
		overrides_stack_.push(&new_one);
	}
}

void construct_context::pop_overrides ()
{
	RX_ASSERT(!overrides_stack_.empty());
	overrides_stack_.pop();
}

const data::runtime_values_data* construct_context::get_overrides ()
{
	return &overrides_;
}

const string_type& construct_context::rt_name () const
{
	return *rt_names_.rbegin();
}

void construct_context::push_rt_name (const string_type& name)
{
	rt_names_.push_back(name);
	runtime_data_.push_back(object_data_prototype());
}

runtime_data_prototype construct_context::pop_rt_name ()
{
	rt_names_.pop_back();
	runtime_data_prototype ret = std::move(runtime_data_.rbegin()->runtime_data);
	runtime_data_.pop_back();
	return ret;
}

runtime_data_prototype& construct_context::runtime_data ()
{
	switch (state_)
	{
	case active_state_t::regular:
		return runtime_data_.rbegin()->runtime_data;
	case active_state_t::in_method:
		return runtime_data_.rbegin()->methods.rbegin()->runtime_data;
	case active_state_t::in_program:
		return runtime_data_.rbegin()->programs.rbegin()->runtime_data;
	default:
		RX_ASSERT(false);
		return runtime_data_.rbegin()->runtime_data;
	}
}

void construct_context::start_program (const string_type& name)
{
	RX_ASSERT(state_ == active_state_t::regular);
	program_data_prototype temp;
	temp.name = name;
	runtime_data_.rbegin()->programs.emplace_back(std::move(temp));
	state_ = active_state_t::in_program;
}

void construct_context::start_method (const string_type& name)
{
	RX_ASSERT(state_ == active_state_t::regular);
	method_data_prototype temp;
	temp.name = name;
	runtime_data_.rbegin()->methods.emplace_back(std::move(temp));
	state_ = active_state_t::in_method;
}

void construct_context::end_program ()
{
	RX_ASSERT(state_ == active_state_t::in_program);
	state_ = active_state_t::regular;
}

void construct_context::end_method ()
{
	RX_ASSERT(state_ == active_state_t::in_method);
	state_ = active_state_t::regular;
}

runtime::logic_blocks::method_data& construct_context::method_data ()
{
	RX_ASSERT(state_ == active_state_t::in_method);
	return runtime_data_.rbegin()->methods.rbegin()->method;
}

runtime::logic_blocks::program_data& construct_context::program_data ()
{
	RX_ASSERT(state_ == active_state_t::in_program);
	return runtime_data_.rbegin()->programs.rbegin()->program;
}

void construct_context::start_display (const string_type& name)
{
	RX_ASSERT(state_ == active_state_t::regular);
	display_data_prototype temp;
	temp.name = name;
	runtime_data_.rbegin()->displays.emplace_back(std::move(temp));
	state_ = active_state_t::in_display;
}

void construct_context::end_display ()
{
	RX_ASSERT(state_ == active_state_t::in_display);
	state_ = active_state_t::regular;
}

runtime::display_blocks::display_data& construct_context::display_data ()
{
	RX_ASSERT(state_ == active_state_t::in_display);
	return runtime_data_.rbegin()->displays.rbegin()->display;
}

void construct_context::register_warining (runtime_status_record data)
{
	runtime_status_data warning;
	warning.type = runtime_status_type::warning;
	warning.data = std::move(data);
	for (auto& one : rt_names_)
		warning.path += one;
	warnings_.emplace_back(std::move(warning));
}


// Class rx_platform::meta::runtime_data_prototype 


rx_result runtime_data_prototype::add_const_value (const string_type& name, rx_simple_value value)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{// new one
		members_index_type new_idx = static_cast<members_index_type>(const_values.size());
		const_values.push_back({ value });
		items.push_back({ name, (new_idx << rt_type_shift) | rt_const_index_type });
		return true;
	}
	else
	{// override so check it
		auto& elem = items[idx];
		switch (elem.index & rt_type_mask)
		{
		case rt_const_index_type:
			{
				if(value.get_type() != const_values[elem.index>> rt_type_shift].value.get_type())
					return "Can't override constant value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(const_values.size());
				const_values.push_back({ value });
				elem.index = (new_idx << rt_type_shift) | rt_const_index_type;
			}
			break;
		case rt_value_index_type:
			{
				if (value.get_type() != values[elem.index >> rt_type_shift].value.get_type())
					return "Can't override value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(const_values.size());
				const_values.push_back({ value });
				elem.index = (new_idx << rt_type_shift) | rt_const_index_type;
			}
			break;
		case rt_variable_index_type:
			return "Well, don't know what to do with this one?"s + RX_NOT_SUPPORTED;
			break;
		}
		return true;
	}
}

rx_result runtime_data_prototype::add_value (const string_type& name, rx_timed_value value, bool read_only, bool persistent)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(values.size());
		value_data temp_val;
		temp_val.value = std::move(value);
		temp_val.value_opt[runtime::structure::value_opt_readonly] = read_only;
		temp_val.value_opt[runtime::structure::value_opt_persistent] = persistent;
		values.emplace_back(std::move(temp_val));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_value_index_type });
		return true;
	}
	else
	{// override so check it
		auto& elem = items[idx];
		switch (elem.index & rt_type_mask)
		{
		case rt_const_index_type:
			{
				if (value.get_type() != const_values[elem.index >> rt_type_shift].value.get_type())
					return "Can't override constant value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(values.size());
				value_data temp_val;
				temp_val.value = std::move(value);
				temp_val.value_opt[runtime::structure::value_opt_readonly] = read_only;
				temp_val.value_opt[runtime::structure::value_opt_persistent] = persistent;
				values.push_back({ temp_val });
				elem.index = (new_idx << rt_type_shift) | rt_value_index_type;
			}
			break;
		case rt_value_index_type:
			{
				if (value.get_type() != values[elem.index >> rt_type_shift].value.get_type())
					return "Can't override value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(values.size());
				value_data temp_val;
				temp_val.value = std::move(value);
				temp_val.value_opt[runtime::structure::value_opt_readonly] = read_only;
				temp_val.value_opt[runtime::structure::value_opt_persistent] = persistent;
				values.push_back({ temp_val });
				elem.index = (new_idx << rt_type_shift) | rt_value_index_type;
			}
			break;
		case rt_variable_index_type:
			return "Well, don't know what to do with this one?"s + RX_NOT_SUPPORTED;
			break;
		}
		return true;
	}
}

int runtime_data_prototype::check_member_name (const string_type& name) const
{
	int idx = 0;
	for (const auto& one : items)
	{
		if (one.name == name)
			return idx;
		idx++;
	}
	return -1;
}

rx_result runtime_data_prototype::add (const string_type& name, runtime::structure::mapper_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(mappers.size());
		mappers.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_mapper_index_type });
		return true;
	}
	else
	{
		auto& elem = items[idx];
		if (((elem.index & rt_type_mask) == rt_mapper_index_type))
		{
			members_index_type new_idx = static_cast<members_index_type>(mappers.size());
			mappers.emplace_back(std::move(id), std::move(value));
			elem.index = (new_idx << rt_type_shift) | rt_mapper_index_type;
			return true;
		}
		else
		{
			return name  + " has invalid type for mapper to override";
		}
	}
}

rx_result runtime_data_prototype::add (const string_type& name, runtime::structure::struct_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(structs.size());
		structs.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_struct_index_type });
		return true;
	}
	else
	{
		// override so check it
		auto& elem = items[idx];
		// here i should check about the sub-classing
		/*if (value.value.get_type() != const_values[elem.index >> rt_type_shift].value.get_type())
			return "Can't override constant value, wrong value type!";*/
		if (((elem.index & rt_type_mask) == rt_struct_index_type))
		{
			if (id != structs[elem.index >> rt_type_shift].first &&
				!rx_internal::model::platform_types_manager::instance().get_simple_type_repository<basic_types::struct_type>().is_derived_from(id, structs[elem.index >> rt_type_shift].first))
				return "Can't override struct, wrong struct type!";
			members_index_type new_idx = static_cast<members_index_type>(structs.size());
			structs.emplace_back(std::move(id), std::move(value));
			elem.index = (new_idx << rt_type_shift) | rt_struct_index_type;
			return true;
		}
		else
		{
			return name + " has invalid type for struct to override";
		}
	}
}

rx_result runtime_data_prototype::add_variable (const string_type& name, runtime::structure::variable_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(variables.size());
		variables.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_variable_index_type });
		return true;
	}
	else
	{
		// override so check it
		auto& elem = items[idx];
		switch (elem.index & rt_type_mask)
		{
		case rt_const_index_type:
			{
				if (value.value.get_type() != const_values[elem.index >> rt_type_shift].value.get_type())
					return "Can't override constant value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(variables.size());
				variables.emplace_back(std::move(id), std::move(value));
				elem.index = (new_idx << rt_type_shift) | rt_variable_index_type;
			}
			break;
		case rt_value_index_type:
			{
				if (value.value.get_type() != values[elem.index >> rt_type_shift].value.get_type())
					return "Can't override value, wrong value type!";
				members_index_type new_idx = static_cast<members_index_type>(variables.size());
				variables.emplace_back(std::move(id), std::move(value));
				elem.index = (new_idx << rt_type_shift) | rt_variable_index_type;
			}
			break;
		case rt_variable_index_type:
			{
				if (value.value.get_type() != variables[elem.index >> rt_type_shift].second.value.get_type())
					return "Can't override variable, wrong value type!";
				if(id != variables[elem.index >> rt_type_shift].first &&
					!rx_internal::model::platform_types_manager::instance().get_simple_type_repository<basic_types::variable_type>().is_derived_from(id, variables[elem.index >> rt_type_shift].first))
					return "Can't override variable, wrong variable type!";
				members_index_type new_idx = static_cast<members_index_type>(variables.size());
				variables.emplace_back(std::move(id), std::move(value));
				elem.index = (new_idx << rt_type_shift) | rt_variable_index_type;
			}
			break;
		default:
			return "JOJ!!!";
		}
		return true;
	}
}

rx_result runtime_data_prototype::add (const string_type& name, runtime::structure::source_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(sources.size());
		sources.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_source_index_type });
		return true;
	}
	else
	{
		auto& elem = items[idx];
		if (((elem.index & rt_type_mask) == rt_source_index_type))
		{
			members_index_type new_idx = static_cast<members_index_type>(sources.size());
			sources.emplace_back(std::move(id), std::move(value));
			elem.index = (new_idx << rt_type_shift) | rt_source_index_type;
			return true;
		}
		else
		{
			return name + " has invalid type for source to override";
		}
	}
}

rx_result runtime_data_prototype::add (const string_type& name, runtime::structure::filter_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(filters.size());
		filters.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_filter_index_type });
		return true;
	}
	else
	{
		auto& elem = items[idx];
		if (((elem.index & rt_type_mask) == rt_filter_index_type))
		{
			if (id != filters[elem.index >> rt_type_shift].first &&
				!rx_internal::model::platform_types_manager::instance().get_simple_type_repository<basic_types::filter_type>().is_derived_from(id, structs[elem.index >> rt_type_shift].first))
				return "Can't override filter, wrong filter type!";
			members_index_type new_idx = static_cast<members_index_type>(filters.size());
			filters.emplace_back(std::move(id), std::move(value));
			elem.index = (new_idx << rt_type_shift) | rt_filter_index_type;
			return true;
		}
		else
		{
			return name + " has invalid type for filter to override";
		}
	}
}

rx_result runtime_data_prototype::add (const string_type& name, runtime::structure::event_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(events.size());
		events.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_event_index_type });
		return true;
	}
	else
	{
		auto& elem = items[idx];
		if (((elem.index & rt_type_mask) == rt_event_index_type))
		{
			if (id != events[elem.index >> rt_type_shift].first &&
				!rx_internal::model::platform_types_manager::instance().get_simple_type_repository<basic_types::event_type>().is_derived_from(id, structs[elem.index >> rt_type_shift].first))
				return "Can't override filter, wrong filter type!";
			members_index_type new_idx = static_cast<members_index_type>(events.size());
			events.emplace_back(std::move(id), std::move(value));
			elem.index = (new_idx << rt_type_shift) | rt_event_index_type;
			return true;
		}
		else
		{
			return name + " has invalid type for event to override";
		}
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
// Class rx_platform::meta::object_type_creation_data 


// Class rx_platform::meta::type_creation_data 


// Class rx_platform::meta::type_create_context 

type_create_context::type_create_context ()
{
}



bool type_create_context::created () const
{
	return errors_.empty();
}

void type_create_context::add_error (const string_type& error)
{
	errors_.push_back(error);
}

void type_create_context::reinit ()
{
	errors_.clear();
}


// Class rx_platform::meta::check_record 


// Class rx_platform::meta::type_check_source 

type_check_source::type_check_source (const string_type& source, type_check_context* ctx)
      : ctx_(ctx)
{
	ctx_->push_source(source);
}


type_check_source::~type_check_source()
{
	ctx_->pop_source();
}



// Class rx_platform::meta::object_data_prototype 


// Class rx_platform::meta::method_data_prototype 


// Class rx_platform::meta::program_data_prototype 


// Class rx_platform::meta::display_data_prototype 


// Class rx_platform::meta::data_blocks_prototype 


void data_blocks_prototype::add (const string_type& name, data_blocks_prototype&& value)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(children.size());
		children.emplace_back(std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_data_index_type });
	}
}

void data_blocks_prototype::add_value (const string_type& name, rx_simple_value val)
{
	if (check_name(name))
	{
		members_index_type new_idx = static_cast<members_index_type>(values.size());
		values.push_back({ val });
		items.push_back({ name, (new_idx << rt_type_shift) | rt_const_index_type });
	}
}

bool data_blocks_prototype::check_name (const string_type& name) const
{
	for (const auto& one : items)
	{
		if (one.name == name)
			return false;
	}
	return true;
}

runtime::structure::block_data data_blocks_prototype::create_runtime ()
{
	runtime::structure::block_data ret;
	std::vector<block_data> complex_items;
	complex_items.reserve(children.size());
	for (auto& one : children)
	{
		complex_items.emplace_back(one.create_runtime());
	}
	ret.items = const_size_vector<index_data>(std::move(items));
	ret.values = const_size_vector<const_value_data>(std::move(values));
	ret.children = const_size_vector<block_data>(std::move(complex_items));
	return ret;
}


// Class rx_platform::meta::dependencies_context 


} // namespace meta
} // namespace rx_platform

