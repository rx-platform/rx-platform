

/****************************************************************************
*
*  system\runtime\rx_tag_blocks.cpp
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


// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_tag_blocks
#include "system/runtime/rx_tag_blocks.h"

#include "runtime_internal/rx_value_point.h"


namespace rx_platform {

namespace runtime {

namespace tag_blocks {

// Class rx_platform::runtime::tag_blocks::tags_holder 


rx_result tags_holder::get_value (string_view_type path, rx_value& val, runtime_process_context* ctx) const
{
	auto result = item_->get_value(path, val, const_cast<runtime_process_context*>(ctx));
	return result;
}

rx_result tags_holder::get_struct_value (string_view_type path, data::runtime_values_data& data, runtime_value_type type, runtime_process_context* ctx) const
{
	const structure::runtime_item* item = nullptr;
	if(!path.empty())
		item = item_->get_child_item(path);
	if (!item)
		return "Invalid path";
	item->collect_data(data, type);
	return true;
}

void tags_holder::fill_data (const data::runtime_values_data& data, runtime_process_context* ctx)
{
	item_->fill_data(data);
}

void tags_holder::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
	item_->collect_data(data, type);
}

rx_result tags_holder::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx)
{
	return item_->browse_items(prefix, path, filter, items, ctx);
}

rx_result tags_holder::initialize_runtime (runtime_init_context& ctx, relations::relations_holder* relations)
{
	ctx.structure.push_item(*item_);
	auto result = item_->initialize_runtime(ctx);
	if (result)
	{
		connected_tags_.init_tags(ctx.context, relations);

		result = common_tags_.initialize_runtime(ctx);
	}
	return result;
}

rx_result tags_holder::deinitialize_runtime (runtime_deinit_context& ctx)
{
	auto result = item_->deinitialize_runtime(ctx);
	return result;
}

rx_result tags_holder::start_runtime (runtime_start_context& ctx)
{
	ctx.structure.push_item(*item_);
	auto result = item_->start_runtime(ctx);
	return result;
}

rx_result tags_holder::stop_runtime (runtime_stop_context& ctx)
{
	auto result = item_->stop_runtime(ctx);
	connected_tags_.runtime_stopped(rx_time::now());
	return result;
}

bool tags_holder::serialize (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool tags_holder::deserialize (base_meta_reader& stream, uint8_t type)
{
	return false;
}

void tags_holder::set_runtime_data (structure::runtime_item::smart_ptr&& prototype)
{
	item_ = std::move(prototype);
}

bool tags_holder::is_this_yours (string_view_type path) const
{
	return item_->is_this_yours(path);
}

void tags_holder::target_relation_removed (relations::relation_data::smart_ptr what)
{
	connected_tags_.target_relation_removed(std::move(what));
}

rx_result tags_holder::get_value_ref (string_view_type path, rt_value_ref& ref)
{
	return item_->get_value_ref(path, ref);
}


// Class rx_platform::runtime::tag_blocks::common_runtime_tags 

common_runtime_tags::common_runtime_tags()
      : last_scan_time_(-1),
        max_scan_time_(0),
        loop_count_(0),
        on_(false),
        test_(false),
        blocked_(false),
        simulate_(false),
        queues_size_(0)
{
}



rx_result common_runtime_tags::initialize_runtime (runtime_init_context& ctx)
{
	auto bind_result = last_scan_time_.bind("Object.LastScanTime", ctx);
	if (bind_result)
	{
		bind_result = max_scan_time_.bind("Object.MaxScanTime", ctx);
		if (bind_result)
		{
			bind_result = loop_count_.bind("Object.LoopCount", ctx);
			if (bind_result)
			{
				bind_result = on_.bind("Object.On", ctx);
				if (bind_result)
				{
					bind_result = test_.bind("Object.Test", ctx);
					if (bind_result)
					{
						bind_result = blocked_.bind("Object.Blocked", ctx);
						if (bind_result)
						{
							bind_result = simulate_.bind("Object.Simulate", ctx);
							if (bind_result)
							{
								bind_result = queues_size_.bind("Object.ProcessQueues", ctx);
							}
						}
					}
				}
			}
		}
	}
	if (!bind_result)
		bind_result.register_error("Unable to bind to common tag.");
	return bind_result;
}


} // namespace tag_blocks
} // namespace runtime
} // namespace rx_platform

