

/****************************************************************************
*
*  system\runtime\rx_internal_objects.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_internal_objects
#include "system/runtime/rx_internal_objects.h"

#include "system/server/rx_server.h"
#include "terminal/rx_console.h"
using namespace rx_platform::runtime;


namespace rx_platform {

namespace sys_objects {

// Class rx_platform::sys_objects::system_application 

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


// Class rx_platform::sys_objects::system_domain 

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


// Class rx_platform::sys_objects::unassigned_application 

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


// Class rx_platform::sys_objects::unassigned_domain 

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


// Class rx_platform::sys_objects::system_object 

system_object::system_object()
      : current_time_(rx_time::now())
{
}


system_object::~system_object()
{
}



namespace_item_attributes system_object::get_attributes () const
{
    return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

system_object::smart_ptr system_object::instance ()
{
    static smart_ptr g_inst;
    if (!g_inst)
        g_inst = smart_ptr::create_from_pointer(new system_object());
    return g_inst;
}

rx_result system_object::initialize_runtime (runtime::runtime_init_context& ctx)
{
    ctx.set_item_static("Info.Instance", rx_gate::instance().get_rx_name());
    ctx.set_item_static("Info.Node", rx_gate::instance().get_host()->get_default_name());
    ctx.set_item_static("Info.StartTime", rx_gate::instance().get_started());
    ctx.set_item_static("Info.Instance", rx_gate::instance().get_rx_name());

    auto result = current_time_.bind("Info.Time", ctx);
    if (result)
    {
        timer_ = create_timer_function([this]()
            {
                current_time_ = rx_time::now();
            });
    }

    ctx.set_item_static("Info.PlatformVer", rx_gate::instance().get_rx_version());
    ctx.set_item_static("Info.LibraryVer", rx_gate::instance().get_lib_version());
    hosting::hosts_type hosts;
    rx_gate::instance().get_host()->get_host_info(hosts);
    if (hosts.size() > 0)
        ctx.set_item_static("Info.HostVer", hosts[0]);
    ctx.set_item_static("Info.TerminalVer", rx_internal::terminal::console::console_runtime::get_terminal_info());
    ctx.set_item_static("Info.CompilerVer", rx_gate::instance().get_comp_version());

    return true;
}

rx_result system_object::start_runtime (runtime::runtime_start_context& ctx)
{

    if (timer_)
    {
        timer_->start(200, true);
    }

    /*
    what->complex_data.register_const_value_static("Instance", rx_gate::instance().get_rx_name());
    what->complex_data.register_const_value_static("Node", rx_gate::instance().get_host()->get_default_name());
    what->complex_data.register_const_value_static("StartTime", rx_gate::instance().get_started());
    what->complex_data.register_simple_value_static("Time", rx_time::now(), true, false);

    what->complex_data.register_const_value_static("PlatformVer", rx_gate::instance().get_rx_version());
    what->complex_data.register_const_value_static("LibraryVer", rx_gate::instance().get_lib_version());
    what->complex_data.register_const_value_static("TerminalVer", terminal::console::console_runtime::get_terminal_info());
    what->complex_data.register_const_value_static("CompilerVer", rx_gate::instance().get_comp_version());
    */
    return true;
}


} // namespace sys_objects
} // namespace rx_platform

