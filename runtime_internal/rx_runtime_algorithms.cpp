

/****************************************************************************
*
*  runtime_internal\rx_runtime_algorithms.cpp
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


// rx_runtime_algorithms
#include "runtime_internal/rx_runtime_algorithms.h"

#include "system/meta/rx_obj_types.h"
#include "rx_runtime_internal.h"
#include "sys_internal/rx_async_functions.h"
#include "model/rx_meta_internals.h"
#include "system/server/rx_platform_item.h"


namespace rx_internal {

namespace sys_runtime {

namespace algorithms {

//////////////////////////////////////////////////////////////////////////////////////
// create_runtime_structure
template<>
rx_result create_runtime_structure<meta::object_types::object_type>(rx_object_ptr what)
{
	return object_algorithms::connect_domain(what);
}
template<>
rx_result create_runtime_structure<meta::object_types::application_type>(rx_application_ptr what)
{
	return true;
}
template<>
rx_result create_runtime_structure<meta::object_types::port_type>(rx_port_ptr what)
{
	return port_algorithms::connect_application(what);
}
template<>
rx_result create_runtime_structure<meta::object_types::domain_type>(rx_domain_ptr what)
{
	return domain_algorithms::connect_application(what);
}
//////////////////////////////////////////////////////////////////////////////////////
// delete_runtime_structure
template<>
rx_result delete_runtime_structure<meta::object_types::object_type>(rx_object_ptr what)
{
	return object_algorithms::disconnect_domain(what);
}
template<>
rx_result delete_runtime_structure<meta::object_types::application_type>(rx_application_ptr what)
{
	return true;
}
template<>
rx_result delete_runtime_structure<meta::object_types::port_type>(rx_port_ptr what)
{
	return port_algorithms::disconnect_application(what);
}
template<>
rx_result delete_runtime_structure<meta::object_types::domain_type>(rx_domain_ptr what)
{
	return domain_algorithms::disconnect_application(what);
}
//////////////////////////////////////////////////////////////////////////////////////
// init_runtime
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
rx_result init_runtime<meta::object_types::relation_type>(rx_relation_ptr what, runtime::runtime_init_context& ctx)
{
	return relations_algorithms::init_runtime(what, ctx);
}
//////////////////////////////////////////////////////////////////////////////////////
// deinit_runtime
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
template<>
rx_result deinit_runtime<meta::object_types::relation_type>(rx_relation_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	return relations_algorithms::deinit_runtime(what, callback, ctx);
}

// Class rx_internal::sys_runtime::algorithms::object_algorithms 


rx_result object_algorithms::init_runtime (rx_object_ptr what, runtime::runtime_init_context& ctx)
{
	auto result = what->get_instance_data().before_init_runtime(what, ctx);
	if (result)
	{
		result = what->initialize_runtime(ctx);
		if (result)
		{
			RUNTIME_LOG_TRACE("object_algorithms", 100, "Initialized "s + rx_item_type_name(rx_object) + " "s + what->meta_info().get_full_path());

			rx_post_function<rx_object_ptr>([](rx_object_ptr whose)
				{
					auto ctx = whose->create_start_context();
					auto result = start_runtime(whose, ctx);
					if (result)
					{
						RUNTIME_LOG_TRACE("object_algorithms", 100, ("Started "s + rx_item_type_name(rx_object) + " "s + whose->meta_info().get_full_path()).c_str());
					}
					else
					{
						for (const auto& error : result.errors())
							RUNTIME_LOG_ERROR("object_algorithms", 800, error.c_str());
						RUNTIME_LOG_ERROR("object_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_object) + " "s + whose->meta_info().get_full_path()).c_str());
					}
				}, what, what->get_executer());
		}
		else
		{
			result = "Unable to initialize domain runtime";
		}
	}
	else
	{
		result = "Unable to initialize instance_data";
	}
	if (!result)
	{
		for (const auto& error : result.errors())
			RUNTIME_LOG_ERROR("object_algorithms", 800, error.c_str());
		RUNTIME_LOG_ERROR("object_algorithms", 800, ("Error initializing "s + rx_item_type_name(rx_object) + " "s + what->meta_info().get_name()).c_str() + result.errors_line());
	}

	return result;
}

rx_result object_algorithms::start_runtime (rx_object_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	if (ret)
	{
		platform_runtime_manager::instance().get_cache().add_to_cache(what->get_item_ptr());
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
				RUNTIME_LOG_TRACE("object_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_object) + " "s + whose->meta_info().get_full_path()).c_str());
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
				RUNTIME_LOG_ERROR("object_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_object) + " "s + whose->meta_info().get_full_path()).c_str());
			}
		}, what, what->get_executer());

	return true;
}

rx_result object_algorithms::stop_runtime (rx_object_ptr what, runtime::runtime_stop_context& ctx)
{

	platform_runtime_manager::instance().get_cache().remove_from_cache(what->meta_info().get_full_path());

	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
		what->get_instance_data().after_stop_runtime(what, ctx);
	}
	return ret;
}

rx_result object_algorithms::connect_domain (rx_object_ptr what)
{
	if (what->get_instance_data().domain_id)
	{
		auto domain_ptr = model::platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(what->get_instance_data().domain_id);
		if (!domain_ptr)
		{
			RUNTIME_LOG_WARNING("object_algorithms", 900, "Domain Id is invalid, connecting object "s
				+ what->meta_info().get_full_path() + " to unassigned domain.");
			domain_ptr = model::platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(RX_NS_SYSTEM_UNASS_ID);
		}
		if (domain_ptr)
		{
			auto temp_ptr = domain_ptr.value();
			if (what->get_instance_data().connect_domain(std::move(temp_ptr), what))
				RUNTIME_LOG_TRACE("object_algorithms", 100, what->meta_info().get_full_path()
					+ " connected to domain " + domain_ptr.value()->meta_info().get_full_path());
		}
		else
		{
			std::ostringstream message;
			message << "Unable to connect to domain "
				<< what->get_instance_data().domain_id ? what->get_instance_data().domain_id.to_string() : RX_NULL_ITEM_NAME;
			message << domain_ptr.errors_line();
			return rx_result(message.str());
		}
	}
	else
	{
		RUNTIME_LOG_WARNING("object_algorithms", 900, "Domain Id is " RX_NULL_ITEM_NAME ", connecting object "s
			+ what->meta_info().get_full_path() + " to unassigned domain.");
		auto domain_ptr = model::platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(RX_NS_SYSTEM_UNASS_ID);
		if (domain_ptr)
		{
			auto temp_ptr = domain_ptr.value();
			if (what->get_instance_data().connect_domain(std::move(temp_ptr), what))
				RUNTIME_LOG_TRACE("object_algorithms", 100, what->meta_info().get_full_path()
					+ " connected to domain " + domain_ptr.value()->meta_info().get_full_path());
		}
		else
		{
			std::ostringstream message;
			message << "Unable to connect to domain "
				<< what->get_instance_data().domain_id ? what->get_instance_data().domain_id.to_string() : RX_NULL_ITEM_NAME;
			return rx_result(message.str());
		}
	}
	return true;
}

rx_result object_algorithms::disconnect_domain (rx_object_ptr what)
{
	if (what->get_instance_data().disconnect_domain(what))
		RUNTIME_LOG_TRACE("object_algorithms", 100, what->meta_info().get_full_path()
			+ " disconnected from domain");
	return true;
}


// Class rx_internal::sys_runtime::algorithms::domain_algorithms 


rx_result domain_algorithms::init_runtime (rx_domain_ptr what, runtime::runtime_init_context& ctx)
{
	auto result = what->get_instance_data().before_init_runtime(what, ctx);
	if (result)
	{
		result = what->initialize_runtime(ctx);
		if (result)
		{
			RUNTIME_LOG_TRACE("domain_algorithms", 100, "Initialized "s + rx_item_type_name(rx_domain) + " "s + what->meta_info().get_full_path());

			rx_post_function<rx_domain_ptr>([](rx_domain_ptr whose)
				{
					runtime::runtime_start_context start_ctx = whose->create_start_context();
					auto result = start_runtime(whose, start_ctx);
					if (result)
					{
						RUNTIME_LOG_TRACE("domain_algorithms", 100, ("Started "s + rx_item_type_name(rx_domain) + " "s + whose->meta_info().get_full_path()).c_str());
					}
					else
					{
						RUNTIME_LOG_ERROR("domain_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_domain) + " "s + whose->meta_info().get_full_path()).c_str() + result.errors_line());

					}
				}, what, what->get_executer());
		}
		else
		{
			result = "Unable to initialize domain runtime";
		}
	}
	else
	{
		result = "Unable to initialize instance_data";
	}
	if (!result)
	{
		for (const auto& error : result.errors())
			RUNTIME_LOG_ERROR("domain_algorithms", 800, error.c_str());
		RUNTIME_LOG_ERROR("domain_algorithms", 800, ("Error initializing "s + rx_item_type_name(rx_object) + " "s + what->meta_info().get_name()).c_str() + result.errors_line());
	}
	return result;
}

rx_result domain_algorithms::start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	if (ret)
	{
		platform_runtime_manager::instance().get_cache().add_to_cache(what->get_item_ptr());
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
				RUNTIME_LOG_TRACE("domain_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());
				rx_post_function<rx_domain_ptr>([callback](rx_domain_ptr whose)
					{
						runtime::runtime_deinit_context deinit_ctx;
						auto result = whose->deinitialize_runtime(deinit_ctx);
						if (result)
						{
							whose->get_instance_data().after_deinit_runtime(whose, deinit_ctx);
						}
						/*auto erase_result = platform_runtime_manager::instance().applications_.erase(whose->meta_info().get_id());
						if (erase_result == 0)
						{
							RUNTIME_LOG_CRITICAL("application_algorithms", 500, "this application is not registered in cache!!!");
						}*/
						callback(std::move(result));
					}, whose, RX_DOMAIN_META);
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("domain_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("domain_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());
			}
		}, what, what->get_executer());

	return true;
}

