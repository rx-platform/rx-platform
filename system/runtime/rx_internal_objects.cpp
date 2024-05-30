

/****************************************************************************
*
*  system\runtime\rx_internal_objects.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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
#include "terminal/rx_vt100.h"
#include "http_server/rx_http_server.h"
using namespace rx_platform::runtime;


namespace rx_platform {

namespace sys_objects {
namespace {
system_application::smart_ptr g_system_application_inst;
world_application::smart_ptr g_world_application_inst;
host_application::smart_ptr g_host_application_inst;
unassigned_application::smart_ptr g_unassigned_application_inst;
system_domain::smart_ptr g_system_domain_inst;
unassigned_domain::smart_ptr g_unassigned_domain_inst;
system_object::smart_ptr g_system_object_inst;
host_object::smart_ptr g_host_object_inst;
memory_object::smart_ptr g_memory_object_inst;
}

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
    if (!g_system_application_inst)
        g_system_application_inst = smart_ptr::create_from_pointer_without_bind(new system_application());
    return g_system_application_inst;
}

void system_application::deinitialize ()
{
    if (g_system_application_inst)
        g_system_application_inst = smart_ptr::null_ptr;
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
    if (!g_system_domain_inst)
        g_system_domain_inst = smart_ptr::create_from_pointer_without_bind(new system_domain());
    return g_system_domain_inst;
}

void system_domain::deinitialize ()
{
    if (g_system_domain_inst)
        g_system_domain_inst = smart_ptr::null_ptr;
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
    if (!g_unassigned_application_inst)
        g_unassigned_application_inst = smart_ptr::create_from_pointer_without_bind(new unassigned_application());
    return g_unassigned_application_inst;
}

void unassigned_application::deinitialize ()
{
    if (g_unassigned_application_inst)
        g_unassigned_application_inst = smart_ptr::null_ptr;
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
    if (!g_unassigned_domain_inst)
        g_unassigned_domain_inst = smart_ptr::create_from_pointer_without_bind(new unassigned_domain());
    return g_unassigned_domain_inst;
}

void unassigned_domain::deinitialize ()
{
    if (g_unassigned_domain_inst)
        g_unassigned_domain_inst = smart_ptr::null_ptr;
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
    return (namespace_item_attributes)(namespace_item_read_access | namespace_item_pull_access | namespace_item_system);
}

system_object::smart_ptr system_object::instance ()
{
    if (!g_system_object_inst)
        g_system_object_inst = smart_ptr::create_from_pointer_without_bind(new system_object());
    return g_system_object_inst;
}

rx_result system_object::initialize_runtime (runtime::runtime_init_context& ctx)
{
    ctx.set_item_static("Info.Instance", rx_gate::instance().get_instance_name().c_str());
    ctx.set_item_static("Info.Node", rx_gate::instance().get_node_name().c_str());
    ctx.set_item_static("Info.StartTime", rx_gate::instance().get_started());

    auto result = current_time_.bind("Info.Time", ctx);
    if (result)
    {

    }

    ctx.set_item_static("Info.PlatformVer", rx_gate::instance().get_rx_version().c_str());
    ctx.set_item_static("Info.LibraryVer", rx_gate::instance().get_lib_version().c_str());
    ctx.set_item_static("Info.TerminalVer", rx_internal::terminal::term_ports::vt100_endpoint::get_terminal_info().c_str());
    ctx.set_item_static("Info.HttpVer", rx_internal::rx_http_server::http_server::get_server_info().c_str());
    ctx.set_item_static("Info.CompilerVer", rx_gate::instance().get_comp_version().c_str());

    return true;
}

rx_result system_object::start_runtime (runtime::runtime_start_context& ctx)
{
    timer_ = create_timer_function([this]()
        {
            system_tick();
        });
    if (timer_)
    {
        timer_->start(200, true);
    }
    return true;
}

void system_object::deinitialize ()
{
    if (timer_)
        timer_->cancel();
    timer_ = rx_timer_ptr::null_ptr;
    if (g_system_object_inst)
        g_system_object_inst = smart_ptr::null_ptr;
}

void system_object::system_tick ()
{
    current_time_ = rx_time::now();
}


// Class rx_platform::sys_objects::host_object 

host_object::host_object()
      : free_memory_(0)
{
}


host_object::~host_object()
{
}



namespace_item_attributes host_object::get_attributes () const
{
    return (namespace_item_attributes)(namespace_item_read_access | namespace_item_pull_access | namespace_item_system);
}

host_object::smart_ptr host_object::instance ()
{
    if (!g_host_object_inst)
        g_host_object_inst = smart_ptr::create_from_pointer_without_bind(new host_object());
    return g_host_object_inst;
}

rx_result host_object::initialize_runtime (runtime::runtime_init_context& ctx)
{
    hosting::hosts_type hosts;
    rx_gate::instance().get_host()->get_host_info(hosts);
    if (hosts.size() > 0)
        ctx.set_item_static("Info.OSHostVer", hosts[0].c_str());
    if(hosts.size()>1)
        ctx.set_item_static("Info.HostVer", hosts[hosts.size() - 1].c_str());

    ctx.set_item_static("Info.FirmwareVer", rx_gate::instance().get_hal_version().c_str());
    ctx.set_item_static("Info.OSVer", rx_gate::instance().get_os_info().c_str());
    char buff[0x100];
    size_t cpu_count = 1;
    rx_collect_processor_info(buff, sizeof(buff) / sizeof(buff[0]), &cpu_count);
    ctx.set_item_static("Info.CPU", buff);
    ctx.set_item_static("Info.CPUCores", (uint32_t)cpu_count);
    ctx.set_item_static("Info.LittleEndian", rx_big_endian == 0);
    ctx.set_item_static("Info.FirmwareVer", rx_gate::instance().get_hal_version().c_str());

    size_t total = 0;
    size_t free = 0;
    size_t process = 0;
    rx_collect_memory_info(&total, &free, &process);

    ctx.set_item_static("Info.MemoryTotal", (uint64_t)total);
    ctx.set_item_static("Info.MemoryFree", (uint64_t)free);
    ctx.set_item_static("Info.PageSize", (uint32_t)rx_os_page_size());

    auto result = free_memory_.bind("Info.MemoryFree", ctx);
    if (result)
    {
    }

    return true;
}

rx_result host_object::start_runtime (runtime::runtime_start_context& ctx)
{
    timer_ = create_timer_function([this]()
        {
            host_tick();
        });
    if (timer_)
    {
        timer_->start(5000, true);
    }
    return true;
}

void host_object::deinitialize ()
{
    if (timer_)
        timer_->cancel();
    timer_ = rx_timer_ptr::null_ptr;
    if (g_host_object_inst)
        g_host_object_inst = smart_ptr::null_ptr;
}

void host_object::host_tick ()
{
    size_t total = 0;
    size_t free = 0;
    size_t process = 0;
    rx_collect_memory_info(&total, &free, &process);
    free_memory_ = (uint64_t)free;

}


// Class rx_platform::sys_objects::world_application 

world_application::world_application()
{
}


world_application::~world_application()
{
}



namespace_item_attributes world_application::get_attributes () const
{
    return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

world_application::smart_ptr world_application::instance ()
{
    if (!g_world_application_inst)
        g_world_application_inst = smart_ptr::create_from_pointer_without_bind(new world_application());
    return g_world_application_inst;
}

void world_application::deinitialize ()
{
    if (g_world_application_inst)
        g_world_application_inst = smart_ptr::null_ptr;
}


// Class rx_platform::sys_objects::host_application 

host_application::host_application()
{
}


host_application::~host_application()
{
}



namespace_item_attributes host_application::get_attributes () const
{
    return (namespace_item_attributes)(namespace_item_read_access | namespace_item_system);
}

host_application::smart_ptr host_application::instance ()
{
    if (!g_host_application_inst)
        g_host_application_inst = smart_ptr::create_from_pointer_without_bind(new host_application());
    return g_host_application_inst;
}

void host_application::deinitialize ()
{
    if (g_host_application_inst)
        g_host_application_inst = smart_ptr::null_ptr;
}


// Class rx_platform::sys_objects::memory_object 

memory_object::memory_object()
{
}


memory_object::~memory_object()
{
}



namespace_item_attributes memory_object::get_attributes () const
{
    return (namespace_item_attributes)(namespace_item_read_access | namespace_item_pull_access | namespace_item_system);
}

memory_object::smart_ptr memory_object::instance ()
{
    if (!g_memory_object_inst)
        g_memory_object_inst = smart_ptr::create_from_pointer_without_bind(new memory_object());
    return g_memory_object_inst;
}

rx_result memory_object::initialize_runtime (runtime::runtime_init_context& ctx)
{
    rx_result result(true);
    result = heap_status_.bind("Status", ctx);
    /*result = heap_total_.bind("Info.HeapTotal", ctx);
    if (result)
    {
    }

    result = heap_used_.bind("Info.HeapUsed", ctx);
    if (result)
    {
    }

    result = heap_perc_.bind("Info.HeapPercent", ctx);
    if (result)
    {
    }*/

    return result;
}

