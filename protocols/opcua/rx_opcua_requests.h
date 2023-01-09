

/****************************************************************************
*
*  protocols\opcua\rx_opcua_requests.h
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


#ifndef rx_opcua_requests_h
#define rx_opcua_requests_h 1




#include "rx_opcua_binary.h"


namespace protocols {

namespace opcua {
typedef std::vector<common::endpoint_description> opcua_endpoints_type;
typedef std::vector<common::application_description> application_descriptions_type;

namespace opcua_addr_space
{
class server_address_space;
}

namespace requests {
class opcua_request_base;
typedef std::unique_ptr<opcua_request_base> opcua_request_ptr;
class opcua_response_base;
typedef std::unique_ptr<opcua_response_base> opcua_response_ptr;





class opcua_request_base 
{

  public:
      opcua_request_base();

      opcua_request_base (uint32_t req_id, uint32_t req_handle);


      virtual rx_node_id get_binary_request_id () = 0;

      virtual opcua_request_ptr create_empty () const = 0;

      virtual rx_result deserialize_binary (binary::ua_binary_istream& stream);

      rx_result deserialize_header_binary (binary::ua_binary_istream& stream);

      virtual rx_result serialize_binary (binary::ua_binary_ostream& stream);

      rx_result serialize_header_binary (binary::ua_binary_ostream& stream);

      virtual opcua_response_ptr do_job (opcua_server_endpoint_ptr ep) = 0;


      uint32_t request_id;

      rx_time_struct timestamp;

      uint32_t request_handle;

      uint32_t diagnostics;

      string_type audit_entry_id;

      uint32_t timeout;

      opcua_extension_ptr additional;

      rx_node_id authentication_token;

      virtual ~opcua_request_base() {}
      opcua_request_base(const opcua_request_base&) = delete;
      opcua_request_base(opcua_request_base&&) = delete;
      opcua_request_base& operator=(const opcua_request_base&) = delete;
      opcua_request_base& operator=(opcua_request_base&&) = delete;
  protected:

      void move_header_to (opcua_request_base* where);


  private:


};



class opcua_response_base;

typedef std::unique_ptr<opcua_response_base> opcua_response_ptr;




class opcua_response_base 
{

  public:
      opcua_response_base (const opcua_request_base& req);


      virtual rx_node_id get_binary_response_id () = 0;

      virtual opcua_response_ptr create_empty () const = 0;

      virtual rx_result deserialize_binary (binary::ua_binary_istream& stream);

      rx_result deserialize_header_binary (binary::ua_binary_istream& stream);

      virtual rx_result serialize_binary (binary::ua_binary_ostream& stream) const;

      rx_result serialize_header_binary (binary::ua_binary_ostream& stream);

      virtual rx_result process_response (opcua_client_endpoint_ptr ep);


      uint32_t request_id;

      rx_time_struct timestamp;

      uint32_t request_handle;

      uint32_t result;

      diagnostic_info diagnostics;

      opcua_extension_ptr additional;

      opcua_response_base() = default;
      virtual ~opcua_response_base() {}
      opcua_response_base(const opcua_response_base&) = delete;
      opcua_response_base(opcua_response_base&&) = delete;
      opcua_response_base& operator=(const opcua_response_base&) = delete;
      opcua_response_base& operator=(opcua_response_base&&) = delete;
  protected:

  private:


};






class opcua_requests_repository 
{
    typedef std::map<rx_node_id, opcua_request_ptr> registered_requests_type;
    typedef std::map<rx_node_id, opcua_response_ptr> registered_responses_type;

  public:

      static opcua_requests_repository& instance ();

      void init_requests ();

      rx_result register_request (opcua_request_ptr req);

      opcua_request_ptr get_request (const rx_node_id& id) const;

      void init_responses ();

      rx_result register_response (opcua_response_ptr resp);

      opcua_response_ptr get_response (const rx_node_id& id) const;


  protected:

  private:


      registered_requests_type registered_requests_;

      registered_responses_type registered_responses_;


};






class opcua_service_fault : public opcua_response_base  
{

  public:
      opcua_service_fault (const opcua_request_base& req, uint32_t result_code);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_service_fault() = default;
  protected:

  private:


};






class opcua_unsupported_request : public opcua_request_base  
{

  public:
      opcua_unsupported_request (const rx_node_id& id);


      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);


  protected:

  private:


      rx_node_id request_type_id_;


};


} // namespace requests
} // namespace opcua
} // namespace protocols



#endif