rx_result domain_algorithms::stop_runtime (rx_domain_ptr what, runtime::runtime_stop_context& ctx)
{
	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
		what->get_instance_data().after_stop_runtime(what, ctx);
	}
	return ret;
}

rx_result domain_algorithms::connect_application (rx_domain_ptr what)
{
	if (what->get_instance_data().app_id)
	{
		auto application_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(what->get_instance_data().app_id);
		if (!application_ptr)
		{
			RUNTIME_LOG_WARNING("domain_algorithms", 900, "Application Id is invalid, connecting domain "s
				+ what->meta_info().get_full_path() + " to unassigned application.");
			application_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(RX_NS_SYSTEM_DOM_ID);
		}
		if (application_ptr)
		{
			auto temp_ptr = application_ptr.value();
			if (what->get_instance_data().connect_application(std::move(temp_ptr), what))
				RUNTIME_LOG_TRACE("domain_algorithms", 100, what->meta_info().get_full_path()
					+ " connected to application " + application_ptr.value()->meta_info().get_full_path());
		}
		else
		{
			std::ostringstream message;
			message << "Unable to connect to application "
				<< what->get_instance_data().app_id ? what->get_instance_data().app_id.to_string() : RX_NULL_ITEM_NAME;
			message << application_ptr.errors_line();
			return rx_result(message.str());
		}
	}
	else
	{
		RUNTIME_LOG_WARNING("domain_algorithms", 900, "Application Id is " RX_NULL_ITEM_NAME ", connecting object "s
			+ what->meta_info().get_full_path() + " to unassigned application.");
		auto application_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(RX_NS_SYSTEM_APP_ID);
		if (application_ptr)
		{
			auto temp_ptr = application_ptr.value();
			if (what->get_instance_data().connect_application(std::move(temp_ptr), what))
				RUNTIME_LOG_TRACE("domain_algorithms", 100, what->meta_info().get_full_path()
					+ " connected to application " + application_ptr.value()->meta_info().get_full_path());
		}
		else
		{
			std::ostringstream message;
			message << "Unable to connect to application "
				<< what->get_instance_data().app_id ? what->get_instance_data().app_id.to_string() : RX_NULL_ITEM_NAME;
			return rx_result(message.str());
		}
	}
	return true;
}

