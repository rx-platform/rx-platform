

/****************************************************************************
*
*  runtime_internal\rx_events.cpp
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


// rx_events
#include "runtime_internal/rx_events.h"



namespace rx_internal {

namespace sys_runtime {

namespace events {


rx_result register_event_constructors()
{
    auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<event_type>().register_constructor(
        RX_NS_OBJECT_CHANGED_ID, [] {
            return rx_create_reference<object_chaged_event>();
        });

    return result;
}

// Class rx_internal::sys_runtime::events::object_chaged_event 

std::map<rx_node_id, object_chaged_event::smart_ptr> object_chaged_event::runtime_instances;

object_chaged_event::object_chaged_event()
      : enabled_(0)
{
}


object_chaged_event::~object_chaged_event()
{
}



rx_result object_chaged_event::initialize_event_internal (runtime::runtime_init_context& ctx)
{
    if (ctx.context->object_changed == nullptr)
        ctx.context->object_changed = ctx.event;
    return true;
}

rx_result object_chaged_event::initialize_event (runtime::runtime_init_context& ctx)
{
    RX_ASSERT(ctx.context->object_changed == ctx.event);
    return true;
}

rx_result object_chaged_event::start_event (runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result object_chaged_event::stop_event (runtime::runtime_stop_context& ctx)
{
    return true;
}

rx_result object_chaged_event::deinitialize_event (runtime::runtime_deinit_context& ctx)
{
    return true;
}


} // namespace events
} // namespace sys_runtime
} // namespace rx_internal

