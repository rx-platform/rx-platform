

/****************************************************************************
*
*  protocols\opcua\rx_opcua_server.h
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


#ifndef rx_opcua_server_h
#define rx_opcua_server_h 1



// rx_method_set
#include "protocols/opcua/rx_method_set.h"
// rx_opcua_subscriptions
#include "protocols/opcua/rx_opcua_subscriptions.h"
// rx_opcua_attributes
#include "protocols/opcua/rx_opcua_attributes.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include "rx_opcua_params.h"
using namespace protocols::opcua::common;


namespace protocols {

namespace opcua {
namespace opcua_addr_space
{
class opcua_address_space_base;
}
typedef std::unique_ptr<requests::opcua_attributes::opcua_write_request> opcua_write_request_ptr;
typedef std::unique_ptr<requests::opcua_method::opcua_call_request> opcua_execute_request_ptr;





class opcua_server_endpoint_base : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(opcua_server_endpoint_base);
    struct write_request_data
    {
        std::vector<std::pair<runtime_transaction_id_t, opcua_result_t> > results;
        opcua_write_request_ptr request_ptr;
        size_t pending_count;
    };
    struct execute_request_data
    {
        std::vector<std::pair<runtime_transaction_id_t, execute_result> > results;
        opcua_execute_request_ptr request_ptr;
        size_t pending_count;
    };
    typedef std::vector<write_request_data> write_requests_type;
    typedef std::map<runtime_transaction_id_t, size_t> write_cache_type;
    typedef std::vector<execute_request_data> execute_requests_type;
    typedef std::map<runtime_transaction_id_t, size_t> execute_cache_type;

  public:
      opcua_server_endpoint_base (const string_type& server_type, const application_description& app_description, const string_type& ep_path, opcua_addr_space::opcua_address_space_base* addr_space, opcua_subscriptions::opcua_subscriptions_collection* subs, const string_type& port_path);


      common::endpoint_description get_endpoint_description (const string_type& ep_url, bool discovery);

      opcua_addr_space::opcua_address_space_base* get_address_space ();

      static application_description fill_application_description (const string_type& app_uri, const string_type& app_name, const string_type& app_bind, const string_type& server_type);

      opcua_subscriptions::opcua_subscriptions_collection* get_subscriptions ();

      virtual rx_result send_response (requests::opcua_response_ptr resp) = 0;

      void queue_write_request (opcua_write_request_ptr req);

      void write_response (opcua_result_t status, runtime_transaction_id_t trans_id);

      void queue_execute_request (opcua_execute_request_ptr req);

      void execute_response (opcua_result_t status, rx_simple_value out_data, runtime_transaction_id_t trans_id);


      const string_type& get_ep_path () const
      {
        return ep_path_;
      }


      const string_type& get_port_path () const
      {
        return port_path_;
      }



  protected:

      rx_protocol_result_t connected_function (rx_session* session);


  private:


      opcua_subscriptions::opcua_subscriptions_collection *subscriptions_;

      write_requests_type write_requests_;

      execute_requests_type execute_requests_;


      string_type server_type_;

      opcua_addr_space::opcua_address_space_base* address_space_;

      string_type ep_path_;

      locks::slim_lock transactions_lock_;

      write_cache_type write_cache_;

      execute_cache_type execute_cache_;

      string_type port_path_;

      application_description app_description_;


};






class opcua_server_endpoint_data 
{

  public:
      opcua_server_endpoint_data();

      ~opcua_server_endpoint_data();


      application_description description;


  protected:

  private:


};


} // namespace opcua
} // namespace protocols



#endif
