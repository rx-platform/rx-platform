

/****************************************************************************
*
*  system\libraries\rx_plugin.cpp
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


// rx_plugin
#include "system/libraries/rx_plugin.h"

#include "model/rx_meta_internals.h"


namespace rx_platform {
template<typename typeT>
rx_result register_plugin_constructor(const rx_node_id& id, std::function<typename typeT::RImplPtr()> f)
{
	return rx_internal::model::platform_types_manager::instance().get_type_repository<typeT>().register_constructor(id, f);
}
template rx_result register_plugin_constructor<object_type>(const rx_node_id& id, std::function<rx_object_impl_ptr()> f);
template rx_result register_plugin_constructor<domain_type>(const rx_node_id& id, std::function<rx_domain_impl_ptr()> f);
template rx_result register_plugin_constructor<port_type>(const rx_node_id& id, std::function<rx_port_impl_ptr()> f);
template rx_result register_plugin_constructor<application_type>(const rx_node_id& id, std::function<rx_application_impl_ptr()> f);

template<typename typeT>
rx_result register_plugin_simple_constructor(const rx_node_id& id, std::function<typename typeT::RTypePtr()> f)
{
    return rx_internal::model::platform_types_manager::instance().get_simple_type_repository<typeT>().register_constructor(id, f);
}
template rx_result register_plugin_simple_constructor<mapper_type>(const rx_node_id& id, std::function<runtime::mapper_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<source_type>(const rx_node_id& id, std::function<runtime::source_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<filter_type>(const rx_node_id& id, std::function<runtime::filter_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<variable_type>(const rx_node_id& id, std::function<runtime::variable_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<event_type>(const rx_node_id& id, std::function<runtime::event_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<struct_type>(const rx_node_id& id, std::function<runtime::struct_runtime_ptr()> f);


namespace library {

// Class rx_platform::library::rx_plugin_base 

rx_plugin_base::rx_plugin_base()
{
}


rx_plugin_base::~rx_plugin_base()
{
}



} // namespace library
} // namespace rx_platform

