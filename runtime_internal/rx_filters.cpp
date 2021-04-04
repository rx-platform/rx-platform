

/****************************************************************************
*
*  runtime_internal\rx_filters.cpp
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


// rx_filters
#include "runtime_internal/rx_filters.h"

#include "system/runtime/rx_runtime_helpers.h"
#include "model/rx_meta_internals.h"


namespace rx_internal {

namespace sys_runtime {

namespace filters {
rx_result register_filter_constructors()
{
	auto result = rx_internal::model::platform_types_manager::instance().get_simple_type_repository<filter_type>().register_constructor(
		RX_LINEAR_SCALING_FILTER_TYPE_ID, [] {
			return rx_create_reference<linear_scaling_filter>();
		});
	return true;
}

// Class rx_internal::sys_runtime::filters::linear_scaling_filter 


rx_result linear_scaling_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
	auto result = ctx.bind_item(".HiEU");
	if (!result)
		return result.errors();
	hi_eu_ = result.value();
	result = ctx.bind_item(".LowEU");
	if (!result)
		return result.errors();
	low_eu_ = result.value();
	result = ctx.bind_item(".HiRaw");
	if (!result)
		return result.errors();
	hi_raw_ = result.value();
	result = ctx.bind_item(".LowRaw");
	if (!result)
		return result.errors();
	low_raw_ = result.value();
	return true;
}

rx_result linear_scaling_filter::filter_input (rx_value& val)
{
	double hi_eu = get_binded_as(hi_eu_, 0);
	double low_eu = get_binded_as(low_eu_, 0);
	double hi_raw = get_binded_as(hi_raw_, 0);
	double low_raw = get_binded_as(low_raw_, 0);
	double in = val.get_float_value();
	if (abs(hi_raw - low_raw) == 0)
	{
		val.set_quality(RX_BAD_QUALITY_DIVISION_BY_ZERO);
		return true;
	}
	double result = ((in - low_raw) * (hi_eu - low_eu)) / (hi_raw - low_raw) + low_eu;
	if (!val.set_from_float(result, val.get_type()))
		val.set_quality(RX_BAD_QUALITY_TYPE_MISMATCH);
	return true;
}

rx_result linear_scaling_filter::filter_output (rx_simple_value& val)
{
	double hi_eu = get_binded_as(hi_eu_, 0);
	double low_eu = get_binded_as(low_eu_, 0);
	double hi_raw = get_binded_as(hi_raw_, 0);
	double low_raw = get_binded_as(low_raw_, 0);
	double in = val.get_float_value();
	if (abs(hi_raw - low_raw) == 0)
	{

		return "Division by zero";
	}
	double result = ((in - low_eu) * (hi_raw - low_raw)) / (hi_eu - low_eu) + low_raw;
	if (!val.set_from_float(result, val.get_type()))
		return "Invalid conversion";
	return true;
}


// Class rx_internal::sys_runtime::filters::bound_filter 


rx_result bound_filter::initialize_filter (runtime::runtime_init_context& ctx)
{
  return true;
}

rx_result bound_filter::filter_input (rx_value& val)
{
  return true;
}

rx_result bound_filter::filter_output (rx_simple_value& val)
{
  return true;
}


} // namespace filters
} // namespace sys_runtime
} // namespace rx_internal

