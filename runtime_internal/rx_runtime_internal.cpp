

/****************************************************************************
*
*  runtime_internal\rx_runtime_internal.cpp
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


// rx_runtime_internal
#include "runtime_internal/rx_runtime_internal.h"

#include "system/meta/rx_obj_types.h"
#include "system/runtime/rx_runtime_helpers.h"


namespace sys_runtime {

// Class sys_runtime::platform_runtime_manager 


platform_runtime_manager& platform_runtime_manager::instance ()
{
	static platform_runtime_manager g_instance;
	return g_instance;
}

rx_thread_handle_t platform_runtime_manager::resolve_app_processor (const application_instance_data& data)
{
	rx_thread_handle_t ret = 0;
	int this_cpu = data.processor;
	if (this_cpu < 0)
		ret = resolve_processor_auto();
	cpu_coverage_[ret]++;
	return ret;
}

rx_thread_handle_t platform_runtime_manager::resolve_domain_processor (const domain_instance_data& data)
{
	rx_thread_handle_t ret = 0;
	int this_cpu = data.processor;
	if (this_cpu < 0)
		ret = resolve_processor_auto();
	else
		ret = this_cpu;
	cpu_coverage_[ret]++;
	return ret;
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
	RUNTIME_LOG_INFO("platform_runtime_manager", 900, "Detected CPU "s + buff);
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
	return true;
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

void platform_runtime_manager::get_applications (api::query_result& result)
{
	result.items.reserve(applications_.size());
	for (const auto& one : applications_)
	{
		result.items.emplace_back(api::query_result_detail{ rx_application, one.second->meta_info() });
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


// Parameterized Class sys_runtime::execute_runtime_job 

template <class typeT>
execute_runtime_job<typeT>::execute_runtime_job (targetPtr target)
      : target_(target)
{
}



template <class typeT>
void execute_runtime_job<typeT>::process ()
{
	target_->process_runtime();
}

template class execute_runtime_job<meta::object_types::object_type>;
template class execute_runtime_job<meta::object_types::domain_type>;
template class execute_runtime_job<meta::object_types::application_type>;
template class execute_runtime_job<meta::object_types::port_type>;
} // namespace sys_runtime

