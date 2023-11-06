

/****************************************************************************
*
*  runtime_internal\rx_variables.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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

#include "system/runtime/rx_runtime_helpers.h"
#include "model/rx_meta_internals.h"
#include "system/runtime/rx_value_templates.h"
using namespace rx_platform::runtime;

// rx_variables
#include "runtime_internal/rx_variables.h"



namespace rx_internal {

namespace sys_runtime {

namespace variables {
rx_result register_variables_constructors()
{
	auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<variable_type>().register_constructor(
		RX_REGISTER_VARIABLE_TYPE_ID, [] {
			return rx_create_reference<register_variable>();
		});
	return result;
}

// Class rx_internal::sys_runtime::variables::register_variable 


rx_result register_variable::initialize_variable (runtime::runtime_init_context& ctx)
{
	auto result = persist_.bind(".Persist", ctx);
	return true;
}

rx_result register_variable::start_variable (runtime::runtime_start_context& ctx)
{
	rx_value value = ctx.get_current_variable_value();
	value_ = rx_timed_value(value.to_simple(), value.get_time());
	process_variable(ctx.context);
	return true;
}

rx_value register_variable::get_variable_input (runtime_process_context* ctx, runtime_sources_type& sources)
{
	return ctx->adapt_value(value_);
}

rx_result register_variable::variable_write (write_data&& data, runtime_process_context* ctx, runtime_sources_type& sources)
{
	value_ = rx_timed_value(std::move(data.value), rx_time::now());
	if (persist_)
		ctx->runtime_dirty();
	process_variable(ctx);
	variable_result_pending(ctx, true, data.transaction_id);
	return true;
}


} // namespace variables
} // namespace sys_runtime
} // namespace rx_internal

