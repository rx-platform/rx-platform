

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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_internal_builders
#include "sys_internal/rx_internal_builders.h"

#include "system/server/rx_server.h"
#include "terminal/rx_commands.h"
#include "system/server/rx_inf.h"
#include "system/meta/rx_classes.h"
#include "system/meta/rx_obj_classes.h"
#include "testing/rx_test.h"
#include "model/rx_meta.h"
#include "rx_configuration.h"


namespace sys_internal {

namespace builders {

template<class T>
void add_type_to_configuration(server_directory_ptr dir, rx_reference<T> what)
{
	dir->add_item(what->get_item_ptr());
	model::internal_types_manager::instance().get_type_cache<T>().register_class(what);
}

template<class T>
void add_object_to_configuration(server_directory_ptr dir, rx_reference<T> what)
{
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



server_directory_ptr rx_platform_builder::buid_platform_system (hosting::rx_platform_host* host, namespace_data_t& data)
{
	std::vector<std::unique_ptr<rx_platform_builder> > builders;
	// types builders
	builders.emplace_back(std::make_unique<root_folder_builder>());
	builders.emplace_back(std::make_unique<basic_types_builder>());
	builders.emplace_back(std::make_unique<system_classes_builder>());
	builders.emplace_back(std::make_unique<port_classes_builder>());
	// objects builders
	builders.emplace_back(std::make_unique<system_objects_builder>());

	auto root = rx_create_reference<sys_internal::internal_ns::platform_root>();

	for (auto& one : builders)
	{
		one->build(root);
	}

	// now add host classes
	/*std::vector<object_class_ptr> host_classes;
	auto current = host;
	while (current)
	{
		current->get_host_classes()
	}*/

	return root;
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


// Class sys_internal::builders::root_folder_builder 


void root_folder_builder::build (platform_root::smart_ptr root)
{
	root->add_sub_directory(rx_create_reference<world_directory>());
	root->add_sub_directory(rx_create_reference<storage_directory>());
	root->add_sub_directory(rx_create_reference<unassigned_directory>());

	auto sys_dir = rx_create_reference<namespace_directory>(RX_NS_SYS_NAME);
	sys_dir->add_sub_directory(rx_create_reference<namespace_directory>(RX_NS_BIN_NAME));

	auto classes_dir = rx_create_reference<namespace_directory>(RX_NS_CLASSES_NAME);
	classes_dir->add_sub_directory(rx_create_reference<namespace_directory>(RX_NS_BASE_CLASSES_NAME));
	classes_dir->add_sub_directory(rx_create_reference<namespace_directory>(RX_NS_SYSTEM_CLASSES_NAME));
	classes_dir->add_sub_directory(rx_create_reference<namespace_directory>(RX_NS_PORT_CLASSES_NAME));
	sys_dir->add_sub_directory(classes_dir);

	auto objects_dir = rx_create_reference<namespace_directory>(RX_NS_OBJ_NAME);
	objects_dir->add_sub_directory(rx_create_reference<namespace_directory>(RX_NS_SYSTEM_OBJ_NAME));
	objects_dir->add_sub_directory(rx_create_reference<namespace_directory>(RX_NS_PORT_OBJ_NAME));
	sys_dir->add_sub_directory(objects_dir);

	auto plugins_dir = rx_create_reference<namespace_directory>(RX_NS_PLUGINS_NAME);
	plugins_dir->add_sub_directory(rx_create_reference<namespace_directory>(RX_NS_HOST_NAME));

	sys_dir->add_sub_directory(plugins_dir);

	root->add_sub_directory(sys_dir);
}


// Class sys_internal::builders::basic_types_builder 


void basic_types_builder::build (platform_root::smart_ptr root)
{
	auto dir = root->get_sub_directory(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_BASE_CLASSES_NAME);
	if (dir)
	{
		//build base object type
		auto obj = rx_create_reference<object_class>(object_defs::object_type_creation_data{ RX_CLASS_OBJECT_BASE_NAME, RX_CLASS_OBJECT_BASE_ID, rx_node_id::null_id, true });
		build_basic_object_type<object_class>(dir, obj);

		//build derived object types
		auto app = rx_create_reference<application_class>(object_defs::object_type_creation_data{ RX_CLASS_APPLICATION_BASE_NAME, RX_CLASS_APPLICATION_BASE_ID, rx_node_id::null_id, true });
		build_basic_application_type<application_class>(dir, app);
		auto domain = rx_create_reference<domain_class>(object_defs::object_type_creation_data{ RX_CLASS_DOMAIN_BASE_NAME, RX_CLASS_DOMAIN_BASE_ID, rx_node_id::null_id, true });
		build_basic_domain_type<domain_class>(dir, domain);
		auto port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_CLASS_PORT_BASE_NAME, RX_CLASS_PORT_BASE_ID, rx_node_id::null_id, true });
		build_basic_port_type<port_class>(dir, port);

		//build base types, user extensible
		auto str = rx_create_reference<basic_defs::struct_class>(basic_defs::type_creation_data { RX_CLASS_STRUCT_BASE_NAME, RX_CLASS_STRUCT_BASE_ID, rx_node_id::null_id, true });
		build_basic_type<basic_defs::struct_class>(dir, str);
		auto var = rx_create_reference<basic_defs::variable_class>(RX_CLASS_VARIABLE_BASE_NAME, RX_CLASS_VARIABLE_BASE_ID, rx_node_id::null_id, true);
		build_basic_type<basic_defs::variable_class>(dir, var);
		
		// build base types, code only extensible
		auto map = rx_create_reference<basic_defs::mapper_class>(RX_CLASS_MAPPER_BASE_NAME, RX_CLASS_MAPPER_BASE_ID, rx_node_id::null_id, true);
		build_basic_type<basic_defs::mapper_class>(dir, map);
		auto evnt = rx_create_reference<basic_defs::event_class>(RX_CLASS_EVENT_BASE_NAME, RX_CLASS_EVENT_BASE_ID, rx_node_id::null_id, true);
		build_basic_type<basic_defs::event_class>(dir, evnt);
		auto filt = rx_create_reference<basic_defs::filter_class>(RX_CLASS_FILTER_BASE_NAME, RX_CLASS_FILTER_BASE_ID, rx_node_id::null_id, true);
		build_basic_type<basic_defs::filter_class>(dir, filt);
		auto src = rx_create_reference<basic_defs::source_class>(RX_CLASS_SOURCE_BASE_NAME, RX_CLASS_SOURCE_BASE_ID, rx_node_id::null_id, true);
		build_basic_type<basic_defs::source_class>(dir, src);
	}
}

template<class T>
void basic_types_builder::build_basic_object_type(server_directory_ptr dir, rx_reference<T> what)
{
	what->complex_data().register_const_value_static("Description", ""s);
	what->complex_data().register_simple_value_static("Note", false, ""s);
	dir->add_item(what->get_item_ptr());
	model::internal_types_manager::instance().get_type_cache<T>().register_class(what);
}
template<class T>
void basic_types_builder::build_basic_domain_type(server_directory_ptr dir, rx_reference<T> what)
{
	
	what->complex_data().register_simple_value_static("Processor", true, -1); 
	build_basic_object_type(dir, what);
}
template<class T>
void basic_types_builder::build_basic_application_type(server_directory_ptr dir, rx_reference<T> what)
{
	build_basic_domain_type(dir, what);
}
template<class T>
void basic_types_builder::build_basic_port_type(server_directory_ptr dir, rx_reference<T> what)
{
	build_basic_object_type(dir, what);
}
template<class T>
void basic_types_builder::build_basic_type(server_directory_ptr dir, rx_reference<T> what)
{
	what->complex_data().register_const_value_static("Description", ""s);
	dir->add_item(what->get_item_ptr());
	model::internal_types_manager::instance().get_type_cache<T>().register_class(what);
}

// Class sys_internal::builders::system_classes_builder 


void system_classes_builder::build (platform_root::smart_ptr root)
{
	auto dir = root->get_sub_directory(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_SYSTEM_CLASSES_NAME);
	if (dir)
	{
		// system application and domain types
		auto app = rx_create_reference<application_class>(object_defs::object_type_creation_data{ RX_NS_SYSTEM_APP_TYPE_NAME, RX_NS_SYSTEM_APP_TYPE_ID, RX_CLASS_APPLICATION_BASE_ID, true });
		add_type_to_configuration(dir, app);
		auto dom = rx_create_reference<domain_class>(object_defs::object_type_creation_data{ RX_NS_SYSTEM_DOM_TYPE_NAME, RX_NS_SYSTEM_DOM_TYPE_ID, RX_CLASS_DOMAIN_BASE_ID, true });
		add_type_to_configuration(dir, dom);
		// unassigned application and domain types
		app = rx_create_reference<application_class>(object_defs::object_type_creation_data{ RX_NS_SYSTEM_UNASS_APP_TYPE_NAME, RX_NS_SYSTEM_APP_TYPE_ID, RX_CLASS_APPLICATION_BASE_ID, true });
		add_type_to_configuration(dir, app);
		dom = rx_create_reference<domain_class>(object_defs::object_type_creation_data{ RX_NS_SYSTEM_UNASS_TYPE_NAME, RX_NS_SYSTEM_UNASS_TYPE_ID, RX_CLASS_DOMAIN_BASE_ID, true });
		add_type_to_configuration(dir, dom);
		// other system object types
		auto obj = rx_create_reference<object_class>(object_defs::object_type_creation_data{ RX_COMMANDS_MANAGER_TYPE_NAME, RX_COMMANDS_MANAGER_TYPE_ID, RX_CLASS_OBJECT_BASE_ID, true });
		add_type_to_configuration(dir, obj);
		obj = rx_create_reference<object_class>(object_defs::object_type_creation_data{ RX_NS_SERVER_RT_TYPE_NAME, RX_NS_SERVER_RT_TYPE_ID, RX_CLASS_OBJECT_BASE_ID, true });
		add_type_to_configuration(dir, obj);
		// pool type
		obj = rx_create_reference<object_class>(object_defs::object_type_creation_data{ RX_POOL_TYPE_NAME, RX_POOL_TYPE_ID, RX_CLASS_OBJECT_BASE_ID, true });
		obj->complex_data().register_const_value_static("IOPool", false);
		obj->complex_data().register_const_value_static("Threads", (uint8_t)1);
		obj->complex_data().register_simple_value_static("Queue", true, (uint8_t)0);
		obj->complex_data().register_simple_value_static("CPUTime", true, rx_time());
		obj->complex_data().register_simple_value_static("CPU", true, (uint8_t)0);
		add_type_to_configuration(dir, obj);
		obj = rx_create_reference<object_class>(object_defs::object_type_creation_data{ RX_LOG_TYPE_NAME, RX_LOG_TYPE_ID, RX_CLASS_OBJECT_BASE_ID, true });
		add_type_to_configuration(dir, obj);
		obj = rx_create_reference<object_class>(object_defs::object_type_creation_data{ RX_IO_MANAGER_TYPE_NAME, RX_IO_MANAGER_TYPE_ID, RX_CLASS_OBJECT_BASE_ID, true });
		add_type_to_configuration(dir, obj);
	}
}


// Class sys_internal::builders::port_classes_builder 


void port_classes_builder::build (platform_root::smart_ptr root)
{
	auto dir = root->get_sub_directory(RX_NS_SYS_NAME "/" RX_NS_CLASSES_NAME "/" RX_NS_PORT_CLASSES_NAME);
	if (dir)
	{// physical ports
		auto port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_PHYSICAL_PORT_TYPE_NAME, RX_PHYSICAL_PORT_TYPE_ID, RX_CLASS_PORT_BASE_ID, true });
		add_type_to_configuration(dir, port);
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_TTY_PORT_TYPE_NAME, RX_TTY_PORT_TYPE_ID, RX_PHYSICAL_PORT_TYPE_ID, true });
		add_type_to_configuration(dir, port);
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_ETHERNET_PORT_TYPE_NAME, RX_ETHENERT_PORT_TYPE_ID, RX_PHYSICAL_PORT_TYPE_ID, true });
		add_type_to_configuration(dir, port);
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_UDP_PORT_TYPE_NAME, RX_UDP_PORT_TYPE_ID, RX_PHYSICAL_PORT_TYPE_ID, true });
		add_type_to_configuration(dir, port);
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_TCP_PORT_TYPE_NAME, RX_TCP_PORT_TYPE_ID, RX_PHYSICAL_PORT_TYPE_ID, true });
		add_type_to_configuration(dir, port);
		// tcp/ip port types
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_TCP_LISTEN_PORT_TYPE_NAME, RX_TCP_LISTEN_PORT_TYPE_ID, RX_TCP_PORT_TYPE_ID, true });
		add_type_to_configuration(dir, port);
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_TCP_SERVER_PORT_TYPE_NAME, RX_TCP_SERVER_PORT_TYPE_ID, RX_TCP_PORT_TYPE_ID, true });
		add_type_to_configuration(dir, port);
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_TCP_CLIENT_PORT_TYPE_NAME, RX_TCP_CLIENT_PORT_TYPE_ID, RX_TCP_PORT_TYPE_ID, true });
		add_type_to_configuration(dir, port);
		// transport ports
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_TRANSPORT_PORT_TYPE_NAME, RX_TRANSPORT_PORT_TYPE_ID, RX_CLASS_PORT_BASE_ID, true} );
		add_type_to_configuration(dir, port);
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_VT00_TYPE_NAME, RX_VT00_TYPE_ID, RX_TRANSPORT_PORT_TYPE_ID, true} );
		add_type_to_configuration(dir, port);
		// protocol ports
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_PROTOCOL_PORT_TYPE_NAME, RX_PROTOCOL_PORT_TYPE_ID, RX_CLASS_PORT_BASE_ID, true} );
		add_type_to_configuration(dir, port);
		port = rx_create_reference<port_class>(object_defs::object_type_creation_data{ RX_CONSOLE_TYPE_NAME, RX_CONSOLE_TYPE_ID, RX_PROTOCOL_PORT_TYPE_ID, true} );
		add_type_to_configuration(dir, port);
	}
}


// Class sys_internal::builders::system_objects_builder 


void system_objects_builder::build (platform_root::smart_ptr root)
{
	auto dir = root->get_sub_directory(RX_NS_SYS_NAME "/" RX_NS_OBJ_NAME "/" RX_NS_SYSTEM_OBJ_NAME);
	if (dir)
	{
		// system application and domain
		auto app = model::internal_types_manager::instance().get_type_cache<rx_platform::meta::object_defs::application_class>().create_runtime(RX_NS_SYSTEM_APP_NAME, RX_NS_SYSTEM_APP_ID, RX_NS_SYSTEM_APP_TYPE_ID, true);
		add_object_to_configuration(dir, app);
	}
}


} // namespace builders
} // namespace sys_internal

