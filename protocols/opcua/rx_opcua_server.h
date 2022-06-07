

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

#include "rx_opcua_params.h"


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
      opcua_server_endpoint_base (const string_type& endpoint_url, const string_type& app_name, const string_type& app_uri, opcua_addr_space::opcua_address_space_base* addr_space);


      common::endpoint_description get_endpoint_description (const string_type& ep_url);

      common::application_description get_application_description (const string_type& ep_url);

      opcua_addr_space::opcua_address_space_base* get_address_space ();


  protected:

  private:


      string_type app_name_;

      string_type app_uri_;

      string_type endpoint_url_;

      opcua_addr_space::opcua_address_space_base* address_space_;


};


} // namespace opcua_server
} // namespace opcua
} // namespace protocols



#endif
