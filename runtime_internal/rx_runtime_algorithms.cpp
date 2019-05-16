

/****************************************************************************
*
*  runtime_internal\rx_runtime_algorithms.cpp
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


// rx_runtime_algorithms
#include "runtime_internal/rx_runtime_algorithms.h"

#include "system/meta/rx_obj_types.h"
#include "rx_runtime_internal.h"
#include "system/server/rx_server.h"


namespace sys_runtime {

namespace algorithms {
template<>
rx_result init_runtime<meta::object_types::object_type>(rx_object_ptr what, const runtime_init_context& ctx)
{
	return object_algorithms::init_runtime(what, ctx);
}
template<>
rx_result init_runtime<meta::object_types::application_type>(rx_application_ptr what, const runtime_init_context& ctx)
{
	return application_algorithms::init_runtime(what, ctx);
}
template<>
rx_result init_runtime<meta::object_types::port_type>(rx_port_ptr what, const runtime_init_context& ctx)
{
	return port_algorithms::init_runtime(what, ctx);
}
template<>
rx_result init_runtime<meta::object_types::domain_type>(rx_domain_ptr what, const runtime_init_context& ctx)
{
	return domain_algorithms::init_runtime(what, ctx);
}

// Class sys_runtime::algorithms::object_algorithms 


rx_result object_algorithms::init_runtime (rx_object_ptr what, const runtime_init_context& ctx)
{
	auto ret = what->initialize_object();
	if (ret)
	{
	}
	return ret;
}

rx_result object_algorithms::start_runtime (rx_object_ptr what, const runtime_start_context& ctx)
{
	return "Not implemented!";
}

rx_result object_algorithms::deinit_runtime (rx_object_ptr what, const runtime_deinit_context& ctx)
{
	return "Not implemented!";
}

rx_result object_algorithms::stop_runtime (rx_object_ptr what, const runtime_stop_context& ctx)
{
	return "Not implemented!";
}


// Class sys_runtime::algorithms::application_algorithms 


rx_result application_algorithms::init_runtime (rx_application_ptr what, const runtime_init_context& ctx)
{
	auto ret = what->initialize_object();
	if (ret)
	{
		auto it = platform_runtime_manager::instance().applications_.find(what->meta_info().get_id());
		if (it == platform_runtime_manager::instance().applications_.end())
		{
			platform_runtime_manager::instance().applications_.emplace(what->meta_info().get_id(), what);
		}
		rx_post_function<rx_application_ptr>([](rx_application_ptr whose)
			{
				runtime_start_context start_ctx;
				auto result = start_runtime(whose, start_ctx);
				if (result)
				{
					RUNTIME_LOG_TRACE("application_algorithms", 100, ("Started "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_name()).c_str());
				}
				else
				{
					for (const auto& error : result.errors())
						RUNTIME_LOG_ERROR("application_algorithms", 800, error.c_str());
					RUNTIME_LOG_ERROR("application_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_name()).c_str());
				}
			}, what, what->get_executer());
	}
	return ret;
}

rx_result application_algorithms::start_runtime (rx_application_ptr what, const runtime_start_context& ctx)
{
	return "Not implemented!";
}

rx_result application_algorithms::deinit_runtime (rx_application_ptr what, const runtime_deinit_context& ctx)
{
	return "Not implemented!";
}

rx_result application_algorithms::stop_runtime (rx_application_ptr what, const runtime_stop_context& ctx)
{
	return "Not implemented!";
}


// Class sys_runtime::algorithms::domain_algorithms 


rx_result domain_algorithms::init_runtime (rx_domain_ptr what, const runtime_init_context& ctx)
{
	return "Not implemented!";
}

rx_result domain_algorithms::start_runtime (rx_domain_ptr what, const runtime_start_context& ctx)
{
	return "Not implemented!";
}

rx_result domain_algorithms::deinit_runtime (rx_domain_ptr what, const runtime_deinit_context& ctx)
{
	return "Not implemented!";
}

rx_result domain_algorithms::stop_runtime (rx_domain_ptr what, const runtime_stop_context& ctx)
{
	return "Not implemented!";
}


// Class sys_runtime::algorithms::port_algorithms 


rx_result port_algorithms::init_runtime (rx_port_ptr what, const runtime_init_context& ctx)
{
	return "Not implemented!";
}

rx_result port_algorithms::start_runtime (rx_port_ptr what, const runtime_start_context& ctx)
{
	return "Not implemented!";
}

rx_result port_algorithms::deinit_runtime (rx_port_ptr what, const runtime_deinit_context& ctx)
{
	return "Not implemented!";
}

rx_result port_algorithms::stop_runtime (rx_port_ptr what, const runtime_stop_context& ctx)
{
	return "Not implemented!";
}


} // namespace algorithms
} // namespace sys_runtime

