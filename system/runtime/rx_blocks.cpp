

/****************************************************************************
*
*  system\runtime\rx_blocks.cpp
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


// rx_blocks
#include "system/runtime/rx_blocks.h"

#include "sys_internal/rx_internal_ns.h"
#include "lib/rx_ser_lib.h"
#include "system/serialization/rx_ser.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace runtime {

namespace blocks {

// Class rx_platform::runtime::blocks::filter_runtime 

string_type filter_runtime::type_name = RX_CPP_FILTER_TYPE_NAME;

filter_runtime::filter_runtime()
{
}



string_type filter_runtime::get_type_name () const
{
  return type_name;

}

rx_result filter_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result filter_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result filter_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result filter_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::mapper_runtime 

string_type mapper_runtime::type_name = RX_CPP_MAPPER_TYPE_NAME;

mapper_runtime::mapper_runtime()
{
}


mapper_runtime::~mapper_runtime()
{
}



string_type mapper_runtime::get_type_name () const
{
  return type_name;

}

rx_result mapper_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return "Not implemented for mapper_runtime at "s + ctx.path.get_current_path();
}

rx_result mapper_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return "Not implemented for mapper_runtime";
}

rx_result mapper_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return "Not implemented for mapper_runtime at "s + ctx.path.get_current_path();
}

rx_result mapper_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return "Not implemented for mapper_runtime";
}


// Class rx_platform::runtime::blocks::source_runtime 

string_type source_runtime::type_name = RX_CPP_SOURCE_TYPE_NAME;

source_runtime::source_runtime()
      : input_(false),
        output_(false)
{
}


source_runtime::~source_runtime()
{
}



string_type source_runtime::get_type_name () const
{
  return type_name;

}

rx_result source_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	my_variable_ = ctx.variables.get_current_variable();
	return true;
}

rx_result source_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	my_variable_ = rx_variable_ptr::null_ptr;
	return true;
}

rx_result source_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result source_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::struct_runtime 

string_type struct_runtime::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_runtime::struct_runtime()
{
}

struct_runtime::struct_runtime (const string_type& name, const rx_node_id& id, bool system)
{
}



bool struct_runtime::serialize_definition (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!stream.start_object(struct_runtime::type_name.c_str()))
		return false;/*
	if (!complex_runtime_item::serialize_definition(stream, type, ts, mode))
		return false;*/
	if (!stream.end_object())
		return false;
	return true;
}

bool struct_runtime::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	RX_ASSERT(false);//
	return true;
}

string_type struct_runtime::get_type_name () const
{
  return type_name;

}

rx_result struct_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result struct_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result struct_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result struct_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::variable_runtime 

string_type variable_runtime::type_name = RX_CPP_VARIABLE_TYPE_NAME;

variable_runtime::variable_runtime()
{
}

variable_runtime::variable_runtime (const string_type& name, const rx_node_id& id, bool system)
{
}


variable_runtime::~variable_runtime()
{
}



string_type variable_runtime::get_type_name () const
{
  return type_name;

}

rx_result variable_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result variable_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return "Not implemented for variable_runtime";
}

rx_result variable_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result variable_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return "Not implemented for variable_runtime";
}


// Class rx_platform::runtime::blocks::event_runtime 

string_type event_runtime::type_name = RX_CPP_EVENT_TYPE_NAME;

event_runtime::event_runtime()
{
}



string_type event_runtime::get_type_name () const
{
  return type_name;

}

rx_result event_runtime::initialize_runtime (runtime::runtime_init_context& ctx)
{
	return true;
}

rx_result event_runtime::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	return true;
}

rx_result event_runtime::start_runtime (runtime::runtime_start_context& ctx)
{
	return true;
}

rx_result event_runtime::stop_runtime (runtime::runtime_stop_context& ctx)
{
	return true;
}


// Class rx_platform::runtime::blocks::runtime_holder 


rx_result runtime_holder::read_value (const string_type& path, rx_value& val) const
{
	auto state = get_object_state();
	return item_->get_value(state, path, val);
}

rx_result runtime_holder::write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx)
{
	if (path.empty())
	{// our value
		return RX_ACCESS_DENIED;
	}
	std::function<rx_result(const string_type&, rx_simple_value)> func = [this, ctx](const string_type& path, rx_simple_value val)
	{
		structure::write_context my_ctx = structure::write_context::create_write_context(this);
		return item_->write_value(path, std::move(val), my_ctx);
	};
	if (ctx.object && ctx.object.unsafe_ptr())
		return func(path, std::move(val));
	else
	{
		rx_do_with_callback<rx_result, decltype(ctx.object), const string_type&, rx_simple_value>(func, 0, callback, ctx.object, path, std::move(val));
		return true;
	}
}

bool runtime_holder::serialize (base_meta_writer& stream, uint8_t type) const
{
	data::runtime_values_data temp_data;
	item_->collect_data(temp_data);
	if (!stream.write_init_values("values", temp_data))
		return false;

	if (!stream.start_array("programs", programs_.size()))
		return false;
	for (const auto& one : programs_)
	{
		if (!one->save_program(stream, type))
			return false;
	}
	if (!stream.end_array())
		return false;
	return true;
}

