

/****************************************************************************
*
*  sys_internal\rx_internal_builders.cpp
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


// rx_internal_builders
#include "sys_internal/rx_internal_builders.h"

#include "rx_storage_build.h"
#include "system/server/rx_server.h"
#include "terminal/rx_commands.h"
#include "system/server/rx_inf.h"
#include "testing/rx_test.h"
#include "api/rx_meta_api.h"
#include "rx_configuration.h"
#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "sys_internal/rx_plugin_manager.h"
using namespace rx_platform::meta::object_types;


namespace sys_internal {

namespace builders {

template<class T>
void add_type_to_configuration(rx_directory_ptr dir, rx_reference<T> what, bool abstract_type)
{
	what->meta_info().resolve();
	what->complex_data().set_abstract(abstract_type);
	model::platform_types_manager::instance().internal_get_type_cache<T>().register_type(what);
	dir->add_item(what->get_item_ptr());
}

template<class T>
void add_simple_type_to_configuration(rx_directory_ptr dir, rx_reference<T> what, bool abstract_type)
{
	what->meta_info().resolve();
	what->complex_data().set_abstract(abstract_type);
	model::platform_types_manager::instance().internal_get_simple_type_cache<T>().register_type(what);
	dir->add_item(what->get_item_ptr());
}


template<class T>
rx_result add_object_to_configuration(rx_directory_ptr dir, meta_data meta, typename T::instance_data_t data, tl::type2type<T>)
{
	meta.resolve();
	auto create_result = model::algorithms::runtime_model_algorithm<T>::create_runtime_sync(
		meta, nullptr, std::move(data), dir, rx_object_ptr::null_ptr);
	if (create_result)
	{
		auto rx_type_item = create_result.value()->get_item_ptr();
		BUILD_LOG_TRACE("code_objects", 100, ("Created "s + rx_item_type_name(T::RType::type_id) + " "s + rx_type_item->get_name()).c_str());
		return true;
	}
	else
	{
		create_result.register_error("Error creating "s + rx_item_type_name(T::RType::type_id) + " " + meta.get_name());
		for(const auto& one : create_result.errors())
			BUILD_LOG_ERROR("code_objects", 900, one.c_str());
		return create_result.errors();
	}
}


// Class sys_internal::builders::rx_platform_builder 

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



rx_result_with<rx_directory_ptr> rx_platform_builder::buid_platform (hosting::rx_platform_host* host, namespace_data_t& data, const meta::meta_configuration_data_t& meta_data)
{
	register_system_constructors();

	auto sys_builders = get_system_builders(data, meta_data, host);
	auto user_builders = get_user_builders(data, host);
	auto test_builders = get_test_builders(data, host);
	auto other_builders = get_other_builders(data, host);

	rx_result errors = true;

	auto root = rx_create_reference<sys_internal::internal_ns::platform_root>();
	root->add_sub_directory(rx_create_reference<unassigned_directory>());

	for (auto& one : sys_builders)
	{
		auto result = one->do_build(root);
		if (!result)
		{
			BUILD_LOG_ERROR("rx_platform_builder", 900, "Error building platform system!");
			errors.register_errors(result.errors());
		}
	}
	BUILD_LOG_INFO("rx_platform_builder", 900, "Building unassigned system!");
	errors = buid_unassigned(root, host, data);
	if(errors)
		BUILD_LOG_INFO("rx_platform_builder", 900, "Unassigned system built!");
	// system is critical so an error in building system is fatal
	else
		return errors.errors();

	BUILD_LOG_INFO("rx_platform_builder", 900, "Building plugins...");
	for (auto one : sys_internal::plugins::plugins_manager::instance().get_plugins())
	{
		BUILD_LOG_INFO("rx_platform_builder", 900, ("Found plugin "s + one->get_plugin_name() + " [" + one->get_plugin_info() + "]..."s));
		auto storage_ptr = one->get_storage();
		if (storage_ptr)
		{
			storage::configuration_storage_builder builder(storage_ptr);
			builder.do_build(root);
		}
		BUILD_LOG_INFO("rx_platform_builder", 900, ("Plugin "s + one->get_plugin_name() + " built."s));
	}
	BUILD_LOG_INFO("rx_platform_builder", 900, "All plugins built.");

	for (auto& one : user_builders)
	{
		auto result = one->do_build(root);
		if (!result)
		{
			BUILD_LOG_ERROR("rx_platform_builder", 900, "There were error while building platform user configuration!");
		}
	}
	for (auto& one : test_builders)
	{
		auto result = one->do_build(root);
		if (!result)
		{
			BUILD_LOG_WARNING("rx_platform_builder", 900, "There were error while building platform test configuration!");
		}
	}
	for (auto& one : other_builders)
	{
		auto result = one->do_build(root);
		if (!result)
		{
			BUILD_LOG_WARNING("rx_platform_builder", 900, "Error building platform additional configuration!");
		}
	}

	return rx_result_with<rx_directory_ptr>(root);
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_system_builders (namespace_data_t& data, const meta::meta_configuration_data_t& meta_data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// create system folder structure
	builders.emplace_back(std::make_unique<root_folder_builder>());
	if (meta_data.build_system_from_code)
	{
		// types builders
		builders.emplace_back(std::make_unique<basic_types_builder>());
		//builders.emplace_back(std::make_unique<support_types_builder>());
		builders.emplace_back(std::make_unique<system_types_builder>());
		builders.emplace_back(std::make_unique<port_types_builder>());
		//// objects builders
		builders.emplace_back(std::make_unique<system_objects_builder>());
	}
	else
	{
		// storage builder
		builders.emplace_back(std::make_unique<storage::configuration_storage_builder>(host->get_system_storage()));
	}
	return builders;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_user_builders (namespace_data_t& data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// storage builder
	builders.emplace_back(std::make_unique<storage::configuration_storage_builder>(host->get_user_storage()));
	return builders;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_test_builders (namespace_data_t& data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// storage builder
	builders.emplace_back(std::make_unique<storage::configuration_storage_builder>(host->get_test_storage()));
	return builders;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_other_builders (namespace_data_t& data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	return builders;
}

void rx_platform_builder::register_system_constructors ()
{
	// system app
	model::platform_types_manager::instance().internal_get_type_cache<application_type>().register_constructor(
		RX_NS_SYSTEM_APP_TYPE_ID, [] { return rx_gate::instance().get_manager().get_system_app(); } );
	// system domain
	model::platform_types_manager::instance().internal_get_type_cache<domain_type>().register_constructor(
		RX_NS_SYSTEM_DOM_TYPE_ID, [] { return rx_gate::instance().get_manager().get_system_domain(); });
	// unassigned app
	model::platform_types_manager::instance().internal_get_type_cache<application_type>().register_constructor(
		RX_NS_SYSTEM_UNASS_APP_TYPE_ID, [] { return rx_gate::instance().get_manager().get_unassigned_app(); });
	// unassigned domain
	model::platform_types_manager::instance().internal_get_type_cache<domain_type>().register_constructor(
		RX_NS_SYSTEM_UNASS_TYPE_ID, [] { return rx_gate::instance().get_manager().get_unassigned_domain(); });
}

rx_result rx_platform_builder::buid_unassigned (platform_root::smart_ptr root, hosting::rx_platform_host* host, namespace_data_t& data)
{
	string_type path(RX_NS_UNASSIGNED_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir = root->get_sub_directory(path);
	if (dir)
	{
		runtime::objects::domain_instance_data instance_data;
		instance_data.processor = 1;
		instance_data.app_id = RX_NS_SYSTEM_UNASS_APP_ID;
		meta_data meta(RX_NS_SYSTEM_UNASS_NAME, RX_NS_SYSTEM_UNASS_ID, RX_NS_SYSTEM_UNASS_TYPE_ID, namespace_item_attributes::namespace_item_internal_access, full_path);
		add_object_to_configuration(dir, std::move(meta), std::move(instance_data), tl::type2type<domain_type>());
		
		runtime::objects::application_instance_data app_instance_data;
		app_instance_data.processor = 1;
		meta_data app_meta(RX_NS_SYSTEM_UNASS_APP_NAME, RX_NS_SYSTEM_UNASS_APP_ID, RX_NS_SYSTEM_UNASS_APP_TYPE_ID, namespace_item_attributes::namespace_item_internal_access, full_path);
		add_object_to_configuration(dir, std::move(app_meta), std::move(app_instance_data), tl::type2type<application_type>());
	}
	return true;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_plugin_builders (namespace_data_t& data, hosting::rx_platform_host* host)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	return builders;
}


// Class sys_internal::builders::root_folder_builder 


rx_result root_folder_builder::do_build (rx_directory_ptr root)
{
	root->add_sub_directory(rx_create_reference<world_directory>());

	auto sys_dir = rx_create_reference<system_directory>();
	sys_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_BIN_NAME));

	auto classes_dir = rx_create_reference<internal_directory>(RX_NS_CLASSES_NAME);
	classes_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_BASE_CLASSES_NAME));
	classes_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_SYSTEM_CLASSES_NAME));
	classes_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_PORT_CLASSES_NAME));
	classes_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_SUPPORT_CLASSES_NAME));
	sys_dir->add_sub_directory(classes_dir);

	auto objects_dir = rx_create_reference<internal_directory>(RX_NS_OBJ_NAME);
	objects_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_SYSTEM_OBJ_NAME));
	objects_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_PORT_OBJ_NAME));
	sys_dir->add_sub_directory(objects_dir);

	auto plugins_dir = rx_create_reference<internal_directory>(RX_NS_PLUGINS_NAME);
	plugins_dir->add_sub_directory(rx_create_reference<host_directory>());
	auto& plugins = plugins::plugins_manager::instance().get_plugins();
	for (auto& plugin : plugins)
	{
		plugins_dir->add_sub_directory(rx_create_reference<plugin_directory>(plugin));
	}

	sys_dir->add_sub_directory(plugins_dir);

	root->add_sub_directory(sys_dir);
	BUILD_LOG_INFO("root_folder_builder", 900, "Root folder structure built.");
	return true;
}


// Class sys_internal::builders::basic_types_builder 


rx_result basic_types_builder::do_build (rx_directory_ptr root)
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_BASE_CLASSES_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir = root->get_sub_directory(path);
	if (dir)
	{		

		//build base types, user extensible
		auto str = rx_create_reference<basic_types::struct_type>(meta::type_creation_data{
			RX_CLASS_STRUCT_BASE_NAME
			, RX_CLASS_STRUCT_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::struct_type>(dir, str);
		auto var = rx_create_reference<basic_types::variable_type>(meta::type_creation_data{
			RX_CLASS_VARIABLE_BASE_NAME
			, RX_CLASS_VARIABLE_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::variable_type>(dir, var);

		// build base types, code only extensible
		auto map = rx_create_reference<basic_types::mapper_type>(meta::type_creation_data{
			RX_CLASS_MAPPER_BASE_NAME
			, RX_CLASS_MAPPER_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::mapper_type>(dir, map);
		auto evnt = rx_create_reference<basic_types::event_type>(meta::type_creation_data{
			RX_CLASS_EVENT_BASE_NAME
			, RX_CLASS_EVENT_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::event_type>(dir, evnt);
		auto filt = rx_create_reference<basic_types::filter_type>(meta::type_creation_data{
			RX_CLASS_FILTER_BASE_NAME
			, RX_CLASS_FILTER_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::filter_type>(dir, filt);
		auto src = rx_create_reference<basic_types::source_type>(meta::type_creation_data{
			RX_CLASS_SOURCE_BASE_NAME
			, RX_CLASS_SOURCE_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_type<basic_types::source_type>(dir, src);

		//build general data for runtime objects
		str = rx_create_reference<basic_types::struct_type>(meta::type_creation_data{
			RX_NS_OBJECT_DATA_NAME
			, RX_NS_OBJECT_DATA_ID
			, RX_CLASS_STRUCT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_object_data_struct_type(dir, str);

		//build base object type
		auto obj = rx_create_reference<object_type>(meta::object_type_creation_data{
			RX_CLASS_OBJECT_BASE_NAME
			, RX_CLASS_OBJECT_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_object_type<object_type>(dir, obj);

		//build derived object types
		auto app = rx_create_reference<application_type>(meta::object_type_creation_data{
			RX_CLASS_APPLICATION_BASE_NAME
			, RX_CLASS_APPLICATION_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_application_type<application_type>(dir, app);
		auto domain = rx_create_reference<domain_type>(meta::object_type_creation_data{
			RX_CLASS_DOMAIN_BASE_NAME
			, RX_CLASS_DOMAIN_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_domain_type<domain_type>(dir, domain);
		auto port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_CLASS_PORT_BASE_NAME
			, RX_CLASS_PORT_BASE_ID
			, rx_node_id::null_id
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		build_basic_port_type<port_type>(dir, port);

	}
	BUILD_LOG_INFO("basic_types_builder", 900, "Basic types built.");
	return true;
}

void basic_types_builder::build_object_data_struct_type(rx_directory_ptr dir, struct_type_ptr what)
{
	what->complex_data().register_const_value_static("Description", ""s);
	what->complex_data().register_simple_value_static("Note", false, ""s);
	what->complex_data().register_simple_value_static("LastScanTime", true, 0.0);
	add_simple_type_to_configuration<struct_type>(dir, what, false);
}
template<class T>
void basic_types_builder::build_basic_object_type(rx_directory_ptr dir, rx_reference<T> what)
{
	what->complex_data().register_struct("Object", RX_NS_OBJECT_DATA_ID);
	add_type_to_configuration(dir, what, true);
}
template<class T>
void basic_types_builder::build_basic_domain_type(rx_directory_ptr dir, rx_reference<T> what)
{
	what->complex_data().register_simple_value_static("CPU", true, -1);
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
	build_basic_object_type(dir, what);
}
template<class T>
void basic_types_builder::build_basic_type(rx_directory_ptr dir, rx_reference<T> what)
{
	what->meta_info().resolve();
	model::platform_types_manager::instance().internal_get_simple_type_cache<T>().register_type(what);
	dir->add_item(what->get_item_ptr());
}
// Class sys_internal::builders::system_types_builder 


rx_result system_types_builder::do_build (rx_directory_ptr root)
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_SYSTEM_CLASSES_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir = root->get_sub_directory(path);
	if (dir)
	{
		// system application and domain types
		auto app = rx_create_reference<application_type>(meta::object_type_creation_data{
			RX_NS_SYSTEM_APP_TYPE_NAME
			, RX_NS_SYSTEM_APP_TYPE_ID
			, RX_CLASS_APPLICATION_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, app, false);
		auto dom = rx_create_reference<domain_type>(meta::object_type_creation_data{
			RX_NS_SYSTEM_DOM_TYPE_NAME
			, RX_NS_SYSTEM_DOM_TYPE_ID
			, RX_CLASS_DOMAIN_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, dom, false);
		// unassigned application and domain types
		app = rx_create_reference<application_type>(meta::object_type_creation_data{
			RX_NS_SYSTEM_UNASS_APP_TYPE_NAME
			, RX_NS_SYSTEM_UNASS_APP_TYPE_ID
			, RX_CLASS_APPLICATION_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, app, false);
		dom = rx_create_reference<domain_type>(meta::object_type_creation_data{
			RX_NS_SYSTEM_UNASS_TYPE_NAME
			, RX_NS_SYSTEM_UNASS_TYPE_ID
			, RX_CLASS_DOMAIN_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, dom, false);
		// other system object types
		auto obj = rx_create_reference<object_type>(meta::object_type_creation_data{
			RX_COMMANDS_MANAGER_TYPE_NAME
			, RX_COMMANDS_MANAGER_TYPE_ID
			, RX_CLASS_OBJECT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);
		obj = rx_create_reference<object_type>(meta::object_type_creation_data{
			RX_NS_SERVER_RT_TYPE_NAME
			, RX_NS_SERVER_RT_TYPE_ID
			, RX_CLASS_OBJECT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);
		// pool type
		obj = rx_create_reference<object_type>(meta::object_type_creation_data{
			RX_POOL_TYPE_NAME
			, RX_POOL_TYPE_ID
			, RX_CLASS_OBJECT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		obj->complex_data().register_const_value_static("IOPool", false);
		obj->complex_data().register_const_value_static("Threads", (uint8_t)1);
		obj->complex_data().register_simple_value_static("Queue", true, (uint8_t)0);
		obj->complex_data().register_simple_value_static("CPUTime", true, rx_time());
		obj->complex_data().register_simple_value_static("CPU", true, (uint8_t)0);
		add_type_to_configuration(dir, obj, false);
		obj = rx_create_reference<object_type>(meta::object_type_creation_data{
			RX_LOG_TYPE_NAME
			, RX_LOG_TYPE_ID
			, RX_CLASS_OBJECT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);
		obj = rx_create_reference<object_type>(meta::object_type_creation_data{
			RX_IO_MANAGER_TYPE_NAME
			, RX_IO_MANAGER_TYPE_ID
			, RX_CLASS_OBJECT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, obj, false);
	}
	BUILD_LOG_INFO("system_classes_builder", 900, "System types built.");
	return true;
}


// Class sys_internal::builders::port_types_builder 


rx_result port_types_builder::do_build (rx_directory_ptr root)
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_PORT_CLASSES_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir = root->get_sub_directory(path);
	if (dir)
	{// physical ports
		auto port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_PHYSICAL_PORT_TYPE_NAME
			, RX_PHYSICAL_PORT_TYPE_ID
			, RX_CLASS_PORT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			}); 
		port->complex_data().register_struct("Status", RX_PHY_PORT_STATUS_TYPE_ID);
		add_type_to_configuration(dir, port, true);

		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_TTY_PORT_TYPE_NAME
			, RX_TTY_PORT_TYPE_ID
			, RX_PHYSICAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);
		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_ETHERNET_PORT_TYPE_NAME
			, RX_ETHENERT_PORT_TYPE_ID
			, RX_PHYSICAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);
		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_UDP_PORT_TYPE_NAME
			, RX_UDP_PORT_TYPE_ID
			, RX_PHYSICAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data().register_struct("Bind", RX_IP_BIND_TYPE_ID);
		add_type_to_configuration(dir, port, false);
		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_TCP_PORT_TYPE_NAME
			, RX_TCP_PORT_TYPE_ID
			, RX_PHYSICAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data().register_struct("Bind", RX_IP_BIND_TYPE_ID);
		add_type_to_configuration(dir, port, true);
		// tcp/ip port types
		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_TCP_LISTEN_PORT_TYPE_NAME
			, RX_TCP_LISTEN_PORT_TYPE_ID
			, RX_TCP_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);
		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_TCP_SERVER_PORT_TYPE_NAME
			, RX_TCP_SERVER_PORT_TYPE_ID
			, RX_TCP_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);
		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_TCP_CLIENT_PORT_TYPE_NAME
			, RX_TCP_CLIENT_PORT_TYPE_ID
			, RX_TCP_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);
		// transport ports
		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_TRANSPORT_PORT_TYPE_NAME
			, RX_TRANSPORT_PORT_TYPE_ID
			, RX_CLASS_PORT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data().register_struct("Status", RX_PORT_STATUS_TYPE_ID);
		add_type_to_configuration(dir, port, true);

		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_VT00_TYPE_NAME
			, RX_VT00_TYPE_ID
			, RX_TRANSPORT_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);
		// protocol ports
		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_PROTOCOL_PORT_TYPE_NAME
			, RX_PROTOCOL_PORT_TYPE_ID
			, RX_CLASS_PORT_BASE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		port->complex_data().register_struct("Status", RX_PORT_STATUS_TYPE_ID);
		add_type_to_configuration(dir, port, true);

		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_CONSOLE_TYPE_NAME
			, RX_CONSOLE_TYPE_ID
			, RX_PROTOCOL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
		add_type_to_configuration(dir, port, false);
	}
	BUILD_LOG_INFO("port_classes_builder", 900, "Port types built.");
	return true;
}


// Class sys_internal::builders::system_objects_builder 


rx_result system_objects_builder::do_build (rx_directory_ptr root)
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_OBJ_NAME "/" RX_NS_SYSTEM_OBJ_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir = root->get_sub_directory(path);
	if (dir)
	{
		runtime::objects::application_instance_data app_instance_data;
		app_instance_data.processor = 1;
		meta_data meta(RX_NS_SYSTEM_APP_NAME, RX_NS_SYSTEM_APP_ID, RX_NS_SYSTEM_APP_TYPE_ID, namespace_item_attributes::namespace_item_internal_access, full_path);
		auto result = add_object_to_configuration(dir, std::move(meta), std::move(app_instance_data), tl::type2type<application_type>());

		if (result)
		{
			runtime::objects::domain_instance_data domain_instance_data;
			domain_instance_data.processor = 1;
			domain_instance_data.app_id = RX_NS_SYSTEM_APP_ID;
			meta = meta_data(RX_NS_SYSTEM_DOM_NAME, RX_NS_SYSTEM_DOM_ID, RX_NS_SYSTEM_DOM_TYPE_ID, namespace_item_attributes::namespace_item_internal_access, full_path);
			result = add_object_to_configuration(dir, std::move(meta), std::move(domain_instance_data), tl::type2type<domain_type>());
		}

	}
	BUILD_LOG_INFO("system_objects_builder", 900, "System objects built.");
	return true;
}


} // namespace builders
} // namespace sys_internal

