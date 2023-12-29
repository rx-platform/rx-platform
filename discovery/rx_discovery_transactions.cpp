

/****************************************************************************
*
*  discovery\rx_discovery_transactions.cpp
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


// rx_discovery_transactions
#include "discovery/rx_discovery_transactions.h"



namespace rx_internal {

namespace discovery {

// Class rx_internal::discovery::browse_peer_transaction 

browse_peer_transaction::browse_peer_transaction (response_callback_t callback, peer_connection_ptr conn)
      : my_connection_(conn),
        callback_(callback)
{
}



rx_result browse_peer_transaction::process (response_ptr_t response_ptr)
{
	callback_(response_ptr->items, my_connection_);
	return true;
}

rx_result browse_peer_transaction::process (rx_protocol::error_message_ptr error_ptr)
{
	return true;
}


// Class rx_internal::discovery::query_peer_transaction 

query_peer_transaction::query_peer_transaction (response_callback_t callback, peer_connection_ptr conn)
      : my_connection_(conn),
        callback_(callback)
{
}



rx_result query_peer_transaction::process (response_ptr_t response_ptr)
{
	callback_(response_ptr->items, my_connection_);
	return true;
}

rx_result query_peer_transaction::process (rx_protocol::error_message_ptr error_ptr)
{
	return true;
}


} // namespace discovery
} // namespace rx_internal

