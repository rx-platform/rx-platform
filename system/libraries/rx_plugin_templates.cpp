

/****************************************************************************
*
*  system\libraries\rx_plugin_templates.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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

#include "system/meta/rx_obj_types.h"
#include "system/meta/rx_types.h"

// rx_plugin_templates
#include "system/libraries/rx_plugin_templates.h"

#include "model/rx_meta_internals.h"

namespace rx_platform
{

template<typename typeT>
rx_result register_monitored_plugin_constructor_impl(library::rx_plugin_base* plugin, const rx_node_id& id
    , std::function<constructed_data_t<typename typeT::RImplPtr>(const rx_node_id&)> f)
{
    return rx_internal::model::platform_types_manager::instance().get_type_repository<typeT>().register_constructor(id, f);
}
template rx_result register_monitored_plugin_constructor_impl<object_types::application_type>(
    library::rx_plugin_base* plugin, const rx_node_id& id
    , std::function<constructed_data_t<object_types::application_type::RImplPtr>(const rx_node_id&)> f);
template rx_result register_monitored_plugin_constructor_impl<object_types::domain_type>(
    library::rx_plugin_base* plugin, const rx_node_id& id
    , std::function<constructed_data_t<object_types::domain_type::RImplPtr>(const rx_node_id&)> f);
template rx_result register_monitored_plugin_constructor_impl<object_types::port_type>(
    library::rx_plugin_base* plugin, const rx_node_id& id
    , std::function<constructed_data_t<object_types::port_type::RImplPtr>(const rx_node_id&)> f);
template rx_result register_monitored_plugin_constructor_impl<object_types::object_type>(
    library::rx_plugin_base* plugin, const rx_node_id& id
    , std::function<constructed_data_t<object_types::object_type::RImplPtr>(const rx_node_id&)> f);

template<typename typeT>
rx_result register_plugin_constructor(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<typename typeT::RImplPtr()> f)
{
	return rx_internal::model::platform_types_manager::instance().get_type_repository<typeT>().register_constructor(id, f);
}
template rx_result register_plugin_constructor<meta::object_types::object_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<rx_object_impl_ptr()> f);
template rx_result register_plugin_constructor<meta::object_types::domain_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<rx_domain_impl_ptr()> f);
template rx_result register_plugin_constructor<meta::object_types::port_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<rx_port_impl_ptr()> f);
template rx_result register_plugin_constructor<meta::object_types::application_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<rx_application_impl_ptr()> f);

template<typename typeT>
rx_result register_plugin_simple_constructor(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<typename typeT::RTypePtr()> f)
{
	return rx_internal::model::platform_types_manager::instance().get_simple_type_repository<typeT>().register_constructor(id, f);
}
template rx_result register_plugin_simple_constructor<meta::basic_types::mapper_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<runtime::mapper_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<meta::basic_types::source_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<runtime::source_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<meta::basic_types::filter_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<runtime::filter_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<meta::basic_types::variable_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<runtime::variable_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<meta::basic_types::event_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<runtime::event_runtime_ptr()> f);
template rx_result register_plugin_simple_constructor<meta::basic_types::struct_type>(library::rx_plugin_base* plugin, const rx_node_id& id, std::function<runtime::struct_runtime_ptr()> f);

} // rx_platform



