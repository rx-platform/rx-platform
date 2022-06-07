

/****************************************************************************
*
*  protocols\opcua\rx_opcua_discovery.h
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


#ifndef rx_opcua_discovery_h
#define rx_opcua_discovery_h 1



// rx_opcua_requests
#include "protocols/opcua/rx_opcua_requests.h"



namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_discovery {





class opcua_get_endpoints : public opcua_request_base  
{

  public:

      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);


      string_type endpoint;

      string_array locale_ids;

      string_array profile_uris;


  protected:

  private:


};






class opcua_get_endpoints_response : public opcua_response_base  
{

  public:
      opcua_get_endpoints_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;


      opcua_endpoints_type endpoints;

      opcua_get_endpoints_response() = default;
  protected:

  private:


};






class opcua_find_servers : public opcua_request_base  
{

  public:

      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);


      string_type endpoint;

      string_array locale_ids;

      string_array server_uris;


  protected:

  private:


};






class opcua_find_servers_response : public opcua_response_base  
{

  public:
      opcua_find_servers_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;


      application_descriptions_type servers;

      opcua_find_servers_response() = default;
  protected:

  private:


};


} // namespace opcua_discovery
} // namespace requests
} // namespace opcua
} // namespace protocols



#endif