rx_result domain_algorithms::disconnect_application (rx_domain_ptr what)
{
	if (what->get_instance_data().disconnect_application(what))
		RUNTIME_LOG_TRACE("domain_algorithms", 100, what->meta_info().get_full_path()
			+ " disconnected from application");
	return true;
}


// Class rx_internal::sys_runtime::algorithms::port_algorithms 


rx_result port_algorithms::init_runtime (rx_port_ptr what, runtime::runtime_init_context& ctx)
{
	auto result = what->get_instance_data().before_init_runtime(what, ctx);
	if (result)
	{
		result = what->initialize_runtime(ctx);
		if (result)
		{
			RUNTIME_LOG_TRACE("port_algorithms", 100, "Initialized "s + rx_item_type_name(rx_port) + " "s + what->meta_info().get_full_path());


			runtime::algorithms::object_runtime_algorithms<port_type>::fire_job(*what);

			rx_post_function<rx_port_ptr>([](rx_port_ptr whose)
				{
					runtime::runtime_start_context start_ctx = whose->create_start_context();
					auto result = start_runtime(whose, start_ctx);
					if (result)
					{
						RUNTIME_LOG_TRACE("port_algorithms", 100, ("Started "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());
					}
					else
					{
						for (const auto& error : result.errors())
							RUNTIME_LOG_ERROR("port_algorithms", 800, error.c_str());
						RUNTIME_LOG_ERROR("port_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());
					}
				}, what, what->get_executer());
		}
		else
		{
			result = "Unable to initialize domain runtime";
		}
	}
	else
	{
		result = "Unable to initialize instance_data";
	}

	return result;
}

rx_result port_algorithms::start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	if (ret)
	{
		platform_runtime_manager::instance().get_cache().add_to_cache(what->get_item_ptr());
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
				RUNTIME_LOG_TRACE("port_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());
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
				RUNTIME_LOG_ERROR("port_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());
			}
		}, what, what->get_executer());

	return true;
}

rx_result port_algorithms::stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx)
{
	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
		what->get_instance_data().after_stop_runtime(what, ctx);
	}
	return ret;
}

