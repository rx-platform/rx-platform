

/****************************************************************************
*
*  sys_internal\rx_internal_objects.cpp
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


// rx_internal_objects
#include "sys_internal/rx_internal_objects.h"



namespace sys_internal {

namespace sys_objects {

// Class sys_internal::sys_objects::system_application 

system_application::system_application()
	: application_runtime(RX_NS_SYSTEM_APP_NAME, RX_NS_SYSTEM_APP_ID, true)
{
	init_object();
}


system_application::~system_application()
{
}



namespace_item_attributes system_application::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_application | namespace_item_read_access | namespace_item_system);
}


// Class sys_internal::sys_objects::system_domain 

system_domain::system_domain()
	: domain_runtime(RX_NS_SYSTEM_DOM_NAME, RX_NS_SYSTEM_DOM_ID, true)
{
	init_object();
}


system_domain::~system_domain()
{
}



namespace_item_attributes system_domain::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_domain | namespace_item_read_access | namespace_item_system);
}


// Class sys_internal::sys_objects::unassigned_application 

unassigned_application::unassigned_application()
	: application_runtime(RX_NS_SYSTEM_UNASS_APP_NAME, RX_NS_SYSTEM_UNASS_APP_ID)
{
	init_object();
}


unassigned_application::~unassigned_application()
{
}



namespace_item_attributes unassigned_application::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_domain | namespace_item_read_access | namespace_item_system);
}


// Class sys_internal::sys_objects::unssigned_domain 

unssigned_domain::unssigned_domain()
	: domain_runtime(RX_NS_SYSTEM_UNASS_NAME, RX_NS_SYSTEM_UNASS_ID)
{
	init_object();
}


unssigned_domain::~unssigned_domain()
{
}



namespace_item_attributes unssigned_domain::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_domain | namespace_item_read_access | namespace_item_system);
}


} // namespace sys_objects
} // namespace sys_internal

