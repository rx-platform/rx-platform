

/****************************************************************************
*
*  protocols\opcua\rx_opcua_server.h
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


#ifndef rx_opcua_server_h
#define rx_opcua_server_h 1



// rx_ptr
#include "lib/rx_ptr.h"
// rx_opcua_subscriptions
#include "protocols/opcua/rx_opcua_subscriptions.h"

#include "rx_opcua_params.h"
using namespace protocols::opcua::common;


namespace protocols {

namespace opcua {
namespace opcua_addr_space
{
class opcua_address_space_base;
}

namespace opcua_server {





class opcua_server_endpoint_base : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(opcua_server_endpoint_base);

  public:
      opcua_server_endpoint_base (const string_type& server_type, const application_description& app_descr, opcua_addr_space::opcua_address_space_base* addr_space, opcua_subscriptions::opcua_subscriptions_collection* subs);


      common::endpoint_description get_endpoint_description (const string_type& ep_url, bool discovery);

      common::application_description get_application_description (const string_type& ep_url);

      opcua_addr_space::opcua_address_space_base* get_address_space ();

      static application_description fill_application_description (const string_type& app_uri, const string_type& app_name, const string_type& app_bind, const string_type& server_type);

      opcua_subscriptions::opcua_subscriptions_collection* get_subscriptions ();

      virtual rx_result send_response (requests::opcua_response_ptr resp) = 0;


  protected:

  private:


      opcua_subscriptions::opcua_subscriptions_collection *subscriptions_;


      string_type server_type_;

      opcua_addr_space::opcua_address_space_base* address_space_;

      application_description application_description_;


};


} // namespace opcua_server
} // namespace opcua
} // namespace protocols



#endif
