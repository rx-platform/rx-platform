

/****************************************************************************
*
*  sys_internal\rx_internal_objects.cpp
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


// rx_internal_objects
#include "sys_internal/rx_internal_objects.h"

#include "system/server/rx_server.h"


namespace rx_internal {

namespace sys_objects {

// Class rx_internal::sys_objects::system_application 

system_application::system_application()
{
}


system_application::~system_application()
{
}



namespace_item_attributes system_application::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

system_application::smart_ptr system_application::instance ()
{
    static smart_ptr g_inst;
    if (!g_inst)
        g_inst = smart_ptr::create_from_pointer(new system_application());
    return g_inst;
}


// Class rx_internal::sys_objects::system_domain 

system_domain::system_domain()
{
}


system_domain::~system_domain()
{
}



namespace_item_attributes system_domain::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

system_domain::smart_ptr system_domain::instance ()
{
    static smart_ptr g_inst;
    if (!g_inst)
        g_inst = smart_ptr::create_from_pointer(new system_domain());
    return g_inst;
}


// Class rx_internal::sys_objects::unassigned_application 

unassigned_application::unassigned_application()
{
}


unassigned_application::~unassigned_application()
{
}



namespace_item_attributes unassigned_application::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

unassigned_application::smart_ptr unassigned_application::instance ()
{
    static smart_ptr g_inst;
    if (!g_inst)
        g_inst = smart_ptr::create_from_pointer(new unassigned_application());
    return g_inst;
}


// Class rx_internal::sys_objects::unassigned_domain 

unassigned_domain::unassigned_domain()
{
}


unassigned_domain::~unassigned_domain()
{
}



namespace_item_attributes unassigned_domain::get_attributes () const
{
	return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

unassigned_domain::smart_ptr unassigned_domain::instance ()
{
    static smart_ptr g_inst;
    if (!g_inst)
        g_inst = smart_ptr::create_from_pointer(new unassigned_domain());
    return g_inst;
}


} // namespace sys_objects
} // namespace rx_internal



// Detached code regions:
// WARNING: this code will be lost if code is regenerated.
#if 0
    static smart_ptr g_inst;
    if (!g_inst)
        g_inst = smart_ptr::create_from_pointer(new system_application());
    return g_inst;

#endif
