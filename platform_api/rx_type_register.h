

/****************************************************************************
*
*  D:\RX\Native\Source\platform_api\rx_type_register.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_type_register_h
#define rx_type_register_h 1


#include "rx_objects.h"
#include "rx_var_types.h"
#include "rx_basic_types.h"
#include "rx_logic_types.h"


namespace rx_platform_api
{

template<class T>
rx_result register_runtime()
{
    if constexpr (T::type_id == rx_item_type::rx_object_type)
        return register_object_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_port_type)
        return register_port_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_domain_type)
        return register_domain_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_application_type)
        return register_application_runtime<T>(T::type_node_id);

    else if constexpr (T::type_id == rx_item_type::rx_struct_type)
        return register_struct_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_variable_type)
        return register_variable_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_event_type)
        return register_event_runtime<T>(T::type_node_id);

    else if constexpr (T::type_id == rx_item_type::rx_source_type)
        return register_source_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_mapper_type)
            return register_mapper_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_filter_type)
        return register_filter_runtime<T>(T::type_node_id);

    else if constexpr (T::type_id == rx_item_type::rx_method_type)
        return register_method_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_source_type)
        return register_source_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_display_type)
        return register_display_runtime<T>(T::type_node_id);

    else
        return "Unknown type";
}


template<class T>
rx_result register_monitored_runtime()
{
    if constexpr (T::type_id == rx_item_type::rx_object_type)
        return register_monitored_object_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_port_type)
        return register_monitored_port_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_domain_type)
        return register_monitored_domain_runtime<T>(T::type_node_id);
    else if constexpr (T::type_id == rx_item_type::rx_application_type)
        return register_monitored_application_runtime<T>(T::type_node_id);

    else
        return "Unknown type";
}

} // namespace rx_platform_api




#endif
