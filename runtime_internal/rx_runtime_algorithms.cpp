

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
#include "system/server/rx_async_functions.h"
#include "model/rx_meta_internals.h"


namespace sys_runtime {

namespace algorithms {
template<>
rx_result init_runtime<meta::object_types::object_type>(rx_object_ptr what, runtime::runtime_init_context& ctx)
{
	return object_algorithms::init_runtime(what, ctx);
}
template<>
rx_result init_runtime<meta::object_types::application_type>(rx_application_ptr what, runtime::runtime_init_context& ctx)
{
	return application_algorithms::init_runtime(what, ctx);
}
template<>
rx_result init_runtime<meta::object_types::port_type>(rx_port_ptr what, runtime::runtime_init_context& ctx)
{
	return port_algorithms::init_runtime(what, ctx);
}
template<>
rx_result init_runtime<meta::object_types::domain_type>(rx_domain_ptr what, runtime::runtime_init_context& ctx)
{
	return domain_algorithms::init_runtime(what, ctx);
}
template<>
rx_result deinit_runtime<meta::object_types::object_type>(rx_object_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	return object_algorithms::deinit_runtime(what, callback, ctx);
}
template<>
rx_result deinit_runtime<meta::object_types::application_type>(rx_application_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	return application_algorithms::deinit_runtime(what, callback, ctx);
}
template<>
rx_result deinit_runtime<meta::object_types::port_type>(rx_port_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	return port_algorithms::deinit_runtime(what, callback, ctx);
}
template<>
rx_result deinit_runtime<meta::object_types::domain_type>(rx_domain_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	return domain_algorithms::deinit_runtime(what, callback, ctx);
}

// Class sys_runtime::algorithms::object_algorithms 


rx_result object_algorithms::init_runtime (rx_object_ptr what, runtime::runtime_init_context& ctx)
{
	auto result = what->initialize_runtime(ctx);
	if (result)
	{
		if (what->get_instance_data().domain_id)
		{
			auto domain_ptr = model::platform_types_manager::instance().internal_get_type_cache<domain_type>().get_runtime(what->get_instance_data().domain_id);
			if (!domain_ptr)
			{
				RUNTIME_LOG_WARNING("object_algorithms", 900, "Domain Id is invalid, connecting object to unassigned domain.");
				domain_ptr = rx_gate::instance().get_manager().get_unassigned_domain();
			}
			if (domain_ptr)
			{
				what->connect_domain(std::move(domain_ptr));
			}
			else
			{
				std::ostringstream message;
				message << "Unable to connect to domain "
					<< what->get_instance_data().domain_id ? what->get_instance_data().domain_id.to_string() : RX_NULL_ITEM_NAME;
				result = rx_result(message.str());
			}
		}
		else
		{
			RUNTIME_LOG_WARNING("object_algorithms", 900, "Domain Id is null, connecting object to unassigned domain.");
			auto domain_ptr = rx_gate::instance().get_manager().get_unassigned_domain();
			if (domain_ptr)
			{
				what->connect_domain(std::move(domain_ptr));
			}
			else
			{
				std::ostringstream message;
				message << "Unable to connect to domain "
					<< what->get_instance_data().domain_id ? what->get_instance_data().domain_id.to_string() : RX_NULL_ITEM_NAME;
				result = rx_result(message.str());
			}
		}
		if (result)
		{
			rx_post_function<rx_object_ptr>([](rx_object_ptr whose)
				{
					runtime::runtime_start_context start_ctx;
					auto result = start_runtime(whose, start_ctx);
					if (result)
					{
						RUNTIME_LOG_TRACE("object_algorithms", 100, ("Started "s + rx_item_type_name(rx_object) + " "s + whose->meta_info().get_name()).c_str());
					}
					else
					{
						for (const auto& error : result.errors())
							RUNTIME_LOG_ERROR("object_algorithms", 800, error.c_str());
						RUNTIME_LOG_ERROR("object_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_object) + " "s + whose->meta_info().get_name()).c_str());
					}
				}, what, what->get_executer());
		}
	}
	else
	{
		for (const auto& error : result.errors())
			RUNTIME_LOG_ERROR("object_algorithms", 800, error.c_str());
		RUNTIME_LOG_ERROR("object_algorithms", 800, ("Error initializing "s + rx_item_type_name(rx_object) + " "s + what->meta_info().get_name()).c_str());

	}
	return result;
}

rx_result object_algorithms::start_runtime (rx_object_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	if (ret)
	{
	}
	return ret;
}

rx_result object_algorithms::deinit_runtime (rx_object_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	rx_post_function<rx_object_ptr>([callback](rx_object_ptr whose)
		{
			runtime::runtime_stop_context stop_ctx;
			auto result = stop_runtime(whose, stop_ctx);
			if (result)
			{
				RUNTIME_LOG_TRACE("object_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_name()).c_str());
				rx_post_function<rx_object_ptr>([callback](rx_object_ptr whose)
					{
						runtime::runtime_deinit_context deinit_ctx;
						auto result = whose->deinitialize_runtime(deinit_ctx);
						callback(std::move(result));
					}, whose, RX_DOMAIN_META);
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("object_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("object_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_name()).c_str());
			}
		}, what, what->get_executer());

	return true;
}

rx_result object_algorithms::stop_runtime (rx_object_ptr what, runtime::runtime_stop_context& ctx)
{
	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
	}
	return ret;
}


// Class sys_runtime::algorithms::domain_algorithms 


rx_result domain_algorithms::init_runtime (rx_domain_ptr what, runtime::runtime_init_context& ctx)
{
	auto result = what->initialize_runtime(ctx);
	if (result)
	{
		if (what->get_instance_data().app_id)
		{
			auto application_ptr = model::platform_types_manager::instance().internal_get_type_cache<application_type>().get_runtime(what->get_instance_data().app_id);
			if (!application_ptr)
			{
				RUNTIME_LOG_WARNING("domain_algorithms", 900, "Application Id is invalid, connecting domain to unassigned application.");
				application_ptr = rx_gate::instance().get_manager().get_unassigned_app();
			}
			if (application_ptr)
			{
				what->connect_application(std::move(application_ptr));
			}
			else
			{
				std::ostringstream message;
				message << "Unable to connect to application "
					<< what->get_instance_data().app_id ? what->get_instance_data().app_id.to_string() : RX_NULL_ITEM_NAME;
				result = rx_result(message.str());
			}
		}
		else
		{
			RUNTIME_LOG_WARNING("domain_algorithms", 900, "Application Id is null, connecting domain to unassigned application.");
			auto application_ptr = rx_gate::instance().get_manager().get_unassigned_app();
			if (application_ptr)
			{
				what->connect_application(std::move(application_ptr));
			}
			else
			{
				std::ostringstream message;
				message << "Unable to connect to application "
					<< what->get_instance_data().app_id ? what->get_instance_data().app_id.to_string() : RX_NULL_ITEM_NAME;
				result = rx_result(message.str());
			}
		}
		if (result)
		{
			rx_post_function<rx_domain_ptr>([](rx_domain_ptr whose)
				{
					runtime::runtime_start_context start_ctx;
					auto result = start_runtime(whose, start_ctx);
					if (result)
					{
						RUNTIME_LOG_TRACE("domain_algorithms", 100, ("Started "s + rx_item_type_name(rx_domain) + " "s + whose->meta_info().get_name()).c_str());
					}
					else
					{
						for (const auto& error : result.errors())
							RUNTIME_LOG_ERROR("domain_algorithms", 800, error.c_str());
						RUNTIME_LOG_ERROR("domain_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_domain) + " "s + whose->meta_info().get_name()).c_str());
					}
				}, what, what->get_executer());
		}
	}
	return result;
}

rx_result domain_algorithms::start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	if (ret)
	{
	}
	return ret;
}

rx_result domain_algorithms::deinit_runtime (rx_domain_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	rx_post_function<rx_domain_ptr>([callback](rx_domain_ptr whose)
		{
			runtime::runtime_stop_context stop_ctx;
			auto result = stop_runtime(whose, stop_ctx);
			if (result)
			{
				RUNTIME_LOG_TRACE("domain_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_name()).c_str());
				rx_post_function<rx_domain_ptr>([callback](rx_domain_ptr whose)
					{
						runtime::runtime_deinit_context deinit_ctx;
						auto result = whose->deinitialize_runtime(deinit_ctx);
						callback(std::move(result));
					}, whose, RX_DOMAIN_META);
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("domain_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("domain_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_name()).c_str());
			}
		}, what, what->get_executer());

	return true;
}

rx_result domain_algorithms::stop_runtime (rx_domain_ptr what, runtime::runtime_stop_context& ctx)
{
	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
	}
	return ret;
}


// Class sys_runtime::algorithms::port_algorithms 


rx_result port_algorithms::init_runtime (rx_port_ptr what, runtime::runtime_init_context& ctx)
{
	auto result = what->initialize_runtime(ctx);
	if (result)
	{
		if (what->get_instance_data().app_id)
		{
			auto application_ptr = model::platform_types_manager::instance().internal_get_type_cache<application_type>().get_runtime(what->get_instance_data().app_id);
			if (!application_ptr)
			{
				RUNTIME_LOG_WARNING("port_algorithms", 900, "Application Id is invalid, connecting domain to unassigned application.");
				application_ptr = rx_gate::instance().get_manager().get_unassigned_app();
			}
			if (application_ptr)
			{
				what->connect_application(std::move(application_ptr));
			}
			else
			{
				std::ostringstream message;
				message << "Unable to connect to application "
					<< what->get_instance_data().app_id ? what->get_instance_data().app_id.to_string() : RX_NULL_ITEM_NAME;
				result = rx_result(message.str());
			}
		}
		else
		{
			RUNTIME_LOG_WARNING("port_algorithms", 900, "Application Id is null, connecting domain to unassigned application.");
			auto application_ptr = rx_gate::instance().get_manager().get_unassigned_app();
			if (application_ptr)
			{
				what->connect_application(std::move(application_ptr));
			}
			else
			{
				std::ostringstream message;
				message << "Unable to connect to application "
					<< what->get_instance_data().app_id ? what->get_instance_data().app_id.to_string() : RX_NULL_ITEM_NAME;
				result = rx_result(message.str());
			}
		}
		if (result)
		{
			rx_post_function<rx_port_ptr>([](rx_port_ptr whose)
				{
					runtime::runtime_start_context start_ctx;
					auto result = start_runtime(whose, start_ctx);
					if (result)
					{
						RUNTIME_LOG_TRACE("port_algorithms", 100, ("Started "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_name()).c_str());
					}
					else
					{
						for (const auto& error : result.errors())
							RUNTIME_LOG_ERROR("port_algorithms", 800, error.c_str());
						RUNTIME_LOG_ERROR("port_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_name()).c_str());
					}
				}, what, what->get_executer());
		}
	}
	return result;
}

rx_result port_algorithms::start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	if (ret)
	{
	}
	return ret;
}

rx_result port_algorithms::deinit_runtime (rx_port_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	rx_post_function<rx_port_ptr>([callback](rx_port_ptr whose)
		{
			runtime::runtime_stop_context stop_ctx;
			auto result = stop_runtime(whose, stop_ctx);
			if (result)
			{
				RUNTIME_LOG_TRACE("port_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_name()).c_str());
				rx_post_function<rx_port_ptr>([callback](rx_port_ptr whose)
					{
						runtime::runtime_deinit_context deinit_ctx;
						auto result = whose->deinitialize_runtime(deinit_ctx);
						callback(std::move(result));
					}, whose, RX_DOMAIN_META);
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("port_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("port_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_name()).c_str());
			}
		}, what, what->get_executer());

	return true;
}

rx_result port_algorithms::stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx)
{
	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
	}
	return ret;
}


