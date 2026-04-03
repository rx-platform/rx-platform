

/****************************************************************************
*
*  system\runtime\rx_event_manager.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


// rx_event_manager
#include "system/runtime/rx_event_manager.h"



namespace rx_platform {

namespace runtime {

namespace events {

// Class rx_platform::runtime::events::runtime_events_manager 

runtime_events_manager::runtime_events_manager()
	: physical_job_thread("EventsThread", RX_DOMAIN_EXTERN)
{
}


runtime_events_manager::~runtime_events_manager()
{
}



void runtime_events_manager::event_fired (fired_event_data data)
{
	std::function<void(fired_event_data)> what = [this](fired_event_data data) mutable
		{
			auto type_cache = std::move(data.types);
			string_type full_path(data.path + '|' + data.queue);
			if (data.state_event)
			{
				auto event_it = event_cache_.find(full_path);
				if (event_it != event_cache_.end())
				{
					if (data.remove_queue)
					{
						event_cache_.erase(event_it);
					}
					else
					{
						event_it->second->simple_value = data.simple_value;
						event_it->second->struct_value = data.struct_value;
						event_it->second->time = data.time;
						event_it->second->test = data.test;
						event_it->second->state_event = true;// data.state_event from if!!!;
						if (type_cache != event_it->second->types)
						{
							remove_types(event_it->second->types, event_it->second.get());
							event_it->second->types.swap(type_cache);
							add_types(event_it->second->types, event_it->second.get());
						}
						else
						{
							type_cache.clear();
						}
					}
				}
				else if (!data.remove_queue)
				{
					auto& event_data = event_cache_[full_path];
					event_data = std::make_unique<fired_event_data>();
					event_data->simple_value = data.simple_value;
					event_data->struct_value = data.struct_value;
					event_data->time = data.time;
					event_data->test = data.test;
					event_data->queue = data.queue;
					event_data->state_event = data.state_event;
					type_cache.swap(event_data->types);
					add_types(event_data->types, event_data.get());
				}
			}
			for (const auto& type : type_cache)
			{
				auto sub_it = subscribers_.find(type);
				if (sub_it != subscribers_.end())
				{
					for (auto& sub : sub_it->second)
					{
						if (sub.second.filter.matches(data.path, data.queue))
						{
							if (sub.second.bin_value)
							{
								simple_event_item item;
								item.event_id = data.queue;
								item.ts = data.time;
								item.value = data.simple_value;
								sub.second.callback->event_fired(sub.first, std::move(item));
							}
							else
							{
								data_event_item item;
								item.event_id = data.queue;
								item.ts = data.time;
								item.data = data.struct_value;
								sub.second.callback->event_fired(sub.first, std::move(item));
							}
						}
					}
				}
			}
		};


	auto job = rx_create_job<decltype(what), fired_event_data>()(rx_reference_ptr(), std::move(what), std::move(data));
	if (job)
		append(job);
	else
		RX_ASSERT(false);
	
}

void runtime_events_manager::remove_types (const types_cache& types, fired_event_data* whose)
{
	for (const auto& type : types)
	{
		auto it = types_cache_.find(type);
		if (it != types_cache_.end())
		{
			it->second.erase(whose);
			if(it->second.empty())
			{
				types_cache_.erase(it);
			}
		}
	}
}

void runtime_events_manager::add_types (const types_cache& types, fired_event_data* whose)
{
	for (const auto& type : types)
	{
		auto it = types_cache_.find(type);
		if(it != types_cache_.end())
		{
			it->second.insert(whose);
		}
		else
		{
			std::set<fired_event_data*> new_set;
			new_set.insert(whose);
			types_cache_.emplace(type, std::move(new_set));
		}
	}
}

runtime_events_manager& runtime_events_manager::instance ()
{
	static runtime_events_manager inst;
	return inst;
}

rx_result_with<runtime_handle_t> runtime_events_manager::register_subscriber (rx_events_callback::smart_ptr who, rx_node_id type, event_filter filter, bool bin)
{
	runtime_handle_t handle = rx_get_new_handle();
	std::function<void(rx_events_callback::smart_ptr, rx_node_id, event_filter, bool)> what = [this, handle](rx_events_callback::smart_ptr who, rx_node_id type, event_filter filter, bool bin) mutable
		{
			auto& subs = subscribers_[type];
			subscriber_data_t data;
			data.callback = std::move(who);
			data.filter = std::move(filter);
			data.bin_value = bin;
			auto it = types_cache_.find(type);
			if (it != types_cache_.end())
			{
				for (auto& event : it->second)
				{
					if (data.filter.matches(event->queue, event->queue))
					{
						if (data.bin_value)
						{
							simple_event_item item;
							item.event_id = event->queue;
							item.ts = event->time;
							item.value = event->simple_value;
							data.callback->event_fired(handle, std::move(item));
						}
						else
						{
							data_event_item item;
							item.event_id = event->queue;
							item.ts = event->time;
							item.data = event->struct_value;
							data.callback->event_fired(handle, std::move(item));
						}
					}
				}
			}
			subs.emplace(handle, std::move(data));

		};



	auto job = rx_create_job<decltype(what), rx_events_callback::smart_ptr, rx_node_id, event_filter, bool>()
		(rx_reference_ptr(), std::move(what), std::move(who), std::move(type), std::move(filter), std::move(bin));
	if (job)
	{
		append(job);
		return handle;
	}
	else
	{
		return RX_NOT_CONNECTED;
	}
}

rx_result runtime_events_manager::unregister_subscriber (rx_node_id type, runtime_handle_t handle)
{
	std::function<void(rx_node_id, runtime_handle_t)> what = [this](rx_node_id type, runtime_handle_t handle) mutable
		{
			auto& subs = subscribers_[type];
			auto it = subs.find(handle);
			if (it != subs.end())
			{
				subs.erase(it);
			}
		};

	auto job = rx_create_job<decltype(what), rx_node_id, runtime_handle_t>()
		(rx_reference_ptr(), std::move(what), std::move(type), std::move(handle));
	if (job)
	{
		append(job);
		return true;
	}
	else
	{
		return RX_NOT_CONNECTED;
	}
}

rx_result runtime_events_manager::start (hosting::rx_platform_host* host, const runtime_data_t& data, const io_manager_data_t& io_data)
{
	this->run(RX_PRIORITY_LOW);
	return true;
}

void runtime_events_manager::stop ()
{
	this->end();
}


// Class rx_platform::runtime::events::fired_event_data 


// Class rx_platform::runtime::events::rx_events_callback 


} // namespace events
} // namespace runtime
} // namespace rx_platform

