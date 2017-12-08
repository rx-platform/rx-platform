

/****************************************************************************
*
*  system\meta\rx_obj_classes.cpp
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


// rx_obj_classes
#include "system/meta/rx_obj_classes.h"



namespace rx_platform {

namespace meta {

// Class rx_platform::meta::port_class 

string_type port_class::type_name = "PORT CLASS";

port_class::port_class()
{
}

port_class::port_class (const string_type& name, const rx_node_id& id, bool system)
{
}


port_class::~port_class()
{
}



// Class rx_platform::meta::object_class 

string_type object_class::type_name = "OBJECT CLASS";

object_class::object_class()
{
}

object_class::object_class (const string_type& name, const rx_node_id& id, bool system)
	: object_class_t(name, id, system)
{
}


object_class::~object_class()
{
}



void object_class::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

void object_class::get_value (values::rx_value& val) const
{
}

const string_type& object_class::get_item_name () const
{
	return get_name();
}

namespace_item_attributes object_class::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_attributes::namespace_item_command | namespace_item_attributes::namespace_item_execute_access | namespace_item_attributes::namespace_item_read_access | namespace_item_attributes::namespace_item_system);
}


// Class rx_platform::meta::domain_class 

string_type domain_class::type_name = "DOMAIN CLASS";

domain_class::domain_class()
{
}

domain_class::domain_class (const string_type& name, const rx_node_id& id, bool system)
{
}


domain_class::~domain_class()
{
}



// Class rx_platform::meta::application_class 

string_type application_class::type_name = "DOMAIN CLASS";

application_class::application_class()
{
}

application_class::application_class (const string_type& name, const rx_node_id& id, bool system)
{
}


application_class::~application_class()
{
}



} // namespace meta
} // namespace rx_platform

