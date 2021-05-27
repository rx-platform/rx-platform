

/****************************************************************************
*
*  system\runtime\rx_transaction_limiter.cpp
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


// rx_transaction_limiter
#include "system/runtime/rx_transaction_limiter.h"



namespace rx_platform {

namespace runtime {

namespace io_types {

namespace ports_lib {

// Class rx_platform::runtime::io_types::ports_lib::limiter_endpoint 

limiter_endpoint::limiter_endpoint (runtime::items::port_runtime* port)
      : port_(port)
{
}



rx_protocol_stack_endpoint* limiter_endpoint::bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func)
{

    return &stack_endpoint_;
}

rx_protocol_result_t limiter_endpoint::received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}

rx_protocol_result_t limiter_endpoint::connected_function (rx_protocol_stack_endpoint* reference, rx_session* session)
{
    return RX_PROTOCOL_NOT_IMPLEMENTED;
}


// Class rx_platform::runtime::io_types::ports_lib::transaction_limiter_port 


} // namespace ports_lib
} // namespace io_types
} // namespace runtime
} // namespace rx_platform