rx_result memory_object::start_runtime (runtime::runtime_start_context& ctx)
{
    timer_ = create_timer_function([this]()
        {
            memory_tick();
        });
    if (timer_)
    {
        timer_->start(5000, true);
    }
    return true;
}

void memory_object::deinitialize ()
{
    if (timer_)
        timer_->cancel();
    timer_ = rx_timer_ptr::null_ptr;
    if (g_memory_object_inst)
        g_memory_object_inst = smart_ptr::null_ptr;
}

void memory_object::memory_tick ()
{

    rx_platform_heap_status status[0x20];
    size_t total_heap = 0;
    size_t heap_used = 0;
    size_t heap_trigger = 0;
    size_t heap_alloc = 0;


    rx_heap_status(status, &total_heap, &heap_used, &heap_trigger, &heap_alloc);

    std::vector<rx_simple_value> buckets_data;

    int idx = 0;
    while (status[idx].bucket_size)
    {
        std::vector<rx_simple_value> arr(5, rx_simple_value());
        arr[0].assign_static(status[idx].bucket_size);
        arr[1].assign_static(status[idx].occuupied);
        arr[2].assign_static(status[idx].free);
        arr[3].assign_static(status[idx].buckets_capacity);
        arr[4].assign_static(status[idx].bucket_max_used);
        rx_simple_value temp_val;
        temp_val.assign_static(arr);
        buckets_data.push_back(std::move(temp_val));
        idx++;
    }
    rx_simple_value rx_tot;
    rx_simple_value rx_used;
    rx_simple_value rx_perc;
    rx_simple_value rx_buckets;
    rx_tot.assign_static(total_heap);
    rx_used.assign_static(heap_used);
    rx_perc.assign_static((double)heap_used / (double)total_heap * 100.0);
    rx_buckets.assign_array(buckets_data);

    rx_simple_value full;
    full.assign_static(std::vector< rx_simple_value>{
            rx_tot,
            rx_used,
            rx_perc,
            rx_buckets
        });


   //heap_status_.commit(std::move(full));

}


} // namespace sys_objects
} // namespace rx_platform

