

/****************************************************************************
*
*  runtime_internal\rx_runtime_algorithms.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_runtime_algorithms
#include "runtime_internal/rx_runtime_algorithms.h"

#include "system/meta/rx_obj_types.h"
#include "rx_runtime_internal.h"
#include "sys_internal/rx_async_functions.h"
#include "model/rx_meta_internals.h"
#include "system/server/rx_platform_item.h"
#include "api/rx_namespace_api.h"
#include "model/rx_model_dependencies.h"


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
rx_result deinit_runtime<meta::object_types::object_type>(rx_object_ptr what
	, rx_result_callback&& callback)
{
	return object_algorithms::deinit_runtime(what, std::move(callback));
}
template<>
rx_result deinit_runtime<meta::object_types::application_type>(rx_application_ptr what
	, rx_result_callback&& callback)
{
	return application_algorithms::deinit_runtime(what, std::move(callback));
}
template<>
rx_result deinit_runtime<meta::object_types::port_type>(rx_port_ptr what
	, rx_result_callback&& callback)
{
	return port_algorithms::deinit_runtime(what, std::move(callback));
}
template<>
rx_result deinit_runtime<meta::object_types::domain_type>(rx_domain_ptr what
	, rx_result_callback&& callback)
{
	return domain_algorithms::deinit_runtime(what, std::move(callback));
}
template<>
rx_result deinit_runtime<meta::object_types::relation_type>(rx_relation_ptr what
	, rx_result_callback&& callback)
{
	return relations_algorithms::deinit_runtime(what, std::move(callback));
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

			rx_post_function_to(what->get_executer(), what, [](rx_object_ptr whose)
				{
					auto ctx = runtime::algorithms::runtime_holder_algorithms<meta::object_types::object_type>::create_start_context(*whose);
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
				}, what);
		}
		else
		{
			result.register_error("Unable to initialize domain runtime");
		}
	}
	else
	{
		result.register_error("Unable to initialize instance_data");
	}
	if (!result)
	{
		RUNTIME_LOG_ERROR("object_algorithms", 800, ("Error initializing "s + rx_item_type_name(rx_object) + " "s + what->meta_info().name).c_str() + result.errors_line());
	}

	return result;
}

rx_result object_algorithms::start_runtime (rx_object_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	if (ret)
	{
		platform_runtime_manager::instance().get_cache().add_to_cache(what);
	}
	return ret;
}

rx_result object_algorithms::deinit_runtime (rx_object_ptr what, rx_result_callback&& callback)
{
	rx_post_function_to(what->get_executer(), what, 
		[](rx_object_ptr whose, rx_result_callback&& callback)
		{
			runtime::runtime_stop_context stop_ctx;
			auto result = stop_runtime(whose, stop_ctx);
			if (result)
			{
				result = algorithms::delete_runtime_structure<object_type>(whose);
				RUNTIME_LOG_TRACE("object_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_object) + " "s + whose->meta_info().get_full_path()).c_str());
				rx_post_function_to(RX_DOMAIN_META, whose, 
					[](rx_object_ptr whose, rx_result_callback&& callback)
					{
						runtime::runtime_deinit_context deinit_ctx;
						auto result = whose->deinitialize_runtime(deinit_ctx);
						if(result)
							result = whose->get_instance_data().after_deinit_runtime(whose, deinit_ctx);
						callback(std::move(result));

					}, whose, std::move(callback));
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("object_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("object_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_object) + " "s + whose->meta_info().get_full_path()).c_str());

				callback(std::move(result));
			}
		}, what, std::move(callback));

	return true;
}

rx_result object_algorithms::stop_runtime (rx_object_ptr what, runtime::runtime_stop_context& ctx)
{
	platform_runtime_manager::instance().get_cache().remove_from_cache(what->get_item_ptr());
	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
		ret = what->get_instance_data().after_stop_runtime(what, ctx);
	}
	return ret;
}

rx_result object_algorithms::connect_domain (rx_object_ptr what)
{
	bool connect_unassigned = true;
	if (!what->get_instance_data().get_data().domain_ref.is_null())
	{
		auto domain_id = api::ns::rx_resolve_runtime_reference<domain_type>(what->get_instance_data().get_data().domain_ref, what->get_directories(), tl::type2type<domain_type>());
		if (domain_id)
		{
			auto domain_ptr = model::platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(domain_id.value());
					
			if (domain_ptr)
			{
				auto temp_ptr = domain_ptr.value();
				if (what->get_instance_data().connect_domain(std::move(temp_ptr), what))
				{
					RUNTIME_LOG_INFO("object_algorithms", 100, what->meta_info().get_full_path()
						+ " connected to domain " + domain_ptr.value()->meta_info().get_full_path());
					connect_unassigned = false;

					model::platform_types_manager::instance().get_dependecies_cache().add_dependency(
						what->meta_info().id, domain_ptr.value()->meta_info().id);
				}
				else
				{
					std::ostringstream message;
					message << "Unable to connect to domain "
						<< !what->get_instance_data().get_data().domain_ref.is_null() ? what->get_instance_data().get_data().domain_ref.to_string() : RX_NULL_ITEM_NAME;
					
					RUNTIME_LOG_ERROR("object_algorithms", 900, message.str());
					return rx_result(message.str());
				}
			}
			else
			{
				RUNTIME_LOG_WARNING("object_algorithms", 900, "Domain Id is invalid, connecting object "s
					+ what->meta_info().get_full_path() + " to unassigned domain.");
			}				
		}
		else
		{
			std::ostringstream ss;
			ss << "Domain ref <"
				<< what->get_instance_data().get_data().domain_ref.to_string()
				<< "> is invalid, connecting object"
				<< what->meta_info().get_full_path()
				<< " to unassigned domain.";
			RUNTIME_LOG_WARNING("object_algorithms", 900, ss.str());
		}
	}
	else
	{
		RUNTIME_LOG_WARNING("object_algorithms", 900, "Domain reference is " RX_NULL_ITEM_NAME ", connecting object "s
			+ what->meta_info().get_full_path() + " to unassigned domain.");
	}
	if(connect_unassigned)
	{
		auto domain_ptr = model::platform_types_manager::instance().get_type_repository<domain_type>().get_runtime(RX_NS_SYSTEM_UNASS_ID);
		if (domain_ptr)
		{
			auto temp_ptr = domain_ptr.value();
			if (what->get_instance_data().connect_domain(std::move(temp_ptr), what))
				RUNTIME_LOG_INFO("object_algorithms", 100, what->meta_info().get_full_path()
					+ " connected to domain " + domain_ptr.value()->meta_info().get_full_path());

			if (domain_ptr)
				model::platform_types_manager::instance().get_dependecies_cache().add_dependency(
					what->meta_info().id, domain_ptr.value()->meta_info().id);
		}
		else
		{
			std::ostringstream message;
			message << "Unable to connect to domain "
				<< !what->get_instance_data().get_data().domain_ref.is_null() ? what->get_instance_data().get_data().domain_ref.to_string() : RX_NULL_ITEM_NAME;
			RUNTIME_LOG_ERROR("object_algorithms", 900, message.str());
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

			rx_post_function_to(what->get_executer(), what,[](rx_domain_ptr whose)
				{
					auto start_ctx = runtime::algorithms::runtime_holder_algorithms<meta::object_types::domain_type>::create_start_context(*whose);
					auto result = start_runtime(whose, start_ctx);
					if (result)
					{
						RUNTIME_LOG_TRACE("domain_algorithms", 100, ("Started "s + rx_item_type_name(rx_domain) + " "s + whose->meta_info().get_full_path()).c_str());
					}
					else
					{
						RUNTIME_LOG_ERROR("domain_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_domain) + " "s + whose->meta_info().get_full_path()).c_str() + result.errors_line());

					}
				}, what);
		}
		else
		{
			result.register_error("Unable to initialize domain runtime");
		}
	}
	else
	{
		result.register_error("Unable to initialize instance_data");
	}
	if (!result)
	{
		for (const auto& error : result.errors())
			RUNTIME_LOG_ERROR("domain_algorithms", 800, error.c_str());
		RUNTIME_LOG_ERROR("domain_algorithms", 800, ("Error initializing "s + rx_item_type_name(rx_object) + " "s + what->meta_info().name).c_str() + result.errors_line());
	}
	return result;
}

rx_result domain_algorithms::start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	if (ret)
	{
		platform_runtime_manager::instance().get_cache().add_to_cache(what);
	}
	return ret;
}

rx_result domain_algorithms::deinit_runtime (rx_domain_ptr what, rx_result_callback&& callback)
{
	rx_post_function_to(what->get_executer(), what, [](rx_domain_ptr whose, rx_result_callback&& callback)
		{
			runtime::runtime_stop_context stop_ctx;
			auto result = stop_runtime(whose, stop_ctx);
			if (result)
			{
				result = algorithms::delete_runtime_structure<domain_type>(whose);
				RUNTIME_LOG_TRACE("domain_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());
				rx_post_function_to(RX_DOMAIN_META, whose,
					[](rx_domain_ptr whose, rx_result_callback&& callback)
					{
						runtime::runtime_deinit_context deinit_ctx;
						auto result = whose->deinitialize_runtime(deinit_ctx);
						if (result)
							result = whose->get_instance_data().after_deinit_runtime(whose, deinit_ctx);
						callback(std::move(result));
					}, whose, std::move(callback));
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("domain_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("domain_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());
				
				callback(std::move(result));
			}
		}, what, std::move(callback));

	return true;
}

rx_result domain_algorithms::stop_runtime (rx_domain_ptr what, runtime::runtime_stop_context& ctx)
{
	platform_runtime_manager::instance().get_cache().remove_from_cache(what->get_item_ptr());
	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
		what->get_instance_data().after_stop_runtime(what, ctx);
	}
	return ret;
}

rx_result domain_algorithms::connect_application (rx_domain_ptr what)
{
	bool connect_unassigned = true;
	if (!what->get_instance_data().get_data().app_ref.is_null())
	{
		auto app_id = api::ns::rx_resolve_runtime_reference<application_type>(what->get_instance_data().get_data().app_ref, what->get_directories(), tl::type2type<application_type>());
		if (app_id)
		{
			auto app_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(app_id.value());

			if (app_ptr)
			{
				auto temp_ptr = app_ptr.value();
				if (what->get_instance_data().connect_application(std::move(temp_ptr), what))
				{
					RUNTIME_LOG_INFO("domain_algorithms", 100, what->meta_info().get_full_path()
						+ " connected to application " + app_ptr.value()->meta_info().get_full_path());
					connect_unassigned = false;

					model::platform_types_manager::instance().get_dependecies_cache().add_dependency(
						what->meta_info().id, app_ptr.value()->meta_info().id);
				}
				else
				{
					std::ostringstream message;
					message << "Unable to connect to application "
						<< !what->get_instance_data().get_data().app_ref.is_null() ? what->get_instance_data().get_data().app_ref.to_string() : RX_NULL_ITEM_NAME;

					RUNTIME_LOG_ERROR("domain_algorithms", 900, message.str());
					return rx_result(message.str());
				}
			}
			else
			{
				RUNTIME_LOG_WARNING("domain_algorithms", 900, "Application Id is invalid, connecting domain "s
					+ what->meta_info().get_full_path() + " to unassigned application.");
			}
		}
		else
		{
			std::ostringstream ss;
			ss << "Application ref <"
				<< what->get_instance_data().get_data().app_ref.to_string()
				<< "> is invalid, connecting domain"
				<< what->meta_info().get_full_path()
				<< " to unassigned application.";
			RUNTIME_LOG_WARNING("domain_algorithms", 900, ss.str());
		}
	}
	else
	{
		RUNTIME_LOG_WARNING("domain_algorithms", 900, "Application reference is " RX_NULL_ITEM_NAME ", connecting domain "s
			+ what->meta_info().get_full_path() + " to unassigned application.");
	}
	if (connect_unassigned)
	{
		auto app_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(RX_NS_SYSTEM_UNASS_APP_ID);
		if (app_ptr)
		{
			auto temp_ptr = app_ptr.value();
			if (what->get_instance_data().connect_application(std::move(temp_ptr), what))
				RUNTIME_LOG_INFO("domain_algorithms", 100, what->meta_info().get_full_path()
					+ " connected to application " + app_ptr.value()->meta_info().get_full_path());

			if (app_ptr)
				model::platform_types_manager::instance().get_dependecies_cache().add_dependency(
					what->meta_info().id, app_ptr.value()->meta_info().id);
		}
		else
		{
			std::ostringstream message;
			message << "Unable to connect to application "
				<< !what->get_instance_data().get_data().app_ref.is_null() ? what->get_instance_data().get_data().app_ref.to_string() : RX_NULL_ITEM_NAME;
			RUNTIME_LOG_ERROR("domain_algorithms", 900, message.str());
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
			result = what->get_instance_data().stack_data.init_runtime_data(ctx);
			RUNTIME_LOG_TRACE("port_algorithms", 100, "Initialized "s + rx_item_type_name(rx_port) + " "s + what->meta_info().get_full_path());

			rx_post_function_to(what->get_executer(), what, [](rx_port_ptr whose)
				{
					auto start_ctx = runtime::algorithms::runtime_holder_algorithms<meta::object_types::port_type>::create_start_context(*whose);
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
				}, what);
		}
		else
		{
			result.register_error("Unable to initialize port runtime");
		}
	}
	else
	{
		result.register_error("Unable to initialize instance_data");
	}

	return result;
}

rx_result port_algorithms::start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx)
{
	auto ret = what->start_runtime(ctx);
	if (ret)
	{
		platform_runtime_manager::instance().get_cache().add_to_cache(what);
	}
	return ret;
}

rx_result port_algorithms::deinit_runtime (rx_port_ptr what, rx_result_callback&& callback)
{
	rx_post_function_to(what->get_executer(), what, [](rx_port_ptr whose, rx_result_callback&& callback)
		{
			runtime::runtime_stop_context stop_ctx;
			auto result = stop_runtime(whose, stop_ctx);
			if (result)
			{
				result = algorithms::delete_runtime_structure<port_type>(whose);
				RUNTIME_LOG_TRACE("port_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());
				rx_post_function_to(RX_DOMAIN_META, whose,
					[](rx_port_ptr whose, rx_result_callback&& callback)
					{
						runtime::runtime_deinit_context deinit_ctx;
						auto result = whose->deinitialize_runtime(deinit_ctx);
						if(result)
							result = whose->get_instance_data().after_deinit_runtime(whose, deinit_ctx);
						callback(std::move(result));

					}, whose, std::move(callback));
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("port_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("port_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_port) + " "s + whose->meta_info().get_full_path()).c_str());

				callback(std::move(result));
			}
		}, what, std::move(callback));

	return true;
}

rx_result port_algorithms::stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx)
{
	platform_runtime_manager::instance().get_cache().remove_from_cache(what->get_item_ptr());
	auto ret = what->stop_runtime(ctx);
	if (ret)
	{
		what->get_instance_data().after_stop_runtime(what, ctx);
	}
	return ret;
}

rx_result port_algorithms::connect_application (rx_port_ptr what)
{
	bool connect_unassigned = true;
	if (!what->get_instance_data().get_data().app_ref.is_null())
	{
		auto app_id = api::ns::rx_resolve_runtime_reference<application_type>(what->get_instance_data().get_data().app_ref, what->get_directories(), tl::type2type<application_type>());
		if (app_id)
		{
			auto app_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(app_id.value());

			if (app_ptr)
			{
				auto temp_ptr = app_ptr.value();
				if (what->get_instance_data().connect_application(std::move(temp_ptr), what))
				{
					RUNTIME_LOG_INFO("port_algorithms", 100, what->meta_info().get_full_path()
						+ " connected to application " + app_ptr.value()->meta_info().get_full_path());
					connect_unassigned = false;

					model::platform_types_manager::instance().get_dependecies_cache().add_dependency(
						what->meta_info().id, app_ptr.value()->meta_info().id);
				}
				else
				{
					std::ostringstream message;
					message << "Unable to connect to application "
						<< !what->get_instance_data().get_data().app_ref.is_null() ? what->get_instance_data().get_data().app_ref.to_string() : RX_NULL_ITEM_NAME;

					RUNTIME_LOG_ERROR("port_algorithms", 900, message.str());
					return rx_result(message.str());
				}
			}
			else
			{
				RUNTIME_LOG_WARNING("port_algorithms", 900, "Application Id is invalid, connecting port "s
					+ what->meta_info().get_full_path() + " to unassigned application.");
			}
		}
		else
		{
			std::ostringstream ss;
			ss << "Application ref <"
				<< what->get_instance_data().get_data().app_ref.to_string()
				<< "> is invalid, connecting port"
				<< what->meta_info().get_full_path()
				<< " to unassigned application.";
			RUNTIME_LOG_WARNING("port_algorithms", 900, ss.str());
		}
	}
	else
	{
		RUNTIME_LOG_WARNING("port_algorithms", 900, "Application reference is " RX_NULL_ITEM_NAME ", connecting port "s
			+ what->meta_info().get_full_path() + " to unassigned application.");
	}
	if (connect_unassigned)
	{
		auto app_ptr = model::platform_types_manager::instance().get_type_repository<application_type>().get_runtime(RX_NS_SYSTEM_UNASS_APP_ID);
		if (app_ptr)
		{
			auto temp_ptr = app_ptr.value();
			if (what->get_instance_data().connect_application(std::move(temp_ptr), what))
				RUNTIME_LOG_INFO("port_algorithms", 100, what->meta_info().get_full_path()
					+ " connected to application " + app_ptr.value()->meta_info().get_full_path());

			if (app_ptr)
				model::platform_types_manager::instance().get_dependecies_cache().add_dependency(
					what->meta_info().id, app_ptr.value()->meta_info().id);
		}
		else
		{
			std::ostringstream message;
			message << "Unable to connect to application "
				<< !what->get_instance_data().get_data().app_ref.is_null() ? what->get_instance_data().get_data().app_ref.to_string() : RX_NULL_ITEM_NAME;
			RUNTIME_LOG_ERROR("port_algorithms", 900, message.str());
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
	auto it = platform_runtime_manager::instance().applications_.find(what->meta_info().id);
	if (it == platform_runtime_manager::instance().applications_.end())
	{
		platform_runtime_manager::instance().applications_.emplace(what->meta_info().id, what);
		ret = what->get_instance_data().before_init_runtime(what, ctx);
		if (ret)
		{
			ret = what->initialize_runtime(ctx);
			if (ret)
			{
				RUNTIME_LOG_TRACE("application_algorithms", 100, "Initialized "s + rx_item_type_name(rx_application) + " "s + what->meta_info().get_full_path());

				runtime::tag_blocks::binded_tags* binded = ctx.tags;
				rx_post_function_to(what->get_executer(), what, [](rx_application_ptr whose, runtime::tag_blocks::binded_tags* binded)
					{
						auto start_ctx = runtime::algorithms::runtime_holder_algorithms<meta::object_types::application_type>::create_start_context(*whose);
						auto result = start_runtime(whose, start_ctx, binded);
						if (result)
						{
							RUNTIME_LOG_TRACE("application_algorithms", 100, ("Started "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_full_path()).c_str());
						}
						else
						{
							RUNTIME_LOG_ERROR("application_algorithms", 800, ("Error starting "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_full_path()).c_str() + result.errors_line());
						}
					}, what, binded);
			}
			else
			{
				ret.register_error("Unable to initialize application runtime");
			}
		}
		else
		{
			ret.register_error("Unable to initialize instance_data");
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

rx_result application_algorithms::start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx, runtime::tag_blocks::binded_tags* binded)
{
	auto ret = what->get_instance_data().before_start_runtime(what, ctx, binded);
	ret = what->start_runtime(ctx);
	if (ret)
	{
		platform_runtime_manager::instance().get_cache().add_to_cache(what);
	}
	return ret;
}

rx_result application_algorithms::deinit_runtime (rx_application_ptr what, rx_result_callback&& callback)
{
	rx_post_function_to(what->get_executer(), what, [](rx_application_ptr whose, rx_result_callback&& callback)
		{
			runtime::runtime_stop_context stop_ctx;
			auto result = stop_runtime(whose, stop_ctx);
			if (result)
			{
				result = algorithms::delete_runtime_structure<application_type>(whose);
				RUNTIME_LOG_TRACE("application_algorithms", 100, ("Stopped "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_full_path()).c_str());
				rx_post_function_to(RX_DOMAIN_META, whose,
					[](rx_application_ptr whose, rx_result_callback&& callback)
						{

							runtime::runtime_deinit_context deinit_ctx;
							auto result = whose->deinitialize_runtime(deinit_ctx);
							if (result)
							{
								whose->get_instance_data().after_deinit_runtime(whose, deinit_ctx);
							}
							auto erase_result = platform_runtime_manager::instance().applications_.erase(whose->meta_info().id);
							if (erase_result == 0)
							{
								RUNTIME_LOG_CRITICAL("application_algorithms", 500, "this application is not registered in cache!!!");
							}
							callback(std::move(result));

						}, whose, std::move(callback));
			}
			else
			{
				for (const auto& error : result.errors())
					RUNTIME_LOG_ERROR("application_algorithms", 800, error.c_str());
				RUNTIME_LOG_ERROR("application_algorithms", 800, ("Error stopping "s + rx_item_type_name(rx_application) + " "s + whose->meta_info().get_full_path()).c_str());

				callback(std::move(result));
			}
		}, what, std::move(callback));

	return true;
}

rx_result application_algorithms::stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx)
{
	platform_runtime_manager::instance().get_cache().remove_from_cache(what->get_item_ptr());
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

rx_result relations_algorithms::deinit_runtime (rx_relation_ptr what, rx_result_callback&& callback)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result relations_algorithms::stop_runtime (rx_relation_ptr what, runtime::runtime_stop_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}


// Class rx_internal::sys_runtime::algorithms::shutdown_algorithms 


void shutdown_algorithms::stop_applications (std::vector<rx_application_ptr> apps)
{
	for (auto app : apps)
	{
		std::vector<rx_port_ptr> ports = app->get_instance_data().get_ports();
		stop_ports(std::move(ports));
		runtime::runtime_stop_context stop_ctx;
		algorithms::application_algorithms::stop_runtime(app, stop_ctx);
	}
}

void shutdown_algorithms::stop_domains (std::vector<rx_domain_ptr> domains)
{
	for (auto domain : domains)
	{
		std::vector<rx_object_ptr> objects = domain->get_instance_data().get_objects();
		stop_objects(std::move(objects));
		runtime::runtime_stop_context stop_ctx;
		algorithms::domain_algorithms::stop_runtime(domain, stop_ctx);
	}
}

void shutdown_algorithms::stop_ports (std::vector<rx_port_ptr> ports)
{
	for (auto port : ports)
	{
		runtime::runtime_stop_context stop_ctx;
		algorithms::port_algorithms::stop_runtime(port, stop_ctx);
	}
}

void shutdown_algorithms::stop_objects (std::vector<rx_object_ptr> objects)
{
	for (auto one : objects)
	{
		runtime::runtime_stop_context stop_ctx;
		algorithms::object_algorithms::stop_runtime(one, stop_ctx);
	}
}

void shutdown_algorithms::deinit_applications (std::vector<rx_application_ptr> apps)
{
	for (auto app : apps)
	{
		std::vector<rx_port_ptr> ports = app->get_instance_data().get_ports();
		deinit_ports(std::move(ports));
		std::vector<rx_domain_ptr> domains = app->get_instance_data().get_domains();
		deinit_domains(std::move(domains));
		runtime::runtime_deinit_context deinit_ctx;
		app->deinitialize_runtime(deinit_ctx);
		app->get_instance_data().after_deinit_runtime(app, deinit_ctx);
		platform_runtime_manager::instance().applications_.erase(app->meta_info().id);
	}
}

void shutdown_algorithms::deinit_domains (std::vector<rx_domain_ptr> domains)
{
	for (auto domain : domains)
	{
		std::vector<rx_object_ptr> objects = domain->get_instance_data().get_objects();
		deinit_objects(std::move(objects));
		algorithms::domain_algorithms::disconnect_application(domain);
		runtime::runtime_deinit_context deinit_ctx;
		domain->deinitialize_runtime(deinit_ctx);
		domain->get_instance_data().after_deinit_runtime(domain, deinit_ctx);
	}
}

void shutdown_algorithms::deinit_ports (std::vector<rx_port_ptr> ports)
{
	for (auto port : ports)
	{
		algorithms::port_algorithms::disconnect_application(port);
		runtime::runtime_deinit_context deinit_ctx;
		port->deinitialize_runtime(deinit_ctx);
		port->get_instance_data().after_deinit_runtime(port, deinit_ctx);
	}
}

void shutdown_algorithms::deinit_objects (std::vector<rx_object_ptr> objects)
{
	for (auto one : objects)
	{
		algorithms::object_algorithms::disconnect_domain(one);
		runtime::runtime_deinit_context deinit_ctx;
		one->deinitialize_runtime(deinit_ctx);
		one->get_instance_data().after_deinit_runtime(one, deinit_ctx);
	}
}


} // namespace algorithms
} // namespace sys_runtime
} // namespace rx_internal

