

/****************************************************************************
*
*  system\runtime\rx_event_blocks.cpp
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


// rx_event_blocks
#include "system/runtime/rx_event_blocks.h"



namespace rx_platform {

namespace runtime {

namespace event_blocks {

// Class rx_platform::runtime::event_blocks::runtime_events 

runtime_events::runtime_events()
{
}


runtime_events::~runtime_events()
{
}



void runtime_events::register_event (structure::event_data* who, const string_type& id)
{
}

void runtime_events::event_fired (const structure::event_data* whose, rx_simple_value value, data::runtime_values_data data)
{
}


} // namespace event_blocks
} // namespace runtime
} // namespace rx_platform

