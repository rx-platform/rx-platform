

/****************************************************************************
*
*  system\libraries\rx_plugin_templates.h
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


#ifndef rx_plugin_templates_h
#define rx_plugin_templates_h 1


#include "system/runtime/rx_objbase.h"


namespace rx_platform 
{


extern locks::slim_lock* g_runtime_lock;

namespace library
{
class rx_plugin_base;
}


template<typename typeT>
rx_result register_monitored_plugin_constructor_impl(library::rx_plugin_base* plugin
    , const rx_node_id& id, std::function<rx_platform::constructed_data_t<typename typeT::RImplPtr>(const rx_node_id&)> f);


template<typename typeT, typename derivedT>
rx_result register_plugin_constructor(library::rx_plugin_base* plugin
    , const rx_node_id& id, std::function<typename derivedT::smart_ptr()> f)
{
    auto* container = &derivedT::runtime_instances;
    std::function<constructed_data_t<typename typeT::RImplPtr>(const rx_node_id&)> func =
        [container, f](const rx_node_id& new_id) // full constructor function
    {
        constructed_data_t<typename typeT::RImplPtr> ret;
        auto new_ptr = f();
        if (!new_ptr)
        {
            std::ostringstream ss;
            ss << "Instance " << new_id.to_string()
                << " could not be created for class";
            return ret;
        }
        else
        {
            ret.ptr = new_ptr;
        }
        ret.register_f = [container, new_ptr](const rx_node_id& id)
        {
            locks::auto_slim_lock _(g_runtime_lock);
            auto it = container->find(id);
            if (it == container->end())
                container->emplace(id, new_ptr);
        };
        ret.unregister_f = [container](const rx_node_id& id)
        {
            locks::auto_slim_lock _(g_runtime_lock);
            container->erase(id);
        };
        return ret;
    };
    return register_monitored_plugin_constructor_impl<typeT>(plugin, id, func);
}

template<typename typeT>
rx_result register_plugin_constructor(library::rx_plugin_base* plugin
    , const rx_node_id& id, std::function<typename typeT::RImplPtr()> f);
template<typename typeT>
rx_result register_plugin_simple_constructor(library::rx_plugin_base* plugin
    , const rx_node_id& id, std::function<typename typeT::RTypePtr()> f);


template<typename typeT>
rx_result register_plugin_type(library::rx_plugin_base* plugin, typename typeT::smart_ptr what);
template<typename typeT>
rx_result register_plugin_simple_type(library::rx_plugin_base* plugin, typename typeT::smart_ptr what);

rx_result register_plugin_relation_type(library::rx_plugin_base* plugin, relation_type_ptr what);
rx_result register_plugin_data_type(library::rx_plugin_base* plugin, data_type_ptr what);

template<typename typeT>
rx_result register_plugin_runtime(library::rx_plugin_base* plugin, const typename typeT::instance_data_t& instance_data, const data::runtime_values_data* data);

} // rx_platform




#endif
