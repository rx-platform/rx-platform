

/****************************************************************************
*
*  system\meta\rx_meta_support.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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

#include "system/runtime/rx_rt_item_types.h"
#include "api/rx_platform_api.h"
//#include "system/runtime/rx_blocks.h"
//#include "rx_def_blocks.h"
#include "system/meta/rx_obj_types.h"
#include "model/rx_meta_internals.h"
#include "system/meta/rx_meta_algorithm.h"
using namespace rx_platform::runtime::structure;
using namespace rx_internal::model;


namespace rx_platform {

meta_data create_type_meta_data(const object_type_creation_data& type_data)
{
	meta_data ret;
	ret.set_id(type_data.id);
	ret.set_name(type_data.name);
	ret.set_path(type_data.path);
	ret.set_parent(type_data.base_id);
	ret.set_created_time(rx_time::now());
	ret.set_modified_time(ret.created_time);
	ret.set_attributes(type_data.attributes);

	return ret;
}

meta_data create_type_meta_data(const type_creation_data& type_data)
{
	meta_data ret;
	ret.set_id(type_data.id);
	ret.set_name(type_data.name);
	ret.set_path(type_data.path);
	ret.set_parent(type_data.base_id);
	ret.set_created_time(rx_time::now());
	ret.set_modified_time(ret.created_time);
	ret.set_attributes(type_data.attributes);

	return ret;
}
bool should_run_at(string_view_type val)
{
	if (val == RX_OTHERS_STORAGE_NAME || val == RX_CODE_STORAGE_NAME)
		return false;
	else
		return true;
}
rx_result_with<rx_storage_ptr> resolve_storage(const meta_data& data)
{
	if (should_run_at(data.run_at))
	{
		auto dir = rx_gate::instance().get_directory(data.path);
		if (dir)
		{
			auto storage = dir->get_storage();
			if (!storage)
				return "No storage defined for item";
			else
				return storage;
		}
		else
			return "Unable to locate item's storage!";
	}
	else
	{
		rx_storage_ptr ret = rx_create_reference<storage_base::rx_others_storage>();
		return ret;
	}
}

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

construct_context::construct_context (const meta_data& meta)
      : now(rx_time::now()),
        state_(active_state_t::regular),
        current_display_(-1),
        current_program_(-1),
        current_method_(-1),
        changed_event(nullptr),
        in_model_(true)
{
	rt_names_.push_back(meta.name);
	access_guards_.emplace("", security::security_guard(meta, rx_security_null));
	runtime_data_.runtime_data.push_back(runtime_data_prototype());
	block_stack.push(&changed_data_block);
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
		auto new_one = overrides_stack_.top()->add_child(name);
		if(new_one)
			*new_one = *vals;
		overrides_stack_.push(new_one);
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
	runtime_stack().push_back(runtime_data_prototype());
}

runtime_data_prototype construct_context::pop_rt_name ()
{
	rt_names_.pop_back();
	runtime_data_prototype ret = std::move(*runtime_stack().rbegin());
	runtime_stack().pop_back();
	return ret;
}

runtime_data_prototype& construct_context::runtime_data ()
{
	return *runtime_stack().rbegin();
}

runtime_data_type& construct_context::runtime_stack ()
{
	switch (state_)
	{
	case active_state_t::regular:
		return runtime_data_.runtime_data;
	case active_state_t::in_method:
		RX_ASSERT(current_method_ >= 0);
		RX_ASSERT(current_method_ < runtime_data_.methods.size());
		return runtime_data_.methods[current_method_].runtime_data;
	case active_state_t::in_program:
		RX_ASSERT(current_program_ >= 0);
		RX_ASSERT(current_program_ < runtime_data_.programs.size());
		return runtime_data_.programs[current_program_].runtime_data;
	case active_state_t::in_display:
		RX_ASSERT(current_display_ >= 0);
		RX_ASSERT(current_display_ < runtime_data_.displays.size());
		return runtime_data_.displays[current_display_].runtime_data;
	default:
		RX_ASSERT(false);
		return runtime_data_.runtime_data;
	}
}

void construct_context::start_program (const string_type& name)
{
	RX_ASSERT(state_ == active_state_t::regular);
	program_data_prototype temp;
	temp.name = name;
	int programs_size = (int)runtime_data_.programs.size();
	for (int i = 0; i < programs_size; i++)
	{
		if (runtime_data_.programs[i].name == name)
		{// override of method
			runtime_data_.programs[i] = std::move(temp);
			state_ = active_state_t::in_program;
			current_program_ = i;
			block_stack.push(nullptr);
			return;
		}
	}
	// new method so we're good
	current_program_ = programs_size;
	runtime_data_.programs.emplace_back(std::move(temp));
	block_stack.push(nullptr);
	state_ = active_state_t::in_program;
}

void construct_context::start_method (const string_type& name, rx_node_id& inputs_id, rx_node_id& outputs_id)
{
	RX_ASSERT(state_ == active_state_t::regular);
	method_data_prototype temp;
	temp.name = name;
	int methods_size = (int)runtime_data_.methods.size();
	for (int i = 0; i < methods_size; i++)
	{
		if (runtime_data_.methods[i].name == name)
		{// override of method
			inputs_id = runtime_data_.methods[i].get_inputs_id();
			outputs_id = runtime_data_.methods[i].get_inputs_id();
			runtime_data_.methods[i] = std::move(temp);
			state_ = active_state_t::in_method;
			block_stack.push(nullptr);
			current_method_ = i;
			return;
		}
	}
	// new method so we're good
	inputs_id = rx_node_id();
	outputs_id = rx_node_id();
	current_method_ = methods_size;
	runtime_data_.methods.push_back(std::move(temp));
	block_stack.push(nullptr);
	state_ = active_state_t::in_method;
}

void construct_context::end_program (runtime::logic_blocks::program_data data)
{
	RX_ASSERT(state_ == active_state_t::in_program);
	RX_ASSERT(current_program_ >= 0 && current_program_ < runtime_data_.programs.size());
	data.name = runtime_data_.programs[current_program_].name;
	runtime_data_.programs[current_method_].program = std::move(data);
	current_program_ = -1;
	block_stack.pop();
	state_ = active_state_t::regular;
}

void construct_context::end_method (runtime::logic_blocks::method_data data, rx_node_id inputs_id, rx_node_id outputs_id)
{
	RX_ASSERT(state_ == active_state_t::in_method);
	RX_ASSERT(current_method_ >= 0 && current_method_ < runtime_data_.methods.size());
	data.name = runtime_data_.methods[current_method_].name;
	runtime_data_.methods[current_method_].method = std::move(data);
	runtime_data_.methods[current_method_].set_inputs_id(inputs_id);
	runtime_data_.methods[current_method_].set_outputs_id(outputs_id);
	current_method_ = -1;
	block_stack.pop();
	state_ = active_state_t::regular;
}

runtime::logic_blocks::method_data& construct_context::method_data ()
{
	RX_ASSERT(state_ == active_state_t::in_method);
	RX_ASSERT(current_method_ >= 0 && current_method_ < runtime_data_.methods.size());
	return runtime_data_.methods[current_method_].method;
}

runtime::logic_blocks::program_data& construct_context::program_data ()
{
	RX_ASSERT(state_ == active_state_t::in_program);
	RX_ASSERT(current_program_ >= 0 && current_program_ < runtime_data_.programs.size());
	return runtime_data_.programs[current_program_].program;
}

void construct_context::start_display (const string_type& name)
{
	RX_ASSERT(state_ == active_state_t::regular);
	display_data_prototype temp;
	temp.name = name;
	int displays_size = (int)runtime_data_.displays.size();
	for (int i = 0; i < displays_size; i++)
	{
		if (runtime_data_.displays[i].name == name)
		{// override of display
			runtime_data_.displays[i] = std::move(temp);
			current_display_ = i;
			state_ = active_state_t::in_display;
			block_stack.push(nullptr);
			return;
		}
	}
	// new display so we're good
	current_display_ = displays_size;
	runtime_data_.displays.push_back(std::move(temp));
	block_stack.push(nullptr);
	state_ = active_state_t::in_display;
}

void construct_context::end_display (runtime::display_blocks::display_data data)
{
	RX_ASSERT(state_ == active_state_t::in_display);
	RX_ASSERT(current_display_ >= 0 && current_display_ < runtime_data_.displays.size());
	data.name = runtime_data_.displays[current_display_].name;
	runtime_data_.displays[current_display_].display = std::move(data);
	current_display_ = -1;
	block_stack.pop();
	state_ = active_state_t::regular;
}

runtime::display_blocks::display_data& construct_context::display_data ()
{
	RX_ASSERT(state_ == active_state_t::in_display);
	RX_ASSERT(current_display_ >= 0 && current_display_ < runtime_data_.displays.size());
	return runtime_data_.displays[current_display_].display;
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

object_data_prototype& construct_context::object_data ()
{
	return runtime_data_;
}

bool construct_context::out_of_model ()
{
	bool ret = in_model_;
	in_model_ = false;
	return ret;
}

void construct_context::end_of_model_out (bool prev)
{
	if (prev)
		in_model_ = true;
}

bool construct_context::is_in_model () const
{
	return in_model_;
}

string_type construct_context::get_current_path () const
{
	string_type ret;
	size_t size = rt_names_.size();
	if (size > 1)
	{
		for (size_t i = 1; i < size; i++)
		{
			if (!ret.empty())
				ret += RX_OBJECT_DELIMETER;
			ret += rt_names_[i];
		}
	}
	if (!ret.empty())
		ret += RX_OBJECT_DELIMETER;
	return ret;
}

std::map<string_type, security::security_guard> construct_context::normalize_security_guards ( security::security_guard root)
{
	if (!root.is_null() && access_guards_.size() > 0)
	{// no guard so use root one
		access_guards_[""] = std::move(root);
	}
	return access_guards_;
}


// Class rx_platform::meta::runtime_data_prototype 


rx_result runtime_data_prototype::add_const_value (const string_type& name, rx_simple_value value, const std::bitset<32>& value_opt)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{// new one
		members_index_type new_idx = static_cast<members_index_type>(const_values.size());
		runtime::structure::const_value_data temp;
		temp.value = std::move(value);
		const_values.push_back(std::move(temp));
		const_values_opts.push_back(value_opt);
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
				auto& this_val = const_values[elem.index >> rt_type_shift];
				if(this_val.is_array())
					return "Can't override constant value, can not replace array with simple value!";
				if(value.get_type() != this_val.get_item()->value.get_type())
					return "Can't override constant value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(const_values.size());
				runtime::structure::const_value_data temp;
				temp.value = std::move(value);
				if (!check_read_only(value_opt, const_values_opts[elem.index >> rt_type_shift]))
					return "Can't override const value wrong read-only value";
				const_values.emplace_back(std::move(temp));
				const_values_opts.push_back(value_opt);
				elem.index = (new_idx << rt_type_shift) | rt_const_index_type;
			}
			break;
		case rt_value_index_type:
			{
				auto& this_val = values[elem.index >> rt_type_shift];
				if (this_val.is_array())
					return "Can't override value, can not replace array with simple value!";
				if (value.get_type() != this_val.get_item()->value.get_type())
					return "Can't override value, wrong value type!";
				if (!check_read_only(value_opt, this_val.get_item()->value_opt))
					return "Can't override const value wrong read-only value";

				members_index_type new_idx = static_cast<members_index_type>(const_values.size());
				runtime::structure::const_value_data temp;
				temp.value = std::move(value);
				const_values.emplace_back(std::move(temp));
				const_values_opts.push_back(value_opt);
				elem.index = (new_idx << rt_type_shift) | rt_const_index_type;
			}
			break;
		case rt_variable_index_type:
			return "Well, don't know what to do with this one?"s + RX_NOT_SUPPORTED;
			break;
		default:
			return name + " has invalid type to override";
		}
		return true;
	}
}

rx_result runtime_data_prototype::add_const_value (const string_type& name, std::vector<values::rx_simple_value> value, const std::bitset<32>& value_opt)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{// new one
		members_index_type new_idx = static_cast<members_index_type>(const_values.size());
		std::vector<runtime::structure::const_value_data> temp_array;
		int size = (int)value.size();
		if (size > 0)
		{
			for (int i = 0; i < size; i++)
			{
				runtime::structure::const_value_data temp;
				temp.value = std::move(value[i]);
				temp_array.push_back(std::move(temp));
			}
		}
		const_values.emplace_back(std::move(temp_array));
		const_values_opts.push_back(value_opt);
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
				auto& this_val = const_values[elem.index >> rt_type_shift];
				if (!this_val.is_array())
					return "Can't override constant value, can not replace simple value with array!";
				if (value[0].get_type() != this_val.get_item(0)->value.get_type())
					return "Can't override constant value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(const_values.size());

				std::vector<runtime::structure::const_value_data> temp_array;
				int size = (int)value.size();
				if (size > 0)
				{
					for (int i = 0; i < size; i++)
					{
						runtime::structure::const_value_data temp;
						temp.value = std::move(value[i]);
						temp_array.push_back(std::move(temp));
					}
				}
				const_values.emplace_back(std::move(temp_array));
				const_values_opts.push_back(value_opt);
				elem.index = (new_idx << rt_type_shift) | rt_const_index_type;
			}
			break;
		case rt_value_index_type:
			{
				auto& this_val = values[elem.index >> rt_type_shift];
				if (!this_val.is_array())
					return "Can't override value, can not replace simple value with array!";
				if (value[0].get_type() != values[elem.index >> rt_type_shift].get_item(0)->value.get_type())
					return "Can't override value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(const_values.size());

				std::vector<runtime::structure::const_value_data> temp_array;
				int size = (int)value.size();
				if (size > 0)
				{
					for (int i = 0; i < size; i++)
					{
						runtime::structure::const_value_data temp;
						temp.value = std::move(value[i]);
						temp_array.push_back(std::move(temp));
					}
				}
				const_values.emplace_back(std::move(temp_array));
				const_values_opts.push_back(value_opt);
				elem.index = (new_idx << rt_type_shift) | rt_const_index_type;
			}
			break;
		case rt_variable_index_type:
			return "Well, don't know what to do with this one?"s + RX_NOT_SUPPORTED;
			break;
		default:
			return name + " has invalid type to override";
		}
		return true;
	}
}

rx_result runtime_data_prototype::add_value (const string_type& name, rx_timed_value value, const std::bitset<32>& value_opt)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(values.size());
		value_data temp_val;
		temp_val.value = std::move(value);
		temp_val.value_opt = value_opt;
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
				auto& this_val = const_values[elem.index >> rt_type_shift];
				if (this_val.is_array())
					return "Can't override value, can not replace array with simple value!";
				if (value.get_type() != this_val.get_item()->value.get_type())
					return "Can't override const value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(values.size());
				value_data temp_val;
				temp_val.value = std::move(value);
				temp_val.value_opt = value_opt;
				if (!check_read_only(temp_val.value_opt, const_values_opts[elem.index >> rt_type_shift]))
					return "Can't override const value wrong read-only value";
				values.push_back(std::move(temp_val));
				elem.index = (new_idx << rt_type_shift) | rt_value_index_type;
			}
			break;
		case rt_value_index_type:
			{
				auto& this_val = values[elem.index >> rt_type_shift];
				if (this_val.is_array())
					return "Can't override value, can not replace array with simple value!";

				if (value.get_type() != this_val.get_item()->value.get_type())
					return "Can't override value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(values.size());
				value_data temp_val;
				temp_val.value = std::move(value);
				temp_val.value_opt = value_opt;
				if (!check_read_only(temp_val.value_opt, this_val.get_item()->value_opt))
					return "Can't override value wrong read-only value";
				values.push_back(std::move(temp_val));
				elem.index = (new_idx << rt_type_shift) | rt_value_index_type;
			}
			break;
		case rt_variable_index_type:
			return "Well, don't know what to do with this one?"s + RX_NOT_SUPPORTED;
			break;
		default:
			return name + " has invalid type to override";
		}
		return true;
	}
}

rx_result runtime_data_prototype::add_value (const string_type& name, std::vector<rx_timed_value> value, const std::bitset<32>& value_opt)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{// new one
		members_index_type new_idx = static_cast<members_index_type>(values.size());
		std::vector<runtime::structure::value_data> temp_array;
		int size = (int)value.size();
		if (size > 0)
		{
			for (int i = 0; i < size; i++)
			{
				runtime::structure::value_data temp;
				temp.value_opt = value_opt;
				temp.value = std::move(value[i]);
				temp_array.push_back(std::move(temp));
			}
		}
		values.emplace_back(std::move(temp_array));
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
				auto& this_val = const_values[elem.index >> rt_type_shift];
				if (!this_val.is_array())
					return "Can't override constant value, can not replace simple value with array!";
				if (value[0].get_type() != this_val.get_item(0)->value.get_type())
					return "Can't override constant value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(values.size());

				std::vector<runtime::structure::value_data> temp_array;
				int size = (int)value.size();
				if (size > 0)
				{
					for (int i = 0; i < size; i++)
					{
						runtime::structure::value_data temp;
						temp.value = std::move(value[i]);
						temp_array.push_back(std::move(temp));
					}
				}
				values.emplace_back(std::move(temp_array));
				elem.index = (new_idx << rt_type_shift) | rt_value_index_type;
			}
			break;
		case rt_value_index_type:
			{
				auto& this_val = values[elem.index >> rt_type_shift];
				if (!this_val.is_array())
					return "Can't override value, can not replace simple value with array!";
				if (value[0].get_type() != values[elem.index >> rt_type_shift].get_item(0)->value.get_type())
					return "Can't override value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(values.size());

				std::vector<runtime::structure::value_data> temp_array;
				int size = (int)value.size();
				if (size > 0)
				{
					for (int i = 0; i < size; i++)
					{
						runtime::structure::value_data temp;
						temp.value = std::move(value[i]);
						temp_array.push_back(std::move(temp));
					}
				}
				values.emplace_back(std::move(temp_array));
				elem.index = (new_idx << rt_type_shift) | rt_value_index_type;
			}
			break;
		case rt_variable_index_type:
			return "Well, don't know what to do with this one?"s + RX_NOT_SUPPORTED;
			break;
		default:
			return name + " has invalid type to override";
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

rx_result runtime_data_prototype::add_mapper (const string_type& name, runtime::structure::mapper_data&& value, rx_node_id id)
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

rx_result runtime_data_prototype::add_struct (const string_type& name, runtime::structure::struct_data&& value, rx_node_id id, runtime::structure::block_data block)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(structs.size());
		structs.emplace_back(std::move(id), std::move(value));
		struct_blocks.emplace_back(block);
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
			struct_blocks.emplace_back(block);
			elem.index = (new_idx << rt_type_shift) | rt_struct_index_type;
			return true;
		}
		else
		{
			return name + " has invalid type for struct to override";
		}
	}
}

rx_result runtime_data_prototype::add_struct (const string_type& name, std::vector<runtime::structure::struct_data> value, rx_node_id id, runtime::structure::block_data block)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(structs.size());
		structs.emplace_back(std::move(id), std::move(value));
		struct_blocks.emplace_back(block);
		items.push_back({ name, (new_idx << rt_type_shift) | rt_struct_index_type });
		return true;
	}
	else
	{
		// override so check it
		auto& elem = items[idx];
		switch (elem.index & rt_type_mask)
		{
		case rt_struct_index_type:
			{
				auto& this_val = structs[elem.index >> rt_type_shift].second;
				if (this_val.is_array())
					return "Can't override constant value, can not replace array with simple value!";
				if (id != structs[elem.index >> rt_type_shift].first &&
					!rx_internal::model::platform_types_manager::instance().get_simple_type_repository<basic_types::struct_type>().is_derived_from(id, structs[elem.index >> rt_type_shift].first))
					return "Can't override struct, wrong struct type!";
				members_index_type new_idx = static_cast<members_index_type>(structs.size());
				structs.emplace_back(std::move(id), std::move(value));
				struct_blocks.emplace_back(block);
				elem.index = (new_idx << rt_type_shift) | rt_struct_index_type;
			}
			break;
		default:
			return name + " has invalid type to override";
		}
		return true;
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
				auto& this_val = const_values[elem.index >> rt_type_shift];
				if (!check_read_only(value.value_opt, const_values_opts[elem.index >> rt_type_shift]))
					return "Can't override const value wrong read-only value";
				if (this_val.is_array())
					return "Can't override constant value, can not replace array with simple value!";
				if (value.value.get_type() != this_val.get_item()->value.get_type())
					return "Can't override constant value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(variables.size());
				variables.emplace_back(std::move(id), std::move(value));
				elem.index = (new_idx << rt_type_shift) | rt_variable_index_type;
			}
			break;
		case rt_value_index_type:
			{
				auto& this_val = values[elem.index >> rt_type_shift];
				if(!check_read_only(value.value_opt, this_val.get_item()->value_opt))
					return "Can't override value wrong read-only value";
				if (this_val.is_array())
					return "Can't override value, can not replace array with simple value!";
				if (value.value.get_type() != this_val.get_item()->value.get_type())
					return "Can't override value, wrong value type!";
				members_index_type new_idx = static_cast<members_index_type>(variables.size());
				variables.emplace_back(std::move(id), std::move(value));
				elem.index = (new_idx << rt_type_shift) | rt_variable_index_type;
			}
			break;
		case rt_variable_index_type:
			{
				auto& this_val = variables[elem.index >> rt_type_shift].second;
				if (!check_read_only(value.value_opt, this_val.get_item()->value_opt))
					return "Can't override variable wrong read-only value";
				if (this_val.is_array())
					return "Can't override variable, can not replace array with simple value!";
				if (value.value.get_type() != this_val.get_item()->value.get_type())
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
			return name + " has invalid type to override";
		}
		return true;
	}
}

rx_result runtime_data_prototype::add_variable (const string_type& name, std::vector<runtime::structure::variable_data> value, rx_node_id id)
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
				auto& this_val = const_values[elem.index >> rt_type_shift];
				if (!this_val.is_array())
					return "Can't override constant value, can not replace simple value with array!";
				if (value[0].value.get_type() != this_val.get_item(0)->value.get_type())
					return "Can't override constant value, wrong value type!";

				members_index_type new_idx = static_cast<members_index_type>(variables.size());
				variables.emplace_back(std::move(id), std::move(value));
				elem.index = (new_idx << rt_type_shift) | rt_variable_index_type;
			}
			break;
		case rt_value_index_type:
			{
				auto& this_val = values[elem.index >> rt_type_shift];
				if (!this_val.is_array())
					return "Can't override value, can not replace simple value with array!";
				if (value[0].value.get_type() != this_val.get_item(0)->value.get_type())
					return "Can't override value, wrong value type!";
				members_index_type new_idx = static_cast<members_index_type>(variables.size());
				variables.emplace_back(std::move(id), std::move(value));
				elem.index = (new_idx << rt_type_shift) | rt_variable_index_type;
			}
			break;
		case rt_variable_index_type:
			{
				auto& this_val = variables[elem.index >> rt_type_shift].second;
				if (this_val.is_array())
					return "Can't override variable, can not replace array with simple value!";
				if (value[0].value.get_type() != this_val.get_item(0)->value.get_type())
					return "Can't override variable, wrong value type!";
				if (id != variables[elem.index >> rt_type_shift].first &&
					!rx_internal::model::platform_types_manager::instance().get_simple_type_repository<basic_types::variable_type>().is_derived_from(id, variables[elem.index >> rt_type_shift].first))
					return "Can't override variable, wrong variable type!";
				members_index_type new_idx = static_cast<members_index_type>(variables.size());
				variables.emplace_back(std::move(id), std::move(value));
				elem.index = (new_idx << rt_type_shift) | rt_variable_index_type;
			}
			break;
		default:
			return name + " has invalid type to override";
		}
		return true;
	}
}

rx_result runtime_data_prototype::add_source (const string_type& name, runtime::structure::source_data&& value, rx_node_id id, const security::security_guard& sec)
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

rx_result runtime_data_prototype::add_filter (const string_type& name, runtime::structure::filter_data&& value, rx_node_id id)
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
				!rx_internal::model::platform_types_manager::instance().get_simple_type_repository<basic_types::filter_type>().is_derived_from(id, filters[elem.index >> rt_type_shift].first))
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

rx_result runtime_data_prototype::add_event (const string_type& name, runtime::structure::event_data&& value, rx_node_id id)
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
				!rx_internal::model::platform_types_manager::instance().get_simple_type_repository<basic_types::event_type>().is_derived_from(id, events[elem.index >> rt_type_shift].first))
				return "Can't override filter, wrong event type!";
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

rx_result runtime_data_prototype::add_variable_block (const string_type& name, runtime::structure::variable_block_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(variable_blocks.size());
		variable_blocks.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_variable_data_index_type });
		return true;
	}
	else
	{
		return RX_NOT_IMPLEMENTED;
	}
}

rx_result runtime_data_prototype::add_variable_block (const string_type& name, std::vector<runtime::structure::variable_block_data> value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(variable_blocks.size());
		variable_blocks.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_variable_data_index_type });
		return true;
	}
	else
	{
		return RX_NOT_IMPLEMENTED;
	}
}

rx_result runtime_data_prototype::add_value_block (const string_type& name, runtime::structure::value_block_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(blocks.size());
		blocks.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_value_data_index_type });
		return true;
	}
	else
	{
		// override so check it
		auto& elem = items[idx];
		switch (elem.index & rt_type_mask)
		{
			case rt_value_data_index_type:
				{
					auto& this_val = blocks[elem.index >> rt_type_shift];
					if (!check_read_only(value.struct_value.value_opt, this_val.second.get_item()->struct_value.value_opt))
						return "Can't override const value wrong read-only value";
					if (this_val.second.is_array())
						return "Can't override constant value, can not replace array with scalar data!";
					if(!platform_types_manager::instance().get_data_types_repository().is_derived_from(id, blocks[elem.index >> rt_type_shift].first))

						return "Can't override data, wrong data type!";

					members_index_type new_idx = static_cast<members_index_type>(blocks.size());
					blocks.emplace_back(std::move(id), std::move(value));
					elem.index = (new_idx << rt_type_shift) | rt_value_data_index_type;
					return true;
				}
				break;
			case rt_variable_data_index_type:
				{
					auto& this_val = variable_blocks[elem.index >> rt_type_shift];
					if (!check_read_only(value.struct_value.value_opt, this_val.second.get_item()->variable.value_opt))
						return "Can't override variable data wrong read-only value";
					if (this_val.second.is_array())
						return "Can't override variable data, can not replace array with scalar data!";

					if (!platform_types_manager::instance().get_data_types_repository().is_derived_from(id, variable_blocks[elem.index >> rt_type_shift].first))
						return "Can't override variable data, wrong data type!";

					members_index_type new_idx = static_cast<members_index_type>(blocks.size());
					blocks.emplace_back(std::move(id), std::move(value));
					elem.index = (new_idx << rt_type_shift) | rt_value_data_index_type;
					return true;
				}
				break;
			default:
				return name + " has invalid type to override";
		}
	}
}

rx_result runtime_data_prototype::add_value_block (const string_type& name, std::vector<runtime::structure::value_block_data> value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(blocks.size());
		blocks.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_value_data_index_type });
		return true;
	}
	else
	{
		return RX_NOT_IMPLEMENTED;
	}
}

runtime_data_prototype runtime_data_prototype::strip_normalized_prototype ()
{
	runtime_data_prototype ret;
	for (auto& item : items)
	{
		size_t idx = (item.index >> rt_type_shift);
		switch (item.index & rt_type_mask)
		{
		case rt_const_index_type:
			{
				members_index_type new_idx= static_cast<members_index_type>(ret.const_values.size());
				ret.const_values.push_back(std::move(const_values[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_const_index_type });
			}
			break;
		case rt_value_index_type:
			{
				members_index_type new_idx = static_cast<members_index_type>(ret.values.size());
				ret.values.push_back(std::move(values[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_value_index_type });
			}
			break;
		case rt_variable_index_type:
			{
				members_index_type new_idx = static_cast<members_index_type>(ret.variables.size());
				ret.variables.push_back(std::move(variables[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_variable_index_type });
			}
			break;
		case rt_struct_index_type:
			{
				members_index_type new_idx = static_cast<members_index_type>(ret.structs.size());
				ret.structs.push_back(std::move(structs[idx]));
				ret.struct_blocks.push_back(std::move(struct_blocks[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_struct_index_type });
			}
			break;
		case rt_source_index_type:
			{
				members_index_type new_idx = static_cast<members_index_type>(ret.sources.size());
				ret.sources.push_back(std::move(sources[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_source_index_type });
			}
			break;
		case rt_mapper_index_type:
			{
				members_index_type new_idx = static_cast<members_index_type>(ret.mappers.size());
				ret.mappers.push_back(std::move(mappers[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_mapper_index_type });
			}
			break;
		case rt_filter_index_type:
			{
				members_index_type new_idx = static_cast<members_index_type>(ret.filters.size());
				ret.filters.push_back(std::move(filters[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_filter_index_type });
			}
			break;
		case rt_event_index_type:
			{
				members_index_type new_idx = static_cast<members_index_type>(ret.events.size());
				ret.events.push_back(std::move(events[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_event_index_type });
			}
			break;
		case rt_value_data_index_type:
			{
				members_index_type new_idx = static_cast<members_index_type>(ret.blocks.size());
				ret.blocks.push_back(std::move(blocks[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_value_data_index_type });
			}
			break;
		case rt_variable_data_index_type:
			{
				members_index_type new_idx = static_cast<members_index_type>(ret.variable_blocks.size());
				ret.variable_blocks.push_back(std::move(variable_blocks[idx]));
				ret.items.push_back({ item.name, (new_idx << rt_type_shift) | rt_variable_data_index_type });
			}
			break;
		}
	}
	return ret;
}

runtime::structure::block_data runtime_data_prototype::create_block_data ()
{
	block_data_prototype block_proto;
	for (auto& item : items)
	{
		if (!item.name.empty() && item.name[0] != '_')
		{
			size_t idx = (item.index >> rt_type_shift);
			switch (item.index & rt_type_mask)
			{
				case rt_const_index_type:
					{
						if (const_values[idx].is_array())
						{
							block_proto.add_empty_array_value(item.name, const_values[idx].get_prototype()->simple_get_value());
						}
						else
						{
							block_proto.add_value(item.name, const_values[idx].get_item()->simple_get_value());
						}
					}
					break;
				case rt_value_index_type:
					{
						if (values[idx].is_array())
						{
							block_proto.add_empty_array_value(item.name, values[idx].get_prototype()->simple_get_value());
						}
						else
						{
							block_proto.add_value(item.name, values[idx].get_item()->simple_get_value());
						}
					}
					break;
				case rt_variable_index_type:
					{
						if (variables[idx].second.is_array())
						{
							block_proto.add_empty_array_value(item.name, variables[idx].second.get_prototype()->simple_get_value());
						}
						else
						{
							block_proto.add_value(item.name, variables[idx].second.get_item()->simple_get_value());
						}
					}
					break;
				case rt_struct_index_type:
					{
						if (!struct_blocks[idx].items.empty())
						{
							block_data_prototype block_struck;
							if (structs[idx].second.is_array())
							{
								block_proto.add_empty_array(item.name, runtime::structure::block_data(struct_blocks[idx]), rx_node_id());
							}
							else
							{
								block_proto.add(item.name, runtime::structure::block_data(struct_blocks[idx]), rx_node_id());
							}
						}
					}
					break;
				case rt_value_data_index_type:
					{
						if (blocks[idx].second.is_array())
						{
							block_proto.add_empty_array(item.name, runtime::structure::block_data(blocks[idx].second.get_prototype()->block), rx_node_id());
						}
						else
						{
							block_proto.add(item.name, runtime::structure::block_data(blocks[idx].second.get_item()->block), rx_node_id());
						}
					}
					break;
				case rt_variable_data_index_type:
					{
						if (variable_blocks[idx].second.is_array())
						{
							block_proto.add_empty_array(item.name, runtime::structure::block_data(variable_blocks[idx].second.get_prototype()->block), rx_node_id());
						}
						else
						{
							block_proto.add(item.name, runtime::structure::block_data(variable_blocks[idx].second.get_item()->block), rx_node_id());
						}
					}
					break;
				default:
					;
			}
		}
	}
	return block_proto.create_block();
}

template <class runtime_data_type>
runtime_item::smart_ptr create_runtime_data_from_prototype(runtime_data_prototype& prototype)
{
	std::unique_ptr<runtime_data_type> ret = std::make_unique<runtime_data_type>();
	if constexpr (runtime_data_type::has_variables())
		ret->variables.copy_from(std::move(prototype.variables));
	if constexpr (runtime_data_type::has_structs())
		ret->structs.copy_from(std::move(prototype.structs));
	if constexpr (runtime_data_type::has_sources())
		ret->sources.copy_from(std::move(prototype.sources));
	if constexpr (runtime_data_type::has_mappers())
		ret->mappers.copy_from(std::move(prototype.mappers));
	if constexpr (runtime_data_type::has_filters())
		ret->filters.copy_from(std::move(prototype.filters));
	if constexpr (runtime_data_type::has_events())
		ret->events.copy_from(std::move(prototype.events));
	if constexpr (runtime_data_type::has_block_data())
		ret->blocks.copy_from(std::move(prototype.blocks));
	if constexpr (runtime_data_type::has_variable_blocks_data())
		ret->variable_blocks.copy_from(std::move(prototype.variable_blocks));

	ret->values = const_size_vector< array_wrapper<value_data> >(std::move(prototype.values));
	ret->const_values = const_size_vector<array_wrapper<const_value_data> >(std::move(prototype.const_values));
	ret->items = const_size_vector<index_data>(std::move(prototype.items));

	return ret;
}


runtime_item::smart_ptr create_runtime_data(runtime_data_prototype& vprototype)
{
	runtime_data_prototype prototype = vprototype.strip_normalized_prototype();

	uint_fast8_t effective_type = (prototype.variables.empty() ? rt_bit_none : rt_bit_has_variables)
		| (prototype.structs.empty() ? rt_bit_none : rt_bit_has_structs)
		| (prototype.sources.empty() ? rt_bit_none : rt_bit_has_sources)
		| (prototype.mappers.empty() ? rt_bit_none : rt_bit_has_mappers)
		| (prototype.filters.empty() ? rt_bit_none : rt_bit_has_filters)
		| (prototype.events.empty() ? rt_bit_none : rt_bit_has_events)
		| (prototype.blocks.empty() ? rt_bit_none : rt_bit_has_data_blocks)
		| (prototype.variable_blocks.empty() ? rt_bit_none : rt_bit_has_variable_data_blocks);

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
	case 0x40:
		return create_runtime_data_from_prototype<runtime_data_type40>(prototype);
	case 0x41:
		return create_runtime_data_from_prototype<runtime_data_type41>(prototype);
	case 0x42:
		return create_runtime_data_from_prototype<runtime_data_type42>(prototype);
	case 0x43:
		return create_runtime_data_from_prototype<runtime_data_type43>(prototype);
	case 0x44:
		return create_runtime_data_from_prototype<runtime_data_type44>(prototype);
	case 0x45:
		return create_runtime_data_from_prototype<runtime_data_type45>(prototype);
	case 0x46:
		return create_runtime_data_from_prototype<runtime_data_type46>(prototype);
	case 0x47:
		return create_runtime_data_from_prototype<runtime_data_type47>(prototype);
	case 0x48:
		return create_runtime_data_from_prototype<runtime_data_type48>(prototype);
	case 0x49:
		return create_runtime_data_from_prototype<runtime_data_type49>(prototype);
	case 0x4a:
		return create_runtime_data_from_prototype<runtime_data_type4a>(prototype);
	case 0x4b:
		return create_runtime_data_from_prototype<runtime_data_type4b>(prototype);
	case 0x4c:
		return create_runtime_data_from_prototype<runtime_data_type4c>(prototype);
	case 0x4d:
		return create_runtime_data_from_prototype<runtime_data_type4d>(prototype);
	case 0x4e:
		return create_runtime_data_from_prototype<runtime_data_type4e>(prototype);
	case 0x4f:
		return create_runtime_data_from_prototype<runtime_data_type4f>(prototype);
	case 0x50:
		return create_runtime_data_from_prototype<runtime_data_type50>(prototype);
	case 0x51:
		return create_runtime_data_from_prototype<runtime_data_type51>(prototype);
	case 0x52:
		return create_runtime_data_from_prototype<runtime_data_type52>(prototype);
	case 0x53:
		return create_runtime_data_from_prototype<runtime_data_type53>(prototype);
	case 0x54:
		return create_runtime_data_from_prototype<runtime_data_type54>(prototype);
	case 0x55:
		return create_runtime_data_from_prototype<runtime_data_type55>(prototype);
	case 0x56:
		return create_runtime_data_from_prototype<runtime_data_type56>(prototype);
	case 0x57:
		return create_runtime_data_from_prototype<runtime_data_type57>(prototype);
	case 0x58:
		return create_runtime_data_from_prototype<runtime_data_type58>(prototype);
	case 0x59:
		return create_runtime_data_from_prototype<runtime_data_type59>(prototype);
	case 0x5a:
		return create_runtime_data_from_prototype<runtime_data_type5a>(prototype);
	case 0x5b:
		return create_runtime_data_from_prototype<runtime_data_type5b>(prototype);
	case 0x5c:
		return create_runtime_data_from_prototype<runtime_data_type5c>(prototype);
	case 0x5d:
		return create_runtime_data_from_prototype<runtime_data_type5d>(prototype);
	case 0x5e:
		return create_runtime_data_from_prototype<runtime_data_type5e>(prototype);
	case 0x5f:
		return create_runtime_data_from_prototype<runtime_data_type5f>(prototype);
	case 0x60:
		return create_runtime_data_from_prototype<runtime_data_type60>(prototype);
	case 0x61:
		return create_runtime_data_from_prototype<runtime_data_type61>(prototype);
	case 0x62:
		return create_runtime_data_from_prototype<runtime_data_type62>(prototype);
	case 0x63:
		return create_runtime_data_from_prototype<runtime_data_type63>(prototype);
	case 0x64:
		return create_runtime_data_from_prototype<runtime_data_type64>(prototype);
	case 0x65:
		return create_runtime_data_from_prototype<runtime_data_type65>(prototype);
	case 0x66:
		return create_runtime_data_from_prototype<runtime_data_type66>(prototype);
	case 0x67:
		return create_runtime_data_from_prototype<runtime_data_type67>(prototype);
	case 0x68:
		return create_runtime_data_from_prototype<runtime_data_type68>(prototype);
	case 0x69:
		return create_runtime_data_from_prototype<runtime_data_type69>(prototype);
	case 0x6a:
		return create_runtime_data_from_prototype<runtime_data_type6a>(prototype);
	case 0x6b:
		return create_runtime_data_from_prototype<runtime_data_type6b>(prototype);
	case 0x6c:
		return create_runtime_data_from_prototype<runtime_data_type6c>(prototype);
	case 0x6d:
		return create_runtime_data_from_prototype<runtime_data_type6d>(prototype);
	case 0x6e:
		return create_runtime_data_from_prototype<runtime_data_type6e>(prototype);
	case 0x6f:
		return create_runtime_data_from_prototype<runtime_data_type6f>(prototype);
	case 0x70:
		return create_runtime_data_from_prototype<runtime_data_type70>(prototype);
	case 0x71:
		return create_runtime_data_from_prototype<runtime_data_type71>(prototype);
	case 0x72:
		return create_runtime_data_from_prototype<runtime_data_type72>(prototype);
	case 0x73:
		return create_runtime_data_from_prototype<runtime_data_type73>(prototype);
	case 0x74:
		return create_runtime_data_from_prototype<runtime_data_type74>(prototype);
	case 0x75:
		return create_runtime_data_from_prototype<runtime_data_type75>(prototype);
	case 0x76:
		return create_runtime_data_from_prototype<runtime_data_type76>(prototype);
	case 0x77:
		return create_runtime_data_from_prototype<runtime_data_type77>(prototype);
	case 0x78:
		return create_runtime_data_from_prototype<runtime_data_type78>(prototype);
	case 0x79:
		return create_runtime_data_from_prototype<runtime_data_type79>(prototype);
	case 0x7a:
		return create_runtime_data_from_prototype<runtime_data_type7a>(prototype);
	case 0x7b:
		return create_runtime_data_from_prototype<runtime_data_type7b>(prototype);
	case 0x7c:
		return create_runtime_data_from_prototype<runtime_data_type7c>(prototype);
	case 0x7d:
		return create_runtime_data_from_prototype<runtime_data_type7d>(prototype);
	case 0x7e:
		return create_runtime_data_from_prototype<runtime_data_type7e>(prototype);
	case 0x7f:
		return create_runtime_data_from_prototype<runtime_data_type7f>(prototype);
	case 0x80:
		return create_runtime_data_from_prototype<runtime_data_type80>(prototype);
	case 0x81:
		return create_runtime_data_from_prototype<runtime_data_type81>(prototype);
	case 0x82:
		return create_runtime_data_from_prototype<runtime_data_type82>(prototype);
	case 0x83:
		return create_runtime_data_from_prototype<runtime_data_type83>(prototype);
	case 0x84:
		return create_runtime_data_from_prototype<runtime_data_type84>(prototype);
	case 0x85:
		return create_runtime_data_from_prototype<runtime_data_type85>(prototype);
	case 0x86:
		return create_runtime_data_from_prototype<runtime_data_type86>(prototype);
	case 0x87:
		return create_runtime_data_from_prototype<runtime_data_type87>(prototype);
	case 0x88:
		return create_runtime_data_from_prototype<runtime_data_type88>(prototype);
	case 0x89:
		return create_runtime_data_from_prototype<runtime_data_type89>(prototype);
	case 0x8a:
		return create_runtime_data_from_prototype<runtime_data_type8a>(prototype);
	case 0x8b:
		return create_runtime_data_from_prototype<runtime_data_type8b>(prototype);
	case 0x8c:
		return create_runtime_data_from_prototype<runtime_data_type8c>(prototype);
	case 0x8d:
		return create_runtime_data_from_prototype<runtime_data_type8d>(prototype);
	case 0x8e:
		return create_runtime_data_from_prototype<runtime_data_type8e>(prototype);
	case 0x8f:
		return create_runtime_data_from_prototype<runtime_data_type8f>(prototype);
	case 0x90:
		return create_runtime_data_from_prototype<runtime_data_type90>(prototype);
	case 0x91:
		return create_runtime_data_from_prototype<runtime_data_type91>(prototype);
	case 0x92:
		return create_runtime_data_from_prototype<runtime_data_type92>(prototype);
	case 0x93:
		return create_runtime_data_from_prototype<runtime_data_type93>(prototype);
	case 0x94:
		return create_runtime_data_from_prototype<runtime_data_type94>(prototype);
	case 0x95:
		return create_runtime_data_from_prototype<runtime_data_type95>(prototype);
	case 0x96:
		return create_runtime_data_from_prototype<runtime_data_type96>(prototype);
	case 0x97:
		return create_runtime_data_from_prototype<runtime_data_type97>(prototype);
	case 0x98:
		return create_runtime_data_from_prototype<runtime_data_type98>(prototype);
	case 0x99:
		return create_runtime_data_from_prototype<runtime_data_type99>(prototype);
	case 0x9a:
		return create_runtime_data_from_prototype<runtime_data_type9a>(prototype);
	case 0x9b:
		return create_runtime_data_from_prototype<runtime_data_type9b>(prototype);
	case 0x9c:
		return create_runtime_data_from_prototype<runtime_data_type9c>(prototype);
	case 0x9d:
		return create_runtime_data_from_prototype<runtime_data_type9d>(prototype);
	case 0x9e:
		return create_runtime_data_from_prototype<runtime_data_type9e>(prototype);
	case 0x9f:
		return create_runtime_data_from_prototype<runtime_data_type9f>(prototype);
	case 0xa0:
		return create_runtime_data_from_prototype<runtime_data_typea0>(prototype);
	case 0xa1:
		return create_runtime_data_from_prototype<runtime_data_typea1>(prototype);
	case 0xa2:
		return create_runtime_data_from_prototype<runtime_data_typea2>(prototype);
	case 0xa3:
		return create_runtime_data_from_prototype<runtime_data_typea3>(prototype);
	case 0xa4:
		return create_runtime_data_from_prototype<runtime_data_typea4>(prototype);
	case 0xa5:
		return create_runtime_data_from_prototype<runtime_data_typea5>(prototype);
	case 0xa6:
		return create_runtime_data_from_prototype<runtime_data_typea6>(prototype);
	case 0xa7:
		return create_runtime_data_from_prototype<runtime_data_typea7>(prototype);
	case 0xa8:
		return create_runtime_data_from_prototype<runtime_data_typea8>(prototype);
	case 0xa9:
		return create_runtime_data_from_prototype<runtime_data_typea9>(prototype);
	case 0xaa:
		return create_runtime_data_from_prototype<runtime_data_typeaa>(prototype);
	case 0xab:
		return create_runtime_data_from_prototype<runtime_data_typeab>(prototype);
	case 0xac:
		return create_runtime_data_from_prototype<runtime_data_typeac>(prototype);
	case 0xad:
		return create_runtime_data_from_prototype<runtime_data_typead>(prototype);
	case 0xae:
		return create_runtime_data_from_prototype<runtime_data_typeae>(prototype);
	case 0xaf:
		return create_runtime_data_from_prototype<runtime_data_typeaf>(prototype);
	case 0xb0:
		return create_runtime_data_from_prototype<runtime_data_typeb0>(prototype);
	case 0xb1:
		return create_runtime_data_from_prototype<runtime_data_typeb1>(prototype);
	case 0xb2:
		return create_runtime_data_from_prototype<runtime_data_typeb2>(prototype);
	case 0xb3:
		return create_runtime_data_from_prototype<runtime_data_typeb3>(prototype);
	case 0xb4:
		return create_runtime_data_from_prototype<runtime_data_typeb4>(prototype);
	case 0xb5:
		return create_runtime_data_from_prototype<runtime_data_typeb5>(prototype);
	case 0xb6:
		return create_runtime_data_from_prototype<runtime_data_typeb6>(prototype);
	case 0xb7:
		return create_runtime_data_from_prototype<runtime_data_typeb7>(prototype);
	case 0xb8:
		return create_runtime_data_from_prototype<runtime_data_typeb8>(prototype);
	case 0xb9:
		return create_runtime_data_from_prototype<runtime_data_typeb9>(prototype);
	case 0xba:
		return create_runtime_data_from_prototype<runtime_data_typeba>(prototype);
	case 0xbb:
		return create_runtime_data_from_prototype<runtime_data_typebb>(prototype);
	case 0xbc:
		return create_runtime_data_from_prototype<runtime_data_typebc>(prototype);
	case 0xbd:
		return create_runtime_data_from_prototype<runtime_data_typebd>(prototype);
	case 0xbe:
		return create_runtime_data_from_prototype<runtime_data_typebe>(prototype);
	case 0xbf:
		return create_runtime_data_from_prototype<runtime_data_typebf>(prototype);
	case 0xc0:
		return create_runtime_data_from_prototype<runtime_data_typec0>(prototype);
	case 0xc1:
		return create_runtime_data_from_prototype<runtime_data_typec1>(prototype);
	case 0xc2:
		return create_runtime_data_from_prototype<runtime_data_typec2>(prototype);
	case 0xc3:
		return create_runtime_data_from_prototype<runtime_data_typec3>(prototype);
	case 0xc4:
		return create_runtime_data_from_prototype<runtime_data_typec4>(prototype);
	case 0xc5:
		return create_runtime_data_from_prototype<runtime_data_typec5>(prototype);
	case 0xc6:
		return create_runtime_data_from_prototype<runtime_data_typec6>(prototype);
	case 0xc7:
		return create_runtime_data_from_prototype<runtime_data_typec7>(prototype);
	case 0xc8:
		return create_runtime_data_from_prototype<runtime_data_typec8>(prototype);
	case 0xc9:
		return create_runtime_data_from_prototype<runtime_data_typec9>(prototype);
	case 0xca:
		return create_runtime_data_from_prototype<runtime_data_typeca>(prototype);
	case 0xcb:
		return create_runtime_data_from_prototype<runtime_data_typecb>(prototype);
	case 0xcc:
		return create_runtime_data_from_prototype<runtime_data_typecc>(prototype);
	case 0xcd:
		return create_runtime_data_from_prototype<runtime_data_typecd>(prototype);
	case 0xce:
		return create_runtime_data_from_prototype<runtime_data_typece>(prototype);
	case 0xcf:
		return create_runtime_data_from_prototype<runtime_data_typecf>(prototype);
	case 0xd0:
		return create_runtime_data_from_prototype<runtime_data_typed0>(prototype);
	case 0xd1:
		return create_runtime_data_from_prototype<runtime_data_typed1>(prototype);
	case 0xd2:
		return create_runtime_data_from_prototype<runtime_data_typed2>(prototype);
	case 0xd3:
		return create_runtime_data_from_prototype<runtime_data_typed3>(prototype);
	case 0xd4:
		return create_runtime_data_from_prototype<runtime_data_typed4>(prototype);
	case 0xd5:
		return create_runtime_data_from_prototype<runtime_data_typed5>(prototype);
	case 0xd6:
		return create_runtime_data_from_prototype<runtime_data_typed6>(prototype);
	case 0xd7:
		return create_runtime_data_from_prototype<runtime_data_typed7>(prototype);
	case 0xd8:
		return create_runtime_data_from_prototype<runtime_data_typed8>(prototype);
	case 0xd9:
		return create_runtime_data_from_prototype<runtime_data_typed9>(prototype);
	case 0xda:
		return create_runtime_data_from_prototype<runtime_data_typeda>(prototype);
	case 0xdb:
		return create_runtime_data_from_prototype<runtime_data_typedb>(prototype);
	case 0xdc:
		return create_runtime_data_from_prototype<runtime_data_typedc>(prototype);
	case 0xdd:
		return create_runtime_data_from_prototype<runtime_data_typedd>(prototype);
	case 0xde:
		return create_runtime_data_from_prototype<runtime_data_typede>(prototype);
	case 0xdf:
		return create_runtime_data_from_prototype<runtime_data_typedf>(prototype);
	case 0xe0:
		return create_runtime_data_from_prototype<runtime_data_typee0>(prototype);
	case 0xe1:
		return create_runtime_data_from_prototype<runtime_data_typee1>(prototype);
	case 0xe2:
		return create_runtime_data_from_prototype<runtime_data_typee2>(prototype);
	case 0xe3:
		return create_runtime_data_from_prototype<runtime_data_typee3>(prototype);
	case 0xe4:
		return create_runtime_data_from_prototype<runtime_data_typee4>(prototype);
	case 0xe5:
		return create_runtime_data_from_prototype<runtime_data_typee5>(prototype);
	case 0xe6:
		return create_runtime_data_from_prototype<runtime_data_typee6>(prototype);
	case 0xe7:
		return create_runtime_data_from_prototype<runtime_data_typee7>(prototype);
	case 0xe8:
		return create_runtime_data_from_prototype<runtime_data_typee8>(prototype);
	case 0xe9:
		return create_runtime_data_from_prototype<runtime_data_typee9>(prototype);
	case 0xea:
		return create_runtime_data_from_prototype<runtime_data_typeea>(prototype);
	case 0xeb:
		return create_runtime_data_from_prototype<runtime_data_typeeb>(prototype);
	case 0xec:
		return create_runtime_data_from_prototype<runtime_data_typeec>(prototype);
	case 0xed:
		return create_runtime_data_from_prototype<runtime_data_typeed>(prototype);
	case 0xee:
		return create_runtime_data_from_prototype<runtime_data_typeee>(prototype);
	case 0xef:
		return create_runtime_data_from_prototype<runtime_data_typeef>(prototype);
	case 0xf0:
		return create_runtime_data_from_prototype<runtime_data_typef0>(prototype);
	case 0xf1:
		return create_runtime_data_from_prototype<runtime_data_typef1>(prototype);
	case 0xf2:
		return create_runtime_data_from_prototype<runtime_data_typef2>(prototype);
	case 0xf3:
		return create_runtime_data_from_prototype<runtime_data_typef3>(prototype);
	case 0xf4:
		return create_runtime_data_from_prototype<runtime_data_typef4>(prototype);
	case 0xf5:
		return create_runtime_data_from_prototype<runtime_data_typef5>(prototype);
	case 0xf6:
		return create_runtime_data_from_prototype<runtime_data_typef6>(prototype);
	case 0xf7:
		return create_runtime_data_from_prototype<runtime_data_typef7>(prototype);
	case 0xf8:
		return create_runtime_data_from_prototype<runtime_data_typef8>(prototype);
	case 0xf9:
		return create_runtime_data_from_prototype<runtime_data_typef9>(prototype);
	case 0xfa:
		return create_runtime_data_from_prototype<runtime_data_typefa>(prototype);
	case 0xfb:
		return create_runtime_data_from_prototype<runtime_data_typefb>(prototype);
	case 0xfc:
		return create_runtime_data_from_prototype<runtime_data_typefc>(prototype);
	case 0xfd:
		return create_runtime_data_from_prototype<runtime_data_typefd>(prototype);
	case 0xfe:
		return create_runtime_data_from_prototype<runtime_data_typefe>(prototype);
	case 0xff:
		return create_runtime_data_from_prototype<runtime_data_typeff>(prototype);
	}
	return runtime_item::smart_ptr();

}

bool runtime_data_prototype::check_read_only(const std::bitset<32>& to, const std::bitset<32>& from)
{
	return
		to[runtime::structure::value_opt_readonly] ||
		 !from[runtime::structure::value_opt_readonly];
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


const rx_node_id method_data_prototype::get_inputs_id () const
{
  return inputs_id_;
}

void method_data_prototype::set_inputs_id (rx_node_id value)
{
  inputs_id_ = value;
}

const rx_node_id& method_data_prototype::get_outputs_id () const
{
  return outputs_id_;
}

void method_data_prototype::set_outputs_id (const rx_node_id& value)
{
  outputs_id_ = value;
}


// Class rx_platform::meta::program_data_prototype 


// Class rx_platform::meta::display_data_prototype 


// Class rx_platform::meta::dependencies_context 


// Class rx_platform::meta::config_part_container 


rx_result config_part_container::serialize (const string_type& name, base_meta_writer& stream, uint8_t type) const
{
	size_t count = objects.size() + domains.size() + ports.size() + apps.size();
	if (!stream.start_array(name.c_str(), count))
		return stream.get_error();

	for (const auto& one : apps)
	{
		auto result = one->serialize(stream, type);
		if (!result)
			return result;
	}
	for (const auto& one : domains)
	{
		auto result = one->serialize(stream, type);
		if (!result)
			return result;
	}
	for (const auto& one : ports)
	{
		auto result = one->serialize(stream, type);
		if (!result)
			return result;
	}
	for (const auto& one : objects)
	{
		auto result = one->serialize(stream, type);
		if (!result)
			return result;
	}

	return true;
}

rx_result config_part_container::deserialize (const string_type& name, base_meta_reader& stream, uint8_t type)
{
	if (!stream.start_array(name.c_str()))
		return stream.get_error();

	while (!stream.array_end())
	{
		if (!stream.start_object("item"))
			return stream.get_error();
		meta_data meta;
		rx_item_type target_type;
		auto result = meta.deserialize_meta_data(stream, STREAMING_TYPE_OBJECT, target_type);
		if (!result)
		{
			result.register_error("Error building "s + meta.get_full_path());
			return result;
		}
		switch (target_type)
		{
		case rx_item_type::rx_object:
			{
				runtime_data::object_runtime_data data;
				result = data.deserialize(stream, type);
				if (!result)
					return result;
				data.meta_info = std::move(meta);
				objects.emplace_back(std::make_unique<runtime_data::object_runtime_data>(data));
			}
			break;
		case rx_item_type::rx_port:
			{
				runtime_data::port_runtime_data data;
				result = data.deserialize(stream, type);
				if (!result)
					return result;
				data.meta_info = std::move(meta);
				ports.emplace_back(std::make_unique<runtime_data::port_runtime_data>(data));
			}
			break;
		case rx_item_type::rx_domain:
			{
				runtime_data::domain_runtime_data data;
				result = data.deserialize(stream, type);
				if (!result)
					return result;
				data.meta_info = std::move(meta);
				domains.emplace_back(std::make_unique<runtime_data::domain_runtime_data>(data));
			}
			break;
		case rx_item_type::rx_application:
			{
				runtime_data::application_runtime_data data;
				result = data.deserialize(stream, type);
				if (!result)
					return result;
				data.meta_info = std::move(meta);
				apps.emplace_back(std::make_unique<runtime_data::application_runtime_data>(data));
			}
			break;
		case rx_item_type::rx_object_type:
			{
				using algorithm_type = typename object_types::object_type::algorithm_type;

				auto item = rx_create_reference<object_types::object_type>();
				auto result = algorithm_type::deserialize_type(*item, stream, STREAMING_TYPE_TYPE);
				if (!result)
					return result;
				item->meta_info = meta;
				object_types.emplace_back(item);
			}
			break;
		case rx_item_type::rx_port_type:
			{
				using algorithm_type = typename object_types::port_type::algorithm_type;

				auto item = rx_create_reference<object_types::port_type>();
				auto result = algorithm_type::deserialize_type(*item, stream, STREAMING_TYPE_TYPE);
				if (!result)
					return result;
				item->meta_info = meta;
				port_types.emplace_back(item);
			}
			break;
		case rx_item_type::rx_domain_type:
			{
				using algorithm_type = typename object_types::domain_type::algorithm_type;

				auto item = rx_create_reference<object_types::domain_type>();
				auto result = algorithm_type::deserialize_type(*item, stream, STREAMING_TYPE_TYPE);
				if (!result)
					return result;
				item->meta_info = meta;
				domain_types.emplace_back(item);
			}
			break;
		case rx_item_type::rx_application_type:
			{
				using algorithm_type = typename object_types::application_type::algorithm_type;

				auto item = rx_create_reference<object_types::application_type>();
				auto result = algorithm_type::deserialize_type(*item, stream, STREAMING_TYPE_TYPE);
				if (!result)
					return result;
				item->meta_info = meta;
				app_types.emplace_back(item);
			}
			break;
		case rx_item_type::rx_struct_type:
			{
				using algorithm_type = typename basic_types::struct_type::algorithm_type;

				auto item = rx_create_reference<basic_types::struct_type>();
				auto result = algorithm_type::deserialize_type(*item, stream, STREAMING_TYPE_TYPE);
				if (!result)
					return result;
				item->meta_info = meta;
				struct_types.emplace_back(item);
			}
			break;
		case rx_item_type::rx_variable_type:
			{
				using algorithm_type = typename basic_types::variable_type::algorithm_type;

				auto item = rx_create_reference<basic_types::variable_type>();
				auto result = algorithm_type::deserialize_type(*item, stream, STREAMING_TYPE_TYPE);
				if (!result)
					return result;
				item->meta_info = meta;
				variable_types.emplace_back(item);
			}
			break;
		default:
			return RX_NOT_IMPLEMENTED;
		}
		if (!stream.end_object())
			return stream.get_error();
	}
	return true;
}


// Class rx_platform::meta::block_data_prototype 


rx_result block_data_prototype::add_value (const string_type& name, rx_simple_value value)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{// new one
		members_index_type new_idx = static_cast<members_index_type>(values.size());
		runtime::structure::const_value_data temp;
		temp.value = std::move(value);
		values.push_back(std::move(temp));
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
					auto& this_val = values[elem.index >> rt_type_shift];
					if (this_val.is_array())
						return "Can't override constant value, can not replace array with simple value!";
					if (value.get_type() != this_val.get_item()->value.get_type())
						return "Can't override constant value, wrong value type!";

					members_index_type new_idx = static_cast<members_index_type>(values.size());
					runtime::structure::const_value_data temp;
					temp.value = std::move(value);
					values.emplace_back(std::move(temp));
					elem.index = (new_idx << rt_type_shift) | rt_const_index_type;
				}
				break;
			default:
				return name + " has invalid type to override";
		}
		return true;
	}
}

rx_result block_data_prototype::add_empty_array_value (const string_type& name, rx_simple_value value)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{// new one
		members_index_type new_idx = static_cast<members_index_type>(values.size());
		runtime::structure::const_value_data temp_const;
		temp_const.value = std::move(value);
		runtime::structure::array_wrapper<runtime::structure::const_value_data> temp;
		temp.declare_null_array(std::move(temp_const));
		values.push_back(std::move(temp));

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
					auto& this_val = values[elem.index >> rt_type_shift];
					if (!this_val.is_array())
						return "Can't override constant value, can not replace simple value with array!";
					if (value[0].get_type() != this_val.get_item(0)->value.get_type())
						return "Can't override constant value, wrong value type!";

					members_index_type new_idx = static_cast<members_index_type>(values.size());
					runtime::structure::const_value_data temp_const;
					temp_const.value = std::move(value);
					runtime::structure::array_wrapper<runtime::structure::const_value_data> temp;
					temp.declare_null_array(std::move(temp_const));
					values.push_back(std::move(temp));

					elem.index = (new_idx << rt_type_shift) | rt_const_index_type;
				}
				break;
			default:
				return name + " has invalid type to override";
		}
		return true;
	}
}

rx_result block_data_prototype::add_value (const string_type& name, std::vector<values::rx_simple_value> value)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{// new one
		members_index_type new_idx = static_cast<members_index_type>(values.size());
		std::vector<runtime::structure::const_value_data> temp_array;
		int size = (int)value.size();
		if (size > 0)
		{
			for (int i = 0; i < size; i++)
			{
				runtime::structure::const_value_data temp;
				temp.value = std::move(value[i]);
				temp_array.push_back(std::move(temp));
			}
		}
		values.emplace_back(std::move(temp_array));
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
					auto& this_val = values[elem.index >> rt_type_shift];
					if (!this_val.is_array())
						return "Can't override constant value, can not replace simple value with array!";
					if (value[0].get_type() != this_val.get_item(0)->value.get_type())
						return "Can't override constant value, wrong value type!";

					members_index_type new_idx = static_cast<members_index_type>(values.size());

					std::vector<runtime::structure::const_value_data> temp_array;
					int size = (int)value.size();
					if (size > 0)
					{
						for (int i = 0; i < size; i++)
						{
							runtime::structure::const_value_data temp;
							temp.value = std::move(value[i]);
							temp_array.push_back(std::move(temp));
						}
					}
					values.emplace_back(std::move(temp_array));
					elem.index = (new_idx << rt_type_shift) | rt_const_index_type;
				}
				break;
			default:
				return name + " has invalid type to override";
		}
		return true;
	}
}

rx_result block_data_prototype::add (const string_type& name, runtime::structure::block_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(children.size());
		children.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_data_index_type });
		return true;
	}
	else
	{
		// override so check it
		auto& elem = items[idx];
		// here i should check about the sub-classing
		if (((elem.index & rt_type_mask) == rt_data_index_type))
		{
			auto& this_val = children[elem.index >> rt_type_shift];

			if (this_val.second.is_array())
				return "Can't override type, can not replace array data!";

			if (id != children[elem.index >> rt_type_shift].first &&
				!rx_internal::model::platform_types_manager::instance().get_data_types_repository().is_derived_from(id, children[elem.index >> rt_type_shift].first))
				return "Can't override data, wrong data type!";
			members_index_type new_idx = static_cast<members_index_type>(children.size());
			children.emplace_back(std::move(id), std::move(value));
			elem.index = (new_idx << rt_type_shift) | rt_data_index_type;
			return true;
		}
		else
		{
			return name + " has invalid type for Data to override";
		}
	}
}

rx_result block_data_prototype::add_empty_array (const string_type& name, runtime::structure::block_data&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(children.size());

		runtime::structure::array_wrapper<runtime::structure::block_data> temp;
		temp.declare_null_array(std::move(value));

		children.emplace_back(std::move(id), std::move(temp));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_data_index_type });
		return true;
	}
	else
	{
		// override so check it
		auto& elem = items[idx];
		// here i should check about the sub-classing
		if (((elem.index & rt_type_mask) == rt_data_index_type))
		{
			auto& this_val = children[elem.index >> rt_type_shift];

			if (!this_val.second.is_array())
				return "Can't override type, can not replace regular data with array!";

			if (id != children[elem.index >> rt_type_shift].first &&
				!rx_internal::model::platform_types_manager::instance().get_data_types_repository().is_derived_from(id, children[elem.index >> rt_type_shift].first))
				return "Can't override struct, wrong struct type!";
			members_index_type new_idx = static_cast<members_index_type>(children.size());

			runtime::structure::array_wrapper<runtime::structure::block_data> temp;
			temp.declare_null_array(std::move(value));

			children.emplace_back(std::move(id), std::move(temp));
			elem.index = (new_idx << rt_type_shift) | rt_data_index_type;
			return true;
		}
		else
		{
			return name + " has invalid type for Data to override";
		}
	}
}

rx_result block_data_prototype::add (const string_type& name, std::vector<runtime::structure::block_data>&& value, rx_node_id id)
{
	auto idx = check_member_name(name);
	if (idx < 0)
	{
		members_index_type new_idx = static_cast<members_index_type>(children.size());
		children.emplace_back(std::move(id), std::move(value));
		items.push_back({ name, (new_idx << rt_type_shift) | rt_data_index_type });
		return true;
	}
	else
	{
		// override so check it
		auto& elem = items[idx];
		switch (elem.index & rt_type_mask)
		{
			case rt_data_index_type:
				{
					auto& this_val = children[elem.index >> rt_type_shift].second;
					if (this_val.is_array())
						return "Can't override constant value, can not replace array with simple value!";
					if (id != children[elem.index >> rt_type_shift].first &&
						!rx_internal::model::platform_types_manager::instance().get_data_types_repository().is_derived_from(
							id, children[elem.index >> rt_type_shift].first))
						return "Can't override variable, wrong variable type!";
					members_index_type new_idx = static_cast<members_index_type>(children.size());
					children.emplace_back(std::move(id), std::move(value));
					elem.index = (new_idx << rt_type_shift) | rt_data_index_type;
				}
				break;
			default:
				return name + " has invalid type to override";
		}
		return true;
	}
}

int block_data_prototype::check_member_name (const string_type& name) const
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

runtime::structure::block_data block_data_prototype::create_block ()
{

	std::vector<runtime::structure::index_data> loc_items;
	std::vector<runtime::structure::array_wrapper<runtime::structure::const_value_data> > loc_values;
	std::vector<runtime::structure::array_wrapper<runtime::structure::block_data> > loc_children;

	for (const auto& one : items)
	{
		size_t idx = (one.index >> rt_type_shift);
		switch (one.index & rt_type_mask)
		{
			case rt_const_index_type:
				{
					members_index_type new_idx = static_cast<members_index_type>(loc_values.size());
					loc_values.push_back(std::move(values[idx]));
					loc_items.push_back({ one.name, (new_idx << rt_type_shift) | rt_const_index_type });
				}
				break;
			case rt_data_index_type:
				{
					members_index_type new_idx = static_cast<members_index_type>(loc_children.size());
					loc_children.push_back(std::move(children[idx].second));
					loc_items.push_back({ one.name, (new_idx << rt_type_shift) | rt_data_index_type });
				}
				break;
			default:
				RX_ASSERT(false);
		}

	}

	runtime::structure::block_data what;
	what.items = const_size_vector<runtime::structure::index_data>(std::move(loc_items));
	what.children = const_size_vector<runtime::structure::array_wrapper<runtime::structure::block_data> >(std::move(loc_children));
	what.values = const_size_vector<runtime::structure::array_wrapper<runtime::structure::const_value_data>>(std::move(loc_values));
	return what;
}


} // namespace meta
} // namespace rx_platform