rx_result port_algorithms::connect_application (rx_port_ptr what)
{
	if (what->get_instance_data().app_id)
	{
		auto application_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(what->get_instance_data().app_id);
		if (!application_ptr)
		{
			RUNTIME_LOG_WARNING("port_algorithms", 900, "Application Id is invalid, connecting port "s
				+ what->meta_info().get_full_path() + " to unassigned domain.");
			application_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(RX_NS_SYSTEM_APP_ID);
		}
		if (application_ptr)
		{
			auto temp_ptr = application_ptr.value();
			if (what->get_instance_data().connect_application(std::move(temp_ptr), what))
				RUNTIME_LOG_TRACE("port_algorithms", 100, what->meta_info().get_full_path()
					+ " connected to application " + application_ptr.value()->meta_info().get_full_path());
		}
		else
		{
			std::ostringstream message;
			message << "Unable to connect to application "
				<< what->get_instance_data().app_id ? what->get_instance_data().app_id.to_string() : RX_NULL_ITEM_NAME;
			message << application_ptr.errors_line();
			return rx_result(message.str());
		}
	}
	else
	{
		RUNTIME_LOG_WARNING("port_algorithms", 900, "Application Id is " RX_NULL_ITEM_NAME ", connecting object "s
			+ what->meta_info().get_full_path() + " to unassigned application.");
		auto application_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(RX_NS_SYSTEM_APP_ID);
		if (application_ptr)
		{
			auto temp_ptr = application_ptr.value();
			if (what->get_instance_data().connect_application(std::move(temp_ptr), what))
				RUNTIME_LOG_TRACE("port_algorithms", 100, what->meta_info().get_full_path()
					+ " connected to application " + application_ptr.value()->meta_info().get_full_path());
		}
		else
		{
			std::ostringstream message;
			message << "Unable to connect to application "
				<< what->get_instance_data().app_id ? what->get_instance_data().app_id.to_string() : RX_NULL_ITEM_NAME;
			return rx_result(message.str());
		}
	}
	return true;
}

