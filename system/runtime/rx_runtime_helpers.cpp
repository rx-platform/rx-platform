

/****************************************************************************
*
*  system\runtime\rx_runtime_helpers.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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

#include "system/server/rx_log.h"

// rx_ns_resolver
#include "system/server/rx_ns_resolver.h"
// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_operational
#include "system/runtime/rx_operational.h"
// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_relations
#include "system/runtime/rx_relations.h"
// rx_runtime_holder
#include "system/runtime/rx_runtime_holder.h"
// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"

#include "system/runtime/rx_blocks.h"
#include "rx_configuration.h"
#include "sys_internal/rx_inf.h"
#include "rx_rt_item_types.h"


namespace rx_platform {

namespace runtime {

// Class rx_platform::runtime::io_capabilities


void io_capabilities::set_input (bool val)
{
    settings_[0] = val;
}

void io_capabilities::set_output (bool val)
{
    settings_[1] = val;
}

bool io_capabilities::get_input () const
{
    return settings_.test(0);
}

bool io_capabilities::get_output () const
{
    return settings_.test(1);
}

void io_capabilities::set_complex (bool val)
{
	settings_[2] = val;
}

bool io_capabilities::get_complex () const
{
	return settings_.test(2);
}

void io_capabilities::set_in_method (bool val)
{
	settings_[3] = val;
}

bool io_capabilities::get_in_method () const
{
	return settings_.test(3);
}

void io_capabilities::set_in_event (bool val)
{
	settings_[4] = val;
}

bool io_capabilities::get_in_event () const
{
	return settings_.test(4);
}


// Class rx_platform::runtime::runtime_deinit_context

runtime_deinit_context::runtime_deinit_context (const meta_data& meta_data)
      : meta(meta_data)
{
}



// Class rx_platform::runtime::runtime_init_context

runtime_init_context::runtime_init_context (structure::runtime_item& root, const meta_data& meta, runtime_process_context* context, tag_blocks::binded_tags* binded, ns::rx_directory_resolver* directories, rx_item_type type)
      : context(context),
        next_handle_(0x80000000),
        meta(meta),
        directories(directories),
        item_type(type),
        method(nullptr),
        event(nullptr)
    , structure(root)
    , tags(binded)
{
}



runtime_handle_t runtime_init_context::get_new_handle ()
{
	return next_handle_++;
}

rx_result_with<runtime_handle_t> runtime_init_context::bind_item (const string_type& path, tag_blocks::binded_callback_t callback)
{
	return tags->bind_item(path, *this, callback);
}

rx_result_with<runtime_handle_t> runtime_init_context::bind_item (const string_type& path, tag_blocks::binded_callback_t callback, tag_blocks::write_callback_t write_callback)
{
	return tags->bind_item_with_write(path, *this, callback, write_callback);
}

rx_result runtime_init_context::set_item (const string_type& path, rx_simple_value&& value)
{
	return tags->set_item(path, std::move(value), *this);
}

rx_result runtime_init_context::get_item (const string_type& path, rx_simple_value& val)
{
	return tags->get_item(path, val, *this);
}

rx_result_with<runtime_handle_t> runtime_init_context::connect_item (const string_type& path, uint32_t rate, tag_blocks::binded_callback_t callback)
{
	if(!path.empty() && path[0]=='.')
		return tags->connect_item("."s + path, rate, *this, callback);

	return tags->connect_item(path, rate, *this, callback);
}

rx_time runtime_init_context::now ()
{
	return context->now();
}

data::runtime_data_model runtime_init_context::get_data_model (string_view_type path)
{
	return tags->get_data_model(string_type(path), structure);
}


// Class rx_platform::runtime::runtime_path_resolver


void runtime_path_resolver::push_to_path (const string_type& name)
{
	if (!path_.empty())
		path_ += RX_OBJECT_DELIMETER;
	path_ += name;
}

void runtime_path_resolver::pop_from_path ()
{
	RX_ASSERT(!path_.empty());
	if (!path_.empty())
	{
		size_t idx = path_.rfind(RX_OBJECT_DELIMETER);
		if (idx == string_type::npos)
		{
			path_.clear();
		}
		else
		{
			path_.resize(idx);
		}
	}
}

const string_type& runtime_path_resolver::get_current_path () const
{
	return path_;
}

string_type runtime_path_resolver::get_parent_path (size_t level) const
{
	if (!path_.empty() && level > 0)
	{
		size_t idx = path_.rfind(RX_OBJECT_DELIMETER);
		if (idx == string_type::npos)
		{
			return "";
		}
		else
		{
			string_type ret(path_.substr(0, idx));
			level--;
			size_t off_idx = idx - 1;
			while (!ret.empty() && level > 0 && off_idx > 0)
			{
				off_idx = path_.rfind(RX_OBJECT_DELIMETER, off_idx - 1);
				if (off_idx == string_type::npos)
					return "";
				else
					ret.resize(off_idx);
				off_idx--;
			}
			return ret;
		}
	}
	return "";
}


// Class rx_platform::runtime::runtime_start_context

runtime_start_context::runtime_start_context (structure::runtime_item& root, runtime_process_context* context, tag_blocks::binded_tags* binded, ns::rx_directory_resolver* directories, relations::relations_holder* relations, threads::job_thread* jobs_queue)
      : context(context),
        relations_(relations),
        tags(binded),
        directories(directories),
        simulation(false),
        queue(jobs_queue),
        method(nullptr)
    , structure(root)
{
}



runtime_handle_t runtime_start_context::connect (const string_type& path, uint32_t rate, std::function<void(const values::rx_value&)> callback)
{
	return context->connect(path, rate, callback, *this);
}

rx_result runtime_start_context::register_relation_subscriber (const string_type& name, relation_subscriber* who)
{
	return relations_->register_relation_subscriber(name, who);
}

rx_result runtime_start_context::register_extern_relation_subscriber (const string_type& name, relation_subscriber_data* who)
{
	return relations_->register_extern_relation_subscriber(name, who);
}

rx_result runtime_start_context::set_item (const string_type& path, rx_simple_value&& value)
{
	return tags->set_item(path, std::move(value), *this);
}

rx_result runtime_start_context::get_item (const string_type& path, rx_simple_value& val)
{
	return tags->get_item(path, val, *this);
}

void runtime_start_context::add_periodic_job (jobs::periodic_job::smart_ptr job)
{
	rx_internal::infrastructure::server_runtime::instance().append_timer_job(job, queue);
}

void runtime_start_context::add_calc_periodic_job (jobs::periodic_job::smart_ptr job)
{
	rx_internal::infrastructure::server_runtime::instance().append_calculation_job(job, queue);
}

void runtime_start_context::add_io_periodic_job (jobs::periodic_job::smart_ptr job)
{
	rx_internal::infrastructure::server_runtime::instance().append_timer_io_job(job);
}

rx_value& runtime_start_context::get_current_variable_value ()
{
	static rx_value g_null_value;
	auto entry = variables.get_current_variable();

	if (entry.index() == 0 && std::get<0>(entry))
	{
		return std::get<0>(entry)->value;
	}
	else if (entry.index() == 1 && std::get<1>(entry))
	{
		return std::get<1>(entry)->variable.value;
	}
	return g_null_value;
}

rx_time runtime_start_context::now ()
{
	return context->now();
}


// Class rx_platform::runtime::runtime_stop_context

runtime_stop_context::runtime_stop_context (const meta_data& meta_data, runtime_process_context* context)
      : context(context),
        meta(meta_data)
{
}



// Class rx_platform::runtime::runtime_structure_resolver

runtime_structure_resolver::runtime_structure_resolver (structure::runtime_item& root)
      : root_(root)
{
}



void runtime_structure_resolver::push_item (structure::runtime_item& item)
{
	items_.push(item);
}

void runtime_structure_resolver::pop_item ()
{
	RX_ASSERT(!items_.empty());
	if (!items_.empty())
		items_.pop();
}

structure::runtime_item& runtime_structure_resolver::get_current_item ()
{
	RX_ASSERT(!items_.empty());
	static structure::empty_item_data g_empty;
	if (!items_.empty())
		return items_.top();
	else
		return g_empty;
}

structure::runtime_item& runtime_structure_resolver::get_root ()
{
    return root_;
}

data::runtime_data_model runtime_structure_resolver::get_data_type (const string_type& path)
{
	auto it = data_types_.find(path);
	if (it != data_types_.end())
		return it->second;
	else
		return data::runtime_data_model();
}

void runtime_structure_resolver::register_data_type (const string_type& path, data::runtime_data_model data)
{
	data_types_.emplace(path, data);
}


// Class rx_platform::runtime::variables_stack


void variables_stack::push_variable (structure::variable_data* what)
{
	variables_.push(what);
}

void variables_stack::push_variable (structure::variable_block_data* what)
{
	variables_.push(what);
}

void variables_stack::pop_variable ()
{
	if (!variables_.empty())
		variables_.pop();
}

variable_stack_entry variables_stack::get_current_variable () const
{
	if (!variables_.empty())
		return variables_.top();
	else
		return variable_stack_entry();
}


// Class rx_platform::runtime::mappers_stack


void mappers_stack::push_mapper (const rx_node_id& id, structure::mapper_data* what)
{
	auto it = mappers_.find(id);
	if (it != mappers_.end())
	{
		it->second.push_back(what);
	}
	else
	{
		auto result = mappers_.emplace(id, mappers_type::mapped_type());
		result.first->second.push_back(what);
	}
}

void mappers_stack::pop_mapper (const rx_node_id& id)
{
	auto it = mappers_.find(id);
	if (it != mappers_.end())
	{
		it->second.pop_back();
	}
	else
	{
		RX_ASSERT(false);
	}
}

std::vector<rx_simple_value> mappers_stack::get_mapping_values (const rx_node_id& id, const string_type& path)
{
	std::vector<rx_simple_value> ret_value;
	auto it = mappers_.find(id);
	if (it != mappers_.end() && !it->second.empty())
	{
		for (const auto& one : it->second)
		{
			rx_simple_value val;
			if (one->item->get_local_value(path, val))
				ret_value.emplace_back(std::move(val));
		}
	}
	return ret_value;
}


// Class rx_platform::runtime::sources_stack


void sources_stack::push_source (const rx_node_id& id, structure::source_data* what)
{
	auto it = sources_.find(id);
	if (it != sources_.end())
	{
		it->second.push_back(what);
	}
	else
	{
		auto result = sources_.emplace(id, sources_type::mapped_type());
		result.first->second.push_back(what);
	}
}

void sources_stack::pop_source (const rx_node_id& id)
{
	auto it = sources_.find(id);
	if (it != sources_.end())
	{
		it->second.pop_back();
	}
	else
	{
		RX_ASSERT(false);
	}
}

std::vector<rx_simple_value> sources_stack::get_source_values (const rx_node_id& id, const string_type& path)
{
	std::vector<rx_simple_value> ret_value;
	auto it = sources_.find(id);
	if (it != sources_.end() && !it->second.empty())
	{
		for (const auto& one : it->second)
		{
			rx_simple_value val;
			if (one->item->get_local_value(path, val))
				ret_value.emplace_back(std::move(val));
		}
	}
	return ret_value;
}


} // namespace runtime
} // namespace rx_platform

