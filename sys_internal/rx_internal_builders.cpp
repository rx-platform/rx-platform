

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



rx_result_with<rx_directory_ptr> rx_platform_builder::buid_platform (hosting::rx_platform_host* host, namespace_data_t& data)
{
	auto sys_builders = get_system_builders(data);
	auto user_builders = get_user_builders(data);
	auto test_builders = get_test_builders(data);
	auto other_builders = get_other_builders(data);

	rx_result errors = true;

	auto root = rx_create_reference<sys_internal::internal_ns::platform_root>();
	for (auto& one : sys_builders)
	{
		auto result = one->do_build(root);
		if (!result)
		{
			BUILD_LOG_ERROR("rx_platform_builder", 900, "Error building platform system!");
			errors.register_errors(result.errors());
		}
	}
	// system is critical so an error in building system is fatal
	if (!errors)
		return errors.errors();

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
	/*
	server_directories_type dirs;
	server_items_type items;
	server_items_type sub_items;

	auto system_application = rx_system_application();
	auto system_domain = rx_system_domain();


	sub_items.emplace_back(system_application->get_item_ptr());
	sub_items.emplace_back(system_domain->get_item_ptr());
	sub_items.emplace_back(rx_platform::rx_gate::instance().get_runtime().get_item_ptr());
	//io_pool(IO_POOL_NAME, IO_POOL_ID, true);
	sub_items.emplace_back(rx_platform::rx_gate::instance().get_runtime().get_io_pool()->get_item_ptr());
	ns::rx_platform_item::smart_ptr general_pool = rx_platform::rx_gate::instance().get_runtime().get_general_pool()->get_item_ptr();
	if (general_pool)
		sub_items.emplace_back(general_pool);
	ns::rx_platform_item::smart_ptr workers = rx_platform::rx_gate::instance().get_runtime().get_workers()->get_item_ptr();
	if (workers)
		sub_items.emplace_back(workers);
	namespace_directory::smart_ptr objects(RX_NS_OBJ_NAME, dirs, sub_items);


	dirs.clear();
	items.clear();
	sub_items.clear();

	world_directory::smart_ptr world(RX_NS_WORLD_NAME, dirs, items);
	root_directories_.push_back(world);


	dirs.clear();
	items.clear();

	storage_directory::smart_ptr storage(RX_NS_STORAGE_NAME, dirs, items);
	root_directories_.push_back(storage);

	dirs.clear();
	items.clear();
	items.emplace_back(rx_gate::instance().get_manager().get_unassigned_app()->get_item_ptr());
	items.emplace_back(rx_gate::instance().get_manager().get_unassigned_domain()->get_item_ptr());
	unassigned_directory::smart_ptr unassigned(RX_NS_UNASSIGNED_NAME, dirs, items);
	root_directories_.push_back(unassigned);

	dirs.clear();
	items.clear();
	std::vector<prog::command_ptr> commands;
	terminal::commands::server_command_manager::instance()->get_commands(commands);
	for (auto one : commands)
		items.push_back(one->get_item_ptr());
	items.emplace_back(terminal::commands::server_command_manager::instance()->get_item_ptr());
	namespace_directory::smart_ptr bin(RX_NS_BIN_NAME, dirs, items);

	dirs.clear();
	items.clear();

	server_directories_type dummy_empty;
	string_array categories;
	testing::testing_enviroment::instance().get_categories(categories);
	for (const auto& one : categories)
	{
		items.clear();
		std::vector<testing::test_case::smart_ptr> cases;
		testing::testing_enviroment::instance().collect_test_cases(one, cases);
		for (auto tcase : cases)
		{
			items.emplace_back(tcase->get_item_ptr());
		}
		dirs.push_back(namespace_directory::smart_ptr(one, dummy_empty, items));
	}
	items.clear();
	namespace_directory::smart_ptr test(RX_NS_TEST_NAME, dirs, items);

	dirs.clear();
	items.clear();
	sub_items.clear();// just in case


	dirs.clear();
	items.clear();
	namespace_directory::smart_ptr classes(RX_NS_CLASS_NAME, dirs, items);


	dirs.clear();
	items.clear();

	std::vector<meta::object_class_ptr> host_classes;
	std::vector<runtime::object_runtime_ptr> host_objects;

	host->get_host_classes(host_classes);
	for (auto one : host_classes)
		model::internal_classes_manager::instance().get_type_cache<object_class>().register_class(one);
	host->get_host_objects(host_objects);

	for (auto one : host_classes)
		items.emplace_back(one->get_item_ptr());

	for (auto one : host_objects)
		items.emplace_back(one->get_item_ptr());

	namespace_directory::smart_ptr host_dir(RX_NS_HOST_NAME, dirs, items);

	dirs.clear();
	items.clear();
	dirs.push_back(host_dir);

	namespace_directory::smart_ptr plugins(RX_NS_PLUGINS_NAME, dirs, items);


	dirs.clear();
	items.clear();

	dirs.push_back(objects);
	dirs.push_back(bin);
	dirs.push_back(classes);
	dirs.push_back(plugins);

	namespace_directory::smart_ptr sys(RX_NS_SYS_NAME, dirs, items);

	root_directories_.push_back(sys);
	root_directories_.push_back(test);
	*/
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_system_builders (namespace_data_t& data)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// create system folder structure
	builders.emplace_back(std::make_unique<root_folder_builder>());
	if (data.build_system_from_code)
	{
		// types builders
		builders.emplace_back(std::make_unique<basic_types_builder>());
		builders.emplace_back(std::make_unique<system_classes_builder>());
		builders.emplace_back(std::make_unique<port_classes_builder>());
		//// objects builders
		builders.emplace_back(std::make_unique<system_objects_builder>());
	}
	else
	{
		// storage builder
		builders.emplace_back(std::make_unique<storage::configuration_storage_builder>(meta::rx_storage_type::system_storage));
	}
	return builders;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_user_builders (namespace_data_t& data)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// storage builder
	builders.emplace_back(std::make_unique<storage::configuration_storage_builder>(meta::rx_storage_type::user_storage));
	return builders;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_test_builders (namespace_data_t& data)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// storage builder
	builders.emplace_back(std::make_unique<storage::configuration_storage_builder>(meta::rx_storage_type::test_storage));
	return builders;
}

