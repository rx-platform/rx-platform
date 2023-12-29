

/****************************************************************************
*
*  protocols\opcua\rx_opcua_session.h
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


#ifndef rx_opcua_session_h
#define rx_opcua_session_h 1



// rx_opcua_requests
#include "protocols/opcua/rx_opcua_requests.h"



namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_session {





class opcua_create_session : public opcua_request_base  
{
    typedef std::vector<common::endpoint_description> endpoints_type;

  public:
      opcua_create_session (uint32_t req_id, uint32_t req_handle);


      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);

      rx_result serialize_binary (binary::ua_binary_ostream& stream);


      application_description client_description;

      string_type server_uri;

      string_type endpoint_url;

      string_type session_name;

      byte_string client_nounce;

      byte_string client_certificate;

      double session_timeout;

      uint32_t max_message_size;

      opcua_create_session() = default;
  protected:

  private:


};






class opcua_create_session_response : public opcua_response_base  
{

  public:
      opcua_create_session_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      rx_result process_response (opcua_client_endpoint_ptr ep);


      rx_node_id session_id;

      rx_node_id authentication_token;

      double session_timeout;

      byte_string server_nounce;

      byte_string server_certificate;

      opcua_endpoints_type endpoints;

      string_type signature_algorithm;

      byte_string signature_data;

      uint32_t max_message_size;

      opcua_create_session_response() = default;
  protected:

  private:


};


class opcua_identity_token : public common::ua_extension
{
public:
    opcua_identity_token(rx_node_id class_id, rx_node_id binary_id, rx_node_id xml_id);
};
typedef std::unique_ptr<opcua_identity_token> identity_token_ptr;
class opcua_anonymus_identity_token : public opcua_identity_token
{
public:
    opcua_anonymus_identity_token();
    opcua_extension_ptr make_copy();
private:
    void internal_serialize_extension(binary::ua_binary_ostream& stream) const;
    void internal_deserialize_extension(binary::ua_binary_istream& stream);
};




class opcua_activate_session : public opcua_request_base  
{

  public:
      opcua_activate_session (uint32_t req_id, uint32_t req_handle);


      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);

      rx_result serialize_binary (binary::ua_binary_ostream& stream);


      string_type signature_algorithm;

      byte_string signature_data;

      string_array locale_ids;

      identity_token_ptr identity_token;

      string_type token_signature_algorithm;

      byte_string token_signature_data;

      opcua_activate_session() = default;
  protected:

  private:


};






class opcua_close_session : public opcua_request_base  
{

  public:

      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);


      bool delete_subscriptions;


  protected:

  private:


};






class opcua_activate_session_response : public opcua_response_base  
{

  public:
      opcua_activate_session_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      rx_result process_response (opcua_client_endpoint_ptr ep);


      byte_string server_nounce;

      opcua_activate_session_response() = default;
  protected:

  private:


};






class opcua_close_session_response : public opcua_response_base  
{

  public:
      opcua_close_session_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      rx_result process_response (opcua_client_endpoint_ptr ep);

      opcua_close_session_response() = default;
  protected:

  private:


};


} // namespace opcua_session
} // namespace requests
} // namespace opcua
} // namespace protocols



#endif
