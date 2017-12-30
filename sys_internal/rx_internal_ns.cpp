

/****************************************************************************
*
*  sys_internal\rx_internal_ns.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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


#include "stdafx.h"


// rx_internal_ns
#include "sys_internal/rx_internal_ns.h"

#include "system/server/rx_server.h"
#include "terminal/rx_commands.h"
#include "system/server/rx_inf.h"
#include "testing/rx_test.h"
#include "classes/rx_meta.h"
#include "rx_configuration.h"



namespace sys_internal {

namespace internal_ns {

// Class sys_internal::internal_ns::root_server_directory 

server_directories_type root_server_directory::root_directories_;

server_items_type root_server_directory::root_items_;

root_server_directory::root_server_directory()
	: rx_server_directory("",root_directories_,root_items_)
{
}


root_server_directory::~root_server_directory()
{
}



void root_server_directory::initialize (hosting::rx_platform_host* host, namespace_data_t& data)
{

	server_directories_type dirs;
	server_items_type items;
	server_items_type sub_items;


	dirs.clear();
	items.clear();

	world_directory::smart_ptr world(RX_NS_WORLD_NAME, dirs, items);



	root_directories_.push_back(world);

	dirs.clear();
	items.clear();
	items.push_back(rx_gate::instance().get_manager().get_unassigned_app());
	items.push_back(rx_gate::instance().get_manager().get_unassigned_domain());
	unassigned_directory::smart_ptr unassigned(RX_NS_UNASSIGNED_NAME, dirs, items);
	root_directories_.push_back(unassigned);

	dirs.clear();
	items.clear();
	std::vector<prog::command_ptr> commands;
	terminal::commands::server_command_manager::instance()->get_commands(commands);
	for (auto one : commands)
		items.push_back(one);
	items.push_back(terminal::commands::server_command_manager::instance());
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
			items.push_back(tcase);
		}
		dirs.push_back(namespace_directory::smart_ptr(one, dummy_empty, items));
	}
	items.clear();
	namespace_directory::smart_ptr test(RX_NS_TEST_NAME, dirs, items);

	dirs.clear();
	items.clear();
	sub_items.clear();// just in case
	sub_items.push_back(rx_gate::instance().get_manager().get_system_app());
	sub_items.push_back(rx_gate::instance().get_manager().get_system_domain());
	sub_items.push_back(rx_platform::rx_gate::instance().get_runtime().get_item_ptr());
	 //io_pool(IO_POOL_NAME, IO_POOL_ID, true);
	sub_items.push_back(rx_platform::rx_gate::instance().get_runtime().get_io_pool());
	ns::rx_platform_item::smart_ptr general_pool = rx_platform::rx_gate::instance().get_runtime().get_general_pool();
	if(general_pool)
		sub_items.push_back(general_pool);
	ns::rx_platform_item::smart_ptr workers = rx_platform::rx_gate::instance().get_runtime().get_workers();
	if(workers)
		sub_items.push_back(workers);
	namespace_directory::smart_ptr objects(RX_NS_OBJ_NAME, dirs, sub_items);


	dirs.clear();
	items.clear();
	namespace_directory::smart_ptr classes(RX_NS_CLASS_NAME, dirs, items);


	dirs.clear();
	items.clear();

	std::vector<meta::object_class_ptr> host_classes;
	std::vector<objects::object_runtime_ptr> host_objects;

	host->get_host_classes(host_classes);
	for (auto one : host_classes)
		model::internal_classes_manager::instance().get_type_cache<object_class>().register_class(one);
	host->get_host_objects(host_objects);

	for (auto one : host_classes)
		items.push_back(one);

	for (auto one : host_objects)
		items.push_back(one);

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

}

void root_server_directory::deinitialize ()
{
}

namespace_item_attributes root_server_directory::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

bool root_server_directory::generate_json (std::ostream& def, std::ostream& err)
{
	return true;
}


// Class sys_internal::internal_ns::namespace_directory 

namespace_directory::namespace_directory (const string_type& name, const server_directories_type& sub_directories, const server_items_type& items)
	: rx_server_directory(name,sub_directories,items)
{
}


namespace_directory::~namespace_directory()
{
}



namespace_item_attributes namespace_directory::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

bool namespace_directory::generate_json (std::ostream& def, std::ostream& err)
{
	return true;
}


// Class sys_internal::internal_ns::unassigned_directory 

unassigned_directory::unassigned_directory (const string_type& name, const server_directories_type& sub_directories, const server_items_type& items)
	: rx_server_directory(name,sub_directories,items)
{
}


unassigned_directory::~unassigned_directory()
{
}



namespace_item_attributes unassigned_directory::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

bool unassigned_directory::generate_json (std::ostream& def, std::ostream& err)
{
	return true;
}


// Class sys_internal::internal_ns::world_directory 

world_directory::world_directory (const string_type& name, const server_directories_type& sub_directories, const server_items_type& items)
	: rx_server_directory(name, sub_directories, items)
{
}


world_directory::~world_directory()
{
}



namespace_item_attributes world_directory::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_write_access | namespace_item_system);
}

bool world_directory::generate_json (std::ostream& def, std::ostream& err)
{
	return true;
}


// Class sys_internal::internal_ns::system_server_item 

system_server_item::system_server_item()
{
}


system_server_item::~system_server_item()
{
}



// Class sys_internal::internal_ns::simple_platform_item 

simple_platform_item::simple_platform_item (const string_type& name, const rx_value& value, namespace_item_attributes attributes, const string_type& type_name, rx_time created_time)
      : value_(value),
        attributes_(attributes),
        type_name_(type_name),
        created_time_(created_time)
	, rx_platform_item(name)
{
}


simple_platform_item::~simple_platform_item()
{
}



void simple_platform_item::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
	has_own_code_info = false;
}

string_type simple_platform_item::get_type_name () const
{
	return type_name_;
}

values::rx_value simple_platform_item::get_value () const
{
	return value_;
}

namespace_item_attributes simple_platform_item::get_attributes () const
{
	return attributes_;
}

bool simple_platform_item::generate_json (std::ostream& def, std::ostream& err) const
{
	def << "Not implemented for simple item.";
	return false;
}

bool simple_platform_item::is_browsable () const
{
	return false;
}

//template simple_platform_item< rx_platform::objects::const_value_item, RX_CONST_VALUE_TYPE_IDX  >;
// Parameterized Class sys_internal::internal_ns::runtime_simple_platform_item 

template <class T, int class_name_idx>
runtime_simple_platform_item<T,class_name_idx>::runtime_simple_platform_item()
{
}

template <class T, int class_name_idx>
runtime_simple_platform_item<T,class_name_idx>::runtime_simple_platform_item (const string_type& name, const rx_value& value, namespace_item_attributes attributes, const string_type& type_name)
{
}


template <class T, int class_name_idx>
runtime_simple_platform_item<T,class_name_idx>::~runtime_simple_platform_item()
{
}



template <class T, int class_name_idx>
void runtime_simple_platform_item<T,class_name_idx>::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

template <class T, int class_name_idx>
string_type runtime_simple_platform_item<T,class_name_idx>::get_type_name ()
{
}

template <class T, int class_name_idx>
values::rx_value runtime_simple_platform_item<T,class_name_idx>::get_value ()
{
}

template <class T, int class_name_idx>
namespace_item_attributes runtime_simple_platform_item<T,class_name_idx>::get_attributes ()
{
}

template <class T, int class_name_idx>
bool runtime_simple_platform_item<T,class_name_idx>::generate_json (std::ostream& def, std::ostream& err)
{
}

template <class T, int class_name_idx>
bool runtime_simple_platform_item<T,class_name_idx>::is_browsable ()
{
}


} // namespace internal_ns
} // namespace sys_internal

