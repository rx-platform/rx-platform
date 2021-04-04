

/****************************************************************************
*
*  system\runtime\rx_display_blocks.cpp
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


// rx_display_blocks
#include "system/runtime/rx_display_blocks.h"



namespace rx_platform {

namespace runtime {

namespace display_blocks {

// Class rx_platform::runtime::display_blocks::displays_holder 


rx_result displays_holder::get_value (const string_type& path, rx_value& val, runtime_process_context* ctx, bool& not_mine) const
{
	return RX_NOT_IMPLEMENTED;
}

rx_result displays_holder::initialize_displays (runtime::runtime_init_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result displays_holder::deinitialize_displays (runtime::runtime_deinit_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result displays_holder::start_displays (runtime::runtime_start_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result displays_holder::stop_displays (runtime::runtime_stop_context& ctx)
{
	return RX_NOT_IMPLEMENTED;
}

void displays_holder::fill_data (const data::runtime_values_data& data, runtime_process_context* ctx)
{
}

void displays_holder::collect_data (data::runtime_values_data& data, runtime_value_type type) const
{
}

rx_result displays_holder::browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, bool& not_mine)
{
	return RX_NOT_IMPLEMENTED;
}

bool displays_holder::serialize (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool displays_holder::deserialize (base_meta_reader& stream, uint8_t type)
{
	return false;
}


// Class rx_platform::runtime::display_blocks::display_data 

display_data::display_data (structure::runtime_item::smart_ptr&& rt, display_runtime_ptr&& var)
{
}



} // namespace display_blocks
} // namespace runtime
} // namespace rx_platform

