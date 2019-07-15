

/****************************************************************************
*
*  runtime_internal\rx_internal_data_source.cpp
*
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


// rx_internal_data_source
#include "runtime_internal/rx_internal_data_source.h"

#include "system/server/rx_async_functions.h"


namespace sys_runtime {

namespace data_source {

// Class sys_runtime::data_source::internal_data_source 

internal_data_source::internal_data_source (const string_type& path)
{
}



void internal_data_source::add_item (const string_type& path, uint32_t rate, value_handle_extended& handle)
{
	auto transaction_state = new rx_transaction_state<rx_result, int>();

}

void internal_data_source::remove_item (const value_handle_extended& handle)
{
}

bool internal_data_source::is_empty () const
{
	return false;
}


// Class sys_runtime::data_source::internal_data_subscription 


} // namespace data_source
} // namespace sys_runtime