std::vector<std::unique_ptr<rx_platform_builder> > rx_platform_builder::get_other_builders (namespace_data_t& data)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	return builders;
}


// Class sys_internal::builders::root_folder_builder


rx_result root_folder_builder::do_build (platform_root::smart_ptr root)
{
	root->add_sub_directory(rx_create_reference<world_directory>());
	root->add_sub_directory(rx_create_reference<unassigned_directory>());

	auto sys_dir = rx_create_reference<internal_directory>(RX_NS_SYS_NAME);
	sys_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_BIN_NAME));

	auto classes_dir = rx_create_reference<internal_directory>(RX_NS_CLASSES_NAME);
	classes_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_BASE_CLASSES_NAME));
	classes_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_SYSTEM_CLASSES_NAME));
	classes_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_PORT_CLASSES_NAME));
	sys_dir->add_sub_directory(classes_dir);

	auto objects_dir = rx_create_reference<internal_directory>(RX_NS_OBJ_NAME);
	objects_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_SYSTEM_OBJ_NAME));
	objects_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_PORT_OBJ_NAME));
	sys_dir->add_sub_directory(objects_dir);

	auto plugins_dir = rx_create_reference<internal_directory>(RX_NS_PLUGINS_NAME);
	plugins_dir->add_sub_directory(rx_create_reference<internal_directory>(RX_NS_HOST_NAME));

	sys_dir->add_sub_directory(plugins_dir);

	root->add_sub_directory(sys_dir);
	BUILD_LOG_INFO("root_folder_builder", 900, "Root folder structure built.");
	return true;
}


// Class sys_internal::builders::basic_types_builder


rx_result basic_types_builder::do_build (platform_root::smart_ptr root)
{
	string_type path(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_BASE_CLASSES_NAME);
	string_type full_path = RX_DIR_DELIMETER + path;
	auto dir = root->get_sub_directory(path);
	if (dir)
	{
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
	}
	BUILD_LOG_INFO("basic_types_builder", 900, "Basic types built.");
	return true;
}

template<class T>
void basic_types_builder::build_basic_object_type(rx_directory_ptr dir, rx_reference<T> what)
{
	what->complex_data().register_const_value_static("Description", ""s);
	what->complex_data().register_simple_value_static("Note", false, ""s);
	add_type_to_configuration(dir, what, true);
}
template<class T>
void basic_types_builder::build_basic_domain_type(rx_directory_ptr dir, rx_reference<T> what)
{
	what->complex_data().register_simple_value_static("Processor", true, -1);
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
	what->complex_data().register_const_value_static("Description", ""s);
	what->meta_info().resolve();
	model::platform_types_manager::instance().internal_get_simple_type_cache<T>().register_type(what);
	dir->add_item(what->get_item_ptr());
}
// Class sys_internal::builders::system_classes_builder


rx_result system_classes_builder::do_build (platform_root::smart_ptr root)
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
			, RX_NS_SYSTEM_APP_TYPE_ID
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


// Class sys_internal::builders::port_classes_builder


rx_result port_classes_builder::do_build (platform_root::smart_ptr root)
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
		add_type_to_configuration(dir, port, false);
		port = rx_create_reference<port_type>(meta::object_type_creation_data{
			RX_TCP_PORT_TYPE_NAME
			, RX_TCP_PORT_TYPE_ID
			, RX_PHYSICAL_PORT_TYPE_ID
			, namespace_item_attributes::namespace_item_internal_access
			, full_path
			});
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


rx_result system_objects_builder::do_build (platform_root::smart_ptr root)
{
	//auto dir = root->get_sub_directory(RX_NS_SYS_NAME "/" RX_NS_OBJ_NAME "/" RX_NS_SYSTEM_OBJ_NAME);
	//if (dir)
	//{
	//	// system application and domain
	//	auto app = model::platform_types_manager::instance().create_runtime<rx_platform::meta::object_types::application_type>(RX_NS_SYSTEM_APP_NAME, RX_NS_SYSTEM_APP_ID, RX_NS_SYSTEM_APP_TYPE_ID, true);
	//	//add_object_to_configuration(dir, app);
	//}
	BUILD_LOG_INFO("system_objects_builder", 900, "System objects built.");
	return true;
}


} // namespace builders
} // namespace sys_internal

