

/****************************************************************************
*
*  sys_internal\rx_internal_builders.cpp
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


// rx_internal_builders
#include "sys_internal/rx_internal_builders.h"

#include "rx_storage_build.h"
#include "system/server/rx_server.h"
#include "terminal/rx_commands.h"
#include "sys_internal/rx_inf.h"
#include "testing/rx_test.h"
#include "api/rx_meta_api.h"
#include "rx_configuration.h"
#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "sys_internal/rx_plugin_manager.h"
#include "system/server/rx_directory_cache.h"
#include "system/runtime/rx_internal_objects.h"
using namespace rx_platform::meta::object_types;


namespace rx_internal {

namespace builders {

template<class T>
rx_reference<T> create_type(meta::object_type_creation_data data)
{
	auto ret = rx_create_reference<T>();
	ret->meta_info = data;
	return ret;
}
template<class T>
rx_reference<T> create_type(meta::type_creation_data data)
{
	auto ret = rx_create_reference<T>();
	ret->meta_info = data;
	return ret;
}

template<class T>
void add_type_to_configuration(rx_directory_ptr dir, rx_reference<T> what, bool abstract_type)
{
	what->meta_info = create_meta_for_new(what->meta_info);
	what->complex_data.is_abstract = abstract_type;
	auto result = model::platform_types_manager::instance().get_type_repository<T>().register_type(what);
	if (!result)
	{
		for (const auto& one : result.errors())
		{
			BUILD_LOG_ERROR("builders", 500, one);
		}
		BUILD_LOG_ERROR("builders", 500, "Unable to register "s + rx_item_type_name(T::type_id) + " "s + what->meta_info.get_full_path());
	}
	result = dir->add_item(what->get_item_ptr());
	if (!result)
	{
		for (const auto& one : result.errors())
		{
			BUILD_LOG_ERROR("builders", 500, one);
		}
		BUILD_LOG_ERROR("builders", 500, "Unable to add "s + rx_item_type_name(T::type_id) + " "s + what->meta_info.get_full_path() + " to directory.");
	}
}

template<class T>
void add_simple_type_to_configuration(rx_directory_ptr dir, rx_reference<T> what, bool abstract_type)
{
	what->meta_info = create_meta_for_new(what->meta_info);
	what->complex_data.is_abstract = abstract_type;
	auto result = model::platform_types_manager::instance().get_simple_type_repository<T>().register_type(what);
	if (!result)
	{
		for (const auto& one : result.errors())
		{
			BUILD_LOG_ERROR("builders", 500, one);
		}
		BUILD_LOG_ERROR("builders", 500, "Unable to register "s + rx_item_type_name(T::type_id) + " "s + what->meta_info.get_full_path());
	}
	result = dir->add_item(what->get_item_ptr());
	if (!result)
	{
		BUILD_LOG_ERROR("builders", 500, "Unable to add "s + rx_item_type_name(T::type_id) + " "s + what->meta_info.get_full_path() + " to directory.");
		for (const auto& one : result.errors())
		{
			BUILD_LOG_ERROR("builders", 500, one);
		}
	}
}


void add_data_type_to_configuration(rx_directory_ptr dir, data_type_ptr what)
{
	what->meta_info = create_meta_for_new(what->meta_info);
	auto result = model::platform_types_manager::instance().get_data_types_repository().register_type(what);
	if (!result)
	{
		for (const auto& one : result.errors())
		{
			BUILD_LOG_ERROR("builders", 500, one);
		}
		BUILD_LOG_ERROR("builders", 500, "Unable to register "s + rx_item_type_name(basic_types::data_type::type_id) + " "s + what->meta_info.get_full_path());
	}
	result = dir->add_item(what->get_item_ptr());
	if (!result)
	{
		BUILD_LOG_ERROR("builders", 500, "Unable to add "s + rx_item_type_name(basic_types::data_type::type_id) + " "s + what->meta_info.get_full_path() + " to directory.");
		for (const auto& one : result.errors())
		{
			BUILD_LOG_ERROR("builders", 500, one);
		}
	}
}

void add_relation_type_to_configuration(rx_directory_ptr dir, relation_type::smart_ptr what)
{
	what->meta_info = create_meta_for_new(what->meta_info);
	auto result = model::platform_types_manager::instance().get_relations_repository().register_type(what);
	if (!result)
	{
		for (const auto& one : result.errors())
		{
			BUILD_LOG_ERROR("builders", 500, one);
		}
		BUILD_LOG_ERROR("builders", 500, "Unable to register "s + rx_item_type_name(rx_item_type::rx_relation_type) + " "s + what->meta_info.get_full_path());
	}
	result = dir->add_item(what->get_item_ptr());
	if (!result)
	{
		BUILD_LOG_ERROR("builders", 500, "Unable to add "s + rx_item_type_name(rx_item_type::rx_relation_type) + " "s + what->meta_info.get_full_path() + " to directory.");
		for (const auto& one : result.errors())
		{
			BUILD_LOG_ERROR("builders", 500, one);
		}
	}
}

template<class T>
rx_result add_object_to_configuration(rx_directory_ptr dir, typename T::instance_data_t&& data, data::runtime_values_data&& runtime_data, tl::type2type<T>
, std::function<void(typename T::RTypePtr)> result_f = std::function<void(typename T::RTypePtr)>())
{
	data.meta_info = create_meta_for_new(data.meta_info);
	auto create_result = model::algorithms::runtime_model_algorithm<T>::create_runtime_sync(std::move(data), std::move(runtime_data));
	return true;
	if (create_result)
	{
		auto rx_type_item = create_result.value()->get_item_ptr();
		BUILD_LOG_TRACE("code_objects", 100, ("Created "s + rx_item_type_name(T::RImplType::type_id) + " "s + rx_type_item->get_name()).c_str());
		if (result_f)
			result_f(create_result.value());
		return true;
	}
	else
	{
		create_result.register_error("Error creating "s + rx_item_type_name(T::RImplType::type_id) + " " + data.meta_info.get_full_path());
		for(const auto& one : create_result.errors())
			BUILD_LOG_ERROR("code_objects", 900, one.c_str());
		return create_result.errors();
	}
}


// Class rx_internal::builders::rx_platform_builder 

rx_platform_builder::rx_platform_builder()
{
}

rx_platform_builder::rx_platform_builder(const rx_platform_builder &right)
{
}


rx_platform_builder::~rx_platform_builder()
{
}


rx_platform_builder & rx_platform_builder::operator=(const rx_platform_builder &right)
{
	assert(false);
	return *this;
}



rx_result rx_platform_builder::build_platform (hosting::rx_platform_host* host, namespace_data_t& data, const meta_configuration_data_t& meta_data)
{
	rx_result errors = register_system_constructors();
	if (!errors)
		return errors;

	auto sys_builders = get_system_builders(data, meta_data, host);
	auto user_builders = get_user_builders(data, host);
	auto test_builders = get_test_builders(data, host);
	auto other_builders = get_other_builders(data, host);

	auto root = ns::rx_directory_cache::instance().get_root();

	BUILD_LOG_INFO("rx_platform_builder", 900, "Building platform...");

	BUILD_LOG_INFO("rx_platform_builder", 900, "Building system...");
	for (auto& one : sys_builders)
	{
		auto result = one->do_build();
		if (!result)
		{
			BUILD_LOG_ERROR("rx_platform_builder", 900, "Error building platform system!");
			errors.register_errors(result.errors());
			return errors;
		}
	}
	BUILD_LOG_INFO("rx_platform_builder", 900, "System built.");
	BUILD_LOG_INFO("rx_platform_builder", 900, "Building unassigned system...");
	errors = buid_unassigned(host, data);
	if (errors)
		BUILD_LOG_INFO("rx_platform_builder", 900, "Unassigned system built.");
	// unassigned is critical so an error in building system is fatal
	else
		return errors;

	if (meta_data.build_system_from_code)
	//@@@@STUPID but have to be turned off for a while, hence the @'s :)
	{
		BUILD_LOG_INFO("rx_platform_builder", 900, "Building host items...");
		auto dir_ptr = ns::rx_directory_cache::instance().get_directory(RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_HOST_NAME);
		if (dir_ptr)
		{
			hosting::host_platform_builder host_builder;
			host_builder.host_root = dir_ptr->meta_info().get_full_path();
			errors = host->build_host(host_builder);
			if (!errors)
			{
				errors.register_error("Unable to build host "s + host->get_host_name());
				return errors;
			}
		}
		else
		{
			errors.register_error("Unable to build host "s + host->get_host_name() + "Invalid directory!");
			return errors;
		}
		BUILD_LOG_INFO("rx_platform_builder", 900, "Host items built.");

		BUILD_LOG_INFO("rx_platform_builder", 900, "Building plugins...");
		for (auto one : rx_internal::plugins::plugins_manager::instance().get_plugins())
		{
			auto info = one->get_plugin_info();
			BUILD_LOG_INFO("rx_platform_builder", 900, ("Found plugin "s + one->get_plugin_name() + " [" + info.plugin_version + "]..."s));
			string_type root_path(RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_PLUGINS_NAME RX_DIR_DELIMETER_STR);
			root_path += one->get_plugin_name();
			auto dir_ptr = ns::rx_directory_cache::instance().get_directory(root_path);
			if (dir_ptr)
			{
				library::plugin_builder builder;
				builder.plugin_root = dir_ptr->meta_info().get_full_path();;
				errors = one->build_plugin(builder);
				if (!errors)
				{
					errors.register_error("Unable to build plugin "s + one->get_plugin_name());
					return errors;
				}
			}
			else
			{
				errors.register_error("Unable to build plugin "s + one->get_plugin_name() + "Invalid directory!");
				return errors;
			}
			BUILD_LOG_INFO("rx_platform_builder", 900, ("Plugin "s + one->get_plugin_name() + " built."s));
		}
		BUILD_LOG_INFO("rx_platform_builder", 900, "All plugins built.");
	}
	else
	{
		BUILD_LOG_INFO("rx_platform_builder", 900, "Building host items...");

		auto storage_ptr = host->get_system_storage(host->get_host_name());
		if (storage_ptr)
		{
			storage::configuration_storage_builder builder(storage_ptr.value());
			errors = builder.do_build();
			if (!errors)
			{
				errors.register_error("Unable to build host "s + host->get_host_name());
				return errors;
			}
		}
		BUILD_LOG_INFO("rx_platform_builder", 900, "Host items built.");

		BUILD_LOG_INFO("rx_platform_builder", 900, "Building plugins...");
		for (auto one : rx_internal::plugins::plugins_manager::instance().get_plugins())
		{
			auto info = one->get_plugin_info();
			BUILD_LOG_INFO("rx_platform_builder", 900, ("Found plugin "s + one->get_plugin_name() + " [" + info.plugin_version + "]..."s));
			auto storage_ptr = host->get_system_storage(one->get_plugin_name());
			if (storage_ptr)
			{
				storage::configuration_storage_builder builder(storage_ptr.value());
				errors = builder.do_build();
				if (!errors)
				{
					errors.register_error("Unable to build plugin "s + one->get_plugin_name());
					return errors;
				}
			}
			BUILD_LOG_INFO("rx_platform_builder", 900, ("Plugin "s + one->get_plugin_name() + " built."s));
		}
		BUILD_LOG_INFO("rx_platform_builder", 900, "All plugins built.");

	}

	BUILD_LOG_INFO("rx_platform_builder", 900, "Building user items...");

	for (auto& one : user_builders)
	{
		auto result = one->do_build();
		if (!result)
		{
			BUILD_LOG_ERROR("rx_platform_builder", 900, "Errors occurred while building platform user configuration!");
		}
	}
	BUILD_LOG_INFO("rx_platform_builder", 900, "User items built.");

	if (!test_builders.empty())
	{
		for (auto& one : test_builders)
		{
			auto result = one->do_build();
			if (!result)
			{
				BUILD_LOG_WARNING("rx_platform_builder", 900, "Errors occurred while building platform test configuration!");
			}
		}
	}
	for (auto& one : other_builders)
	{
		auto result = one->do_build();
		if (!result)
		{
			BUILD_LOG_WARNING("rx_platform_builder", 900, "Error building platform additional configuration!");
		}
	}

	BUILD_LOG_INFO("rx_platform_builder", 900, "Platform built, initializing...");

	return true;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_system_builders (namespace_data_t& data, const meta_configuration_data_t& meta_data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// create system folder structure
	builders.emplace_back(std::make_unique<root_folder_builder>());
	if (meta_data.build_system_from_code)
	{
		// types builders
		builders.emplace_back(std::make_unique<basic_types_builder>());
		builders.emplace_back(std::make_unique<support_types_builder>());
		builders.emplace_back(std::make_unique<terminal_commands_builder>());
		builders.emplace_back(std::make_unique<system_types_builder>());
		builders.emplace_back(std::make_unique<port_types_builder>());
		builders.emplace_back(std::make_unique<relation_types_builder>());
		builders.emplace_back(std::make_unique<simulation_types_builder>());
		//// objects builders
		builders.emplace_back(std::make_unique<system_objects_builder>());
		builders.emplace_back(std::make_unique<system_ports_builder>());
	}
	else
	{
		// storage builder
		auto storage_result = host->get_system_storage("sys");
		if (storage_result)
			builders.emplace_back(std::make_unique<storage::configuration_storage_builder>(storage_result.value()));
	}
	return builders;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_user_builders (namespace_data_t& data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// storage builder
	auto storage_result = host->get_user_storage();
	if (storage_result)
		builders.emplace_back(std::make_unique<storage::configuration_storage_builder>(storage_result.value()));
	return builders;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_test_builders (namespace_data_t& data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// storage builder
	auto storage_result = host->get_test_storage();
	if (storage_result)
		builders.emplace_back(std::make_unique<storage::configuration_storage_builder>(storage_result.value()));
	return builders;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_other_builders (namespace_data_t& data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	return builders;
}

rx_result rx_platform_builder::register_system_constructors ()
{
	// system app
	auto result = model::platform_types_manager::instance().get_type_repository<application_type>().register_constructor(
		RX_NS_SYSTEM_APP_TYPE_ID, [] { return rx_platform::sys_objects::system_application::instance(); } );
	if (!result)
	{
		result.register_error("Error registering constructor for system application!");
		return result;
	}
	// system domain
	result = model::platform_types_manager::instance().get_type_repository<domain_type>().register_constructor(
		RX_NS_SYSTEM_DOM_TYPE_ID, [] { return rx_platform::sys_objects::system_domain::instance(); });
	if (!result)
	{
		result.register_error("Error registering constructor for system domain!");
		return result;
	}
	// system object
	result = model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
		RX_NS_SYSTEM_TYPE_ID, [] { return rx_platform::sys_objects::system_object::instance(); });
	if (!result)
	{
		result.register_error("Error registering constructor for system object!");
		return result;
	}
	// host object
	result = model::platform_types_manager::instance().get_type_repository<object_type>().register_constructor(
		RX_NS_HOST_TYPE_ID, [] { return rx_platform::sys_objects::host_object::instance(); });
	if (!result)
	{
		result.register_error("Error registering constructor for system object!");
		return result;
	}
	// unassigned app
	result = model::platform_types_manager::instance().get_type_repository<application_type>().register_constructor(
		RX_NS_SYSTEM_UNASS_APP_TYPE_ID, [] { return rx_platform::sys_objects::unassigned_application::instance(); });
	if (!result)
	{
		result.register_error("Error registering constructor for unassigned application!");
		return result;
	}
	// unassigned domain
	result = model::platform_types_manager::instance().get_type_repository<domain_type>().register_constructor(
		RX_NS_SYSTEM_UNASS_TYPE_ID, [] { return rx_platform::sys_objects::unassigned_domain::instance(); });
	if (!result)
	{
		result.register_error("Error registering constructor for unassigned domain!");
		return result;
	}
	return true;
}

rx_result rx_platform_builder::buid_unassigned (hosting::rx_platform_host* host, namespace_data_t& data)
{
	string_type path(RX_NS_UNASSIGNED_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir)
	{
		runtime_data::application_runtime_data app_instance_data;
		app_instance_data.meta_info.name = RX_NS_SYSTEM_UNASS_APP_NAME;
		app_instance_data.meta_info.id = RX_NS_SYSTEM_UNASS_APP_ID;
		app_instance_data.meta_info.parent = RX_NS_SYSTEM_UNASS_APP_TYPE_ID;
		app_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		app_instance_data.meta_info.path = full_path;
		app_instance_data.instance_data.processor = 1;
		app_instance_data.instance_data.priority = rx_domain_priority::low;
		auto result = add_object_to_configuration(dir, std::move(app_instance_data), data::runtime_values_data(), tl::type2type<application_type>());
		if (!result)
		{
			result.register_error("Unable to add unassigned application!");
			return result;
		}
		runtime_data::domain_runtime_data instance_data;
		instance_data.meta_info.name = RX_NS_SYSTEM_UNASS_NAME;
		instance_data.meta_info.id = RX_NS_SYSTEM_UNASS_ID;
		instance_data.meta_info.parent = RX_NS_SYSTEM_UNASS_TYPE_ID;
		instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		instance_data.meta_info.path = full_path;
		instance_data.instance_data.processor = 1;
		instance_data.instance_data.app_ref = rx_item_reference(RX_NS_SYSTEM_UNASS_APP_NAME);
		instance_data.instance_data.priority = rx_domain_priority::low;
		auto result2 = add_object_to_configuration(dir, std::move(instance_data), data::runtime_values_data(), tl::type2type<domain_type>());
		if (!result2)
		{
			result2.register_error("Unable to add unassigned domain!");
			return result2;
		}
	}
	return true;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_plugin_builders (namespace_data_t& data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	return builders;
}

void rx_platform_builder::deinitialize ()
{
	rx_platform::sys_objects::system_application::instance()->deinitialize();
	rx_platform::sys_objects::system_domain::instance()->deinitialize();
	rx_platform::sys_objects::system_object::instance()->deinitialize();
	rx_platform::sys_objects::host_object::instance()->deinitialize();
	rx_platform::sys_objects::unassigned_application::instance()->deinitialize();
	rx_platform::sys_objects::unassigned_domain::instance()->deinitialize();
	// now delete directories recursive

	recursive_destory_fs(ns::rx_directory_cache::instance().get_root());
}

void rx_platform_builder::recursive_destory_fs (rx_directory_ptr dir)
{
	platform_directories_type dirs;
	platform_items_type items;
	if(ns::rx_directory_cache::instance().get_sub_directories(dir, dirs))
	for (auto one : dirs)
	{
		recursive_destory_fs(one);
	}
	ns::rx_directory_cache::instance().remove_directory(dir->meta_info().get_full_path());
}


// Class rx_internal::builders::root_folder_builder 


rx_result root_folder_builder::do_build ()
{
	std::vector<std::pair<string_type, rx_storage_ptr> > dirs =
	{
		{ RX_DIR_DELIMETER_STR RX_NS_WORLD_NAME
			, rx_gate::instance().get_host()->get_user_storage().value() }, // /world
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME
			, rx_gate::instance().get_host()->get_system_storage("sys").value() }, // /sys
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_BIN_NAME
			, rx_storage_ptr::null_ptr }, // /sys/bin
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_HOST_NAME
			, rx_gate::instance().get_host()->get_system_storage(rx_gate::instance().get_host()->get_host_name()).value() }, // /sys/host
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_PLUGINS_NAME
			, rx_storage_ptr::null_ptr }, // /sys/plugins
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_OBJ_NAME
			, rx_storage_ptr::null_ptr }, // /sys/runtime
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_OBJ_NAME RX_DIR_DELIMETER_STR RX_NS_SYSTEM_OBJ_NAME
			, rx_storage_ptr::null_ptr },
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_OBJ_NAME RX_DIR_DELIMETER_STR RX_NS_PORT_OBJ_NAME
			, rx_storage_ptr::null_ptr },
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_CLASSES_NAME
			, rx_storage_ptr::null_ptr }, // /sys/types
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_CLASSES_NAME RX_DIR_DELIMETER_STR RX_NS_BASE_CLASSES_NAME
			, rx_storage_ptr::null_ptr },
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_CLASSES_NAME RX_DIR_DELIMETER_STR RX_NS_SYSTEM_CLASSES_NAME
			, rx_storage_ptr::null_ptr },
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_CLASSES_NAME RX_DIR_DELIMETER_STR RX_NS_PORT_CLASSES_NAME
			, rx_storage_ptr::null_ptr },
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_CLASSES_NAME RX_DIR_DELIMETER_STR RX_NS_SUPPORT_CLASSES_NAME
			, rx_storage_ptr::null_ptr },
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_CLASSES_NAME RX_DIR_DELIMETER_STR RX_NS_SIMULATION_CLASSES_NAME
			, rx_storage_ptr::null_ptr },
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_CLASSES_NAME RX_DIR_DELIMETER_STR RX_NS_RELATIONS_NAME
			, rx_storage_ptr::null_ptr },
		{ RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_CLASSES_NAME RX_DIR_DELIMETER_STR RX_NS_TERMINAL_NAME
			, rx_storage_ptr::null_ptr },
		{ RX_DIR_DELIMETER_STR RX_NS_UNASSIGNED_NAME
			, rx_storage_ptr::null_ptr }, // /unassigned
	};

	for (const auto& one : dirs)
	{
		auto ret = ns::rx_directory_cache::instance().add_directory(one.first, one.second);
		if (!ret)
		{
			ret.register_error("Unable to add directory "s + one.first + ".");
			return ret.errors();
		}
	}

	auto& plugins = plugins::plugins_manager::instance().get_plugins();
	for (auto& plugin : plugins)
	{
		string_type path = RX_DIR_DELIMETER_STR RX_NS_SYS_NAME RX_DIR_DELIMETER_STR RX_NS_PLUGINS_NAME RX_DIR_DELIMETER_STR;
		path += plugin->get_plugin_name();
		auto ret = ns::rx_directory_cache::instance().add_directory(path
			, rx_gate::instance().get_host()->get_system_storage(plugin->get_plugin_name()).value());
		if (!ret)
		{
			ret.register_error("Unable to add directory "s + path + ".");
			return ret.errors();
		}
	}

	BUILD_LOG_INFO("root_folder_builder", 900, "Root folder structure built.");
	return true;
}


// Class rx_internal::builders::basic_types_builder 


rx_result basic_types_builder::do_build ()
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_BASE_CLASSES_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir_result)
	{
		auto dir = dir_result;
		auto dtype = create_type<basic_types::data_type>(meta::type_creation_data{
			RX_CLASS_DATA_BASE_NAME
			, RX_CLASS_DATA_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		model::platform_types_manager::instance().get_data_types_repository().register_type(dtype);
		dir->add_item(dtype->get_item_ptr());

		//build base types, user extensible
		auto str = create_type<basic_types::struct_type>(meta::type_creation_data{
			RX_CLASS_STRUCT_BASE_NAME
			, RX_CLASS_STRUCT_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::struct_type>(dir, str);
		auto var = create_type<basic_types::variable_type>(meta::type_creation_data{
			RX_CLASS_VARIABLE_BASE_NAME
			, RX_CLASS_VARIABLE_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::variable_type>(dir, var);

		// build base types, code only extensible
		auto map = create_type<basic_types::mapper_type>(meta::type_creation_data{
			RX_CLASS_MAPPER_BASE_NAME
			, RX_CLASS_MAPPER_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::mapper_type>(dir, map);
		auto evnt = create_type<basic_types::event_type>(meta::type_creation_data{
			RX_CLASS_EVENT_BASE_NAME
			, RX_CLASS_EVENT_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::event_type>(dir, evnt);
		auto filt = create_type<basic_types::filter_type>(meta::type_creation_data{
			RX_CLASS_FILTER_BASE_NAME
			, RX_CLASS_FILTER_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::filter_type>(dir, filt);
		auto src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_CLASS_SOURCE_BASE_NAME
			, RX_CLASS_SOURCE_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::source_type>(dir, src);

		auto met = create_type<basic_types::method_type>(meta::type_creation_data{
			RX_CLASS_METHOD_BASE_NAME
			, RX_CLASS_METHOD_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::method_type>(dir, met);

		auto prog = create_type<basic_types::program_type>(meta::type_creation_data{
			RX_CLASS_PROGRAM_BASE_NAME
			, RX_CLASS_PROGRAM_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::program_type>(dir, prog);

		auto disp = create_type<basic_types::display_type>(meta::type_creation_data{
			RX_CLASS_DISPLAY_BASE_NAME
			, RX_CLASS_DISPLAY_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::display_type>(dir, disp);

		//build general data for runtime objects
		str = create_type<basic_types::struct_type>(meta::type_creation_data{
			RX_NS_OBJECT_DATA_NAME
			, RX_NS_OBJECT_DATA_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_object_data_struct_type(dir, str);

		// port related helper structures
		str = create_type<struct_type>(meta::type_creation_data{
			RX_PORT_STATUS_TYPE_NAME
			, RX_PORT_STATUS_TYPE_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		str->complex_data.register_simple_value_static("Binded", false, true, false);
		str->complex_data.register_simple_value_static("Assembled", false, true, false);
		str->complex_data.register_simple_value_static<int16_t>("Endpoints", 0, true, false);
		str->complex_data.register_simple_value_static<int64_t>("RxPackets", 0, false, false);
		str->complex_data.register_simple_value_static<int64_t>("TxPackets", 0, false, false);
		str->complex_data.register_simple_value_static<uint32_t>("Buffers", 0, true, false);
		str->complex_data.register_simple_value_static<int64_t>("DropedBuffers", 0, false, false);
		add_simple_type_to_configuration<struct_type>(dir, str, false);

		str = create_type<struct_type>(meta::type_creation_data{
			RX_PORT_OPTIONS_TYPE_NAME
			, RX_PORT_OPTIONS_TYPE_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		str->complex_data.register_const_value_static<uint32_t>("BuffBackCapacity", 0x100);
		str->complex_data.register_const_value_static<uint32_t>("BuffFrontCapacity", 0x10);
		str->complex_data.register_const_value_static<uint32_t>("BuffDiscardSize", 0x1000);
		add_simple_type_to_configuration<struct_type>(dir, str, false);

		//build base object type
		auto obj = create_type<object_type>(meta::object_type_creation_data{
			RX_CLASS_OBJECT_BASE_NAME
			, RX_CLASS_OBJECT_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		meta::object_types::relation_attribute rel_attr;
		build_basic_object_type<object_type>(dir, obj);
		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_USER_OBJECT_TYPE_NAME
			, RX_USER_OBJECT_TYPE_ID
			, RX_CLASS_OBJECT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);
		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_INTERNAL_OBJECT_TYPE_NAME
			, RX_INTERNAL_OBJECT_TYPE_ID
			, RX_CLASS_OBJECT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);

		//build derived object types
		auto app = create_type<application_type>(meta::object_type_creation_data{
			RX_CLASS_APPLICATION_BASE_NAME
			, RX_CLASS_APPLICATION_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_application_type<application_type>(dir, app);
		app = create_type<application_type>(meta::object_type_creation_data{
			RX_USER_APP_TYPE_NAME
			, RX_USER_APP_TYPE_ID
			, RX_CLASS_APPLICATION_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, app, false);
		app = create_type<application_type>(meta::object_type_creation_data{
			RX_INTERNAL_APP_TYPE_NAME
			, RX_INTERNAL_APP_TYPE_ID
			, RX_CLASS_APPLICATION_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, app, false);
		auto domain = create_type<domain_type>(meta::object_type_creation_data{
			RX_CLASS_DOMAIN_BASE_NAME
			, RX_CLASS_DOMAIN_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_domain_type<domain_type>(dir, domain);
		domain = create_type<domain_type>(meta::object_type_creation_data{
			RX_USER_DOMAIN_TYPE_NAME
			, RX_USER_DOMAIN_TYPE_ID
			, RX_CLASS_DOMAIN_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, domain, false);
		domain = create_type<domain_type>(meta::object_type_creation_data{
			RX_INTERNAL_DOMAIN_TYPE_NAME
			, RX_INTERNAL_DOMAIN_TYPE_ID
			, RX_CLASS_DOMAIN_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, domain, false);

		auto port = create_type<port_type>(meta::object_type_creation_data{
			RX_CLASS_PORT_BASE_NAME
			, RX_CLASS_PORT_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_port_type<port_type>(dir, port);
		// build relations
		relation_type_data def_data;
		def_data.abstract_type = true;
		def_data.sealed_type = false;
		def_data.symmetrical = false;
		def_data.hierarchical = false;
		def_data.dynamic = true;
		auto relation = create_type<relation_type>(meta::object_type_creation_data{
			RX_NS_RELATION_BASE_NAME
			, RX_NS_RELATION_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		relation->relation_data = def_data;
		model::platform_types_manager::instance().get_relations_repository().register_type(relation);
		dir->add_item(relation->get_item_ptr());

	}
	BUILD_LOG_INFO("basic_types_builder", 900, "Basic types built.");
	return true;
}

void basic_types_builder::build_object_data_struct_type(rx_directory_ptr dir, struct_type_ptr what)
{
	what->complex_data.register_const_value_static("PID", "");
	what->complex_data.register_const_value_static("Description", "");
	what->complex_data.register_simple_value_static("Note", "", false, true);
	what->complex_data.register_simple_value_static("LastScanTime", 0.0, true, false);
	what->complex_data.register_simple_value_static<uint32_t>("LoopCount", 0, true, false);
	what->complex_data.register_simple_value_static("MaxScanTime", 0.0, true, false);
	what->complex_data.register_simple_value_static<uint32_t>("ProcessQueues", 0, true, false);
	what->complex_data.register_simple_value_static("On", true, false, true);
	what->complex_data.register_simple_value_static("Test", false, false, true);
	what->complex_data.register_simple_value_static("Blocked", false, false, true);
	what->complex_data.register_const_value_static("Simulate", false);
	what->complex_data.register_const_value_static("SimActive", false);
	add_simple_type_to_configuration<struct_type>(dir, what, false);
}
template<class T>
void basic_types_builder::build_basic_object_type(rx_directory_ptr dir, rx_reference<T> what)
{
	what->complex_data.register_struct("Object", RX_NS_OBJECT_DATA_ID);
	add_type_to_configuration(dir, what, true);
}
template<class T>
void basic_types_builder::build_basic_domain_type(rx_directory_ptr dir, rx_reference<T> what)
{
	what->complex_data.register_const_value_static("CPU", -1);
	build_basic_object_type(dir, what);
}
template<class T>
void basic_types_builder::build_basic_application_type(rx_directory_ptr dir, rx_reference<T> what)
{
	build_basic_domain_type(dir, what);
}
template<class T>
void basic_types_builder::build_basic_port_type(rx_directory_ptr dir, rx_reference<T> what)
{
	what->complex_data.register_struct("Options", RX_PORT_OPTIONS_TYPE_ID);
	what->complex_data.register_struct("Status", RX_PORT_STATUS_TYPE_ID);
	build_basic_object_type(dir, what);
}
template<class T>
void basic_types_builder::build_basic_type(rx_directory_ptr dir, rx_reference<T> what)
{
	model::platform_types_manager::instance().get_simple_type_repository<T>().register_type(what);
	dir->add_item(what->get_item_ptr());
}
// Class rx_internal::builders::system_types_builder 


rx_result system_types_builder::do_build ()
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_SYSTEM_CLASSES_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir_result)
	{
		auto dir = dir_result;
		auto obj = create_type<object_type>(meta::object_type_creation_data{
			RX_NS_SYSTEM_TYPE_NAME
			, RX_NS_SYSTEM_TYPE_ID
			, RX_INTERNAL_OBJECT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		obj->complex_data.register_struct("Info", RX_NS_SYSTEM_INFO_TYPE_ID);
		add_type_to_configuration(dir, obj, false);
		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_NS_HOST_TYPE_NAME
			, RX_NS_HOST_TYPE_ID
			, RX_INTERNAL_OBJECT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		obj->complex_data.register_struct("Info", RX_NS_HOST_INFO_ID);
		add_type_to_configuration(dir, obj, false);
		auto str = create_type<basic_types::struct_type>(meta::type_creation_data{
			RX_NS_SYSTEM_INFO_TYPE_NAME
			, RX_NS_SYSTEM_INFO_TYPE_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_instance_info_struct_type(dir, str);
		add_simple_type_to_configuration(dir, str, false);
		str = create_type<basic_types::struct_type>(meta::type_creation_data{
			RX_NS_HOST_INFO_NAME
			, RX_NS_HOST_INFO_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_host_info_struct_type(dir, str);
		add_simple_type_to_configuration(dir, str, false);
		// system subtypes
		auto ev = create_type<event_type>(meta::object_type_creation_data{
			RX_NS_CHANGED_DATA_EVENT_NAME
			, RX_NS_CHANGED_DATA_EVENT_ID
			, RX_CLASS_EVENT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		ev->arguments = rx_node_id(RX_NS_CHANGED_DATA_ID);
		add_simple_type_to_configuration(dir, ev, false);
		// system application and domain types
		auto app = create_type<application_type>(meta::object_type_creation_data{
			RX_NS_SYSTEM_APP_TYPE_NAME
			, RX_NS_SYSTEM_APP_TYPE_ID
			, RX_INTERNAL_APP_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		app->complex_data.register_event(event_attribute("ItemChanged", RX_NS_CHANGED_DATA_EVENT_ID));
		add_type_to_configuration(dir, app, false);
		auto dom = create_type<domain_type>(meta::object_type_creation_data{
			RX_NS_SYSTEM_DOM_TYPE_NAME
			, RX_NS_SYSTEM_DOM_TYPE_ID
			, RX_INTERNAL_DOMAIN_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, dom, false);
		dom = create_type<domain_type>(meta::object_type_creation_data{
			RX_HOST_DOMAIN_TYPE_NAME
			, RX_HOST_DOMAIN_TYPE_ID
			, RX_INTERNAL_DOMAIN_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, dom, false);
		// unassigned application and domain types
		app = create_type<application_type>(meta::object_type_creation_data{
			RX_NS_SYSTEM_UNASS_APP_TYPE_NAME
			, RX_NS_SYSTEM_UNASS_APP_TYPE_ID
			, RX_INTERNAL_APP_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, app, false);
		app = create_type<application_type>(meta::object_type_creation_data{
			RX_HOST_APP_TYPE_NAME
			, RX_HOST_APP_TYPE_ID
			, RX_INTERNAL_APP_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, app, false);
		dom = create_type<domain_type>(meta::object_type_creation_data{
			RX_NS_SYSTEM_UNASS_TYPE_NAME
			, RX_NS_SYSTEM_UNASS_TYPE_ID
			, RX_INTERNAL_DOMAIN_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, dom, false);
		// data types used by system classes
		auto dtype = create_type<basic_types::data_type>(meta::type_creation_data{
			RX_NS_CHANGED_DATA_NAME
			, RX_NS_CHANGED_DATA_ID
			, RX_CLASS_DATA_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		dtype->complex_data.register_value_static("TS", rx_time::now());
		dtype->complex_data.register_value_static("Id", rx_node_id());
		dtype->complex_data.register_value_static("Path", "");
		model::platform_types_manager::instance().get_data_types_repository().register_type(dtype);
		dir->add_item(dtype->get_item_ptr());
		// other system object types
		
		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_NS_SERVER_RT_TYPE_NAME
			, RX_NS_SERVER_RT_TYPE_ID
			, RX_INTERNAL_OBJECT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		obj->complex_data.register_struct("Runtime", RX_RT_DATA_TYPE_ID);
		add_type_to_configuration(dir, obj, false);
		// pool type
		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_POOL_TYPE_NAME
			, RX_POOL_TYPE_ID
			, RX_INTERNAL_OBJECT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		obj->complex_data.register_struct("Pool", RX_POOL_DATA_TYPE_ID);
		add_type_to_configuration(dir, obj, false);

		// physical thread type
		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_PHYSICAL_THREAD_TYPE_NAME
			, RX_PHYSICAL_THREAD_TYPE_ID
			, RX_INTERNAL_OBJECT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		obj->complex_data.register_struct("Thread", RX_THREAD_DATA_TYPE_ID);
		obj->complex_data.register_struct("Pool", RX_POOL_DATA_TYPE_ID);
		add_type_to_configuration(dir, obj, false);

		// unassigned thread type
		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_UNASSIGNED_POOL_TYPE_NAME
			, RX_UNASSIGNED_POOL_TYPE_ID
			, RX_PHYSICAL_THREAD_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);// unassigned thread type
		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_META_POOL_TYPE_NAME
			, RX_META_POOL_TYPE_ID
			, RX_PHYSICAL_THREAD_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);

		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_LOG_TYPE_NAME
			, RX_LOG_TYPE_ID
			, RX_INTERNAL_OBJECT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);

		obj = create_type<object_type>(meta::object_type_creation_data{
			RX_IO_MANAGER_TYPE_NAME
			, RX_IO_MANAGER_TYPE_ID
			, RX_INTERNAL_OBJECT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);

		auto port = create_type<port_type>(meta::object_type_creation_data{
			RX_RX_JSON_TYPE_NAME
			, RX_RX_JSON_TYPE_ID
			, RX_APPLICATION_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_TCP_RX_PORT_TYPE_NAME
			, RX_TCP_RX_PORT_TYPE_ID
			, RX_TCP_SERVER_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_NS_HTTP_TYPE_NAME
			, RX_NS_HTTP_TYPE_ID
			, RX_APPLICATION_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_TCP_HTTP_PORT_TYPE_NAME
			, RX_TCP_HTTP_PORT_TYPE_ID
			, RX_TCP_SERVER_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);
	}
	BUILD_LOG_INFO("system_classes_builder", 900, "System types built.");
	return true;
}

void system_types_builder::build_instance_info_struct_type(rx_directory_ptr dir, struct_type_ptr what)
{
	what->complex_data.register_const_value_static("Instance", "");
	what->complex_data.register_const_value_static("Node", "");
	what->complex_data.register_const_value_static("StartTime", rx_gate::instance().get_started());
	what->complex_data.register_simple_value_static("Time", rx_time::now(), true, false);

	what->complex_data.register_const_value_static("PlatformVer", "");
	what->complex_data.register_const_value_static("LibraryVer", "");
	what->complex_data.register_const_value_static("TerminalVer", "");
	what->complex_data.register_const_value_static("HttpVer", "");
	what->complex_data.register_const_value_static("CompilerVer", "");
}
void system_types_builder::build_host_info_struct_type(rx_directory_ptr dir, struct_type_ptr what)
{
	what->complex_data.register_const_value_static("HostVer", "");
	what->complex_data.register_const_value_static("OSHostVer", "");
	what->complex_data.register_const_value_static("FirmwareVer", "");

	what->complex_data.register_const_value_static("OSVer", "");
	what->complex_data.register_const_value_static("CPU", "");
	what->complex_data.register_const_value_static("CPUCores", 0u);
	what->complex_data.register_const_value_static("LittleEndian", false);
	what->complex_data.register_const_value_static<uint64_t>("MemoryTotal", 0ull);
	what->complex_data.register_simple_value_static<uint64_t>("MemoryFree", 0ull, true, false);
	what->complex_data.register_const_value_static<uint32_t>("PageSize", 0u);
}
// Class rx_internal::builders::port_types_builder 


rx_result port_types_builder::do_build ()
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_PORT_CLASSES_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir_result)
	{
		auto dir = dir_result;
		// physical ports
		auto port = create_type<port_type>(meta::object_type_creation_data{
			RX_EXTERNAL_PORT_TYPE_NAME
			, RX_EXTERNAL_PORT_TYPE_ID
			, RX_CLASS_PORT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data.register_struct("Status", RX_PHY_PORT_STATUS_TYPE_ID);
		add_type_to_configuration(dir, port, true);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_SERIAL_PORT_TYPE_NAME
			, RX_SERIAL_PORT_TYPE_ID
			, RX_EXTERNAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data.register_struct("Options", RX_SERIAL_PORT_OPTIONS_TYPE_ID);
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_ETHERNET_PORT_TYPE_NAME
			, RX_ETHENERT_PORT_TYPE_ID
			, RX_EXTERNAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_UDP_PORT_TYPE_NAME
			, RX_UDP_PORT_TYPE_ID
			, RX_EXTERNAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data.register_struct("Bind", RX_IP_BIND_TYPE_ID);
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_TCP_SERVER_PORT_TYPE_NAME
			, RX_TCP_SERVER_PORT_TYPE_ID
			, RX_EXTERNAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data.register_struct("Bind", RX_IP_BIND_TYPE_ID);
		port->complex_data.register_struct("Timeouts", RX_SERVER_TIMEOUTS_TYPE_ID);
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_TCP_CLIENT_PORT_TYPE_NAME
			, RX_TCP_CLIENT_PORT_TYPE_ID
			, RX_EXTERNAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data.register_struct("Connect", RX_IP_BIND_TYPE_ID);
		port->complex_data.register_struct("Bind", RX_IP_BIND_TYPE_ID);
		port->complex_data.register_struct("Timeouts", RX_CLIENT_TIMEOUTS_TYPE_ID);
		add_type_to_configuration(dir, port, false);
		// transport ports
		port = create_type<port_type>(meta::object_type_creation_data{
			RX_TRANSPORT_PORT_TYPE_NAME
			, RX_TRANSPORT_PORT_TYPE_ID
			, RX_CLASS_PORT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		meta::object_types::relation_attribute rel_attr;
		rel_attr.name = "StackTop";
		rel_attr.relation_type = RX_NS_PORT_STACK_ID;
		rel_attr.target = RX_CLASS_PORT_BASE_ID;
		port->object_data.register_relation(rel_attr, port->complex_data);
		add_type_to_configuration(dir, port, true);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_ROUTED_TRANSPORT_PORT_TYPE_NAME
			, RX_ROUTED_TRANSPORT_PORT_TYPE_ID
			, RX_CLASS_PORT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		rel_attr.name = "StackTop";
		rel_attr.relation_type = RX_NS_PORT_STACK_ID;
		rel_attr.target = RX_CLASS_PORT_BASE_ID;
		port->object_data.register_relation(rel_attr, port->complex_data);
		add_type_to_configuration(dir, port, true);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_TELNET_TYPE_NAME
			, RX_TELNET_TYPE_ID
			, RX_TRANSPORT_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_OPCUA_TRANSPORT_PORT_TYPE_NAME
			, RX_OPCUA_TRANSPORT_PORT_TYPE_ID
			, RX_TRANSPORT_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);

		// special types of ports, routers, limiters, bridges....
		port = create_type<port_type>(meta::object_type_creation_data{
			RX_IP4_ROUTER_TYPE_NAME
			, RX_IP4_ROUTER_TYPE_ID
			, RX_ROUTED_TRANSPORT_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data.register_struct("Options", RX_ROUTER_PORT_OPTIONS_TYPE_ID);
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_BYTE_ROUTER_TYPE_NAME
			, RX_BYTE_ROUTER_TYPE_ID
			, RX_ROUTED_TRANSPORT_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data.register_struct("Options", RX_ROUTER_PORT_OPTIONS_TYPE_ID);
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_INITIATOR_TO_LISTENER_PORT_TYPE_NAME
			, RX_INITIATOR_TO_LISTENER_PORT_TYPE_ID
			, RX_ROUTED_TRANSPORT_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_LISTENER_TO_INITIATOR_PORT_TYPE_NAME
			, RX_LISTENER_TO_INITIATOR_PORT_TYPE_ID
			, RX_ROUTED_TRANSPORT_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_TRANS_LIMITER_TYPE_NAME
			, RX_TRANS_LIMITER_TYPE_ID
			, RX_TRANSPORT_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data.register_struct("Timeouts", RX_MASTER_TIMEOUTS_TYPE_ID);
		port->complex_data.register_struct("Status", RX_MASTER_PORT_STATUS_TYPE_ID);
		port->complex_data.register_struct("Options", RX_LIMITER_PORT_OPTIONS_TYPE_ID);
		add_type_to_configuration(dir, port, false);

		port = create_type<port_type>(meta::object_type_creation_data{
			RX_STXETX_TYPE_NAME
			, RX_STXETX_TYPE_ID
			, RX_TRANSPORT_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		//port->complex_data.register_struct("Timeouts", RX_MASTER_TIMEOUTS_TYPE_ID);
		//port->complex_data.register_struct("Status", RX_MASTER_PORT_STATUS_TYPE_ID);
		port->complex_data.register_struct("Options", RX_STXETX_PORT_OPTIONS_TYPE_ID);
		add_type_to_configuration(dir, port, false);

		// protocol ports
		port = create_type<port_type>(meta::object_type_creation_data{
			RX_APPLICATION_PORT_TYPE_NAME
			, RX_APPLICATION_PORT_TYPE_ID
			, RX_CLASS_PORT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		rel_attr.name = "StackTop";
		rel_attr.relation_type = RX_NS_PORT_STACK_ID;
		rel_attr.target = RX_CLASS_PORT_BASE_ID;
		port->object_data.register_relation(rel_attr, port->complex_data);
		add_type_to_configuration(dir, port, true);


	}
	BUILD_LOG_INFO("port_classes_builder", 900, "Port types built.");
	return true;
}


// Class rx_internal::builders::system_objects_builder 


rx_result system_objects_builder::do_build ()
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_OBJ_NAME "/" RX_NS_SYSTEM_OBJ_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir_result)
	{
		auto dir = dir_result;
		runtime_data::application_runtime_data app_instance_data;
		app_instance_data.meta_info.name = RX_NS_SYSTEM_APP_NAME;
		app_instance_data.meta_info.id = RX_NS_SYSTEM_APP_ID;
		app_instance_data.meta_info.parent = RX_NS_SYSTEM_APP_TYPE_ID;
		app_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		app_instance_data.meta_info.path = full_path;
		app_instance_data.instance_data.processor = 0;
		app_instance_data.instance_data.priority = rx_domain_priority::normal;
		auto result = add_object_to_configuration(dir, std::move(app_instance_data), data::runtime_values_data(), tl::type2type<application_type>());

		runtime_data::domain_runtime_data domain_instance_data;
		domain_instance_data.meta_info.name = RX_NS_SYSTEM_DOM_NAME;
		domain_instance_data.meta_info.id = RX_NS_SYSTEM_DOM_ID;
		domain_instance_data.meta_info.parent = RX_NS_SYSTEM_DOM_TYPE_ID;
		domain_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		domain_instance_data.meta_info.path = full_path;
		domain_instance_data.instance_data.processor = -1;
		domain_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		domain_instance_data.instance_data.priority = rx_domain_priority::normal;
		result = add_object_to_configuration(dir, std::move(domain_instance_data), data::runtime_values_data(), tl::type2type<domain_type>());

		runtime_data::object_runtime_data instance_data;
		instance_data = runtime_data::object_runtime_data();
		instance_data.meta_info.name = RX_NS_SYSTEM_NAME;
		instance_data.meta_info.id = RX_NS_SYSTEM_ID;
		instance_data.meta_info.parent = RX_NS_SYSTEM_TYPE_ID;
		instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		instance_data.meta_info.path = full_path;
		instance_data.instance_data.domain_ref = rx_node_id(RX_NS_SYSTEM_DOM_ID);
		result = add_object_to_configuration(dir, std::move(instance_data), data::runtime_values_data(), tl::type2type<object_type>());

		instance_data = runtime_data::object_runtime_data();
		instance_data.meta_info.name = RX_NS_SERVER_RT_NAME;
		instance_data.meta_info.id = RX_NS_SERVER_RT_ID;
		instance_data.meta_info.parent = RX_NS_SERVER_RT_TYPE_ID;
		instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		instance_data.meta_info.path = full_path;
		instance_data.instance_data.domain_ref = rx_node_id(RX_NS_SYSTEM_DOM_ID);
		result = add_object_to_configuration(dir, std::move(instance_data), data::runtime_values_data(), tl::type2type<object_type>());
		// we did the move so make another object
		instance_data = runtime_data::object_runtime_data();
		instance_data.meta_info.name = IO_POOL_NAME;
		instance_data.meta_info.id = IO_POOL_ID;
		instance_data.meta_info.parent = RX_POOL_TYPE_ID;
		instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		instance_data.meta_info.path = full_path;
		instance_data.instance_data.domain_ref = rx_node_id(RX_NS_SYSTEM_DOM_ID);
		result = add_object_to_configuration(dir, std::move(instance_data), data::runtime_values_data(), tl::type2type<object_type>());

		instance_data = runtime_data::object_runtime_data();
		instance_data.meta_info.name = META_POOL_NAME;
		instance_data.meta_info.id = META_POOL_ID;
		instance_data.meta_info.parent = RX_META_POOL_TYPE_ID;
		instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		instance_data.meta_info.path = full_path;
		instance_data.instance_data.domain_ref = rx_node_id(RX_NS_SYSTEM_DOM_ID);
		result = add_object_to_configuration(dir, std::move(instance_data), data::runtime_values_data(), tl::type2type<object_type>());


		instance_data = runtime_data::object_runtime_data();
		instance_data.meta_info.name = UNASSIGNED_POOL_NAME;
		instance_data.meta_info.id = UNASSIGNED_POOL_ID;
		instance_data.meta_info.parent = RX_UNASSIGNED_POOL_TYPE_ID;
		instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		instance_data.meta_info.path = full_path;
		instance_data.instance_data.domain_ref = rx_node_id(RX_NS_SYSTEM_DOM_ID);
		result = add_object_to_configuration(dir, std::move(instance_data), data::runtime_values_data(), tl::type2type<object_type>());

	}
	BUILD_LOG_INFO("system_objects_builder", 900, "System objects built.");
	return true;
}


// Class rx_internal::builders::support_types_builder 


rx_result support_types_builder::do_build ()
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_SUPPORT_CLASSES_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir_result)
	{
		auto dir = dir_result;
		// base mappers and sources
		auto map = create_type<basic_types::mapper_type>(meta::type_creation_data{
			RX_EXTERN_MAPPER_TYPE_NAME
			, RX_EXTERN_MAPPER_TYPE_ID
			, RX_CLASS_MAPPER_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		map->complex_data.register_const_value_static("Port", "");
		add_simple_type_to_configuration<mapper_type>(dir, map, true);

		map = create_type<basic_types::mapper_type>(meta::type_creation_data{
			RX_PARENT_MAPPER_TYPE_NAME
			, RX_PARENT_MAPPER_TYPE_ID
			, RX_CLASS_MAPPER_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_simple_type_to_configuration<mapper_type>(dir, map, true);

		map = create_type<basic_types::mapper_type>(meta::type_creation_data{
			RX_EXTERN_PARENT_MAPPER_TYPE_NAME
			, RX_EXTERN_PARENT_MAPPER_TYPE_ID
			, RX_PARENT_MAPPER_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		map->complex_data.register_const_value_static("Port", "");
		add_simple_type_to_configuration<mapper_type>(dir, map, true);

		map = create_type<basic_types::mapper_type>(meta::type_creation_data{
			RX_SYSTEM_MAPPER_TYPE_NAME
			, RX_SYSTEM_MAPPER_TYPE_ID
			, RX_CLASS_MAPPER_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_simple_type_to_configuration<mapper_type>(dir, map, true);

		auto src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_EXTERN_SOURCE_TYPE_NAME
			, RX_EXTERN_SOURCE_TYPE_ID
			, RX_CLASS_SOURCE_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		src->complex_data.register_const_value_static("Port", "");
		add_simple_type_to_configuration<source_type>(dir, src, true);
		src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_USER_SOURCE_TYPE_NAME
			, RX_USER_SOURCE_TYPE_ID
			, RX_CLASS_SOURCE_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_simple_type_to_configuration<source_type>(dir, src, true);

		src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_PARENT_SOURCE_TYPE_NAME
			, RX_PARENT_SOURCE_TYPE_ID
			, RX_CLASS_SOURCE_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		src->complex_data.register_const_value_static("Path", "");
		add_simple_type_to_configuration<source_type>(dir, src, false);

		src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_EXTERN_PARENT_SOURCE_TYPE_NAME
			, RX_EXTERN_PARENT_SOURCE_TYPE_ID
			, RX_PARENT_SOURCE_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		src->complex_data.register_const_value_static("Port", "");
		add_simple_type_to_configuration<source_type>(dir, src, false);

		src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_PLATFORM_SOURCE_TYPE_NAME
			, RX_PLATFORM_SOURCE_TYPE_ID
			, RX_CLASS_SOURCE_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		src->complex_data.register_simple_value_static("Path", "", false, true);
		add_simple_type_to_configuration<source_type>(dir, src, false);

		src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_SYSTEM_SOURCE_TYPE_NAME
			, RX_SYSTEM_SOURCE_TYPE_ID
			, RX_CLASS_SOURCE_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_simple_type_to_configuration<source_type>(dir, src, false);

		src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_REGISTER_SOURCE_TYPE_NAME
			, RX_REGISTER_SOURCE_TYPE_ID
			, RX_CLASS_SOURCE_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		src->complex_data.register_const_value_static("Persist", false, true);
		add_simple_type_to_configuration<source_type>(dir, src, false);

		// standard filters
		auto filter = create_type<basic_types::filter_type>(meta::type_creation_data{
			RX_LINEAR_SCALING_FILTER_TYPE_NAME
			, RX_LINEAR_SCALING_FILTER_TYPE_ID
			, RX_CLASS_FILTER_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		filter->complex_data.register_simple_value_static("HiEU", 10.0, false, true);
		filter->complex_data.register_simple_value_static("LowEU", 0.0, false, true);
		filter->complex_data.register_simple_value_static("HiRaw", 100.0, false, true);
		filter->complex_data.register_simple_value_static("LowRaw", 0.0, false, true);
		add_simple_type_to_configuration<filter_type>(dir, filter, false);

		filter = create_type<basic_types::filter_type>(meta::type_creation_data{
			RX_LIMIT_FILTER_TYPE_NAME
			, RX_LIMIT_FILTER_TYPE_ID
			, RX_CLASS_FILTER_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		filter->complex_data.register_simple_value_static("HiInput", 1000.0, false, true);
		filter->complex_data.register_simple_value_static("LowInput", 0.0, false, true);
		filter->complex_data.register_simple_value_static("HiOutput", 1000.0, false, true);
		filter->complex_data.register_simple_value_static("LowOutput", 0.0, false, true);
		add_simple_type_to_configuration<filter_type>(dir, filter, false);

		filter = create_type<basic_types::filter_type>(meta::type_creation_data{
			RX_CUTOFF_FILTER_TYPE_NAME
			, RX_CUTOFF_FILTER_TYPE_ID
			, RX_CLASS_FILTER_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		filter->complex_data.register_simple_value_static("InCutoffValue", 0.0, false, true);
		filter->complex_data.register_simple_value_static("OutCutoffValue", 0.0, false, true);
		add_simple_type_to_configuration<filter_type>(dir, filter, true);

		filter = create_type<basic_types::filter_type>(meta::type_creation_data{
			RX_LO_CUTOFF_FILTER_TYPE_NAME
			, RX_LO_CUTOFF_FILTER_TYPE_ID
			, RX_CUTOFF_FILTER_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_simple_type_to_configuration<filter_type>(dir, filter, false);

		auto variable = create_type<basic_types::variable_type>(meta::type_creation_data{
			RX_REGISTER_VARIABLE_TYPE_NAME
			, RX_REGISTER_VARIABLE_TYPE_ID
			, RX_CLASS_VARIABLE_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_simple_type_to_configuration<variable_type>(dir, variable, false);

		filter = create_type<basic_types::filter_type>(meta::type_creation_data{
			RX_HI_CUTOFF_FILTER_TYPE_NAME
			, RX_HI_CUTOFF_FILTER_TYPE_ID
			, RX_CUTOFF_FILTER_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_simple_type_to_configuration<filter_type>(dir, filter, false);


		auto what = create_type<struct_type>(meta::type_creation_data{
			RX_PHY_PORT_STATUS_TYPE_NAME
			, RX_PHY_PORT_STATUS_TYPE_ID
			, RX_PORT_STATUS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_simple_value_static<int64_t>("RxBytes", 0, false, false);
		what->complex_data.register_simple_value_static<int64_t>("TxBytes", 0, false, false);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_CLIENT_PORT_STATUS_TYPE_NAME
			, RX_CLIENT_PORT_STATUS_TYPE_ID
			, RX_PORT_STATUS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_simple_value_static<bool>("Connected", false, false, false);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_MASTER_PORT_STATUS_TYPE_NAME
			, RX_MASTER_PORT_STATUS_TYPE_ID
			, RX_PORT_STATUS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_simple_value_static<bool>("Online", false, true, false);
		what->complex_data.register_simple_value_static<int64_t>("Reads", 0, false, false);
		what->complex_data.register_simple_value_static<int64_t>("Writes", 0, false, false);
		what->complex_data.register_simple_value_static<int64_t>("FailedReads", 0, false, false);
		what->complex_data.register_simple_value_static<int64_t>("FailedWrites", 0, false, false);
		what->complex_data.register_simple_value_static<float>("LastRead", 0, true, false);
		what->complex_data.register_simple_value_static<float>("MaxRead", 0, false, false);
		what->complex_data.register_simple_value_static<float>("LastWrite", 0, true, false);
		what->complex_data.register_simple_value_static<float>("MaxWrite", 0, false, false);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_IP_BIND_TYPE_NAME
			, RX_IP_BIND_TYPE_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_const_value_static("IPAddress", "");
		what->complex_data.register_const_value_static<uint16_t>("IPPort", 0);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_TIMEOUTS_TYPE_NAME
			, RX_TIMEOUTS_TYPE_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_SERVER_TIMEOUTS_TYPE_NAME
			, RX_SERVER_TIMEOUTS_TYPE_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_simple_value_static<uint32_t>("ReceiveTimeout", 10000, false, true);
		what->complex_data.register_simple_value_static<uint32_t>("SendTimeout", 1000, false, true);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_CLIENT_TIMEOUTS_TYPE_NAME
			, RX_CLIENT_TIMEOUTS_TYPE_ID
			, RX_TIMEOUTS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_simple_value_static<uint32_t>("ReceiveTimeout", 10000, false, true);
		what->complex_data.register_simple_value_static<uint32_t>("SendTimeout", 1000, false, true);
		what->complex_data.register_simple_value_static<uint32_t>("ConnectTimeout", 2000, false, true);
		what->complex_data.register_simple_value_static<uint32_t>("ReconnectTimeout", 5000, false, true);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_MASTER_TIMEOUTS_TYPE_NAME
			, RX_MASTER_TIMEOUTS_TYPE_ID
			, RX_TIMEOUTS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_simple_value_static<uint32_t>("ReadTimeout", 200, false, true);
		what->complex_data.register_simple_value_static<uint32_t>("WriteTimeout", 500, false, true);
		add_simple_type_to_configuration<struct_type>(dir, what, false);


		what = create_type<struct_type>(meta::type_creation_data{
			RX_ROUTER_PORT_OPTIONS_TYPE_NAME
			, RX_ROUTER_PORT_OPTIONS_TYPE_ID
			, RX_PORT_OPTIONS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_const_value_static("Initiator", false);
		what->complex_data.register_const_value_static("Listener", false);
		what->complex_data.register_simple_value_static<uint32_t>("SessionTimeout", 2000, false, true);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_LIMITER_PORT_OPTIONS_TYPE_NAME
			, RX_LIMITER_PORT_OPTIONS_TYPE_ID
			, RX_PORT_OPTIONS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_simple_value_static("Limit", 1, false, true);
		what->complex_data.register_simple_value_static("IgnoreZeros", false, false, true);
		what->complex_data.register_simple_value_static("UsePacketId", true, false, true);
		what->complex_data.register_simple_value_static("QueueSize", 0, true, false);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_BRIDGE_PORT_OPTIONS_TYPE_NAME
			, RX_BRIDGE_PORT_OPTIONS_TYPE_ID
			, RX_PORT_OPTIONS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_simple_type_to_configuration<struct_type>(dir, what, false);


		what = create_type<struct_type>(meta::type_creation_data{
			RX_STXETX_PORT_OPTIONS_TYPE_NAME
			, RX_STXETX_PORT_OPTIONS_TYPE_ID
			, RX_PORT_OPTIONS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
				});
		what->complex_data.register_const_value_static("MaxPacketSize", 0x1000u);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_SERIAL_PORT_OPTIONS_TYPE_NAME
			, RX_SERIAL_PORT_OPTIONS_TYPE_ID
			, RX_PORT_OPTIONS_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_const_value_static("Port", "");
		what->complex_data.register_const_value_static<uint32_t>("BaudRate", 19200);
		what->complex_data.register_const_value_static<uint8_t>("DataBits", 8);
		what->complex_data.register_const_value_static<uint8_t>("StopBits", 0);
		what->complex_data.register_const_value_static<uint8_t>("Parity", 0);
		what->complex_data.register_const_value_static<bool>("Handshake", false);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_POOL_DATA_TYPE_NAME
			, RX_POOL_DATA_TYPE_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_const_value_static("Threads", (uint16_t)0);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_THREAD_DATA_TYPE_NAME
			, RX_THREAD_DATA_TYPE_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_const_value_static("ThreadId", (uint32_t)0);
		what->complex_data.register_simple_value_static("Queue", (uint16_t)0, true, false);
		what->complex_data.register_simple_value_static("MaxQueue", (uint16_t)0, true, false);
		add_simple_type_to_configuration<struct_type>(dir, what, false);

		what = create_type<struct_type>(meta::type_creation_data{
			RX_RT_DATA_TYPE_NAME
			, RX_RT_DATA_TYPE_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		what->complex_data.register_const_value_static<uint16_t>("IOThreads", 0);
		what->complex_data.register_const_value_static<uint16_t>("Workers", 0);
		what->complex_data.register_const_value_static<bool>("CalcTimer", false);
		add_simple_type_to_configuration<struct_type>(dir, what, false);
	}
	return true;
}


// Class rx_internal::builders::relation_types_builder 


rx_result relation_types_builder::do_build ()
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_RELATIONS_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir_result)
	{
		auto dir = dir_result;
		relation_type_data def_data;
		def_data.abstract_type = false;
		def_data.sealed_type = false;
		def_data.symmetrical = false;
		def_data.hierarchical = true;
		def_data.dynamic = false;
		def_data.inverse_name = RX_MACRO_SYMBOL_STR "name" RX_MACRO_SYMBOL_STR;

		auto relation = create_type<relation_type>(meta::object_type_creation_data{
			RX_NS_PORT_STACK_NAME
			, RX_NS_PORT_STACK_ID
			, RX_NS_RELATION_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		relation->relation_data = def_data;
		add_relation_type_to_configuration(dir, relation);

		relation = create_type<relation_type>(meta::object_type_creation_data{
			RX_NS_PORT_REF_NAME
			, RX_NS_PORT_REF_ID
			, RX_NS_RELATION_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		relation->relation_data = def_data;
		relation->relation_data.dynamic = true;
		add_relation_type_to_configuration(dir, relation);

		def_data.sealed_type = true;
		relation = create_type<relation_type>(meta::object_type_creation_data{
			RX_NS_APPLICATION_RELATION_NAME
			, RX_NS_APPLICATION_RELATION_ID
			, RX_NS_RELATION_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		relation->relation_data = def_data;
		add_relation_type_to_configuration(dir, relation);

		def_data.sealed_type = true;
		relation = create_type<relation_type>(meta::object_type_creation_data{
			RX_NS_PORT_APPLICATION_RELATION_NAME
			, RX_NS_PORT_APPLICATION_RELATION_ID
			, RX_NS_RELATION_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		relation->relation_data = def_data;
		add_relation_type_to_configuration(dir, relation);

		relation = create_type<relation_type>(meta::object_type_creation_data{
			RX_NS_DOMAIN_RELATION_NAME
			, RX_NS_DOMAIN_RELATION_ID
			, RX_NS_RELATION_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		relation->relation_data = def_data;
		add_relation_type_to_configuration(dir, relation);
	}
	return true;
}


// Class rx_internal::builders::simulation_types_builder 


rx_result simulation_types_builder::do_build ()
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_SIMULATION_CLASSES_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir_result)
	{
		auto dir = dir_result;
		// base mappers and sources
		auto src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_SIMULATION_SOURCE_TYPE_NAME
			, RX_SIMULATION_SOURCE_TYPE_ID
			, RX_CLASS_SOURCE_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		src->complex_data.register_simple_value_static<uint32_t>("Period", 50, false, true);
		add_simple_type_to_configuration<source_type>(dir, src, true);
		src = create_type<basic_types::source_type>(meta::type_creation_data{
			RX_RAMP_SIMULATION_SOURCE_TYPE_NAME
			, RX_RAMP_SIMULATION_SOURCE_TYPE_ID
			, RX_SIMULATION_SOURCE_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		src->complex_data.register_simple_value_static<double>("Amplitude", 100, false, true);
		src->complex_data.register_simple_value_static<double>("Increment", 1, false, true);
		src->complex_data.register_simple_value_static<bool>("Double", false, false, true);
		add_simple_type_to_configuration<source_type>(dir, src, true);

	}
	return true;
}


// Class rx_internal::builders::system_ports_builder 


rx_result system_ports_builder::do_build ()
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_OBJ_NAME "/" RX_NS_PORT_OBJ_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir_result)
	{
		auto dir = dir_result;
		runtime_data::port_runtime_data port_instance_data;
		port_instance_data.meta_info.name = RX_NS_SYSTEM_TCP_NAME;
		port_instance_data.meta_info.id = RX_NS_SYSTEM_TCP_ID;
		port_instance_data.meta_info.parent = RX_TCP_RX_PORT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("Bind.IPPort", 0);
		port_instance_data.overrides.add_value_static("Timeouts.ReceiveTimeout", 16000);
		auto result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		port_instance_data.meta_info.name = RX_NS_SYSTEM_OPCUABIN_NAME;
		port_instance_data.meta_info.id = RX_NS_SYSTEM_OPCUABIN_ID;
		port_instance_data.meta_info.parent = RX_OPCUA_TRANSPORT_PORT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("StackTop", "./" RX_NS_SYSTEM_TCP_NAME);
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		port_instance_data.meta_info.name = RX_NS_SYSTEM_RXJSON_NAME;
		port_instance_data.meta_info.id = RX_NS_SYSTEM_RXJSON_ID;
		port_instance_data.meta_info.parent = RX_RX_JSON_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("StackTop", "./" RX_NS_SYSTEM_OPCUABIN_NAME);
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		port_instance_data.meta_info.name = RX_NS_HTTP_TCP_NAME;
		port_instance_data.meta_info.id = RX_NS_HTTP_TCP_ID;
		port_instance_data.meta_info.parent = RX_TCP_HTTP_PORT_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("Bind.IPPort", 0);
		port_instance_data.overrides.add_value_static("Timeouts.ReceiveTimeout", 300000);
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());

		port_instance_data.meta_info.name = RX_NS_HTTP_NAME;
		port_instance_data.meta_info.id = RX_NS_HTTP_ID;
		port_instance_data.meta_info.parent = RX_NS_HTTP_TYPE_ID;
		port_instance_data.meta_info.attributes = namespace_item_attributes::namespace_item_internal_access;
		port_instance_data.meta_info.path = full_path;
		port_instance_data.instance_data.app_ref = rx_node_id(RX_NS_SYSTEM_APP_ID);
		port_instance_data.overrides.add_value_static("StackTop", "./" RX_NS_HTTP_TCP_NAME);
		result = add_object_to_configuration(dir, std::move(port_instance_data), data::runtime_values_data(), tl::type2type<port_type>());


	}
	BUILD_LOG_INFO("system_ports_builder", 900, "System ports built.");
	return true;
}


// Class rx_internal::builders::terminal_commands_builder 


rx_result terminal_commands_builder::do_build ()
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_TERMINAL_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir_result = ns::rx_directory_cache::instance().get_directory(full_path);
	if (dir_result)
	{
		auto dir = dir_result;

		auto dtype = create_type<basic_types::data_type>(meta::type_creation_data{
			RX_CLASS_CONSOLE_IN_NAME
			, RX_CLASS_CONSOLE_IN_ID
			, RX_CLASS_DATA_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		dtype->complex_data.register_value_static("In", "");
		add_data_type_to_configuration(dir, dtype);

		dtype = create_type<basic_types::data_type>(meta::type_creation_data{
			RX_CLASS_CONSOLE_OUT_NAME
			, RX_CLASS_CONSOLE_OUT_ID
			, RX_CLASS_DATA_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		dtype->complex_data.register_value_static("Out", "");
		dtype->complex_data.register_value_static("Err", "");
		dtype->complex_data.register_value_static("Result", false);
		add_data_type_to_configuration(dir, dtype);

		auto met = create_type<basic_types::method_type>(meta::type_creation_data{
				RX_CLASS_CONSOLE_COMMAND_BASE_NAME
				, RX_CLASS_CONSOLE_COMMAND_BASE_ID
				, RX_CLASS_METHOD_BASE_ID
				, namespace_item_attributes::namespace_item_internal_access
				, full_path
			});
		met->complex_data.register_const_value_static("TermName", ""s);
		met->inputs = rx_item_reference(RX_CLASS_CONSOLE_IN_NAME);
		met->outputs = rx_item_reference(RX_CLASS_CONSOLE_OUT_NAME);
		add_simple_type_to_configuration<basic_types::method_type>(dir, met, true);


		auto obj = create_type<object_type>(meta::object_type_creation_data{
			RX_COMMANDS_MANAGER_TYPE_NAME
			, RX_COMMANDS_MANAGER_TYPE_ID
			, RX_INTERNAL_OBJECT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});

		auto internal_commands = terminal::commands::server_command_manager::instance()->get_internal_commands();
		for (auto& one : internal_commands)
		{
			met = create_type<basic_types::method_type>(meta::type_creation_data{
				one->get_console_name() + "Cmd"
				, one->get_console_name().c_str()
				, RX_CLASS_CONSOLE_COMMAND_BASE_ID
				, namespace_item_attributes::namespace_item_internal_access
				, full_path
				});
			obj->complex_data.overrides.add_value_static("TermName", one->get_console_name());
			obj->object_data.register_method(def_blocks::method_attribute(one->get_console_name(), met->meta_info.name), obj->complex_data);
			add_simple_type_to_configuration<basic_types::method_type>(dir, met, false);
		}

		add_type_to_configuration(dir, obj, false);

		auto port = create_type<port_type>(meta::object_type_creation_data{
			RX_VT00_TYPE_NAME
			, RX_VT00_TYPE_ID
			, RX_APPLICATION_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);

	}
	BUILD_LOG_INFO("terminal_commands_builder", 900, "Terminal commands built.");
	return true;
}


} // namespace builders
} // namespace rx_internal

