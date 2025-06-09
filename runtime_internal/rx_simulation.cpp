

/****************************************************************************
*
*  runtime_internal\rx_simulation.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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

#include "system/runtime/rx_value_templates.h"
#include "system/meta/rx_meta_attr_algorithm.h"

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
      : persistent_(false)
{
}



rx_result local_register_source::source_write (write_data&& data, runtime_process_context* ctx)
{
    
    rx_value value(std::move(data.value), rx_time::now());
    if (value.convert_to(get_value_type()))
    {        
        value.set_quality(RX_GOOD_QUALITY);
        value = ctx->adapt_value(value);
		rx_simple_value old_value = current_value_;
        current_value_ = value.to_simple();
		if (current_value_ == old_value)
		{
			source_result_received(true, data.transaction_id);
			return true;
		}
        source_value_changed(std::move(value));
        if (persistent_)
        {
            ctx->runtime_dirty([ctx, this, old_value = std::move(old_value), trans_id = data.transaction_id](rx_result result)
                {
                    if (!result)
                    {
                        current_value_ = old_value;
                        rx_value value(std::move(old_value), rx_time::now());
                        if (value.convert_to(get_value_type()))
                        {
                            value.set_quality(RX_GOOD_QUALITY);
                            value = ctx->adapt_value(value);
                            source_value_changed(std::move(value));
						}
						else
						{
							RX_ASSERT(false);// shouldn't happen
						}

                    }
                    source_result_received(std::move(result), trans_id);
                });
		}
        else
        {
            source_result_received(true, data.transaction_id);
        }
        return true;
    }
    else
    {
        return RX_INVALID_CONVERSION;
    }
}

rx_result local_register_source::start_source (runtime::runtime_start_context& ctx)
{
    auto cur_var = ctx.variables.get_current_variable();
    if (cur_var.index() == 0)
    {
        persistent_ |= std::get<0>(cur_var)->value_opt[value_opt_persistent];
    }
    else if (cur_var.index() == 1)
    {
		persistent_ |= std::get<1>(cur_var)->variable.value_opt[value_opt_persistent];
    }
    rx_value value = ctx.get_current_variable_value();
    if(value.convert_to(get_value_type()))
        value.set_quality(RX_GOOD_QUALITY);
    else
        value.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
    value = ctx.context->adapt_value(value);
    current_value_ = value.to_simple();
    source_value_changed(std::move(value));
    return true;
}

rx_result local_register_source::initialize_source (runtime::runtime_init_context& ctx)
{
    persistent_ = ctx.get_item_static(".Persist", false);
    return true;
}


} // namespace simulation
} // namespace sys_runtime
} // namespace rx_internal

