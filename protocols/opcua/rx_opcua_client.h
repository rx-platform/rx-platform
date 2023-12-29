

/****************************************************************************
*
*  protocols\opcua\rx_opcua_client.h
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


#ifndef rx_opcua_client_h
#define rx_opcua_client_h 1



// rx_opcua_requests
#include "protocols/opcua/rx_opcua_requests.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include "protocols/opcua/rx_opcua_binary.h"
#include "protocols/opcua/rx_opcua_identifiers.h"
#include "protocols/opcua/rx_opcua_requests.h"
#include "protocols/opcua/rx_opcua_session.h"
#include "protocols/opcua/rx_opcua_subs_set.h"
#include "rx_opcua_params.h"
using namespace protocols::opcua::common;

#include "lib/rx_lock.h"


namespace protocols {

namespace opcua {





struct opcua_client_session_data 
{


      rx_node_id session_id;

      rx_node_id authentication_token;

      double session_timeout;

      byte_string server_nounce;

  public:

  protected:

  private:


};


struct pending_client_subscription_data
{
    uint32_t interval;
};

struct active_client_subscription_data
{
    uint32_t interval;
    uint32_t sequence_number = 0;
};




class subscriptions_data 
{
  public:
    
    typedef std::map<uint32_t, pending_client_subscription_data> pending_subscriptions_type;
    typedef std::map<uint32_t, active_client_subscription_data> active_subscriptions_type;

  public:

      pending_subscriptions_type pending;

      active_subscriptions_type active;


  protected:

  private:


};


enum class opcua_client_state
{
    not_connected = 0,
    sent_create_session = 1,
    sent_activate_session = 2,
    active = 3
};





class opcua_client_endpoint_base : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(opcua_client_endpoint_base);

  public:
      opcua_client_endpoint_base (const string_type& client_type, const application_description& app_descr);


      static application_description fill_application_description (const string_type& app_uri, const string_type& app_name, const string_type& app_bind, const string_type& server_type);

      virtual rx_result send_request (requests::opcua_request_ptr req) = 0;

      virtual rx_result session_activated () = 0;

      virtual rx_result subscription_created (uint32_t subscription_id) = 0;

      virtual void subscription_notification (requests::opcua_subscription::opcua_data_change_notification* data) = 0;

      rx_result create_subscription_items (uint32_t subs_id, std::vector<create_monitored_item_data> to_create);

      virtual rx_result create_items_response (const std::vector<create_monitored_item_result>& results) = 0;


      opcua_client_session_data session;

      subscriptions_data subscriptions;


      uint32_t current_request_id;

      opcua_client_state state;

      locks::slim_lock transactions_lock;


  protected:

      rx_protocol_result_t client_connected (rx_protocol_stack_endpoint* reference, rx_session* session);

      rx_protocol_result_t received (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      rx_result create_subscription (uint32_t interval);


  private:


      string_type client_type_;

      application_description application_description_;


};


} // namespace opcua
} // namespace protocols



#endif
