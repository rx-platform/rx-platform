

/****************************************************************************
*
*  system\meta\rx_construction_templates.h
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


#ifndef rx_construction_templates_h
#define rx_construction_templates_h 1


#include "system/runtime/rx_objbase.h"


namespace rx_platform
{
namespace library
{
class rx_plugin_base;
}


extern locks::slim_lock* g_runtime_lock;

template<typename typeT>
rx_result register_monitored_constructor_impl(const rx_node_id& id
    , std::function<constructed_data_t<typename typeT::RImplPtr>(const rx_node_id&)> f);

template<typename derivedT>
rx_result_with<typename derivedT::smart_ptr> get_runtime_instance(const rx_node_id& id)
{
    auto* container = &derivedT::runtime_instances;
    locks::auto_lock_t _(g_runtime_lock);
    auto it = container->find(id);
    if (it == container->end())
    {
        std::ostringstream ss;
        ss << "Instance " << id.to_string()
            << " not registered.";
        return ss.str();
    }
    return it->second;
}

}// rx_platform




#endif
