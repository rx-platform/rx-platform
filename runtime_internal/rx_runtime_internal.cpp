

/****************************************************************************
*
*  runtime_internal\rx_runtime_internal.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


// rx_runtime_internal
#include "runtime_internal/rx_runtime_internal.h"

#include "system/meta/rx_obj_types.h"
#include "system/runtime/rx_runtime_helpers.h"
#include "runtime_internal/rx_data_source.h"
#include "system/server/rx_platform_item.h"
#include "rx_simulation.h"


namespace rx_internal {

namespace sys_runtime {

// Class rx_internal::sys_runtime::platform_runtime_manager 


platform_runtime_manager& platform_runtime_manager::instance ()
{
	static platform_runtime_manager g_instance;
	return g_instance;
}

rx_thread_handle_t platform_runtime_manager::resolve_app_processor (const application_instance_data& data)
{
	rx_thread_handle_t this_cpu = data.processor;
	if (this_cpu < 0)
		this_cpu = resolve_processor_auto();
	this_cpu = this_cpu % cpu_coverage_.size();
	cpu_coverage_[this_cpu]++;
	return this_cpu;
}

rx_thread_handle_t platform_runtime_manager::resolve_domain_processor (const domain_instance_data& data)
{
	rx_thread_handle_t this_cpu = data.processor;
	if (this_cpu < 0)
	{
		// handle get application and get his
		this_cpu = resolve_processor_auto();
	}
	else
	{
		this_cpu = this_cpu % cpu_coverage_.size();
	}
	cpu_coverage_[this_cpu]++;
	return this_cpu;
}

void platform_runtime_manager::remove_one (rx_thread_handle_t from_where)
{
	// TOTO CPU!!!!
	//cpu_coverage_[from_where- first_cpu_]--;
}

rx_result platform_runtime_manager::initialize (hosting::rx_platform_host* host, runtime_data_t& data)
{
	size_t cpu_count = 1;
	char buff[0x40];
	rx_collect_processor_info(buff, sizeof(buff), &cpu_count);
	std::ostringstream ss;
	ss << "Detected "
		<< cpu_count
		<< (rx_big_endian ? " Big-endian" : " Little-endian")
		<< " CPU Cores "
		<< " [PID:"
		<< rx_pid
		<< "]";
	RUNTIME_LOG_INFO("platform_runtime_manager", 900, ss.str());

	if (cpu_count > (size_t)data.io_pool_size)
	{
		first_cpu_ = data.io_pool_size;
	}
	else
	{
		first_cpu_ = 0;
	}
	last_cpu_ = first_cpu_ + data.workers_pool_size;
	cpu_coverage_.assign(last_cpu_ - first_cpu_, 0);
	rx_result result = data_source::data_source_factory::instance().register_internal_sources();
	if (result)
	{
		result = simulation::register_simulation_constructors();
	}
	return result;
}

rx_thread_handle_t platform_runtime_manager::resolve_processor_auto ()
{
	int min = std::numeric_limits<int>::max();
	rx_thread_handle_t ret = 0;
	for (size_t i = 0; i<cpu_coverage_.size(); i++)
	{
		if (cpu_coverage_[i] < min)
		{
			min = cpu_coverage_[i];
			ret = i;
		}
	}
	return ret;
}

void platform_runtime_manager::get_applications (api::query_result& result, const string_type& path)
{
	//we are heavy and we're doing it fast
	result.items.reserve(applications_.size());

	string_type buffer;

	for (const auto& one : applications_)
	{
		if (!path.empty())
		{
			one.second->meta_info().get_full_path_with_buffer(buffer);
			if (path.size() <= buffer.size())
			{
				size_t size = std::min(path.size(), buffer.size());
				// sorry guys i have to do it, i know i'm waisting a byte
				if (0 == memcmp(buffer.c_str(), path.c_str(), size))
				{
					result.items.emplace_back(api::query_result_detail{ rx_application, one.second->meta_info() });
				}
			}
		}
		else // path.empty()
		{
			result.items.emplace_back(api::query_result_detail{ rx_application, one.second->meta_info() });
		}
	}
}

runtime_handle_t platform_runtime_manager::get_new_handle ()
{
	static std::atomic<runtime_handle_t> g_handle(1);
	runtime_handle_t ret = g_handle++;
	if (!ret)// avoid zero
		return get_new_handle();
	else
		return ret;
}


// Class rx_internal::sys_runtime::runtime_cache 


void runtime_cache::add_to_cache (platform_item_ptr&& item)
{
	string_type path = item->meta_info().get_full_path();
	locks::auto_slim_lock _(&lock_);
	auto it = path_cache_.find(path);
	if(it!=path_cache_.end())
	{
		it->second = std::move(item);
	}
	else
	{
		path_cache_.emplace(std::move(path), std::move(item));
	}
}

std::vector<platform_item_ptr> runtime_cache::get_items (const string_array& paths)
{
	std::vector<platform_item_ptr> ret;
	ret.reserve(paths.size());
	locks::auto_slim_lock _(&lock_);
	for (const auto& path : paths)
	{
		auto it = path_cache_.find(path);
		if (it != path_cache_.end())
			ret.emplace_back(it->second->clone());
		else
			ret.emplace_back(platform_item_ptr());
	}
	return ret;
}

platform_item_ptr runtime_cache::get_item (const string_type& path)
{
	locks::auto_slim_lock _(&lock_); auto it = path_cache_.find(path);
	if (it != path_cache_.end())
		return it->second->clone();
	else
		return platform_item_ptr();
}

void runtime_cache::remove_from_cache (const string_type& path)
{
	locks::auto_slim_lock _(&lock_);
	auto it = path_cache_.find(path);
	if (it != path_cache_.end())
		path_cache_.erase(it);
}


} // namespace sys_runtime
} // namespace rx_internal

