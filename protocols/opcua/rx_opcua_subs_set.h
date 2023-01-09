

/****************************************************************************
*
*  protocols\opcua\rx_opcua_subs_set.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_opcua_subs_set_h
#define rx_opcua_subs_set_h 1



// rx_opcua_requests
#include "protocols/opcua/rx_opcua_requests.h"



namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_subscription {





class opcua_create_subs_request : public opcua_request_base  
{

  public:
      opcua_create_subs_request (uint32_t req_id, uint32_t req_handle);


      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);

      rx_result serialize_binary (binary::ua_binary_ostream& stream);


      double publish_interval;

      uint32_t lifetime_count;

      uint32_t keep_alive_count;

      uint32_t max_notifications;

      bool enabled;

      uint8_t priority;

      opcua_create_subs_request() = default;
  protected:

  private:


};






class opcua_create_subs_response : public opcua_response_base  
{

  public:
      opcua_create_subs_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      rx_result process_response (opcua_client_endpoint_ptr ep);


      uint32_t subscription_id;

      double publish_interval;

      uint32_t lifetime_count;

      uint32_t keep_alive_count;

      opcua_create_subs_response() = default;
  protected:

  private:


};






class opcua_delete_subs_request : public opcua_request_base  
{

  public:

      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);


      std::vector<uint32_t> subscription_ids;


  protected:

  private:


};






class opcua_delete_subs_response : public opcua_response_base  
{

  public:
      opcua_delete_subs_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;


      std::vector<opcua_result_t> results;

      std::vector<diagnostic_info> diagnostics_info;

      opcua_delete_subs_response() = default;
  protected:

  private:


};


struct subscription_ack
{
    uint32_t subscription_id;
    uint32_t sequence_number;

    void serialize(binary::ua_binary_ostream& stream) const;
    void deserialize(binary::ua_binary_istream& stream);
};




class opcua_publish_request : public opcua_request_base  
{

  public:
      opcua_publish_request (uint32_t req_id, uint32_t req_handle);


      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);

      rx_result serialize_binary (binary::ua_binary_ostream& stream);


      std::vector<subscription_ack> subscription_acks;

      opcua_server_endpoint_ptr endpoint;

      opcua_publish_request() = default;
  protected:

  private:


};


class opcua_notification_data : public common::ua_extension
{
public:
    opcua_notification_data(rx_node_id class_id, rx_node_id binary_id, rx_node_id xml_id);
};

typedef std::unique_ptr<opcua_notification_data> notification_data_ptr;

struct monitored_item_notification
{
    uint32_t client_handle;
    data_value value;

    void serialize(binary::ua_binary_ostream& stream) const;
    void deserialize(binary::ua_binary_istream& stream);
};

class opcua_data_change_notification : public opcua_notification_data
{
public:
    opcua_data_change_notification();
    std::vector< monitored_item_notification> monitored_items;
    std::vector<diagnostic_info> diagnostics_info;

private:
    void internal_serialize_extension(binary::ua_binary_ostream& stream) const;
    void internal_deserialize_extension(binary::ua_binary_istream& stream);
};




class opcua_publish_response : public opcua_response_base  
{

  public:
      opcua_publish_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      rx_result process_response (opcua_client_endpoint_ptr ep);


      uint32_t subscription_id;

      std::vector<uint32_t> sequence_numbers;

      bool more_notifications;

      uint32_t sequence_number;

      rx_time publish_time;

      notification_data_ptr notification;

      std::vector<opcua_result_t> results;

      std::vector<diagnostic_info> diagnostics_info;

      opcua_publish_response() = default;
  protected:

  private:


};






class opcua_republish_request : public opcua_request_base  
{

  public:

      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);


      uint32_t subscription_id;

      uint32_t sequence_number;


  protected:

  private:


};






class opcua_republish_response : public opcua_response_base  
{

  public:
      opcua_republish_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;


      uint32_t sequence_number;

      rx_time publish_time;

      notification_data_ptr notification;

      opcua_republish_response() = default;
  protected:

  private:


};


} // namespace opcua_subscription
} // namespace requests
} // namespace opcua
} // namespace protocols



#endif
