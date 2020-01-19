

/****************************************************************************
*
*  system\runtime\rx_io_relations.cpp
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


// rx_io_relations
#include "system/runtime/rx_io_relations.h"



namespace rx_platform {

namespace runtime {

namespace relations {

// Class rx_platform::runtime::relations::port_up_relation 


void port_up_relation::process_stack ()
{
}

rx_result port_up_relation::initialize_runtime (runtime::runtime_init_context& ctx)
{
    return true;
}

rx_result port_up_relation::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    return true;
}

rx_result port_up_relation::start_runtime (runtime::runtime_start_context& ctx)
{
    return true;
}

rx_result port_up_relation::stop_runtime (runtime::runtime_stop_context& ctx)
{
    return true;
}


} // namespace relations
} // namespace runtime
} // namespace rx_platform

