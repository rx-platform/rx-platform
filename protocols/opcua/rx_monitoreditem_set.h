

/****************************************************************************
*
*  protocols\opcua\rx_monitoreditem_set.h
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


#ifndef rx_monitoreditem_set_h
#define rx_monitoreditem_set_h 1



// rx_opcua_requests
#include "protocols/opcua/rx_opcua_requests.h"



namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_monitoreditem {





class opcua_create_mon_items_request : public opcua_request_base  
{

  public:
      opcua_create_mon_items_request (uint32_t req_id, uint32_t req_handle);


      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);

      rx_result serialize_binary (binary::ua_binary_ostream& stream);


      uint32_t subscription_id;

      timestamps_return_type timestamps_to_return;

      std::vector<create_monitored_item_data> to_create;

      opcua_create_mon_items_request() = default;
  protected:

  private:


};






class opcua_create_mon_items_response : public opcua_response_base  
{

  public:
      opcua_create_mon_items_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      rx_result process_response (opcua_client_endpoint_ptr ep);


      std::vector<create_monitored_item_result> results;

      std::vector<diagnostic_info> diagnostics_info;

      opcua_create_mon_items_response() = default;
  protected:

  private:


};






class opcua_delete_mon_items_request : public opcua_request_base  
{

  public:
      opcua_delete_mon_items_request (uint32_t req_id, uint32_t req_handle);


      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);

      rx_result serialize_binary (binary::ua_binary_ostream& stream);


      uint32_t subscription_id;

      std::vector<uint32_t> to_delete;

      opcua_delete_mon_items_request() = default;
  protected:

  private:


};






class opcua_delete_mon_items_response : public opcua_response_base  
{

  public:
      opcua_delete_mon_items_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      rx_result process_response (opcua_client_endpoint_ptr ep);


      std::vector<opcua_result_t> results;

      std::vector<diagnostic_info> diagnostics_info;

      opcua_delete_mon_items_response() = default;
  protected:

  private:


};


} // namespace opcua_monitoreditem
} // namespace requests
} // namespace opcua
} // namespace protocols



#endif
