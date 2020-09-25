

/****************************************************************************
*
*  system\meta\rx_obj_types.cpp
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


// rx_obj_types
#include "system/meta/rx_obj_types.h"

#include "sys_internal/rx_internal_ns.h"
using namespace rx_platform::meta::meta_algorithm;


namespace rx_platform {

namespace meta {

namespace object_types {

// Class rx_platform::meta::object_types::application_type 

rx_item_type application_type::type_id = rx_item_type::rx_application_type;


platform_item_ptr application_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::object_types::domain_type 

rx_item_type domain_type::type_id = rx_item_type::rx_domain_type;


platform_item_ptr domain_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::object_types::object_type 

rx_item_type object_type::type_id = rx_item_type::rx_object_type;


void object_type::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

platform_item_ptr object_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::object_types::object_data_type 

object_data_type::object_data_type()
{
}

object_data_type::object_data_type (const string_type& name, const rx_node_id& id, const rx_node_id& parent, bool system, bool sealed, bool abstract)
{
}



rx_result object_data_type::resolve (rx_directory_ptr dir)
{
	// nothing yet to resolve here
	return true;
}

bool object_data_type::check_type (type_check_context& ctx)
{
    return true;
}

rx_result object_data_type::register_relation (const relation_attribute& what, complex_data_type& complex_data)
{
    auto ret = complex_data.check_name(what.name, 0);
    if (ret)
    {
        relations_.emplace_back(what);
    }
    return ret;
}


// Class rx_platform::meta::object_types::port_type 

const rx_item_type port_type::type_id = rx_item_type::rx_port_type;


platform_item_ptr port_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::object_types::relation_attribute 


// Class rx_platform::meta::object_types::relation_type_data 


rx::data::runtime_values_data& relation_type_data::get_overrides () const
{
	static rx::data::runtime_values_data dummy;
	return dummy;
}


// Class rx_platform::meta::object_types::relation_type 

rx_item_type relation_type::type_id = rx_item_type::rx_relation_type;


platform_item_ptr relation_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}

void relation_type::set_runtime_data (runtime_data_prototype& prototype, RTypePtr where)
{
}


} // namespace object_types
} // namespace meta
} // namespace rx_platform

