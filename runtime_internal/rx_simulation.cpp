

/****************************************************************************
*
*  runtime_internal\rx_simulation.cpp
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


// rx_simulation
#include "runtime_internal/rx_simulation.h"

#include "model/rx_meta_internals.h"


namespace rx_internal {

namespace sys_runtime {

namespace simulation {
rx_result register_simulation_constructors()
{
    auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<source_type>().register_constructor(
        RX_REGISTER_SOURCE_TYPE_ID, [] {
            return rx_create_reference<local_register_source>();
        });
    return true;
}

// Class rx_internal::sys_runtime::simulation::local_register_source 

local_register_source::local_register_source()
{
}



rx_result local_register_source::source_write (structure::write_data&& data, runtime_process_context* ctx)
{
    rx_value value = rx_value::from_simple(std::move(data.value), rx_time::now());
    value.set_quality(RX_GOOD_QUALITY);
    source_value_changed(std::move(value));
    source_result_received(true, data.transaction_id);
    return true;
}

rx_result local_register_source::start_source (runtime::runtime_start_context& ctx)
{
    rx_value value = ctx.variables.get_current_variable()->get_value(ctx.context);
    value.set_quality(RX_GOOD_QUALITY);
    source_value_changed(std::move(value));
    return true;
}


} // namespace simulation
} // namespace sys_runtime
} // namespace rx_internal

