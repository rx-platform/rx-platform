

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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
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
	: rx_platform_directory("", namespace_item_internal_access)
{
}


platform_root::~platform_root()
{
}



// Class sys_internal::internal_ns::user_directory 

user_directory::user_directory (const string_type& name)
	: rx_platform_directory(name, namespace_item_read_access | namespace_item_write_access | namespace_item_delete_access)
{
}


user_directory::~user_directory()
{
}



// Class sys_internal::internal_ns::unassigned_directory 

unassigned_directory::unassigned_directory()
	: rx_platform_directory(RX_NS_UNASSIGNED_NAME, namespace_item_internal_access)
{
}


unassigned_directory::~unassigned_directory()
{
}



// Class sys_internal::internal_ns::world_directory 

world_directory::world_directory()
	: rx_platform_directory(RX_NS_WORLD_NAME, namespace_item_internal_access)
{
}


world_directory::~world_directory()
{
}



// Class sys_internal::internal_ns::internal_directory 

internal_directory::internal_directory (const string_type& name)
	: rx_platform_directory(name, namespace_item_internal_access)
{
}


internal_directory::~internal_directory()
{
}



} // namespace internal_ns
} // namespace sys_internal

template class sys_internal::internal_ns::rx_item_implementation<rx_domain_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<rx_application_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<rx_object_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<rx_port_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<testing::test_case::smart_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<prog::command_ptr>;
template class sys_internal::internal_ns::rx_item_implementation<program_runtime_ptr>;

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
