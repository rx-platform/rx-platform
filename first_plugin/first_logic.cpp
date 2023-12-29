

/****************************************************************************
*
*  first_plugin\first_logic.cpp
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

#include "first_plugin_version.h"
#include "version/rx_code_macros.h"

// first_logic
#include "first_plugin/first_logic.h"



// Class first_method 

first_method::first_method()
{
}


first_method::~first_method()
{
}



rx_result first_method::initialize_method (rx_init_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_method", 100, _rx_func_);
	return true;
}

rx_result first_method::start_method (rx_start_context& ctx)
{
	RX_PLUGIN_LOG_DEBUG("first_method", 100, _rx_func_);
	return true;
}

rx_result first_method::stop_method ()
{
	RX_PLUGIN_LOG_DEBUG("first_method", 100, _rx_func_);
	return true;
}

rx_result first_method::deinitialize_method ()
{
	RX_PLUGIN_LOG_DEBUG("first_method", 100, _rx_func_);
	return true;
}

rx_result first_method::method_execute (runtime_transaction_id_t id, bool test, rx_security_handle_t identity, rx_simple_value val, rx_process_context& ctx)
{
	if (val.is_struct() && val.struct_size() == 2)
	{
		double one = 0;
		double two = 0;
		one = val[0].extract_static(one);
		two = val[1].extract_static(two);

		double three = one - two;

		rx_simple_value ret = rx_create_value_static(three);

		execute_result_received(true, id, std::move(ret));

		return true;
	}
	return RX_INVALID_ARGUMENT;
}