// Class sys_runtime::algorithms::application_algorithms 


rx_result application_algorithms::init_runtime (rx_application_ptr what, runtime::runtime_init_context& ctx)
{
	auto ret = what->initialize_runtime(ctx);
	if (ret)
	{
		auto it = platform_runtime_manager::instance().applications_.find(what->meta_info().get_id());
		if (it == platform_runtime_manager::instance().applications_.end())
		{
			platform_runtime_manager::instance().applications_.emplace(what->meta_info().get_id(), what);
		}
		rx_post_function<rx_application_ptr>([](rx_application_ptr whose)
			{
				runtime::runtime_start_context start_ctx;
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

rx_result application_algorithms::start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	return ret;
}

rx_result application_algorithms::deinit_runtime (rx_application_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	rx_post_function<rx_application_ptr>([callback](rx_application_ptr whose)
		{
			runtime::runtime_stop_context stop_ctx;
			auto result = stop_runtime(whose, stop_ctx);
			if (result)
			{
				RUNTIME_LOG_TRACE("application_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_name()).c_str());
				rx_post_function<rx_application_ptr>([callback](rx_application_ptr whose)
					{
						runtime::runtime_deinit_context deinit_ctx;
						auto result = whose->deinitialize_runtime(deinit_ctx);
						callback(std::move(result));
					}, whose, RX_DOMAIN_META);
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("application_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("application_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_name()).c_str());
			}
		}, what, what->get_executer());

	return true;
}

rx_result application_algorithms::stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx)
{
	auto ret = what->stop_runtime(ctx);
	return ret;
}


} // namespace algorithms
} // namespace sys_runtime

