

/****************************************************************************
*
*  system\meta\rx_construction_templates.cpp
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

#include "rx_obj_types.h"

// rx_construction_templates
#include "system/meta/rx_construction_templates.h"

#include "model/rx_meta_internals.h"

namespace rx_platform
{

template<typename typeT>
rx_result register_monitored_constructor_impl(const rx_node_id& id
    , std::function<constructed_data_t<typename typeT::RImplPtr>(const rx_node_id&)> f)
{
    return rx_internal::model::platform_types_manager::instance().get_type_repository<typeT>().register_constructor(id, f);
}
template rx_result register_monitored_constructor_impl<object_types::application_type>(const rx_node_id& id
    , std::function<constructed_data_t<object_types::application_type::RImplPtr>(const rx_node_id&)> f);
template rx_result register_monitored_constructor_impl<object_types::domain_type>(const rx_node_id& id
    , std::function<constructed_data_t<object_types::domain_type::RImplPtr>(const rx_node_id&)> f);
template rx_result register_monitored_constructor_impl<object_types::port_type>(const rx_node_id& id
    , std::function<constructed_data_t<object_types::port_type::RImplPtr>(const rx_node_id&)> f);
template rx_result register_monitored_constructor_impl<object_types::object_type>(const rx_node_id& id
    , std::function<constructed_data_t<object_types::object_type::RImplPtr>(const rx_node_id&)> f);

} // rx_platform


