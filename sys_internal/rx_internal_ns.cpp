

/****************************************************************************
*
*  sys_internal\rx_internal_ns.cpp
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


// rx_internal_ns
#include "sys_internal/rx_internal_ns.h"

#include "testing/rx_test.h"
#include "sys_internal/rx_internal_builders.h"


namespace sys_internal {

namespace internal_ns {

// Class sys_internal::internal_ns::platform_root 

platform_root::platform_root()
	: rx_server_directory("")
{
}


platform_root::~platform_root()
{
}



namespace_item_attributes platform_root::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}


// Class sys_internal::internal_ns::namespace_directory 

namespace_directory::namespace_directory (const string_type& name)
	: rx_server_directory(name)
{
}


namespace_directory::~namespace_directory()
{
}



namespace_item_attributes namespace_directory::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}


// Class sys_internal::internal_ns::unassigned_directory 

unassigned_directory::unassigned_directory()
	: rx_server_directory(RX_NS_UNASSIGNED_NAME)
{
}


unassigned_directory::~unassigned_directory()
{
}



namespace_item_attributes unassigned_directory::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}


// Class sys_internal::internal_ns::world_directory 

world_directory::world_directory()
	: rx_server_directory(RX_NS_WORLD_NAME)
{
}


world_directory::~world_directory()
{
}



namespace_item_attributes world_directory::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_write_access | namespace_item_system);
}


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
  return "simple";
}

template <class T, int class_name_idx>
values::rx_value runtime_simple_platform_item<T,class_name_idx>::get_value ()
{
  return values::rx_value {};
}

template <class T, int class_name_idx>
namespace_item_attributes runtime_simple_platform_item<T,class_name_idx>::get_attributes ()
{
  return 0;
}

template <class T, int class_name_idx>
bool runtime_simple_platform_item<T,class_name_idx>::generate_json (std::ostream& def, std::ostream& err)
{
  return false;
}

template <class T, int class_name_idx>
bool runtime_simple_platform_item<T,class_name_idx>::is_browsable ()
{
  return false;
}

template <class T, int class_name_idx>
size_t runtime_simple_platform_item<T,class_name_idx>::get_size () const
{
  return 0;
}

template <class T, int class_name_idx>
rx_node_id runtime_simple_platform_item<T,class_name_idx>::get_node_id () const
{
	return rx_node_id::null_id;
}


// Class sys_internal::internal_ns::storage_directory 

storage_directory::storage_directory()
	: rx_server_directory(RX_NS_STORAGE_NAME)
{
}


storage_directory::~storage_directory()
{
}



namespace_item_attributes storage_directory::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_write_access | namespace_item_system);
}

void storage_directory::get_content (server_directories_type& sub_directories, server_items_type& sub_items, const string_type& pattern) const
{
	rx_gate::instance().get_host()->get_storage()->list_storage("", sub_directories, sub_items, pattern);
	rx_server_directory::get_content(sub_directories, sub_items, pattern);
}


} // namespace internal_ns
} // namespace sys_internal

template class sys_internal::internal_ns::rx_item_implementation<runtime::rx_domain_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<runtime::rx_application_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<runtime::object_runtime_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<testing::test_case::smart_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<prog::command_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<logic::program_runtime_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<hosting::rx_platform_file::smart_ptr>;

template class sys_internal::internal_ns::rx_meta_item_implementation<meta::application_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::domain_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::port_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::object_type_ptr>;

template class sys_internal::internal_ns::rx_meta_item_implementation<meta::struct_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::mapper_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::variable_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::event_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::filter_type_ptr>;
template class sys_internal::internal_ns::rx_meta_item_implementation<meta::source_type_ptr>;