rx_result port_algorithms::disconnect_application (rx_port_ptr what)
{
	if (what->get_instance_data().disconnect_application(what))
		RUNTIME_LOG_TRACE("port_algorithms", 100, what->meta_info().get_full_path()
			+ " disconnected from application");
	return true;
}


// Class rx_internal::sys_runtime::algorithms::application_algorithms 


rx_result application_algorithms::init_runtime (rx_application_ptr what, runtime::runtime_init_context& ctx)
{
	rx_result ret;
	auto it = platform_runtime_manager::instance().applications_.find(what->meta_info().get_id());
	if (it == platform_runtime_manager::instance().applications_.end())
	{
		platform_runtime_manager::instance().applications_.emplace(what->meta_info().get_id(), what);
		ret = what->get_instance_data().before_init_runtime(what, ctx);
		if (ret)
		{
			ret = what->initialize_runtime(ctx);
			if (ret)
			{
				RUNTIME_LOG_TRACE("application_algorithms", 100, "Initialized "s + rx_item_type_name(rx_application) + " "s + what->meta_info().get_full_path());

				runtime::operational::binded_tags* binded = ctx.tags;
				rx_post_function<rx_application_ptr>([binded](rx_application_ptr whose)
					{
						runtime::runtime_start_context start_ctx = whose->create_start_context();
						auto result = start_runtime(whose, start_ctx, binded);
						if (result)
						{
							RUNTIME_LOG_TRACE("application_algorithms", 100, ("Started "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_full_path()).c_str());
						}
						else
						{
							RUNTIME_LOG_ERROR("application_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_full_path()).c_str() + result.errors_line());
						}
					}, what, what->get_executer());
			}
			else
			{
				ret = "Unable to initialize application runtime";
			}
		}
		else
		{
			ret = "Unable to initialize instance_data";
		}
	}
	else
	{
		ret = "Application with this node id is already registered";
	}
	if (!ret)
	{
		RUNTIME_LOG_CRITICAL("application_algorithms", 800, ("Error initializing "s + rx_item_type_name(rx_application) + " "s + what->meta_info().get_full_path()).c_str() + ret.errors_line());
	}
	return ret;
}

rx_result application_algorithms::start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx, runtime::operational::binded_tags* binded)
{
	auto ret = what->get_instance_data().before_start_runtime(what, ctx, binded);
	ret = what->start_runtime(ctx);
	if (ret)
	{
		platform_runtime_manager::instance().get_cache().add_to_cache(what->get_item_ptr());
	}
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
				RUNTIME_LOG_TRACE("application_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_full_path()).c_str());
				rx_post_function<rx_application_ptr>([callback](rx_application_ptr whose)
					{

						runtime::runtime_deinit_context deinit_ctx;
						auto result = whose->deinitialize_runtime(deinit_ctx);
						if (result)
						{
							whose->get_instance_data().after_deinit_runtime(whose, deinit_ctx);
						}
						auto erase_result = platform_runtime_manager::instance().applications_.erase(whose->meta_info().get_id());
						if (erase_result == 0)
						{
							RUNTIME_LOG_CRITICAL("application_algorithms", 500, "this application is not registered in cache!!!");
						}
						callback(std::move(result));
					}, whose, RX_DOMAIN_META);
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("application_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("application_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_full_path()).c_str());
			}
		}, what, what->get_executer());

	return true;
}

rx_result application_algorithms::stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx)
{
	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
		what->get_instance_data().after_stop_runtime(what, ctx);
	}
	return ret;
}


// Class rx_internal::sys_runtime::algorithms::relations_algorithms 


rx_result relations_algorithms::init_runtime (rx_relation_ptr what, runtime::runtime_init_context& ctx)
{
	return (string_type(_rx_func_)+ " not implemented");
}

rx_result relations_algorithms::start_runtime (rx_relation_ptr what, runtime::runtime_start_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result relations_algorithms::deinit_runtime (rx_relation_ptr what, std::function<void(rx_result&&)> callback, runtime::runtime_deinit_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result relations_algorithms::stop_runtime (rx_relation_ptr what, runtime::runtime_stop_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}


} // namespace algorithms
} // namespace sys_runtime
} // namespace rx_internal

