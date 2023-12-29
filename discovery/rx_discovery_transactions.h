

/****************************************************************************
*
*  discovery\rx_discovery_transactions.h
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


#ifndef rx_discovery_transactions_h
#define rx_discovery_transactions_h 1



// rx_query_messages
#include "sys_internal/rx_query_messages.h"
// rx_discovery_main
#include "discovery/rx_discovery_main.h"

using namespace rx_internal::rx_protocol::messages::query_messages;


namespace rx_internal {

namespace discovery {





class browse_peer_transaction : public rx_protocol::messages::query_messages::browse_request_message  
{
    typedef std::unique_ptr<rx_protocol::messages::query_messages::browse_response_message> response_ptr_t;
    typedef std::function<void(browse_result_items_type&, peer_connection_ptr)> response_callback_t;

    friend class peer_connection;

  public:
      browse_peer_transaction (response_callback_t callback, peer_connection_ptr conn);


      rx_result process (response_ptr_t response_ptr);

      rx_result process (rx_protocol::error_message_ptr error_ptr);


  protected:

  private:


      rx_reference<peer_connection> my_connection_;


      response_callback_t callback_;


};






class query_peer_transaction : public rx_protocol::messages::query_messages::query_request_message  
{
    typedef std::unique_ptr<rx_protocol::messages::query_messages::query_response_message> response_ptr_t;
    typedef std::function<void(query_result_items_type&, peer_connection_ptr)> response_callback_t;


  public:
      query_peer_transaction (response_callback_t callback, peer_connection_ptr conn);


      rx_result process (response_ptr_t response_ptr);

      rx_result process (rx_protocol::error_message_ptr error_ptr);


  protected:

  private:


      rx_reference<peer_connection> my_connection_;


      response_callback_t callback_;


};


} // namespace discovery
} // namespace rx_internal



#endif