bool runtime_holder::deserialize (base_meta_reader& stream, uint8_t type)
{
	data::runtime_values_data temp_data;
	if (!stream.read_init_values("values", temp_data))
		return false;
	structure::init_context ctx;
	item_->fill_data(temp_data, ctx);

	if (!stream.start_array("programs"))
		return false;
	while (!stream.array_end())
	{
		logic::ladder_program::smart_ptr one(pointers::_create_new);
		if (!one->load_program(stream, type))
			return false;
		programs_.push_back(one);
	}
	return true;
}

rx_result runtime_holder::initialize_runtime (runtime::runtime_init_context& ctx)
{
	ctx.structure.set_root(this);
	ctx.structure.push_item(*item_);
	ctx.tags = &binded_tags_;
	auto result = item_->initialize_runtime(ctx);
	if (result)
	{
		for (auto& one : programs_)
		{
			result = one->initialize_runtime(ctx);
			if (!result)
				break;
		}
	}
	ctx.structure.pop_item();
	return result;
}

rx_result runtime_holder::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
	rx_result result;
	for (auto& one : programs_)
	{
		result = one->deinitialize_runtime(ctx);
		if (!result)
			break;
	}
	if (result)
	{
		result = item_->deinitialize_runtime(ctx);
	}

	return result;
}

rx_result runtime_holder::start_runtime (runtime::runtime_start_context& ctx)
{
	ctx.structure.set_root(this);
	ctx.structure.push_item(*item_);
	auto result = item_->start_runtime(ctx);
	if (result)
	{
		for (auto& one : programs_)
		{
			result = one->start_runtime(ctx);
			if (!result)
				break;
		}
	}
	ctx.structure.pop_item();
	return result;
}

rx_result runtime_holder::stop_runtime (runtime::runtime_stop_context& ctx)
{
	rx_result result;
	for (auto& one : programs_)
	{
		result = one->stop_runtime(ctx);
		if (!result)
			break;
	}
	if (result)
	{
		result = item_->stop_runtime(ctx);
	}
	return result;
}

rx_result runtime_holder::connect_items (const string_array& paths, std::vector<rx_result_with<runtime_handle_t> >& results, bool& has_errors)
{
	if (paths.empty())
		return true;
	results.clear();// just in case
	results.reserve(paths.size());
	has_errors = false;
	for (const auto& path : paths)
	{
		auto one_result = connected_tags_.connect_tag(path, this);
		if (!has_errors && !one_result)
			has_errors = true;
		results.emplace_back(std::move(one_result));
	}
	return true;
}

rx_result runtime_holder::disconnect_items (const std::vector<runtime_handle_t>& items, std::vector<rx_result>& results, bool& has_errors)
{
	if (items.empty())
		return true;
	results.clear();// just in case
	results.reserve(items.size());
	has_errors = false;
	for (const auto& handle : items)
	{
		auto one_result = connected_tags_.disconnect_tag(handle);
		if (!has_errors && !one_result)
			has_errors = true;
		results.emplace_back(std::move(one_result));
	}
	return true;
}

rx_result runtime_holder::do_command (rx_object_command_t command_type)
{
	switch (command_type)
	{
	case rx_turn_off:
		{
			if (mode_.turn_off())
			{
				change_time_ = rx_time::now();
				item_->object_state_changed({ mode_, change_time_, this });
			}
		}
		break;
	case rx_turn_on:
		{
			if (mode_.turn_on())
			{
				change_time_ = rx_time::now();
				item_->object_state_changed({ mode_, change_time_, this });
			}
		}
		break;
	case rx_set_blocked:
		{
			if (mode_.set_blocked())
			{
				change_time_ = rx_time::now();
				item_->object_state_changed({ mode_, change_time_, this });
			}
		}
		break;
	case rx_reset_blocked:
		{
			if (mode_.reset_blocked())
			{
				change_time_ = rx_time::now();
				item_->object_state_changed({ mode_, change_time_, this });
			}

		}
		break;
	case rx_set_test:
		{
			if (mode_.set_test())
			{
				change_time_ = rx_time::now();
				item_->object_state_changed({ mode_, change_time_, this });
			}
		}
		break;
	case rx_reset_test:
		{
			if (mode_.reset_test())
			{
				change_time_ = rx_time::now();
				item_->object_state_changed({ mode_, change_time_, this });
			}
		}
		break;
	}
	return true;
}

void runtime_holder::set_runtime_data (meta::runtime_data_prototype& prototype)
{
	item_ = std::move(create_runtime_data(prototype));
}

structure::hosting_object_data runtime_holder::get_object_state () const
{
	return structure::hosting_object_data{ mode_, change_time_, this };
}

void runtime_holder::fill_data (const data::runtime_values_data& data)
{
	auto ctx = structure::init_context::create_initialization_context(this);
	item_->fill_data(data, ctx);
}

void runtime_holder::collect_data (data::runtime_values_data& data) const
{
	item_->collect_data(data);
}

rx_result runtime_holder::get_value_ref (const string_type& path, rt_value_ref& ref)
{
	return item_->get_value_ref(path, ref);
}

void runtime_holder::process_runtime (jobs::job_ptr next_job)
{
}


} // namespace blocks
} // namespace runtime
} // namespace rx_platform

