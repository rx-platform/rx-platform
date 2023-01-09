

/****************************************************************************
*
*  protocols\opcua\rx_opcua_basic_client.h
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


#ifndef rx_opcua_basic_client_h
#define rx_opcua_basic_client_h 1



// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// rx_blocks_templates
#include "system/runtime/rx_blocks_templates.h"
// dummy
#include "dummy.h"
// rx_opcua_client
#include "protocols/opcua/rx_opcua_client.h"

namespace protocols {
namespace opcua {
namespace opcua_basic_client {
class opcua_basic_client_port;
class opcua_basic_source;

} // namespace opcua_basic_client
} // namespace opcua
} // namespace protocols


#include "protocols/opcua/rx_opcua_binary.h"
#include "protocols/opcua/rx_opcua_identifiers.h"
#include "protocols/opcua/rx_opcua_requests.h"
#include "protocols/opcua/rx_opcua_session.h"


namespace protocols {

namespace opcua {

namespace opcua_basic_client {
typedef rx_reference<opcua_basic_source> basic_source_ptr;






class opcua_basic_client_endpoint : public opcua_client_endpoint_base  
{
    DECLARE_REFERENCE_PTR(opcua_basic_client_endpoint);

  public:
      opcua_basic_client_endpoint (opcua_basic_client_port* port, const string_type& client_type, const application_description& app_descr, uint32_t interval);

      ~opcua_basic_client_endpoint();


      void close_endpoint ();

      rx_protocol_stack_endpoint* get_endpoint ();

      rx_result send_request (requests::opcua_request_ptr req);

      rx_result session_activated ();

      rx_result subscription_created (uint32_t subscription_id);

      void subscription_notification (requests::opcua_subscription::opcua_data_change_notification* data);

      rx_result add_subscription_items (std::vector<create_monitored_item_data> to_create);

      rx_result create_items_response (const std::vector<create_monitored_item_result>& results);


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }


      opcua_basic_client_port* get_port ()
      {
        return port_;
      }



  protected:

  private:

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);

      static rx_protocol_result_t disconnected_function (rx_protocol_stack_endpoint* reference, rx_session* session, rx_protocol_result_t reason);

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);



      rx_protocol_stack_endpoint stack_entry_;


      rx_thread_handle_t executer_;

      opcua_basic_client_port* port_;

      uint32_t current_request_id_;

      opcua_client_state state_;

      locks::slim_lock transactions_lock_;

      uint32_t publish_interval_;

      uint32_t subscription_id_;


};







typedef rx_platform::runtime::io_types::ports_templates::master_client_port_impl< opcua_basic_client_endpoint  > opcua_basic_client_port_base;






class basic_monitored_item 
{
    typedef std::set<basic_source_ptr> registered_sources_type;

  public:
      basic_monitored_item();

      ~basic_monitored_item();


      rx_result register_source (basic_source_ptr who);

      rx_result unregister_source (basic_source_ptr who);

      void subscription_disconnected (rx_time now);

      void data_changed (const data_value& value);


  protected:

  private:


      registered_sources_type registered_sources_;


      data_value value_;

      uint32_t server_handle_;


};






class opcua_basic_client_port : public opcua_basic_client_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 0, 1, "\
OPC UA client protocol port class. Basic implementation OPC UA binary protocol basic client.");

    DECLARE_REFERENCE_PTR(opcua_basic_client_port);

    struct pending_item_data
    {
        std::pair<rx_node_id, attribute_id> key;
        uint32_t handle;
    };

    std::atomic<uint32_t> next_item_handle_ = 1;

    typedef std::vector <pending_item_data> pending_type;
    typedef std::vector <std::pair<rx_node_id, attribute_id>> queued_type;
    typedef std::map <uint32_t, std::pair<rx_node_id, attribute_id> > active_type;
    typedef std::map <std::pair<rx_node_id, attribute_id>, std::unique_ptr<basic_monitored_item> > registered_items_type;

  public:
      opcua_basic_client_port();


      void stack_assembled ();

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result register_source (basic_source_ptr who);

      rx_result unregister_source (basic_source_ptr who);

      void subscription_notification (requests::opcua_subscription::opcua_data_change_notification* data);

      void subscription_disconnected (requests::opcua_subscription::opcua_data_change_notification* data);

      rx_result create_items_response (const std::vector<create_monitored_item_result>& results);


      static std::map<rx_node_id, opcua_basic_client_port::smart_ptr> runtime_instances;


  protected:

  private:

      void fill_application_description (const string_type& app_uri, const string_type& app_name, const string_type& app_bind, const string_type& server_type);



      registered_items_type registered_items_;


      rx_timer_ptr timer_;

      application_description application_description_;

      uint32_t publish_interval_;

      locks::slim_lock sources_lock_;

      pending_type queued_items_;

      pending_type pending_items_;

      active_type active_items_;


};







typedef rx_platform::runtime::blocks::blocks_templates::extern_source_impl< opcua_basic_client_port  > opcua_basic_source_base;






class opcua_basic_source : public opcua_basic_source_base  
{
    DECLARE_CODE_INFO("rx", 0, 5, 0, "\
Implementation of OPC UA Basic Source");

    DECLARE_REFERENCE_PTR(opcua_basic_source);

  public:
      opcua_basic_source();

      ~opcua_basic_source();


      rx_result initialize_source (runtime::runtime_init_context& ctx);

      void port_connected (port_ptr_t port);

      void port_disconnected (port_ptr_t port);

      void opcua_value_changed (const data_value& val);


      const read_value_id& get_value_id () const
      {
        return value_id_;
      }



  protected:

  private:


      read_value_id value_id_;


};


} // namespace opcua_basic_client
} // namespace opcua
} // namespace protocols



#endif
