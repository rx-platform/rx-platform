

/****************************************************************************
*
*  system\displays\rx_displays.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_displays
#include "system/displays/rx_displays.h"



namespace rx_platform {

namespace displays {

// Class rx_platform::displays::display_runtime 

display_runtime::display_runtime()
{
}

display_runtime::display_runtime (const string_type& name, const rx_node_id& id)
{
}


display_runtime::~display_runtime()
{
}



bool display_runtime::serialize (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool display_runtime::deserialize (base_meta_reader& stream, uint8_t type)
{
	return false;
}

bool display_runtime::save_display (base_meta_writer& stream, uint8_t type) const
{
	return false;
}

bool display_runtime::load_display (base_meta_reader& stream, uint8_t type)
{
	return false;
}

rx_result display_runtime::initialize_display (runtime::runtime_init_context& ctx, const string_type& disp_path)
{
	return true;
}

rx_result display_runtime::deinitialize_display (runtime::runtime_deinit_context& ctx, const string_type& disp_path)
{
	return true;
}

rx_result display_runtime::start_display (runtime::runtime_start_context& ctx, const string_type& disp_path)
{
	return true;
}

rx_result display_runtime::stop_display (runtime::runtime_stop_context& ctx, const string_type& disp_path)
{
	return true;
}

rx_result display_runtime::register_display (runtime::runtime_start_context& ctx, const string_type& disp_path)
{
	return RX_NOT_SUPPORTED;
}

rx_result display_runtime::unregister_display (runtime::runtime_stop_context& ctx, const string_type& disp_path)
{
	return RX_NOT_SUPPORTED;
}

rx_result display_runtime::handle_request (rx_platform::http::http_request& req, rx_platform::http::http_response& resp)
{
	resp.set_string_content("Display handler not implemented!");
	resp.headers["Content-Type"] = "text/plain";
	resp.result = 200;
	return true;
}


} // namespace displays
} // namespace rx_platform

